//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_THREAD_H__
#define __XPR_THREAD_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_time.h"
#include "xpr_string.h"

namespace xpr
{
class Thread;

typedef xpr_uint64_t ThreadId;

enum ThreadState
{
    ThreadStateNone = 0,
    ThreadStateRunning,
    ThreadStateTerminated,
};

enum
{
    ThreadPriorityMin    = 1,
    ThreadPriorityNormal = 5,
    ThreadPriorityMax    = 10,
};

class XPR_DL_API Thread
{
    DISALLOW_COPY_AND_ASSIGN(Thread);

public:
    class Runnable
    {
    public:
        virtual xpr_sint_t runThread(Thread &aThread) = 0;
    };

public:
    struct NativeHandle
    {
#if defined(XPR_CFG_OS_WINDOWS)
        HANDLE mHandle;
#else
        pthread_t mHandle;
#endif
    };

public:
    Thread(const xpr::string *aThreadName = XPR_NULL);
    virtual ~Thread(void);

public:
    virtual xpr_rcode_t  start(Runnable *aRunnable, xpr_size_t aStackSize = 0);
    virtual xpr_rcode_t  join(xpr_sint_t *aExitCode = XPR_NULL);
    virtual xpr_rcode_t  detach(void);
    virtual ThreadId     getThreadId(void) const;
    virtual ThreadState  getThreadState(void) const;
    virtual xpr_sint_t   getThreadPriority(void) const;
    virtual NativeHandle getThreadHandle(void) const;
    virtual void         getThreadName(xpr::string &aThreadName) const;
    virtual const xpr::string &getThreadName(void) const;
    virtual xpr_sint_t   getExitCode(void) const;
    virtual xpr_bool_t   isRunning(void) const;
    virtual xpr_bool_t   isEqual(const Thread &aThread) const;
    virtual void         setPriority(xpr_sint_t aPriority);
    virtual void         swap(Thread &aThread);

public:
    static ThreadId      getCurThreadId(void);
    static void          yield(void);
    static void          sleep(xpr_time_t aWaitTime);

public:
    bool operator == (const Thread &aThread) const;
    bool operator != (const Thread &aThread) const;

private:
#if defined(XPR_CFG_OS_WINDOWS)
    static xpr_uint_t XPR_STDCALL threadEntry(void *sParam);
#else
    static void* threadEntry(void *sParam);
#endif

private:
    NativeHandle mHandle;
    xpr::string  mThreadName;
    ThreadId     mThreadId;
    ThreadState  mThreadState;
    xpr_sint_t   mPriority;
    xpr_uint_t   mExitCode;
};

typedef Thread thread;
} // namespace xpr

#endif // __XPR_THREAD_H__
