//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_MUTEX_H__
#define __XPR_MUTEX_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_time.h"

namespace xpr
{
class XPR_DL_API Mutex
{
public:
    Mutex(void);
    ~Mutex(void);

public:
    xpr_rcode_t lock(void);
    xpr_rcode_t tryLock(void);
    xpr_rcode_t unlock(void);

private:
    struct NativeHandle
    {
#if defined(XPR_CFG_OS_WINDOWS)
        CRITICAL_SECTION mCs;
#else
        pthread_mutex_t mMutex;
#endif
    };

    NativeHandle mHandle;
};

class MutexGuard
{
public:
    explicit MutexGuard(Mutex &aMutex)
        : mMutex(aMutex)
    {
        mMutex.lock();
    }

    ~MutexGuard(void)
    {
        mMutex.unlock();
    }

private:
    Mutex &mMutex;
};
} // namespace xpr

#endif // __XPR_MUTEX_H__
