//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CRC_CREATE_H__
#define __FXFILE_CRC_CREATE_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class CrcCreate : public Thread
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

public:
    CrcCreate(void);
    virtual ~CrcCreate(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    void setMethod(xpr_sint_t aMethod);
    void setEach(xpr_bool_t aEach);
    void setPath(const xpr_tchar_t *aPath);

    void addPath(const xpr::string &aPath);

    xpr_size_t getCount(void);
    const xpr_tchar_t *getPath(xpr_sint_t aIndex);
    Status getStatus(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<xpr::string> PathDeque;
    PathDeque    mPathDeque;

    xpr::string  mPath;
    xpr_sint_t   mMethod;
    xpr_bool_t   mEach;

    Status       mStatus;
    xpr_size_t   mProcessedCount;
    xpr_size_t   mSucceededCount;
    xpr::Mutex   mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CRC_CREATE_H__
