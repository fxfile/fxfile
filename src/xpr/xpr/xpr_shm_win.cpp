//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_shm.h"
#include "xpr_rcode.h"
#include "xpr_debug.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
namespace
{
XPR_INLINE void generateShmName(xpr_key_t aKey, xpr::string &aName)
{
    aName.format(XPR_STRING_LITERAL("xpr::Shm_%08x"), aKey);
}
} // namespace anonymous

Shm::Shm(void)
{
    mHandle.mHandle = XPR_NULL;
}

Shm::~Shm(void)
{
    close();
}

xpr_rcode_t Shm::create(xpr_key_t aKey, xpr_size_t aReqSize, xpr_mode_t aPermission)
{
    if (aReqSize == 0)
        return XPR_RCODE_EINVAL;

    XPR_ASSERT(mHandle.mHandle == XPR_NULL);

    xpr::string sName;
    generateShmName(aKey, sName);

    mHandle.mHandle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                          XPR_NULL,
                                          PAGE_READWRITE,
                                          0,
                                          (DWORD)aReqSize,
                                          sName.c_str());

    if (mHandle.mHandle == XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Shm::open(xpr_key_t aKey)
{
    XPR_ASSERT(mHandle.mHandle == XPR_NULL);

    xpr::string sName;
    generateShmName(aKey, sName);

    mHandle.mHandle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sName.c_str());

    if (mHandle.mHandle == XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_bool_t Shm::isOpened(void)
{
    return (mHandle.mHandle != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

void Shm::close(void)
{
    if (mHandle.mHandle != XPR_NULL)
    {
        ::CloseHandle(mHandle.mHandle);
        mHandle.mHandle = XPR_NULL;
    }
}

xpr_rcode_t Shm::attach(void **aAttachedAddr)
{
    XPR_ASSERT(aAttachedAddr != XPR_NULL);

    if (mHandle.mHandle == XPR_NULL)
        return XPR_RCODE_EBADF;

    void *sAttachedAddr = ::MapViewOfFile(mHandle.mHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (sAttachedAddr == XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

    *aAttachedAddr = sAttachedAddr;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Shm::detach(void *aAttachedAddr)
{
    XPR_ASSERT(aAttachedAddr != XPR_NULL);

    xpr_bool_t sResult = ::UnmapViewOfFile(aAttachedAddr);
    if (sResult == XPR_FALSE)
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}
#endif
} // namespace xpr
