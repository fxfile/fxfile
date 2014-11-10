//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PIC_CONV_H__
#define __FXFILE_PIC_CONV_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "thread.h"

#include "gfl/libgfl.h"

#ifdef _DEBUG
#include "gfl/libgfle.h"
#endif

namespace fxfile
{
namespace cmd
{
class PicConv : protected xpr::Thread::Runnable
{
public:
    enum Status
    {
        StatusNone,
        StatusConverting,
        StatusConvertCompleted,
        StatusStopped,
    };

public:
    PicConv(void);
    virtual ~PicConv(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg, xpr_uint_t aMsgClr);
    void addPath(const xpr_tchar_t *aPath);

    void setSavParams(GFL_SAVE_PARAMS *aGflSaveParams);
    void setKeepOrg(xpr_bool_t aKeepOrg);
    void setColorMode(xpr_bool_t aAllApply, GFL_MODE aGflMode, GFL_MODE_PARAMS aGflModeParams);

    xpr_bool_t start(void);
    void stop(void);

    xpr_size_t getCount(void);
    Status getStatus(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

protected:
    xpr_bool_t       mKeepOrg;

    GFL_SAVE_PARAMS *mGflSaveParams;

    xpr_bool_t       mAllApply;
    GFL_MODE         mGflMode;
    GFL_MODE_PARAMS  mGflModeParams;

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;
    xpr_uint_t mMsgClr;

    typedef std::deque<xpr::string> PathDeque;
    PathDeque mPathDeque;

    HANDLE     mEvent;

    Status     mStatus;
    xpr_size_t mProcessedCount;
    xpr_size_t mSucceededCount;
    xpr::Mutex mMutex;
    Thread     mThread;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PIC_CONV_H__
