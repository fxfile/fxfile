//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL WINAPI DllMain(HANDLE hinstDLL,
                    DWORD dwReason,
                    LPVOID lpvReserved)
{
    // nothing to do
    return TRUE;
}
