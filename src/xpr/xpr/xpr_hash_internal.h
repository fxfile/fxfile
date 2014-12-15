//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_HASH_INTERNAL_H__
#define __XPR_HASH_INTERNAL_H__ 1
#pragma once

#include "xpr_types.h"

namespace xpr
{
class HashInternal
{
public:
    HashInternal(void) {}
    virtual ~HashInternal(void) {}

public:
    virtual xpr_rcode_t reset(void) = 0;
    virtual xpr_rcode_t update(xpr_byte_t *aValue, xpr_size_t aBytes) = 0;
    virtual const xpr_byte_t *getHashValue(void) const = 0;
    virtual xpr_size_t getHashValueLength(void) const = 0;
};
} // namespace xpr

#endif // __XPR_HASH_INTERNAL_H__
