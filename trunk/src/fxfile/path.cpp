//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "path.h"

#include "env_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
LPITEMIDLIST GetEnvPidl(const xpr::tstring &aEnv, xpr_uint_t *aCSIDL)
{
    return EnvPath::instance().getPidl(aEnv, aCSIDL);
}

LPITEMIDLIST GetEnvPidl(const xpr_tchar_t *aEnv, xpr_uint_t *aCSIDL)
{
    return EnvPath::instance().getPidl(aEnv, aCSIDL);
}

xpr_bool_t GetEnvPidl(const xpr::tstring &aEnv, LPITEMIDLIST *aPidl, xpr_uint_t *aCSIDL)
{
    return EnvPath::instance().getPidl(aEnv, aPidl, aCSIDL);
}

xpr_bool_t GetEnvPath(const xpr::tstring &aEnv, xpr::tstring &aPath, xpr_uint_t *aCSIDL)
{
    return EnvPath::instance().getPath(aEnv, aPath, aCSIDL);
}

xpr_bool_t GetEnvPath(const xpr_tchar_t *aEnv, xpr_tchar_t *aPath, xpr_uint_t *aCSIDL)
{
    if (XPR_IS_NULL(aEnv) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr::tstring sPath;
    if (EnvPath::instance().getPath(aEnv, sPath, aCSIDL) == XPR_FALSE)
        return XPR_FALSE;

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

static inline xpr_bool_t GetEnv(const xpr::tstring &aEnvPath, xpr::tstring &aEnv)
{
    aEnv.clear();

    if (aEnvPath.length() >= 3 && aEnvPath[0] == XPR_STRING_LITERAL('%'))
    {
        xpr_size_t sFind = aEnvPath.find('%', 1);
        if (sFind != xpr::tstring::npos)
            aEnv = aEnvPath.substr(0, sFind + 1);
    }

    return aEnv.empty() ? XPR_FALSE : XPR_TRUE;
}

void GetEnvRealPath(const xpr::tstring &aEnvPath, xpr::tstring &aRealPath)
{
    aRealPath.clear();

    xpr::tstring sEnv;
    if (GetEnv(aEnvPath, sEnv) == XPR_TRUE)
    {
        GetEnvPath(sEnv, aRealPath);
        if (aRealPath.empty() == XPR_FALSE)
            aRealPath.append(aEnvPath.c_str()+sEnv.length());
    }

    if (aRealPath.empty() == XPR_TRUE)
    {
        aRealPath = aEnvPath;
    }
}

xpr_bool_t GetEnvRealPidl(const xpr::tstring &aEnvPath, LPITEMIDLIST *aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    *aPidl = GetEnvRealPidl(aEnvPath);

    return (*aPidl != XPR_NULL);
}

LPITEMIDLIST GetEnvRealPidl(const xpr::tstring &aEnvPath)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;

    xpr::tstring sSubPath;
    xpr_uint_t sCSIDL = CSIDL_NONE;

    xpr::tstring sEnv;
    if (GetEnv(aEnvPath, sEnv) == XPR_TRUE)
    {
        sFullPidl = GetEnvPidl(sEnv, &sCSIDL);

        sSubPath = aEnvPath.c_str() + sEnv.length();
        if (sSubPath.empty() == XPR_FALSE)
        {
            if (sSubPath[0] == XPR_STRING_LITERAL('\\'))
                sSubPath.erase(0, 1);
        }
    }

    if (sSubPath.empty() == XPR_TRUE)
    {
        if (XPR_IS_NOT_NULL(sFullPidl))
            return sFullPidl;

        return fxfile::base::Pidl::create(aEnvPath.c_str());
    }

    if (XPR_IS_NULL(sFullPidl))
        return XPR_NULL;

    if (sSubPath.empty() == XPR_TRUE)
        return sFullPidl;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};

    _tcscpy(sPath, sSubPath.c_str());
    sPath[_tcslen(sPath) + 1] = XPR_STRING_LITERAL('\0'); // xpr_double_t null-terminated string

    xpr_tchar_t *sSplit = sPath;
    while (true)
    {
        sSplit = _tcschr(sSplit, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NULL(sSplit))
            break;

        sSplit[0] = XPR_STRING_LITERAL('\0');
        sSplit++;
    }

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    LPSHELLFOLDER sEnumShellFolder = XPR_NULL;
    LPENUMIDLIST sEnumIdList = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;
    LPITEMIDLIST sFullPidl2 = XPR_NULL;

    xpr_ulong_t sFetched;
    DWORD sFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN;

    xpr_sint_t sLoop = 0;
    xpr_bool_t sMatched = XPR_FALSE;

    sSplit = sPath;
    while (*sSplit != 0)
    {
        sMatched = XPR_FALSE;
        sEnumShellFolder = XPR_NULL;

        if (sLoop == 0 && sCSIDL == CSIDL_DESKTOP)
        {
            sEnumShellFolder = sShellFolder;
            sHResult = sEnumShellFolder->AddRef();
        }
        else
        {
            sHResult = sShellFolder->BindToObject(sFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sEnumShellFolder);
        }

        if (FAILED(sHResult) || XPR_IS_NULL(sEnumShellFolder))
            break;

        sPidl = XPR_NULL;
        sEnumIdList = XPR_NULL;

        sHResult = sEnumShellFolder->EnumObjects(XPR_NULL, sFlags, &sEnumIdList);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sEnumIdList))
        {
            while (S_OK == sEnumIdList->Next(1, &sPidl, &sFetched))
            {
                sHResult = GetName(sEnumShellFolder, sPidl, SHGDN_INFOLDER, sName);
                if (SUCCEEDED(sHResult) && !_tcsicmp(sSplit, sName))
                {
                    sMatched = XPR_TRUE;
                    break;
                }

                COM_FREE(sPidl);
            }
        }

        COM_RELEASE(sEnumIdList);
        COM_RELEASE(sEnumShellFolder);

        if (XPR_IS_NULL(sPidl))
            break;

        sFullPidl2 = fxfile::base::Pidl::concat(sFullPidl, sPidl);
        COM_FREE(sFullPidl);
        COM_FREE(sPidl);

        sFullPidl = sFullPidl2;

        sSplit += _tcslen(sSplit) + 1;
        sLoop++;
    }

    if (XPR_IS_FALSE(sMatched))
    {
        COM_FREE(sFullPidl);
    }

    return sFullPidl;
}

xpr_bool_t IsEqualFilter(const xpr_tchar_t *aFilter, const xpr_tchar_t *aExt)
{
    if (XPR_IS_NULL(aFilter))
        return XPR_FALSE;

    if (_tcsicmp(aFilter, XPR_STRING_LITERAL("*.*")) == 0)
        return XPR_TRUE;

    if (XPR_IS_NULL(aExt))
        return XPR_FALSE;

    // '.txt' -> 'txt'
    if (aExt[0] == XPR_STRING_LITERAL('.'))
        aExt++;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_size_t sLen;
    xpr_size_t sFileExtLen;
    const xpr_tchar_t *sExt2;

    sFileExtLen = _tcslen(aExt);

    while (aFilter[0] != '\0')
    {
        sExt2 = _tcschr(aFilter, ';');
        if (XPR_IS_NOT_NULL(sExt2)) sLen = (xpr_size_t)(sExt2 - aFilter);
        else                        sLen = _tcslen(aFilter);

        if (sFileExtLen == sLen && !_tcsnicmp(aFilter, aExt, sLen))
        {
            sResult = XPR_TRUE;
            break;
        }

        if (XPR_IS_NULL(sExt2))
            break;

        aFilter = _tcschr(aFilter, ';');
        if (XPR_IS_NULL(aFilter))
            break;

        aFilter++;
    }

    return sResult;
}

xpr_bool_t IsEqualFilterPath(const xpr_tchar_t *aFilter, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aFilter) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    const xpr_tchar_t *sExt;
    sExt = GetFileExt(aPath);

    return IsEqualFilter(aFilter, sExt);
}

void SplitPath(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName)
{
    if (XPR_IS_NULL(aPath))
        return;

    xpr_tchar_t *sSplit = (xpr_tchar_t *)_tcsrchr(aPath, XPR_STRING_LITERAL('\\'));
    if (XPR_IS_NULL(sSplit))
        return;

    *sSplit = XPR_STRING_LITERAL('\0');

    if (XPR_IS_NOT_NULL(aDir))
        _tcscpy(aDir, aPath);

    *sSplit = XPR_STRING_LITERAL('\\');

    if (XPR_IS_NOT_NULL(aFileName))
        _tcscpy(aFileName, sSplit + 1);
}

void SplitPathExt(const xpr_tchar_t *aPath, xpr_tchar_t *aDir, xpr_tchar_t *aFileName, xpr_tchar_t *aExt)
{
    SplitPath(aPath, aDir, aFileName);

    xpr_tchar_t *sExt2 = (xpr_tchar_t *)GetFileExt(aFileName);
    if (XPR_IS_NOT_NULL(sExt2))
    {
        if (XPR_IS_NOT_NULL(aExt))
            _tcscpy(aExt, sExt2);

        *sExt2 = XPR_STRING_LITERAL('\0');
    }
}

// BUGBUG
void SplitFileNameExt(xpr_tchar_t *aPath, xpr_tchar_t *aFileName, xpr_tchar_t *aExt)
{
    if (XPR_IS_NULL(aPath))
    {
        if (XPR_IS_NOT_NULL(aFileName))
            aFileName[0] = 0;

        if (XPR_IS_NOT_NULL(aExt))
            aExt[0] = 0;

        return;
    }

    if (XPR_IS_NOT_NULL(aExt))
        aExt[0] = '\0';

    if (aPath != aFileName)
        _tcscpy(aFileName, aPath);

    xpr_tchar_t *sExt2 = (xpr_tchar_t *)GetFileExt(aFileName);
    if (XPR_IS_NOT_NULL(sExt2))
    {
        if (XPR_IS_NOT_NULL(aExt))
            _tcscpy(aExt, sExt2);

        *sExt2 = '\0';
    }
}

void CombinePath(xpr_tchar_t *aPath, const xpr_tchar_t *aDir, const xpr_tchar_t *aFileName, const xpr_tchar_t *aExt)
{
    if (XPR_IS_NULL(aPath))
        return;

    aPath[0] = XPR_STRING_LITERAL('\0');

    if (XPR_IS_NOT_NULL(aDir))
        _tcscat(aPath, aDir);

    if (aPath[0] != XPR_STRING_LITERAL('\0'))
    {
        if (aPath[_tcslen(aPath)-1] != XPR_STRING_LITERAL('\\'))
            _tcscat(aPath, XPR_STRING_LITERAL("\\"));
    }

    if (XPR_IS_NOT_NULL(aFileName))
        _tcscat(aPath, aFileName);

    if (XPR_IS_NOT_NULL(aExt))
        _tcscat(aPath, aExt);
}

void CombinePath(xpr::tstring &aPath, const xpr::tstring &aDir, const xpr::tstring &aFileName, const xpr_tchar_t *aExt)
{
    aPath.clear();

    aPath += aDir;

    if (aPath[0] != XPR_STRING_LITERAL('\0'))
    {
        if (aPath[aPath.length()-1] != XPR_STRING_LITERAL('\\'))
            aPath += XPR_STRING_LITERAL('\\');
    }

    aPath += aFileName;

    if (XPR_IS_NOT_NULL(aExt))
        aPath += aExt;
}

void ConvUrlPath(xpr::tstring &aPath)
{
    xpr_size_t sFind = 0;
    while ((sFind = aPath.find(XPR_STRING_LITERAL('\\'), sFind)) != xpr::tstring::npos)
    {
        aPath.replace(sFind, 1, 1, XPR_STRING_LITERAL('/'));
        sFind++;
    }

    aPath.insert(0, XPR_STRING_LITERAL("file:///"));
}

void ConvDevPath(xpr::tstring &aPath)
{
    xpr_size_t sFind = 0;
    while ((sFind = aPath.find(XPR_STRING_LITERAL('\\'), sFind)) != xpr::tstring::npos)
    {
        aPath.replace(sFind, 1, 2, XPR_STRING_LITERAL('\\'));
        sFind += 2;
    }
}

xpr_bool_t SetNewPath(xpr_tchar_t       *aPath,
                      const xpr_tchar_t *aDir,
                      const xpr_tchar_t *aFileName,
                      const xpr_tchar_t *aExt,
                      const xpr_sint_t   aMaxNumber)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t i;
    for (i = 1; i < aMaxNumber; ++i)
    {
        _tcscpy(aPath, aDir);
        _tcscat(aPath, XPR_STRING_LITERAL("\\"));
        _tcscat(aPath, aFileName);

        if (i > 1)
        {
            _stprintf(aPath + _tcslen(aPath), XPR_STRING_LITERAL(" (%d)"), i);

            if (i ==  50) _tcscat(aPath, XPR_STRING_LITERAL(" ^^"));
            if (i == 100) _tcscat(aPath, XPR_STRING_LITERAL(" --"));
            if (i == 200) _tcscat(aPath, XPR_STRING_LITERAL(" !!"));
        }

        if (XPR_IS_NOT_NULL(aExt))
            _tcscat(aPath, aExt);

        if (IsExistFile(aPath) == XPR_FALSE)
        {
            sResult = XPR_TRUE;
            break;
        }
    }

    return sResult;
}

xpr_bool_t SetNewPath(xpr::tstring       &aPath,
                      const xpr::tstring &aDir,
                      const xpr::tstring &aFileName,
                      const xpr::tstring &aExt,
                      const xpr_sint_t    aMaxNumber)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t i;
    xpr_tchar_t sNumberText[100] = {0};

    for (i = 1; i < aMaxNumber; ++i)
    {
        aPath = aDir + XPR_STRING_LITERAL('\\') + aFileName;

        if (i > 1)
        {
            _stprintf(sNumberText, XPR_STRING_LITERAL(" (%d)"), i);
            aPath += sNumberText;

            if (i ==  50) aPath += XPR_STRING_LITERAL(" ^^");
            if (i == 100) aPath += XPR_STRING_LITERAL(" ~~");
            if (i == 200) aPath += XPR_STRING_LITERAL(" !!");
        }

        aPath += aExt;

        if (IsExistFile(aPath) == XPR_FALSE)
        {
            sResult = XPR_TRUE;
            break;
        }
    }

    return sResult;
}

void RemoveLastSplit(xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    const xpr_size_t sLen = _tcslen(aPath);
    if (sLen <= 0)
        return;

    if (aPath[sLen - 1] == '\\')
        aPath[sLen - 1] = '\0';
}

void RemoveLastSplit(xpr::tstring &aPath)
{
    const xpr_size_t sLen = aPath.length();
    if (sLen <= 0)
        return;

    if (aPath[sLen-1] == '\\')
        aPath.erase(sLen-1);
}

static xpr_bool_t _fxIsEqualPath(xpr::tstring aPath1, xpr::tstring aPath2, xpr_bool_t aCase)
{
    xpr_size_t sLen1 = aPath1.length();
    xpr_size_t sLen2 = aPath2.length();

    if (sLen1 > 1 && aPath1[sLen1-1] == XPR_STRING_LITERAL('\\')) { aPath1.erase(sLen1-1); sLen1--; }
    if (sLen2 > 1 && aPath2[sLen2-1] == XPR_STRING_LITERAL('\\')) { aPath2.erase(sLen2-1); sLen2--; }

    if (sLen1 != sLen2)
        return XPR_FALSE;

    xpr_sint_t sCompare;
    if (XPR_IS_TRUE(aCase))
        sCompare = _tcscmp(aPath1.c_str(), aPath2.c_str());
    else
        sCompare = _tcsicmp(aPath1.c_str(), aPath2.c_str());

    return (sCompare == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t IsEqualPath(const xpr_tchar_t *aPath1, const xpr_tchar_t *aPath2, xpr_bool_t aCase)
{
    if (XPR_IS_NULL(aPath1) && XPR_IS_NULL(aPath2)) return XPR_TRUE;
    if (XPR_IS_NULL(aPath1) || XPR_IS_NULL(aPath2)) return XPR_FALSE;

    xpr::tstring sPath1(aPath1);
    xpr::tstring sPath2(aPath2);

    return _fxIsEqualPath(sPath1, sPath2, aCase);
}

xpr_bool_t IsEqualPath(const xpr::tstring &aPath1, const xpr::tstring &aPath2, xpr_bool_t aCase)
{
    xpr::tstring sPath1(aPath1);
    xpr::tstring sPath2(aPath1);

    return _fxIsEqualPath(sPath1, sPath2, aCase);
}

xpr_bool_t VerifyFileName(const xpr::tstring &aFileName)
{
    xpr_bool_t sResult = XPR_TRUE;
    const xpr_tchar_t *sInvalidChar = XPR_STRING_LITERAL("\\/:*?\"<>|");

    xpr_size_t sFind = aFileName.find_first_of(sInvalidChar);
    if (sFind != xpr::tstring::npos)
        sResult = XPR_FALSE;

    return sResult;
}

xpr_bool_t VerifyFileName(const xpr_tchar_t *aFileName)
{
    if (XPR_IS_NULL(aFileName))
        return XPR_FALSE;

    xpr::tstring sFileName(aFileName);
    return VerifyFileName(sFileName);
}

void RemoveInvalidChar(xpr::tstring &aFileName)
{
    xpr_size_t sFind = 0;
    const xpr_tchar_t *sInvalidChar = XPR_STRING_LITERAL("\\/:*?\"<>|");

    while (true)
    {
        sFind = aFileName.find_first_of(sInvalidChar, sFind);
        if (sFind == xpr::tstring::npos)
            break;

        aFileName.erase(sFind, 1);
    }
}
} // namespace fxfile
