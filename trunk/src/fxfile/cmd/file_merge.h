//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_MERGE_H__
#define __FXFILE_FILE_MERGE_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class FileMerge : protected xpr::Thread::Runnable
{
public:
    enum Status
    {
        StatusNone,
        StatusDestDirNotExist,
        StatusNotWritable,
        StatusMerging,
        StatusMergeCompleted,
        StatusStopped,
    };

public:
    FileMerge(void);
    virtual ~FileMerge(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void addPath(const xpr_tchar_t *aPath);
    void setDestPath(const xpr_tchar_t *aDestPath);
    void setBufferSize(DWORD aBufferSize);

    xpr_bool_t start(void);
    void stop(void);

    Status getStatus(xpr_size_t *sMergedCount = XPR_NULL);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::string> PathDeque;
    PathDeque    mPathDeque;

    xpr::string  mDestPath;
    DWORD        mBufferSize;

    xpr_size_t   mMergedCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
    Thread       mThread;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_MERGE_H__
