//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "bookmark.h"

#include "conf_file_ex.h"

#include "shell_icon.h"
#include "option.h"
#include "resource.h"
#include "conf_dir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
static const xpr_tchar_t kBookmarkSection[] = XPR_STRING_LITERAL("bookmark");
static const xpr_tchar_t kNameKey[]         = XPR_STRING_LITERAL("bookmark.item%d.name");
static const xpr_tchar_t kPathKey[]         = XPR_STRING_LITERAL("bookmark.item%d.path");
static const xpr_tchar_t kIconKey[]         = XPR_STRING_LITERAL("bookmark.item%d.icon");
static const xpr_tchar_t kIconIndexKey[]    = XPR_STRING_LITERAL("bookmark.item%d.icon_idex");
static const xpr_tchar_t kParameterKey[]    = XPR_STRING_LITERAL("bookmark.item%d.paramter");
static const xpr_tchar_t kStartupKey[]      = XPR_STRING_LITERAL("bookmark.item%d.startup");
static const xpr_tchar_t kShowStatusKey[]   = XPR_STRING_LITERAL("bookmark.item%d.show_status");
static const xpr_tchar_t kHotKeyKey[]       = XPR_STRING_LITERAL("bookmark.item%d.hotkey");

// user defined message
enum
{
    WM_SHELL_ASYNC_ICON = WM_USER + 100,
};

BookmarkItem::BookmarkItem(void)
    : mSignature(BookmarkMgr::instance().generateSignature())
    , mIconIndex(0)
    , mShowCmd(0)
    , mHotKey(0)
    , mIconCache(XPR_NULL)
    , mPending(XPR_FALSE)
{
}

BookmarkItem::~BookmarkItem(void)
{
    clear();
}

BookmarkItem& BookmarkItem::operator=(const BookmarkItem &aSrc)
{
    mSignature = aSrc.mSignature;
    mName      = aSrc.mName;
    mPath      = aSrc.mPath;
    mIconPath  = aSrc.mIconPath;
    mIconIndex = aSrc.mIconIndex;
    mParam     = aSrc.mParam;
    mStartup   = aSrc.mStartup;
    mShowCmd   = aSrc.mShowCmd;
    mHotKey    = aSrc.mHotKey;

    DESTROY_ICON(mIconCache);

    mIconCache = ::DuplicateIcon(XPR_NULL, aSrc.mIconCache);
    mPending   = aSrc.mPending;

    return *this;
}

void BookmarkItem::clear(xpr_bool_t aNewSignature)
{
    mSignature = (aNewSignature == XPR_TRUE) ? BookmarkMgr::instance().generateSignature() : 0;
    mName.clear();
    mPath.clear();
    mIconPath.clear();
    mIconIndex = 0;
    mParam.clear();
    mStartup.clear();
    mShowCmd = 0;
    mHotKey = 0;

    DESTROY_ICON(mIconCache);
    mPending = XPR_FALSE;
}

xpr_uint_t BookmarkItem::newSignature(void)
{
    mSignature = BookmarkMgr::instance().generateSignature();
    return mSignature;
}

xpr_uint_t BookmarkItem::getSignature(void)
{
    return mSignature;
}

void BookmarkItem::setPending(xpr_bool_t aPending)
{
    mPending = aPending;
}

LPITEMIDLIST BookmarkItem::getPidl(void)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;
    BookmarkItem::getPidl(&sFullPidl);
    return sFullPidl;
}

xpr_bool_t BookmarkItem::getPidl(LPITEMIDLIST *aFullPidls)
{
    return Path2Pidl(mPath, aFullPidls);
}

xpr_bool_t BookmarkItem::isFastIcon(void)
{
    if (IsNetItem(mIconPath.c_str()) == XPR_TRUE)
        return XPR_FALSE;

    if (IsNetItem(mPath.c_str()) == XPR_TRUE)
        return XPR_FALSE;

    return XPR_TRUE;
}

HICON BookmarkItem::getIcon(void)
{
    return mIconCache;
}

void BookmarkItem::setIcon(HICON aSetIcon)
{
    DESTROY_ICON(mIconCache);

    mIconCache = aSetIcon;
    mPending = XPR_FALSE;
}

void BookmarkItem::destroyIcon(void)
{
    DESTROY_ICON(mIconCache);
    mPending = XPR_FALSE;
}

xpr_bool_t BookmarkItem::isSeparator(void)
{
    return (_tcscmp(mPath.c_str(), BOOKMARK_SEPARATOR) == 0) ? XPR_TRUE : XPR_FALSE;
}

void BookmarkItem::getTooltip(xpr_tchar_t *aTooltip, xpr_bool_t aWithName, xpr_bool_t aWithPending)
{
    if (aTooltip == XPR_NULL)
        return;

    aTooltip[0] = XPR_STRING_LITERAL('\0');

    if (aWithPending == XPR_TRUE && mPending == XPR_TRUE)
        _stprintf(aTooltip+_tcslen(aTooltip), XPR_STRING_LITERAL("%s\n\n"), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.icon_pending")));

    if (XPR_IS_TRUE(aWithName))
        _stprintf(aTooltip+_tcslen(aTooltip), XPR_STRING_LITERAL("%s: \"%s\"\n"), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.name")), mName.c_str());

    if (mHotKey > 0)
        _stprintf(aTooltip+_tcslen(aTooltip), XPR_STRING_LITERAL("%s: \"%s\"\n%s Ctrl+%c"), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.path")), mPath.c_str(), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey")), mHotKey);
    else
        _stprintf(aTooltip+_tcslen(aTooltip), XPR_STRING_LITERAL("%s: \"%s\"\n%s %s"), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.path")), mPath.c_str(), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey")), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey.none")));
}

BookmarkMgr::BookmarkMgr(void)
    : mSignature(0)
    , mShellIcon(XPR_NULL)
    , mFastNetIcon(XPR_TRUE)
{
    CreateEx(0, AfxRegisterWndClass(CS_GLOBALCLASS), XPR_STRING_LITERAL(""), 0,0,0,0,0,0,0);
}

BookmarkMgr::~BookmarkMgr(void)
{
    clear();

    mAsyncNotifyMap.clear();

    if (mShellIcon != XPR_NULL)
    {
        mShellIcon->Stop(1000);
        XPR_SAFE_DELETE(mShellIcon);
    }

    HICON sIcon;
    ICON_TYPE_MAP::iterator sIterator = mIconTypeMap.begin();
    for (; sIterator != mIconTypeMap.end(); ++sIterator)
    {
        sIcon = sIterator->second;
        DESTROY_ICON(sIcon);
    }

    mIconTypeMap.clear();

    DestroyWindow();
}

BEGIN_MESSAGE_MAP(BookmarkMgr, CWnd)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON, OnShellAsyncIcon)
END_MESSAGE_MAP()

void BookmarkMgr::setFastNetIcon(xpr_bool_t aFastNetIcon)
{
    mFastNetIcon = aFastNetIcon;
}

xpr_sint_t BookmarkMgr::getCount(void)
{
    return (xpr_sint_t)mBookmarkDeque.size();
}

BookmarkItem *BookmarkMgr::getBookmark(xpr_sint_t nBookmark)
{
    if (!FXFILE_STL_IS_INDEXABLE(nBookmark, mBookmarkDeque))
        return XPR_NULL;

    return mBookmarkDeque[nBookmark];
}

xpr_uint_t BookmarkMgr::generateSignature(void)
{
    return ++mSignature;
}

xpr_sint_t BookmarkMgr::findSignature(xpr_uint_t aSignature)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    BookmarkItem *sBookmark;

    sCount = getCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmark = getBookmark(i);
        if (sBookmark == XPR_NULL)
            continue;

        if (sBookmark->mSignature == aSignature)
            return i;
    }

    return -1;
}

xpr_sint_t BookmarkMgr::addBookmark(const xpr_tchar_t *aName,
                                    const xpr_tchar_t *aPath,
                                    const xpr_tchar_t *aIconPath,
                                    xpr_sint_t         aIconIndex,
                                    const xpr_tchar_t *aParam,
                                    const xpr_tchar_t *aStartup,
                                    xpr_sint_t         aShowCmd,
                                    DWORD              aHotKey)
{
    return insertBookmark(
        -1,
        aName,
        aPath,
        aIconPath,
        aIconIndex,
        aParam,
        aStartup,
        aShowCmd,
        aHotKey);
}

xpr_sint_t BookmarkMgr::addBookmark(BookmarkItem *aBookmark)
{
    return insertBookmark(-1, aBookmark);
}

xpr_sint_t BookmarkMgr::insertBookmark(xpr_sint_t         aBookmarkIndex,
                                       const xpr_tchar_t *aName,
                                       const xpr_tchar_t *aPath,
                                       const xpr_tchar_t *aIconPath,
                                       xpr_sint_t         aIconIndex,
                                       const xpr_tchar_t *aParam,
                                       const xpr_tchar_t *aStartup,
                                       xpr_sint_t         aShowCmd,
                                       DWORD              aHotKey)
{
    BookmarkItem *sBookmark = new BookmarkItem;
    if (sBookmark == XPR_NULL)
        return -1;

    sBookmark->mName = aName;
    sBookmark->mPath = aPath;

    if (aIconPath != XPR_NULL)
        sBookmark->mIconPath = aIconPath;

    if (aParam != XPR_NULL)
        sBookmark->mParam = aParam;

    if (aStartup != XPR_NULL)
        sBookmark->mStartup = aStartup;

    sBookmark->mIconIndex = aIconIndex;
    sBookmark->mShowCmd = aShowCmd;
    sBookmark->mHotKey = aHotKey;

    return insertBookmark(aBookmarkIndex, sBookmark);
}

xpr_sint_t BookmarkMgr::insertBookmark(xpr_sint_t aBookmarkIndex, BookmarkItem *aBookmark)
{
    BookmarkDeque::iterator sIterator = mBookmarkDeque.end();
    if (aBookmarkIndex != -1)
        sIterator = mBookmarkDeque.begin() + aBookmarkIndex;

    aBookmark->mPending = XPR_FALSE;

    mBookmarkDeque.insert(sIterator, aBookmark);

    if (aBookmarkIndex == -1)
        aBookmarkIndex = (xpr_sint_t)mBookmarkDeque.size() - 1;

    return aBookmarkIndex;
}

xpr_bool_t BookmarkMgr::initDefault(void)
{
    clear();

    getDefaultBookmark(mBookmarkDeque);

    return XPR_TRUE;
}

xpr_bool_t BookmarkMgr::getDefaultBookmark(BookmarkDeque &aBookmarkDeque)
{
    typedef std::list<xpr::tstring> StringList;
    StringList sDefaultPaths;

    {
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%Desktop%"));

        if (xpr::getOsVer() >= xpr::kOsVerWin7)
            sDefaultPaths.push_back(XPR_STRING_LITERAL("%Libraries%"));

        sDefaultPaths.push_back(XPR_STRING_LITERAL("%Personal%"));
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%MyDocuments%"));
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%MyMusic%"));
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%MyPictures%"));
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%MyVideo%"));
        sDefaultPaths.push_back(XPR_STRING_LITERAL("%MyNetwork%"));
    }

    BookmarkItem *sBookmark;
    LPITEMIDLIST sFullPidl;
    StringList::iterator sIterator;

    sIterator = sDefaultPaths.begin();
    for (; sIterator != sDefaultPaths.end(); ++sIterator)
    {
        const xpr::tstring &sPath = *sIterator;

        sFullPidl = GetEnvRealPidl(sPath);
        if (sFullPidl == XPR_NULL)
            continue;

        sBookmark = new BookmarkItem;

        GetName(sFullPidl, SHGDN_INFOLDER, sBookmark->mName);
        sBookmark->mPath = sPath;
        COM_FREE(sFullPidl);

        aBookmarkDeque.push_back(sBookmark);
    }

    sDefaultPaths.clear();

    return XPR_TRUE;
}

void BookmarkMgr::clear(void)
{
    if (mShellIcon != XPR_NULL)
    {
        mShellIcon->clear();
        mShellIcon->Stop(1000);
    }

    BookmarkItem *sBookmark;
    BookmarkDeque::iterator sIterator;

    sIterator = mBookmarkDeque.begin();
    for (; sIterator != mBookmarkDeque.end(); ++sIterator)
    {
        sBookmark = *sIterator;
        XPR_SAFE_DELETE(sBookmark);
    }

    mBookmarkDeque.clear();
}

xpr_bool_t BookmarkMgr::load(const xpr_tchar_t *aPath)
{
    clear();

    fxfile::base::ConfFileEx sConfFile(aPath);
    if (sConfFile.load() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t         i;
    BookmarkItem      *sBookmark;
    xpr_sint_t         sHotKey;
    xpr_tchar_t        sKey[0xff];
    const xpr_tchar_t *sValue;
    ConfFile::Section *sSection;

    sSection = sConfFile.findSection(kBookmarkSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    for (i = 0; ; ++i)
    {
        _stprintf(sKey, kNameKey, i + 1);

        sValue = sConfFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            break;

        sBookmark = new BookmarkItem;

        sBookmark->mName = sValue;

        _stprintf(sKey, kPathKey, i + 1);
        sBookmark->mPath = sConfFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));

        _stprintf(sKey, kIconKey, i + 1);
        sValue = sConfFile.getValueS(sSection, sKey, XPR_NULL);

        if (XPR_IS_NOT_NULL(sValue) && _tcslen(sValue) > 0)
        {
            sBookmark->mIconPath = sValue;

            _stprintf(sKey, kIconIndexKey, i + 1);
            sBookmark->mIconIndex = sConfFile.getValueI(sSection, sKey, -1);
        }

        _stprintf(sKey, kParameterKey, i + 1);
        sValue = sConfFile.getValueS(sSection, sKey, XPR_NULL);

        if (XPR_IS_NOT_NULL(sValue) && _tcslen(sValue) > 0)
        {
            sBookmark->mParam = sValue;
        }

        _stprintf(sKey, kStartupKey, i + 1);
        sValue = sConfFile.getValueS(sSection, sKey, XPR_NULL);

        if (XPR_IS_NOT_NULL(sValue) && _tcslen(sValue) > 0)
        {
            sBookmark->mStartup = sValue;
        }

        _stprintf(sKey, kShowStatusKey, i + 1);
        sBookmark->mShowCmd = sConfFile.getValueI(sSection, sKey, 0);

        if (sBookmark->mShowCmd < 0) sBookmark->mShowCmd = 0;
        if (sBookmark->mShowCmd > 2) sBookmark->mShowCmd = 0;

        _stprintf(sKey, kHotKeyKey, i + 1);
        sHotKey = sConfFile.getValueI(sSection, sKey, -1);

        if (sHotKey >= 0)
            sHotKey += 0x20030; // CTRL key
        sBookmark->mHotKey = (sHotKey != -1) ? sHotKey : 0;

        mBookmarkDeque.push_back(sBookmark);

        if (mBookmarkDeque.size() == MAX_BOOKMARK)
            break;
    }

    return XPR_TRUE;
}

xpr_bool_t BookmarkMgr::save(const xpr_tchar_t *aPath) const
{
    fxfile::base::ConfFileEx sConfFile(aPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile bookmark configuration file"));

    xpr_sint_t         i;
    xpr_sint_t         sHotKey;
    xpr_tchar_t        sKey[0xff];
    ConfFile::Section *sSection;
    BookmarkItem      *sBookmark;
    BookmarkDeque::const_iterator sIterator;

    sSection = sConfFile.addSection(kBookmarkSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mBookmarkDeque.begin();
    for (i = 0; sIterator != mBookmarkDeque.end(); ++sIterator, ++i)
    {
        sBookmark = *sIterator;
        XPR_ASSERT(sBookmark != XPR_NULL);

        _stprintf(sKey, kNameKey, i + 1);
        sConfFile.setValueS(sSection, sKey, sBookmark->mName);

        _stprintf(sKey, kPathKey, i + 1);
        sConfFile.setValueS(sSection, sKey, sBookmark->mPath);

        if (sBookmark->mIconPath.empty() == false)
        {
            _stprintf(sKey, kIconKey, i + 1);
            sConfFile.setValueS(sSection, sKey,      sBookmark->mIconPath);

            _stprintf(sKey, kIconIndexKey, i + 1);
            sConfFile.setValueI(sSection, sKey, sBookmark->mIconIndex);
        }

        if (sBookmark->mParam.empty() == false)
        {
            _stprintf(sKey, kParameterKey, i + 1);
            sConfFile.setValueS(sSection, sKey, sBookmark->mParam);
        }

        if (sBookmark->mStartup.empty() == false)
        {
            _stprintf(sKey, kStartupKey, i + 1);
            sConfFile.setValueS(sSection, sKey, sBookmark->mStartup);
        }

        if (sBookmark->mShowCmd > 0)
        {
            _stprintf(sKey, kShowStatusKey, i + 1);
            sConfFile.setValueI(sSection, sKey, sBookmark->mShowCmd);
        }

        sHotKey = sBookmark->mHotKey;
        sHotKey = (sHotKey >= 0x20030) ? (sHotKey -= 0x20030) : (-1); // CTRL Key
        if (sHotKey != -1)
        {
            _stprintf(sKey, kHotKeyKey, i + 1);
            sConfFile.setValueI(sSection, sKey, sHotKey);
        }
    }

    sConfFile.save(xpr::CharSetUtf16);

    return XPR_TRUE;
}

xpr_bool_t BookmarkMgr::load(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeBookmark, sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = BookmarkMgr::instance().load(sPath);
    if (sResult == XPR_FALSE)
        BookmarkMgr::instance().initDefault();

    return sResult;
}

xpr_bool_t BookmarkMgr::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeBookmark, sPath, XPR_MAX_PATH);

    BookmarkMgr::instance().save(sPath);

    return XPR_TRUE;
}

void BookmarkMgr::regAsyncIcon(HWND aHwnd, xpr_uint_t aMsg)
{
    if (aHwnd == XPR_NULL || aMsg == 0)
        return;

    mAsyncNotifyMap.insert(ASYNC_NOTIFY_MAP::value_type(aHwnd, aMsg));
}

void BookmarkMgr::unregAsyncIcon(HWND aHwnd)
{
    if (aHwnd == XPR_NULL)
        return;

    mAsyncNotifyMap.erase(aHwnd);
}

HICON BookmarkMgr::getTypeIcon(IconType aIconType, xpr_bool_t aDuplicate)
{
    HICON sIcon;

    if (mIconTypeMap.empty())
    {
        xpr_sint_t i;
        xpr_uint_t sId;

        for (i = IconTypeBegin; i < IconTypeEnd; ++i)
        {
            sId = 0;

            switch (i)
            {
            case IconTypeNot:     sId = IDI_NOT;     break;
            case IconTypePending: sId = IDI_PENDING; break;
            }

            if (sId >= 0)
            {
                sIcon = (HICON)::LoadImage(
                    AfxGetApp()->m_hInstance,
                    MAKEINTRESOURCE(sId),
                    IMAGE_ICON,
                    16,
                    16,
                    0);

                if (sIcon)
                {
                    mIconTypeMap[(IconType)i] = sIcon;
                }
            }
        }
    }

    ICON_TYPE_MAP::iterator sIterator = mIconTypeMap.find(aIconType);
    if (sIterator == mIconTypeMap.end())
        return XPR_FALSE;

    sIcon = sIterator->second;
    if (aDuplicate == XPR_TRUE)
        sIcon = ::DuplicateIcon(XPR_NULL, sIcon);

    return sIcon;
}

void BookmarkMgr::getAllIcon(xpr_bool_t aLarge)
{
    typedef std::list<xpr_sint_t> IndexList;

    xpr_sint_t i;
    xpr_sint_t sCount;
    HICON sIcon;
    BookmarkItem *sBookmark;
    IndexList::iterator sIterator;

    IndexList sNetworkIndexList;
    IndexList sLocalIndexList;

    sCount = getCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmark = getBookmark(i);
        if (sBookmark == XPR_NULL)
            continue;

        if (IsNetItem(sBookmark->mPath.c_str()) == XPR_TRUE)
            sNetworkIndexList.push_back(i);
        else
            sLocalIndexList.push_back(i);
    }

    sIterator = sLocalIndexList.begin();
    for (; sIterator != sLocalIndexList.end(); ++sIterator)
    {
        i = *sIterator;
        getIcon(i, sIcon);
    }

    sIterator = sNetworkIndexList.begin();
    for (; sIterator != sNetworkIndexList.end(); ++sIterator)
    {
        i = *sIterator;
        getIcon(i, sIcon);
    }

    sLocalIndexList.clear();
    sNetworkIndexList.clear();
}

BookmarkMgr::Result BookmarkMgr::getIcon(xpr_sint_t aBookmarkIndex, HICON &aIcon, xpr_bool_t aAvailableSync, xpr_bool_t aLarge)
{
    aIcon = XPR_NULL;

    BookmarkItem *sBookmark = getBookmark(aBookmarkIndex);
    if (sBookmark == XPR_NULL)
        return ResultFailed;

    if (sBookmark->mPath == BOOKMARK_SEPARATOR)
        return ResultSucceeded;

    if (sBookmark->mIconCache != XPR_NULL)
    {
        aIcon = sBookmark->mIconCache;
        return ResultSucceeded;
    }

    if (sBookmark->mPending == XPR_TRUE)
    {
        aIcon = getTypeIcon(IconTypePending);
        return ResultPending;
    }

    if (sBookmark->mIconPath.empty() && sBookmark->mPath.empty())
    {
        aIcon = getTypeIcon(IconTypeNot, XPR_TRUE);
        sBookmark->setIcon(aIcon);

        return ResultSucceeded;
    }

    //if (IsExistFile(sBookmark->mIconPath) == XPR_FALSE && ::IsExistFile(sBookmark->mPath) == XPR_FALSE)
    //{
    //    aIcon = GetTypeIcon(IconTypeNot, XPR_TRUE);
    //    sBookmark->SetIcon(aIcon);

    //    return ResultSucceeded;
    //}

    if (aAvailableSync == XPR_TRUE)
    {
        if ((mFastNetIcon == XPR_TRUE) || (mFastNetIcon == XPR_FALSE && sBookmark->isFastIcon() == XPR_TRUE))
        {
            if (sBookmark->mPath != BOOKMARK_SEPARATOR)
            {
                aIcon = ShellIcon::getIcon(sBookmark->mIconPath, sBookmark->mIconIndex, sBookmark->mPath, mFastNetIcon);
                if (aIcon == XPR_NULL)
                    aIcon = getTypeIcon(BookmarkMgr::IconTypeNot, XPR_TRUE);
            }

            sBookmark->setIcon(aIcon);

            return ResultSucceeded;
        }
    }

    if (mShellIcon == XPR_NULL)
    {
        mShellIcon = new ShellIcon;
        mShellIcon->setOwner(m_hWnd, WM_SHELL_ASYNC_ICON);
    }

    ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
    sAsyncIcon->mType       = ShellIcon::TypeIcon;
    sAsyncIcon->mFlags      = XPR_IS_TRUE(mFastNetIcon) ? ShellIcon::FlagFastNetIcon : ShellIcon::FlagNone;
    sAsyncIcon->mCode       = 0;
    sAsyncIcon->mItem       = aBookmarkIndex;
    sAsyncIcon->mSignature  = sBookmark->mSignature;
    sAsyncIcon->mPath       = sBookmark->mPath;
    sAsyncIcon->mIconPath   = sBookmark->mIconPath;
    sAsyncIcon->mIconIndex  = sBookmark->mIconIndex;

    if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(sAsyncIcon);
        return ResultFailed;
    }

    aIcon = getTypeIcon(IconTypePending);
    sBookmark->mPending = XPR_TRUE;

    return ResultPending;
}

LRESULT BookmarkMgr::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    ShellIcon::AsyncIcon *sAsyncIcon = (ShellIcon::AsyncIcon *)wParam;
    if (sAsyncIcon == XPR_NULL)
        return 0;

    xpr_sint_t sBookmarkIndex = -1;
    BookmarkItem *sBookmark = XPR_NULL;

    if (sAsyncIcon->mItem != -1 && sAsyncIcon->mItem >= 0)
    {
        sBookmark = getBookmark((xpr_sint_t)sAsyncIcon->mItem);
        if (sBookmark != XPR_NULL && sBookmark->mSignature == sAsyncIcon->mSignature)
            sBookmarkIndex = (xpr_sint_t)sAsyncIcon->mItem;
    }

    if (sBookmarkIndex < 0)
    {
        sBookmarkIndex = findSignature(sAsyncIcon->mSignature);
        if (sBookmarkIndex >= 0)
            sBookmark = getBookmark(sBookmarkIndex);
    }

    if (sBookmarkIndex >= 0 && sBookmark != XPR_NULL)
    {
        HICON sIcon = sAsyncIcon->mResult.mIcon;
        if (sIcon == XPR_NULL)
            sIcon = getTypeIcon(IconTypeNot, XPR_TRUE);

        DESTROY_ICON(sBookmark->mIconCache);
        sBookmark->mIconCache = sIcon;
        sBookmark->mPending = XPR_FALSE;

        sAsyncIcon->mResult.mIcon = XPR_NULL;

        HWND sHwnd;
        xpr_uint_t sMsg;
        ASYNC_NOTIFY_MAP::iterator sIterator;

        sIterator = mAsyncNotifyMap.begin();
        for (; sIterator != mAsyncNotifyMap.end(); ++sIterator)
        {
            sHwnd = sIterator->first;
            sMsg = sIterator->second;

            ::SendMessage(sHwnd, sMsg, (WPARAM)sBookmarkIndex, (LPARAM)sBookmark);
        }
    }

    XPR_SAFE_DELETE(sAsyncIcon);

    return 0;
}
} // namespace fxfile
