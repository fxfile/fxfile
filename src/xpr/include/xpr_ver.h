//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_VER_H__
#define __XPR_VER_H__
#pragma once

#include "xpr_time.h"

namespace xpr
{
#define XPR_MAJOR_VER 0
#define XPR_MINOR_VER 2
#define XPR_PATCH_VER 0

#define XPR_VER_AT_LEAST(major, minor, patch) \
    (((major) < XPR_MAJOR_VER) || \
     ((major) == XPR_MAJOR_VER && (minor) < XPR_MINOR_VER) || \
     ((major) == XPR_MAJOR_VER && (minor) == XPR_MINOR_VER && (patch) <= XPR_PATCH_VER))

#define XPR_VER_STRING_DEFINE(x) #x
#define XPR_VER_STRING \
    XPR_VER_STRING_DEFINE(XPR_MAJOR_VER) "." \
    XPR_VER_STRING_DEFINE(XPR_MINOR_VER) "." \
    XPR_VER_STRING_DEFINE(XPR_PATCH_VER)

typedef struct
{
    xpr_sint_t mMajor;
    xpr_sint_t mMinor;
    xpr_sint_t mPatch;
} ver_t;

void getVer(ver_t &aVer);
const xpr_char_t *getVer(void);
const xpr_char_t *getBuildTime(void);
void getBuildTime(Time &aTime);
} // namespace xpr

#endif // __XPR_VER_H__
