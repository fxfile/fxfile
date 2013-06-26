//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_MATH_H__
#define __XPR_MATH_H__ 1
#pragma once

#include "xpr_types.h"

namespace xpr
{
#define XPR_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define XPR_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define XPR_ABS(x, y) (((x) < 0) ? -(x) : (x))
#define XPR_ROUNDUP(x, y) ((((x) + ((y)-1)) / (y)) * (y))

#define XPR_RANGE(v1,x,v2) { if ((v1) > (x)) { (x) = (v1); } else if ((x) > (v2)) { (x) = (v2); } }
#define XPR_IS_RANGE(r1, x, r2) ((r1) <= (x) && (x) <= (r2))
#define XPR_IS_OUT_OF_RANGE(r1, x, r2) ((x) < (r1) || (r2) < (x))

#define XPR_MATH_E     (2.7182818284590452353602874713526624977572470937000)
#define XPR_MATH_LN2   (0.69314718055994530941723212145817656807550013436026)
#define XPR_MATH_LN10  (2.3025850929940456840179914546843642076011014886288)
#define XPR_MATH_PI    (3.1415926535897932384626433832795028841971693993751)
#define XPR_MATH_PI_2  (1.5707963267948966192313216916397514420985846996876)
#define XPR_MATH_PI_4  (0.78539816339744830961566084581987572104929234984378)
#define XPR_MATH_SQRT2 (1.4142135623730950488016887242096980785696718753769)
} // namespace xpr

#endif // __XPR_MATH_H__
