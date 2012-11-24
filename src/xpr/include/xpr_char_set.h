//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_CHAR_SET_H__
#define __XPR_CHAR_SET_H__
#pragma once

#include "xpr_types.h"

namespace xpr
{
enum CharSet
{
    CharSetNone,
    CharSetMultiBytes,
    CharSetUtf8,
    CharSetUtf16,
    CharSetUtf16be,
    CharSetUtf32,
    CharSetUtf32be,
};

xpr_rcode_t convertCharSet(const void *aInput,  xpr_size_t *aInputBytes,  CharSet aInputCharSet,
                                 void *aOutput, xpr_size_t *aOutputBytes, CharSet aOutputCharSet);

#define XPR_MBS_TO_MBS(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetMultiBytes, aOutput, aOutputBytes, xpr::CharSetMultiBytes)
#define XPR_MBS_TO_UTF8(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetMultiBytes, aOutput, aOutputBytes, xpr::CharSetUtf8)
#define XPR_MBS_TO_UTF16(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetMultiBytes, aOutput, aOutputBytes, xpr::CharSetUtf16)
#define XPR_UTF16_TO_MBS(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf16, aOutput, aOutputBytes, xpr::CharSetMultiBytes)
#define XPR_UTF16_TO_UTF8(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf16, aOutput, aOutputBytes, xpr::CharSetUtf8)
#define XPR_UTF16_TO_UTF16(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf16, aOutput, aOutputBytes, xpr::CharSetUtf16)
#define XPR_UTF8_TO_MBS(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf8, aOutput, aOutputBytes, xpr::CharSetMultiBytes)
#define XPR_UTF8_TO_UTF8(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf8, aOutput, aOutputBytes, xpr::CharSetUtf8)
#define XPR_UTF8_TO_UTF16(aInput, aInputBytes, aOutput, aOutputBytes) \
    xpr::convertCharSet(aInput, aInputBytes, xpr::CharSetUtf8, aOutput, aOutputBytes, xpr::CharSetUtf16)

#if defined(XPR_CFG_UNICODE)
#define XPR_TCS_TO_MBS    XPR_UTF16_TO_MBS
#define XPR_TCS_TO_UTF8   XPR_UTF16_TO_UTF8
#define XPR_TCS_TO_UTF16  XPR_UTF16_TO_UTF16
#define XPR_MBS_TO_TCS    XPR_MBS_TO_UTF16
#define XPR_UTF8_TO_TCS   XPR_UTF8_TO_UTF16
#define XPR_UTF16_TO_TCS  XPR_UTF16_TO_UTF16
#else // not XPR_CFG_UNICODE
#define XPR_TCS_TO_MBS    XPR_MBS_TO_MBS
#define XPR_TCS_TO_UTF8   XPR_MBS_TO_UTF8
#define XPR_TCS_TO_UTF16  XPR_MBS_TO_UTF16
#define XPR_MBS_TO_TCS    XPR_MBS_TO_MBS
#define XPR_UTF8_TO_TCS   XPR_UTF8_TO_MBS
#define XPR_UTF16_TO_TCS  XPR_UTF16_TO_MBS
#endif // XPR_CFG_UNICODE
} // namespace xpr

#endif // __XPR_CHAR_SET_H__
