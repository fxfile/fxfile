//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_memory.h"
#include "xpr_rcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
xpr_rcode_t MemAlloc(void **aAddr, xpr_size_t aSize)
{
    if (aAddr == XPR_NULL || aSize == 0)
        return XPR_RCODE_EINVAL;

#if defined(XPR_CFG_OS_WINDOWS)

    HANDLE sHeap = ::GetProcessHeap();
    if (sHeap != XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    *aAddr = ::HeapAlloc(sHeap, 0, aSize);

#else

    *aAddr = malloc(aSize);

#endif

    if (*aAddr == XPR_NULL)
        return XPR_RCODE_ENOMEM;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t MemCalloc(void **aAddr, xpr_size_t aNumber, xpr_size_t aSize)
{
    if (aAddr == XPR_NULL || aNumber == 0 || aSize == 0)
        return XPR_RCODE_EINVAL;

#if defined(XPR_CFG_OS_WINDOWS)

    HANDLE sHeap = ::GetProcessHeap();
    if (sHeap != XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    *aAddr = ::HeapAlloc(sHeap, HEAP_ZERO_MEMORY, aNumber * aSize);

#else

    *aAddr = calloc(aNumber * aSize);

#endif

    if (*aAddr == XPR_NULL)
        return XPR_RCODE_ENOMEM;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t MemRealloc(void **aAddr, xpr_size_t aSize)
{
    if (aAddr == XPR_NULL || aSize == 0)
        return XPR_RCODE_EINVAL;

    void *sNewAddr = XPR_NULL;

#if defined(XPR_CFG_OS_WINDOWS)

    HANDLE sHeap = ::GetProcessHeap();
    if (sHeap != XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    if (*aAddr == XPR_NULL)
    {
        sNewAddr = ::HeapAlloc(sHeap, HEAP_ZERO_MEMORY, aSize);
    }
    else
    {
        sNewAddr = ::HeapReAlloc(sHeap, HEAP_ZERO_MEMORY, *aAddr, aSize);
    }

#else

    sNewAddr = realloc(*aAddr, aSize);

#endif

    if (sNewAddr == XPR_NULL)
        return XPR_RCODE_ENOMEM;

    *aAddr = sNewAddr;

    return XPR_RCODE_SUCCESS;
}

void MemFree(void *aAddr)
{
    if (aAddr == XPR_NULL)
        return;

#if defined(XPR_CFG_OS_WINDOWS)

    HANDLE sHeap = ::GetProcessHeap();
    ::HeapFree(sHeap, 0, aAddr);

#else

    free(aAddr);

#endif
}

void MemSet(void *aAddr, xpr_uint8_t aValue, xpr_size_t aSize)
{
    if (aAddr == XPR_NULL || aSize == 0)
        return;

#if defined(XPR_CFG_OS_WINDOWS)

    ::FillMemory(aAddr, aSize, aValue);

#else

    memset(aAddr, (xpr_sint32_t)aValue, aSize);

#endif
}

void MemZeroSet(void *aAddr, xpr_size_t aSize)
{
    MemSet(aAddr, 0x00, aSize);
}

void MemCpy(void *aDest, const void *aSrc, xpr_size_t aSize)
{
    if (aSize == 0)
        return;

#if defined(XPR_CFG_OS_WINDOWS)

    ::CopyMemory(aDest, aSrc, aSize);

#else

    memcpy(aDest, aSrc, aSize);

#endif
}

void MemMove(void *aDest, const void *aSrc, xpr_size_t aSize)
{
    if (aSize == 0)
        return;

#if defined(XPR_CFG_OS_WINDOWS)

    ::MoveMemory(aDest, aSrc, aSize);

#else

    memmove(aDest, aSrc, aSize);

#endif
}

xpr_sint_t MemCmp(const void *aAddr1, const void *aAddr2, xpr_size_t aSize)
{
    if (aSize == 0)
        return 0;

    xpr_sint_t sResult = memcmp(aAddr1, aAddr2, aSize);

    return sResult;
}
} // namespace xpr
