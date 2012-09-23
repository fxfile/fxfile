//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_ustring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
//#define XPR_UTF8_LENGTH(c)        \
//    ((c) < 0x80 ? 1 :             \
//    ((c) < 0x800 ? 2 :            \
//    ((c) < 0x10000 ? 3 :          \
//    ((c) < 0x200000 ? 4 :         \
//    ((c) < 0x4000000 ? 5 : 6)))))
//
//static const xpr_char_t gUtf8SkipData[256] = {
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
//    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
//};
//
//#define XPR_UTF8_NEXT_CHAR(p) (xpr_char_t *)((p) + gUtf8SkipData[*(const xpr_char_t *)(p)])
//
//XPR_INLINE xpr_char32_t getUtf8Char(const xpr_char_t *aString)
//{
//    xpr_uchar_t sChar = static_cast<xpr_uchar_t>(*aString);
//
//    xpr_sint_t sLength = 0;
//    if (sChar < 0x80)
//    {
//        sLength = 1;
//        sChar &= 0x7f;
//    }
//    else if (XPR_TEST_MASK_BITS(sChar, 0xe0, 0xc0))
//    {
//        sLength = 2;
//        sChar &= 0x1f;
//    }
//    else if (XPR_TEST_MASK_BITS(sChar, 0xf0, 0xe0))
//    {
//        sLength = 3;
//        sChar &= 0x0f;
//    }
//    else if (XPR_TEST_MASK_BITS(sChar, 0xf8, 0xf0))
//    {
//        sLength = 4;
//        sChar &= 0x07;
//    }
//    else if (XPR_TEST_MASK_BITS(sChar, 0xfc, 0xf8))
//    {
//        sLength = 5;
//        sChar &= 0x03;
//    }
//    else if (XPR_TEST_MASK_BITS(sChar, 0xfe, 0xfc))
//    {
//        sLength = 6;
//        sChar &= 0x01;
//    }
//    else
//    {
//        return static_cast<xpr_char32_t>(-1);
//    }
//
//    xpr_sint_t i;
//    xpr_char32_t sChar32 = static_cast<xpr_char32_t>(sChar);
//
//    for (i = 1; i < sLength; ++i)
//    {
//        if ((aString[i] & 0xc0) != 0x80)
//        {
//            sChar32 = -1;
//            break;
//        }
//
//        sChar32 <<= 6;
//        sChar32 |= (aString[i] & 0x3f);
//    }
//
//    return sChar32;
//}
//
//XPR_INLINE xpr_size_t getUtf8ByteOffset(const xpr_char_t *aString, xpr_size_t aOffset)
//{
//    if (aOffset == ustring::npos)
//        return ustring::npos;
//
//    const xpr_char_t *p = aString;
//
//    for (; aOffset != 0; --aOffset)
//    {
//        const xpr_uint_t c = static_cast<xpr_uint_t>(*aString);
//        if (c == 0)
//            return ustring::npos;
//
//        p += gUtf8SkipData[c];
//    }
//
//    return (p - aString);
//}
//
//XPR_INLINE xpr_size_t getUtf8ByteOffset(const xpr_char_t *aString, xpr_size_t aOffset, xpr_size_t aMaxLength)
//{
//    if (aOffset == ustring::npos)
//        return ustring::npos;
//
//    const xpr_char_t *p = aString;
//    const xpr_char_t *sEndString = aString + aMaxLength;
//
//    for (; aOffset != 0; --aOffset)
//    {
//        if (p >= sEndString)
//            return ustring::npos;
//
//        p += gUtf8SkipData[static_cast<xpr_uint_t>(*aString)];
//    }
//
//    return (p - aString);
//}
} // namespace xpr
