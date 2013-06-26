//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_SYSTEM_H__
#define __XPR_SYSTEM_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
enum
{
    kOsVerWinUnknown  = 0,

    kOsVerWin9x       = 10,
    kOsVerWin95       = 20,
    kOsVerWin95Osr2   = 21,
    kOsVerWin98       = 30,
    kOsVerWin98SE     = 30,
    kOsVerWinMe       = 40,

    kOsVerWinNT       = 100,
    kOsVerWinNT35     = 110,
    kOsVerWinNT351    = 111,
    kOsVerWinNT4      = 120,
    kOsVerWinNT4Sp2   = 121,
    kOsVerWinNT4Sp3   = 122,
    kOsVerWinNT4Sp4   = 123,
    kOsVerWinNT4Sp5   = 124,
    kOsVerWinNT4Sp6   = 125,
    kOsVerWin2000     = 130,
    kOsVerWin2000Sp1  = 131,
    kOsVerWin2000Sp2  = 132,
    kOsVerWin2000Sp3  = 133,
    kOsVerWin2000Sp4  = 134,
    kOsVerWinXP       = 140,
    kOsVerWinXPSp1    = 141,
    kOsVerWinXPSp2    = 142,
    kOsVerWinXPSp3    = 143,
    kOsVerWinXPx64    = 145,
    kOsVerWin2003     = 150,
    kOsVerWin2003R2   = 151,
    kOsVerWinVista    = 160,
    kOsVerWinVistaSp1 = 161,
    kOsVerWinVistaSp2 = 162,
    kOsVerWinVistaSp3 = 163,
    kOsVerWin2008     = 170,
    kOsVerWin2008R2   = 171,
    kOsVerWin7        = 180,
    kOsVerWin7Sp1     = 181,
    kOsVerWin7Sp2     = 182,
    kOsVerWin8        = 190,
    kOsVerWin2012     = 200,

    kOsVerWinHigher,
};

struct SystemInfo
{
    xpr_uint_t mOsVer;
};

XPR_DL_API xpr_rcode_t initSystemInfo(void);

XPR_DL_API SystemInfo *getSystemInfo(void);
XPR_DL_API xpr_uint_t  getOsVer(void);

} // namespace xpr

#endif // __XPR_SYSTEM_H__
