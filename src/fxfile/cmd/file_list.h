//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_LIST_H__
#define __FXFILE_FILE_LIST_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class FileList : public Thread
{
public:
    enum Status
    {
        StatusNone,
        StatusFailed,
        StatusCreating,
        StatusCreateCompleted,
        StatusStopped,
    };

    enum
    {
        FlagsNone      = 0x00000000,
        FlagsByLine    = 0x00000001,
        FlagsOnlyFile  = 0x00000002,
        FlagsWithDir   = 0x00000004,
        FlagsExtension = 0x00000008,
        FlagsAttribute = 0x00000010,
        FlagsSplitChar = 0x00000020,
    };

public:
    FileList(void);
    virtual ~FileList(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    void setTextFile(const xpr_tchar_t *aTextFile);
    void addPath(const xpr_tchar_t *aPath);

    xpr_uint_t getFlags(void);
    void setFlags(xpr_uint_t aFlags);
    void setSplitChar(const xpr_tchar_t *aSplitChar);

    Status getStatus(void);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::tstring> PathDeque;
    PathDeque    mPathDeque;

    xpr_sint_t   mPathMaxLen;
    xpr_sint_t   mFileMaxLen;

    xpr::tstring mTextFilePath;
    xpr_uint_t   mFlags;
    xpr::tstring mSplitChar;

    Status       mStatus;
    xpr::Mutex   mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_LIST_H__
