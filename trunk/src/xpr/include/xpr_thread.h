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

class XPR_DL_API Thread
{
    DISALLOW_COPY_AND_ASSIGN(Thread);

public:
    enum ThreadState
    {
        ThreadStateNone = 0,
        ThreadStateRunning,
        ThreadStateTerminated,
    };

    class XPR_DL_API Delegate
    {
    public:
        Delegate(void) {}
        virtual ~Delegate(void) {}

    public:
        virtual xpr_sint_t onThreadMain(Thread &aThread) = 0;
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
    Thread(const xpr_tchar_t *aThreadName = XPR_NULL);
    virtual ~Thread(void);

public:
    virtual xpr_rcode_t  start(Delegate *aDelegate, xpr_size_t aStackSize = 0);
    virtual xpr_rcode_t  join(xpr_sint_t *aExitCode = XPR_NULL);
    virtual xpr_rcode_t  detach(void);
    virtual ThreadId     getThreadId(void) const;
    virtual ThreadState  getThreadState(void) const;
    virtual NativeHandle getThreadHandle(void) const;
    virtual void         getThreadName(xpr::string &aThreadName) const;
    virtual const xpr::string &getThreadName(void) const;
    virtual xpr_sint_t   getExitCode(void) const;
    virtual xpr_bool_t   isEqual(const Thread &aThread) const;
    virtual void         swap(Thread &aThread);

public:
    static ThreadId      getCurThreadId(void);
    static void          yield(void);
    static void          sleep(xpr_time_t aWaitTime);

public:
    bool operator == (const Thread &aThread) const;
    bool operator != (const Thread &aThread) const;

protected:
#if defined(XPR_CFG_OS_WINDOWS)
    static xpr_uint_t XPR_STDCALL ThreadEntry(void *sParam);
#else
    static void* ThreadEntry(void *sParam);
#endif

protected:
    NativeHandle mHandle;
    xpr::string  mThreadName;
    ThreadId     mThreadId;
    ThreadState  mThreadState;
    xpr_uint_t   mExitCode;
};

typedef Thread thread;
} // namespace xpr

#endif // __XPR_THREAD_H__
