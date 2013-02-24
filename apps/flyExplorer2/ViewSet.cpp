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

static const xpr_tchar_t kViewSetSection   [] = XPR_STRING_LITERAL("view_set");
static const xpr_tchar_t kVersionKey       [] = XPR_STRING_LITERAL("view_set.version");
static const xpr_tchar_t kKeyKey           [] = XPR_STRING_LITERAL("view_set.key");
static const xpr_tchar_t kAllSubApplyKey   [] = XPR_STRING_LITERAL("view_set.all_sub_aplly");
static const xpr_tchar_t kStyleKey         [] = XPR_STRING_LITERAL("view_set.style");
static const xpr_tchar_t kSortIdKey        [] = XPR_STRING_LITERAL("view_set.sort_id");
static const xpr_tchar_t kSortFormatIdKey  [] = XPR_STRING_LITERAL("view_set.sort_format_id");
static const xpr_tchar_t kSortPropertyIdKey[] = XPR_STRING_LITERAL("view_set.sort_property_id");
static const xpr_tchar_t kSortAscendingKey [] = XPR_STRING_LITERAL("view_set.sort_ascending");
static const xpr_tchar_t kColumnKey        [] = XPR_STRING_LITERAL("view_set.column");
static const xpr_tchar_t kColumnCountKey   [] = XPR_STRING_LITERAL("view_set.column_count");
static const xpr_tchar_t kWidthKey         [] = XPR_STRING_LITERAL("view_set.width");
static const xpr_tchar_t kFormatIdKey      [] = XPR_STRING_LITERAL("view_set.format_id");
static const xpr_tchar_t kPropertyIdKey    [] = XPR_STRING_LITERAL("view_set.property_id");

ViewSet::ViewSet(void)
    : mInstPath(XPR_TRUE)
{
}

ViewSet::~ViewSet(void)
{
    mSubSet.clear();
    mKeyMap.clear();
}

void ViewSet::readIndex(void)
{
    mSubSet.clear();
    mKeyMap.clear();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeViewSet, sPath, XPR_MAX_PATH, XPR_STRING_LITERAL("index"));

    fxb::IniFileEx sIniFile(sPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return;

    const xpr_tchar_t     *sSectionName;
    const xpr_tchar_t     *sHash;
    xpr_bool_t             sAllSubApply;
    fxb::IniFile::Section *sSection;
    fxb::IniFile::SectionIterator sSectionIterator;

    sSectionIterator = sIniFile.beginSection();
    for (; sSectionIterator != sIniFile.endSection(); ++sSectionIterator)
    {
        sSection = *sSectionIterator;
        XPR_ASSERT(sSection != XPR_NULL);

        sSectionName = sIniFile.getSectionName(sSection);

        sHash        = sIniFile.getValueS(sSection, kKeyKey,         XPR_NULL);
        sAllSubApply = sIniFile.getValueB(sSection, kAllSubApplyKey, XPR_FALSE);

        if (XPR_IS_NULL(sHash))
            continue;

        mKeyMap[sSectionName] = sHash;

        if (XPR_IS_TRUE(sAllSubApply))
            mSubSet.insert(sSectionName);
    }
}

void ViewSet::saveIndex(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSaveDir(CfgPath::TypeViewSet, sPath, XPR_MAX_PATH);
    _tcscat(sPath, XPR_STRING_LITERAL("\\index.ini"));

    fxb::IniFileEx sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer view set index file"));

    KeyMap::iterator       sIterator;
    SubSet::iterator       sSubIterator;
    const xpr_tchar_t     *sSubPath;
    const xpr_tchar_t     *sHash;
    fxb::IniFile::Section *sSection;

    sIterator = mKeyMap.begin();
    for (; sIterator != mKeyMap.end(); ++sIterator)
    {
        sSubPath = sIterator->first.c_str();
        sHash    = sIterator->second.c_str();

        sSection = sIniFile.addSection(sSubPath);
        XPR_ASSERT(sSection != XPR_NULL);

        sIniFile.setValueS(sSection, kKeyKey, sHash);

        sSubIterator = mSubSet.find(sSubPath);
        if (sSubIterator == mSubSet.end())
            continue;

        sIniFile.setValueI(sSection, kAllSubApplyKey, XPR_TRUE);
    }

    sIniFile.writeFile(xpr::CharSetUtf16);
}

xpr_bool_t ViewSet::getViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFolderViewSet))
        return XPR_FALSE;

    xpr_bool_t sInstPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstPath) && aPath[1] == XPR_STRING_LITERAL(':'))
        sInstPath = XPR_FALSE;

    readIndex();

    xpr_bool_t sAllSubApply = XPR_FALSE;
    static xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sPath, aPath);

    {
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        for (; sIterator != mSubSet.end(); ++sIterator)
        {
            if (_tcsnicmp(sIterator->c_str(), sPath, sIterator->length()) == 0)
            {
                _tcscpy(sPath, sIterator->c_str());
                sAllSubApply = XPR_TRUE;
                break;
            }
        }
    }

    const xpr_tchar_t *sCrcVal;

    if (XPR_IS_TRUE(sInstPath))
    {
        KeyMap::iterator sIterator;

        sIterator = mKeyMap.find(sPath);
        if (sIterator == mKeyMap.end())
            return XPR_FALSE;

        sCrcVal = sIterator->second.c_str();
    }

    {
        xpr_tchar_t sIniPath[XPR_MAX_PATH + 1] = {0};
        if (XPR_IS_TRUE(sInstPath))
        {
            CfgPath::instance().getLoadPath(CfgPath::TypeViewSet, sIniPath, XPR_MAX_PATH, sCrcVal);
        }
        else
        {
            _stprintf(sIniPath, XPR_STRING_LITERAL("%s\\viewset.ini"), sPath);
        }

        fxb::IniFileEx sIniFile(sIniPath);
        if (sIniFile.readFile() == XPR_FALSE)
            return XPR_FALSE;

        fxb::IniFile::Section *sSection = sIniFile.findSection(kViewSetSection);
        XPR_ASSERT(sSection != XPR_NULL);

        xpr_sint_t         i;
        xpr_tchar_t        sSectionName[0xff];
        const xpr_tchar_t *sValue;
        xpr_tchar_t        sValueW[0xff];
        xpr_size_t         sInputBytes;
        xpr_size_t         sOutputBytes;
        HRESULT            sHResult;

        aFolderViewSet->mViewStyle   = sIniFile.getValueI(sSection, kStyleKey, 1);
        aFolderViewSet->mAllSubApply = sAllSubApply;
        aFolderViewSet->mColumnCount = sIniFile.getValueI(sSection, kColumnCountKey, 0);
        aFolderViewSet->mColumnItem  = new ColumnItem[aFolderViewSet->mColumnCount];

        sValue = sIniFile.getValueS(sSection, kSortFormatIdKey, XPR_STRING_LITERAL(""));

        sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
        sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
        sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

        sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet->mColumnSortInfo.mFormatId);
        if (FAILED(sHResult))
            aFolderViewSet->mColumnSortInfo.mFormatId = GUID_NULL;

        aFolderViewSet->mColumnSortInfo.mPropertyId = sIniFile.getValueI(sSection, kSortPropertyIdKey,    0);
        aFolderViewSet->mColumnSortInfo.mAscending  = sIniFile.getValueI(sSection, kSortAscendingKey, 1);

        for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
        {
            aFolderViewSet->mColumnItem[i].mFormatId   = GUID_NULL;
            aFolderViewSet->mColumnItem[i].mPropertyId = -1;
            aFolderViewSet->mColumnItem[i].mWidth      = -1;
        }

        for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
        {
            _stprintf(sSectionName, XPR_STRING_LITERAL("%s%d"), kColumnKey, i+1);

            sSection = sIniFile.findSection(sSectionName);
            XPR_ASSERT(sSection != XPR_NULL);

            sValue = sIniFile.getValueS(sSection, kFormatIdKey, XPR_STRING_LITERAL(""));

            sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
            sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
            sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet->mColumnItem[i].mFormatId);
            if (FAILED(sHResult))
                aFolderViewSet->mColumnItem[i].mFormatId = GUID_NULL;

            aFolderViewSet->mColumnItem[i].mPropertyId = sIniFile.getValueI(sSection, kPropertyIdKey, -1);
            aFolderViewSet->mColumnItem[i].mWidth      = sIniFile.getValueI(sSection, kWidthKey,      -1);

            if (aFolderViewSet->mColumnItem[i].mWidth <= 0 || aFolderViewSet->mColumnItem[i].mWidth > MAX_COLUMN_SIZE)
                aFolderViewSet->mColumnItem[i].mWidth = DEF_COLUMN_SIZE;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ViewSet::setViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFolderViewSet))
        return XPR_FALSE;

    xpr_bool_t sInstPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstPath) && aPath[1] == XPR_STRING_LITERAL(':'))
        sInstPath = XPR_FALSE;

    readIndex();

    static xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sPath, aPath);

    {
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
    }

    xpr_tchar_t sCrcVal[0xff] = {0};

    if (XPR_IS_TRUE(sInstPath))
    {
        xpr_byte_t *sData;

        CMD5 sMd5;
        sMd5.init();

        sData = (xpr_byte_t *)aFolderViewSet;
        sMd5.update(sData, sizeof(DWORD));          sData += sizeof(DWORD);
        sMd5.update(sData, sizeof(ColumnSortInfo)); sData += sizeof(ColumnSortInfo);
        sMd5.update(sData, sizeof(xpr_bool_t));     sData += sizeof(xpr_bool_t);
        sMd5.update(sData, sizeof(xpr_sint_t));     sData += sizeof(xpr_sint_t);

        sData = (xpr_byte_t *)aFolderViewSet->mColumnItem;
        sMd5.update(sData, sizeof(ColumnItem) * aFolderViewSet->mColumnCount);

        sMd5.finalize();

        const xpr_char_t *sMd5CrcVal = sMd5.hex_digest();
        if (XPR_IS_NOT_NULL(sMd5CrcVal))
        {
            xpr_size_t sInputBytes = strlen(sMd5CrcVal) * sizeof(xpr_char_t);
            xpr_size_t sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sMd5CrcVal, &sInputBytes, sCrcVal, &sOutputBytes);
            sCrcVal[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
        }

        XPR_SAFE_DELETE_ARRAY(sMd5CrcVal);
    }

    xpr_bool_t sExistKey = XPR_FALSE;

    if (XPR_IS_TRUE(sInstPath))
    {
        sExistKey = mKeyMap.find(sPath) != mKeyMap.end();

        mKeyMap[sPath] = sCrcVal;
    }

    {
        xpr_tchar_t sIniPath[XPR_MAX_PATH + 1] = {0};
        if (XPR_IS_TRUE(sInstPath))
        {
            CfgPath::instance().getSaveDir(CfgPath::TypeViewSet, sIniPath, XPR_MAX_PATH);
            _stprintf(sIniPath+_tcslen(sIniPath), XPR_STRING_LITERAL("\\%s.ini"), sCrcVal);
        }
        else
        {
            _stprintf(sIniPath, XPR_STRING_LITERAL("%s\\viewset.ini"), sPath);
        }

        fxb::IniFileEx sIniFile(sIniPath);
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer view set file"));

        xpr_sint_t             i;
        xpr_tchar_t            sSectionName[0xff];
        xpr_tchar_t            sValue[0xff];
        xpr_size_t             sInputBytes;
        xpr_size_t             sOutputBytes;
        OLECHAR               *sFormatIdValue;
        fxb::IniFile::Section *sSection;

        sSection = sIniFile.addSection(kViewSetSection);
        XPR_ASSERT(sSection != XPR_NULL);

        sFormatIdValue = XPR_NULL;
        ::StringFromIID(aFolderViewSet->mColumnSortInfo.mFormatId, &sFormatIdValue);

        sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
        sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
        sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        sIniFile.setValueI(sSection, kVersionKey,        1);
        sIniFile.setValueI(sSection, kStyleKey,          aFolderViewSet->mViewStyle);
        sIniFile.setValueS(sSection, kSortFormatIdKey,   sValue);
        sIniFile.setValueI(sSection, kSortPropertyIdKey, aFolderViewSet->mColumnSortInfo.mPropertyId);
        sIniFile.setValueI(sSection, kSortAscendingKey,  aFolderViewSet->mColumnSortInfo.mAscending);
        sIniFile.setValueI(sSection, kColumnCountKey,    aFolderViewSet->mColumnCount);

        COM_FREE(sFormatIdValue);

        for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
        {
            if (aFolderViewSet->mColumnItem[i].mPropertyId == -1 || aFolderViewSet->mColumnItem[i].mWidth == -1)
                continue;

            _stprintf(sSectionName, XPR_STRING_LITERAL("%s%d"), kColumnKey, i+1);

            sSection = sIniFile.addSection(sSectionName);
            XPR_ASSERT(sSection != XPR_NULL);

            sFormatIdValue = XPR_NULL;
            ::StringFromIID(aFolderViewSet->mColumnItem[i].mFormatId, &sFormatIdValue);

            sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
            sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sIniFile.setValueS(sSection, kFormatIdKey,   sValue);
            sIniFile.setValueI(sSection, kPropertyIdKey, aFolderViewSet->mColumnItem[i].mPropertyId);
            sIniFile.setValueI(sSection, kWidthKey,      aFolderViewSet->mColumnItem[i].mWidth);

            COM_FREE(sFormatIdValue);
        }

        sIniFile.writeFile(xpr::CharSetUtf16);

        if (XPR_IS_FALSE(sInstPath))
        {
            // set system and hidden file attributes
            DWORD sFileAttributes = ::GetFileAttributes(sIniPath);
            sFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
            sFileAttributes |= FILE_ATTRIBUTE_SYSTEM;
            ::SetFileAttributes(sIniPath, sFileAttributes);
        }
    }

    saveIndex();

    return XPR_TRUE;
}

void ViewSet::setCfgViewSet(xpr_bool_t aInstPath)
{
    mInstPath = aInstPath;
}

void ViewSet::verify(void)
{
    if (XPR_IS_FALSE(mInstPath))
        return;

    readIndex();

    {
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        while (sIterator != mSubSet.end())
        {
            if (sIterator->length() >= 2)
            {
                if (sIterator->at(0) != XPR_STRING_LITERAL(':'))
                {
                    if (fxb::IsExistFile(sIterator->c_str()) == XPR_FALSE)
                    {
                        sIterator = mSubSet.erase(sIterator);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    {
        KeyMap::iterator sIterator;

        sIterator = mKeyMap.begin();
        while (sIterator != mKeyMap.end())
        {
            if (sIterator->first.length() >= 2)
            {
                if (sIterator->first.at(0) != XPR_STRING_LITERAL(':'))
                {
                    if (fxb::IsExistFile(sIterator->first.c_str()) == XPR_FALSE)
                    {
                        sIterator = mKeyMap.erase(sIterator);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    {
        typedef std::set<std::tstring> KeySet;
        KeySet sKeySet;

        {
            KeyMap::iterator sIterator = mKeyMap.begin();
            for (; sIterator != mKeyMap.end(); ++sIterator)
            {
                sKeySet.insert(sIterator->second.c_str());
            }
        }

        typedef std::deque<std::tstring> DelPathDeque;
        DelPathDeque sDelPathDeque;

        xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
        CfgPath::instance().getLoadDir(CfgPath::TypeViewSet, sDir, XPR_MAX_PATH);

        HANDLE sFindFile;
        xpr_tchar_t *sExt;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        xpr_tchar_t sFileName[XPR_MAX_PATH + 1];
        WIN32_FIND_DATA sWin32FindData;

        xpr_tchar_t sFindPath[XPR_MAX_PATH + 1];
        _stprintf(sFindPath, XPR_STRING_LITERAL("%s\\*.*"), sDir);

        sFindFile = ::FindFirstFile(sFindPath, &sWin32FindData);
        if (sFindFile != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (_tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
                    continue;

                if (_tcsicmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("index.ini")) == 0)
                    continue;

                _tcscpy(sFileName, sWin32FindData.cFileName);

                sExt = (xpr_tchar_t *)fxb::GetFileExt(sFileName);
                if (XPR_IS_NOT_NULL(sExt))
                {
                    sExt[0] = XPR_STRING_LITERAL('\0');

                    if (sKeySet.find(sFileName) == sKeySet.end())
                    {
                        _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s"), sDir, sWin32FindData.cFileName);
                        sDelPathDeque.push_back(sPath);
                    }
                }
            }
            while (::FindNextFile(sFindFile, &sWin32FindData) == XPR_TRUE);

            ::FindClose(sFindFile);
            sFindFile = XPR_NULL;
        }

        {
            DelPathDeque::iterator sIterator;

            sIterator = sDelPathDeque.begin();
            for (; sIterator != sDelPathDeque.end(); ++sIterator)
                ::DeleteFile(sIterator->c_str());
        }

        sKeySet.clear();
        sDelPathDeque.clear();
    }

    saveIndex();
}

void ViewSet::clear(void)
{
    if (XPR_IS_FALSE(mInstPath))
        return;

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadDir(CfgPath::TypeViewSet, sDir, XPR_MAX_PATH);
    _tcscat(sDir, XPR_STRING_LITERAL("\\*.*"));

    sDir[_tcslen(sDir) + 1] = '\0';
    fxb::SHSilentDeleteFile(sDir);
}

ViewSetMgr::ViewSetMgr(void)
{
}

ViewSetMgr::~ViewSetMgr(void)
{
}
