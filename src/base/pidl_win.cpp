//
// Copyright (c) 2013-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "pidl_win.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace base
{
typedef WINSHELLAPI xpr_bool_t   (WINAPI *ILIsParentFunc            )(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl, xpr_bool_t aImmediate);
typedef WINSHELLAPI xpr_bool_t   (WINAPI *ILIsEqualFunc             )(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILFindLastIDFunc          )(LPCITEMIDLIST aPidl);
typedef WINSHELLAPI xpr_bool_t   (WINAPI *ILRemoveLastIDFunc        )(LPITEMIDLIST aPidl);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILCloneFunc               )(LPCITEMIDLIST aPidl);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILFindChildFunc           )(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILCreateFromPathFunc      )(const xpr_char_t *aPath);
typedef WINSHELLAPI HRESULT      (WINAPI *SHGetKnownFolderIDListFunc)(REFKNOWNFOLDERID aRfid, DWORD aFlags, HANDLE aToken, LPITEMIDLIST *aPidl);

LPITEMIDLIST Pidl::alloc(xpr_size_t aSize)
{
    LPITEMIDLIST sPidl = (LPITEMIDLIST)::CoTaskMemAlloc(aSize);

    if (XPR_IS_NOT_NULL(sPidl))
    {
        memset(sPidl, 0, aSize);
    }

    return sPidl;
}

void Pidl::free(LPITEMIDLIST aPidl)
{
    ::CoTaskMemFree(aPidl);
}

LPITEMIDLIST Pidl::clone(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return XPR_NULL;
    }

    xpr_size_t sSize = Pidl::getSize(aPidl);

    LPITEMIDLIST sNewPidl = Pidl::alloc(sSize);

    if (XPR_IS_NOT_NULL(sNewPidl))
    {
        memcpy(sNewPidl, aPidl, sSize);
    }

    return sNewPidl;
}

LPITEMIDLIST Pidl::concat(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    LPITEMIDLIST sNewPidl;
    xpr_size_t   sSize1;
    xpr_size_t   sSize2;

    if (XPR_IS_NOT_NULL(aPidl1)) // may be XPR_NULL
    {
        sSize1 = Pidl::getSize(aPidl1) - sizeof(aPidl1->mkid.cb);
    }
    else
    {
        sSize1 = 0;
    }

    sSize2 = Pidl::getSize(aPidl2);

    sNewPidl = Pidl::alloc(sSize1 + sSize2);
    if (XPR_IS_NOT_NULL(sNewPidl))
    {
        if (XPR_IS_NOT_NULL(aPidl1))
        {
            memcpy(sNewPidl, aPidl1, sSize1);
        }

        memcpy(((xpr_byte_t *)sNewPidl) + sSize1, aPidl2, sSize2);
    }

    return sNewPidl;
}

xpr_size_t Pidl::getSize(LPCITEMIDLIST aPidl)
{
    xpr_size_t sSize = 0;

    if (XPR_IS_NOT_NULL(aPidl))
    {
        sSize += sizeof(aPidl->mkid.cb); // NULL terminator

        while (XPR_IS_NOT_NULL(aPidl))
        {
            sSize += aPidl->mkid.cb;

            aPidl = Pidl::next(aPidl);
        }
    }

    return sSize;
}

LPITEMIDLIST Pidl::next(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return XPR_NULL;
    }

    xpr_sint_t sSize = aPidl->mkid.cb;
    if (sSize == 0)
    {
        return XPR_NULL;
    }

    LPCITEMIDLIST sPidl = aPidl;

    sPidl = (LPITEMIDLIST)(((xpr_byte_t *)sPidl) + sSize);

    return (sPidl->mkid.cb == 0) ? XPR_NULL : (LPITEMIDLIST)sPidl;
}

xpr_sint_t Pidl::getItemCount(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return 0;
    }

    xpr_sint_t    i;
    xpr_sint_t    sCount = 0;
    LPCITEMIDLIST sPidl  = aPidl;

    for (i = 0; ; ++i)
    {
        if (sPidl->mkid.cb == 0)
        {
            break;
        }

        sPidl = (LPITEMIDLIST)((xpr_byte_t *)sPidl + sPidl->mkid.cb);

        ++sCount;
    }

    return sCount;
}

xpr_sint_t Pidl::compare(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    if (aPidl1 == aPidl2)
    {
        return 0;
    }

    if (XPR_IS_NULL(aPidl1) || XPR_IS_NULL(aPidl2))
    {
        return -1;
    }

    xpr_size_t sSize1 = Pidl::getSize(aPidl1);
    xpr_size_t sSize2 = Pidl::getSize(aPidl2);

    if (sSize1 != sSize2)
    {
        return -1;
    }

    xpr_sint_t sResult = memcmp(aPidl1, aPidl2, sSize1);

    return sResult;
}

xpr_sint_t Pidl::compare(LPCITEMIDLIST aPidl, xpr_sint_t aSpecialFolder)
{
    if (XPR_IS_NULL(aPidl))
    {
        return -1;
    }

    xpr_sint_t   sResult = 0;
    HRESULT      sComResult;
    LPITEMIDLIST sSpecialPidl = XPR_NULL;

    sComResult = ::SHGetSpecialFolderLocation(XPR_NULL, aSpecialFolder, &sSpecialPidl);
    if (FAILED(sComResult) || XPR_IS_NULL(sSpecialPidl))
    {
        sResult = -1;
    }
    else
    {
        if (Pidl::getSize(aPidl) != Pidl::getSize(sSpecialPidl))
        {
            sResult = -1;
        }
        else
        {
            sResult = memcmp(aPidl, sSpecialPidl, Pidl::getSize(aPidl));
        }
    }

    COM_FREE(sSpecialPidl);

    return sResult;
}

xpr_bool_t Pidl::isEqual(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILIsEqualFunc sILIsEqualFunc = (ILIsEqualFunc)::GetProcAddress(sDll, (const xpr_char_t *)21);
        if (XPR_IS_NOT_NULL(sILIsEqualFunc))
        {
            sResult = sILIsEqualFunc(aPidl1, aPidl2);
        }

        ::FreeLibrary(sDll);
    }

    return sResult;
}

xpr_bool_t Pidl::isParent(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl)
{
    if (XPR_IS_NULL(aParentPidl) || XPR_IS_NULL(aChildPidl))
    {
        return XPR_FALSE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILIsParentFunc sILIsParentFunc = (ILIsParentFunc)::GetProcAddress(sDll, (const xpr_char_t*)23);
        if (XPR_IS_NOT_NULL(sILIsParentFunc))
        {
            sResult = sILIsParentFunc(aParentPidl, aChildPidl, XPR_TRUE);
        }

        ::FreeLibrary(sDll);
    }

    return sResult;
}

LPITEMIDLIST Pidl::findLastItem(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return XPR_NULL;
    }

    LPITEMIDLIST sLastPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILFindLastIDFunc sILFindLastIDFunc = (ILFindLastIDFunc)::GetProcAddress(sDll, (const xpr_char_t*)16);
        if (XPR_IS_NOT_NULL(sILFindLastIDFunc))
        {
            sLastPidl = sILFindLastIDFunc(aPidl);
        }

        ::FreeLibrary(sDll);
    }

    return sLastPidl;
}

xpr_bool_t Pidl::removeLastItem(LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return XPR_FALSE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILRemoveLastIDFunc sILRemoveLastIDFunc = (ILRemoveLastIDFunc)::GetProcAddress(sDll, (const xpr_char_t*)17);
        if (XPR_IS_NOT_NULL(sILRemoveLastIDFunc))
        {
            sResult = sILRemoveLastIDFunc(aPidl);
        }

        ::FreeLibrary(sDll);
    }

    return sResult;
}

LPITEMIDLIST Pidl::findChildItem(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl)
{
    if (XPR_IS_NULL(aParentPidl) || XPR_IS_NULL(aChildPidl))
    {
        return XPR_NULL;
    }

    LPITEMIDLIST sChildPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILFindChildFunc sILFindChildFunc = (ILFindChildFunc)::GetProcAddress(sDll, (const xpr_char_t*)24);
        if (XPR_IS_NOT_NULL(sILFindChildFunc))
        {
            sChildPidl = sILFindChildFunc(aParentPidl, aChildPidl);
        }

        ::FreeLibrary(sDll);
    }

    return sChildPidl;
}

xpr_bool_t Pidl::isDesktopFolder(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
    {
        return XPR_TRUE;
    }

    return (aPidl->mkid.cb != 0) ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t Pidl::isSimplePidl(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_TRUE(Pidl::isDesktopFolder(aPidl)))
    {
        return XPR_TRUE;
    }

    xpr_bool_t sIsSimplePidl = XPR_TRUE;

    xpr_ushort_t sSize = aPidl->mkid.cb;

    LPCITEMIDLIST sNextPidl = (LPCITEMIDLIST)(((xpr_byte_t *)aPidl) + sSize);
    if (sNextPidl->mkid.cb > 0)
    {
        sIsSimplePidl = XPR_FALSE;
    }

    return sIsSimplePidl;
}

LPITEMIDLIST Pidl::create(const xpr_tchar_t *aPath)
{
    LPITEMIDLIST sPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILCreateFromPathFunc sILCreateFromPathFunc = (ILCreateFromPathFunc)::GetProcAddress(sDll, (const xpr_char_t*)157);
        if (XPR_IS_NOT_NULL(sILCreateFromPathFunc))
        {
#ifdef XPR_CFG_UNICODE
            sPidl = sILCreateFromPathFunc((const xpr_char_t *)aPath);
#else
            if (xpr::getOsVer() >= xpr::kOsVerWinNT)
            {
                xpr_wchar_t sWidePath[XPR_MAX_PATH + 1];
                xpr_size_t sInputBytes;
                xpr_size_t sOutputBytes;

                sInputBytes = _tcslen(aPath) * sizeof(xpr_tchar_t);
                sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
                XPR_TCS_TO_UTF16(aPath, &sInputBytes, sWidePath, &sOutputBytes);
                sWidePath[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

                sPidl = sILCreateFromPathFunc((const xpr_char_t *)sWidePath);
            }
            else
            {
                sPidl = sILCreateFromPathFunc((const xpr_char_t *)aPath);
            }
#endif
        }

        ::FreeLibrary(sDll);
    }

    return sPidl;
}

LPITEMIDLIST Pidl::create(const xpr::string &aPath)
{
    const xpr_tchar_t *sPath = aPath.c_str();

    return create(sPath);
}

LPITEMIDLIST Pidl::create(xpr_sint_t aSpecialFolder)
{
    HRESULT      sComResult;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    sComResult = ::SHGetSpecialFolderLocation(XPR_NULL, aSpecialFolder, &sFullPidl);
    if (FAILED(sComResult) || XPR_IS_NULL(sFullPidl))
    {
        XPR_ASSERT(sFullPidl == XPR_NULL);

        COM_FREE(sFullPidl);
    }

    return sFullPidl;
}

LPITEMIDLIST Pidl::create(const KNOWNFOLDERID &aKnownFolderId)
{
    HRESULT      sComResult;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        SHGetKnownFolderIDListFunc sSHGetKnownFolderIDListFunc = (SHGetKnownFolderIDListFunc)::GetProcAddress(sDll, (const xpr_char_t *)XPR_MBCS_STRING_LITERAL("SHGetKnownFolderIDList"));
        if (XPR_IS_NOT_NULL(sSHGetKnownFolderIDListFunc))
        {
            sComResult = sSHGetKnownFolderIDListFunc(aKnownFolderId, 0, XPR_NULL, &sFullPidl);
            if (FAILED(sComResult) || XPR_IS_NULL(sFullPidl))
            {
                XPR_ASSERT(sFullPidl == XPR_NULL);

                COM_FREE(sFullPidl);
            }
        }

        ::FreeLibrary(sDll);
    }

    return sFullPidl;
}

HRESULT Pidl::create(const xpr_tchar_t *aPath, LPITEMIDLIST &aFullPidl)
{
    LPITEMIDLIST sFullPidl = create(aPath);
    if (XPR_IS_NULL(sFullPidl))
    {
        return E_FAIL;
    }

    aFullPidl = sFullPidl;

    return S_OK;
}

HRESULT Pidl::create(const xpr::string &aPath, LPITEMIDLIST &aFullPidl)
{
    LPITEMIDLIST sFullPidl = create(aPath);
    if (XPR_IS_NULL(sFullPidl))
    {
        return E_FAIL;
    }

    aFullPidl = sFullPidl;

    return S_OK;
}

HRESULT Pidl::create(xpr_sint_t aSpecialFolder, LPITEMIDLIST &aFullPidl)
{
    LPITEMIDLIST sFullPidl = create(aSpecialFolder);
    if (XPR_IS_NULL(sFullPidl))
    {
        return E_FAIL;
    }

    aFullPidl = sFullPidl;

    return S_OK;
}

HRESULT Pidl::create(const KNOWNFOLDERID &aKnownFolderId, LPITEMIDLIST &aFullPidl)
{
    LPITEMIDLIST sFullPidl = create(aKnownFolderId);
    if (XPR_IS_NULL(sFullPidl))
    {
        return E_FAIL;
    }

    aFullPidl = sFullPidl;

    return S_OK;
}

// IShellFolder + PIDL -> full qualified PIDL
LPITEMIDLIST Pidl::getFullPidl(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aSimplePidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aSimplePidl))
    {
        return XPR_NULL;
    }

    LPITEMIDLIST  sFullPidl   = XPR_NULL;
    IDataObject  *sDataObject = XPR_NULL;
    HRESULT       sComResult;

    sComResult = aShellFolder->GetUIObjectOf(0, 1, &aSimplePidl, IID_IDataObject, 0, (LPVOID *)&sDataObject);
    if (SUCCEEDED(sComResult))
    {
        FORMATETC sFormatEtc = {0};
        sFormatEtc.cfFormat = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
        sFormatEtc.ptd      = XPR_NULL;
        sFormatEtc.dwAspect = DVASPECT_CONTENT;
        sFormatEtc.lindex   = -1;
        sFormatEtc.tymed    = TYMED_HGLOBAL;

        STGMEDIUM sStgMedium = {0,};
        sComResult = sDataObject->GetData(&sFormatEtc, &sStgMedium);

        if (SUCCEEDED(sComResult))
        {
            LPIDA sPida = (LPIDA)::GlobalLock(sStgMedium.hGlobal);

            LPCITEMIDLIST sFolderPidl = (LPCITEMIDLIST)((xpr_byte_t *)sPida + sPida->aoffset[0]);
            LPCITEMIDLIST sChildPidl  = (LPCITEMIDLIST)((xpr_byte_t *)sPida + sPida->aoffset[1]);

            sFullPidl = concat(sFolderPidl, sChildPidl);

            ::GlobalUnlock(sStgMedium.hGlobal);

            if (XPR_IS_NOT_NULL(sStgMedium.pUnkForRelease))
            {
                ::ReleaseStgMedium(&sStgMedium);
            }
        }

        COM_RELEASE(sDataObject);
    }

    return sFullPidl;
}

// full qualified PIDL -> IShellFolder, PIDL
xpr_bool_t Pidl::getSimplePidl(LPCITEMIDLIST aFullPidl, LPSHELLFOLDER &aShellFolder, LPCITEMIDLIST &aSimplePidl)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        return XPR_FALSE;
    }

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST  sSimplePidl  = XPR_NULL;
    HRESULT       sComResult     = E_FAIL;

    if (Pidl::isSimplePidl(aFullPidl) == XPR_TRUE)
    {
        LPITEMIDLIST sChildPidl;

        sChildPidl = Pidl::findLastItem(aFullPidl);

        sComResult = ::SHGetDesktopFolder((LPSHELLFOLDER *)&sShellFolder);
        if (SUCCEEDED(sComResult))
        {
            sSimplePidl = sChildPidl;
        }
    }
    else
    {
        LPSHELLFOLDER sDesktopShellFolder;
        LPITEMIDLIST  sParentPidl;
        LPITEMIDLIST  sChildPidl;

        sChildPidl = Pidl::findLastItem(aFullPidl);

        sParentPidl = Pidl::clone(aFullPidl);

        if (XPR_IS_TRUE(Pidl::removeLastItem(sParentPidl)))
        {
            sComResult = ::SHGetDesktopFolder(&sDesktopShellFolder);
            if (SUCCEEDED(sComResult))
            {
                sComResult = sDesktopShellFolder->BindToObject(
                    sParentPidl,
                    XPR_NULL,
                    IID_IShellFolder,
                    (LPVOID *)&sShellFolder);

                if (SUCCEEDED(sComResult))
                {
                    sSimplePidl = sChildPidl;
                }

                COM_RELEASE(sDesktopShellFolder);
            }
        }

        COM_FREE(sParentPidl);
    }

    if (XPR_IS_NULL(sSimplePidl))
    {
        COM_RELEASE(sShellFolder);

        return XPR_FALSE;
    }

    if (XPR_IS_NULL(sShellFolder))
    {
        return XPR_FALSE;
    }

    aShellFolder = sShellFolder;
    aSimplePidl  = sSimplePidl;

    return XPR_TRUE;
}

// parent IShellFolder, simple PIDL -> IShellFolder
xpr_bool_t Pidl::getShellFolder(LPSHELLFOLDER aParentShellFolder, LPCITEMIDLIST aPidl, LPSHELLFOLDER &aShellFolder)
{
    XPR_ASSERT(aParentShellFolder != XPR_NULL);
    XPR_ASSERT(aPidl != XPR_NULL);

    HRESULT sComResult;

    sComResult = aParentShellFolder->BindToObject(
        (LPCITEMIDLIST)aPidl,
        0,
        IID_IShellFolder,
        reinterpret_cast<LPVOID *>(&aShellFolder));

    if (FAILED(sComResult) || XPR_IS_NULL(aShellFolder))
    {
        sComResult = ::SHGetDesktopFolder(&aShellFolder); // desktop PIDL is null.

        if (FAILED(sComResult))
        {
            return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

// full qualified PIDL -> IShellFolder
xpr_bool_t Pidl::getShellFolder(LPCITEMIDLIST aFullPidl, LPSHELLFOLDER &aShellFolder)
{
    XPR_ASSERT(aFullPidl != XPR_NULL);

    xpr_bool_t    sResult            = XPR_FALSE;
    LPSHELLFOLDER sParentShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl              = XPR_NULL;

    if (getSimplePidl(aFullPidl, sParentShellFolder, sPidl) == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    sResult = getShellFolder(sParentShellFolder, sPidl, aShellFolder);

    COM_RELEASE(sParentShellFolder);

    return sResult;
}

xpr_bool_t Pidl::getName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr_tchar_t *aName, xpr_size_t aMaxLen)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        sResult = getName(sShellFolder, sPidl, aFlags, aName, aMaxLen);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t Pidl::getName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr::string &aName)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        sResult = getName(sShellFolder, sPidl, aFlags, aName);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t Pidl::getName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr_tchar_t *aName, xpr_size_t aMaxLen)
{
    STRRET  sStrRet = {0};
    HRESULT sComResult;

    sComResult = aShellFolder->GetDisplayNameOf(aPidl, aFlags, &sStrRet);
    if (SUCCEEDED(sComResult))
    {
        sComResult = ::StrRetToBuf(&sStrRet, aPidl, aName, (xpr_uint_t)aMaxLen);
        if (SUCCEEDED(sComResult))
        {
            return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

xpr_bool_t Pidl::getName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr::string &aName)
{
    STRRET     sStrRet = {0};
    HRESULT    sComResult;
    xpr_bool_t sResult = XPR_FALSE;

    sComResult = aShellFolder->GetDisplayNameOf(aPidl, aFlags, &sStrRet);
    if (SUCCEEDED(sComResult))
    {
        switch (sStrRet.uType)
        {
        case STRRET_WSTR:
            {
#if defined(XPR_CFG_UNICODE)
                aName = sStrRet.pOleStr;
#else
                xpr_size_t sStringLength = wcslen(sStrRet.pOleStr);

                aName.clear();
                aName.reserve(sStringLength + 1);

                xpr_tchar_t *sName = (xpr_tchar_t *)aName.c_str();

                xpr_size_t sInputBytes = sStringLength * sizeof(xpr_wchar_t);
                xpr_size_t sOutputBytes = sStringLength * sizeof(xpr_tchar_t);
                XPR_UTF16_TO_MBS(sStrRet.pOleStr, &sInputBytes, sName, &sOutputBytes);
                sName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
#endif
                sResult = XPR_TRUE;
                break;
            }

        case STRRET_CSTR:
        case STRRET_OFFSET:
            {
                const xpr_char_t *sString;
                switch (sStrRet.uType)
                {
                case STRRET_CSTR:
                    sString = sStrRet.cStr;
                    break;

                case STRRET_OFFSET:
                    sString = ((const xpr_char_t *)&aPidl->mkid) + sStrRet.uOffset;
                    break;

                default:
                    XPR_ASSERT(0);
                    break;
                }

                xpr_size_t sStringLength = strlen(sString);

                aName.clear();
                aName.reserve(sStringLength + 1);

                xpr_tchar_t *sName = (xpr_tchar_t *)aName.c_str();

                xpr_size_t sInputBytes = sStringLength * sizeof(xpr_char_t);
                xpr_size_t sOutputBytes = sStringLength * sizeof(xpr_tchar_t);
                XPR_MBS_TO_TCS(sStrRet.pOleStr, &sInputBytes, sName, &sOutputBytes);
                sName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                sResult = XPR_TRUE;
                break;
            }

        default:
            {
                sResult = XPR_FALSE;
                break;
            }
        }
    }

    return sResult;
}

static void filterAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aAttributes)
{
    HRESULT         sComResult;
    xpr_tchar_t     sPath[XPR_MAX_PATH + 1] = {0};
    WIN32_FIND_DATA sWin32FindData = {0};

    sComResult = Pidl::getName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath, XPR_MAX_PATH);
    if (FAILED(sComResult))
    {
        return;
    }

    HANDLE sFindFile = ::FindFirstFile(sPath, &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_HIDDEN))
        {
            aAttributes |= SFGAO_GHOSTED;
        }

        if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            aAttributes |= SFGAO_FOLDER;
        }
        else
        {
            aAttributes &= ~SFGAO_FOLDER;
        }

        ::FindClose(sFindFile);
    }
}

xpr_ulong_t Pidl::getAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_ulong_t sAttributes = SFGAO_CAPABILITYMASK | SFGAO_DISPLAYATTRMASK | SFGAO_CONTENTSMASK | 0x7F300000;

    HRESULT sComResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &sAttributes);
    if (FAILED(sComResult))
    {
        return 0;
    }

    filterAttributes(aShellFolder, aPidl, sAttributes);

    return sAttributes;
}

xpr_bool_t Pidl::getAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aAttributes)
{
    HRESULT sComResult = aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &aAttributes);
    if (FAILED(sComResult))
    {
        return XPR_FALSE;
    }

    filterAttributes(aShellFolder, aPidl, aAttributes);

    return XPR_TRUE;
}

xpr_ulong_t Pidl::getAttributes(LPCITEMIDLIST aFullPidl)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        return getAttributes(sShellFolder, sPidl);
    }

    COM_RELEASE(sShellFolder);

    return 0;
}

xpr_bool_t Pidl::getAttributes(LPCITEMIDLIST aFullPidl, xpr_ulong_t &aAttributes)
{
    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = getSimplePidl(aFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult))
    {
        return getAttributes(sShellFolder, sPidl, aAttributes);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t Pidl::hasAttributes(LPCITEMIDLIST aFullPidl, xpr_ulong_t aAttributes)
{
    xpr_ulong_t sAttributes = aAttributes;
    xpr_bool_t  sResult     = XPR_FALSE;

    sResult = getAttributes(aFullPidl, sAttributes);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    sResult = XPR_TEST_BITS(sAttributes, aAttributes);

    return sResult;
}

xpr_bool_t Pidl::hasAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t aAttributes)
{
    xpr_ulong_t sAttributes = aAttributes;
    xpr_bool_t  sResult     = XPR_FALSE;

    sResult = getAttributes(aShellFolder, aPidl, sAttributes);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    sResult = XPR_TEST_BITS(sAttributes, aAttributes);

    return sResult;
}

xpr_bool_t Pidl::getInfotip(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aInfotip, xpr_size_t aMaxLen)
{
    xpr_bool_t  sResult    = XPR_FALSE;
    IQueryInfo *sQueryInfo = XPR_NULL;
    HRESULT     sComResult;

    sComResult = aShellFolder->GetUIObjectOf(
        XPR_NULL,
        1,
        (LPCITEMIDLIST *)&aPidl,
        IID_IQueryInfo,
        0,
        (LPVOID *)&sQueryInfo);

    if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sQueryInfo))
    {
        xpr_wchar_t *sWideInfotip = XPR_NULL;

        sComResult = sQueryInfo->GetInfoTip(SHGDN_INFOLDER, &sWideInfotip);
        if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sWideInfotip))
        {
            xpr_size_t sInfotipLen = wcslen(sWideInfotip);
            if (sInfotipLen != 0 && sInfotipLen < aMaxLen)
            {
                xpr_size_t sInputBytes = sInfotipLen * sizeof(xpr_wchar_t);
                xpr_size_t sOutputBytes = aMaxLen * sizeof(xpr_tchar_t);
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

xpr_bool_t Pidl::getInfotip(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr::string &aInfotip)
{
    xpr_bool_t  sResult    = XPR_FALSE;
    IQueryInfo *sQueryInfo = XPR_NULL;
    HRESULT     sComResult;

    sComResult = aShellFolder->GetUIObjectOf(
        XPR_NULL,
        1,
        (LPCITEMIDLIST *)&aPidl,
        IID_IQueryInfo,
        0,
        (LPVOID *)&sQueryInfo);

    if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sQueryInfo))
    {
        xpr_wchar_t *sWideInfotip = XPR_NULL;

        sComResult = sQueryInfo->GetInfoTip(SHGDN_INFOLDER, &sWideInfotip);
        if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sWideInfotip))
        {
            xpr_size_t sInfotipLen = wcslen(sWideInfotip);
            if (sInfotipLen != 0)
            {
                aInfotip.clear();
                aInfotip.reserve(sInfotipLen + 1);

                xpr_tchar_t *sInfotip = (xpr_tchar_t *)aInfotip.c_str();

                xpr_size_t sInputBytes = sInfotipLen * sizeof(xpr_wchar_t);
                xpr_size_t sOutputBytes = sInfotipLen * sizeof(xpr_tchar_t);
                XPR_UTF16_TO_TCS(sWideInfotip, &sInputBytes, sInfotip, &sOutputBytes);
                sInfotip[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                aInfotip.update();

                sResult = XPR_TRUE;
            }
        }

        COM_FREE(sWideInfotip);
    }

    COM_RELEASE(sQueryInfo);

    return sResult;
}
} // namespace base
} // namespace fxfile
