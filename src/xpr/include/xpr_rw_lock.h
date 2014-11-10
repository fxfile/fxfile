//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_RW_LOCK_H__
#define __XPR_RW_LOCK_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_time.h"

namespace xpr
{
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

#endif // __XPR_RW_LOCK_H__
