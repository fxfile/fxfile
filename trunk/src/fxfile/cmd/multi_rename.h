//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_MULTI_RENAME_H__
#define __FXFILE_MULTI_RENAME_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "thread.h"

namespace fxfile
{
namespace cmd
{
class MultiRename : protected xpr::Thread::Runnable
{
public:
    enum
    {
        FlagReadOnlyRename,
    };

    enum Status
    {
        StatusNone,
        StatusPreparing,
        StatusValidating,
        StatusRenaming,
        StatusRenameCompleted,
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
        ResultPathNotExist,
        ResultReadOnly,
        ResultShared,
        ResultUnknownError,
    };

public:
    MultiRename(void);
    virtual ~MultiRename(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setBackupName(const xpr_tchar_t *aBackup);
    void setFlags(xpr_uint_t aFlags);
    xpr_uint_t getFlags(void);
    xpr_bool_t isFlag(xpr_uint_t aFlag);

    void addPath(const xpr_tchar_t *aDir, const xpr_tchar_t *aOld, const xpr_tchar_t *aNew);
    void addPath(const xpr::string &aDir, const xpr::string &aOld, const xpr::string &aNew);

    xpr_bool_t start(void);
    void stop(void);

    Status getStatus(xpr_size_t *aPreparedCount = XPR_NULL, xpr_size_t *aValidatedCount = XPR_NULL, xpr_size_t *aRenamedCount = XPR_NULL);
    Result getItemResult(xpr_size_t aIndex);
    xpr_sint_t getInvalidItem(void);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;

    struct RenItem
    {
        xpr::string mDir;
        xpr::string mOld;
        xpr::string mNew;
        Result      mResult;
    };

    typedef std::deque<RenItem *> RenDeque;
    RenDeque     mRenDeque;

    xpr::string  mBackup;
    xpr_uint_t   mFlags;

    xpr_sint_t   mInvalidItem;
    xpr_size_t   mPreparedCount;
    xpr_size_t   mValidatedCount;
    xpr_size_t   mRenamedCount;
    Status       mStatus;
    xpr::Mutex   mMutex;
    Thread       mThread;
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_MULTI_RENAME_H__
