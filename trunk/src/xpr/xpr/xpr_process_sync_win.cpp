//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_process_sync.h"
#include "xpr_debug.h"
#include "xpr_rcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
ProcessMutex::ProcessMutex(void)
{
    mHandle.mHandle = XPR_NULL;
}

ProcessMutex::~ProcessMutex(void)
{
    close();
}

XPR_INLINE void generateMutexName(xpr_key_t aKey, xpr_char_t *aName, xpr_size_t aMaxLength)
{
    _snprintf(aName,
              aMaxLength,
              XPR_MBCS_STRING_LITERAL("xpr::ProcessMutex_%08x"),
              aKey);
}

xpr_rcode_t ProcessMutex::create(xpr_key_t aKey)
{
    XPR_ASSERT(mHandle.mHandle == XPR_NULL);

    xpr_char_t sName[0xff] = {0};
    generateMutexName(aKey, sName, XPR_COUNT_OF(sName));

    mHandle.mHandle = ::CreateMutexA(XPR_NULL, XPR_FALSE, sName);
    if (mHandle.mHandle == XPR_NULL)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t ProcessMutex::destroy(void)
{
    return close();
}

xpr_rcode_t ProcessMutex::open(xpr_key_t aKey)
{
    XPR_ASSERT(mHandle.mHandle == XPR_NULL);

    xpr_char_t sName[0xff] = {0};
    generateMutexName(aKey, sName, XPR_COUNT_OF(sName));

    mHandle.mHandle = ::OpenMutexA(MUTEX_ALL_ACCESS, XPR_FALSE, sName);
    if (mHandle.mHandle == XPR_NULL)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t ProcessMutex::close(void)
{
    if (mHandle.mHandle != XPR_NULL)
    {
        xpr_bool_t sResult = ::CloseHandle(mHandle.mHandle);
        mHandle.mHandle = XPR_NULL;

        if (sResult == XPR_FALSE)
        {
            return XPR_RCODE_GET_OS_ERROR();
        }
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t ProcessMutex::lock(void)
{
    DWORD sResult = ::WaitForSingleObject(mHandle.mHandle, INFINITE);
    if (sResult == WAIT_ABANDONED || sResult == WAIT_OBJECT_0)
    {
        return XPR_RCODE_SUCCESS;
    }

    return XPR_RCODE_GET_OS_ERROR();
}

xpr_rcode_t ProcessMutex::tryLock(void)
{
    DWORD sResult = ::WaitForSingleObject(mHandle.mHandle, INFINITE);
    if (sResult == WAIT_ABANDONED || sResult == WAIT_OBJECT_0)
    {
        return XPR_RCODE_SUCCESS;
    }
    else if (sResult == WAIT_TIMEOUT)
    {
        return XPR_RCODE_EBUSY;
    }

    return XPR_RCODE_GET_OS_ERROR();
}

xpr_rcode_t ProcessMutex::unlock(void)
{
    xpr_bool_t sResult = ::ReleaseMutex(mHandle.mHandle);
    if (sResult == XPR_FALSE)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}
} // namespace xpr
