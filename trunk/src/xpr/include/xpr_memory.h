//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_MEMORY_H__
#define __XPR_MEMORY_H__
#pragma once

#include "xpr_types.h"

namespace xpr
{
xpr_rcode_t MemAlloc(void **aAddr, xpr_size_t aSize);
xpr_rcode_t MemCalloc(void **aAddr, xpr_size_t aNumber, xpr_size_t aSize);
xpr_rcode_t MemRealloc(void **aAddr, xpr_size_t aSize);
void        MemFree(void *aAddr);

void        MemSet(void *aAddr, xpr_uint8_t aValue, xpr_size_t aSize);
void        MemZeroSet(void *aAddr, xpr_size_t aSize);
void        MemCpy(void *aDest, const void *aSrc, xpr_size_t aSize);
void        MemMove(void *aDest, const void *aSrc, xpr_size_t aSize);
xpr_sint_t  MemCmp(const void *aAddr1, const void *aAddr2, xpr_size_t aSize);

#define XPR_SAFE_DELETE(addr) { delete (addr); (addr) = XPR_NULL; }
#define XPR_SAFE_DELETE_ARRAY(addr) { delete[] (addr); (addr) = XPR_NULL; }
} // namespace xpr

#endif // __XPR_MEMORY_H__
