//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_COMBINE_H__
#define __FXFILE_FILE_COMBINE_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class FileCombine : public Thread
{
public:
    enum Status
    {
        StatusNone,
        StatusFileNotExist,
        StatusDestDirNotExist,
        StatusNotWritable,
        StatusNotCombine,
        StatusCombining,
        StatusCombineCompleted,
        StatusInvalidCrcCode,
        StatusStopped,
    };

public:
    FileCombine(void);
    virtual ~FileCombine(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setPath(const xpr_tchar_t *aPath);
    void setDestDir(const xpr_tchar_t *aDestDir);
    void setBufferSize(DWORD aBufferSize);

    Status GetStatus(xpr_size_t *aCombinedCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    xpr::tstring mPath;
    xpr::tstring mDestDir;
    DWORD        mBufferSize;

    xpr_size_t   mCombinedCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_COMBINE_H__
