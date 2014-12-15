//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_HASH_KNUTH_MULTIPLICATIVE_H__
#define __XPR_HASH_KNUTH_MULTIPLICATIVE_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_hash_internal.h"

namespace xpr
{
class HashKnuthMultiplicative32 : public HashInternal
{
    DISALLOW_COPY_AND_ASSIGN(HashKnuthMultiplicative32);

public:
    HashKnuthMultiplicative32(void);
    virtual ~HashKnuthMultiplicative32(void);

public:
    xpr_rcode_t reset(void);
    xpr_rcode_t update(xpr_byte_t *aValue, xpr_size_t aBytes);
    const xpr_byte_t *getHashValue(void) const;
    xpr_size_t getHashValueLength(void) const;

private:
    xpr_uint32_t mHashValue;
};

class HashKnuthMultiplicative64 : public HashInternal
{
    DISALLOW_COPY_AND_ASSIGN(HashKnuthMultiplicative64);

public:
    HashKnuthMultiplicative64(void);
    virtual ~HashKnuthMultiplicative64(void);

public:
    xpr_rcode_t reset(void);
    xpr_rcode_t update(xpr_byte_t *aValue, xpr_size_t aBytes);
    const xpr_byte_t *getHashValue(void) const;
    xpr_size_t getHashValueLength(void) const;

private:
    xpr_uint64_t mHashValue;
};
} // namespace xpr

#endif // __XPR_HASH_KNUTH_MULTIPLICATIVE_H__
