//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_MEMORY_H__
#define __XPR_MEMORY_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
XPR_DL_API xpr_rcode_t xpr_malloc(void **aAddr, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_calloc(void **aAddr, xpr_size_t aNumber, xpr_size_t aSize);
XPR_DL_API xpr_rcode_t xpr_realloc(void **aAddr, xpr_size_t aSize);
XPR_DL_API void        xpr_free(void *aAddr);

XPR_DL_API void        xpr_memset(void *aAddr, xpr_uint8_t aValue, xpr_size_t aSize);
XPR_DL_API void        xpr_memset_zero(void *aAddr, xpr_size_t aSize);
XPR_DL_API void        xpr_memcpy(void *aDest, const void *aSrc, xpr_size_t aSize);
XPR_DL_API void        xpr_memmove(void *aDest, const void *aSrc, xpr_size_t aSize);
XPR_DL_API xpr_sint_t  xpr_memcmp(const void *aAddr1, const void *aAddr2, xpr_size_t aSize);

#define XPR_SAFE_DELETE(addr) { delete (addr); (addr) = XPR_NULL; }
#define XPR_SAFE_DELETE_ARRAY(addr) { delete[] (addr); (addr) = XPR_NULL; }
} // namespace xpr

#endif // __XPR_MEMORY_H__
