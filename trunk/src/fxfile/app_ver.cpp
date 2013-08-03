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
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
const xpr_tchar_t kDevVer   [] = XPR_STRING_LITERAL("dev");
const xpr_tchar_t kStableVer[] = XPR_STRING_LITERAL("stable");

void getAppVer(xpr_tchar_t *aAppVer)
{
    if (XPR_IS_NOT_NULL(aAppVer))
    {
        _stprintf(aAppVer,
                  XPR_STRING_LITERAL("%d.%d.%d"),
                  FXFILE_MAJOR_VER, FXFILE_MINOR_VER, FXFILE_PATCH_VER);
    }
}

void getFullAppVer(xpr_tchar_t *aAppVer)
{
    if (XPR_IS_NOT_NULL(aAppVer))
    {
        _stprintf(aAppVer,
                  XPR_STRING_LITERAL("v%d.%d.%d-%s%s"),
                  FXFILE_MAJOR_VER, FXFILE_MINOR_VER, FXFILE_PATCH_VER,
                  ((FXFILE_MINOR_VER % 2) == 0) ? kDevVer : kStableVer,
#ifdef XPR_CFG_UNICODE
        XPR_STRING_LITERAL(" Unicode")
#else
        XPR_STRING_LITERAL("")
#endif
                  );
    }
}
} // namespace fxfile
