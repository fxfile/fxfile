//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_scrap.h"

#include "fxb_context_menu.h"
#include "fxb_ini_file.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
enum
{
    MODE_COPY,
    MODE_MOVE,
    MODE_DELETE,
    MODE_RECYCLEBIN,
};

FileScrap::FileScrap(void)
    : mIdMgr(InvalidGroupId)
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
    mGroupDeque.push_back(sGroup);

    mGroupMap[aGroupName] = sGroup;
    mGroupIdMap[sGroupId] = sGroup;

    return sGroupId;
}

xpr_uint_t FileScrap::generateGroupId(void)
{
    xpr_uint_t sGroupId = mIdMgr;

    while (true)
    {
        sGroupId = ++mIdMgr;
        if (sGroupId == InvalidGroupId)
            sGroupId = ++mIdMgr;

        if (mGroupIdMap.find(sGroupId) == mGroupIdMap.end())
            break;
    }

    return sGroupId;
}

xpr_size_t FileScrap::getGroupCount(void)
{
    return mGroupDeque.size();
}

FileScrap::Group *FileScrap::getGroup(xpr_size_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mGroupDeque))
        return XPR_NULL;

    return mGroupDeque[aIndex];
}

FileScrap::Group *FileScrap::findGroup(xpr_uint_t aGroupId)
{
    GroupIdMap::iterator sIterator = mGroupIdMap.find(aGroupId);
    if (sIterator == mGroupIdMap.end())
        return XPR_NULL;

    return sIterator->second;
}

FileScrap::Group *FileScrap::findGroup(const xpr_tchar_t *aGroupName)
{
    if (XPR_IS_NULL(aGroupName))
        return XPR_NULL;

    GroupMap::iterator sIterator = mGroupMap.find(aGroupName);
    if (sIterator == mGroupMap.end())
        return XPR_NULL;

    return sIterator->second;
}

xpr_uint_t FileScrap::findGroupId(const xpr_tchar_t *aGroupName)
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

    XPR_SAFE_DELETE(sGroup);

    mGroupIdMap.erase(sIterator);

    if (aGroupId == mCurGroupId)
        mCurGroupId = mDefGroupId;
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

    mIdMgr = InvalidGroupId;
}

xpr_size_t FileScrap::getItemCount(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return 0;

    return sGroup->mItemDeque.size();
}

void FileScrap::setCurGourpId(xpr_uint_t aGroupId)
{
    if (findGroup(aGroupId) == XPR_NULL)
        return;

    mCurGroupId = aGroupId;
}

xpr_uint_t FileScrap::getCurGroupId(void) const
{
    return mCurGroupId;
}

xpr_uint_t FileScrap::getDefGroupId(void) const
{
    return mDefGroupId;
}

void FileScrap::addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath)
{
    if (aGroupId == InvalidGroupId || XPR_IS_NULL(aPath))
        return;

    if (isItemExist(aGroupId, aPath) == XPR_TRUE)
        return;

    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    Item *sItem = new Item;
    if (XPR_IS_NULL(sItem))
        return;

    sItem->mGroupId = aGroupId;
    sItem->mPath    = aPath;

    sGroup->mItemDeque.push_back(sItem);
}

xpr_bool_t FileScrap::isItemExist(xpr_uint_t aGroupId, const xpr_tchar_t *aPath)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    Item *sItem;
    ItemDeque::iterator sIterator;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
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

    if (!XPR_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
        return;

    ItemDeque::iterator sIterator;
    sIterator = sGroup->mItemDeque.begin() + aIndex;

    Item *sItem = *sIterator;
    XPR_SAFE_DELETE(sItem);

    sGroup->mItemDeque.erase(sIterator);
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
}

void FileScrap::getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex, xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    const xpr_tchar_t *sPath = getItemPath(aGroupId, aIndex);
    if (XPR_IS_NULL(sPath))
        return;

    _tcscpy(aPath, sPath);
}

const xpr_tchar_t *FileScrap::getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_NULL;

    if (!XPR_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
        return XPR_NULL;

    Item *sItem = sGroup->mItemDeque[aIndex];
    if (XPR_IS_NULL(sItem))
        return XPR_NULL;

    return sItem->mPath.c_str();
}

FileScrap::Item *FileScrap::getItem(xpr_uint_t aGroupId, xpr_size_t aIndex)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_NULL;

    if (!XPR_STL_IS_INDEXABLE(aIndex, sGroup->mItemDeque))
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
}

void FileScrap::deleteOperation(xpr_uint_t aGroupId)
{
    doOperation(aGroupId, MODE_DELETE, XPR_NULL);
}

void FileScrap::recycleBinOperation(xpr_uint_t aGroupId)
{
    doOperation(aGroupId, MODE_RECYCLEBIN, XPR_NULL);
}

void FileScrap::doOperation(xpr_uint_t aGroupId, xpr_sint_t aMode, const xpr_tchar_t *aTarget)
{
    xpr_sint_t sFunc, sFlags = 0;

    if (aMode == MODE_COPY)            { sFunc = FO_COPY; }
    else if (aMode == MODE_MOVE)       { sFunc = FO_MOVE; }
    else if (aMode == MODE_DELETE)     { sFunc = FO_DELETE; sFlags = FOF_NOCONFIRMATION; }
    else if (aMode == MODE_RECYCLEBIN) { sFunc = FO_DELETE; sFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION; }
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
        sHResult = SHGetPidlFromPath(sPath, &sPidl);
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
        sPidls,
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

xpr_bool_t FileScrap::isItemFileAllExist(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return XPR_TRUE;

    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    for (; sIterator != sGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        if (IsExistFile(sItem->mPath.c_str()) == XPR_FALSE)
            return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_size_t FileScrap::getItemCountNotExist(xpr_uint_t aGroupId)
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

void FileScrap::removeItemNotExist(xpr_uint_t aGroupId)
{
    Group *sGroup = findGroup(aGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    ItemDeque::iterator sIterator;
    Item *sItem;

    sIterator = sGroup->mItemDeque.begin();
    while (sIterator != sGroup->mItemDeque.end())
    {
        sItem = *sIterator;
        if (XPR_IS_NOT_NULL(sItem))
        {
            if (IsExistFile(sItem->mPath) == XPR_FALSE)
            {
                sIterator = sGroup->mItemDeque.erase(sIterator);
                continue;
            }
        }

        sIterator++;
    }
}

static const xpr_tchar_t _fxFileScrap[] = XPR_STRING_LITERAL("FileScrap");
static const xpr_tchar_t _fxDefault[]   = XPR_STRING_LITERAL("Default");
static const xpr_tchar_t _fxNoGroup[]   = XPR_STRING_LITERAL("NoGroup");
static const xpr_tchar_t _fxVersion[]   = XPR_STRING_LITERAL("version");

xpr_bool_t FileScrap::loadFromFile(const xpr_tchar_t *aPath)
{
    removeAllGroups();
    setDefGroup();

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sExt[XPR_MAX_PATH + 1] = {0};
    SplitPathExt(aPath, sDir, sFileName, sExt);

    IniFile sIniFile(aPath);
    if (sIniFile.readFileW() == XPR_FALSE)
        return XPR_FALSE;

    typedef std::map<std::tstring, xpr_uint_t> GroupIdMap;
    GroupIdMap sGroupIdMap;

    xpr_sint_t i;
    xpr_uint_t sGroupId;
    xpr_tchar_t sEntry[0xff];
    const xpr_tchar_t *sGroup;
    const xpr_tchar_t *sFile;

    for (i = 0; i < MAX_FILE_SCRAP_GROUP; ++i)
    {
        _stprintf(sEntry, XPR_STRING_LITERAL("Group%d"), i);

        sGroup = sIniFile.getValueS(_fxFileScrap, sEntry);
        if (XPR_IS_NULL(sGroup))
            break;

        sGroupId = addGroup(sGroup);
        if (sGroupId != InvalidGroupId)
            sGroupIdMap[sGroup] = sGroupId;
    }

    const xpr_tchar_t *sDefGroup = sIniFile.getValueS(_fxFileScrap, _fxDefault, XPR_NULL);
    if (XPR_IS_NOT_NULL(sDefGroup))
    {
        sGroupId = findGroupId(sDefGroup);
        if (sGroupId != InvalidGroupId)
            mCurGroupId = sGroupId;
    }

    for (i = 0; ; ++i)
    {
        _stprintf(sEntry, XPR_STRING_LITERAL("File%d"), i);

        sFile = sIniFile.getValueS(_fxNoGroup, sEntry);
        if (XPR_IS_NULL(sFile))
            break;

        addItem(mDefGroupId, sFile);
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    GroupIdMap::iterator sIterator;

    sIterator = sGroupIdMap.begin();
    for (; sIterator != sGroupIdMap.end(); ++sIterator)
    {
        _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s_%s%s"), sDir, sFileName, sIterator->first.c_str(), sExt);

        IniFile sIniFile(sPath);
        if (sIniFile.readFileW() == XPR_FALSE)
            continue;

        for (i = 0; ; ++i)
        {
            _stprintf(sEntry, XPR_STRING_LITERAL("File%d"), i);

            sFile = sIniFile.getValueS(_fxFileScrap, sEntry);
            if (XPR_IS_NULL(sFile))
                break;

            addItem((xpr_uint_t)sIterator->second, sFile);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t FileScrap::saveToFile(const xpr_tchar_t *aPath)
{
    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sExt[XPR_MAX_PATH + 1] = {0};
    SplitPathExt(aPath, sDir, sFileName, sExt);

    IniFile sIniFile(aPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer FileScrap File"));

    // version
    sIniFile.setValueS(_fxFileScrap, _fxVersion, XPR_STRING_LITERAL("1.2"));

    xpr_sint_t sIndex;
    xpr_tchar_t sEntry[0xff];
    xpr_tchar_t sValue[0xff];

    GroupDeque::iterator sGroupIterator;
    Group *sGroup;
    ItemDeque::iterator sIterator;
    Item *sItem;

    sGroup = findGroup(mCurGroupId);
    if (XPR_IS_NOT_NULL(sGroup))
    {
        _tcscpy(sValue, sGroup->mGroupName.c_str());
        if (mCurGroupId == mDefGroupId)
            _tcscpy(sValue, _fxNoGroup);

        sIniFile.setValueS(_fxFileScrap, _fxDefault, sValue);
    }

    sGroupIterator = mGroupDeque.begin();
    for (sIndex = 0; sGroupIterator != mGroupDeque.end(); ++sGroupIterator)
    {
        sGroup = *sGroupIterator;
        if (XPR_IS_NULL(sGroup))
            continue;

        if (sGroup->mGroupId == mDefGroupId)
            continue;

        _stprintf(sEntry, XPR_STRING_LITERAL("Group%d"), sIndex++);
        sIniFile.setValueS(_fxFileScrap, sEntry, sGroup->mGroupName.c_str());
    }

    sGroup = findGroup(mDefGroupId);
    if (XPR_IS_NOT_NULL(sGroup))
    {
        sIterator = sGroup->mItemDeque.begin();
        for (sIndex = 0; sIterator != sGroup->mItemDeque.end(); ++sIterator)
        {
            sItem = *sIterator;
            if (XPR_IS_NULL(sItem))
                continue;

            _stprintf(sEntry, XPR_STRING_LITERAL("File%d"), sIndex++);
            sIniFile.setValueS(_fxNoGroup, sEntry, sItem->mPath.c_str());
        }
    }

    sIniFile.writeFileW();

    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

        sGroupIterator = mGroupDeque.begin();
        for (; sGroupIterator != mGroupDeque.end(); ++sGroupIterator)
        {
            sGroup = *sGroupIterator;
            if (XPR_IS_NULL(sGroup))
                continue;

            if (sGroup->mGroupId == mDefGroupId)
                continue;

            _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s_%s%s"), sDir, sFileName, sGroup->mGroupName.c_str(), sExt);

            IniFile sIniFile(sPath);
            sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer FileScrap File"));

            sIterator = sGroup->mItemDeque.begin();
            for (sIndex = 0; sIterator != sGroup->mItemDeque.end(); ++sIterator)
            {
                sItem = *sIterator;
                if (XPR_IS_NULL(sItem))
                    continue;

                _stprintf(sEntry, XPR_STRING_LITERAL("File%d"), sIndex++);
                sIniFile.setValueS(_fxFileScrap, sEntry, sItem->mPath.c_str());
            }

            sIniFile.writeFileW();
        }
    }

    return XPR_TRUE;
}
} // namespace fxb
