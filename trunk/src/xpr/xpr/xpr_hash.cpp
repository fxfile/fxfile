//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_hash.h"
#include "xpr_memory.h"
#include "xpr_rcode.h"
#include "xpr_debug.h"
#include "xpr_hash_internal.h"
#include "xpr_hash_knuth_multiplicative.h"
#include "xpr_hash_pseudo_randomizing.h"

namespace xpr
{
#define XPR_HASH_INTERNAL() ((HashInternal *)(mHashInternal))

Hash::Hash(void)
    : mHashInternal(XPR_NULL)
{
}

Hash::~Hash(void)
{
    HashInternal *sHashInternal = (HashInternal *)mHashInternal;

    XPR_SAFE_DELETE(sHashInternal);
    mHashInternal = XPR_NULL;
}

xpr_rcode_t Hash::reset(Method aMethod)
{
    Method sMethod = aMethod;

    if (sMethod == Default)
    {
        sMethod = getDefaultMethod();
    }

    HashInternal *sHashInternal;
    switch (sMethod)
    {
    case Default:
    case KnuthMultiplicative32:
        sHashInternal = new HashKnuthMultiplicative32;
        break;

    case KnuthMultiplicative64:
        sHashInternal = new HashKnuthMultiplicative64;
        break;

    case PseudoRandomizing:
        sHashInternal = new HashPseudoRandomizing;
        break;

    default:
        return XPR_RCODE_EINVAL;
    }

    mHashInternal = sHashInternal;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Hash::update(xpr_byte_t *aValue, xpr_size_t aBytes)
{
    XPR_ASSERT(mHashInternal != XPR_NULL);

    return XPR_HASH_INTERNAL()->update(aValue, aBytes);
}

const xpr_byte_t *Hash::getHashValue(void) const
{
    XPR_ASSERT(mHashInternal != XPR_NULL);

    return XPR_HASH_INTERNAL()->getHashValue();
}

xpr_size_t Hash::getHashValueLength(void) const
{
    XPR_ASSERT(mHashInternal != XPR_NULL);

    return XPR_HASH_INTERNAL()->getHashValueLength();
}

Hash::Method Hash::getDefaultMethod(void)
{
    return KnuthMultiplicative;
}
} // namespace xpr
