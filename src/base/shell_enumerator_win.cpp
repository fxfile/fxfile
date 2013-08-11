//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "shell_enumerator.h"
#include "shell_item.h"
#include "pidl_win.h"

namespace fxfile
{
namespace base
{
ShellEnumerator::ShellEnumerator(void)
    : mEnumIdList(XPR_NULL)
{
}

ShellEnumerator::~ShellEnumerator(void)
{
    COM_RELEASE(mEnumIdList);
}

xpr_bool_t ShellEnumerator::enumerate(HWND aHwnd, LPSHELLFOLDER aShellFolder, xpr_sint_t aListType, xpr_sint_t aAttributes)
{
    DWORD   sFlags = 0;
    HRESULT sComResult;

    switch (aListType)
    {
    case ListTypeAll:        sFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS; break;
    case ListTypeOnlyFile:   sFlags = SHCONTF_NONFOLDERS;                   break;
    case ListTypeOnlyFolder: sFlags = SHCONTF_FOLDERS;                      break;
    }

    if (XPR_TEST_BITS(aAttributes, AttributeHidden))
    {
        sFlags |= SHCONTF_INCLUDEHIDDEN;
    }

    if (XPR_TEST_BITS(aAttributes, AttributeSystem))
    {
        sFlags |= SHCONTF_STORAGE;
    }

    LPENUMIDLIST sEnumIdList = XPR_NULL;

    sComResult = aShellFolder->EnumObjects(aHwnd, sFlags, &sEnumIdList);

    if (FAILED(sComResult))
    {
        if (XPR_IS_NOT_NULL(sEnumIdList))
        {
            COM_RELEASE(sEnumIdList);
        }

        return XPR_FALSE;
    }

    COM_RELEASE(mEnumIdList);

    mEnumIdList = sEnumIdList;

    return XPR_TRUE;
}

xpr_bool_t ShellEnumerator::next(LPITEMIDLIST *aPidl)
{
    XPR_ASSERT(aPidl != XPR_NULL);

    if (XPR_IS_NULL(mEnumIdList))
    {
        return XPR_FALSE;
    }

    xpr_ulong_t sFetched = 0;

    if (mEnumIdList->Next(1, aPidl, &sFetched) != S_OK)
    {
        return XPR_FALSE;
    }

    XPR_ASSERT(*aPidl != XPR_NULL);

    return XPR_TRUE;
}
} // namespace base
} // namespace fxfile
