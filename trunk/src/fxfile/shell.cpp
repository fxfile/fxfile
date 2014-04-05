//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "shell.h"

#include <afxole.h>
#include <afxpriv.h>
#include <winioctl.h>      // NT I/O Control - Drive Type
#include <wininet.h>
#include <shobjidl.h>

#include "vwin32.h"    // 9x I/O Control - Drive Type
#include "file_op_thread.h"
#include "size_format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
static const xpr_tchar_t kSpecialFolderLibariesGuidString[] = XPR_STRING_LITERAL("::{031E4825-7B94-4DC3-B131-E946B44C8DD5}");

typedef WINSHELLAPI HRESULT (WINAPI *SHCreateItemFromIDListFunc)(LPCITEMIDLIST, REFIID, void **);
typedef WINSHELLAPI HRESULT (WINAPI *SHGetIDListFromObjectFunc)(IUnknown *, LPITEMIDLIST *);

LPITEMIDLIST Path2Pidl(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    // path representation
    // 1. %fxfile%
    // 2. 'C:\Windows'
    // 3. 'C:/Windows'
    // 4. 'file://C:/Windows'
    // 5. '\\flychk'
    // 6. ::{...}\
    // 7. Desktop\Documents (Old Full Path)
    // 8. Desktop\Computer\C:\Program Files

    xpr::tstring sPath(aPath);

    // '/' -> '\\'
    {
        xpr_size_t sFind = 0;
        while (true)
        {
            sFind = sPath.find(XPR_STRING_LITERAL('/'), sFind);
            if (sFind == xpr::tstring::npos)
                break;

            sPath[sFind] = XPR_STRING_LITERAL('\\');
            sFind++;
        }
    }

    aPath = sPath.c_str();

    if (aPath[0] == XPR_STRING_LITERAL('%'))
    {
        return GetEnvRealPidl(aPath);
    }

    static const xpr_tchar_t kLocalPath[]   = XPR_STRING_LITERAL(":\\");
    static const xpr_tchar_t kNetworkPath[] = XPR_STRING_LITERAL("\\\\");
    static const xpr_tchar_t kUrlPath[]     = XPR_STRING_LITERAL("file:");

    if (_tcsncmp(aPath+1, kLocalPath,   _tcslen(kLocalPath))   == 0 ||
        _tcsncmp(aPath,   kNetworkPath, _tcslen(kNetworkPath)) == 0 ||
        _tcsncmp(aPath,   kUrlPath,     _tcslen(kUrlPath))     == 0 )
    {
        return fxfile::base::Pidl::create(aPath);
    }

    LPITEMIDLIST sFullPidl = GetFullPidl(aPath);
    if (XPR_IS_NOT_NULL(sFullPidl))
        return sFullPidl;

    return GetDispFullPidl(aPath);
}

xpr_bool_t Path2Pidl(const xpr_tchar_t *aPath, LPITEMIDLIST *aFullPidl)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    *aFullPidl = Path2Pidl(aPath);

    return XPR_IS_NOT_NULL(*aFullPidl) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t Path2Pidl(const xpr::tstring &aPath, LPITEMIDLIST *aFullPidl)
{
    return Path2Pidl(aPath.c_str(), aFullPidl);
}

LPITEMIDLIST Path2Pidl(const xpr::tstring &aPath)
{
    return Path2Pidl(aPath.c_str());
}

xpr_bool_t Pidl2Path(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_bool_t aOnlyFileSystemPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr::tstring sPath;
    if (Pidl2Path(aFullPidl, sPath, aOnlyFileSystemPath) == XPR_FALSE)
        return XPR_FALSE;

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

xpr_bool_t Pidl2Path(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath, xpr_bool_t aOnlyFileSystemPath)
{
    if (XPR_IS_TRUE(aOnlyFileSystemPath))
    {
        GetName(aFullPidl, SHGDN_FORPARSING, aPath);
        return XPR_TRUE;
    }

    xpr::tstring sFullPath;
    GetFullPath(aFullPidl, sFullPath);

    if (IsParentVirtualItem(aFullPidl, sFullPath) == XPR_TRUE)
    {
        aPath = sFullPath;
        return XPR_TRUE;
    }

    GetName(aFullPidl, SHGDN_FORPARSING, aPath);

    return XPR_TRUE;
}

//
// folder path
//

// Desktop\Network\127.0.0.1 - Shortcut -> \\127.0.0.1
// cf) aFullPidl, SHGDN_FORPARSING -> C:\Documents and Settings\Administrator\NetHood\127.0.0.1 - Shortcut
xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        sResult = GetFolderPath(sShellFolder, sPidl, aPath, aShellAttributes);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t GetFolderPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath, xpr_ulong_t aShellAttributes)
{
    aPath.clear();

    xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
    if (GetFolderPath(aFullPidl, sPath, aShellAttributes) == XPR_FALSE)
        return XPR_FALSE;

    aPath = sPath;

    return XPR_TRUE;
}

xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aPath, xpr_ulong_t aShellAttributes)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (aShellAttributes == 0)
    {
        aShellAttributes = SFGAO_LINK;
        GetItemAttributes(aShellFolder, aPidl, aShellAttributes);
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    if (IsLibrariesSubFolder(aShellFolder, aPidl) == XPR_TRUE)
    {
        sResult = getDefaultSaveFolderOfLibraryFolder(aShellFolder, aPidl, sPath, XPR_MAX_PATH);
    }

    if (XPR_IS_FALSE(sResult))
    {
        GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);
    }

    if (aShellAttributes & SFGAO_LINK)
    {
        sResult = ResolveShortcut(XPR_NULL, sPath, aPath);
        if (XPR_IS_FALSE(sResult))
            aPath[0] = XPR_STRING_LITERAL('\0');
    }
    else
    {
        _tcscpy(aPath, sPath);
        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t GetFolderPath(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr::tstring &aPath, xpr_ulong_t aShellAttributes)
{
    aPath.clear();

    xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
    if (GetFolderPath(aShellFolder, aPidl, sPath, aShellAttributes) == XPR_FALSE)
        return XPR_FALSE;

    aPath = sPath;

    return XPR_TRUE;
}

//
// full path
//
void GetFullPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aFullPath)
{
    aFullPath.clear();

    if (XPR_IS_NULL(aFullPidl))
    {
        GetName(XPR_NULL, SHGDN_INFOLDER, aFullPath);
        return;
    }

    xpr::tstring sName;

    LPITEMIDLIST sFullPidl2 = fxfile::base::Pidl::clone(aFullPidl);
    while (XPR_IS_NOT_NULL(sFullPidl2))
    {
        if (sFullPidl2->mkid.cb == 0)
            break;

        sName.clear();
        GetName(sFullPidl2, SHGDN_INFOLDER | SHGDN_FORPARSING, sName);

        if (sName.empty() == XPR_TRUE)
            break;

        aFullPath.insert(0, sName);
        aFullPath.insert(0, XPR_STRING_LITERAL("\\"));

        fxfile::base::Pidl::removeLastItem(sFullPidl2);
    }

    if (aFullPath.empty() == XPR_FALSE)
        aFullPath.erase(0, 1);

    COM_FREE(sFullPidl2);
}

void GetFullPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aFullPath)
{
    xpr::tstring sFullPath;
    GetFullPath(aFullPidl, sFullPath);

    _tcscpy(aFullPath, sFullPath.c_str());
}

LPITEMIDLIST GetFullPidl(const xpr_tchar_t *aFullPath)
{
    if (XPR_IS_NULL(aFullPath))
        return XPR_NULL;

    LPITEMIDLIST sFullPidl = XPR_NULL;

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder))
    {
        xpr_wchar_t sWideFullPath[XPR_MAX_PATH + 1] = {0};
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;

        sInputBytes = _tcslen(aFullPath) * sizeof(xpr_tchar_t);
        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(aFullPath, &sInputBytes, sWideFullPath, &sOutputBytes);
        sWideFullPath[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

        sHResult = sShellFolder->ParseDisplayName(XPR_NULL, XPR_NULL, sWideFullPath, XPR_NULL, &sFullPidl, XPR_NULL);
    }

    COM_RELEASE(sShellFolder);

    return sFullPidl;
}

LPITEMIDLIST GetFullPidl(const xpr::tstring &aFullPath)
{
    return GetFullPidl(aFullPath.c_str());
}

xpr_bool_t GetFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    *aFullPidl = GetFullPidl(aFullPath);

    return XPR_IS_NOT_NULL(*aFullPidl) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t GetFullPidl(const xpr::tstring &aFullPath, LPITEMIDLIST *aFullPidl)
{
    return GetFullPidl(aFullPath.c_str(), aFullPidl);
}

void GetDispFullPath(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    if (XPR_IS_NULL(aFullPidl))
    {
        GetName(XPR_NULL, SHGDN_INFOLDER, aPath);
        return;
    }

    xpr::tstring sName;
    LPITEMIDLIST sFullPidl2 = fxfile::base::Pidl::clone(aFullPidl);

    xpr::tstring sPath;
    while (XPR_IS_NOT_NULL(sFullPidl2))
    {
        // new
        if (GetName(sFullPidl2, SHGDN_INFOLDER, sName) == XPR_FALSE)
            break;

        sPath = sName + sPath;
        if (sFullPidl2->mkid.cb != 0)
            sPath.insert(0, XPR_STRING_LITERAL("\\"));
        else
            break;

        fxfile::base::Pidl::removeLastItem(sFullPidl2);
    }

    _tcscpy(aPath, sPath.c_str());

    COM_FREE(sFullPidl2);
}

void GetDispFullPath(LPCITEMIDLIST aFullPidl, xpr::tstring &aPath)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        GetName(XPR_NULL, SHGDN_INFOLDER, aPath);
        return;
    }

    xpr::tstring sName;
    LPITEMIDLIST sFullPidl2 = fxfile::base::Pidl::clone(aFullPidl);

    aPath = XPR_STRING_LITERAL("");
    while (XPR_IS_NOT_NULL(sFullPidl2))
    {
        // new
        if (GetName(sFullPidl2, SHGDN_INFOLDER, sName) == XPR_FALSE)
            break;

        aPath = sName + aPath;
        if (sFullPidl2->mkid.cb != 0)
            aPath = XPR_STRING_LITERAL("\\") + aPath;
        else
            break;

        fxfile::base::Pidl::removeLastItem(sFullPidl2);
    }

    COM_FREE(sFullPidl2);
}

xpr_bool_t GetDispFullPidl(const xpr_tchar_t *aFullPath, LPITEMIDLIST *aFullPidl)
{
    if (XPR_IS_NULL(aFullPath) || XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    xpr_tchar_t sFullPath[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sFullPath, aFullPath);

    xpr_sint_t sCount = 1;

    xpr_tchar_t *sSplit = sFullPath;
    sSplit[_tcslen(sSplit)+1] = '\0';
    while (true)
    {
        sSplit = _tcschr(sSplit, '\\');
        if (sSplit == XPR_NULL)
            break;

        *sSplit = '\0';
        sSplit++;
        sCount++;
    }

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    sName[0] = '\0';
    xpr_tchar_t *sPath = sFullPath;
    xpr_tchar_t sInfolderPath[XPR_MAX_PATH + 1];
    xpr_bool_t sDrivePath;

    LPITEMIDLIST sFullPidl = XPR_NULL;
    LPSHELLFOLDER sShellFolder2 = XPR_NULL;
    LPENUMIDLIST sEnumIdList = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;

    xpr_ulong_t sFetched;
    DWORD sFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN | SHCONTF_STORAGE;

    xpr_sint_t i;
    xpr_bool_t sMatched = XPR_FALSE;

    for (i = 0; i < sCount; ++i) // preventing for infinite loop
    {
        sMatched = XPR_FALSE;

        if (XPR_IS_NULL(sFullPidl))
        {
            ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DESKTOP, &sPidl);
            sFullPidl = sPidl;

            sHResult = GetName(sPidl, SHGDN_INFOLDER, sName);
            if (SUCCEEDED(sHResult) && !_tcsicmp(sPath, sName))
                sMatched = XPR_TRUE;

            sPath += _tcslen(sPath) + 1;
        }
        else
        {
            if (i == 1)
            {
                sShellFolder2 = sShellFolder;
                sShellFolder->AddRef();
            }
            else
            {
                sShellFolder2 = XPR_NULL;
                sHResult = sShellFolder->BindToObject(sFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder2);
                if (FAILED(sHResult) || XPR_IS_NULL(sShellFolder2))
                    break;
            }

            sDrivePath = (_tcschr(sPath, XPR_STRING_LITERAL(':')) != XPR_NULL) ? XPR_TRUE : XPR_FALSE;

            sPidl = XPR_NULL;
            sEnumIdList = XPR_NULL;
            sHResult = sShellFolder2->EnumObjects(XPR_NULL, sFlags, &sEnumIdList);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sEnumIdList))
            {
                while (S_OK == sEnumIdList->Next(1, &sPidl, &sFetched))
                {
                    // Desktop\Computer\win (C:)\Program Files
                    sHResult = GetName(sShellFolder2, sPidl, SHGDN_INFOLDER, sName);
                    if (SUCCEEDED(sHResult) && _tcsicmp(sPath, sName) == 0)
                    {
                        sMatched = XPR_TRUE;
                        break;
                    }

                    // Desktop\Computer\C:\Program Files
                    if (XPR_IS_TRUE(sDrivePath))
                    {
                        sHResult = GetName(sShellFolder2, sPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sInfolderPath);
                        if (SUCCEEDED(sHResult) && _tcsicmp(sPath, sInfolderPath) == 0)
                        {
                            sMatched = XPR_TRUE;
                            break;
                        }
                    }

                    COM_FREE(sPidl);
                }
            }

            COM_RELEASE(sEnumIdList);
            COM_RELEASE(sShellFolder2);

            if (XPR_IS_NOT_NULL(sPidl))
            {
                LPITEMIDLIST sFullPidl2 = fxfile::base::Pidl::concat(sFullPidl, sPidl);
                COM_FREE(sFullPidl);
                COM_FREE(sPidl);

                sFullPidl = sFullPidl2;
            }

            sPath += _tcslen(sPath) + 1;
        }

        if (XPR_IS_TRUE(sMatched) && *sPath == '\0')
        {
            sResult = XPR_TRUE;
            break;
        }
    }

    if (XPR_IS_TRUE(sResult))
        *aFullPidl = sFullPidl;
    else
        COM_FREE(sFullPidl);

    return sResult;
}

LPITEMIDLIST GetDispFullPidl(const xpr_tchar_t *aFullPath)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;
    if (GetDispFullPidl(aFullPath, &sFullPidl) == XPR_FALSE)
        COM_FREE(sFullPidl);

    return sFullPidl;
}

//
// name
//
xpr_bool_t GetName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr::tstring &aFriendlyName)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        sResult = GetName(sShellFolder, sPidl, aFlags, aFriendlyName);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t GetName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        sResult = GetName(sShellFolder, sPidl, aFlags, aFriendlyName);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr::tstring &aFriendlyName)
{
    xpr_tchar_t sFriendlyName[XPR_MAX_PATH + 1] = {0};
    if (GetName(aShellFolder, aPidl, aFlags, sFriendlyName) == XPR_TRUE)
    {
        aFriendlyName = sFriendlyName;
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t GetName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr_tchar_t *aFriendlyName)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aFriendlyName))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    STRRET sStrRet;
    HRESULT sHResult = aShellFolder->GetDisplayNameOf(aPidl, aFlags, &sStrRet);
    if (SUCCEEDED(sHResult))
    {
        sHResult = StrRetToBuf(&sStrRet, aPidl, aFriendlyName, XPR_MAX_PATH);
        if (SUCCEEDED(sHResult))
            sResult = XPR_TRUE;
    }

    if (FAILED(sHResult))
        aFriendlyName[0] = XPR_STRING_LITERAL('\0');

    return sResult;
}

void PathToNormal(xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(aPath);
    if (XPR_IS_NOT_NULL(sExt) && _tcslen(sExt) == 4)
    {
        if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
            sExt[0] = '\0';
    }
}

void PathToNormal(const xpr_tchar_t *aPath, xpr_tchar_t *aNormal)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aNormal))
        return;

    xpr_tchar_t *sSplit = (xpr_tchar_t *)_tcsrchr(aPath, '\\');
    if (XPR_IS_NULL(sSplit))
        return;

    sSplit++;
    _tcscpy(aNormal, sSplit);

    PathToNormal(aNormal);
}

//
// file size
//
xpr_uint64_t GetFileSize(const xpr::tstring &aPath)
{
    return GetFileSize(aPath.c_str());
}

xpr_uint64_t GetFileSize(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return 0;

    WIN32_FIND_DATA sWin32FindData = {0};

    HANDLE sFindFile = ::FindFirstFile(aPath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return 0;

    ::FindClose(sFindFile);

    if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return 0;

    xpr_uint64_t sFileSize = sWin32FindData.nFileSizeLow;
    sFileSize += (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;

    return sFileSize;
}

xpr_bool_t GetFileSize(const xpr_tchar_t *aPath, xpr_tchar_t *aFileSizeText, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aFileSizeText) || aMaxLen <= 0)
        return XPR_FALSE;

    xpr_uint64_t sFileSize = GetFileSize(aPath);

    return GetFileSize(sFileSize, aFileSizeText, aMaxLen);
}

xpr_bool_t GetFileSize(xpr_uint64_t aFileSize, xpr_tchar_t *aFileSizeText, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aFileSizeText) || aMaxLen <= 0)
        return XPR_FALSE;

    aFileSizeText[0] = XPR_STRING_LITERAL('\0');

    SizeFormat::getDefSizeFormat(aFileSize, aFileSizeText, aMaxLen);

    return XPR_TRUE;
}

//
// file type
//
xpr_bool_t GetFileType(const xpr_tchar_t *aPath, xpr_tchar_t *aTypeName)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aTypeName))
        return XPR_FALSE;

    SHFILEINFO sShFileInfo = {0};
    memset(&sShFileInfo, 0, sizeof(SHFILEINFO));

    SHGetFileInfo(aPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME);
    _tcscpy(aTypeName, sShFileInfo.szTypeName);

    return XPR_TRUE;
}

//
// file time
//
xpr_bool_t GetFileTime(const xpr_tchar_t *aPath, xpr_tchar_t *aModifiedFileTime)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aModifiedFileTime))
        return XPR_FALSE;

    *aModifiedFileTime = XPR_STRING_LITERAL('\0');

    WIN32_FIND_DATA sWin32FindData = {0};
    HANDLE sFindFile = FindFirstFile(aPath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    ::FindClose(sFindFile);

    return GetFileTime(sWin32FindData.ftLastWriteTime, aModifiedFileTime);
}

xpr_bool_t GetFileTime(const FILETIME &aFileTime, xpr_tchar_t *aFileTimeText)
{
    xpr_tchar_t sDate[15] = {0};
    xpr_tchar_t sTime[15] = {0};
    FILETIME sFileTime;
    SYSTEMTIME sSystemTime;
    if (FileTimeToLocalFileTime(&aFileTime, &sFileTime) == XPR_FALSE)
    {
        aFileTimeText[0] = 0;
        return XPR_FALSE;
    }

    if (FileTimeToSystemTime(&sFileTime, &sSystemTime) == XPR_FALSE)
    {
        aFileTimeText[0] = 0;
        return XPR_FALSE;
    }

    GetDateFormat(XPR_NULL, 0, &sSystemTime, XPR_STRING_LITERAL("yyyy-MM-dd"), sDate, sizeof(sDate));
    GetTimeFormat(XPR_NULL, 0, &sSystemTime, XPR_STRING_LITERAL("tt hh:mm"), sTime, sizeof(sTime));
    _stprintf(aFileTimeText, XPR_STRING_LITERAL("%s %s"), sDate, sTime);

    return XPR_TRUE;
}

xpr_bool_t GetFileTime(const xpr_tchar_t *aPath, FILETIME *aCreationFileTime, FILETIME *aModifiedFileTime, FILETIME *aLastAccessFileTime)
{
    XPR_ASSERT(aPath != XPR_NULL);

    xpr_bool_t sResult = XPR_FALSE;

    WIN32_FIND_DATA sWin32FindData = {0};
    HANDLE sFindFile = ::FindFirstFile(aPath, &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        if (XPR_IS_NOT_NULL(aCreationFileTime))
        {
            aCreationFileTime->dwLowDateTime  = sWin32FindData.ftCreationTime.dwLowDateTime;
            aCreationFileTime->dwHighDateTime = sWin32FindData.ftCreationTime.dwHighDateTime;
        }

        if (XPR_IS_NOT_NULL(aModifiedFileTime))
        {
            aModifiedFileTime->dwLowDateTime  = sWin32FindData.ftLastWriteTime.dwLowDateTime;
            aModifiedFileTime->dwHighDateTime = sWin32FindData.ftLastWriteTime.dwHighDateTime;
        }

        if (XPR_IS_NOT_NULL(aLastAccessFileTime))
        {
            aLastAccessFileTime->dwLowDateTime  = sWin32FindData.ftLastAccessTime.dwLowDateTime;
            aLastAccessFileTime->dwHighDateTime = sWin32FindData.ftLastAccessTime.dwHighDateTime;
        }

        sResult = XPR_TRUE;
    }

    ::FindClose(sFindFile);

    return sResult;
}

xpr_bool_t SetFileTime(const xpr_tchar_t *aPath,
                       FILETIME          *aCreatedFileTime,
                       FILETIME          *aLastAccessFileTime,
                       FILETIME          *aModifiedFileTime,
                       xpr_bool_t         aForce)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    DWORD sShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    HANDLE sFile = ::CreateFile(aPath, GENERIC_WRITE, sShareMode, XPR_NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, XPR_NULL);
    if (sFile == INVALID_HANDLE_VALUE)
        sFile = ::CreateFile(aPath, 0, sShareMode, XPR_NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, XPR_NULL);

    if (sFile != INVALID_HANDLE_VALUE)
    {
        sResult = SetFileTime(sFile, aCreatedFileTime, aLastAccessFileTime, aModifiedFileTime);
        if (XPR_IS_TRUE(aForce) && XPR_IS_FALSE(sResult))
        {
            // set forcely file time for read-only file
            CLOSE_HANDLE(sFile);

            DWORD sFileAttributes = GetFileAttributes(aPath);
            if (sFileAttributes & FILE_ATTRIBUTE_READONLY)
            {
                sFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(aPath, sFileAttributes);

                sFile = ::CreateFile(aPath, GENERIC_WRITE, sShareMode, XPR_NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, XPR_NULL);
                if (sFile == INVALID_HANDLE_VALUE)
                    sFile = ::CreateFile(aPath, 0, sShareMode, XPR_NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, XPR_NULL);

                if (sFile != INVALID_HANDLE_VALUE)
                    sResult = SetFileTime(sFile, aCreatedFileTime, aLastAccessFileTime, aModifiedFileTime);

                sFileAttributes = GetFileAttributes(aPath);
                sFileAttributes |= FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(aPath, sFileAttributes);
            }
        }
    }

    CLOSE_HANDLE(sFile);

    return sResult;
}

//
// info tip
//
xpr_bool_t GetInfotip(HWND aHwnd, LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aInfotip)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aInfotip))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    IQueryInfo *sQueryInfo = XPR_NULL;

    HRESULT sHResult = aShellFolder->GetUIObjectOf(
        aHwnd,
        1,
        (LPCITEMIDLIST *)&aPidl,
        IID_IQueryInfo,
        0,
        (LPVOID *)&sQueryInfo);

    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sQueryInfo))
    {
        LPWSTR sWideInfotip = XPR_NULL;
        sHResult = sQueryInfo->GetInfoTip(SHGDN_INFOLDER, &sWideInfotip);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sWideInfotip))
        {
            if (sWideInfotip[0] != 0)
            {
                xpr_size_t sInputBytes = wcslen(sWideInfotip) * sizeof(xpr_wchar_t);
                xpr_size_t sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
                XPR_UTF16_TO_TCS(sWideInfotip, &sInputBytes, aInfotip, &sOutputBytes);
                aInfotip[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                sResult = XPR_TRUE;
            }
        }

        COM_FREE(sWideInfotip);
    }

    COM_RELEASE(sQueryInfo);

    return sResult;
}

//
// shell item and file icon
//
HICON GetItemIcon(const xpr_tchar_t *aPath, xpr_bool_t aOpen, xpr_bool_t aLarge)
{
    SHFILEINFO sShFileInfo;
    xpr_uint_t sFlags;
    sShFileInfo.hIcon = XPR_NULL;

    sFlags = SHGFI_ICON;
    if (XPR_IS_FALSE(aLarge)) sFlags |= SHGFI_SMALLICON;
    if (XPR_IS_TRUE(aOpen))   sFlags |= SHGFI_OPENICON;

    ::SHGetFileInfo(aPath, 0, &sShFileInfo, sizeof(SHFILEINFO), sFlags);

    return sShFileInfo.hIcon;
}

HICON GetItemIcon(LPCITEMIDLIST aFullPidl, xpr_bool_t aOpen, xpr_bool_t aLarge)
{
    SHFILEINFO sShFileInfo;
    xpr_uint_t sFlags;
    sShFileInfo.hIcon = XPR_NULL;

    sFlags = SHGFI_PIDL | SHGFI_ICON;
    if (XPR_IS_FALSE(aLarge)) sFlags |= SHGFI_SMALLICON;
    if (XPR_IS_TRUE(aOpen))   sFlags |= SHGFI_OPENICON;

    ::SHGetFileInfo((const xpr_tchar_t *)aFullPidl, 0, &sShFileInfo, sizeof(SHFILEINFO), sFlags);

    return sShFileInfo.hIcon;
}

HICON GetItemIcon(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aOpen, xpr_bool_t aLarge)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return XPR_NULL;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::getFullPidl(aShellFolder, aPidl);
    if (XPR_IS_NULL(sFullPidl))
        return XPR_NULL;

    HICON sIcon = GetItemIcon(sFullPidl);

    COM_FREE(sFullPidl);

    return sIcon;
}

xpr_sint_t GetItemIconIndex(const xpr_tchar_t *aPath, xpr_bool_t aOpen)
{
    SHFILEINFO sShFileInfo;
    xpr_uint_t sFlags = SHGFI_SYSICONINDEX;
    sShFileInfo.iIcon = -1;

    ::SHGetFileInfo(aPath, 0, &sShFileInfo, sizeof(SHFILEINFO), XPR_IS_TRUE(aOpen) ? (sFlags | SHGFI_OPENICON) : sFlags);
    DESTROY_ICON(sShFileInfo.hIcon);

    return sShFileInfo.iIcon;
}

xpr_sint_t GetItemIconIndex(LPCITEMIDLIST aFullPidl, xpr_bool_t aOpen)
{
    SHFILEINFO sShFileInfo;
    xpr_uint_t sFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX;
    sShFileInfo.iIcon = -1;

    ::SHGetFileInfo((const xpr_tchar_t *)aFullPidl, 0, &sShFileInfo, sizeof(SHFILEINFO), XPR_IS_TRUE(aOpen) ? sFlags | SHGFI_OPENICON : sFlags);
    DESTROY_ICON(sShFileInfo.hIcon);

    return sShFileInfo.iIcon;
}

xpr_sint_t GetItemIconIndex(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aOpenIcon)
{
    xpr_sint_t sIconIndex = -1;

    LPSHELLICON sShellIcon = XPR_NULL;
    HRESULT sHResult = aShellFolder->QueryInterface(IID_IShellIcon, (LPVOID *)&sShellIcon);

    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellIcon))
        sHResult = sShellIcon->GetIconOf(aPidl, XPR_IS_TRUE(aOpenIcon) ? GIL_OPENICON : GIL_FORSHELL, &sIconIndex);

    if (FAILED(sHResult) || sIconIndex <= 2)
    {
        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::getFullPidl(aShellFolder, aPidl);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            SHFILEINFO sShFileInfo;
            sShFileInfo.iIcon = -1;

            xpr_uint_t sFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX;
            if (XPR_IS_TRUE(aOpenIcon))
                sFlags |= SHGFI_OPENICON;

            ::SHGetFileInfo((const xpr_tchar_t *)sFullPidl, 0, &sShFileInfo, sizeof(SHFILEINFO), sFlags);
            sIconIndex = sShFileInfo.iIcon;
        }

        COM_FREE(sFullPidl);
    }

    COM_RELEASE(sShellIcon);
    return sIconIndex;
}

xpr_sint_t GetItemIconOverlayIndex(const xpr_tchar_t *aPath, xpr_bool_t aIconOverlay)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aPath);
    if (XPR_IS_NULL(sFullPidl))
        return -1;

    xpr_sint_t sOverlayIndex = GetItemIconOverlayIndex(sFullPidl, aIconOverlay);

    COM_FREE(sFullPidl);

    return sOverlayIndex;
}

xpr_sint_t GetItemIconOverlayIndex(LPCITEMIDLIST aFullPidl, xpr_bool_t aIconOverlay)
{
    if (XPR_IS_NULL(aFullPidl))
        return -1;

    xpr_sint_t    sOverlayIndex = -1;
    xpr_bool_t    sResult;
    LPSHELLFOLDER sShellFolder  = XPR_NULL;
    LPCITEMIDLIST sPidl         = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
        sOverlayIndex = GetItemIconOverlayIndex(sShellFolder, sPidl, aIconOverlay);

    COM_RELEASE(sShellFolder);

    return sOverlayIndex;
}

// This function must be called by worker thread.
// If it's called by UI thread, then it may be locked in the network folder on windows vista.
xpr_sint_t GetItemIconOverlayIndex(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_bool_t aIconOverlay)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return -1;

    if (IsFileSystem(aShellFolder, aPidl) == XPR_FALSE)
        return -1;

    HRESULT sHResult;
    xpr_sint_t sOverlayIndex = -1;
    LPSHELLICONOVERLAY sShellIconOverlay = XPR_NULL;

    sHResult = aShellFolder->QueryInterface(IID_IShellIconOverlay, (LPVOID *)&sShellIconOverlay);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellIconOverlay))
    {
        if (XPR_IS_TRUE(aIconOverlay))
            sHResult = sShellIconOverlay->GetOverlayIconIndex(aPidl, &sOverlayIndex);
        else
            sHResult = sShellIconOverlay->GetOverlayIndex(aPidl, &sOverlayIndex);

        // S_FALSE(0x00000001) - No overlay exists for this file.
        if (sHResult != S_OK)
            sOverlayIndex = -1;
    }

    COM_RELEASE(sShellIconOverlay);

    return sOverlayIndex;
}

xpr_sint_t GetFileExtIconIndex(const xpr_tchar_t *aExt)
{
    if (XPR_IS_NULL(aExt))
        return -1;

    xpr_uint_t sFlags;
    sFlags = SHGFI_SYSICONINDEX;

    SHFILEINFO sShFileInfo = {0};
    ::SHGetFileInfo(aExt, FILE_ATTRIBUTE_NORMAL, &sShFileInfo, sizeof(sShFileInfo), SHGFI_USEFILEATTRIBUTES | sFlags);

    if (sShFileInfo.iIcon < 0)
    {
        xpr_tchar_t sTempFile[XPR_MAX_PATH + 1] = {0};
        GetTempPath(XPR_MAX_PATH, sTempFile);

        if (sTempFile[_tcslen(sTempFile)-1] != XPR_STRING_LITERAL('\\'))
            _tcscat(sTempFile, XPR_STRING_LITERAL("\\"));

        _tcscat(sTempFile, XPR_STRING_LITERAL("temp"));
        _tcscat(sTempFile, aExt);

        xpr_rcode_t sRcode;
        xpr_sint_t sOpenMode;
        xpr::FileIo sFileIo;

        sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        sRcode = sFileIo.open(sTempFile, sOpenMode);
        sFileIo.close();

        ::SHGetFileInfo((const xpr_tchar_t *)sTempFile, 0, &sShFileInfo, sizeof(SHFILEINFO), sFlags);
    }

    return sShFileInfo.iIcon;
}

HICON GetFileExtIcon(const xpr_tchar_t *aExt, xpr_bool_t aLarge)
{
    if (XPR_IS_NULL(aExt))
        return XPR_NULL;

    xpr_uint_t sFlags;
    sFlags = SHGFI_ICON | (XPR_IS_TRUE(aLarge) ? 0 : SHGFI_SMALLICON);

    SHFILEINFO sShFileInfo = {0};
    ::SHGetFileInfo(aExt, FILE_ATTRIBUTE_NORMAL, &sShFileInfo, sizeof(sShFileInfo), SHGFI_USEFILEATTRIBUTES | sFlags);

    if (XPR_IS_NULL(sShFileInfo.hIcon))
    {
        xpr_tchar_t sTempFile[XPR_MAX_PATH + 1] = {0};
        GetTempPath(XPR_MAX_PATH, sTempFile);

        if (sTempFile[_tcslen(sTempFile)-1] != XPR_STRING_LITERAL('\\'))
            _tcscat(sTempFile, XPR_STRING_LITERAL("\\"));

        _tcscat(sTempFile, XPR_STRING_LITERAL("temp"));
        _tcscat(sTempFile, aExt);

        xpr_rcode_t sRcode;
        xpr_sint_t sOpenMode;
        xpr::FileIo sFileIo;

        sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        sRcode = sFileIo.open(sTempFile, sOpenMode);
        sFileIo.close();

        ::SHGetFileInfo((const xpr_tchar_t *)sTempFile, 0, &sShFileInfo, sizeof(sShFileInfo), sFlags);
    }

    return sShFileInfo.hIcon;
}

//
// shell item and file attribues
//

// remove 'zip folder' attrbiute
void SetItemAttribsFilter(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return;

    HRESULT sHResult;
    WIN32_FIND_DATA sWin32FindData;

    sHResult = ::SHGetDataFromIDList(aShellFolder, aPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
    if (SUCCEEDED(sHResult))
    {
        if (sWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            aShellAttributes |= SFGAO_GHOSTED;

        if (sWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            aShellAttributes |= SFGAO_FOLDER;
        else
            aShellAttributes &= ~SFGAO_FOLDER;
    }
}

xpr_ulong_t GetItemAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_ulong_t sShellAttributes = SFGAO_CAPABILITYMASK | SFGAO_DISPLAYATTRMASK | SFGAO_CONTENTSMASK | 0x7F300000;
    HRESULT sHResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&aPidl, &sShellAttributes);
    if (FAILED(sHResult))
        return 0;

    SetItemAttribsFilter(aShellFolder, aPidl, sShellAttributes);

    return sShellAttributes;
}

xpr_bool_t GetItemAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes)
{
    HRESULT sHResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &aShellAttributes);
    if (FAILED(sHResult))
        return XPR_FALSE;

    SetItemAttribsFilter(aShellFolder, aPidl, aShellAttributes);

    return XPR_TRUE;
}

xpr_ulong_t GetItemAttributes(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return 0;

    xpr_bool_t    sResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
        return 0;

    xpr_ulong_t sShellAttributes = GetItemAttributes(sShellFolder, sPidl);
    if (sShellAttributes != 0)
    {
        SetItemAttribsFilter(sShellFolder, sPidl, sShellAttributes);
    }

    COM_RELEASE(sShellFolder);

    return sShellAttributes;
}

xpr_bool_t GetItemAttributes(LPCITEMIDLIST aFullPidl, xpr_ulong_t &aShellAttributes)
{
    if (XPR_IS_NULL(aFullPidl))
        return 0;

    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
        return 0;

    sResult = GetItemAttributes(sShellFolder, sPidl, aShellAttributes);
    if (XPR_IS_TRUE(sResult))
    {
        SetItemAttribsFilter(sShellFolder, sPidl, aShellAttributes);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t IsFolder(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        return XPR_FALSE;
    }

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT       sComResult;

    sComResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sComResult))
    {
        return XPR_FALSE;
    }

    xpr_bool_t    sResult = XPR_FALSE;
    LPCITEMIDLIST sPidl   = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    sResult = IsFolder(sShellFolder, sPidl);

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t IsFolder(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
    {
        return XPR_FALSE;
    }

    xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSANCESTOR;
    HRESULT sComResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &sShellAttributes);

    if (SUCCEEDED(sComResult))
    {
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER) ||
            XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSANCESTOR))
        {
            return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

xpr_bool_t IsFileSystem(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t    sResult = XPR_FALSE;
    LPCITEMIDLIST sPidl   = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
        return XPR_FALSE;

    sResult = IsFileSystem(sShellFolder, sPidl);

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t IsFileSystem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_ulong_t sShellAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER;
    HRESULT sHResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &sShellAttributes);
    if ((SUCCEEDED(sHResult)) && (sShellAttributes & SFGAO_FILESYSTEM))
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t IsFileSystem(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    return (aPath[1] == ':' && aPath[2] == '\\');
}

xpr_bool_t IsFileSystemFolder(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t    sResult = XPR_FALSE;
    LPCITEMIDLIST sPidl   = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult))
        return XPR_FALSE;

    sResult = IsFileSystemFolder(sShellFolder, sPidl);

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t IsFileSystemFolder(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_ulong_t sShellAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER;
    HRESULT sHResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &sShellAttributes);
    if ((SUCCEEDED(sHResult)) && (sShellAttributes & SFGAO_FILESYSTEM))
    {
        WIN32_FIND_DATA sWin32FindData = {0};
        sHResult = ::SHGetDataFromIDList(aShellFolder, aPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
        if (SUCCEEDED(sHResult))
        {
            if (sWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                sResult = XPR_TRUE;
        }
        else
        {
            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

xpr_bool_t IsFileSystemFolder(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    DWORD sFileAttributes = GetFileAttributes(aPath);
    return XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
}

xpr_bool_t IsFileSystemFolder(const xpr::tstring &aPath)
{
    DWORD sFileAttributes = GetFileAttributes(aPath.c_str());
    return XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
}

xpr_bool_t IsNetItem(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (GetName(aFullPidl, SHGDN_FORPARSING, sPath) == XPR_FALSE)
        return XPR_FALSE;

    return IsNetItem(sPath);
}

xpr_bool_t IsNetItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (!GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath))
        return XPR_FALSE;

    return IsNetItem(sPath);
}

xpr_bool_t IsNetItem(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    return (aPath[0] == '\\' && aPath[1] == '\\');
}

xpr_bool_t IsParentVirtualItem(LPCITEMIDLIST aFullPidl)
{
    xpr::tstring sFullPath;
    GetFullPath(aFullPidl, sFullPath);

    return IsParentVirtualItem(aFullPidl, sFullPath);
}

xpr_bool_t IsParentVirtualItem(LPCITEMIDLIST aFullPidl, const xpr::tstring &aFullPath)
{
    if (aFullPath.empty() == XPR_TRUE)
        return XPR_TRUE;

    if (IsFileSystem(aFullPidl))
    {
        xpr_size_t sFind = aFullPath.find(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
        {
            xpr_tchar_t sDriveChar = aFullPath[sFind+1];
            xpr_bool_t sLocalFileSystem = (XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sDriveChar, 'z') || XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sDriveChar, 'Z')) && aFullPath[sFind+2] == XPR_STRING_LITERAL(':');
            xpr_bool_t sNetworkFileSystem = aFullPath[sFind+1] == XPR_STRING_LITERAL('\\') && aFullPath[sFind+2] == XPR_STRING_LITERAL('\\');

            if (XPR_IS_TRUE(sLocalFileSystem) || XPR_IS_TRUE(sNetworkFileSystem))
                return XPR_FALSE;
        }
    }
    else
    {
        xpr::tstring sPath;
        GetName(aFullPidl, SHGDN_FORPARSING, sPath);

        if (sPath.empty() == XPR_FALSE && sPath[0] == XPR_STRING_LITERAL('\\') && sPath[1] == XPR_STRING_LITERAL('\\'))
            return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t IsVirtualItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    GetName(aShellFolder, aPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sName);
    if (sName[0] == XPR_STRING_LITERAL(':') && sName[1] == XPR_STRING_LITERAL(':') && sName[2] == XPR_STRING_LITERAL('{') && sName[_tcslen(sName)-1] == XPR_STRING_LITERAL('}'))
        sResult = XPR_TRUE;
    else
    {
        GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sName);
        if (sName[0] == XPR_STRING_LITERAL(':') && sName[1] == XPR_STRING_LITERAL(':') && sName[2] == XPR_STRING_LITERAL('{') && sName[_tcslen(sName)-1] == XPR_STRING_LITERAL('}') || _tcschr(sName, XPR_STRING_LITERAL('\\')) == XPR_NULL)
            sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t IsVirtualItem(LPCITEMIDLIST aFullPidl)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_FALSE(sResult) || XPR_IS_NULL(sShellFolder) || XPR_IS_NULL(sPidl))
        return XPR_FALSE;

    sResult = IsVirtualItem(sShellFolder, sPidl);

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t IsExistFile(const xpr_tchar_t *aPath)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, aPath);

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(sPath);
    if (sLen > 0)
    {
        xpr_bool_t sNetworkPath = sPath[0] == XPR_STRING_LITERAL('\\') && sPath[1] == XPR_STRING_LITERAL('\\');

        // 9x windows
        if (XPR_IS_FALSE(sNetworkPath) && sPath[1] != XPR_STRING_LITERAL(':'))
            return XPR_TRUE;

        if (sPath[sLen-1] == XPR_STRING_LITERAL('\\'))
        {
            sPath[sLen-1] = XPR_STRING_LITERAL('\0');
            sLen--;
        }

        if (sLen == 2) // Drive Path
        {
            xpr_tchar_t sDriveStrings[XPR_MAX_PATH + 1] = {0};
            if (::GetLogicalDriveStrings(XPR_MAX_PATH, sDriveStrings) != 0)
            {
                xpr_tchar_t *sDrive = sDriveStrings;
                while (true)
                {
                    if (*sDrive == 0)
                        break;

                    if (_tcsnicmp(sDrive, sPath, 2) == 0)
                    {
                        sResult = XPR_TRUE;
                        break;
                    }

                    sDrive += _tcslen(sDrive) + 1;
                }
            }
        }
        else // General Path
        {
            if (_tcsncmp(sPath, XPR_STRING_LITERAL("\\\\"), 2) == 0) // ex) \\flychk\new folder
            {
                xpr_tchar_t *sSplit = _tcschr(sPath+2, XPR_STRING_LITERAL('\\'));
                if (XPR_IS_NOT_NULL(sSplit))
                {
                    sSplit = _tcschr(sSplit+1, XPR_STRING_LITERAL('\\'));
                    if (XPR_IS_NULL(sSplit))
                        sResult = XPR_TRUE;
                }
            }

            if (XPR_IS_FALSE(sResult))
            {
                WIN32_FIND_DATA sWin32FindData;

                HANDLE sFindFile = ::FindFirstFile(sPath, &sWin32FindData);
                if (sFindFile != INVALID_HANDLE_VALUE)
                {
                    ::FindClose(sFindFile);
                    sResult = XPR_TRUE;
                }
            }
        }
    }

    return sResult;
}

xpr_bool_t IsExistFile(const xpr::tstring &aPath)
{
    return IsExistFile(aPath.c_str());
}

xpr_bool_t IsLibrariesSubFolder(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        return XPR_FALSE;
    }

    LPITEMIDLIST sParentFullPidl = base::Pidl::clone(aFullPidl);

    if (base::Pidl::removeLastItem(sParentFullPidl) == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    xpr_bool_t sLibrariesSubFolder = XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetName(sParentFullPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sPath);

    if (_tcscmp(sPath, kSpecialFolderLibariesGuidString) == 0)
    {
        sLibrariesSubFolder = XPR_TRUE;
    }

    COM_FREE(sParentFullPidl);

    return sLibrariesSubFolder;
}

xpr_bool_t IsLibrariesSubFolder(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
    {
        return XPR_FALSE;
    }

    LPITEMIDLIST sFullPidl = base::Pidl::getFullPidl(aShellFolder, aPidl);
    if (XPR_IS_NULL(sFullPidl))
    {
        return XPR_FALSE;
    }

    xpr_bool_t sLibrariesSubFolder = IsLibrariesSubFolder(sFullPidl);

    COM_FREE(sFullPidl);

    return sLibrariesSubFolder;
}

static HRESULT SHCreateItemFromIDList(LPCITEMIDLIST aFullPidl, REFIID riid, void **ppv)
{
    HRESULT sComResult = E_FAIL;

    HMODULE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        SHCreateItemFromIDListFunc sSHCreateItemFromIDListFunc;
        sSHCreateItemFromIDListFunc = (SHCreateItemFromIDListFunc)GetProcAddress(sDll, "SHCreateItemFromIDList");

        if (XPR_IS_NOT_NULL(sSHCreateItemFromIDListFunc))
        {
            sComResult = sSHCreateItemFromIDListFunc(aFullPidl, riid, ppv);
        }

        ::FreeLibrary(sDll);
        sDll = XPR_NULL;
    }

    return sComResult;
}

static HRESULT SHGetIDListFromObject(IUnknown *aUnknown, LPITEMIDLIST *aFullPidl)
{
    HRESULT sComResult = E_FAIL;

    HMODULE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        SHGetIDListFromObjectFunc sSHGetIDListFromObjectFunc;
        sSHGetIDListFromObjectFunc = (SHGetIDListFromObjectFunc)GetProcAddress(sDll, "SHGetIDListFromObject");

        if (XPR_IS_NOT_NULL(sSHGetIDListFromObjectFunc))
        {
            sComResult = sSHGetIDListFromObjectFunc(aUnknown, aFullPidl);
        }

        ::FreeLibrary(sDll);
        sDll = XPR_NULL;
    }

    return sComResult;
}

xpr_bool_t getDefaultSaveFolderOfLibraryFolder(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aSavePath, xpr_size_t aMaxLen)
{
    HRESULT        sComResult;
    IShellItem    *sShellItem    = XPR_NULL;
    IShellLibrary *sShellLibrary = XPR_NULL;

    sComResult = SHCreateItemFromIDList(aFullPidl, IID_IShellItem, (void **)&sShellItem);
    if (SUCCEEDED(sComResult))
    {
        sComResult = CoCreateInstance(CLSID_ShellLibrary, XPR_NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&sShellLibrary));
        if (SUCCEEDED(sComResult))
        {
            sComResult = sShellLibrary->LoadLibraryFromItem(sShellItem, STGM_READ);
            if (SUCCEEDED(sComResult))
            {
                IShellItem *sDefaultSaveFolder = XPR_NULL;

                sComResult = sShellLibrary->GetDefaultSaveFolder(DSFT_DETECT, IID_IShellItem, (void **)&sDefaultSaveFolder);
                if (SUCCEEDED(sComResult))
                {
                    LPITEMIDLIST sFullPidl = XPR_NULL;
                    sComResult = SHGetIDListFromObject(sDefaultSaveFolder, &sFullPidl);

                    GetName(sFullPidl, SHGDN_FORPARSING, aSavePath);

                    COM_RELEASE(sDefaultSaveFolder);
                }
            }

            COM_RELEASE(sShellLibrary);
        }

        COM_RELEASE(sShellItem);
    }

    return XPR_TRUE;
}

xpr_bool_t getDefaultSaveFolderOfLibraryFolder(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aSavePath, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
    {
        return XPR_FALSE;
    }

    LPITEMIDLIST sFullPidl = base::Pidl::getFullPidl(aShellFolder, aPidl);
    if (XPR_IS_NULL(sFullPidl))
    {
        return XPR_FALSE;
    }

    xpr_bool_t sResult = getDefaultSaveFolderOfLibraryFolder(sFullPidl, aSavePath, aMaxLen);

    COM_FREE(sFullPidl);

    return sResult;
}

//
// disk and drive related
//
// disk(C:\\)
// network(\\\\flychk\\new folder\\folder)
xpr_bool_t GetDiskFreeSize(xpr_tchar_t *aPath, xpr_uint64_t &aFreeSize)
{
    aFreeSize = 0;

    // [2009/11/15] bug patched
    // Crash Occurrence in Windows NT 4.0 below
    ULARGE_INTEGER sTemp1 = {0};
    ULARGE_INTEGER sTemp2 = {0};
    ULARGE_INTEGER sFreeSize = {0};
    if (!::GetDiskFreeSpaceEx(aPath, &sTemp1, &sTemp2, &sFreeSize))
        return XPR_FALSE;

    aFreeSize = sFreeSize.QuadPart;

    return XPR_TRUE;
}

xpr_bool_t GetDiskTotalSize(xpr_tchar_t *aPath, xpr_uint64_t &aTotalSize)
{
    aTotalSize = 0;

    // [2009/11/15] bug patched
    // Crash Occurrence in Windows NT 4.0 below
    ULARGE_INTEGER sTemp1 = {0};
    ULARGE_INTEGER sTemp2 = {0};
    ULARGE_INTEGER sTotalSize = {0};
    if (::GetDiskFreeSpaceEx(aPath, &sTemp1, &sTotalSize, &sTemp2) == XPR_FALSE)
        return XPR_FALSE;

    aTotalSize = sTotalSize.QuadPart;

    return XPR_TRUE;
}

xpr_bool_t GetDiskSize(xpr_tchar_t *aPath, xpr_uint64_t &aTotalSize, xpr_uint64_t &aUsedSize, xpr_uint64_t &aFreeSize)
{
    aTotalSize = aUsedSize = aFreeSize = 0;

    // [2009/11/15] bug patched
    // Crash Occurrence in Windows NT 4.0 below
    ULARGE_INTEGER sTemp  = {0};
    ULARGE_INTEGER sTotalSize = {0};
    ULARGE_INTEGER sFreeSize  = {0};
    if (::GetDiskFreeSpaceEx(aPath, &sTemp, &sTotalSize, &sFreeSize) == XPR_FALSE)
        return XPR_FALSE;

    aTotalSize = sTotalSize.QuadPart;
    aFreeSize  = sFreeSize.QuadPart;
    aUsedSize  = sTotalSize.QuadPart - sFreeSize.QuadPart;

    return XPR_TRUE;
}

xpr_sint_t GetRootDriveType(xpr_tchar_t aDriveChar)
{
    xpr_sint_t sDriveType = DriveTypeUnknown;

    xpr_tchar_t sTemp[XPR_MAX_PATH + 1];
    xpr_tchar_t sDrivePath[XPR_MAX_PATH + 1];
    _stprintf(sDrivePath, XPR_STRING_LITERAL("%c:"), aDriveChar);

    xpr_uint_t sOSDriveType = GetDriveType((const xpr_tchar_t *)sDrivePath);
    switch (sOSDriveType)
    {
    case DRIVE_REMOVABLE:
        {
            HANDLE sFile;
            xpr_sint_t sDriveTypeNumber;

            if ((xpr_sint_t)GetVersion() < 0)
            {
                sFile = ::CreateFileA("\\\\.\\VWIN32", 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0);

                if (sFile == INVALID_HANDLE_VALUE)
                {
                    sDriveTypeNumber = -1;
                }

                DWORD cb;
                DIOC_REGISTERS reg;
                DOSDPB dpb;

                dpb.specialFunc = 0;

                xpr_char_t sDrive[2];
                sprintf(sDrive, "%c", sDrivePath[0]);
                strcpy(sDrive, _strlwr(sDrive));
                xpr_sshort_t iDrive = (xpr_sshort_t)atoi(sDrive) + 1;

                reg.reg_EBX   = iDrive;
                reg.reg_EDX   = (DWORD)(uintptr_t)&dpb;
                reg.reg_ECX   = 0x0860;
                reg.reg_EAX   = 0x440D;
                reg.reg_Flags = CARRY_FLAG;

                if (::DeviceIoControl(
                    sFile,
                    VWIN32_DIOC_DOS_IOCTL, 
                    &reg,
                    sizeof(reg),
                    &reg,
                    sizeof(reg),
                    &cb, 0) && !(reg.reg_Flags & CARRY_FLAG))
                {
                    switch (dpb.devType)
                    {
                    case 0:
                    case 1:  sDriveTypeNumber = 525; break;
                    case 2:
                    case 7:
                    case 9:  sDriveTypeNumber =  35; break;
                    case 3:
                    case 4:  sDriveTypeNumber =   8; break;
                    case 5:
                    case 6:
                    case 8:  sDriveTypeNumber =   0; break;
                    default: sDriveTypeNumber =  -1; break;
                    }
                }
                else 
                {
                    sDriveTypeNumber = -1;
                }

                CLOSE_HANDLE(sFile);
            }
            else
            {
                DISK_GEOMETRY SupportedGeometry[20];
                DWORD SupportedGeometryCount;

                _stprintf(sTemp, XPR_STRING_LITERAL("\\\\.\\%c:"), sDrivePath[0]);
                sFile = ::CreateFile(sTemp, 0, FILE_SHARE_READ, XPR_NULL, OPEN_ALWAYS, 0, XPR_NULL);

                if (sFile == INVALID_HANDLE_VALUE)
                    sDriveTypeNumber = -1;
                else
                {
                    DWORD ReturnedByteCount;

                    if (::DeviceIoControl(
                        sFile,
                        IOCTL_DISK_GET_MEDIA_TYPES,
                        XPR_NULL,
                        0,
                        SupportedGeometry,
                        sizeof(SupportedGeometry),
                        &ReturnedByteCount,
                        XPR_NULL))
                    {
                        SupportedGeometryCount = ReturnedByteCount / sizeof(DISK_GEOMETRY);

                        for (xpr_uint_t i = 0; i < SupportedGeometryCount; ++i)
                        {
                            switch (SupportedGeometry[i].MediaType)
                            {
                            case F3_1Pt44_512:
                            case F3_2Pt88_512:
                            case F3_20Pt8_512:
                            case F3_720_512:     sDriveTypeNumber  = 35; break;
                            case F5_1Pt2_512:
                            case F5_360_512:
                            case F5_320_512:
                            case F5_320_1024:
                            case F5_180_512:
                            case F5_160_512:     sDriveTypeNumber = 525; break;
                            case RemovableMedia: sDriveTypeNumber =   0; break;
                            default:             sDriveTypeNumber =  -1; break;
                            }
                        }
                    }
                    else
                    {
                        sDriveTypeNumber = -1;
                    }

                    CLOSE_HANDLE(sFile);
                }
            }

            switch (sDriveTypeNumber)
            {
            case   8: sDriveType = DriveType8InchFloppyDisk;   break;
            case  35: sDriveType = DriveType35InchFloppyDisk;  break;
            case 525: sDriveType = DriveType525InchFloppyDisk; break;
            default:  sDriveType = DriveTypeRemovableDisk;     break;
            }

            break;
        }

    case DRIVE_FIXED:   sDriveType = DriveTypeLocal;   break;
    case DRIVE_REMOTE:  sDriveType = DriveTypeNetwork; break;
    case DRIVE_CDROM:   sDriveType = DriveTypeCdRom;   break;
    case DRIVE_RAMDISK: sDriveType = DriveTypeRam;     break;
    }

    return sDriveType;
}

void GetDriveStrings(xpr_tchar_t *aDriveStrings, xpr_sint_t aMaxLen)
{
    if (XPR_IS_NULL(aDriveStrings))
        return;

    if (aMaxLen == -1)
        aMaxLen = XPR_MAX_PATH + 1;

    memset(aDriveStrings, 0, aMaxLen * sizeof(xpr_tchar_t));

    LPITEMIDLIST sDriveFullPidl = XPR_NULL;
    HRESULT sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sDriveFullPidl);
    if (SUCCEEDED(sHResult))
    {
        LPSHELLFOLDER sShellFolder2 = XPR_NULL;
        ::SHGetDesktopFolder(&sShellFolder2);

        LPSHELLFOLDER sShellFolder = XPR_NULL;
        HRESULT sHResult = sShellFolder2->BindToObject(sDriveFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder);
        COM_RELEASE(sShellFolder2);

        if (SUCCEEDED(sHResult))
        {
            LPENUMIDLIST sEnumIdList = XPR_NULL;
            sHResult = sShellFolder->EnumObjects(XPR_NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &sEnumIdList);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sEnumIdList))
            {
                xpr_tchar_t sName[XPR_MAX_PATH + 1];
                xpr_tchar_t *sColon;
                xpr_tchar_t *sLast = aDriveStrings;

                xpr_ulong_t sFetched;
                LPITEMIDLIST sPidl = XPR_NULL;
                while (S_OK == sEnumIdList->Next(1, &sPidl, &sFetched))
                {
                    GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);
                    sColon = _tcsrchr(sName, ':');
                    if (XPR_IS_NOT_NULL(sColon))
                    {
                        --sColon;
                        _stprintf(sLast, XPR_STRING_LITERAL("%c:\\"), *sColon);

                        sLast += _tcslen(sLast) + 1;
                    }

                    COM_FREE(sPidl);
                }
            }

            COM_RELEASE(sEnumIdList);
        }

        COM_RELEASE(sShellFolder);
        COM_FREE(sDriveFullPidl);
    }

    if (FAILED(sHResult))
        GetLogicalDriveStrings(XPR_MAX_PATH + 1, aDriveStrings);

    _tcsupr(aDriveStrings);
}

xpr_bool_t GetDriveFileSystem(const xpr_tchar_t *aDrive, xpr_tchar_t *aFileSystem, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aDrive) || XPR_IS_NULL(aFileSystem))
        return XPR_FALSE;

    return ::GetVolumeInformation(aDrive, XPR_NULL, 0, XPR_NULL, 0, XPR_NULL, aFileSystem, (DWORD)aMaxLen);
}

xpr_bool_t IsDriveSecure(xpr_tchar_t aDriveChar)
{
    xpr_tchar_t sDrive[4] = {0};
    _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), aDriveChar);

    xpr_tchar_t sLabel[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sFileSystem[XPR_MAX_PATH + 1] = {0};
    GetVolumeInformation(sDrive, sLabel, XPR_MAX_PATH, XPR_NULL, XPR_NULL, XPR_NULL, sFileSystem, XPR_MAX_PATH);

    if (_tcsnicmp(sFileSystem, XPR_STRING_LITERAL("NTFS"), 4) == 0)
        return XPR_TRUE;

    return XPR_FALSE;
}

//
// file exetension
//
xpr_bool_t IsEqualFileExt(const xpr_tchar_t *aPath, const xpr_tchar_t *aExt)
{
    xpr_tchar_t *sFileName = (xpr_tchar_t *)_tcsrchr(aPath, XPR_STRING_LITERAL('\\'));
    if (XPR_IS_NOT_NULL(sFileName))
        sFileName++;
    else
        sFileName = (xpr_tchar_t *)aPath;

    xpr_bool_t sEqual = XPR_FALSE;

    const xpr_tchar_t *sExt = GetFileExt(sFileName);
    if (XPR_IS_NOT_NULL(sExt))
        sEqual = !_tcsicmp(sExt, aExt);

    return sEqual;
}

const xpr_tchar_t *GetFileExt(const xpr::tstring &aPath)
{
    const xpr_tchar_t *sExt = ::PathFindExtension(aPath.c_str());
    if (XPR_IS_NULL(sExt))
        return XPR_NULL;

    if (_tcslen(sExt) <= 0)
        return XPR_NULL;

    return sExt;
}

const xpr_tchar_t *GetFileExt(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_NULL;

    const xpr_tchar_t *sExt = ::PathFindExtension(aPath);
    if (XPR_IS_NULL(sExt))
        return XPR_NULL;

    if (_tcslen(sExt) <= 0)
        return XPR_NULL;

    return sExt;

    //const xpr_tchar_t *sFileName;
    //sFileName = _tcsrchr(aPath, XPR_STRING_LITERAL('\\'));
    //if (sFileName)
    //    sFileName++;
    //else
    //    sFileName = aPath;

    //const xpr_tchar_t *sExt = GetFileExt(aPath);
    //if (XPR_IS_NULL(sExt))
    //    return 0;

    //if (sExt[1] == XPR_STRING_LITERAL(' '))
    //    return 0;

    //return sExt;
}

xpr_bool_t IsFileKnownExt(const xpr_tchar_t *aExt)
{
    if (XPR_IS_NULL(aExt))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    HKEY sRegKey = XPR_NULL;
    xpr_tchar_t sKeyName[0xff] = {0};
    xpr_tchar_t sContentType[XPR_MAX_PATH + 1] = {0};
    DWORD sValueType;
    xpr_ulong_t sBytes;
    xpr_slong_t sError;

    _tcscpy(sKeyName, aExt);

    xpr_sint_t i;
    for (i = 0; i < 20; ++i)
    {
        if (sRegKey != XPR_NULL)
        {
            ::RegCloseKey(sRegKey);
            sRegKey = XPR_NULL;
        }

        sError = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, sKeyName, 0, KEY_READ, &sRegKey);
        if (sError != ERROR_SUCCESS)
            break;

        sValueType = REG_NONE;
        sBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
        sError = ::RegQueryValueEx(sRegKey, XPR_STRING_LITERAL(""), XPR_NULL, &sValueType, reinterpret_cast<xpr_byte_t *>(sContentType), &sBytes);
        if (sValueType == REG_SZ && sError == ERROR_SUCCESS)
        {
            if (_tcslen(sContentType) != 0)
            {
                _tcscpy(sKeyName, sContentType);
                sResult = XPR_TRUE;
                continue;
            }
        }

        sValueType = REG_NONE;
        sBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
        sError = ::RegQueryValueEx(sRegKey, XPR_STRING_LITERAL("Content Type"), XPR_NULL, &sValueType, reinterpret_cast<xpr_byte_t *>(sContentType), &sBytes);
        if (sValueType == REG_SZ && sError == ERROR_SUCCESS)
        {
            if (_tcslen(sContentType) != 0)
            {
                _tcscpy(sKeyName, sContentType);
                sResult = XPR_TRUE;
                continue;
            }
        }

        break;
    }

    if (sRegKey != XPR_NULL)
    {
        ::RegCloseKey(sRegKey);
        sRegKey = XPR_NULL;
    }

    return sResult;
}

//
// file execution
//
xpr_bool_t ExecFile(LPCITEMIDLIST      aFullPidl,
                    const xpr_tchar_t *aStartup,
                    const xpr_tchar_t *aParameter,
                    xpr_sint_t         aShowCmd)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    switch (aShowCmd)
    {
    case 1:  aShowCmd = SW_MINIMIZE;    break;
    case 2:  aShowCmd = SW_MAXIMIZE;    break;
    default: aShowCmd = SW_SHOWDEFAULT; break;
    }

    xpr::tstring sStartup;
    if (XPR_IS_NOT_NULL(aStartup))
        sStartup = aStartup;

    if (sStartup.empty() == XPR_TRUE && IsVirtualItem(aFullPidl) == XPR_FALSE)
    {
        GetName(aFullPidl, SHGDN_FORPARSING, sStartup);

        xpr_size_t sFind = sStartup.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
            sStartup = sStartup.substr(0, sFind);

        if (sStartup.length() == 2)
        {
            if (sStartup[1] == XPR_STRING_LITERAL(':'))
                sStartup += XPR_STRING_LITERAL('\\');
        }
    }

    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
    sShellExecuteInfo.fMask        = SEE_MASK_INVOKEIDLIST;
    sShellExecuteInfo.hwnd         = AfxGetMainWnd()->GetSafeHwnd();
    sShellExecuteInfo.nShow        = aShowCmd;
    sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();
    sShellExecuteInfo.lpIDList     = (void *)aFullPidl;
    sShellExecuteInfo.lpParameters = aParameter;
    sShellExecuteInfo.lpDirectory  = sStartup.c_str();

    xpr_bool_t sResult = ::ShellExecuteEx(&sShellExecuteInfo);
    if (XPR_IS_FALSE(sResult))
    {
        xpr_ulong_t sShellAttributes = GetItemAttributes(aFullPidl);
        if (sShellAttributes & SFGAO_FILESYSTEM)
        {
            sShellExecuteInfo.lpVerb = XPR_STRING_LITERAL("openas");
            sResult = ::ShellExecuteEx(&sShellExecuteInfo);
        }
    }

    return sResult;
}

xpr_bool_t ExecFile(const xpr_tchar_t *aPath,
                    const xpr_tchar_t *aStartup,
                    const xpr_tchar_t *aParameter,
                    xpr_sint_t         aShowCmd)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    switch (aShowCmd)
    {
    case 1:  aShowCmd = SW_MINIMIZE;    break;
    case 2:  aShowCmd = SW_MAXIMIZE;    break;
    default: aShowCmd = SW_SHOWDEFAULT; break;
    }

    xpr::tstring sStartup;
    if (XPR_IS_NOT_NULL(aStartup))
        sStartup = aStartup;

    if (sStartup.empty() == XPR_TRUE)
    {
        sStartup = aPath;
        xpr_size_t sFind = sStartup.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
            sStartup = sStartup.substr(0, sFind);

        if (sStartup.length() == 2)
        {
            if (sStartup[1] == XPR_STRING_LITERAL(':'))
                sStartup += XPR_STRING_LITERAL('\\');
        }
    }

    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
    sShellExecuteInfo.fMask        = 0;
    sShellExecuteInfo.hwnd         = AfxGetMainWnd()->GetSafeHwnd();
    sShellExecuteInfo.nShow        = aShowCmd;
    sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();
    sShellExecuteInfo.lpFile       = aPath;
    sShellExecuteInfo.lpParameters = aParameter;
    sShellExecuteInfo.lpDirectory  = sStartup.c_str();

    xpr_bool_t sResult = ::ShellExecuteEx(&sShellExecuteInfo);
    if (XPR_IS_FALSE(sResult))
    {
        sShellExecuteInfo.lpVerb = XPR_STRING_LITERAL("openas");
        sResult = ::ShellExecuteEx(&sShellExecuteInfo);
    }

    return sResult;
}

xpr_bool_t OpenAsFile(const xpr_tchar_t *aPath,
                      const xpr_tchar_t *aStartup,
                      const xpr_tchar_t *aParameter,
                      xpr_sint_t         aShowCmd)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    switch (aShowCmd)
    {
    case 1:  aShowCmd = SW_MINIMIZE;    break;
    case 2:  aShowCmd = SW_MAXIMIZE;    break;
    default: aShowCmd = SW_SHOWDEFAULT; break;
    }

    xpr::tstring sStartup;
    if (XPR_IS_NOT_NULL(aStartup))
        sStartup = aStartup;

    if (sStartup.empty() == XPR_TRUE)
    {
        sStartup = aPath;
        xpr_size_t sFind = sStartup.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
            sStartup = sStartup.substr(0, sFind);

        if (sStartup.length() == 2)
        {
            if (sStartup[1] == XPR_STRING_LITERAL(':'))
                sStartup += XPR_STRING_LITERAL('\\');
        }
    }

    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);

    if (xpr::getOsVer() < xpr::kOsVerWinVista)
    {
        sShellExecuteInfo.fMask        = 0;
        sShellExecuteInfo.hwnd         = AfxGetMainWnd()->GetSafeHwnd();
        sShellExecuteInfo.nShow        = aShowCmd;
        sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();
        sShellExecuteInfo.lpFile       = aPath;
        sShellExecuteInfo.lpVerb       = XPR_STRING_LITERAL("openas");
        sShellExecuteInfo.lpParameters = aParameter;
        sShellExecuteInfo.lpDirectory  = sStartup.c_str();
    }
    else
    {
        xpr_tchar_t sParameters[XPR_MAX_PATH * 2 + 1] = {0};
        _stprintf(sParameters, XPR_STRING_LITERAL("shell32.dll,OpenAs_RunDLL %s"), aPath);

        sShellExecuteInfo.fMask        = 0;
        sShellExecuteInfo.hwnd         = AfxGetMainWnd()->GetSafeHwnd();
        sShellExecuteInfo.nShow        = aShowCmd;
        sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();
        sShellExecuteInfo.lpFile       = XPR_STRING_LITERAL("rundll32.exe");
        sShellExecuteInfo.lpVerb       = XPR_STRING_LITERAL("open");
        sShellExecuteInfo.lpParameters = sParameters;
        sShellExecuteInfo.lpDirectory  = sStartup.c_str();
    }

    return ::ShellExecuteEx(&sShellExecuteInfo);
}

void NavigateURL(xpr_tchar_t *aUrl)
{
    if (XPR_IS_NULL(aUrl))
        return;

    xpr::tstring sUrl(aUrl);

    xpr_size_t sFind = sUrl.find(XPR_STRING_LITERAL("://"));
    if (sFind == xpr::tstring::npos)
        sUrl.insert(0, XPR_STRING_LITERAL("http://"));

    ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sUrl.c_str(), XPR_NULL, XPR_NULL, 0);
}

//
// directory
//
xpr_bool_t CreateDirectoryLevel(const xpr_tchar_t *aDir, xpr_size_t aOffset, xpr_bool_t aLastDir)
{
    if (XPR_IS_NULL(aDir))
        return XPR_FALSE;

    if (_tcslen(aDir) <= aOffset)
        aOffset = 0;

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sDir, aDir);
    sDir[_tcslen(sDir)+1] = XPR_STRING_LITERAL('\0');

    xpr_tchar_t *sSplit = sDir + aOffset;
    if (sSplit[0] == XPR_STRING_LITERAL('\\'))
        sSplit++;

    xpr_bool_t sResult = XPR_TRUE;

    while (true)
    {
        sSplit = _tcschr(sSplit, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NULL(sSplit))
            break;

        sSplit[0] = XPR_STRING_LITERAL('\0');

        if ((sSplit-sDir) != 2) // drive
        {
            if (IsExistFile(sDir) == XPR_FALSE)
            {
                if (::CreateDirectory(sDir, XPR_NULL) == XPR_FALSE)
                {
                    sResult = XPR_FALSE;
                    break;
                }
            }
        }

        sSplit[0] = XPR_STRING_LITERAL('\\');
        sSplit++;
    }

    if (XPR_IS_NOT_NULL(aLastDir))
    {
        if (IsExistFile(sDir) == XPR_FALSE)
        {
            if (::CreateDirectory(sDir, XPR_NULL) == XPR_FALSE)
                sResult = XPR_FALSE;
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        sResult = XPR_TEST_BITS(GetFileAttributes(aDir), FILE_ATTRIBUTE_DIRECTORY);
    }

    return sResult;
}

//
// file operation
//
void SHSilentDeleteFile(const xpr_tchar_t *aSource)
{
    if (XPR_IS_NULL(aSource))
        return;

    SHFILEOPSTRUCT sShFileOpStruct = {0};
    sShFileOpStruct.hwnd   = XPR_NULL;
    sShFileOpStruct.wFunc  = FO_DELETE;
    sShFileOpStruct.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
    sShFileOpStruct.pFrom  = aSource;
    sShFileOpStruct.pTo    = XPR_NULL;
    ::SHFileOperation(&sShFileOpStruct);
}

void SHSilentCopyFile(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget)
{
    if (XPR_IS_NULL(aSource) || XPR_IS_NULL(aTarget))
        return;

    SHFILEOPSTRUCT sShFileOpStruct = {0};
    sShFileOpStruct.hwnd   = XPR_NULL;
    sShFileOpStruct.wFunc  = FO_COPY;
    sShFileOpStruct.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
    sShFileOpStruct.pFrom  = aSource;
    sShFileOpStruct.pTo    = aTarget;
    ::SHFileOperation(&sShFileOpStruct);
}

void SHSilentMoveFile(const xpr_tchar_t *aSource, const xpr_tchar_t *aTarget)
{
    if (XPR_IS_NULL(aSource) || XPR_IS_NULL(aTarget))
        return;

    SHFILEOPSTRUCT sShFileOpStruct = {0};
    sShFileOpStruct.hwnd   = XPR_NULL;
    sShFileOpStruct.wFunc  = FO_MOVE;
    sShFileOpStruct.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;
    sShFileOpStruct.pFrom  = aSource;
    sShFileOpStruct.pTo    = aTarget;
    ::SHFileOperation(&sShFileOpStruct);
}

xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile, const xpr_tchar_t *aTarget)
{
    if (XPR_IS_NULL(aLinkFile) || XPR_IS_NULL(aTarget))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    IShellLink *sShellLink = XPR_NULL;
    IPersistFile *sPersistFile = XPR_NULL;
    HRESULT sHResult;

    sHResult = ::CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink));

    if (SUCCEEDED(sHResult))
    {
        sShellLink->SetPath(aTarget);

        sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile));
        if (SUCCEEDED(sHResult))
        {
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

            sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
            sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sPersistFile->Save(sWideLinkFile, XPR_TRUE);
            sResult = XPR_TRUE;
        }
    }

    COM_RELEASE(sShellLink);
    COM_RELEASE(sPersistFile);

    return sResult;
}

xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile, LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aLinkFile) || XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    IShellLink *sShellLink = XPR_NULL;
    IPersistFile *sPersistFile = XPR_NULL;
    HRESULT sHResult;

    sHResult = ::CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink));

    if (SUCCEEDED(sHResult))
    {
        sShellLink->SetIDList(aPidl);

        sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile));
        if (SUCCEEDED(sHResult))
        {
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

            sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
            sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sHResult = sPersistFile->Save(sWideLinkFile, XPR_TRUE);
            if (SUCCEEDED(sHResult))
                sResult = XPR_TRUE;
        }
    }

    COM_RELEASE(sShellLink);
    COM_RELEASE(sPersistFile);

    return sResult;
}

xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile,
                            LPCITEMIDLIST      aPidl,
                            const xpr_tchar_t *aStartup,
                            WORD               aHotkey,
                            xpr_sint_t         aShowCmd,
                            const xpr_tchar_t *aDesc)
{
    if (XPR_IS_NULL(aLinkFile) || XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    IShellLink *sShellLink = XPR_NULL;
    IPersistFile *sPersistFile = XPR_NULL;
    HRESULT sHResult;

    sHResult = CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink));

    if (SUCCEEDED(sHResult))
    {
        sShellLink->SetIDList(aPidl);
        sShellLink->SetWorkingDirectory(aStartup);
        sShellLink->SetHotkey(aHotkey);
        sShellLink->SetShowCmd(aShowCmd);
        sShellLink->SetDescription(aDesc);

        sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile));
        if (SUCCEEDED(sHResult))
        {
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

            sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
            sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sHResult = sPersistFile->Save(sWideLinkFile, XPR_TRUE);
            if (SUCCEEDED(sHResult))
                sResult = XPR_TRUE;
        }
    }

    COM_RELEASE(sShellLink);
    COM_RELEASE(sPersistFile);

    return sResult;
}

xpr_bool_t CreateShortcut(const xpr_tchar_t *aLinkFile,
                            const xpr_tchar_t *aTarget,
                            const xpr_tchar_t *aStartup,
                            WORD               aHotkey,
                            xpr_sint_t         aShowCmd,
                            const xpr_tchar_t *aDesc)
{
    if (XPR_IS_NULL(aLinkFile) || XPR_IS_NULL(aTarget))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    IShellLink* sShellLink = XPR_NULL;
    IPersistFile* sPersistFile = XPR_NULL;
    HRESULT sHResult;

    sHResult = CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink));

    if (SUCCEEDED(sHResult))
    {
        sShellLink->SetPath(aTarget);
        sShellLink->SetWorkingDirectory(aStartup);
        sShellLink->SetHotkey(aHotkey);
        sShellLink->SetShowCmd(aShowCmd);
        sShellLink->SetDescription(aDesc);

        sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile));
        if (SUCCEEDED(sHResult))
        {
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;
            xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

            sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
            XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
            sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

            sHResult = sPersistFile->Save(sWideLinkFile, XPR_TRUE);
            if (SUCCEEDED(sHResult))
                sResult = XPR_TRUE;
        }
    }

    COM_RELEASE(sShellLink);
    COM_RELEASE(sPersistFile);

    return sResult;
}

xpr_bool_t ResolveShortcut(HWND         aHwnd,
                           xpr_tchar_t *aLinkFile,
                           xpr_tchar_t *aPath,
                           xpr_tchar_t *aParameter,
                           xpr_tchar_t *aStartup,
                           xpr_tchar_t *aIcon,
                           xpr_sint_t  *aIconIndex)
{
    IShellLink *sShellLink = XPR_NULL;
    IPersistFile *sPersistFile = XPR_NULL;

    HRESULT sHResult = ::CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink));

    if (FAILED(sHResult) || XPR_IS_NULL(sShellLink))
        return XPR_FALSE;

    sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile)); 
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sPersistFile))
    {
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;
        xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

        sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
        sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

        sHResult = sPersistFile->Load(sWideLinkFile, STGM_READ);
        if (SUCCEEDED(sHResult))
        {
            sHResult = sShellLink->Resolve(aHwnd, SLR_NO_UI);
            if (SUCCEEDED(sHResult))
            {
                sHResult = sShellLink->GetPath(aPath, XPR_MAX_PATH, XPR_NULL, SLGP_UNCPRIORITY);

                if (XPR_IS_NOT_NULL(aParameter))
                    sHResult = sShellLink->GetArguments(aParameter, XPR_MAX_PATH);

                if (XPR_IS_NOT_NULL(aStartup))
                    sHResult = sShellLink->GetWorkingDirectory(aStartup, XPR_MAX_PATH);

                if (XPR_IS_NOT_NULL(aIcon) && XPR_IS_NOT_NULL(aIconIndex))
                    sHResult = sShellLink->GetIconLocation(aIcon, XPR_MAX_PATH, aIconIndex);
            }
        } 
    }

    COM_RELEASE(sPersistFile);
    COM_RELEASE(sShellLink);

    return SUCCEEDED(sHResult) ? XPR_TRUE : XPR_FALSE;
} 

xpr_bool_t ResolveShortcut(HWND          aHwnd,
                             xpr_tchar_t  *aLinkFile,
                             LPITEMIDLIST *aFullPidl,
                             xpr_tchar_t  *aParameter,
                             xpr_tchar_t  *aStartup,
                             xpr_tchar_t  *aIcon,
                             xpr_sint_t   *aIconIndex)
{
    IShellLink *sShellLink = XPR_NULL;
    IPersistFile *sPersistFile = XPR_NULL;

    HRESULT sHResult = ::CoCreateInstance(
        CLSID_ShellLink,
        XPR_NULL,
        CLSCTX_INPROC_SERVER,
        IID_IShellLink,
        reinterpret_cast<LPVOID *>(&sShellLink)); 

    if (FAILED(sHResult) || !sShellLink)
        return XPR_FALSE;

    sHResult = sShellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&sPersistFile)); 
    if (SUCCEEDED(sHResult) && sPersistFile)
    {
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;
        xpr_wchar_t sWideLinkFile[XPR_MAX_PATH + 1];

        sInputBytes = _tcslen(aLinkFile) * sizeof(xpr_tchar_t);
        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
        XPR_TCS_TO_UTF16(aLinkFile, &sInputBytes, sWideLinkFile, &sOutputBytes);
        sWideLinkFile[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

        sHResult = sPersistFile->Load(sWideLinkFile, STGM_READ);
        if (SUCCEEDED(sHResult))
        {
            sHResult = sShellLink->Resolve(aHwnd, SLR_NO_UI);
            if (SUCCEEDED(sHResult))
            {
                sHResult = sShellLink->GetIDList(aFullPidl);

                if (XPR_IS_NOT_NULL(aParameter))
                    sHResult = sShellLink->GetArguments(aParameter, XPR_MAX_PATH);

                if (XPR_IS_NOT_NULL(aStartup))
                    sHResult = sShellLink->GetWorkingDirectory(aStartup, XPR_MAX_PATH);

                if (XPR_IS_NOT_NULL(aIcon) && XPR_IS_NOT_NULL(aIconIndex))
                    sHResult = sShellLink->GetIconLocation(aIcon, XPR_MAX_PATH, aIconIndex);
            }
        } 
    }

    COM_RELEASE(sPersistFile);
    COM_RELEASE(sShellLink);

    return SUCCEEDED(sHResult) ? XPR_TRUE : XPR_FALSE;
}
} // namespace fxfile
