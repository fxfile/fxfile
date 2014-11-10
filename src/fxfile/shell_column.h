//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SHELL_COLUMN_H__
#define __FXFILE_SHELL_COLUMN_H__ 1
#pragma once

#include "thread.h"
#include "pattern.h"
#include "view_set.h"

namespace fxfile
{
class ShellColumn : public xpr::Thread::Runnable, public fxfile::base::Singleton<ShellColumn>
{
    friend class fxfile::base::Singleton<ShellColumn>;

protected: ShellColumn(void);
public:   ~ShellColumn(void);

public:
    enum
    {
        FlagsNone = 0,
        FlagsEmptyNotify,
        FlagsEqualNotify,
    };

    typedef struct AsyncInfo
    {
        xpr_uint_t      mCode;
        HWND            mHwnd;
        xpr_uint_t      mMsg;
        xpr_sint_t      mIndex;
        xpr_uint_t      mSignature;
        ColumnId        mColumnId;
        xpr_sint_t      mColumnIndex;
        LPSHELLFOLDER2  mShellFolder2;
        LPITEMIDLIST    mPidl;
        xpr_tchar_t    *mOldText;
        xpr_tchar_t    *mText;
        xpr_sint_t      mMaxLen;
        xpr_uint_t      mFlags;

        AsyncInfo(void)
        {
            mCode                 = 0;
            mHwnd                 = XPR_NULL;
            mMsg                  = 0;
            mIndex                = -1;
            mSignature            = 0;
            mColumnId.mFormatId   = GUID_NULL;
            mColumnId.mPropertyId = -1;
            mColumnIndex          = -1;
            mShellFolder2         = XPR_NULL;
            mPidl                 = XPR_NULL;
            mOldText              = XPR_NULL;
            mText                 = XPR_NULL;
            mMaxLen               = 0;
            mFlags                = FlagsNone;
        }

        ~AsyncInfo(void)
        {
            XPR_SAFE_DELETE_ARRAY(mOldText);
            XPR_SAFE_DELETE_ARRAY(mText);
            COM_RELEASE(mShellFolder2);
            COM_FREE(mPidl);
        }

    } AsyncInfo;

public:
    xpr_sint_t getDetailColumn(const ColumnId &aColumnId);
    ColumnInfo *getColumnInfo(const ColumnId &aColumnId);

    xpr_bool_t isAsyncColumn(LPSHELLFOLDER2 aShellFolder2, const ColumnId &aColumnId);
    xpr_bool_t getColumnText(LPSHELLFOLDER2 aShellFolder2, LPITEMIDLIST aPidl, const ColumnId &aColumnId, xpr_tchar_t *aText, xpr_sint_t aMaxLen);
    xpr_bool_t getAsyncColumnText(AsyncInfo *aAsyncInfo);
    void clearAsyncColumn(xpr_uint_t aCode);

    static xpr_bool_t getColumnText(LPSHELLFOLDER2 aShellFolder2, LPITEMIDLIST aPidl, xpr_sint_t aColumnIndex, xpr_tchar_t *aText, xpr_sint_t aMaxLen);
    static xpr_sint_t getAvgCharWidth(CWnd *aWnd);

protected:
    void loadColumn(void);
    xpr_bool_t isLoadedColumn(void);

    void setBaseItem(LPITEMIDLIST aFullPidl);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

protected:
    typedef std::map<ColumnId, ColumnInfo *> ColumnInfoMap;
    ColumnInfoMap mColumnInfoMap;

    typedef std::deque<AsyncInfo *> AsyncDeque;
    AsyncDeque mAsyncDeque;

    Thread     mThread;
    HANDLE     mEvent;
    xpr::Mutex mMutex;

    LPITEMIDLIST mFullPidl;
};
} // namespace fxfile

#endif // __FXFILE_SHELL_COLUMN_H__
