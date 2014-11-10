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

#include "xpr_mutex.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class TextMerge : protected xpr::Thread::Runnable
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

    xpr_bool_t start(void);
    void stop(void);

    Status getStatus(xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::string> PathDeque;
    PathDeque    mPathDeque;

    xpr::string  mTextFile;
    xpr_bool_t   mEncoding;

    xpr_size_t   mSucceededCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
    Thread       mThread;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TEXT_MERGE_H__
