//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_hash_knuth_multiplicative.h"
#include "xpr_rcode.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
const xpr_uint32_t kConstant32 = XPR_UINT32_LITERAL(0x9e3779b9);
const xpr_uint64_t kConstant64 = XPR_UINT64_LITERAL(0x9e3779b97f4a7c16);
} // namespace anonymous

HashKnuthMultiplicative32::HashKnuthMultiplicative32(void)
{
}

HashKnuthMultiplicative32::~HashKnuthMultiplicative32(void)
{
}

xpr_rcode_t HashKnuthMultiplicative32::reset(void)
{
    mHashValue = 0;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t HashKnuthMultiplicative32::update(xpr_byte_t *aValue, xpr_size_t aBytes)
{
    xpr_size_t    i;
    xpr_uint32_t *sValue4 = (xpr_uint32_t *)aValue;
    xpr_size_t    sLength4 = aBytes / XPR_SIZE_OF(xpr_uint32_t);
    xpr_size_t    sRemainder = aBytes % XPR_SIZE_OF(xpr_uint32_t);

    XPR_ASSERT(aValue != XPR_NULL);

    for (i = 0; i < sLength4; ++i)
    {
        mHashValue += *sValue4++ * kConstant32;
    }

    if (sRemainder > 0)
    {
        xpr_byte_t *sValue1 = (xpr_byte_t *)sValue4;

        for (i = 0; i < sRemainder; ++i)
        {
            mHashValue += *sValue1++ * kConstant32;
        }
    }

    return XPR_RCODE_SUCCESS;
}

const xpr_byte_t *HashKnuthMultiplicative32::getHashValue(void) const
{
    return (const xpr_byte_t *)(&mHashValue);
}

xpr_size_t HashKnuthMultiplicative32::getHashValueLength(void) const
{
    return XPR_SIZE_OF(mHashValue);
}

HashKnuthMultiplicative64::HashKnuthMultiplicative64(void)
{
}

HashKnuthMultiplicative64::~HashKnuthMultiplicative64(void)
{
}

xpr_rcode_t HashKnuthMultiplicative64::reset(void)
{
    mHashValue = 0;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t HashKnuthMultiplicative64::update(xpr_byte_t *aValue, xpr_size_t aBytes)
{
    xpr_size_t    i;
    xpr_uint64_t *sValue8 = (xpr_uint64_t *)aValue;
    xpr_size_t    sLength8 = aBytes / XPR_SIZE_OF(xpr_uint64_t);
    xpr_size_t    sRemainder = aBytes % XPR_SIZE_OF(xpr_uint64_t);

    XPR_ASSERT(aValue != XPR_NULL);

    for (i = 0; i < sLength8; ++i)
    {
        mHashValue += *sValue8++ * kConstant64;
    }

    if (sRemainder > 0)
    {
        xpr_byte_t *sValue1 = (xpr_byte_t *)sValue8;

        for (i = 0; i < sRemainder; ++i)
        {
            mHashValue += *sValue1++ * kConstant64;
        }
    }

    return XPR_RCODE_SUCCESS;
}

const xpr_byte_t *HashKnuthMultiplicative64::getHashValue(void) const
{
    return (const xpr_byte_t *)(&mHashValue);
}

xpr_size_t HashKnuthMultiplicative64::getHashValueLength(void) const
{
    return XPR_SIZE_OF(mHashValue);
}
} // namespace xpr
