//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "AppVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void getAppVer(xpr_tchar_t *aAppVer)
{
    if (XPR_IS_NOT_NULL(aAppVer))
        _stprintf(aAppVer, XPR_STRING_LITERAL("%d.%d.%d"), MAJOR_VER, MINOR_VER, PATCH_VER);
}
