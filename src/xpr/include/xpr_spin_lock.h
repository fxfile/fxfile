//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_SPIN_LOCK_H__
#define __XPR_SPIN_LOCK_H__ 1
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
} // namespace xpr

#endif // __XPR_SPIN_LOCK_H__
