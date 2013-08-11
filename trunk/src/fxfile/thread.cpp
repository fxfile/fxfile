//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "thread.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
Thread::Thread(void)
    : mHandler(XPR_NULL)
    , mThread(XPR_NULL)
    , mStopThread(XPR_NULL)
    , mThreadId(0)
    , mPriority(THREAD_PRIORITY_NORMAL)
{
}

Thread::~Thread(void)
{
    Stop();
}

void Thread::SetThreadHandler(ThreadHandler *aHandler)
{
    mHandler = aHandler;
}

xpr_bool_t Thread::Start(void)
{
    Stop();

    if (OnPreEntry() == XPR_FALSE)
        return XPR_FALSE;

    mStopThread = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread     = (HANDLE)::_beginthreadex(XPR_NULL, 0, &EntryProc, this, 0, &mThreadId);

    if (mThread && mPriority != THREAD_PRIORITY_NORMAL)
        ::SetThreadPriority(mThread, mPriority);

    return (mThread != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t Thread::OnPreEntry(void)
{
    if (XPR_IS_NOT_NULL(mHandler))
        return mHandler->OnThreadPreEntry(*this);

    return XPR_TRUE;
}

void Thread::Stop(DWORD aStopMillseconds, xpr_bool_t aForcely)
{
    if (XPR_IS_NOT_NULL(mThread))
    {
        ::SetEvent(mStopThread);

        DWORD sWait = ::WaitForSingleObject(mThread, aStopMillseconds);

        xpr_bool_t sStoppedThread = XPR_FALSE;
        switch (sWait)
        {
        case WAIT_OBJECT_0:
            {
                sStoppedThread = XPR_TRUE;
                break;
            }

        case WAIT_TIMEOUT:
            {
                if (XPR_IS_TRUE(aForcely))
                {
                    ::TerminateThread(mThread, 0);
                    sStoppedThread = XPR_TRUE;
                }
                break;
            }
        }

        if (XPR_IS_TRUE(sStoppedThread))
        {
            CLOSE_HANDLE(mThread);
            CLOSE_HANDLE(mStopThread);
        }
    }
}

void Thread::StopEvent(void)
{
    if (XPR_IS_NOT_NULL(mThread))
        ::SetEvent(mStopThread);
}

xpr_bool_t Thread::IsRunning(void)
{
    if (XPR_IS_NULL(mThread))
        return XPR_FALSE;

    return ::WaitForSingleObject(mThread, 0) != WAIT_OBJECT_0;
}

unsigned Thread::GetThreadId(void) const
{
    return mThreadId;
}

xpr_sint_t Thread::GetPriority(void)
{
    if (XPR_IS_NOT_NULL(mThread))
        return ::GetThreadPriority(mThread);

    return mPriority;
}

void Thread::SetPriority(xpr_sint_t aPriority)
{
    if (XPR_IS_NOT_NULL(mThread))
        ::SetThreadPriority(mThread, aPriority);

    mPriority = aPriority;
}

bool Thread::operator == (const Thread &aThread) const
{
    return (GetThreadId() == aThread.GetThreadId()) ? true : false;
}

bool Thread::operator != (const Thread &aThread) const
{
    return (GetThreadId() != aThread.GetThreadId()) ? true : false;
}

unsigned __stdcall Thread::EntryProc(void *aParam)
{
    DWORD sExitCode = 0;

    Thread *sThread = (Thread *)aParam;
    if (XPR_IS_NOT_NULL(sThread))
        sExitCode = sThread->OnEntryProc();

    ::_endthreadex(sExitCode);
    return 0;
}

unsigned Thread::OnEntryProc(void)
{
    if (XPR_IS_NOT_NULL(mHandler))
        mHandler->OnThreadEntryProc(*this);

    return 0;
}

xpr_bool_t Thread::IsStop(void)
{
    return ::WaitForSingleObject(mStopThread, 0) == WAIT_OBJECT_0;
}
} // namespace fxfile
