//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_SPLIT_H__
#define __FXB_FILE_SPLIT_H__
#pragma once

namespace fxb
{
class FileSplit : public fxb::Thread
{
public:
    enum
    {
        FlagCrcFile    = 0x00000001,
        FlagDelOrgFile = 0x00000002,
        FlagBatFile    = 0x00000004,
    };

    enum Status
    {
        StatusNone,
        StatusFileNotExist,
        StatusDestDirNotExist,
        StatusNotWritable,
        StatusNotReadable,
        StatusSplitting,
        StatusSplitCompleted,
        StatusStopped,
    };

public:
    FileSplit(void);
    virtual ~FileSplit(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setPath(const xpr_tchar_t *aPath, const xpr_tchar_t *aDestDir);
    xpr_uint_t getFlags(void);
    void setFlags(xpr_uint_t aFlags);
    void setSplitSize(xpr_sint64_t aUnitSize);
    void setBufferSize(xpr_size_t aBufferSize);

    Status getStatus(xpr_size_t *aSplitedCount = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

    void setStatus(Status aStatus);

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    std::tstring mPath;
    std::tstring mDestDir;
    xpr_uint_t   mFlags;
    xpr_sint64_t mUnitSize;
    xpr_size_t   mBufferSize;

    xpr_size_t   mSplitedCount;
    Status       mStatus;
    Cs           mCs;
};
} // namespace fxb

#endif // __FXB_FILE_SPLIT_H__
