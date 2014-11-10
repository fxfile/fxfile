//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SPLIT_H__
#define __FXFILE_FILE_SPLIT_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class FileSplit : protected xpr::Thread::Runnable
{
public:
    enum
    {
        FlagCrcFile    = 0x00000001,
        FlagDelOrgFile = 0x00000002,
        FlagBatFile    = 0x00000004,
    };

    enum Status
    {
        StatusNone,
        StatusFileNotExist,
        StatusDestDirNotExist,
        StatusNotWritable,
        StatusNotReadable,
        StatusSplitting,
        StatusSplitCompleted,
        StatusStopped,
    };

public:
    FileSplit(void);
    virtual ~FileSplit(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setPath(const xpr_tchar_t *aPath, const xpr_tchar_t *aDestDir);
    xpr_uint_t getFlags(void);
    void setFlags(xpr_uint_t aFlags);
    void setSplitSize(xpr_sint64_t aUnitSize);
    void setBufferSize(xpr_size_t aBufferSize);

    xpr_bool_t start(void);
    void stop(void);

    Status getStatus(xpr_size_t *aSplitedCount = XPR_NULL);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

    void setStatus(Status aStatus);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    xpr::string  mPath;
    xpr::string  mDestDir;
    xpr_uint_t   mFlags;
    xpr_sint64_t mUnitSize;
    xpr_size_t   mBufferSize;

    xpr_size_t   mSplitedCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
    Thread       mThread;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_SPLIT_H__
