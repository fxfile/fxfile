//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
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
#undef THIS_FILE
static xpr_char_t THIS_FILE[] = __FILE__;
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

    if (Pidl::getSize(aPidl1) != Pidl::getSize(aPidl2))
    {
        return -1;
    }

    xpr_sint_t sResult = memcmp(aPidl1, aPidl2, Pidl::getSize(aPidl1));

    return sResult;
}

xpr_sint_t Pidl::compare(LPCITEMIDLIST aPidl, xpr_sint_t aSpecialFolder)
{
    if (XPR_IS_NULL(aPidl))
    {
        return -1;
    }

    xpr_sint_t   sResult = 0;
    HRESULT      sHResult;
    LPITEMIDLIST sSpecialPidl = XPR_NULL;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, aSpecialFolder, &sSpecialPidl);
    if (FAILED(sHResult) || XPR_IS_NULL(sSpecialPidl))
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
    if (XPR_IS_FALSE(Pidl::isDesktopFolder(aPidl)))
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

LPITEMIDLIST Pidl::create(xpr_sint_t aSpecialFolder)
{
    HRESULT      sHResult;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, aSpecialFolder, &sFullPidl);
    if (FAILED(sHResult) || XPR_IS_NULL(sFullPidl))
    {
        XPR_ASSERT(sFullPidl == XPR_NULL);

        COM_FREE(sFullPidl);
    }

    return sFullPidl;
}

LPITEMIDLIST Pidl::create(const KNOWNFOLDERID &aKnownFolderId)
{
    HRESULT      sHResult;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        SHGetKnownFolderIDListFunc sSHGetKnownFolderIDListFunc = (SHGetKnownFolderIDListFunc)::GetProcAddress(sDll, (const xpr_char_t *)380);
        if (XPR_IS_NOT_NULL(sSHGetKnownFolderIDListFunc))
        {
            sHResult = sSHGetKnownFolderIDListFunc(aKnownFolderId, 0, XPR_NULL, &sFullPidl);
            if (FAILED(sHResult) || XPR_IS_NULL(sFullPidl))
            {
                XPR_ASSERT(sFullPidl == XPR_NULL);

                COM_FREE(sFullPidl);
            }
        }

        ::FreeLibrary(sDll);
    }

    return sFullPidl;
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
    HRESULT       sHResult;

    sHResult = aShellFolder->GetUIObjectOf(0, 1, &aSimplePidl, IID_IDataObject, 0, (LPVOID *)&sDataObject);
    if (SUCCEEDED(sHResult))
    {
        FORMATETC sFormatEtc = {0};
        sFormatEtc.cfFormat = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
        sFormatEtc.ptd      = XPR_NULL;
        sFormatEtc.dwAspect = DVASPECT_CONTENT;
        sFormatEtc.lindex   = -1;
        sFormatEtc.tymed    = TYMED_HGLOBAL;

        STGMEDIUM sStgMedium;
        sHResult = sDataObject->GetData(&sFormatEtc, &sStgMedium);

        if (SUCCEEDED(sHResult))
        {
            LPIDA sPida = (LPIDA)::GlobalLock(sStgMedium.hGlobal);

            LPCITEMIDLIST sFolderPidl = (LPCITEMIDLIST)((xpr_byte_t *)sPida + sPida->aoffset[0]);
            LPCITEMIDLIST sChildPidl  = (LPCITEMIDLIST)((xpr_byte_t *)sPida + sPida->aoffset[1]);

            sFullPidl = concat(sFolderPidl, sChildPidl);

            ::GlobalUnlock(sStgMedium.hGlobal);
            ::GlobalFree(sStgMedium.hGlobal);
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
    HRESULT       sHResult     = E_FAIL;

    if (Pidl::isSimplePidl(aFullPidl) == XPR_TRUE)
    {
        LPITEMIDLIST sChildPidl;

        sChildPidl = Pidl::findLastItem(aFullPidl);

        sHResult = ::SHGetDesktopFolder((LPSHELLFOLDER *)&sShellFolder);
        if (SUCCEEDED(sHResult))
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
            sHResult = ::SHGetDesktopFolder(&sDesktopShellFolder);
            if (SUCCEEDED(sHResult))
            {
                sHResult = sDesktopShellFolder->BindToObject(
                    sParentPidl,
                    XPR_NULL,
                    IID_IShellFolder,
                    (LPVOID *)&sShellFolder);

                if (SUCCEEDED(sHResult))
                {
                    sSimplePidl = sChildPidl;
                }
            }
        }

        COM_FREE(sParentPidl);
        COM_RELEASE(sDesktopShellFolder);
    }

    if (XPR_IS_NULL(aSimplePidl))
    {
        COM_RELEASE(sShellFolder);

        return XPR_FALSE;
    }

    if (XPR_IS_NULL(sShellFolder))
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}
} // namespace base
} // namespace fxfile
