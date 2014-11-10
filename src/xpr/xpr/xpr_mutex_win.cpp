//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_mutex.h"
#include "xpr_rcode.h"

namespace xpr
{
Mutex::Mutex(void)
{
    ::InitializeCriticalSection(&mHandle.mCs);
}

Mutex::~Mutex(void)
{
    ::DeleteCriticalSection(&mHandle.mCs);
}

xpr_rcode_t Mutex::lock(void)
{
    ::EnterCriticalSection(&mHandle.mCs);

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Mutex::tryLock(void)
{
    xpr_bool_t sResult = ::TryEnterCriticalSection(&mHandle.mCs);
    if (sResult == XPR_FALSE)
        return XPR_RCODE_EBUSY;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Mutex::unlock(void)
{
    ::LeaveCriticalSection(&mHandle.mCs);

    return XPR_RCODE_SUCCESS;
}
} // namespace xpr
