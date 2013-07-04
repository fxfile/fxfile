//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BATCH_CREATE_H__
#define __FXFILE_BATCH_CREATE_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class BatchCreate : public Thread
{
public:
    enum CreateType
    {
        CreateTypeNone = 0,
        CreateTypeFolder,
        CreateTypeFile,
        CreateTypeTextFile,
    };

    enum Status
    {
        StatusNone,
        StatusPreparing,
        StatusVerifying,
        StatusCreating,
        StatusCreateCompleted,
        StatusStopped,
        StatusInvalid,
    };

    enum Result
    {
        ResultNone,
        ResultSucceeded,
        ResultExcessPathLength,
        ResultEmptiedName,
        ResultEqualedName,
        ResultInvalidName,
        ResultInvalidFormat,
        ResultInvalidParameter,
        ResultAlreadyExist,
        ResultExcessMaxCount,
        ResultUnknownError,
    };

public:
    BatchCreate(void);
    virtual ~BatchCreate(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setCreateType(CreateType aCreateType);
    CreateType getCreateType(void);

    Result addPath(const xpr::tstring &aPath);
    Result addFormat(const xpr_tchar_t *aDir, const xpr_tchar_t *aFormat, xpr_sint_t aStart, xpr_sint_t aIncrease, xpr_sint_t aEndOrCount, xpr_bool_t aCount);
    xpr_size_t getCount(void);
    xpr_size_t getMaxCount(void);
    void clear(void);

    Status getStatus(xpr_size_t *aPreparedCount = XPR_NULL, xpr_size_t *aValidatedCount = XPR_NULL, xpr_size_t *aCreatedCount = XPR_NULL);
    Result getItemResult(xpr_size_t aIndex);
    xpr_sint_t getInvalidItem(void);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;
    CreateType mCreateType;
    const xpr_size_t mMaxCount;

    typedef struct Item
    {
        xpr::tstring mPath;
        Result       mResult;
    } Item;

    typedef std::deque<Item *> NewDeque;
    NewDeque mNewDeque;

    xpr_sint_t mInvalidItem;
    xpr_size_t mPreparedCount;
    xpr_size_t mValidatedCount;
    xpr_size_t mCreatedCount;
    Status     mStatus;
    xpr::Mutex mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_CREATE_H__
