//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_CRC_CREATE_H__
#define __FXB_CRC_CREATE_H__
#pragma once

namespace fxb
{
class CrcCreate : public fxb::Thread
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

    void addPath(const std::tstring &aPath);

    xpr_size_t getCount(void);
    const xpr_tchar_t *getPath(xpr_sint_t aIndex);
    Status getStatus(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    typedef std::deque<std::tstring> PathDeque;
    PathDeque    mPathDeque;

    std::tstring mPath;
    xpr_sint_t   mMethod;
    xpr_bool_t   mEach;

    Status       mStatus;
    xpr_size_t   mProcessedCount;
    xpr_size_t   mSucceededCount;
    Cs           mCs;
};
} // namespace fxb

#endif // __FXB_CRC_CREATE_H__
