//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_THREAD_SYNC_H__
#define __XPR_THREAD_SYNC_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_time.h"

namespace xpr
{
//class XPR_DL_API SpinLock
//{
//public:
//    explicit SpinLock(xpr_sint_t sSpinCount = -1);
//    ~SpinLock(void);
//
//public:
//    xpr_rcode_t lock(void);
//    xpr_rcode_t tryLock(void);
//    xpr_rcode_t isLocked(void);
//    xpr_rcode_t unlock(void);
//
//private:
//    struct PrivateObject
//    {
//#if defined(XPR_CFG_CPU_PARISC)
//        volatile xpr_sint32_t mLock[4];
//#else
//        volatile xpr_sint32_t mLock;
//#endif
//        xpr_sint_t            mSpinCount;
//    };
//
//    PrivateObject mObject;
//};
//
//class XPR_DL_API CondVariable
//{
//public:
//    CondVariable(void);
//    ~CondVariable(void);
//
//public:
//    xpr_rcode_t wait(void);
//    xpr_rcode_t timedWait(xpr_sint64_t aTimed);
//    xpr_rcode_t signal(void);
//    xpr_rcode_t broadcastSignal(void);
//
//private:
//    struct NativeHandle
//    {
//#if defined(XPR_CFG_OS_WINDOWS)
//        HANDLE mHandle;
//#else
//        xpr_sint_t mCond;
//#endif
//    };
//
//    NativeHandle mHandle;
//};

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

//class XPR_DL_API Rwlock
//{
//public:
//    Rwlock(void);
//    ~Rwlock(void);
//
//public:
//    xpr_rcode_t lockRead(void);
//    xpr_rcode_t tryLockRead(void);
//    xpr_rcode_t lockWrite(void);
//    xpr_rcode_t tryLockWrite(void);
//    xpr_rcode_t unlock(void);
//
//private:
//    struct NativeHandle
//    {
//#if defined(XPR_CFG_OS_WINDOWS)
//        HANDLE mHandle;
//#else
//        pthread_rwlock_t mRwlock;
//#endif
//    };
//
//    NativeHandle mHandle;
//};
//
//class SpinLockGuard
//{
//public:
//    explicit SpinLockGuard(SpinLock &aSpinLock)
//        : mSpinLock(aSpinLock)
//    {
//        mSpinLock.lock();
//    }
//
//    ~SpinLockGuard(void)
//    {
//        mSpinLock.unlock();
//    }
//
//private:
//    SpinLock &mSpinLock;
//};
//
//class CondVariableGuard
//{
//public:
//    explicit CondVariableGuard(CondVariable &aCondVariable);
//    ~CondVariableGuard(void);
//
//private:
//    CondVariable &mCondVariable;
//};

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

//class RwReadGuard
//{
//public:
//    explicit RwReadGuard(Rwlock &aRwlock)
//        : mRwlock(aRwlock)
//    {
//        mRwlock.lockRead();
//    }
//
//    ~RwReadGuard(void)
//    {
//        mRwlock.unlock();
//    }
//
//private:
//    Rwlock &mRwlock;
//};
//
//class RwWriteGuard
//{
//public:
//    explicit RwWriteGuard(Rwlock &aRwlock)
//        : mRwlock(aRwlock)
//    {
//        mRwlock.lockWrite();
//    }
//
//    ~RwWriteGuard(void)
//    {
//        mRwlock.unlock();
//    }
//
//private:
//    Rwlock &mRwlock;
//};
} // namespace xpr

#endif // __XPR_THREAD_SYNC_H__
