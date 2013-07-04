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

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class FileMerge : public Thread
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

    Status getStatus(xpr_size_t *sMergedCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::tstring> PathDeque;
    PathDeque    mPathDeque;

    xpr::tstring mDestPath;
    DWORD        mBufferSize;

    xpr_size_t   mMergedCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_MERGE_H__
