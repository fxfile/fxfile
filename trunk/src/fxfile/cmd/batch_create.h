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
#define FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH   (1024)
#define FXFILE_BATCH_CREATE_FORMAT_NUMBER_MIN   (0)
#define FXFILE_BATCH_CREATE_FORMAT_NUMBER_DEF   (1)
#define FXFILE_BATCH_CREATE_FORMAT_NUMBER_MAX   (ksint32max)
#define FXFILE_BATCH_CREATE_FORMAT_COUNT_MIN    (1)
#define FXFILE_BATCH_CREATE_FORMAT_COUNT_DEF    (1)
#define FXFILE_BATCH_CREATE_FORMAT_COUNT_MAX    (kuint32max)
#define FXFILE_BATCH_CREATE_FORMAT_INCREASE_MIN (1)
#define FXFILE_BATCH_CREATE_FORMAT_INCREASE_DEF (1)
#define FXFILE_BATCH_CREATE_FORMAT_INCREASE_MAX (kuint32max)
#define FXFILE_BATCH_CREATE_FORMAT_DIGIT_MIN    (1)
#define FXFILE_BATCH_CREATE_FORMAT_DIGIT_DEF    (1)
#define FXFILE_BATCH_CREATE_FORMAT_DIGIT_MAX    (XPR_MAX_PATH)

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
        ResultEqualedName,
        ResultInvalidName,
        ResultWrongFormat,
        ResultInvalidParameter,
        ResultAlreadyExist,
        ResultUnknownError,
    };

public:
    BatchCreate(void);
    virtual ~BatchCreate(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setCreateType(CreateType aCreateType);
    CreateType getCreateType(void) const;

    Result addPath(const xpr::tstring &aPath);
    Result addFormat(const xpr_tchar_t *aDir, const xpr_tchar_t *aFormat, xpr_size_t aCount);
    xpr_size_t getCount(void) const;
    void clear(void);

    Status getStatus(xpr_size_t *aPreparedCount = XPR_NULL, xpr_size_t *aValidatedCount = XPR_NULL, xpr_size_t *aCreatedCount = XPR_NULL) const;
    Result getItemResult(xpr_size_t aIndex) const;
    xpr_sint_t getInvalidItem(void) const;

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;
    CreateType mCreateType;

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
    mutable xpr::Mutex mMutex;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_CREATE_H__
