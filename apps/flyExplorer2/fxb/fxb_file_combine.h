//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_COMBINE_H__
#define __FXB_FILE_COMBINE_H__
#pragma once

namespace fxb
{
class FileCombine : public fxb::Thread
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

    std::tstring mPath;
    std::tstring mDestDir;
    DWORD        mBufferSize;

    xpr_size_t   mCombinedCount;
    Status       mStatus;
    Cs           mCs;
};
} // namespace fxb

#endif // __FXB_FILE_COMBINE_H__
