//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_MERGE_H__
#define __FXB_FILE_MERGE_H__
#pragma once

namespace fxb
{
class FileMerge : public fxb::Thread
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

    typedef std::deque<std::tstring> PathDeque;
    PathDeque    mPathDeque;

    std::tstring mDestPath;
    DWORD        mBufferSize;

    xpr_size_t   mMergedCount;
    Status       mStatus;
    Cs           mCs;
};
} // namespace fxb

#endif // __FXB_FILE_MERGE_H__
