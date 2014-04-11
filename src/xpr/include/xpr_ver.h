//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_VER_H__
#define __XPR_VER_H__ 1
#pragma once

#include "xpr_time.h"
#include "xpr_string.h"

namespace xpr
{
#define XPR_MAJOR_VER 0
#define XPR_MINOR_VER 2
#define XPR_PATCH_VER 0

#define XPR_VER_AT_LEAST(major, minor, patch) \
    (((major) < XPR_MAJOR_VER) || \
     ((major) == XPR_MAJOR_VER && (minor) < XPR_MINOR_VER) || \
     ((major) == XPR_MAJOR_VER && (minor) == XPR_MINOR_VER && (patch) <= XPR_PATCH_VER))

#define XPR_VER_MBCS_STRING \
    XPR_NUMBER_TO_MBCS_STRING(XPR_MAJOR_VER) XPR_MBCS_STRING_LITERAL(".") \
    XPR_NUMBER_TO_MBCS_STRING(XPR_MINOR_VER) XPR_MBCS_STRING_LITERAL(".") \
    XPR_NUMBER_TO_MBCS_STRING(XPR_PATCH_VER)
#define XPR_VER_WIDE_STRING \
    XPR_NUMBER_TO_WIDE_STRING(XPR_MAJOR_VER) XPR_WIDE_STRING_LITERAL(".") \
    XPR_NUMBER_TO_WIDE_STRING(XPR_MINOR_VER) XPR_WIDE_STRING_LITERAL(".") \
    XPR_NUMBER_TO_WIDE_STRING(XPR_PATCH_VER)
#if defined(XPR_CFG_UNICODE)
#define XPR_VER_STRING XPR_VER_WIDE_STRING
#else // not XPR_CFG_UNICODE
#define XPR_VER_STRING XPR_VER_MBCS_STRING
#endif // XPR_CFG_UNICODE

struct Ver
{
    xpr_sint_t mMajor;
    xpr_sint_t mMinor;
    xpr_sint_t mPatch;
};

XPR_DL_API void getVer(Ver &aVer);
XPR_DL_API void getVer(xpr::string &aVer);
XPR_DL_API void getBuildTime(TimeExpr &aTimeExpr);
XPR_DL_API void getBuildTime(xpr::string &aBuildTime);
} // namespace xpr

#endif // __XPR_VER_H__
