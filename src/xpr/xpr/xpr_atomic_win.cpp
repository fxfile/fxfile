//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_atomic.h"
#include "xpr_memory_barrier.h"

namespace xpr
{
XPR_INLINE xpr_sint16_t AtomicFetchAdd16(volatile void *aAddr, volatile xpr_sint16_t aValue)
{
    __asm
    {
        mov ax, aValue;
        mov ebx, aAddr;
        lock xadd [ebx], ax;
    }
}

XPR_INLINE xpr_sint32_t AtomicFetchAdd32(volatile void *aAddr, volatile xpr_sint32_t aValue)
{
    __asm
    {
        mov eax, aValue;
        mov ebx, aAddr;
        lock xadd [ebx], eax;
    }
}

XPR_DL_API xpr_sint16_t AtomicRead16(volatile void *aAddr)
{
    XPR_MEMORY_BARRIER();

    return *(volatile xpr_sint16_t *)aAddr;
}

XPR_DL_API xpr_sint16_t AtomicWrite16(volatile void *aAddr, volatile xpr_sint16_t aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    __asm
    {
        mov ax, aValue;
        mov ebx, aAddr;
        xchg [ebx], ax;
    }

#else

    xpr_sint16_t sOld;
    xpr_sint16_t sTemp;

    do
    {
        sOld = *(volatile xpr_sint16_t *)aAddr;
        sTemp = AtomicCas16(aAddr, sOld, aValue);
    } while (sTemp != sOld);

    return sOld;

#endif
}

XPR_DL_API xpr_sint16_t AtomicInc16(volatile void *aAddr)
{
    return AtomicAdd16(aAddr, 1);
}

XPR_DL_API xpr_sint16_t AtomicDec16(volatile void *aAddr)
{
    return AtomicAdd16(aAddr, -1);
}

XPR_DL_API xpr_sint16_t AtomicAdd16(volatile void *aAddr, volatile xpr_sint16_t aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    return AtomicFetchAdd16(aAddr, aValue) + aValue;

#else

    xpr_sint16_t sOld;
    xpr_sint16_t sTemp;

    do
    {
        sOld = *(volatile xpr_sint16_t *)aAddr;
        sTemp = AtomicCas16(aAddr, sOld, sOld + aValue);
    } while (sTemp != sOld);

    return sOld + aValue;

#endif
}

XPR_DL_API xpr_sint16_t AtomicSub16(volatile void *aAddr, volatile xpr_sint16_t aValue)
{
    return AtomicAdd16(aAddr, -aValue);
}

XPR_DL_API xpr_sint16_t AtomicCas16(volatile void *aAddr, volatile xpr_sint16_t aOldValue, volatile xpr_sint16_t aNewValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    __asm
    {
        mov ax, aOldValue;
        mov ebx, aAddr;
        mov cx, aNewValue;
        lock cmpxchg [ebx], cx;
    }

#else

    return InterlockedCompareExchange16((volatile xpr_sshort_t *)aAddr, (xpr_sshort_t)aNewValue, (xpr_sshort_t)aOldValue);

#endif
}

XPR_DL_API xpr_sint32_t AtomicRead32(volatile void *aAddr)
{
    XPR_MEMORY_BARRIER();

    return *(volatile xpr_sint32_t *)aAddr;
}

XPR_DL_API xpr_sint32_t AtomicWrite32(volatile void *aAddr, volatile xpr_sint32_t aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    __asm
    {
        mov eax, aValue;
        mov ebx, aAddr;
        xchg [ebx], eax;
    }

#else

    return InterlockedExchange((xpr_slong_t volatile *)aAddr, (xpr_slong_t)aValue);

#endif
}

XPR_DL_API xpr_sint32_t AtomicInc32(volatile void *aAddr)
{
    return AtomicAdd32(aAddr, 1);
}

XPR_DL_API xpr_sint32_t AtomicDec32(volatile void *aAddr)
{
    return AtomicAdd32(aAddr, -1);
}

XPR_DL_API xpr_sint32_t AtomicAdd32(volatile void *aAddr, volatile xpr_sint32_t aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    return AtomicFetchAdd32(aAddr, aValue) + aValue;

#else

    return InterlockedExchangeAdd((xpr_slong_t volatile *)aAddr, (xpr_slong_t)aValue) + aValue;

#endif
}

XPR_DL_API xpr_sint32_t AtomicSub32(volatile void *aAddr, volatile xpr_sint32_t aValue)
{
    return AtomicAdd32(aAddr, -aValue);
}

XPR_DL_API xpr_sint32_t AtomicCas32(volatile void *aAddr, volatile xpr_sint32_t aOldValue, volatile xpr_sint32_t aNewValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    __asm
    {
        mov eax, aOldValue;
        mov ebx, aAddr;
        mov ecx, aNewValue;
        lock cmpxchg [ebx], ecx;
    }

#else

    return InterlockedCompareExchange((xpr_slong_t volatile *)aAddr, (xpr_slong_t)aNewValue, (xpr_slong_t)aOldValue);

#endif
}

XPR_DL_API xpr_sint64_t AtomicRead64(volatile void *aAddr)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    __asm
    {
        mov ecx, aAddr;
        mov edx, ecx;
        mov eax, ebx;
        lock cmpxchg8b [ecx];
    }

#else

    XPR_MEMORY_BARRIER();

    return *(volatile xpr_sint64_t *)aAddr;

#endif
}

XPR_DL_API xpr_sint64_t AtomicWrite64(volatile void *aAddr, volatile xpr_sint64_t aValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    xpr_sint64_t sOld;
    xpr_sint64_t sTemp;

    do
    {
        sOld = *(volatile xpr_sint64_t *)aAddr;
        sTemp = AtomicCas64(aAddr, sOld, aValue);
    } while (sTemp != sOld);

    return sOld;

#else

    return (xpr_sint64_t)InterlockedExchangePointer((PVOID volatile *)aAddr, (PVOID)aValue);

#endif
}

XPR_DL_API xpr_sint64_t AtomicInc64(volatile void *aAddr)
{
    return AtomicAdd64(aAddr, 1);
}

XPR_DL_API xpr_sint64_t AtomicDec64(volatile void *aAddr)
{
    return AtomicAdd64(aAddr, -1);
}

XPR_DL_API xpr_sint64_t AtomicAdd64(volatile void *aAddr, volatile xpr_sint64_t aValue)
{
    xpr_sint64_t sOld;
    xpr_sint64_t sTemp;

    do
    {
        sOld = *(volatile xpr_sint64_t *)aAddr;
        sTemp = AtomicCas64(aAddr, sOld, sOld + aValue);
    } while (sTemp != sOld);

    return sOld;
}

XPR_DL_API xpr_sint64_t AtomicSub64(volatile void *aAddr, volatile xpr_sint64_t aValue)
{
    return AtomicAdd64(aAddr, -aValue);
}

XPR_DL_API xpr_sint64_t AtomicCas64(volatile void *aAddr, volatile xpr_sint64_t aOldValue, volatile xpr_sint64_t aNewValue)
{
#if defined(XPR_CFG_COMPILER_32BIT)

    xpr_uint32_t sOldValue0 = (xpr_uint32_t)((xpr_uint64_t)aOldValue & 0xffffffff);
    xpr_uint32_t sOldValue1 = (xpr_uint32_t)((xpr_uint64_t)aOldValue >> 32);
    xpr_uint32_t sNewValue0 = (xpr_uint32_t)((xpr_uint64_t)aNewValue & 0xffffffff);
    xpr_uint32_t sNewValue1 = (xpr_uint32_t)((xpr_uint64_t)aNewValue >> 32);

    __asm
    {
        mov eax, [sOldValue0];
        mov edx, [sOldValue1];
        mov ebx, [sNewValue0];
        mov ecx, [sNewValue1];
        mov esi, [aAddr];
        lock cmpxchg8b [esi];
    }

#else

    return (xpr_sint64_t)InterlockedCompareExchangePointer((PVOID volatile *)aAddr, (PVOID)aNewValue, (PVOID)sOldValue);

#endif
}
} // namespace xpr
