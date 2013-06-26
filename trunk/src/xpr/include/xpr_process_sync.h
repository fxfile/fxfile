//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_PROCESS_SYNC_H__
#define __XPR_PROCESS_SYNC_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
class XPR_DL_API ProcessMutex
{
public:
    ProcessMutex(void);
    ~ProcessMutex(void);

public:
    xpr_rcode_t create(xpr_key_t aKey);
    xpr_rcode_t destroy(void);

public:
    xpr_rcode_t open(xpr_key_t aKey);
    xpr_rcode_t close(void);

public:
    xpr_rcode_t lock(void);
    xpr_rcode_t tryLock(void);
    xpr_rcode_t unlock(void);

private:
    struct NativeHandle
    {
#if defined(XPR_CFG_OS_WINDOWS)
        HANDLE mHandle;
#else
        xpr_sint_t mSemId;
#endif
    };

    NativeHandle mHandle;
};

class ProcessMutexGuard
{
public:
    explicit ProcessMutexGuard(ProcessMutex &aProcessMutex)
        : mProcessMutex(aProcessMutex)
    {
        mProcessMutex.lock();
    }

    ~ProcessMutexGuard(void)
    {
        mProcessMutex.unlock();
    }

private:
    ProcessMutex &mProcessMutex;
};
} // namespace xpr

#endif // __XPR_PROCESS_SYNC_H__
