//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_PIC_CONV_H__
#define __FXB_PIC_CONV_H__
#pragma once

#include "gfl/libgfl.h"

#ifdef _DEBUG
#include "gfl/libgfle.h"
#endif

namespace fxb
{
class PicConv : public fxb::Thread
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

    xpr_size_t getCount(void);
    Status getStatus(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aSucceededCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

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

    typedef std::deque<std::tstring> PathDeque;
    PathDeque mPathDeque;

    HANDLE     mEvent;

    Status     mStatus;
    xpr_size_t mProcessedCount;
    xpr_size_t mSucceededCount;
    Cs         mCs;
};
} // namespace fxb

#endif // __FXB_PIC_CONV_H__
