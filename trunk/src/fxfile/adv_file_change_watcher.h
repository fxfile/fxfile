//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ADV_FILE_CHANGE_WATCHER_H__
#define __FXFILE_ADV_FILE_CHANGE_WATCHER_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "pattern.h"
#include "thread.h"

namespace fxfile
{
class AdvFileChangeWatcher : protected xpr::Thread::Runnable, public fxfile::base::Singleton<AdvFileChangeWatcher>
{
    friend class fxfile::base::Singleton<AdvFileChangeWatcher>;

protected:
    struct AdvWatchOverlapped;

public:
    typedef xpr_uintptr_t AdvWatchId;
    enum { InvalidAdvWatchId = 0 };

    enum Event
    {
        EventNone = 0,
        EventCreated,
        EventModified,
        EventDeleted,
        EventRenamed,
        EventUpdateDir,
    };

    struct NotifyInfo
    {
        HWND         mHwnd;
        xpr_uint_t   mMsg;
        AdvWatchId   mAdvWatchId;
        Event        mEvent;
        xpr::string  mDir;
        xpr::string  mFileName;
        xpr::string  mOldDir;
        xpr::string  mOldFileName;
        xpr_size_t   mNotifyCount;
        xpr_sint64_t mTime;
    };

    class AdvWatchItem
    {
        friend class AdvFileChangeWatcher;

    public:
        AdvWatchItem(void);
        ~AdvWatchItem(void);

    public:
        void clear(void);
        AdvWatchItem &operator = (const AdvWatchItem &aAdvWatchItem);
        xpr_bool_t isValidate(void);

    public:
        HWND        mHwnd;
        xpr_uint_t  mMsg;
        xpr::string mPath;
        xpr_bool_t  mSubPath;
        LPARAM      mParam;

    protected:
        AdvWatchId   mAdvWatchId;
    };

protected:
    enum TaskCommand;
    struct Task;
    typedef std::list<Task *> TaskList;

    class DriveWatchItem;
    typedef std::map<xpr::string, DriveWatchItem *> DriveWatchMap;
    typedef std::map<AdvWatchId, DriveWatchItem *> IdDriveWatchMap;

    typedef std::list<NotifyInfo *> NotifyList;
    typedef std::map<AdvWatchId, NotifyList> NotifyMap;

protected: AdvFileChangeWatcher(void);
public:   ~AdvFileChangeWatcher(void);

public:
    xpr_bool_t create(void);
    void destroy(void);

public:
    AdvWatchId registerWatch(AdvWatchItem *aAdvWatchItem);
    AdvWatchId modifyWatch(AdvWatchId aOldAdvWatchId, AdvWatchItem *aAdvWatchItem);
    void unregisterWatch(AdvWatchId aAdvWatchId);
    void unregisterAllWatches(void);

protected:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);
    xpr_sint_t runMonitorThread(Thread &aThread);
    xpr_sint_t runNotifyThread(Thread &aThread);

protected:
    xpr_bool_t getRootPath(const xpr::string &aPath, xpr::string &aRootPath);

    void processTasks(TaskList &aTaskList);
    void registerTask(Task &aTask);
    void modifyTask(Task &aTask);
    void unregisterTask(Task &aTask);
    void unregisterAllTasks(void);
    void clearTasks(TaskList &aTaskList);

    void clearNotifies(NotifyList &aNotifyList);
    void clearNotifies(NotifyMap &aNotifyMap);

protected:
    Thread mMonitorThread;
    HANDLE      mTaskEvent;
    HANDLE      mTaskAddEvent;
    TaskList    mTaskList;
    xpr::Mutex  mTaskMutex;

    DriveWatchMap   mDriveWatchMap;
    IdDriveWatchMap mIdDriveWatchMap;

    Thread      mNotifyThread;
    HANDLE      mNotifyEvent;
    NotifyList  mNotifyList;
    NotifyMap   mNotifyMap;
    xpr::Mutex  mNotifyMutex;
};
} // namespace fxfile

#endif // __FXFILE_ADV_FILE_CHANGE_WATCHER_H__
