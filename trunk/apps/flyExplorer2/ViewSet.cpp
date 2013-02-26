//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ViewSet.h"

#include "fxb/fxb_ini_file_ex.h"
#include "fxb/fxb_md5.h"
#include "CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EACH_VIEW_SET_FILENAME                  XPR_STRING_LITERAL(".view_set.conf")

static const xpr_tchar_t kIndexSection     [] = XPR_STRING_LITERAL("view_set_index");
static const xpr_tchar_t kIndexPathKey     [] = XPR_STRING_LITERAL("view_set.index%d.path");
static const xpr_tchar_t kIndexHashKey     [] = XPR_STRING_LITERAL("view_set.index%d.hash");
static const xpr_tchar_t kAllSubApplyKey   [] = XPR_STRING_LITERAL("view_set.index%d.all_sub_apply");

static const xpr_tchar_t kHashSection      [] = XPR_STRING_LITERAL("%s");
static const xpr_tchar_t kViewSetSection   [] = XPR_STRING_LITERAL("view_set");
static const xpr_tchar_t kStyleKey         [] = XPR_STRING_LITERAL("view_set.style");
static const xpr_tchar_t kSortFormatIdKey  [] = XPR_STRING_LITERAL("view_set.sort_format_id");
static const xpr_tchar_t kSortPropertyIdKey[] = XPR_STRING_LITERAL("view_set.sort_property_id");
static const xpr_tchar_t kSortAscendingKey [] = XPR_STRING_LITERAL("view_set.sort_ascending");
static const xpr_tchar_t kColumnCountKey   [] = XPR_STRING_LITERAL("view_set.column_count");
static const xpr_tchar_t kFormatIdKey      [] = XPR_STRING_LITERAL("view_set.column%d.format_id");
static const xpr_tchar_t kPropertyIdKey    [] = XPR_STRING_LITERAL("view_set.column%d.property_id");
static const xpr_tchar_t kWidthKey         [] = XPR_STRING_LITERAL("view_set.column%d.width");

FolderViewSet::FolderViewSet(void)
    : mViewStyle(LVS_REPORT)
    , mAllSubApply(XPR_FALSE)
    , mColumnCount(0)
    , mColumnItem(XPR_NULL)
{
    mColumnSortInfo.mFormatId   = GUID_NULL;
    mColumnSortInfo.mPropertyId = 0;
    mColumnSortInfo.mAscending  = XPR_TRUE;
}

FolderViewSet::~FolderViewSet(void)
{
    XPR_SAFE_DELETE_ARRAY(mColumnItem);
}

void FolderViewSet::clone(FolderViewSet &aFolderViewSet) const
{
    XPR_SAFE_DELETE_ARRAY(aFolderViewSet.mColumnItem);

    aFolderViewSet = *this;

    aFolderViewSet.mColumnItem = new ColumnItem[mColumnCount];
    memcpy(aFolderViewSet.mColumnItem, mColumnItem, sizeof(ColumnItem) * mColumnCount);
}

void FolderViewSet::getHashValue(xpr_tchar_t *aHashValue) const
{
    XPR_ASSERT(aHashValue != XPR_NULL);

    CMD5 sMd5;
    sMd5.init();

    sMd5.update((xpr_byte_t *)&mViewStyle,      sizeof(xpr_uint_t));
    sMd5.update((xpr_byte_t *)&mColumnSortInfo, sizeof(ColumnSortInfo));
    sMd5.update((xpr_byte_t *)&mAllSubApply,    sizeof(xpr_bool_t));
    sMd5.update((xpr_byte_t *)&mColumnCount,    sizeof(xpr_sint_t));
    sMd5.update((xpr_byte_t *)mColumnItem,      sizeof(ColumnItem) * mColumnCount);

    sMd5.finalize();

    const xpr_char_t *sHashValue = sMd5.hex_digest();
    if (XPR_IS_NOT_NULL(sHashValue))
    {
        xpr_size_t sInputBytes = strlen(sHashValue) * sizeof(xpr_char_t);
        xpr_size_t sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_MBS_TO_TCS(sHashValue, &sInputBytes, aHashValue, &sOutputBytes);
        aHashValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        XPR_SAFE_DELETE_ARRAY(sHashValue);
    }
    else
    {
        aHashValue[0] = 0;
    }
}

ViewSetMgr::ViewSetMgr(void)
    : mInstalledPath(XPR_TRUE)
{
}

ViewSetMgr::~ViewSetMgr(void)
{
    clear();
}

void ViewSetMgr::clear(void)
{
    FolderViewSet *sFolderViewSet;
    HashMap::iterator sIterator;

    XPR_STL_FOR_EACH(sIterator, mHashMap)
    {
        sFolderViewSet = sIterator->second;
        XPR_ASSERT(sFolderViewSet != XPR_NULL);

        XPR_SAFE_DELETE(sFolderViewSet);
    }

    mSubSet.clear();
    mIndexMap.clear();
    mHashMap.clear();
}

void ViewSetMgr::load(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeViewSet, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return;

    FolderViewSet *sFolderViewSet = XPR_NULL;
    IndexMap::const_iterator sIndexIterator;
    HashMap::const_iterator  sHashIterator;

    // load index
    loadIndex(sIniFile);

    // load view set
    XPR_STL_FOR_EACH(sIndexIterator, mIndexMap)
    {
        const std::tstring &sHashValue = sIndexIterator->second;

        if (XPR_IS_NULL(sFolderViewSet))
        {
            sFolderViewSet = new FolderViewSet;
            if (XPR_IS_NULL(sFolderViewSet))
                break;
        }

        sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator == mHashMap.end())
        {
            if (loadViewSet(sIniFile, sHashValue.c_str(), *sFolderViewSet) == XPR_TRUE)
            {
                mHashMap[sHashValue] = sFolderViewSet;
                sFolderViewSet = XPR_NULL;
            }
        }
    }

    if (XPR_IS_NOT_NULL(sFolderViewSet))
    {
        XPR_SAFE_DELETE(sFolderViewSet);
    }
}

xpr_bool_t ViewSetMgr::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeViewSet, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);

    if (XPR_IS_TRUE(mInstalledPath))
    {
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer view set file"));
    }
    else
    {
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer view set index file"));
    }

    HashMap::const_iterator sIterator;

    // save index
    saveIndex(sIniFile);

    // save view set
    XPR_STL_FOR_EACH(sIterator, mHashMap)
    {
        const std::tstring &sHashValue     = sIterator->first;
        FolderViewSet      *sFolderViewSet = sIterator->second;

        XPR_ASSERT(sFolderViewSet != XPR_NULL);

        saveViewSet(sIniFile, sHashValue.c_str(), *sFolderViewSet);
    }

    return sIniFile.writeFile(xpr::CharSetUtf16);
}

void ViewSetMgr::loadIndex(fxb::IniFileEx &aIniFile)
{
    xpr_sint_t             i;
    xpr_tchar_t            sKey[0xff];
    const xpr_tchar_t     *sPath;
    const xpr_tchar_t     *sHash;
    xpr_bool_t             sAllSubApply;
    fxb::IniFile::Section *sSection;

    sSection = aIniFile.findSection(kIndexSection);
    if (XPR_IS_NULL(sSection))
        return;

    for (i = 0; ; ++i)
    {
        _stprintf(sKey, kIndexPathKey, i + 1);

        sPath = aIniFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sPath))
            break;

        _stprintf(sKey, kIndexHashKey, i + 1);

        sHash = aIniFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sHash))
            break;

        _stprintf(sKey, kAllSubApplyKey, i + 1);
        sAllSubApply = aIniFile.getValueB(sSection, sKey, XPR_FALSE);

        mIndexMap[sPath] = sHash;

        if (XPR_IS_TRUE(sAllSubApply))
        {
            mSubSet.insert(sPath);
        }
    }
}

void ViewSetMgr::saveIndex(fxb::IniFileEx &aIniFile) const
{
    xpr_sint_t              i;
    xpr_tchar_t             sKey[0xff];
    fxb::IniFile::Section  *sSection;
    IndexMap::const_iterator sIterator;
    SubSet::const_iterator  sSubIterator;

    sSection = aIniFile.addSection(kIndexSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mIndexMap.begin();
    for (i = 0; sIterator != mIndexMap.end(); ++sIterator, ++i)
    {
        const std::tstring &sPath = sIterator->first;
        const std::tstring &sHash = sIterator->second.c_str();

        _stprintf(sKey, kIndexPathKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sPath);

        _stprintf(sKey, kIndexHashKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sHash);

        sSubIterator = mSubSet.find(sPath);
        if (sSubIterator == mSubSet.end())
            continue;

        _stprintf(sKey, kAllSubApplyKey, i + 1);
        aIniFile.setValueI(sSection, sKey, XPR_TRUE);
    }
}

xpr_bool_t ViewSetMgr::getViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFolderViewSet))
        return XPR_FALSE;

    xpr_bool_t sInstalledPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstalledPath) && aPath[1] == XPR_STRING_LITERAL(':'))
        sInstalledPath = XPR_FALSE;

    xpr_bool_t             sAllSubApply = XPR_FALSE;
    xpr_tchar_t            sPath[XPR_MAX_PATH * 2 + 1] = {0};
    SubSet::const_iterator sIterator;

    _tcscpy(sPath, aPath);

    XPR_STL_FOR_EACH(sIterator, mSubSet)
    {
        const std::tstring &sPathForAllSubApply = *sIterator;

        if (_tcsnicmp(sPathForAllSubApply.c_str(), sPath, sIterator->length()) == 0)
        {
            _tcscpy(sPath, sIterator->c_str());
            sAllSubApply = XPR_TRUE;
            break;
        }
    }

    if (XPR_IS_TRUE(sInstalledPath))
    {
        IndexMap::iterator sIndexIterator = mIndexMap.find(sPath);
        if (sIndexIterator == mIndexMap.end())
            return XPR_FALSE;

        const xpr_tchar_t *sHashValue = sIndexIterator->second.c_str();
        if (XPR_IS_NULL(sHashValue))
            return XPR_FALSE;

        HashMap::const_iterator sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator == mHashMap.end())
            return XPR_FALSE;

        const FolderViewSet *sFolderViewSet = sHashIterator->second;

        sFolderViewSet->clone(*aFolderViewSet);
    }
    else
    {
        xpr_tchar_t sIniPath[XPR_MAX_PATH + 1] = {0};
        _stprintf(sIniPath, XPR_STRING_LITERAL("%s\\")EACH_VIEW_SET_FILENAME, sPath);

        fxb::IniFileEx sIniFile(sIniPath);
        if (sIniFile.readFile() == XPR_FALSE)
            return XPR_FALSE;

        if (loadViewSet(sIniFile, kViewSetSection, *aFolderViewSet) == XPR_FALSE)
            return XPR_FALSE;
    }

    aFolderViewSet->mAllSubApply = sAllSubApply;

    return XPR_TRUE;
}

xpr_bool_t ViewSetMgr::loadViewSet(fxb::IniFileEx &aIniFile, const xpr_tchar_t *aSection, FolderViewSet &aFolderViewSet)
{
    XPR_ASSERT(aSection != XPR_NULL);

    xpr_sint_t         i;
    xpr_tchar_t        sKey[0xff];
    const xpr_tchar_t *sValue;
    xpr_tchar_t        sValueW[0xff];
    xpr_size_t         sInputBytes;
    xpr_size_t         sOutputBytes;
    HRESULT            sHResult;

    fxb::IniFile::Section *sSection = aIniFile.findSection(aSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    aFolderViewSet.mViewStyle   = aIniFile.getValueI(sSection, kStyleKey, 1);
    aFolderViewSet.mAllSubApply = XPR_FALSE;
    aFolderViewSet.mColumnCount = aIniFile.getValueI(sSection, kColumnCountKey, 0);
    aFolderViewSet.mColumnItem  = new ColumnItem[aFolderViewSet.mColumnCount];

    sValue = aIniFile.getValueS(sSection, kSortFormatIdKey, XPR_STRING_LITERAL(""));

    sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
    sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
    XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
    sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

    sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet.mColumnSortInfo.mFormatId);
    if (FAILED(sHResult))
        aFolderViewSet.mColumnSortInfo.mFormatId = GUID_NULL;

    aFolderViewSet.mColumnSortInfo.mPropertyId = aIniFile.getValueI(sSection, kSortPropertyIdKey,    0);
    aFolderViewSet.mColumnSortInfo.mAscending  = aIniFile.getValueI(sSection, kSortAscendingKey, 1);

    for (i = 0; i < aFolderViewSet.mColumnCount; ++i)
    {
        aFolderViewSet.mColumnItem[i].mFormatId   = GUID_NULL;
        aFolderViewSet.mColumnItem[i].mPropertyId = -1;
        aFolderViewSet.mColumnItem[i].mWidth      = -1;
    }

    for (i = 0; i < aFolderViewSet.mColumnCount; ++i)
    {
        _stprintf(sKey, kFormatIdKey, i + 1);
        sValue = aIniFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));

        sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
        sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
        sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

        sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet.mColumnItem[i].mFormatId);
        if (FAILED(sHResult))
            aFolderViewSet.mColumnItem[i].mFormatId = GUID_NULL;

        _stprintf(sKey, kPropertyIdKey, i + 1);
        aFolderViewSet.mColumnItem[i].mPropertyId = aIniFile.getValueI(sSection, sKey, -1);

        _stprintf(sKey, kWidthKey, i + 1);
        aFolderViewSet.mColumnItem[i].mWidth      = aIniFile.getValueI(sSection, sKey, -1);

        if (aFolderViewSet.mColumnItem[i].mWidth <= 0 || aFolderViewSet.mColumnItem[i].mWidth > MAX_COLUMN_SIZE)
            aFolderViewSet.mColumnItem[i].mWidth = DEF_COLUMN_SIZE;
    }

    return XPR_TRUE;
}

xpr_bool_t ViewSetMgr::setViewSet(const xpr_tchar_t *aPath, const FolderViewSet *aFolderViewSet)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFolderViewSet))
        return XPR_FALSE;

    xpr_bool_t sInstalledPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstalledPath) && aPath[1] == XPR_STRING_LITERAL(':'))
        sInstalledPath = XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sPath, aPath);

    if (XPR_IS_TRUE(aFolderViewSet->mAllSubApply))
    {
        mSubSet.insert(sPath);
    }
    else
    {
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        for (; sIterator != mSubSet.end(); ++sIterator)
        {
            if (_tcsnicmp(sIterator->c_str(), sPath, sIterator->length()) == 0)
            {
                _tcscpy(sPath, sIterator->c_str());
                break;
            }
        }
    }

    xpr_bool_t sResult = XPR_FALSE;

    if (XPR_IS_TRUE(sInstalledPath))
    {
        xpr_tchar_t sHashValue[0xff] = {0};
        aFolderViewSet->getHashValue(sHashValue);

        mIndexMap[sPath] = sHashValue;

        HashMap::iterator sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator != mHashMap.end())
        {
            FolderViewSet *sFolderViewSet = sHashIterator->second;
            XPR_ASSERT(sFolderViewSet != XPR_NULL);

            aFolderViewSet->clone(*sFolderViewSet);

            sResult = XPR_TRUE;
        }
        else
        {
            FolderViewSet *sFolderViewSet = new FolderViewSet;
            if (XPR_IS_NOT_NULL(sFolderViewSet))
            {
                aFolderViewSet->clone(*sFolderViewSet);

                mHashMap[sHashValue] = sFolderViewSet;

                sResult = XPR_TRUE;
            }
        }
    }
    else
    {
        xpr_tchar_t sIniPath[XPR_MAX_PATH + 1] = {0};
        _stprintf(sIniPath, XPR_STRING_LITERAL("%s\\")EACH_VIEW_SET_FILENAME, sPath);

        fxb::IniFileEx sIniFile(sIniPath);
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer view set file"));

        saveViewSet(sIniFile, kViewSetSection, *aFolderViewSet);

        sResult = sIniFile.writeFile(xpr::CharSetUtf16);

        if (XPR_IS_FALSE(sInstalledPath))
        {
            // set system and hidden file attributes
            DWORD sFileAttributes = ::GetFileAttributes(sIniPath);
            sFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
            sFileAttributes |= FILE_ATTRIBUTE_SYSTEM;
            ::SetFileAttributes(sIniPath, sFileAttributes);
        }
    }

    return sResult;
}

void ViewSetMgr::saveViewSet(fxb::IniFileEx &aIniFile, const xpr_tchar_t *aSection, const FolderViewSet &aFolderViewSet) const
{
    XPR_ASSERT(aSection != XPR_NULL);

    xpr_sint_t             i;
    xpr_tchar_t            sKey[0xff];
    xpr_tchar_t            sValue[0xff];
    xpr_size_t             sInputBytes;
    xpr_size_t             sOutputBytes;
    OLECHAR               *sFormatIdValue;
    fxb::IniFile::Section *sSection;

    sSection = aIniFile.addSection(aSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sFormatIdValue = XPR_NULL;
    ::StringFromIID(aFolderViewSet.mColumnSortInfo.mFormatId, &sFormatIdValue);

    sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
    sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
    XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
    sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

    aIniFile.setValueI(sSection, kStyleKey,          aFolderViewSet.mViewStyle);
    aIniFile.setValueS(sSection, kSortFormatIdKey,   sValue);
    aIniFile.setValueI(sSection, kSortPropertyIdKey, aFolderViewSet.mColumnSortInfo.mPropertyId);
    aIniFile.setValueI(sSection, kSortAscendingKey,  aFolderViewSet.mColumnSortInfo.mAscending);
    aIniFile.setValueI(sSection, kColumnCountKey,    aFolderViewSet.mColumnCount);

    COM_FREE(sFormatIdValue);

    for (i = 0; i < aFolderViewSet.mColumnCount; ++i)
    {
        if (aFolderViewSet.mColumnItem[i].mPropertyId == -1 || aFolderViewSet.mColumnItem[i].mWidth == -1)
            continue;

        sFormatIdValue = XPR_NULL;
        ::StringFromIID(aFolderViewSet.mColumnItem[i].mFormatId, &sFormatIdValue);

        sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
        sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
        sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        _stprintf(sKey, kFormatIdKey, i + 1);
        aIniFile.setValueS(sSection, sKey, sValue);

        _stprintf(sKey, kPropertyIdKey, i + 1);
        aIniFile.setValueI(sSection, sKey, aFolderViewSet.mColumnItem[i].mPropertyId);

        _stprintf(sKey, kWidthKey, i + 1);
        aIniFile.setValueI(sSection, sKey, aFolderViewSet.mColumnItem[i].mWidth);

        COM_FREE(sFormatIdValue);
    }
}

void ViewSetMgr::setSaveLocation(xpr_bool_t aInstalledPath)
{
    mInstalledPath = aInstalledPath;
}

void ViewSetMgr::verify(void)
{
    if (XPR_IS_FALSE(mInstalledPath))
        return;

    {
        xpr_tchar_t sDriveChar;
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        while (sIterator != mSubSet.end())
        {
            const std::tstring &sPath = *sIterator;

            if (sPath.length() < 2)
            {
                mSubSet.erase(sIterator++);
                continue;
            }

            if (sPath.at(1) == XPR_STRING_LITERAL(':'))
            {
                sDriveChar = sPath.at(0);
                if (XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sDriveChar, XPR_STRING_LITERAL('z')) ||
                    XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sDriveChar, XPR_STRING_LITERAL('Z')))
                {
                    if (fxb::IsExistFile(sPath) == XPR_FALSE)
                    {
                        mSubSet.erase(sIterator++);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    {
        xpr_tchar_t sDriveChar;
        IndexMap::iterator sIterator;

        sIterator = mIndexMap.begin();
        while (sIterator != mIndexMap.end())
        {
            const std::tstring &sPath = sIterator->first;

            if (sPath.length() < 2)
            {
                mIndexMap.erase(sIterator++);
                continue;
            }

            if (sPath.at(1) == XPR_STRING_LITERAL(':'))
            {
                sDriveChar = sPath.at(0);
                if (XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sDriveChar, XPR_STRING_LITERAL('z')) ||
                    XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sDriveChar, XPR_STRING_LITERAL('Z')))
                {
                    if (fxb::IsExistFile(sPath) == XPR_FALSE)
                    {
                        mIndexMap.erase(sIterator++);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    typedef std::tr1::unordered_set<std::tstring> HashSet;
    HashSet sHashSet;

    {
        HashMap::iterator sIterator;

        XPR_STL_FOR_EACH(sIterator, mHashMap)
        {
            const std::tstring &sHash = sIterator->first;

            sHashSet.insert(sHash);
        }
    }

    {
        IndexMap::iterator sIterator;

        sIterator = mIndexMap.begin();
        while (sIterator != mIndexMap.end())
        {
            const std::tstring &sHash = sIterator->second;

            if (sHashSet.find(sHash) == sHashSet.end())
            {
                mIndexMap.erase(sIterator++);
                continue;
            }

            sIterator++;
        }
    }
}
