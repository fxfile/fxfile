//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TEXT_MERGE_H__
#define __FXFILE_TEXT_MERGE_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class TextMerge : public Thread
{
public:
    enum Status
    {
        StatusNone,
        StatusFailed,
        StatusMerging,
        StatusMergeCompleted,
        StatusStopped,
    };

public:
    TextMerge(void);
    virtual ~TextMerge(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    void setTextFile(const xpr_tchar_t *aTextFile);
    void setEncoding(xpr_bool_t aEncoding);
    void addPath(const xpr_tchar_t *aPath);

    Status getStatus(xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::tstring> PathDeque;
    PathDeque    mPathDeque;

    xpr::tstring mTextFile;
    xpr_bool_t   mEncoding;

    xpr_size_t   mSucceededCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TEXT_MERGE_H__
