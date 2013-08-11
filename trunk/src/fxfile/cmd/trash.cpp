//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "trash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
Trash::Trash(void)
{
}

Trash::~Trash(void)
{
}

void Trash::getQueryInfo(xpr_sint64_t *aCount, xpr_sint64_t *aSize)
{
    SHQUERYRBINFO sShQueryRBInfo = {0};
    sShQueryRBInfo.cbSize = sizeof(SHQUERYRBINFO);

    xpr_sint64_t sCount = 0i64;
    xpr_sint64_t sSize  = 0i64;

    xpr_tchar_t sDrives[XPR_MAX_PATH + 1] = {0};
    GetDriveStrings(sDrives);

    xpr_sint_t i;
    HRESULT sHResult;
    const xpr_tchar_t *sDrive = sDrives;

    for (i = 0; i < 0xff; ++i)
    {
        if (sDrive[0] == 0)
            break;

        sShQueryRBInfo.i64Size     = 0i64;
        sShQueryRBInfo.i64NumItems = 0i64;

        sHResult = ::SHQueryRecycleBin(sDrive, &sShQueryRBInfo);
        if (SUCCEEDED(sHResult))
        {
            sCount += sShQueryRBInfo.i64NumItems;
            sSize  += sShQueryRBInfo.i64Size;
        }

        sDrive += _tcslen(sDrive) + 1;
    }

    if (XPR_IS_NOT_NULL(aCount)) *aCount = sCount;
    if (XPR_IS_NOT_NULL(aSize))  *aSize  = sSize;
}

HRESULT Trash::empty(HWND aHwnd, const xpr_tchar_t *aRootPath, DWORD aFlags)
{
    return ::SHEmptyRecycleBin(aHwnd, aRootPath, aFlags);
}
} // namespace cmd
} // namespace fxfile
