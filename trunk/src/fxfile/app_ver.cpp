//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "app_ver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
const xpr_tchar_t kDevVer   [] = XPR_STRING_LITERAL("dev");
const xpr_tchar_t kStableVer[] = XPR_STRING_LITERAL("stable");
} // namespace anonymous

void getAppVer(xpr_tchar_t *aAppVer)
{
    getAppVer(aAppVer, FXFILE_MAJOR_VER, FXFILE_MINOR_VER, FXFILE_PATCH_VER);
}

void getAppVer(xpr_tchar_t *aAppVer, xpr_sint_t aMajorVer, xpr_sint_t aMinorVer, xpr_sint_t aPatchVer)
{
    XPR_ASSERT(aAppVer != XPR_NULL);

    _stprintf(aAppVer,
              XPR_STRING_LITERAL("%d.%d.%d"),
              aMajorVer, aMinorVer, aPatchVer);
}

void getFullAppVer(xpr_tchar_t *aAppVer)
{
    if (XPR_IS_NOT_NULL(aAppVer))
    {
        _stprintf(aAppVer,
                  XPR_STRING_LITERAL("v%d.%d.%d-%s%s%s"),
                  FXFILE_MAJOR_VER, FXFILE_MINOR_VER, FXFILE_PATCH_VER,
                  ((FXFILE_MINOR_VER % 2) == 0) ? kDevVer : kStableVer,
#ifdef XPR_CFG_COMPILER_64BIT
        XPR_STRING_LITERAL(" 64bit"),
#else
        XPR_STRING_LITERAL(" 32bit"),
#endif
#ifdef XPR_CFG_UNICODE
        XPR_STRING_LITERAL(" (Unicode)")
#else
        XPR_STRING_LITERAL("")
#endif
                  );
    }
}

xpr_bool_t isLastedVer(xpr_sint_t aMajorVer, xpr_sint_t aMinorVer, xpr_sint_t aPatchVer)
{
    if (aMajorVer > FXFILE_MAJOR_VER)
    {
        return XPR_FALSE;
    }
    else if (aMajorVer > FXFILE_MAJOR_VER)
    {
        return XPR_TRUE;
    }

    if (aMinorVer > FXFILE_MINOR_VER)
    {
        return XPR_FALSE;
    }
    else if (aMinorVer > FXFILE_MINOR_VER)
    {
        return XPR_TRUE;
    }

    if (aPatchVer > FXFILE_PATCH_VER)
    {
        return XPR_FALSE;
    }
    else if (aPatchVer > FXFILE_PATCH_VER)
    {
        return XPR_TRUE;
    }

    return XPR_TRUE;
}
} // namespace fxfile
