//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_scrap.h"
#include "file_scrap_observer.h"

#include "context_menu.h"
#include "conf_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
enum
{
    MODE_COPY,
    MODE_MOVE,
    MODE_DELETE,
    MODE_TRASH,
};

static const xpr_tchar_t kFileScrapSection[] = XPR_STRING_LITERAL("file_scrap");
static const xpr_tchar_t kGroupSection    [] = XPR_STRING_LITERAL("file_scrap_group_%s");
static const xpr_tchar_t kNoGroup         [] = XPR_STRING_LITERAL("no_group");
static const xpr_tchar_t kGroupKey        [] = XPR_STRING_LITERAL("file_scrap.group%d");
static const xpr_tchar_t kFileKey         [] = XPR_STRING_LITERAL("file_scrap.file%d");
static const xpr_tchar_t kDefaultKey      [] = XPR_STRING_LITERAL("file_scrap.default");

FileScrap::FileScrap(void)
    : mObserver(XPR_NULL)
    , mGroupIdMgr(InvalidGroupId)
    , mFileOpPath(XPR_NULL)
{
    mCurGroupId = mDefGroupId = setDefGroup();
}

FileScrap::~FileScrap(void)
{
    removeAllGroups();

    if (XPR_IS_NOT_NULL(mFileOpPath))
    {
        HeapFree(GetProcessHeap(), 0, mFileOpPath);
        mFileOpPath = XPR_NULL;
    }
}

void FileScrap::setObserver(FileScrapObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_uint_t FileScrap::setDefGroup(void)
{
    return addGroup(kFileScrapNoGroupName);
}

xpr_uint_t FileScrap::addGroup(const xpr_tchar_t *aGroupName)
{
    if (XPR_IS_NULL(aGroupName))
        return InvalidGroupId;

    GroupMap::iterator sIterator = mGroupMap.find(aGroupName);
    if (sIterator != mGroupMap.end())
        return InvalidGroupId;

    Group *sGroup = new Group;
    if (XPR_IS_NULL(sGroup))
        return InvalidGroupId;

    xpr_uint_t sGroupId = generateGroupId();

    sGroup->mGroupId   = sGroupId;
    sGroup->mGroupName = aGroupName;
    sGroup->mIdMgr     = 0;
    mGroupDeque.push_back(sGroup);

    mGroupMap[aGroupName] = sGroup;
    mGroupIdMap[sGroupId] = sGroup;

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onAddGroup(*this, sGroup);

    return sGroupId;
}

xpr_uint_t FileScrap::generateGroupId(void)
{
    xpr_uint_t sGroupId = mGroupIdMgr;

    do
    {
        sGroupId = ++mGroupIdMgr;
        if (sGroupId == InvalidGroupId)
            sGroupId = ++mGroupIdMgr;

        if (mGroupIdMap.find(sGroupId) == mGroupIdMap.end())
            break;
    } while (true);

    return sGroupId;
}

xpr_size_t FileScrap::getGroupCount(void) const
{
    return mGroupDeque.size();
}

FileScrap::Group *FileScrap::getGroup(xpr_size_t aIndex) const
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mGroupDeque))
        return XPR_NULL;

    return mGroupDeque[aIndex];
}

FileScrap::Group *FileScrap::findGroup(xpr_uint_t aGroupId) const
{
    GroupIdMap::const_iterator sIterator = mGroupIdMap.find(aGroupId);
    if (sIterator == mGroupIdMap.end())
        return XPR_NULL;

    return sIterator->second;
}

FileScrap::Group *FileScrap::findGroup(const xpr_tchar_t *aGroupName) const
{
    if (XPR_IS_NULL(aGroupName))
        return XPR_NULL;

    GroupMap::const_iterator sIterator = mGroupMap.find(aGroupName);
    if (sIterator == mGroupMap.end())
        return XPR_NULL;

    return sIterator->second;
}

xpr_uint_t FileScrap::findGroupId(const xpr_tchar_t *aGroupName) const
{
    Group *sGroup = findGroup(aGroupName);
    if (XPR_IS_NULL(sGroup))
        return InvalidGroupId;

    return sGroup->mGroupId;
}

xpr_bool_t FileScrap::renameGroup(const xpr_tchar_t *aOldGroupName, const xpr_tchar_t *aNewGroupName)
{
    if (XPR_IS_NULL(aOldGroupName) || XPR_IS_NULL(aNewGroupName))
        return XPR_FALSE;

    GroupMap::iterator sOldIterator = mGroupMap.find(aOldGroupName);
    GroupMap::iterator sNewIterator = mGroupMap.find(aNewGroupName);

    if (sOldIterator == mGroupMap.end()) return XPR_FALSE;
    if (sNewIterator != mGroupMap.end()) return XPR_FALSE;

    Group *sGroup = sOldIterator->second;
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    sGroup->mGroupName = aNewGroupName;

    mGroupMap.erase(sOldIterator);

    mGroupMap[sGroup->mGroupName] = sGroup;

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onRenameGroup(*this, sGroup);

    return XPR_TRUE;
}

void FileScrap::removeGroup(xpr_uint_t aGroupId)
{
    GroupIdMap::iterator sIterator = mGroupIdMap.find(aGroupId);
    if (sIterator == mGroupIdMap.end())
        return;

    Group *sGroup = sIterator->second;
    if (XPR_IS_NOT_NULL(sGroup))
    {
        {
            GroupMap::iterator sIterator = mGroupMap.find(sGroup->mGroupName.c_str());
            if (sIterator != mGroupMap.end())
                mGroupMap.erase(sIterator);
        }

        {
            GroupDeque::iterator sIterator = mGroupDeque.begin();
            for (; sIterator != mGroupDeque.end(); ++sIterator)
            {
                if ((*sIterator)->mGroupId == sGroup->mGroupId)
                {
                    mGroupDeque.erase(sIterator);
                    break;
                }
            }
        }
    }

    mGroupIdMap.erase(sIterator);

    xpr_bool_t sCurGroupChanged = XPR_FALSE;
    if (aGroupId == mCurGroupId)
    {
        mCurGroupId = mDefGroupId;
        sCurGroupChanged = XPR_TRUE;
    }

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onRemoveGroup(*this, sGroup);

    if (XPR_IS_TRUE(sCurGroupChanged))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            Group *sCurGroup = findGroup(mCurGroupId);

            mObserver->onSetCurGroup(*this, sCurGroup);
        }
    }

    XPR_SAFE_DELETE(sGroup);
}

void FileScrap::removeAllGroups(void)
{
    Group *sGroup;
    GroupDeque::iterator sIterator;

    sIterator = mGroupDeque.begin();
    for (; sIterator != mGroupDeque.end(); ++sIterator)
    {
        sGroup = *sIterator;
        XPR_SAFE_DELETE(sGroup);
    }

    mGroupDeque.clear();
    mGroupMap.clear();
    mGroupIdMap.clear();

    mGroupIdMgr = InvalidGroupId;

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onRemoveAllGroups(*this);
}

xpr_bool_t FileScrap::isItemEmpty(xpr_uint_t aGroupId) const
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return 0;

    return (sGroup->mItemDeque.empty() == true) ? XPR_TRUE : XPR_FALSE;
}

xpr_size_t FileScrap::getItemCount(xpr_uint_t aGroupId) const
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return 0;

    return sGroup->mItemDeque.size();
}

void FileScrap::setCurGroupId(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    if (mCurGroupId == aGroupId)
        return;

    mCurGroupId = aGroupId;

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onSetCurGroup(*this, sGroup);
}

xpr_uint_t FileScrap::getCurGroupId(void) const
{
    return mCurGroupId;
}

xpr_uint_t FileScrap::getDefGroupId(void) const
{
    return mDefGroupId;
}

xpr_bool_t FileScrap::addItem(xpr_uint_t aGroupId, LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_sint_t aCount)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    xpr_sint_t i;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    LPCITEMIDLIST sPidl = XPR_NULL;
    xpr_ulong_t sShellAttributes;

    for (i = 0; i < aCount; ++i)
    {
        sPidl = aPidls[i];

        sShellAttributes = SFGAO_FILESYSTEM;
        GetItemAttributes(aShellFolder, sPidl, sShellAttributes);

        if (!XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
            continue;

        GetName(aShellFolder, sPidl, SHGDN_FORPARSING, sPath);

        addItem(sGroup, sPath);
    }

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onUpdateList(*this, sGroup);

    return XPR_TRUE;
}

xpr_bool_t FileScrap::addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath, xpr_sint_t aCount)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    xpr_sint_t i;
    xpr_tchar_t *sPathEnum = (xpr_tchar_t *)aPath;

    for (i = 0; i < aCount; ++i)
    {
        addItem(sGroup, sPathEnum);

        sPathEnum += _tcslen(sPathEnum) + 1;
    }

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onUpdateList(*this, sGroup);

    return XPR_TRUE;
}

xpr_bool_t FileScrap::addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    addItem(sGroup, aPath);

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onUpdateList(*this, sGroup);

    return XPR_TRUE;
}

void FileScrap::addItem(Group *aGroup, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aGroup))
        return;

    if (isItemExist(aGroup, aPath) == XPR_TRUE)
        return;

    Item *sItem = new Item;
    if (XPR_IS_NULL(sItem))
        return;

    sItem->mGroupId = aGroup->mGroupId;
    sItem->mId      = ++aGroup->mIdMgr;
    sItem->mPath    = aPath;

    aGroup->mItemDeque.push_back(sItem);
}

xpr_bool_t FileScrap::regenerate(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    sGroup->mIdMgr = 0;

    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;

        sItem->mId = ++sGroup->mIdMgr;
    }

    return XPR_TRUE;
}

xpr_bool_t FileScrap::isItemExist(xpr_uint_t aGroupId, const xpr_tchar_t *aPath) const
{
    Group *sGroup = findGroup(aGroupId);

    return isItemExist(sGroup, aPath);
}

xpr_bool_t FileScrap::isItemExist(Group *aGroup, const xpr_tchar_t *aPath) const
{
    if (XPR_IS_NULL(aGroup))
        return XPR_FALSE;

    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = aGroup->mItemDeque.begin();
    for (; sIterator != aGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        if (_tcsicmp(sItem->mPath.c_str(), aPath) == 0)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

void FileScrap::removeItem(xpr_uint_t aGroupId, xpr_size_t aIndex)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    if (!FXFILE_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
        return;

    ItemDeque::iterator sIterator;
    sIterator = sGroup->mItemDeque.begin() + aIndex;

    Item *sItem = *sIterator;
    XPR_SAFE_DELETE(sItem);

    sGroup->mItemDeque.erase(sIterator);
}

void FileScrap::removeItems(xpr_uint_t aGroupId, const IdSet &aIdSet)
{
    if (aIdSet.empty() == true)
        return;

    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    while (sIterator != sGroup->mItemDeque.end())
    {
        sItem = *sIterator;

        if (aIdSet.find(sItem->mId) != aIdSet.end())
        {
            XPR_SAFE_DELETE(sItem);

            sIterator = sGroup->mItemDeque.erase(sIterator);
            continue;
        }

        sIterator++;
    }

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onRemovedItems(*this, sGroup, aIdSet);
}

void FileScrap::removeAllItems(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        XPR_SAFE_DELETE(sItem);
    }

    sGroup->mItemDeque.clear();

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onRemovedAllItems(*this, sGroup);
}

void FileScrap::getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex, xpr_tchar_t *aPath) const
{
    if (XPR_IS_NULL(aPath))
        return;

    const xpr_tchar_t *sPath = getItemPath(aGroupId, aIndex);
    if (XPR_IS_NULL(sPath))
        return;

    _tcscpy(aPath, sPath);
}

const xpr_tchar_t *FileScrap::getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex) const
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_NULL;

    if (!FXFILE_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
        return XPR_NULL;

    Item *sItem = sGroup->mItemDeque[aIndex];
    if (XPR_IS_NULL(sItem))
        return XPR_NULL;

    return sItem->mPath.c_str();
}

FileScrap::Item *FileScrap::getItem(xpr_uint_t aGroupId, xpr_size_t aIndex) const
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_NULL;

    if (!FXFILE_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
        return XPR_NULL;

    return sGroup->mItemDeque[aIndex];
}

xpr_tchar_t *FileScrap::getItemFileOpPath(xpr_uint_t aGroupId, xpr_size_t *aCount)
{
    if (XPR_IS_NOT_NULL(mFileOpPath))
    {
        HeapFree(GetProcessHeap(), 0, mFileOpPath);
        mFileOpPath = XPR_NULL;
    }

    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    xpr_sint_t sSize = 0;
    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NOT_NULL(sItem))
            sSize += (xpr_sint_t)sItem->mPath.length() + 1;
    }

    sSize += 10;
    mFileOpPath = (xpr_tchar_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(xpr_tchar_t) * sSize);

    xpr_tchar_t *sLast = mFileOpPath;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        _tcscpy(sLast, sItem->mPath.c_str());
        sLast += _tcslen(sLast) + 1;
    }

    // double null
    *sLast = XPR_STRING_LITERAL('\0');

    if (XPR_IS_NOT_NULL(aCount))
        *aCount = sGroup->mItemDeque.size();

    return mFileOpPath;
}

void FileScrap::copyOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget)
{
    doOperation(aGroupId, MODE_COPY, aTarget);
}

void FileScrap::moveOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget)
{
    doOperation(aGroupId, MODE_MOVE, aTarget);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        Group *sGroup = findGroup(aGroupId);

        mObserver->onUpdateList(*this, sGroup);
    }
}

void FileScrap::deleteOperation(xpr_uint_t aGroupId)
{
    doOperation(aGroupId, MODE_DELETE, XPR_NULL);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        Group *sGroup = findGroup(aGroupId);

        mObserver->onUpdateList(*this, sGroup);
    }
}

void FileScrap::trashOperation(xpr_uint_t aGroupId)
{
    doOperation(aGroupId, MODE_TRASH, XPR_NULL);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        Group *sGroup = findGroup(aGroupId);

        mObserver->onUpdateList(*this, sGroup);
    }
}

void FileScrap::doOperation(xpr_uint_t aGroupId, xpr_sint_t aMode, const xpr_tchar_t *aTarget)
{
    xpr_sint_t sFunc, sFlags = 0;

    if (aMode == MODE_COPY)        { sFunc = FO_COPY; }
    else if (aMode == MODE_MOVE)   { sFunc = FO_MOVE; }
    else if (aMode == MODE_DELETE) { sFunc = FO_DELETE; sFlags = FOF_NOCONFIRMATION; }
    else if (aMode == MODE_TRASH)  { sFunc = FO_DELETE; sFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION; }
    else return;

    // windows vista bug
    //if (GetCount() >= 2)
    //    sFlags |= FOF_MULTIDESTFILES;

    xpr_tchar_t *sSource = getItemFileOpPath(aGroupId);

    SHFILEOPSTRUCT sShFileOpStruct = {0};
    sShFileOpStruct.hwnd   = XPR_NULL;
    sShFileOpStruct.wFunc  = sFunc;
    sShFileOpStruct.fFlags = sFlags;
    sShFileOpStruct.pFrom  = sSource;
    sShFileOpStruct.pTo    = aTarget;

    if (::SHFileOperation(&sShFileOpStruct) == 0)
    {
        if (sFunc == FO_MOVE || sFunc == FO_DELETE)
            removeAllItems(aGroupId);
    }
}

xpr_bool_t FileScrap::setClipboardCopy(xpr_uint_t aGroupId)
{
    return setClipboard(aGroupId);
}

xpr_bool_t FileScrap::setClipboardCut(xpr_uint_t aGroupId)
{
    return setClipboard(aGroupId, XPR_FALSE);
}

xpr_bool_t FileScrap::setClipboard(xpr_uint_t aGroupId, xpr_bool_t aCopy)
{
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    if (FAILED(::SHGetDesktopFolder(&sShellFolder)))
        return XPR_FALSE;

    xpr_size_t sCount = 0;
    const xpr_tchar_t *sPath = getItemFileOpPath(aGroupId, &sCount);
    if (XPR_IS_NULL(sPath))
    {
        COM_RELEASE(sShellFolder);
        return XPR_FALSE;
    }

    HRESULT sHResult;
    LPITEMIDLIST sPidl;
    LPITEMIDLIST *sPidls;

    sPidls = new LPITEMIDLIST[sCount];

    xpr_size_t i;
    xpr_size_t sPidlCount = 0;
    for (i = 0; i < sCount; ++i)
    {
        sHResult = fxfile::base::Pidl::create(sPath, sPidl);
        if (SUCCEEDED(sHResult))
            sPidls[sPidlCount++] = sPidl;
        sPath += _tcslen(sPath) + 1;
    }

    LPDATAOBJECT sDataObject = XPR_NULL;
    sHResult = sShellFolder->GetUIObjectOf(
        XPR_NULL,
        (xpr_uint_t)sPidlCount,
        (LPCITEMIDLIST *)sPidls,
        IID_IDataObject, XPR_NULL,
        (LPVOID *)&sDataObject);

    ContextMenu::invokeCommand(
        sShellFolder,
        (LPCITEMIDLIST *)sPidls,
        (xpr_uint_t)sPidlCount,
        XPR_IS_TRUE(aCopy) ? CMID_VERB_COPY : CMID_VERB_CUT,
        AfxGetMainWnd()->GetSafeHwnd());

    for (i = 0; i < sCount; ++i)
    {
        COM_FREE(sPidls[i]);
    }

    XPR_SAFE_DELETE_ARRAY(sPidls);

    return XPR_TRUE;
}

xpr_size_t FileScrap::getItemCountNotExist(xpr_uint_t aGroupId) const
{
    xpr_size_t sNotExist = 0;

    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return 0;

    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        if (IsExistFile(sItem->mPath.c_str()) == XPR_FALSE)
            sNotExist++;
    }

    return sNotExist;
}

xpr_size_t FileScrap::removeItemNotExist(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return 0;

    ItemDeque::iterator sIterator;
    Item *sItem;
    IdSet sIdSet;

    sIterator = sGroup->mItemDeque.begin();
    while (sIterator != sGroup->mItemDeque.end())
    {
        sItem = *sIterator;
        if (XPR_IS_NOT_NULL(sItem))
        {
            if (IsExistFile(sItem->mPath) == XPR_FALSE)
            {
                sIdSet.insert(sItem->mId);

                XPR_SAFE_DELETE(sItem);

                sIterator = sGroup->mItemDeque.erase(sIterator);
                continue;
            }
        }

        sIterator++;
    }

    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (sIdSet.empty() == false)
            mObserver->onRemovedItems(*this, sGroup, sIdSet);
    }

    return sIdSet.size();
}

xpr_bool_t FileScrap::load(const xpr_tchar_t *aPath)
{
    // clear all
    removeAllGroups();
    setDefGroup();

    fxfile::base::ConfFileEx sConfFile(aPath);
    if (sConfFile.load() == XPR_FALSE)
        return XPR_FALSE;

    typedef std::map<xpr::tstring, xpr_uint_t> GroupIdMap;
    GroupIdMap sGroupIdMap;
    GroupIdMap::const_iterator sIterator;

    xpr_sint_t         i;
    xpr_uint_t         sGroupId;
    xpr_tchar_t        sKey[0xff];
    xpr_tchar_t        sSectionName[0xff];
    const xpr_tchar_t *sGroup;
    const xpr_tchar_t *sFile;
    const xpr_tchar_t *sDefGroupName;
    ConfFile::Section *sSection;

    // base section
    sSection = sConfFile.findSection(kFileScrapSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    for (i = 0; i < MAX_FILE_SCRAP_GROUP; ++i)
    {
        _stprintf(sKey, kGroupKey, i);

        sGroup = sConfFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sGroup))
            break;

        sGroupId = addGroup(sGroup);
        if (sGroupId != InvalidGroupId)
            sGroupIdMap[sGroup] = sGroupId;
    }

    sDefGroupName = sConfFile.getValueS(sSection, kDefaultKey, XPR_NULL);
    if (XPR_IS_NOT_NULL(sDefGroupName))
    {
        sGroupId = findGroupId(sDefGroupName);
        if (sGroupId != InvalidGroupId)
            mCurGroupId = sGroupId;
    }

    // no group section
    _stprintf(sSectionName, kGroupSection, kNoGroup);

    sSection = sConfFile.findSection(sSectionName);
    if (XPR_IS_NOT_NULL(sSection))
    {
        for (i = 0; ; ++i)
        {
            _stprintf(sKey, kFileKey, i);

            sFile = sConfFile.getValueS(sSection, sKey, XPR_NULL);
            if (XPR_IS_NULL(sFile))
                break;

            addItem(mDefGroupId, sFile);
        }
    }

    // each group sections
    sIterator = sGroupIdMap.begin();
    for (; sIterator != sGroupIdMap.end(); ++sIterator)
    {
        const xpr::tstring &sGroupName = sIterator->first;
        const xpr_uint_t   &sGroupId   = sIterator->second;

        _stprintf(sSectionName, kGroupSection, sGroupName.c_str());

        sSection = sConfFile.findSection(sSectionName);
        if (XPR_IS_NOT_NULL(sSection))
        {
            for (i = 0; ; ++i)
            {
                _stprintf(sKey, kFileKey, i);

                sFile = sConfFile.getValueS(sSection, sKey, XPR_NULL);
                if (XPR_IS_NULL(sFile))
                    break;

                addItem((xpr_uint_t)sIterator->second, sFile);
            }
        }
    }

    return XPR_TRUE;
}

xpr_bool_t FileScrap::save(const xpr_tchar_t *aPath) const
{
    fxfile::base::ConfFileEx sConfFile(aPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile file scrap file"));

    xpr_sint_t         sIndex;
    xpr_tchar_t        sKey[0xff];
    xpr_tchar_t        sValue[0xff];
    xpr_tchar_t        sSectionName[0xff];
    Group             *sGroup;
    Item              *sItem;
    ConfFile::Section *sSection;
    GroupDeque::const_iterator sGroupIterator;
    ItemDeque::const_iterator  sIterator;

    // base section
    sSection = sConfFile.addSection(kFileScrapSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sGroup = findGroup(mCurGroupId);
    if (XPR_IS_NOT_NULL(sGroup))
    {
        _tcscpy(sValue, sGroup->mGroupName.c_str());
        if (mCurGroupId == mDefGroupId)
            _tcscpy(sValue, kNoGroup);

        sConfFile.setValueS(sSection, kDefaultKey, sValue);
    }

    sGroupIterator = mGroupDeque.begin();
    for (sIndex = 0; sGroupIterator != mGroupDeque.end(); ++sGroupIterator)
    {
        sGroup = *sGroupIterator;
        XPR_ASSERT(sGroup != XPR_NULL);

        if (sGroup->mGroupId == mDefGroupId)
            continue;

        _stprintf(sKey, kGroupKey, sIndex++);

        sConfFile.setValueS(sSection, sKey, sGroup->mGroupName);
    }

    // no group section
    _stprintf(sSectionName, kGroupSection, kNoGroup);

    sSection = sConfFile.addSection(sSectionName);
    XPR_ASSERT(sSection != XPR_NULL);

    sGroup = findGroup(mDefGroupId);
    if (XPR_IS_NOT_NULL(sGroup))
    {
        sIterator = sGroup->mItemDeque.begin();
        for (sIndex = 0; sIterator != sGroup->mItemDeque.end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_ASSERT(sItem != XPR_NULL);

            _stprintf(sKey, kFileKey, sIndex++);

            sConfFile.setValueS(sSection, sKey, sItem->mPath);
        }
    }

    // each group sections
    sGroupIterator = mGroupDeque.begin();
    for (; sGroupIterator != mGroupDeque.end(); ++sGroupIterator)
    {
        sGroup = *sGroupIterator;
        XPR_ASSERT(sGroup != XPR_NULL);

        if (sGroup->mGroupId == mDefGroupId)
            continue;

        _stprintf(sSectionName, kGroupSection, sGroup->mGroupName.c_str());

        sSection = sConfFile.addSection(sSectionName);
        XPR_ASSERT(sSection != XPR_NULL);

        sIterator = sGroup->mItemDeque.begin();
        for (sIndex = 0; sIterator != sGroup->mItemDeque.end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_ASSERT(sItem != XPR_NULL);

            _stprintf(sKey, kFileKey, sIndex++);

            sConfFile.setValueS(sSection, sKey, sItem->mPath);
        }
    }

    sConfFile.save(xpr::CharSetUtf16);

    return XPR_TRUE;
}
} // namespace fxfile
