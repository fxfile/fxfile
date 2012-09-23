//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_ATOMIC_H__
#define __XPR_ATOMIC_H__
#pragma once

#include "xpr_types.h"

namespace xpr
{
xpr_sint16_t AtomicRead16(volatile void *aAddr);
xpr_sint16_t AtomicWrite16(volatile void *aAddr, volatile xpr_sint16_t aValue);
xpr_sint16_t AtomicInc16(volatile void *aAddr);
xpr_sint16_t AtomicDec16(volatile void *aAddr);
xpr_sint16_t AtomicAdd16(volatile void *aAddr, volatile xpr_sint16_t aValue);
xpr_sint16_t AtomicSub16(volatile void *aAddr, volatile xpr_sint16_t aValue);
xpr_sint16_t AtomicCas16(volatile void *aAddr, volatile xpr_sint16_t aOldValue, volatile xpr_sint16_t aNewValue);

xpr_sint32_t AtomicRead32(volatile void *aAddr);
xpr_sint32_t AtomicWrite32(volatile void *aAddr, volatile xpr_sint32_t aValue);
xpr_sint32_t AtomicInc32(volatile void *aAddr);
xpr_sint32_t AtomicDec32(volatile void *aAddr);
xpr_sint32_t AtomicAdd32(volatile void *aAddr, volatile xpr_sint32_t aValue);
xpr_sint32_t AtomicSub32(volatile void *aAddr, volatile xpr_sint32_t aValue);
xpr_sint32_t AtomicCas32(volatile void *aAddr, volatile xpr_sint32_t aOldValue, volatile xpr_sint32_t aNewValue);

xpr_sint64_t AtomicRead64(volatile void *aAddr);
xpr_sint64_t AtomicWrite64(volatile void *aAddr, volatile xpr_sint64_t aValue);
xpr_sint64_t AtomicInc64(volatile void *aAddr);
xpr_sint64_t AtomicDec64(volatile void *aAddr);
xpr_sint64_t AtomicAdd64(volatile void *aAddr, volatile xpr_sint64_t aValue);
xpr_sint64_t AtomicSub64(volatile void *aAddr, volatile xpr_sint64_t aValue);
xpr_sint64_t AtomicCas64(volatile void *aAddr, volatile xpr_sint64_t aOldValue, volatile xpr_sint64_t aNewValue);

XPR_INLINE void* AtomicReadPtr(volatile void *aAddr)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicRead32(aAddr);
#else
    return (void *)AtomicRead64(aAddr);
#endif
}

XPR_INLINE void* AtomicWritePtr(volatile void *aAddr, volatile void *aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicWrite32(aAddr, (xpr_sint32_t)(xpr_sintptr_t)aValue);
#else
    return (void *)AtomicWrite64(aAddr, (xpr_sint64_t)aValue);
#endif
}

XPR_INLINE void* AtomicIncPtr(volatile void *aAddr)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicInc32(aAddr);
#else
    return (void *)AtomicInc64(aAddr);
#endif
}

XPR_INLINE void* AtomicDecPtr(volatile void *aAddr)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicDec32(aAddr);
#else
    return (void *)AtomicDec64(aAddr);
#endif
}

XPR_INLINE void* AtomicAddPtr(volatile void *aAddr, volatile void *aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicAdd32(aAddr, (xpr_sint32_t)(xpr_sintptr_t)aValue);
#else
    return (void *)AtomicAdd64(aAddr, (xpr_sint64_t)aValue);
#endif
}

XPR_INLINE void* AtomicSubPtr(volatile void *aAddr, volatile void *aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicSub32(aAddr, (xpr_sint32_t)(xpr_sintptr_t)aValue);
#else
    return (void *)AtomicSub64(aAddr, (xpr_sint64_t)aValue);
#endif
}

XPR_INLINE void* AtomicCasPtr(volatile void *aAddr, volatile void *aOldValue, volatile void *aNewValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)
    return (void *)(xpr_sintptr_t)AtomicCas32(aAddr,
                                              (xpr_sint32_t)(xpr_sintptr_t)aOldValue,
                                              (xpr_sint32_t)(xpr_sintptr_t)aNewValue);
#else
    return (void *)AtomicCas64(aAddr, (xpr_sint64_t)aOldValue, (xpr_sint64_t)aNewValue);
#endif
}
} // namespace xpr

#endif // __XPR_ATOMIC_H__
