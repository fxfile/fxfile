//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_THREAD_H__
#define __FXFILE_THREAD_H__ 1
#pragma once

namespace fxfile
{
class Thread;

class ThreadHandler
{
public:
    virtual xpr_bool_t OnThreadPreEntry(Thread &aThread) = 0;
    virtual unsigned OnThreadEntryProc(Thread &aThread) = 0;
};

class Thread
{
public:
    Thread(void);
    virtual ~Thread(void);

public:
    void SetThreadHandler(ThreadHandler *aHandler);

public:
    xpr_bool_t Start(void);
    void Stop(DWORD aStopMillseconds = INFINITE, xpr_bool_t aForcely = XPR_TRUE);
    void StopEvent(void);

    xpr_bool_t IsRunning(void);
    xpr_bool_t IsStop(void);
    unsigned GetThreadId(void) const;

    xpr_sint_t GetPriority(void);
    void SetPriority(xpr_sint_t aPriority);

    bool operator == (const Thread &aThread) const;
    bool operator != (const Thread &aThread) const;

protected:
    virtual xpr_bool_t OnPreEntry(void);

    virtual unsigned OnEntryProc(void);

private:
    static unsigned __stdcall EntryProc(void *aParam);

protected:
    ThreadHandler *mHandler;

    HANDLE     mThread;
    HANDLE     mStopThread;
    unsigned   mThreadId;
    xpr_sint_t mPriority;
};
} // namespace fxfile

#endif // __FXFILE_THREAD_H__
