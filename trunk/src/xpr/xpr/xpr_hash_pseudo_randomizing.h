//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_HASH_PSEUDO_RANDOMIZING_H__
#define __XPR_HASH_PSEUDO_RANDOMIZING_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_hash_internal.h"

namespace xpr
{
class HashPseudoRandomizing : public HashInternal
{
    DISALLOW_COPY_AND_ASSIGN(HashPseudoRandomizing);

public:
    HashPseudoRandomizing(void);
    virtual ~HashPseudoRandomizing(void);

public:
    xpr_rcode_t reset(void);
    xpr_rcode_t update(xpr_byte_t *aValue, xpr_size_t aBytes);
    const xpr_byte_t *getHashValue(void) const;
    xpr_size_t getHashValueLength(void) const;

private:
    xpr_uint32_t mHashValue;
};
} // namespace xpr

#endif // __XPR_HASH_PSEUDO_RANDOMIZING_H__
