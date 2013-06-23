//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_BIT_H__
#define __XPR_BIT_H__
#pragma once

namespace xpr
{
#define XPR_BIT(n) (1 << (n))
#define XPR_BIT_MASK(n) (XPR_BIT(n) - 1)
#define XPR_TEST_BIT_INDEX(n, i) (((n) & (XPR_BIT(i))) == 1)
#define XPR_SET_BIT_INDEX(n, i) ((n) |= (XPR_BIT(i)))
#define XPR_CLR_BIT_INDEX(n, i) ((n) &= ~(XPR_BIT(i)))

#define XPR_ANY_BITS(x, bits) (((x) & (bits)) ? XPR_TRUE : XPR_FALSE)
#define XPR_TEST_BITS(x, bits) (((x) & (bits)) == (bits))
#define XPR_TEST_NONE_BITS(x, bits) (((x) & (bits)) == (0))
#define XPR_TEST_MASK_BITS(x, mask, bits) (((x) & (mask)) == (bits))
#define XPR_NONE_BITS(x) ((x) == (0))
#define XPR_FLIP_BITS(x) ((x) = ~(x))
#define XPR_SET_BITS(x, bits) ((x) |= (bits))
#define XPR_CLR_BITS(x, bits) ((x) &= ~(bits))
#define XPR_SET_OR_CLR_BITS(x, bits, set) \
    do { if (set) { XPR_SET_BITS(x, bits); } else { XPR_CLR_BITS(x, bits); } } while (false)

#define XPR_FAST_SWAP(x, y) \
    do { (x) ^= (y); (y) ^= (x); (x) ^= (y); } while (false)
} // namespace xpr

#endif // __XPR_BIT_H__
