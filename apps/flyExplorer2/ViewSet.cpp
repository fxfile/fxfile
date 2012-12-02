//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ViewSet.h"

#include "fxb/fxb_ini_file.h"
#include "fxb/fxb_md5.h"
#include "CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kViewSetKey[]          = XPR_STRING_LITERAL("ViewSet");
static const xpr_tchar_t kVersionEntry[]        = XPR_STRING_LITERAL("Version");
static const xpr_tchar_t kKeyEntry[]            = XPR_STRING_LITERAL("Key");
static const xpr_tchar_t kAllSubApplyEntry[]    = XPR_STRING_LITERAL("All Sub Apply");
static const xpr_tchar_t kStyleEntry[]          = XPR_STRING_LITERAL("Style");
static const xpr_tchar_t kSortIdEntry[]         = XPR_STRING_LITERAL("SortId");
static const xpr_tchar_t kSortFormatIdEntry[]   = XPR_STRING_LITERAL("Sort Format Id");
static const xpr_tchar_t kSortPropertyIdEntry[] = XPR_STRING_LITERAL("Sort Property Id");
static const xpr_tchar_t kSortAscendingEntry[]  = XPR_STRING_LITERAL("Ascending");
static const xpr_tchar_t kColumnEntry[]         = XPR_STRING_LITERAL("Column");
static const xpr_tchar_t kColumnCountEntry[]    = XPR_STRING_LITERAL("Column Count");
static const xpr_tchar_t kWidthEntry[]          = XPR_STRING_LITERAL("Width");
static const xpr_tchar_t kFormatIdEntry[]       = XPR_STRING_LITERAL("Format Id");
static const xpr_tchar_t kPropertyIdEntry[]     = XPR_STRING_LITERAL("Property Id");

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

    fxb::IniFile sIniFile(sPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return;

    xpr_size_t i;
    xpr_size_t sKeyCount;
    const xpr_tchar_t *sKey;

    const xpr_tchar_t *sHash;
    xpr_bool_t sAllSubApply;

    sKeyCount = sIniFile.getKeyCount();
    for (i = 0; i < sKeyCount; ++i)
    {
        sKey = sIniFile.getKeyName(i);
        if (XPR_IS_NULL(sKey))
            continue;

        sHash        = sIniFile.getValueS(sKey, kKeyEntry);
        sAllSubApply = sIniFile.getValueI(sKey, kAllSubApplyEntry);

        if (XPR_IS_NULL(sHash))
            continue;

        mKeyMap[sKey] = sHash;

        if (XPR_IS_TRUE(sAllSubApply))
            mSubSet.insert(sKey);
    }
}

void ViewSet::saveIndex(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSaveDir(CfgPath::TypeViewSet, sPath, XPR_MAX_PATH);
    _tcscat(sPath, XPR_STRING_LITERAL("\\index.ini"));

    fxb::IniFile sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer ViewSet Index File"));

    KeyMap::iterator sIterator;
    SubSet::iterator sSubIterator;
    const xpr_tchar_t *sSubPath;
    const xpr_tchar_t *sHash;

    sIterator = mKeyMap.begin();
    for (; sIterator != mKeyMap.end(); ++sIterator)
    {
        sSubPath = sIterator->first.c_str();
        sHash    = sIterator->second.c_str();

        sIniFile.setValueS(sSubPath, kKeyEntry, sHash);

        sSubIterator = mSubSet.find(sSubPath);
        if (sSubIterator == mSubSet.end())
            continue;

        sIniFile.setValueI(sSubPath, kAllSubApplyEntry, XPR_TRUE);
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

        fxb::IniFile sIniFile(sIniPath);
        if (sIniFile.readFile() == XPR_FALSE)
            return XPR_FALSE;

        xpr_sint_t sVersion = sIniFile.getValueI(kViewSetKey, kVersionEntry, 0);
        if (sVersion == 0)
        {
            xpr_sint_t i;
            xpr_sint_t sWidth;
            xpr_sint_t sPropertyId;
            xpr_tchar_t sEntry[0xff];
            const xpr_tchar_t *sValue;

            aFolderViewSet->mViewStyle   = sIniFile.getValueI(kViewSetKey, kStyleEntry, 1);
            aFolderViewSet->mAllSubApply = sAllSubApply;
            aFolderViewSet->mColumnCount = MAX_COLUMN;
            aFolderViewSet->mColumnItem  = new ColumnItem[aFolderViewSet->mColumnCount];

            aFolderViewSet->mColumnSortInfo.mFormatId   = GUID_NULL;
            aFolderViewSet->mColumnSortInfo.mPropertyId = sIniFile.getValueI(kViewSetKey, kSortIdEntry,        0);
            aFolderViewSet->mColumnSortInfo.mAscending  = sIniFile.getValueI(kViewSetKey, kSortAscendingEntry, 1);

            for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
            {
                aFolderViewSet->mColumnItem[i].mFormatId   = GUID_NULL;
                aFolderViewSet->mColumnItem[i].mPropertyId = -1;
                aFolderViewSet->mColumnItem[i].mWidth      = -1;
            }

            xpr_sint_t sIndex = 0;
            for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
            {
                _stprintf(sEntry, XPR_STRING_LITERAL("%s%02d"), kColumnEntry, i+1);

                sValue = sIniFile.getValueS(kViewSetKey, sEntry);
                if (XPR_IS_NULL(sValue))
                    continue;

                sWidth      = -1;
                sPropertyId = -1;

                _stscanf(sValue, XPR_STRING_LITERAL("%d|%d"), &sWidth, &sPropertyId);

                if (sPropertyId < MIN_COLUMN_ID || sPropertyId > MAX_COLUMN_ID)
                    continue;

                if (sWidth <= 0 || sWidth > MAX_COLUMN_SIZE)
                    sWidth = DEF_COLUMN_SIZE;

                aFolderViewSet->mColumnItem[sIndex].mWidth      = sWidth;
                aFolderViewSet->mColumnItem[sIndex].mPropertyId = sPropertyId;
                sIndex++;
            }
        }
        else if (sVersion == 1)
        {
            xpr_sint_t i;
            xpr_tchar_t sKey[0xff];
            const xpr_tchar_t *sValue;
            xpr_tchar_t sValueW[0xff];
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            HRESULT sHResult;

            aFolderViewSet->mViewStyle   = sIniFile.getValueI(kViewSetKey, kStyleEntry, 1);
            aFolderViewSet->mAllSubApply = sAllSubApply;
            aFolderViewSet->mColumnCount = sIniFile.getValueI(kViewSetKey, kColumnCountEntry);
            aFolderViewSet->mColumnItem  = new ColumnItem[aFolderViewSet->mColumnCount];

            sValue = sIniFile.getValueS(kViewSetKey, kSortFormatIdEntry, XPR_STRING_LITERAL(""));

            sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
            sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
            sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet->mColumnSortInfo.mFormatId);
            if (FAILED(sHResult))
                aFolderViewSet->mColumnSortInfo.mFormatId = GUID_NULL;

            aFolderViewSet->mColumnSortInfo.mPropertyId = sIniFile.getValueI(kViewSetKey, kSortPropertyIdEntry,    0);
            aFolderViewSet->mColumnSortInfo.mAscending  = sIniFile.getValueI(kViewSetKey, kSortAscendingEntry, 1);

            for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
            {
                aFolderViewSet->mColumnItem[i].mFormatId   = GUID_NULL;
                aFolderViewSet->mColumnItem[i].mPropertyId = -1;
                aFolderViewSet->mColumnItem[i].mWidth      = -1;
            }

            for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
            {
                _stprintf(sKey, XPR_STRING_LITERAL("%s%d"), kColumnEntry, i+1);

                sValue = sIniFile.getValueS(sKey, kFormatIdEntry, XPR_STRING_LITERAL(""));

                sInputBytes = _tcslen(sValue) * sizeof(xpr_tchar_t);
                sOutputBytes = 0xfe * sizeof(xpr_wchar_t);
                XPR_TCS_TO_UTF16(sValue, &sInputBytes, sValueW, &sOutputBytes);
                sValueW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

                sHResult = ::IIDFromString((LPOLESTR)sValueW, &aFolderViewSet->mColumnItem[i].mFormatId);
                if (FAILED(sHResult))
                    aFolderViewSet->mColumnItem[i].mFormatId = GUID_NULL;

                aFolderViewSet->mColumnItem[i].mPropertyId = sIniFile.getValueI(sKey, kPropertyIdEntry, -1);
                aFolderViewSet->mColumnItem[i].mWidth      = sIniFile.getValueI(sKey, kWidthEntry,      -1);

                if (aFolderViewSet->mColumnItem[i].mWidth <= 0 || aFolderViewSet->mColumnItem[i].mWidth > MAX_COLUMN_SIZE)
                    aFolderViewSet->mColumnItem[i].mWidth = DEF_COLUMN_SIZE;
            }
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

        fxb::IniFile sIniFile(sIniPath);
        sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer ViewSet File"));

        xpr_sint_t i;
        xpr_tchar_t sKey[0xff];
        xpr_tchar_t sValue[0xff];
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;
        OLECHAR *sFormatIdValue;

        sFormatIdValue = XPR_NULL;
        ::StringFromIID(aFolderViewSet->mColumnSortInfo.mFormatId, &sFormatIdValue);

        sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
        sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
        sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        sIniFile.setValueI(kViewSetKey, kVersionEntry,        1);
        sIniFile.setValueI(kViewSetKey, kStyleEntry,          aFolderViewSet->mViewStyle);
        sIniFile.setValueS(kViewSetKey, kSortFormatIdEntry,   sValue);
        sIniFile.setValueI(kViewSetKey, kSortPropertyIdEntry, aFolderViewSet->mColumnSortInfo.mPropertyId);
        sIniFile.setValueI(kViewSetKey, kSortAscendingEntry,  aFolderViewSet->mColumnSortInfo.mAscending);
        sIniFile.setValueI(kViewSetKey, kColumnCountEntry,    aFolderViewSet->mColumnCount);

        ::CoTaskMemFree(sFormatIdValue);

        for (i = 0; i < aFolderViewSet->mColumnCount; ++i)
        {
            if (aFolderViewSet->mColumnItem[i].mPropertyId == -1 || aFolderViewSet->mColumnItem[i].mWidth == -1)
                continue;

            _stprintf(sKey, XPR_STRING_LITERAL("%s%d"), kColumnEntry, i+1);

            sFormatIdValue = XPR_NULL;
            ::StringFromIID(aFolderViewSet->mColumnItem[i].mFormatId, &sFormatIdValue);

            sInputBytes = wcslen(sFormatIdValue) * sizeof(xpr_wchar_t);
            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_TCS_TO_UTF16(sFormatIdValue, &sInputBytes, sValue, &sOutputBytes);
            sValue[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sIniFile.setValueS(sKey, kFormatIdEntry,   sValue);
            sIniFile.setValueI(sKey, kPropertyIdEntry, aFolderViewSet->mColumnItem[i].mPropertyId);
            sIniFile.setValueI(sKey, kWidthEntry,      aFolderViewSet->mColumnItem[i].mWidth);

            ::CoTaskMemFree(sFormatIdValue);
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
