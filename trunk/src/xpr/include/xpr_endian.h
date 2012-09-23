//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_ENDIAN_H__
#define __XPR_ENDIAN_H__
#pragma once

namespace xpr
{
XPR_INLINE xpr_uint16_t swap16(xpr_uint16_t aValue)
{
    return (xpr_uint16_t)(
        (aValue & XPR_UINT16_LITERAL(0xff)) << 8 |
        (aValue & XPR_UINT16_LITERAL(0xff00)) >> 8);
}

XPR_INLINE xpr_uint32_t swap32(xpr_uint32_t aValue)
{
    return (xpr_uint32_t)(
        (aValue & XPR_UINT32_LITERAL(0xff)) << 24 |
        (aValue & XPR_UINT32_LITERAL(0xff00)) << 8 |
        (aValue & XPR_UINT32_LITERAL(0xff0000)) >> 8 |
        (aValue & XPR_UINT32_LITERAL(0xff000000)) >> 24);
}

XPR_INLINE xpr_uint64_t swap64(xpr_uint64_t aValue)
{
    return (xpr_uint64_t)(
        (aValue & XPR_UINT64_LITERAL(0xff)) << 56 |
        (aValue & XPR_UINT64_LITERAL(0xff00)) << 40 |
        (aValue & XPR_UINT64_LITERAL(0xff0000)) << 24 |
        (aValue & XPR_UINT64_LITERAL(0xff000000)) << 8 |
        (aValue & XPR_UINT64_LITERAL(0xff00000000)) >> 8 |
        (aValue & XPR_UINT64_LITERAL(0xff0000000000)) >> 24 |
        (aValue & XPR_UINT64_LITERAL(0xff000000000000)) >> 40 |
        (aValue & XPR_UINT64_LITERAL(0xff00000000000000)) >> 56);
}

XPR_INLINE xpr_bool_t isLittleEndian(void)
{
#if defined(XPR_CFG_LITTLE_ENDIAN)
    return XPR_TRUE;
#else
    return XPR_FALSE;
#endif
}

XPR_INLINE xpr_bool_t isBigEndian(void)
{
#if defined(XPR_CFG_LITTLE_ENDIAN)
    return XPR_FALSE;
#else
    return XPR_TRUE;
#endif
}

#if defined(XPR_CFG_LITTLE_ENDIAN)
#define XPR_HTON_16(x) swap16((x))
#define XPR_HTON_32(x) swap32((x))
#define XPR_HTON_64(x) swap64((x))
#define XPR_NTOH_16(x) swap16((x))
#define XPR_NTOH_32(x) swap32((x))
#define XPR_NTOH_64(x) swap64((x))
#define XPR_TOLE_16(x) (x)
#define XPR_TOLE_32(x) (x)
#define XPR_TOLE_64(x) (x)
#define XPR_TOBE_16(x) swap16((x))
#define XPR_TOBE_32(x) swap32((x))
#define XPR_TOBE_64(x) swap64((x))
#else // not XPR_CFG_LITTLE_ENDIAN
#define XPR_HTON_16(x) (x)
#define XPR_HTON_32(x) (x)
#define XPR_HTON_64(x) (x)
#define XPR_NTOH_16(x) (x)
#define XPR_NTOH_32(x) (x)
#define XPR_NTOH_64(x) (x)
#define XPR_TOLE_16(x) swap16((x))
#define XPR_TOLE_32(x) swap32((x))
#define XPR_TOLE_64(x) swap64((x))
#define XPR_TOBE_16(x) (x)
#define XPR_TOBE_32(x) (x)
#define XPR_TOBE_64(x) (x)
#endif // XPR_CFG_LITTLE_ENDIAN
} // namespace xpr

#endif // __XPR_ENDIAN_H__
