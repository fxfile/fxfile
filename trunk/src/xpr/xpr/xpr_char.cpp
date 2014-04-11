//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_char.h"
#include "xpr_bit.h"

namespace xpr
{
namespace
{
const xpr_uint16_t gCharClassTable[256] = {
    0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
    0x004, 0x104, 0x104, 0x004, 0x104, 0x104, 0x004, 0x004,
    0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
    0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
    0x140, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
    0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
    0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459,
    0x459, 0x459, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
    0x0d0, 0x653, 0x653, 0x653, 0x653, 0x653, 0x653, 0x253,
    0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253,
    0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253,
    0x253, 0x253, 0x253, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
    0x0d0, 0x473, 0x473, 0x473, 0x473, 0x473, 0x473, 0x073,
    0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073,
    0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073,
    0x073, 0x073, 0x073, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x004
};
} // namespace anonymous

xpr_uint16_t getCharClass(xpr_char_t aChar)
{
    return gCharClassTable[aChar];
}

xpr_bool_t isAlnum(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_ALNUM);
}

xpr_bool_t isAlpha(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_ALPHA);
}

// checks for any control character
xpr_bool_t isCntrl(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_CNTRL);
}

xpr_bool_t isDigit(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_DIGIT);
}

// checks for any printable character except for a space
xpr_bool_t isGraph(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_GRAPH);
}

xpr_bool_t isLower(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_LOWER);
}

// chacks for any printable character which contains a space
xpr_bool_t isPrint(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_PRINT);
}

// chacks for any printable character except for a space or an alphanumeric character
xpr_bool_t isPunct(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_PUNCT);
}

// chacks for space character which contains '\f', '\n', '\r', '\t', '\v'.
xpr_bool_t isSpace(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_SPACE);
}

xpr_bool_t isUpper(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_UPPER);
}

// chacks for a hexadecimal digits
xpr_bool_t isXdgit(xpr_char_t aChar)
{
    return XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_XDGIT);
}

xpr_char_t toUpper(xpr_char_t aChar)
{
    if (XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_UPPER))
    {
        return aChar + ('a' - 'A');
    }

    return aChar;
}

xpr_char_t toLower(xpr_char_t aChar)
{
    if (XPR_TEST_BITS(getCharClass(aChar), XPR_CHAR_LOWER))
    {
        return aChar - ('a' - 'A');
    }

    return aChar;
}
} // namespace xpr
