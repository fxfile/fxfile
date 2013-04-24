//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_shell_new.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
ShellNew::ShellNew(void)
    : mBaseId(0)
    , mPrefix(XPR_STRING_LITERAL("New "))
{
}

ShellNew::~ShellNew(void)
{
    clear();
}

void ShellNew::setBaseCommandId(xpr_uint_t aId)
{
    mBaseId = aId;
}

void ShellNew::setText(const xpr_tchar_t *aPrefix, const xpr_tchar_t *aSuffix)
{
    if (XPR_IS_NOT_NULL(aPrefix)) mPrefix = aPrefix;
    if (XPR_IS_NOT_NULL(aSuffix)) mSuffix = aSuffix;
}

void ShellNew::clear(void)
{
    Item *sItem;
    ShellNewDeque::iterator sIterator;

    sIterator = mShellNewDeque.begin();
    for (; sIterator != mShellNewDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        XPR_SAFE_DELETE(sItem);
    }

    mShellNewMap.clear();
    mShellNewDeque.clear();
}

xpr_size_t ShellNew::getCount(void)
{
    return mShellNewDeque.size();
}

ShellNew::Item *ShellNew::getItem(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mShellNewDeque))
        return XPR_NULL;

    return mShellNewDeque[aIndex];
}

void ShellNew::getRegShellNew(void)
{
    clear();

    struct
    {
        const xpr_tchar_t *mValue;
        Type               mType;
        DWORD              mRegValueType;
    } sShellNewData[] = {
        { XPR_STRING_LITERAL("NullFile"), TypeNullFile, REG_SZ        },
        { XPR_STRING_LITERAL("FileName"), TypeFileName, REG_SZ        },
        { XPR_STRING_LITERAL("Data"),     TypeData,     REG_BINARY    },
        { XPR_STRING_LITERAL("Command"),  TypeCommand,  REG_EXPAND_SZ },
        { XPR_NULL,                       TypeNone,     REG_NONE      },
    };

    Item *sItem;
    xpr_uint_t sId = mBaseId;

    DWORD sValueLen;
    xpr_tchar_t sExt[XPR_MAX_PATH + 1];
    xpr_tchar_t sBaseSubKey[0xff];
    xpr_tchar_t sSubKey[0xff];
    xpr_tchar_t sValue[0xff];
    xpr_tchar_t sLinkKey[0xff];

    HKEY sRegKey;
    HKEY sRegSubKey;
    xpr_slong_t sReturn;
    sReturn = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, XPR_NULL, 0L, KEY_READ, &sRegKey);
    if (sReturn != ERROR_SUCCESS)
        return;

    DWORD sKeyCount;
    DWORD sValueCount;

    sKeyCount = 0;
    sReturn = ::RegQueryInfoKey(sRegKey, 0, 0, 0, &sKeyCount, 0, 0, 0, 0, 0, 0, 0);
    if (sReturn == ERROR_SUCCESS)
    {
        DWORD i;
        for (i = 0; i < sKeyCount; ++i)
        {
            sSubKey[0] = XPR_STRING_LITERAL('\0');
            sReturn = ::RegEnumKey(sRegKey, i, sSubKey, 0xfe);
            if (sReturn != ERROR_SUCCESS && sReturn != ERROR_INSUFFICIENT_BUFFER)
                break;

            if (sSubKey[0] != XPR_STRING_LITERAL('.'))
                continue;

            if (_tcsicmp(sSubKey, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sSubKey, XPR_STRING_LITERAL(".url")) == 0)
                continue;

            _tcscpy(sBaseSubKey, sSubKey);
            _tcscpy(sExt, sSubKey);

            xpr_bool_t sContinue = XPR_TRUE;

            xpr_sint_t k;
            for (k = 0; k < 10; ++k)
            {
                sRegSubKey = XPR_NULL;
                sReturn = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, sBaseSubKey, 0L, KEY_READ, &sRegSubKey);
                if (sReturn != ERROR_SUCCESS)
                    break;

                sLinkKey[0] = XPR_STRING_LITERAL('\0');

                sValueLen = XPR_MAX_PATH;
                sValue[0] = XPR_STRING_LITERAL('\0');
                sReturn = ::RegQueryValueEx(sRegSubKey, XPR_STRING_LITERAL(""), XPR_NULL, XPR_NULL, (xpr_byte_t *)sValue, &sValueLen);
                if (sReturn == ERROR_SUCCESS)
                {
                    _tcscpy(sLinkKey, sValue);
                }

                ::RegCloseKey(sRegSubKey);

                _tcscpy(sSubKey, sBaseSubKey);
                _tcscat(sSubKey, XPR_STRING_LITERAL("\\ShellNew"));

                sReturn = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, sSubKey, 0L, KEY_READ, &sRegSubKey);
                if (sReturn == ERROR_SUCCESS)
                {
                    sValueCount = 0;
                    ::RegQueryInfoKey(sRegSubKey, 0, 0, 0, 0, 0, 0, &sValueCount, 0, 0, 0, 0);

                    if (sValueCount > 0)
                    {
                        sContinue = XPR_FALSE;
                        break;
                    }
                }

                ::RegCloseKey(sRegSubKey);

                _tcscpy(sSubKey, sBaseSubKey);
                _tcscat(sSubKey, XPR_STRING_LITERAL("\\"));
                _tcscat(sSubKey, sLinkKey);
                _tcscat(sSubKey, XPR_STRING_LITERAL("\\ShellNew"));

                sReturn = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, sSubKey, 0L, KEY_READ, &sRegSubKey);
                if (sReturn == ERROR_SUCCESS)
                {
                    sValueCount = 0;
                    ::RegQueryInfoKey(sRegSubKey, 0, 0, 0, 0, 0, 0, &sValueCount, 0, 0, 0, 0);

                    if (sValueCount > 0)
                    {
                        sContinue = XPR_FALSE;
                        break;
                    }
                }

                ::RegCloseKey(sRegSubKey);

                if (_tcslen(sLinkKey) == 0)
                    break;

                _tcscpy(sBaseSubKey, sLinkKey);
            }

            if (XPR_IS_TRUE(sContinue))
                continue;

            xpr_sint_t j;
            for (j = 0; sShellNewData[j].mValue; ++j)
            {
                sValueLen = XPR_MAX_PATH;
                sValue[0] = XPR_STRING_LITERAL('\0');
                sReturn = ::RegQueryValueEx(sRegSubKey, sShellNewData[j].mValue, XPR_NULL, XPR_NULL, (xpr_byte_t *)sValue, &sValueLen);
                if (sReturn == ERROR_SUCCESS)
                {
                    SHFILEINFO sShFileInfo = {0};
                    ::SHGetFileInfo(sExt, FILE_ATTRIBUTE_NORMAL, &sShFileInfo, sizeof(sShFileInfo), SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);

                    sItem = new Item;
                    sItem->mType         = sShellNewData[j].mType;
                    sItem->mExt          = sExt;
                    sItem->mRegKey       = sSubKey;
                    sItem->mRegValue     = sShellNewData[j].mValue;
                    sItem->mRegValueType = sShellNewData[j].mRegValueType;
                    sItem->mIconIndex    = GetFileExtIconIndex(sItem->mExt.c_str());
                    sItem->mFileType     = sShFileInfo.szTypeName;

                    mShellNewDeque.push_back(sItem);
                    mShellNewMap[sId++] = sItem;

                    break;
                }
            }

            ::RegCloseKey(sRegSubKey);
        }
    }

    ::RegCloseKey(sRegKey);
}

//
// HKEY_CLASSES_ROOT\\*.ext\\ShellNew
// ---------------------------
// Value       Description
// ---------------------------
// NullFile    ""
// FileName    Path
// Data        Binary data
// Command     Command line
//

xpr_bool_t ShellNew::doCommand(xpr_uint_t aId, const xpr_tchar_t *aDir, xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDir))
        return XPR_FALSE;

    ShellNewMap::iterator sIterator = mShellNewMap.find(aId);
    if (sIterator == mShellNewMap.end())
        return XPR_FALSE;

    Item *sItem = sIterator->second;
    if (XPR_IS_NULL(sItem))
        return XPR_FALSE;

    HKEY sRegKey;
    DWORD sReturn;

    sReturn = RegOpenKeyEx(HKEY_CLASSES_ROOT, sItem->mRegKey.c_str(), 0L, KEY_READ, &sRegKey);
    if (sReturn != ERROR_SUCCESS)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    DWORD sType;
    DWORD sValueLen;
    xpr_byte_t *sValueData = new xpr_byte_t[sizeof(xpr_tchar_t) * (XPR_MAX_PATH + 1)];

    sType = sItem->mRegValueType;
    sValueLen = XPR_MAX_PATH;
    sReturn = RegQueryValueEx(sRegKey, sItem->mRegValue.c_str(), XPR_NULL, &sType, sValueData, &sValueLen);
    if (sReturn == ERROR_INSUFFICIENT_BUFFER)
    {
        XPR_SAFE_DELETE_ARRAY(sValueData);
        sValueData = new xpr_byte_t[sValueLen];

        sReturn = RegQueryValueEx(sRegKey, sItem->mRegValue.c_str(), XPR_NULL, &sType, sValueData, &sValueLen);
    }

    if (sReturn == ERROR_SUCCESS)
    {
        xpr_bool_t sBriefCaseFile = (_tcsicmp(sItem->mExt.c_str(), XPR_STRING_LITERAL(".bfc")) == 0) ? XPR_TRUE : XPR_FALSE;

        xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sDir, aDir);
        if (sDir[_tcslen(sDir)-1] == XPR_STRING_LITERAL('\\'))
            sDir[_tcslen(sDir)-1] = XPR_STRING_LITERAL('\0');

        xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
        _stprintf(sFileName, XPR_STRING_LITERAL("%s%s%s"), mPrefix.c_str(), sItem->mFileType.c_str(), mSuffix.c_str());

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        SetNewPath(sPath, sDir, sFileName, XPR_IS_TRUE(sBriefCaseFile) ? XPR_NULL : sItem->mExt.c_str());

        xpr_rcode_t sRcode;
        xpr_ssize_t sWritten;
        xpr_sint_t sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        xpr::FileIo sFileIo;

        switch (sItem->mType)
        {
        case TypeNullFile:
            {
                sRcode = sFileIo.open(sPath, sOpenMode);
                if (XPR_RCODE_IS_SUCCESS(sRcode) == XPR_TRUE)
                {
                    sFileIo.close();

                    sResult = XPR_TRUE;
                }
                break;
            }

        case TypeData:
            {
                sRcode = sFileIo.open(sPath, sOpenMode);
                if (XPR_RCODE_IS_SUCCESS(sRcode) == XPR_TRUE)
                {
                    sRcode = sFileIo.write(sValueData, sValueLen, &sWritten);
                    sFileIo.close();

                    sResult = XPR_TRUE;
                }
                break;
            }

        case TypeCommand:
            {
                xpr_tchar_t *sValue = (xpr_tchar_t *)sValueData;

                xpr_tchar_t sExpandValue[XPR_MAX_PATH * 3 + 1] = {0};
                ::ExpandEnvironmentStrings(sValue, sExpandValue, XPR_MAX_PATH * 3);

                std::tstring sExpandValue2 = sExpandValue;
                xpr_size_t sFind = sExpandValue2.find(XPR_STRING_LITERAL("%1"));
                if (sFind != std::tstring::npos)
                {
                    sExpandValue2.erase(sFind, 2);
                    sExpandValue2.insert(sFind, sPath);
                }

                if (XPR_IS_TRUE(sBriefCaseFile))
                {
                    sFind = sExpandValue2.find(XPR_STRING_LITERAL("%2"));
                    if (sFind != std::tstring::npos)
                    {
                        xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
                        SplitPathExt(sPath, XPR_NULL, sFileName, XPR_NULL);

                        sExpandValue2.erase(sFind, 2);
                        sExpandValue2.insert(sFind, XPR_STRING_LITERAL("1"));
                    }
                }

                std::tstring sFile;
                sFind = sExpandValue2.find(XPR_STRING_LITERAL(' '));

                sFile = sExpandValue2.substr(0, sFind);
                std::tstring sParam = sExpandValue2.substr(sFind + 1);

                ExecFile(sFile.c_str(), XPR_NULL, sParam.c_str());

                break;
            }

        case TypeFileName:
            {
                xpr_tchar_t *sValue = (xpr_tchar_t *)sValueData;

                xpr_tchar_t sSource[XPR_MAX_PATH + 1] = {0};
                if (sValue[1] != XPR_STRING_LITERAL(':') && sValue[1] != XPR_STRING_LITERAL('\\'))
                {
                    ::SHGetSpecialFolderPath(XPR_NULL, sSource, CSIDL_TEMPLATES, XPR_FALSE);
                    _stprintf(sSource+_tcslen(sSource), XPR_STRING_LITERAL("\\%s"), sValue);

                    if (IsExistFile(sSource) == XPR_FALSE)
                    {
                        ::SHGetSpecialFolderPath(XPR_NULL, sSource, CSIDL_COMMON_TEMPLATES, XPR_FALSE);
                        _stprintf(sSource+_tcslen(sSource), XPR_STRING_LITERAL("\\%s"), sValue);

                        if (IsExistFile(sSource) == XPR_FALSE)
                        {
                            ::SHGetSpecialFolderPath(XPR_NULL, sSource, CSIDL_WINDOWS, XPR_FALSE);
                            _stprintf(sSource+_tcslen(sSource), XPR_STRING_LITERAL("\\ShellNew\\%s"), sValue);
                        }
                    }

                    sSource[_tcslen(sSource)+1] = 0;
                }
                else
                {
                    _tcscpy(sSource, sValue);
                }

                SHFILEOPSTRUCT sShFileOpStruct = {0};
                sShFileOpStruct.hwnd   = XPR_NULL;
                sShFileOpStruct.fFlags = FOF_SILENT | FOF_NOERRORUI;
                sShFileOpStruct.wFunc  = FO_COPY;
                sShFileOpStruct.pFrom  = sSource;
                sShFileOpStruct.pTo    = sPath;

                if (::SHFileOperation(&sShFileOpStruct) == 0)
                    sResult = XPR_TRUE;

                break;
            }
        }

        if (sResult)
        {
            if (XPR_IS_NOT_NULL(aPath))
                _tcscpy(aPath, sPath);
        }
    }

    ::RegCloseKey(sRegKey);

    XPR_SAFE_DELETE_ARRAY(sValueData);

    return sResult;
}
} // namespace fxb
