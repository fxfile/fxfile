//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_CHANGE_WATCHER_H__
#define __FXFILE_FILE_CHANGE_WATCHER_H__ 1
#pragma once

#include "pattern.h"
#include "thread.h"

namespace fxfile
{
class FileChangeWatcher : protected xpr::Thread::Runnable, public fxfile::base::Singleton<FileChangeWatcher>
{
    friend class fxfile::base::Singleton<FileChangeWatcher>;

public:
    typedef xpr_uintptr_t WatchId;
    enum { InvalidWatchId = 0 };

    class WatchItem
    {
        friend class FileChangeWatcher;

    public:
        WatchItem(void)
        {
            clear();
            mTime = 0i64;
        }

        void clear(void)
        {
            mHwnd = XPR_NULL;
            mMsg = XPR_NULL;
            mPath.clear();
            mSubPath = XPR_FALSE;
            mParam = XPR_NULL;
        }

        WatchItem& operator = (const WatchItem &aSrc)
        {
            mHwnd    = aSrc.mHwnd;
            mMsg     = aSrc.mMsg;
            mPath    = aSrc.mPath;
            mSubPath = aSrc.mSubPath;
            mParam   = aSrc.mParam;

            return *this;
        }

    public:
        HWND        mHwnd;
        xpr_uint_t  mMsg;
        xpr::string mPath;
        xpr_bool_t  mSubPath;
        LPARAM      mParam;

    protected:
        xpr_sint64_t mTime;
    };

protected: FileChangeWatcher(void);
public:   ~FileChangeWatcher(void);

public:
    xpr_bool_t create(void);
    void destroy(void);

public:
    WatchId registerWatch(WatchItem *aWatchItem);
    WatchId modifyWatch(WatchId aWatchId, WatchItem *aWatchItem);
    void unregisterWatch(WatchId aWatchId);
    void unregisterAllWatches(void);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

    xpr_sint_t delayNotify(HANDLE aNotifyHandle, xpr_bool_t aResetTimer);

protected:
    typedef std::map<HANDLE, WatchItem *> WatchMap;
    WatchMap   mWatchMap;
    xpr::Mutex mMutex;

    Thread   mThread;
    HANDLE   mEvent;
};
} // namespace fxfile

#endif // __FXFILE_FILE_CHANGE_WATCHER_H__
