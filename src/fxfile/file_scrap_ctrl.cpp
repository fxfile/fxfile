//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_scrap_ctrl.h"
#include "file_scrap_ctrl_observer.h"

#include "fnmatch.h"            // for Name Selection, match

#include "context_menu.h"
#include "file_op_thread.h"
#include "filter.h"
#include "program_ass.h"
#include "file_scrap.h"
#include "shell_icon.h"

#include "gui/FlatHeaderCtrl.h"
#include "gui/DropSource.h"     // for Drag & Drop
#include "gui/BCMenu.h"

#include "option.h"
#include "main_frame.h"
#include "explorer_ctrl.h"
#include "command_string_table.h"

#include <intshcut.h>           // for URL
#include <mapi.h>               // for send mail

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
//
// user defined message
//
enum
{
    WM_SHELL_ASYNC_ICON    = WM_USER + 11,
    WM_SHELL_CHANGE_NOTIFY = WM_USER + 1500,
};

class FileScrapCtrl::ItemData
{
public:
    ItemData(void)
        : mFileScrapItem(XPR_NULL)
        , mFileScrapItemId(0)
        , mSignature(0)
        , mFileExist(XPR_FALSE)
        , mFileName(XPR_NULL)
        , mFileSize(0)
        , mFileAttributes(0)
    {
        memset(&mModifiedFileTime, 0, sizeof(FILETIME));
    }

    ~ItemData(void)
    {
    }

public:
    FileScrap::Item   *mFileScrapItem;
    xpr_uint_t         mFileScrapItemId;

    xpr_uint_t         mSignature;
    xpr_bool_t         mFileExist;
    xpr::tstring       mDir;
    const xpr_tchar_t *mFileName;
    xpr_uint64_t       mFileSize;
    FILETIME           mModifiedFileTime;
    DWORD              mFileAttributes;
};

xpr_uint_t FileScrapCtrl::mCodeMgr = 0;

FileScrapCtrl::FileScrapCtrl(void)
    : mObserver(XPR_NULL)
    , mSortColumn(-1), mSortAscending(XPR_TRUE)
    , mShellIcon(XPR_NULL), mCode(0), mSignature(0)
    , mGroup(XPR_NULL), mFileCount(0), mDirCount(0)
{
}

FileScrapCtrl::~FileScrapCtrl(void)
{
    XPR_SAFE_DELETE(mShellIcon);
}

void FileScrapCtrl::setObserver(FileScrapCtrlObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t FileScrapCtrl::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;
    sStyle |= LVS_REPORT;
    sStyle |= LVS_SHOWSELALWAYS;

    return super::Create(sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(FileScrapCtrl, super)
    ON_WM_CREATE()
    ON_WM_CONTEXTMENU()
    ON_WM_MOUSEACTIVATE()
    ON_WM_INITMENUPOPUP()
    ON_WM_HSCROLL()
    ON_WM_DESTROY()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_SETFOCUS()
    ON_WM_KEYDOWN()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,      OnCustomdraw)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetdispinfo)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM,     OnDeleteitem)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG,      OnBegindrag)
    ON_NOTIFY_REFLECT(LVN_BEGINRDRAG,     OnBeginrdrag)
    ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE,   OnItemActivate)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,    OnColumnclick)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN,        OnLvnKeyDown)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON,    OnShellAsyncIcon)
END_MESSAGE_MAP()

xpr_sint_t FileScrapCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    // enable vista enhanced control
    enableVistaEnhanced(XPR_TRUE);

    DWORD sExStyle = GetExtendedStyle();
    sExStyle |= LVS_EX_FULLROWSELECT;
    SetExtendedStyle(sExStyle);

    InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("file_scrap.list.column.name")),     LVCFMT_LEFT,  170);
    InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("file_scrap.list.column.location")), LVCFMT_LEFT,  200);
    InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("file_scrap.list.column.size")),     LVCFMT_RIGHT,  80);
    InsertColumn(3, theApp.loadString(XPR_STRING_LITERAL("file_scrap.list.column.type")),     LVCFMT_LEFT,  150);
    InsertColumn(4, theApp.loadString(XPR_STRING_LITERAL("file_scrap.list.column.date")),     LVCFMT_LEFT,  140);
    mHeaderCtrl->setSortImage(-1, XPR_TRUE);

    if (XPR_IS_NULL(mShellIcon))
    {
        mShellIcon = new ShellIcon;
        mShellIcon->setOwner(*this, WM_SHELL_ASYNC_ICON);
    }

    FileScrap &sFileScrap = FileScrap::instance();

    clearList();
    initList(sFileScrap.findGroup(sFileScrap.getDefGroupId()), XPR_TRUE);

    return 0;
}

void FileScrapCtrl::OnDestroy(void)
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mShellIcon))
        mShellIcon->Stop();
}

void FileScrapCtrl::clearList(void)
{
    DeleteAllItems();

    mCode = mCodeMgr++;
    if (mCode == 0)
        mCode = mCodeMgr++;

    mSignature = 0;

    mGroup     = XPR_NULL;
    mFileCount = 0;
    mDirCount  = 0;

    mSortColumn = -1;
    mSortAscending = XPR_TRUE;
    mHeaderCtrl->setSortImage(-1, XPR_TRUE);
}

void FileScrapCtrl::initList(FileScrap::Group *aGroup, xpr_bool_t aRedraw)
{
    CWaitCursor sWaitCursor;

    if (XPR_IS_TRUE(aRedraw))
        SetRedraw(XPR_FALSE);

    clearList();

    FileScrap &sFileScrap = FileScrap::instance();

    xpr_sint_t i;
    LVITEM sLvItem = {0};
    ItemData *sItemData;
    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData;
    FileScrap::Item *sFileScrapItem;
    FileScrap::ItemDeque::iterator sIterator;

    sIterator = aGroup->mItemDeque.begin();
    for (i = 0; sIterator != aGroup->mItemDeque.end(); ++sIterator, ++i)
    {
        sFileScrapItem = *sIterator;

        sItemData = new ItemData;
        if (XPR_IS_NULL(sItemData))
            break;

        sItemData->mFileScrapItem   = sFileScrapItem;
        sItemData->mFileScrapItemId = sFileScrapItem->mId;
        sItemData->mSignature       = ++mSignature;
        sItemData->mFileExist       = XPR_TRUE;

        xpr_size_t sFind = sItemData->mFileScrapItem->mPath.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
        {
            sItemData->mDir      = sItemData->mFileScrapItem->mPath.substr(0, sFind).c_str();
            sItemData->mFileName = sItemData->mFileScrapItem->mPath.c_str() + sFind + 1;
        }

        sFindFile = ::FindFirstFile(sFileScrapItem->mPath.c_str(), &sWin32FindData);
        if (sFindFile != INVALID_HANDLE_VALUE)
        {
            sItemData->mFileSize         = (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max + sWin32FindData.nFileSizeLow;
            sItemData->mFileAttributes   = sWin32FindData.dwFileAttributes;
            sItemData->mModifiedFileTime = sWin32FindData.ftLastWriteTime;

            ::FindClose(sFindFile);
        }
        else
        {
            sItemData->mFileSize         = 0;
            sItemData->mFileAttributes   = 0;
            memset(&sItemData->mModifiedFileTime, 0, sizeof(FILETIME));

            sItemData->mFileExist = XPR_FALSE;
        }

        if (IsFileSystemFolder(sFileScrapItem->mPath) == XPR_TRUE)
        {
            ++mDirCount;
        }
        else
        {
            ++mFileCount;
        }

        sLvItem.mask        = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        sLvItem.iItem       = i;
        sLvItem.iSubItem    = 0;
        sLvItem.pszText     = LPSTR_TEXTCALLBACK;
        sLvItem.iImage      = I_IMAGECALLBACK;
        sLvItem.cchTextMax  = XPR_MAX_PATH;
        sLvItem.lParam      = (LPARAM)sItemData;
        InsertItem(&sLvItem);
    }

    mGroup = aGroup;

    updateStatus();

    if (XPR_IS_TRUE(aRedraw))
        SetRedraw();
}

void FileScrapCtrl::updateStatus(void)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onUpdateStatus(*this);
    }
}

void FileScrapCtrl::getStatus(xpr_sint_t &aFileCount, xpr_sint_t &aDirCount) const
{
    aFileCount = mFileCount;
    aDirCount  = mDirCount;
}

void FileScrapCtrl::getDispFileName(const xpr_tchar_t *aFileName, xpr_tchar_t *aDispFileName)
{
    if (XPR_IS_NULL(aFileName) || XPR_IS_NULL(aDispFileName))
        return;

    _tcscpy(aDispFileName, aFileName);

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(aDispFileName);
    if (XPR_IS_NOT_NULL(sExt))
    {
        if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
            sExt[0] = XPR_STRING_LITERAL('\0');
    }
}

void FileScrapCtrl::OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO *)aNmHdr;
    *aResult = 0;

    ItemData *sItemData = (ItemData *)sLvDispInfo->item.lParam;
    if (XPR_IS_NULL(sItemData))
        return;

    LVITEM &sLvItem = sLvDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        sLvItem.pszText[0] = '\0';

        switch (sLvItem.iSubItem)
        {
        case 0:
            getDispFileName(sItemData->mFileName, sLvItem.pszText);
            break;

        case 1:
            _tcscpy(sLvItem.pszText, sItemData->mDir.c_str());
            break;

        case 2:
            if (XPR_TEST_NONE_BITS(sItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                if (XPR_IS_TRUE(sItemData->mFileExist))
                    GetFileSize(sItemData->mFileSize, sLvItem.pszText, sLvItem.cchTextMax);
            }
            break;

        case 3:
            if (XPR_IS_TRUE(sItemData->mFileExist))
                GetFileType(sItemData->mFileScrapItem->mPath.c_str(), sLvItem.pszText);
            break;

        case 4:
            if (XPR_IS_TRUE(sItemData->mFileExist))
                GetFileTime(sItemData->mModifiedFileTime, sLvItem.pszText);
            break;
        }
    }

    if (sLvItem.mask & LVIF_IMAGE)
    {
        sLvItem.state = 0;
        sLvItem.stateMask = 0;

        if (XPR_IS_FALSE(sItemData->mFileExist))
        {
            sLvItem.iImage = -1;
        }
        else
        {
            const xpr_tchar_t *sExt = GetFileExt(sItemData->mFileName);
            if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".exe")) == 0)
            {
                static xpr_sint_t sExeIconIndex = -1;
                if (sExeIconIndex == -1)
                    sExeIconIndex = GetFileExtIconIndex(XPR_STRING_LITERAL(".exe"));

                sLvItem.iImage = sExeIconIndex;

                ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
                sAsyncIcon->mType              = ShellIcon::TypeIconIndex;
                sAsyncIcon->mCode              = mCode;
                sAsyncIcon->mItem              = sLvItem.iItem;
                sAsyncIcon->mSignature         = sItemData->mSignature;
                sAsyncIcon->mPath              = sItemData->mFileScrapItem->mPath;
                sAsyncIcon->mResult.mIconIndex = -1;

                if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
                {
                    XPR_SAFE_DELETE(sAsyncIcon);
                }
            }
            else
            {
                sLvItem.iImage = GetItemIconIndex(sItemData->mFileScrapItem->mPath.c_str());
            }

            if (XPR_TEST_BITS(sItemData->mFileAttributes, FILE_ATTRIBUTE_HIDDEN))
            {
                sLvItem.mask      |= LVIF_STATE;
                sLvItem.state     |= LVIS_CUT;
                sLvItem.stateMask |= LVIS_CUT;
            }

            if (XPR_IS_NOT_NULL(sExt))
            {
                if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
                {
                    sLvItem.mask      |= LVIF_STATE;
                    sLvItem.state     |= INDEXTOOVERLAYMASK(2);
                    sLvItem.stateMask |= LVIS_OVERLAYMASK;
                }
            }

            // shell icon custom overlay
            ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
            sAsyncIcon->mType              = ShellIcon::TypeOverlayIndex;
            sAsyncIcon->mCode              = mCode;
            sAsyncIcon->mItem              = sLvItem.iItem;
            sAsyncIcon->mSignature         = sItemData->mSignature;
            sAsyncIcon->mPath              = sItemData->mFileScrapItem->mPath;
            sAsyncIcon->mResult.mIconIndex = -1;

            if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(sAsyncIcon);
            }
        }
    }

    sLvItem.mask |= LVIF_DI_SETITEM;
}

void FileScrapCtrl::OnCustomdraw(NMHDR *aNmHdr, LRESULT *aResult)
{
    NMLVCUSTOMDRAW *sNmLvCustomDraw = reinterpret_cast<NMLVCUSTOMDRAW *>(aNmHdr);
    *aResult = 0;

    if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *aResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREERASE)
    {
        *aResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        ItemData *sItemData = (ItemData *)sNmLvCustomDraw->nmcd.lItemlParam;

        if (XPR_IS_FALSE(sItemData->mFileExist))
        {
            sNmLvCustomDraw->clrText = RGB(255,0,0);
        }

        *aResult = CDRF_DODEFAULT;
    }
}

void FileScrapCtrl::OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    ItemData *sItemData = (ItemData *)sNmListView->lParam;
    XPR_SAFE_DELETE(sItemData);
}

LRESULT FileScrapCtrl::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu)
{
    LRESULT sResult;

    if (BCMenu::IsMenu(aMenu) == XPR_TRUE)
        sResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sResult = super::OnMenuChar(aChar, aFlags, aMenu);

    return sResult;
}

void FileScrapCtrl::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    //super::OnInitMenuPopup(aPopupMenu, aIndex, aSysMenu);
    ASSERT(aPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = aPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = sBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if sId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if sId(xpr_uint_t) is -1, it's sub-menu.
                    sBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

                        sBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = aPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == aPopupMenu->m_hMenu)
        sState.m_pParentMenu = aPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL && (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t sIndex;
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (sIndex = 0; sIndex < sIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == aPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = aPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = aPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = aPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL || (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 || sState.m_nID == (xpr_uint_t)-1)
            {
                continue;       // First item of popup can't be routed to.
            }
            sState.DoUpdate(this, XPR_TRUE);   // Popups are never auto disabled.
        }
        else
        {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            sState.m_pSubMenu = XPR_NULL;
            sState.DoUpdate(this, XPR_FALSE);
        }

        // Adjust for menu deletions and additions.
        xpr_uint_t sCount = aPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount && aPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void FileScrapCtrl::OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct)
{
    xpr_bool_t sSetFlag = XPR_FALSE;

    if (aMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)aMeasureItemStruct->itemID) == XPR_TRUE)
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)aMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu) == XPR_TRUE)
            {
                mMenu.MeasureItem(aMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sSetFlag))
        super::OnMeasureItem(aIdCtl, aMeasureItemStruct);
}

xpr_bool_t FileScrapCtrl::getSelPidls(LPSHELLFOLDER *aShellFolder, LPCITEMIDLIST *aPidls, xpr_sint_t &aCount)
{
    *aShellFolder = XPR_NULL;
    aCount = 0;

    xpr_sint_t sIndex = -1;

    xpr_sint_t sSelCount = 0;
    if (GetSelectedCount() > 0)
    {
        POSITION sPosition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = GetNextSelectedItem(sPosition);
            if (sIndex >= 0)
                sSelCount++;
        }
    }

    if (sSelCount == 0)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (sSelCount == 1)
    {
        LPSHELLFOLDER sShellFolder = XPR_NULL;

        ItemData *sItemData = (ItemData *)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sItemData))
        {
            const xpr_tchar_t *sPath = sItemData->mFileScrapItem->mPath.c_str();

            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sPath);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                xpr_bool_t    sSuccess;
                LPCITEMIDLIST sPidl = XPR_NULL;

                sSuccess = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
                if (XPR_IS_TRUE(sSuccess) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
                    aPidls[0] = fxfile::base::Pidl::clone(sPidl);
                else
                {
                    COM_RELEASE(sShellFolder);
                }

                COM_FREE(sFullPidl);
            }
        }

        if (XPR_IS_NOT_NULL(sShellFolder))
        {
            aCount = 1;
            *aShellFolder = sShellFolder;

            sResult = XPR_TRUE;
        }
    }
    else
    {
        POSITION sPosition = GetFirstSelectedItemPosition();
        xpr_sint_t sIndex = GetNextSelectedItem(sPosition);

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetItemText(sIndex, 1, sPath, XPR_MAX_PATH + 1);
        sPath[3] = '\0';

        LPITEMIDLIST sParentFullPidl = fxfile::base::Pidl::create(sPath);
        if (XPR_IS_NULL(sParentFullPidl))
            return XPR_FALSE;

        LPSHELLFOLDER sShellFolder2 = XPR_NULL;
        HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder2);
        if (SUCCEEDED(sHResult))
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            sHResult = sShellFolder2->BindToObject(sParentFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder);
            COM_RELEASE(sShellFolder2);

            if (SUCCEEDED(sHResult))
            {
                xpr_sint_t i = 0;
                const xpr_tchar_t *sPath;
                ItemData *sItemData = XPR_NULL;
                LPITEMIDLIST sFullPidl = XPR_NULL;
                LPITEMIDLIST sChildPidl = XPR_NULL;

                sPosition = GetFirstSelectedItemPosition();
                while (XPR_IS_NOT_NULL(sPosition))
                {
                    sIndex = GetNextSelectedItem(sPosition);

                    sItemData = (ItemData *)GetItemData(sIndex);
                    if (XPR_IS_NOT_NULL(sItemData))
                    {
                        sPath = sItemData->mFileScrapItem->mPath.c_str();

                        sFullPidl = fxfile::base::Pidl::create(sPath);
                        if (XPR_IS_NULL(sFullPidl))
                            continue;

                        sChildPidl = fxfile::base::Pidl::clone(fxfile::base::Pidl::findChildItem(sParentFullPidl, sFullPidl));
                        COM_FREE(sFullPidl);
                        if (XPR_IS_NULL(sChildPidl))
                            continue;

                        aPidls[i++] = sChildPidl;
                    }
                }

                aCount = i;
                *aShellFolder = sShellFolder;

                if (aCount > 0)
                    sResult = XPR_TRUE;
            }
        }

        COM_FREE(sParentFullPidl);
    }

    return sResult;
}

xpr_bool_t FileScrapCtrl::getSelFullPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount)
{
    *aShellFolder = XPR_NULL;
    aCount = 0;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    if (SUCCEEDED(sHResult))
    {
        xpr_sint_t i = 0;
        xpr_sint_t sIndex;
        const xpr_tchar_t *sPath;
        ItemData *sItemData = XPR_NULL;
        LPITEMIDLIST sFullPidl = XPR_NULL;

        POSITION sPosition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = GetNextSelectedItem(sPosition);

            sItemData = (ItemData *)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sItemData))
            {
                sPath = sItemData->mFileScrapItem->mPath.c_str();

                sFullPidl = fxfile::base::Pidl::create(sPath);
                if (XPR_IS_NULL(sFullPidl))
                    continue;

                aPidls[i++] = sFullPidl;
            }
        }

        aCount = i;
        *aShellFolder = sShellFolder;

        if (aCount > 0)
            sResult = XPR_TRUE;
    }

    return sResult;
}

void FileScrapCtrl::OnContextMenu(CWnd *aWnd, CPoint aPoint) 
{
    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    const DWORD sStyle = GetStyle();
    const xpr_bool_t sHorzScrl = XPR_TEST_BITS(sStyle, WS_HSCROLL);
    const xpr_bool_t sVertScrl = XPR_TEST_BITS(sStyle, WS_VSCROLL);

    CRect sHorzScrlRect(&sWindowRect);
    CRect sVertScrlRect(&sWindowRect);
    sHorzScrlRect.top  = sHorzScrlRect.bottom - GetSystemMetrics(SM_CYHSCROLL);
    sVertScrlRect.left = sVertScrlRect.right  - GetSystemMetrics(SM_CXVSCROLL);

    if ((XPR_IS_TRUE(sHorzScrl) && sHorzScrlRect.PtInRect(aPoint)) ||
        (XPR_IS_TRUE(sVertScrl) && sVertScrlRect.PtInRect(aPoint)))
    {
        // default scroll bar
        super::OnContextMenu(aWnd, aPoint);
        return;
    }

    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount > 0)
    {
        xpr_sint_t sCount = 0;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];

        if (getSelPidls(&sShellFolder, (LPCITEMIDLIST *)sPidls, sCount) == XPR_TRUE)
        {
            if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
            {
                CRect sRect;
                xpr_sint_t sFocus = GetSelectionMark();
                GetItemRect(sFocus, sRect, LVIR_BOUNDS);
                ClientToScreen(&sRect);

                aPoint.x = sRect.left;
                aPoint.y = sRect.bottom;
            }

            xpr_uint_t sFlags = TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON;
            if (XPR_IS_FALSE(gOpt->mConfig.mAnimationMenu))
                sFlags |= 0x4000L;

            CMenu sMenu;
            sMenu.CreatePopupMenu();

            xpr_bool_t sOwnerMenu = XPR_FALSE;
            ContextMenu sContextMenu(GetSafeHwnd());
            if (sContextMenu.init(sShellFolder, (LPCITEMIDLIST *)sPidls, sCount) == XPR_TRUE && sContextMenu.getMenu(&sMenu) == XPR_TRUE)
            {
                ::InsertMenu(sMenu.m_hMenu, 0, MF_BYPOSITION, sContextMenu.getFirstId() + CMID_OPEN_PARENT_FOLDER, theApp.loadString(XPR_STRING_LITERAL("cmd.search_result.open_parent_folder")));
                ::InsertMenu(sMenu.m_hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);
                //::SetMenuDefaultItem(sMenu.m_hMenu, 0, XPR_TRUE);

                xpr_uint_t sId = ::TrackPopupMenuEx(sMenu.m_hMenu, sFlags, aPoint.x, aPoint.y, m_hWnd, XPR_NULL);
                if (sId != -1)
                {
                    sId -= sContextMenu.getFirstId();

                    if (invokeCommandSelf(&sContextMenu, sId) == XPR_FALSE)
                        sContextMenu.invokeCommand(sId);
                }

                sOwnerMenu = XPR_TRUE;
            }

            sMenu.DestroyMenu();
            sContextMenu.destroySubclass();
            sContextMenu.release();

            if (XPR_IS_FALSE(sOwnerMenu))
            {
                ContextMenu::trackItemMenu(sShellFolder, (LPCITEMIDLIST *)sPidls, sCount, &aPoint, sFlags, m_hWnd);
            }
        }

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            COM_FREE(sPidls[i]);
        }

        COM_RELEASE(sShellFolder);
        XPR_SAFE_DELETE_ARRAY(sPidls);
    }
    else
    {
        //if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
        //{
        //    aPoint.x = sWindowRect.left;
        //    aPoint.y = sWindowRect.top;
        //}

        //BCMenu sMenu;
        //if (sMenu.LoadMenu(IDR_SEARCH_RESULT) == XPR_TRUE)
        //{
        //    BCMenu *sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
        //    if (XPR_IS_NOT_NULL(sPopupMenu))
        //    {
        //        sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
        //    }
        //}
    }
}

xpr_bool_t FileScrapCtrl::invokeCommandSelf(ContextMenu *aContextMenu, xpr_uint_t aId)
{
    if (XPR_IS_NULL(aContextMenu))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sIndex = GetSelectionMark();

    if (aId == CMID_OPEN_PARENT_FOLDER)
    {
        ItemData *sItemData = (ItemData *)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sItemData))
        {
            if (XPR_IS_NOT_NULL(mObserver))
            {
                const xpr_tchar_t *sDir     = sItemData->mDir.c_str();
                const xpr_tchar_t *sSelPath = sItemData->mFileScrapItem->mPath.c_str();

                mObserver->onOpenFolder(*this, sDir, sSelPath);
            }
        }
    }

    return sResult;
}

void FileScrapCtrl::OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    beginDragDrop(sNmListView);
}

void FileScrapCtrl::OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    beginDragDrop(sNmListView);
}

xpr_bool_t FileScrapCtrl::setDataObject(LPDATAOBJECT *aDataObject, xpr_uint_t SFGAO_TYPE)
{
    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount <= 0)
        return XPR_FALSE;

    LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];
    if (XPR_IS_NULL(sPidls))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sCount = 0;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    if (getSelPidls(&sShellFolder, (LPCITEMIDLIST *)sPidls, sCount) == XPR_TRUE)
    {
        if (sCount > 0)
        {
            HRESULT sHResult;
            sHResult = sShellFolder->GetUIObjectOf(
                m_hWnd,
                sCount,
                (LPCITEMIDLIST *)sPidls,
                IID_IDataObject,
                0,
                (LPVOID *)aDataObject);
            if (SUCCEEDED(sHResult))
                sResult = XPR_TRUE;
        }
    }

    for (xpr_sint_t i = 0; i < sCount; ++i)
    {
        COM_FREE(sPidls[i]);
    }

    COM_RELEASE(sShellFolder);
    XPR_SAFE_DELETE_ARRAY(sPidls);

    return sResult;
}

void FileScrapCtrl::beginDragDrop(NM_LISTVIEW *aNmListView)
{
    LPDATAOBJECT sDataObject = XPR_NULL;
    DWORD sDropEffect;

    if (setDataObject(&sDataObject, XPR_NULL) == XPR_TRUE)
    {
        LPDROPSOURCE sDataSource = new DropSource();
        if (XPR_IS_NOT_NULL(sDataSource))
        {
            IDragSourceHelper *sDragSourceHelper = XPR_NULL;

            if (XPR_IS_FALSE(gOpt->mConfig.mDragNoContents))
            {
                ::CoCreateInstance(
                    CLSID_DragDropHelper,
                    XPR_NULL,
                    CLSCTX_INPROC_SERVER,
                    IID_IDragSourceHelper,
                    (LPVOID *)&sDragSourceHelper);
            }

            if (XPR_IS_NOT_NULL(sDragSourceHelper))
            {
                CPoint sPoint(-20,0);
                GetCursorPos(&sPoint);
                ScreenToClient(&sPoint);

                HRESULT sHResult;
                sHResult = sDragSourceHelper->InitializeFromWindow(GetSafeHwnd(), &sPoint, sDataObject);
            }

            ::DoDragDrop(
                sDataObject,
                sDataSource,
                DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
                &sDropEffect);

            COM_RELEASE(sDragSourceHelper);
        }

        COM_RELEASE(sDataSource);
    }

    COM_RELEASE(sDataObject);
}

void FileScrapCtrl::executeSelFolder(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (mObserver->onOpenFolder(*this, aPath, XPR_NULL) == XPR_FALSE)
        {
            doExecuteError(aPath);
        }
    }
}

void FileScrapCtrl::executeLink(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath) == XPR_FALSE)
        return;

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sPath);
    if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
    {
        executeUrl(aShellFolder, aPidl);
    }
    else
    {
        LPITEMIDLIST sFullPidl = XPR_NULL;
        xpr_tchar_t sWorking[XPR_MAX_PATH + 1] = {0};

        if (ResolveShortcut(GetSafeHwnd(), sPath, &sFullPidl, XPR_NULL, sWorking) == XPR_TRUE && XPR_IS_NOT_NULL(sFullPidl))
        {
            xpr_bool_t    sResult;
            LPSHELLFOLDER sShellFolder2 = XPR_NULL;
            LPCITEMIDLIST sPidl2        = XPR_NULL;

            sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder2, sPidl2);
            if (XPR_IS_TRUE(sResult))
            {
                xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
                sShellAttributes = GetItemAttributes(sShellFolder2, sPidl2);

                xpr_bool_t sSucceeded = XPR_TRUE;

                // check for broken link file path
                if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
                {
                    xpr_tchar_t sLink[XPR_MAX_PATH + 1] = {0};
                    GetName(sShellFolder2, sPidl2, SHGDN_FORPARSING, sLink);

                    if (IsExistFile(sLink) == XPR_FALSE)
                        sSucceeded = XPR_FALSE;
                }

                if (XPR_IS_TRUE(sSucceeded))
                {
                    if (XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER)) // case 2: Folder Link
                        executeLinkFolder(sFullPidl);
                    else // case 3: File Link
                        ExecFile(sPidl2, sWorking);
                }
                else
                {
                    doExecuteError(sShellFolder2, sPidl2);
                }
            }

            COM_RELEASE(sShellFolder2);
        }

        COM_FREE(sFullPidl);
    }
}

void FileScrapCtrl::executeUrl(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    HRESULT sHResult;
    IUniformResourceLocator *sUniformResourceLocator = XPR_NULL;

    sHResult = aShellFolder->GetUIObjectOf(
        m_hWnd,
        1,
        (LPCITEMIDLIST *)&aPidl,
        IID_IUniformResourceLocator,
        XPR_NULL,
        reinterpret_cast<LPVOID *>(&sUniformResourceLocator));

    xpr_tchar_t *sUrl = XPR_NULL;
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sUniformResourceLocator))
    {
        sHResult = sUniformResourceLocator->GetURL(&sUrl);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sUrl))
            NavigateURL(sUrl);
    }

    COM_RELEASE(sUniformResourceLocator);
    COM_FREE(sUrl);
}

void FileScrapCtrl::executeLinkFolder(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (mObserver->onOpenFolder(*this, aFullPidl) == XPR_FALSE)
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            GetName(aFullPidl, SHGDN_FORPARSING, sPath);

            doExecuteError(sPath);
        }
    }
}

void FileScrapCtrl::doExecuteError(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);

    doExecuteError(sPath);
}

void FileScrapCtrl::doExecuteError(const xpr_tchar_t *aPath)
{
    xpr_tchar_t sMsg[XPR_MAX_PATH * 2] = {0};
    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("search_result.msg.wrong_path"), XPR_STRING_LITERAL("%s")), aPath);
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
}

void FileScrapCtrl::executeAllSelItems(void)
{
    xpr_sint_t sIndex;
    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        execute(sIndex);
    }
}

void FileScrapCtrl::execute(xpr_sint_t aIndex)
{
    // case 1: open folder
    // case 2: open folder linke
    // case 3: open file link
    // case 4: open url
    // case 5: open file

    if (aIndex == -1)
    {
        aIndex = GetSelectionMark();
    }

    ItemData *sItemData = (ItemData *)GetItemData(aIndex);
    if (XPR_IS_NULL(sItemData))
        return;

    const xpr_tchar_t *sPath = sItemData->mFileScrapItem->mPath.c_str();

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sPath);
    if (XPR_IS_NULL(sFullPidl))
        return;

    xpr_bool_t    sResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
    {
        COM_FREE(sFullPidl);
        return;
    }

    xpr_ulong_t sShellAttributes = 0xFFFFFFFF;
    GetItemAttributes(sShellFolder, sPidl, sShellAttributes);

    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);

    // check for ZIP file on Windows XP or higher.
    xpr_bool_t sFolder = XPR_FALSE;
    if (sShellAttributes & SFGAO_FOLDER)
    {
        sFolder = XPR_TRUE;
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
        {
            WIN32_FIND_DATA sWin32FindData = {0};
            HRESULT sHResult = SHGetDataFromIDList(sShellFolder, sPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
            if (SUCCEEDED(sHResult))
            {
                sFolder = XPR_FALSE;
                if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                    sFolder = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_TRUE(sFolder))
    {
        executeSelFolder(sPath);
    }
    else
    {
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_LINK))
        {
            executeLink(sShellFolder, sPidl);
        }
        else
        {
            ExecFile(sFullPidl);
        }
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);
}

void FileScrapCtrl::OnItemActivate(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMITEMACTIVATE sNmItemActivate = (LPNMITEMACTIVATE)aNmHdr;
    *aResult = 0;

    if (GetSelectedCount() <= 0)
        return;

    execute(sNmItemActivate->iItem);
}

xpr_sint_t FileScrapCtrl::OnMouseActivate(CWnd *aDesktopWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage) 
{
    CPoint sPoint;
    GetCursorPos(&sPoint);
    ScreenToClient(&sPoint);

    xpr_sint_t sIndex = HitTest(sPoint);
    if (sIndex >= 0)
        return MA_NOACTIVATE;

    return super::OnMouseActivate(aDesktopWnd, aHitTest, aMessage);
}

xpr_bool_t FileScrapCtrl::PreTranslateMessage(MSG *aMsg) 
{
    static xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
    sClassName[0] = '\0';

    // Skip Edit Control
    static const xpr_tchar_t *sEditClassName = XPR_STRING_LITERAL("Edit");
    ::GetClassName(aMsg->hwnd, sClassName, MAX_CLASS_NAME);
    if (_tcsicmp(sClassName, sEditClassName) == 0)
        return XPR_FALSE;

    if (aMsg->message == WM_KEYDOWN)
    {
        switch (aMsg->wParam)
        {
        case VK_RETURN:
            if (GetAsyncKeyState(VK_CONTROL) < 0)
                gFrame->executeCommand(ID_FILE_EXEC_PARAM);
            else
                gFrame->executeCommand(ID_FILE_EXEC);
            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(aMsg);
}

void FileScrapCtrl::OnHScroll(xpr_uint_t aSBCode, xpr_uint_t aPos, CScrollBar *aScrollBar) 
{
    super::OnHScroll(aSBCode, aPos, aScrollBar);
}

void FileScrapCtrl::refresh(void)
{
    FileScrap::Group *sGroup = mGroup;

    clearList();
    initList(sGroup, XPR_TRUE);
}

void FileScrapCtrl::sortItems(xpr_sint_t aColumn)
{
    mSortAscending = (aColumn == mSortColumn) ? !mSortAscending : XPR_TRUE;
    sortItems(aColumn, mSortAscending);
}

void FileScrapCtrl::sortItems(xpr_sint_t aColumn, xpr_bool_t aAscending)
{
    if (gOpt->mConfig.mExplorerNoSort == XPR_TRUE)
        return;

    mSortColumn = aColumn;
    mSortAscending = aAscending;

    SortItems(DefaultItemCompareProc, MAKELONG(mSortColumn, mSortAscending));

    if (XPR_IS_NOT_NULL(mHeaderCtrl))
        mHeaderCtrl->setSortImage(mSortColumn, mSortAscending);
}

void FileScrapCtrl::OnColumnclick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    xpr_sint_t sColumn = sNmListView->iSubItem;
    sortItems(sColumn);

    *aResult = 0;
}

xpr_sint_t CALLBACK FileScrapCtrl::DefaultItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    xpr_sint_t sResult = 0;

    ItemData *sItemData1 = (ItemData *)lParam1;
    ItemData *sItemData2 = (ItemData *)lParam2;
    xpr_bool_t sAscending = HIWORD(lParamSort);
    xpr_sint_t sColumn = LOWORD(lParamSort);

    if (sColumn == 0)
    {
        if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
            if (sResult == 0)
                sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
        }
        else if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 1)
    {
        sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
        if (sResult == 0)
        {
            if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
            }
            else if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = -1;
            }
            else if (((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = 1;
            }
        }
    }
    else if (sColumn == 2)
    {
        if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (sItemData1->mFileSize < sItemData2->mFileSize)
                sResult = -1;
            else if (sItemData1->mFileSize > sItemData2->mFileSize)
                sResult = 1;
            else
            {
                sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
            }
        }
        else if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 3)
    {
        if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
            }
            else
            {
                xpr_tchar_t *sExt1 = (xpr_tchar_t *)GetFileExt(sItemData1->mFileName);
                xpr_tchar_t *sExt2 = (xpr_tchar_t *)GetFileExt(sItemData2->mFileName);

                if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2))
                {
                    sResult = lstrcmpi(sExt1, sExt2);
                    if (sResult == 0)
                    {
                        sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
                        if (sResult == 0)
                            sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
                    }
                }
                else if (XPR_IS_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2))
                    sResult = -1;
                else if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NULL(sExt2))
                    sResult = 1;
            }
        }
        else if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 4)
    {
        if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = ::CompareFileTime(&sItemData1->mModifiedFileTime, &sItemData2->mModifiedFileTime);
            if (sResult == 0)
            {
                sResult = lstrcmpi(sItemData1->mFileName, sItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sItemData1->mDir.c_str(), sItemData2->mDir.c_str());
            }
        }
        else if ((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }

    return (sResult * (XPR_IS_TRUE(sAscending) ? 1 : -1));
}

void FileScrapCtrl::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (mObserver != XPR_NULL)
    {
        mObserver->onSetFocus(*this);
    }
}

void FileScrapCtrl::OnKeyDown(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags) 
{
    if (aChar == VK_TAB)
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->onMoveFocus(*this);
        return;
    }

    super::OnKeyDown(aChar, aRepCnt, aFlags);
}

void FileScrapCtrl::OnLvnKeyDown(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_KEYDOWN *sLvKeyDown = (LV_KEYDOWN*)aNmHdr;
    *aResult = 0;
}

LRESULT FileScrapCtrl::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    ShellIcon::AsyncIcon *sAsyncIcon = (ShellIcon::AsyncIcon *)wParam;
    if (XPR_IS_NULL(sAsyncIcon))
        return 0;

    if (sAsyncIcon->mCode == mCode)
    {
        xpr_sint_t sIndex = -1;
        ItemData *sItemData = XPR_NULL;

        if (sAsyncIcon->mItem != -1 && sAsyncIcon->mItem >= 0)
        {
            sItemData = (ItemData *)GetItemData((xpr_sint_t)sAsyncIcon->mItem);
            if (XPR_IS_NOT_NULL(sItemData) && sItemData->mSignature == sAsyncIcon->mSignature)
                sIndex = (xpr_sint_t)sAsyncIcon->mItem;
        }

        if (sIndex < 0)
        {
            sIndex = findItemSignature(sAsyncIcon->mSignature);
            if (sIndex >= 0)
                sItemData = (ItemData *)GetItemData(sIndex);
        }

        if (sIndex >= 0 && XPR_IS_NOT_NULL(sItemData))
        {
            switch (sAsyncIcon->mType)
            {
            case ShellIcon::TypeIconIndex:
                {
                    LVITEM sLvItem = {0};
                    sLvItem.mask   = LVIF_IMAGE;
                    sLvItem.iItem  = sIndex;
                    sLvItem.iImage = sAsyncIcon->mResult.mIconIndex;
                    SetItem(&sLvItem);
                    break;
                }

            case ShellIcon::TypeOverlayIndex:
                {
                    xpr_uint_t sState = 0;

                    if (sAsyncIcon->mResult.mIconIndex >= 0)
                        sState = INDEXTOOVERLAYMASK(sAsyncIcon->mResult.mIconIndex);
                    else
                    {
                        const xpr_tchar_t *sExt = GetFileExt(sItemData->mFileName);
                        if (XPR_IS_NOT_NULL(sExt))
                        {
                            if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
                                sState = INDEXTOOVERLAYMASK(2);
                        }
                    }

                    SetItemState(sIndex, sState, LVIS_OVERLAYMASK);
                    break;
                }
            }
        }
    }

    XPR_SAFE_DELETE(sAsyncIcon);

    return 0;
}

xpr_sint_t FileScrapCtrl::findItemSignature(xpr_uint_t aSignature)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    ItemData *sItemData;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sItemData = (ItemData *)GetItemData(i);
        if (XPR_IS_NULL(sItemData))
            continue;

        if (sItemData->mSignature == aSignature)
            return i;
    }

    return -1;
}

void FileScrapCtrl::selectAll(void)
{
    xpr_sint_t sCount = GetItemCount();

    xpr_sint_t nFocus = GetSelectionMark();
    for (xpr_sint_t i = 0; i < sCount; ++i)
        SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

    SetItemState(nFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void FileScrapCtrl::unselectAll(void)
{
    xpr_sint_t sCount = GetItemCount();
    for (xpr_sint_t i = 0; i < sCount; ++i)
        SetItemState(i, 0, LVIS_SELECTED);
}

void FileScrapCtrl::removeItems(const FileScrap::IdSet &aIdSet)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    ItemData *sItemData;

    sCount = GetItemCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sItemData = (ItemData *)GetItemData(i);

        if (aIdSet.find(sItemData->mFileScrapItemId) != aIdSet.end())
        {
            DeleteItem(i);
        }
    }
}

void FileScrapCtrl::getSelFileScrapItemIdSet(FileScrap::IdSet &sIdSet)
{
    xpr_sint_t sIndex;
    POSITION sPosition;
    ItemData *sItemData;

    sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sItemData = (ItemData *)GetItemData(sIndex);

        sIdSet.insert(sItemData->mFileScrapItemId);
    }
}
} // namespace fxfile
