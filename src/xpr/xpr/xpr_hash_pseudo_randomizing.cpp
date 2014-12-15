//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_hash_pseudo_randomizing.h"
#include "xpr_rcode.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
const xpr_uint32_t kFirstHashValue = XPR_UINT32_LITERAL(2166136261);
const xpr_uint64_t kConstant       = XPR_UINT32_LITERAL(0x9e3779b97f4a7c16);
} // namespace anonymous

HashPseudoRandomizing::HashPseudoRandomizing(void)
{
}

HashPseudoRandomizing::~HashPseudoRandomizing(void)
{
}

xpr_rcode_t HashPseudoRandomizing::reset(void)
{
    mHashValue = kFirstHashValue;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t HashPseudoRandomizing::update(xpr_byte_t *aValue, xpr_size_t aBytes)
{
    xpr_size_t i;
    xpr_size_t sBytes  = aBytes;
    xpr_size_t sStride = 1 + sBytes / 10;

    if (sStride < sBytes)
    {
        sBytes -= sStride;
    }

    for (i = 0; i < sBytes; i += sStride)
    {
        mHashValue = kConstant * mHashValue ^ (xpr_uint32_t)aValue[i];
    }

    return XPR_RCODE_SUCCESS;
}

const xpr_byte_t *HashPseudoRandomizing::getHashValue(void) const
{
    return (const xpr_byte_t *)(&mHashValue);
}

xpr_size_t HashPseudoRandomizing::getHashValueLength(void) const
{
    return XPR_SIZE_OF(mHashValue);
}
} // namespace xpr
