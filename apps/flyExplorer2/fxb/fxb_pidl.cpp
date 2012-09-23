//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_pidl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static xpr_char_t THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
typedef WINSHELLAPI xpr_bool_t (WINAPI *ILIsParentFunc)(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl, xpr_bool_t aImmediate);
typedef WINSHELLAPI xpr_bool_t (WINAPI *ILIsEqualFunc)(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILFindLastIDFunc)(LPCITEMIDLIST aPidl);
typedef WINSHELLAPI xpr_bool_t (WINAPI *ILRemoveLastIDFunc)(LPITEMIDLIST aPidl);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILCloneFunc)(LPCITEMIDLIST aPidl);
typedef WINSHELLAPI LPITEMIDLIST (WINAPI *ILFindChildFunc)(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);

LPITEMIDLIST Next(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_NULL;

    xpr_sint_t sSize = aPidl->mkid.cb;
    if (sSize == 0)
        return XPR_NULL;

    aPidl = (LPITEMIDLIST) (((xpr_byte_t *) aPidl) + sSize);
    return (aPidl->mkid.cb == 0) ? XPR_NULL : (LPITEMIDLIST) aPidl;
}

xpr_uint_t GetSize(LPCITEMIDLIST aPidl)
{
    xpr_uint_t sSize = 0;
    if (XPR_IS_NOT_NULL(aPidl))
    {
        sSize += sizeof(aPidl->mkid.cb);    // Null terminator
        while (XPR_IS_NOT_NULL(aPidl))
        {
            sSize += aPidl->mkid.cb;
            aPidl = Next(aPidl);
        }
    }

    return sSize;
}

LPITEMIDLIST CreatePidl(xpr_uint_t aSize)
{
    LPITEMIDLIST sPidl = (LPITEMIDLIST)::CoTaskMemAlloc(aSize);
    if (XPR_IS_NOT_NULL(sPidl))
        memset(sPidl, 0, aSize);      // zero-init for external task   alloc

    return sPidl;
}

LPITEMIDLIST ConcatPidls(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    LPITEMIDLIST sNewPidl;
    xpr_uint_t sSize1;
    xpr_uint_t sSize2;

    if (XPR_IS_NOT_NULL(aPidl1))  //May be XPR_NULL
        sSize1 = GetSize(aPidl1) - sizeof(aPidl1->mkid.cb);
    else
        sSize1 = 0;

    sSize2 = GetSize(aPidl2);

    sNewPidl = CreatePidl(sSize1 + sSize2);
    if (XPR_IS_NOT_NULL(sNewPidl))
    {
        if (XPR_IS_NOT_NULL(aPidl1))
            memcpy(sNewPidl, aPidl1, sSize1);

        memcpy(((xpr_char_t *)sNewPidl) + sSize1, aPidl2, sSize2);
    }

    return sNewPidl;
}

LPITEMIDLIST CopyItemIDList(LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_NULL;

    // Calculate size of list.
    xpr_uint_t sSize = GetSize(aPidl);

    LPITEMIDLIST aNewPidl = (LPITEMIDLIST)::CoTaskMemAlloc(sSize);
    if (XPR_IS_NOT_NULL(aNewPidl))
        memcpy(aNewPidl, aPidl, sSize);

    return aNewPidl;
}

xpr_sint_t GetItemIDCount(LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return 0;

    xpr_sint_t i;
    xpr_sint_t sCount = 0;

    for (i = 0; ; ++i)
    {
        if (aPidl->mkid.cb == 0)
            break;

        aPidl = (LPITEMIDLIST)((xpr_byte_t *)aPidl + aPidl->mkid.cb);

        ++sCount;
    }

    return sCount;
}

xpr_sint_t CompareItemID(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    if (aPidl1 == aPidl2)
        return 0;

    if (XPR_IS_NULL(aPidl1) || XPR_IS_NULL(aPidl2))
        return -1;

    if (GetSize(aPidl1) != GetSize(aPidl2))
        return -1;

    xpr_sint_t sResult = memcmp(aPidl1, aPidl2, GetSize(aPidl1));
    return sResult;
}

xpr_sint_t CompareItemID(LPCITEMIDLIST aPidl1, xpr_sint_t aSpecialFolder)
{
    if (XPR_IS_NULL(aPidl1))
        return -1;

    LPITEMIDLIST sPidl2 = XPR_NULL;
    HRESULT sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, aSpecialFolder, &sPidl2);
    if (FAILED(sHResult) || XPR_IS_NULL(sPidl2))
        return -1;

    if (GetSize(aPidl1) != GetSize(sPidl2))
        return -1;

    xpr_sint_t sResult = memcmp(aPidl1, sPidl2, GetSize(aPidl1));

    COM_FREE(sPidl2);

    return sResult;
}

//-----------------------------------------------------------------------------
// directly called function in Shell32.dll
xpr_bool_t IL_IsEqual(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2)
{
    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILIsEqualFunc sILIsEqualFunc = (ILIsEqualFunc)::GetProcAddress(sDll, (const xpr_char_t*)21);
        if (XPR_IS_NOT_NULL(sILIsEqualFunc))
            sResult = sILIsEqualFunc(aPidl1, aPidl2);

        ::FreeLibrary(sDll);
    }

    return sResult;
}

xpr_bool_t IL_IsParent(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl)
{
    if (XPR_IS_NULL(aParentPidl) || XPR_IS_NULL(aChildPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILIsParentFunc sILIsParentFunc = (ILIsParentFunc)::GetProcAddress(sDll, (const xpr_char_t*)23);
        if (XPR_IS_NOT_NULL(sILIsParentFunc))
            sResult = sILIsParentFunc(aParentPidl, aChildPidl, XPR_TRUE);

        ::FreeLibrary(sDll);
    }

    return sResult;
}

LPITEMIDLIST IL_Clone(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_NULL;

    LPITEMIDLIST sNewPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILCloneFunc sILCloneFunc = (ILCloneFunc)::GetProcAddress(sDll, (const xpr_char_t*)18);
        if (XPR_IS_NOT_NULL(sILCloneFunc))
            sNewPidl = sILCloneFunc(aPidl);

        ::FreeLibrary(sDll);
    }

    return sNewPidl;
}

LPITEMIDLIST IL_FindLastID(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_NULL;

    LPITEMIDLIST sLastPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILFindLastIDFunc sILFindLastIDFunc = (ILFindLastIDFunc)::GetProcAddress(sDll, (const xpr_char_t*)16);
        if (XPR_IS_NOT_NULL(sILFindLastIDFunc))
            sLastPidl = sILFindLastIDFunc(aPidl);

        ::FreeLibrary(sDll);
    }

    return sLastPidl;
}

xpr_bool_t IL_RemoveLastID(LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILRemoveLastIDFunc sILRemoveLastIDFunc = (ILRemoveLastIDFunc)::GetProcAddress(sDll, (const xpr_char_t*)17);
        if (XPR_IS_NOT_NULL(sILRemoveLastIDFunc))
            sResult = sILRemoveLastIDFunc(aPidl);

        ::FreeLibrary(sDll);
    }

    return sResult;
}

LPITEMIDLIST IL_FindChild(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl)
{
    if (XPR_IS_NULL(aParentPidl) || XPR_IS_NULL(aChildPidl))
        return XPR_NULL;

    LPITEMIDLIST sChildPidl = XPR_NULL;

    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        ILFindChildFunc sILFindChildFunc = (ILFindChildFunc)::GetProcAddress(sDll, (const xpr_char_t*)24);
        if (XPR_IS_NOT_NULL(sILFindChildFunc))
            sChildPidl = sILFindChildFunc(aParentPidl, aChildPidl);

        ::FreeLibrary(sDll);
    }

    return sChildPidl;
}

xpr_bool_t IsPidlDesktop(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_TRUE;

    return (aPidl->mkid.cb != 0) ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t IsPidlSimple(LPCITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_TRUE;

    if (IsPidlDesktop(aPidl) == XPR_FALSE) // aPidl == XPR_NULL || mkid.cb == 0
    {
        xpr_ushort_t sSize = aPidl->mkid.cb;
        LPCITEMIDLIST sNextPidl = (LPCITEMIDLIST)(((xpr_byte_t *)aPidl) + sSize);
        if (sNextPidl->mkid.cb > 0)
            sResult = XPR_FALSE;
    }

    return sResult;
}
} // namespace fxb
