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
static const xpr_tchar_t kUnknownThreadName[] = XPR_STRING_LITERAL("XPR - Unknown Thread");

struct ThreadParam
{
    Thread *mThread;
    ThreadDelegate *mDelegate;
};

Thread::Thread(xpr_tchar_t *aThreadName)
    : mThreadId(0)
    , mThreadName(XPR_NULL)
    , mThreadState(ThreadStateNone)
    , mExitCode(0)
{
    mHandle.mHandle = XPR_NULL;

    if (aThreadName != XPR_NULL)
    {
        mThreadName = new xpr_tchar_t[_tcslen(aThreadName) + 1];
        _tcscpy(mThreadName, aThreadName);
    }
    else
    {
        mThreadName = new xpr_tchar_t[_tcslen(kUnknownThreadName) + 1];
        _tcscpy(mThreadName, kUnknownThreadName);
    }
}

Thread::~Thread(void)
{
    join();

    XPR_SAFE_DELETE_ARRAY(mThreadName);
}

xpr_uint_t XPR_STDCALL Thread::ThreadEntry(void *sParam)
{
    xpr_uint_t sExitCode = 0;

    Thread *sThread = XPR_NULL;
    ThreadDelegate *sDelegate = XPR_NULL;

    ThreadParam *sThreadParam = (ThreadParam *)sParam;
    if (sThreadParam != XPR_NULL)
    {
        sThread = sThreadParam->mThread;
        sDelegate = sThreadParam->mDelegate;

        XPR_SAFE_DELETE(sThreadParam);
    }

    if (sThread != XPR_NULL && sDelegate != XPR_NULL)
    {
        sThread->mThreadState = ThreadStateRunning;

        sExitCode = sDelegate->onThreadMain(*sThread);

        sThread->mExitCode = (xpr_uint_t)sExitCode;
        sThread->mThreadState = ThreadStateTerminated;
    }

    ::_endthreadex(sExitCode);
    return 0;
}

xpr_rcode_t Thread::start(ThreadDelegate *aDelegate, xpr_size_t aStackSize)
{
    if (aDelegate == XPR_NULL)
        return XPR_RCODE_EINVAL;

    if (mHandle.mHandle != XPR_NULL)
        return XPR_RCODE_EALREADY;

    ThreadParam *sThreadParam = new ThreadParam;
    if (sThreadParam == XPR_NULL)
        return XPR_RCODE_ENOMEM;

    sThreadParam->mThread = this;
    sThreadParam->mDelegate = aDelegate;

    xpr_uint_t sThreadId = 0;

    mHandle.mHandle = (HANDLE)::_beginthreadex(XPR_NULL, 0, ThreadEntry, this, 0, &sThreadId);
    if (mHandle.mHandle == XPR_NULL)
        return XPR_RCODE_GET_OS_ERROR();

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
                *aExitCode = sExitCode;
            }

            sRcode = XPR_RCODE_SUCCESS;
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

xpr_rcode_t Thread::getThreadName(xpr_tchar_t *aThreadName) const
{
    if (aThreadName == XPR_NULL)
        return XPR_RCODE_EINVAL;

    if (mThreadName != XPR_NULL)
    {
        aThreadName = new xpr_tchar_t[_tcslen(mThreadName) + 1];
        _tcscpy(aThreadName, mThreadName);
    }
    else
    {
        XPR_ASSERT(0);
    }

    return XPR_RCODE_SUCCESS;
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
