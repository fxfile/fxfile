//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CRC_VERIFY_H__
#define __FXFILE_CRC_VERIFY_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class CrcVerify : public Thread
{
public:
    enum Status
    {
        StatusNone,
        StatusVerifying,
        StatusVerifyCompleted,
        StatusStopped,
    };

    enum VerifyResult
    {
        VerifyResultNone,
        VerifyResultFailed,
        VerifyResultNotEqualed,
        VerifyResultEqualed,
    };

    typedef xpr_char_t CrcVal[64];

    typedef struct VerifyFile
    {
        void clear(void)
        {
            mPath.clear();
            memset(mCrcCode, 0, sizeof(mCrcCode));
            mMethod = 0;
            mResult = VerifyResultNone;
        }

        xpr::tstring mPath;
        CrcVal       mCrcCode;
        xpr_sint_t   mMethod;
        VerifyResult mResult;
    } VerifyFile;

public:
    CrcVerify(void);
    virtual ~CrcVerify(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    void addCrcPath(const xpr_tchar_t *aPath);
    void init(void);

    xpr_size_t getCrcCount(void);
    xpr_size_t getVerifyCount(xpr_size_t aCrc);
    const xpr_tchar_t *getCrcPath(xpr_size_t aCrc);
    VerifyFile *getVerifyFile(xpr_size_t aCrc, xpr_size_t aVerify);
    Status getStatus(void);
    xpr_sint_t getResult(xpr_size_t *aProcessedCount = XPR_NULL, xpr_size_t *aNotEqualedCount = XPR_NULL, xpr_size_t *aFailedCount = XPR_NULL);

protected:
    void resetVerifyResult(void);

    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;

    typedef std::deque<VerifyFile> VerifyFileDeque;

    typedef struct CrcFile
    {
        xpr::tstring    mPath;
        VerifyFileDeque mVerifyFileDeque;
    } CrcFile;

    typedef std::deque<CrcFile> CrcFileDeque;
    CrcFileDeque mCrcFileDeque;

    Status     mStatus;
    xpr::Mutex mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CRC_VERIFY_H__
