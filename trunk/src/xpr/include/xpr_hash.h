//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_HASH_H__
#define __XPR_HASH_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
class XPR_DL_API Hash
{
    DISALLOW_COPY_AND_ASSIGN(Hash);

public:
    enum Method
    {
        Default,
        KnuthMultiplicative,
        KnuthMultiplicative32 = KnuthMultiplicative,
        KnuthMultiplicative64,
        PseudoRandomizing,
    };

public:
    Hash(void);
    ~Hash(void);

public:
    xpr_rcode_t reset(Method aMethod);
    xpr_rcode_t update(xpr_byte_t *aValue, xpr_size_t aBytes);
    const xpr_byte_t *getHashValue(void) const;
    xpr_size_t getHashValueLength(void) const;

public:
    static Method getDefaultMethod(void);

protected:
    void *mHashInternal;
};
} // namespace xpr

#endif // __XPR_HASH_H__
