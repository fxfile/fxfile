//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_ver.h"

namespace xpr
{
XPR_DL_API void getVer(ver_t &aVer)
{
    aVer.mMajor = XPR_MAJOR_VER;
    aVer.mMinor = XPR_MINOR_VER;
    aVer.mPatch = XPR_PATCH_VER;
}

XPR_DL_API const xpr_char_t *getVer(void)
{
    return XPR_VER_STRING;
}

XPR_DL_API const xpr_char_t *getBuildTime(void)
{
    static const xpr_char_t kBuildTime[] = XPR_DATE " " XPR_TIME " PST";
    return kBuildTime;
}
} // namespace xpr
