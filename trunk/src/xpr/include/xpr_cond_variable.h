//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_COND_VARIABLE_H__
#define __XPR_COND_VARIABLE_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_time.h"

namespace xpr
{
//class XPR_DL_API CondVariable
//{
//public:
//    CondVariable(void);
//    ~CondVariable(void);
//
//public:
//    xpr_rcode_t wait(void);
//    xpr_rcode_t timedWait(xpr_time_t aTimeout);
//    xpr_rcode_t signal(void);
//    xpr_rcode_t signalBroadcast(void);
//
//private:
//    struct NativeHandle
//    {
//#if defined(XPR_CFG_OS_WINDOWS)
//        HANDLE mHandle;
//#else
//        pthread_cond_t mCond;
//#endif
//    };
//
//    NativeHandle mHandle;
//};
} // namespace xpr

#endif // __XPR_COND_VARIABLE_H__
