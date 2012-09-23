//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_CHAR_H__
#define __XPR_CHAR_H__
#pragma once

#include "xpr_types.h"

namespace xpr
{
#define XPR_CHAR_ALNUM (1 << 0)
#define XPR_CHAR_ALPHA (1 << 1)
#define XPR_CHAR_CNTRL (1 << 2)
#define XPR_CHAR_DIGIT (1 << 3)
#define XPR_CHAR_GRAPH (1 << 4)
#define XPR_CHAR_LOWER (1 << 5)
#define XPR_CHAR_PRINT (1 << 6)
#define XPR_CHAR_PUNCT (1 << 7)
#define XPR_CHAR_SPACE (1 << 8)
#define XPR_CHAR_UPPER (1 << 9)
#define XPR_CHAR_XDGIT (1 << 10)

xpr_uint16_t getCharClass(xpr_char_t aChar);

xpr_bool_t isAlnum(xpr_char_t aChar);
xpr_bool_t isAlpha(xpr_char_t aChar);
xpr_bool_t isCntrl(xpr_char_t aChar);
xpr_bool_t isDigit(xpr_char_t aChar);
xpr_bool_t isGraph(xpr_char_t aChar);
xpr_bool_t isLower(xpr_char_t aChar);
xpr_bool_t isPrint(xpr_char_t aChar);
xpr_bool_t isPunct(xpr_char_t aChar);
xpr_bool_t isSpace(xpr_char_t aChar);
xpr_bool_t isUpper(xpr_char_t aChar);
xpr_bool_t isXdgit(xpr_char_t aChar);

xpr_char_t toUpper(xpr_char_t aChar);
xpr_char_t toLower(xpr_char_t aChar);
} // namespace xpr

#endif // __XPR_CHAR_H__
