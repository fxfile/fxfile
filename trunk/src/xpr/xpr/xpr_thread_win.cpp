//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_thread.h"
#include "xpr_memory.h"
#include "xpr_rcode.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
const xpr_tchar_t kUnknownThreadName[] = XPR_STRING_LITERAL("XPR - Unknown Thread");
} // namespace anonymous

struct ThreadParam
{
    Thread           *mThread;
    Thread::Runnable *mRunnable;
};

static const xpr_sint_t gThreadPrirityTable[] =
{
    THREAD_PRIORITY_IDLE,          // 0  ThreadPriorityMin
    THREAD_PRIORITY_LOWEST,        // 1
    THREAD_PRIORITY_LOWEST,        // 2
    THREAD_PRIORITY_BELOW_NORMAL,  // 3
    THREAD_PRIORITY_BELOW_NORMAL,  // 4
    THREAD_PRIORITY_NORMAL,        // 5  ThreadPriorityNormal
    THREAD_PRIORITY_ABOVE_NORMAL,  // 6
    THREAD_PRIORITY_ABOVE_NORMAL,  // 7
    THREAD_PRIORITY_HIGHEST,       // 8
    THREAD_PRIORITY_HIGHEST,       // 9
    THREAD_PRIORITY_TIME_CRITICAL, // 10 ThreadPriorityMax
};

Thread::Thread(const xpr::string *aThreadName)
    : mThreadId(0)
    , mThreadState(ThreadStateNone)
    , mPriority(5)
    , mExitCode(0)
{
    mHandle.mHandle = XPR_NULL;

    if (XPR_IS_NOT_NULL(aThreadName))
    {
        mThreadName = *aThreadName;
    }
    else
    {
        mThreadName = kUnknownThreadName;
    }
}

Thread::~Thread(void)
{
    join();
}

xpr_uint_t XPR_STDCALL Thread::threadEntry(void *sParam)
{
    xpr_uint_t sExitCode = 0;

    Thread *sThread = XPR_NULL;
    Runnable *sRunnable = XPR_NULL;

    ThreadParam *sThreadParam = (ThreadParam *)sParam;
    if (sThreadParam != XPR_NULL)
    {
        sThread = sThreadParam->mThread;
        sRunnable = sThreadParam->mRunnable;

        XPR_SAFE_DELETE(sThreadParam);
    }

    if (sThread != XPR_NULL && sRunnable != XPR_NULL)
    {
        sThread->mThreadState = ThreadStateRunning;

        sExitCode = sRunnable->runThread(*sThread);

        sThread->mExitCode = (xpr_uint_t)sExitCode;
        sThread->mThreadState = ThreadStateTerminated;
    }

    ::_endthreadex(sExitCode);

    return 0;
}

xpr_rcode_t Thread::start(Runnable *aRunnable, xpr_size_t aStackSize)
{
    if (aRunnable == XPR_NULL)
        return XPR_RCODE_EINVAL;

    if (mHandle.mHandle != XPR_NULL)
        return XPR_RCODE_EALREADY;

    ThreadParam *sThreadParam = new ThreadParam;
    if (sThreadParam == XPR_NULL)
        return XPR_RCODE_ENOMEM;

    sThreadParam->mThread = this;
    sThreadParam->mRunnable = aRunnable;

    HANDLE sHandle;
    xpr_uint_t sThreadId = 0;

    sHandle = (HANDLE)::_beginthreadex(XPR_NULL, 0, threadEntry, sThreadParam, 0, &sThreadId);
    if (sHandle == XPR_NULL)
    {
        XPR_SAFE_DELETE(sThreadParam);

        return XPR_RCODE_GET_OS_ERROR();
    }

    setPriority(mPriority);

    mHandle.mHandle = sHandle;
    mThreadId = (ThreadId)sThreadId;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t Thread::join(xpr_sint_t *aExitCode)
{
    xpr_rcode_t sRcode = XPR_RCODE_SUCCESS;
    DWORD sExitCode = 0;

    if (mHandle.mHandle != XPR_NULL)
    {
        DWORD sResult = ::WaitForSingleObject(mHandle.mHandle, INFINITE);
        if (sResult == WAIT_OBJECT_0)
        {
            if (::GetExitCodeThread(mHandle.mHandle, &sExitCode) == XPR_TRUE)
            {
                sRcode = XPR_RCODE_SUCCESS;

                if (XPR_IS_NOT_NULL(aExitCode))
                {
                    *aExitCode = sExitCode;
                }
            }
            else
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();
            }
        }
        else
        {
            sRcode = XPR_RCODE_GET_OS_ERROR();
        }
    }
    else
    {
        sRcode = XPR_RCODE_GET_OS_ERROR();
    }

    ::CloseHandle(mHandle.mHandle);
    mHandle.mHandle = XPR_NULL;

    return sRcode;
}

xpr_rcode_t Thread::detach(void)
{
    if (mHandle.mHandle != XPR_NULL)
    {
        DWORD sResult = ::CloseHandle(mHandle.mHandle);
        if (sResult == 0)
            return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

ThreadId Thread::getThreadId(void) const
{
    return mThreadId;
}

ThreadState Thread::getThreadState(void) const
{
    return mThreadState;
}

xpr_sint_t Thread::getThreadPriority(void) const
{
    return mPriority;
}

void Thread::getThreadName(xpr::string &aThreadName) const
{
    aThreadName = mThreadName;
}

const xpr::string &Thread::getThreadName() const
{
    return mThreadName;
}

Thread::NativeHandle Thread::getThreadHandle(void) const
{
    return mHandle;
}

xpr_sint_t Thread::getExitCode(void) const
{
    return mExitCode;
}

xpr_bool_t Thread::isEqual(const Thread &aThread) const
{
    if (aThread.mHandle.mHandle == this->mHandle.mHandle && aThread.mThreadId == this->mThreadId)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t Thread::isRunning(void) const
{
    ThreadState sThreadState = getThreadState();

    return (sThreadState == ThreadStateRunning) ? XPR_TRUE : XPR_FALSE;
}

void Thread::setPriority(xpr_sint_t aPriority)
{
    if (XPR_IS_RANGE(ThreadPriorityMin, aPriority, ThreadPriorityMax))
    {
        mPriority = gThreadPrirityTable[aPriority];

        if (XPR_IS_NOT_NULL(mHandle.mHandle))
        {
            ::SetThreadPriority(mHandle.mHandle, mPriority);
        }
    }
    else
    {
        // out of range
    }
}

void Thread::swap(Thread &aThread)
{
    NativeHandle sHandle      = aThread.mHandle;
    ThreadId     sThreadId    = aThread.mThreadId;
    ThreadState  sThreadState = aThread.mThreadState;
    xpr_uint_t   sExitCode    = aThread.mExitCode;

    aThread.mHandle.mHandle = this->mHandle.mHandle;
    aThread.mThreadId       = this->mThreadId;
    aThread.mThreadState    = this->mThreadState;
    aThread.mExitCode       = this->mExitCode;

    this->mHandle.mHandle   = sHandle.mHandle;
    this->mThreadId         = sThreadId;
    this->mThreadState      = sThreadState;
    this->mExitCode         = sExitCode;
}

ThreadId Thread::getCurThreadId(void)
{
    return (ThreadId)::GetCurrentThreadId();
}

void Thread::yield(void)
{
    ::SwitchToThread();
}

void Thread::sleep(xpr_time_t aWaitTime)
{
    ::Sleep((DWORD)(aWaitTime / 1000));
}

bool Thread::operator == (const Thread &aThread) const
{
    if (isEqual(aThread) == XPR_TRUE)
        return true;

    return false;
}

bool Thread::operator != (const Thread &aThread) const
{
    if (isEqual(aThread) == XPR_FALSE)
        return true;

    return false;
}
} // namespace xpr
