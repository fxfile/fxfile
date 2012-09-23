//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_ADV_FILE_CHANGE_WATCHER_H__
#define __FXB_ADV_FILE_CHANGE_WATCHER_H__
#pragma once

namespace fxb
{
class AdvFileChangeWatcher : protected fxb::ThreadHandler, public xpr::Singleton<AdvFileChangeWatcher>
{
    friend class xpr::Singleton<AdvFileChangeWatcher>;

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
        std::tstring mDir;
        std::tstring mFileName;
        std::tstring mOldDir;
        std::tstring mOldFileName;
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
        HWND         mHwnd;
        xpr_uint_t   mMsg;
        std::tstring mPath;
        xpr_bool_t   mSubPath;
        LPARAM       mParam;

    protected:
        AdvWatchId   mAdvWatchId;
    };

protected:
    enum TaskCommand;
    struct Task;
    typedef std::list<Task *> TaskList;

    class DriveWatchItem;
    typedef std::map<std::tstring, DriveWatchItem *> DriveWatchMap;
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
    xpr_bool_t OnThreadPreEntry(fxb::Thread &aThread);
    unsigned OnThreadEntryProc(fxb::Thread &aThread);
    unsigned OnMonitorThreadEntryProc(void);
    unsigned OnNotifyThreadEntryProc(void);

protected:
    xpr_bool_t getRootPath(const std::tstring &aPath, std::tstring &aRootPath);

    void processTasks(TaskList &aTaskList);
    void registerTask(Task &aTask);
    void modifyTask(Task &aTask);
    void unregisterTask(Task &aTask);
    void unregisterAllTasks(void);
    void clearTasks(TaskList &aTaskList);

    void clearNotifies(NotifyList &aNotifyList);
    void clearNotifies(NotifyMap &aNotifyMap);

protected:
    fxb::Thread mMonitorThread;
    HANDLE      mTaskEvent;
    HANDLE      mTaskAddEvent;
    TaskList    mTaskList;
    Cs          mTaskCs;

    DriveWatchMap   mDriveWatchMap;
    IdDriveWatchMap mIdDriveWatchMap;

    fxb::Thread mNotifyThread;
    HANDLE      mNotifyEvent;
    NotifyList  mNotifyList;
    NotifyMap   mNotifyMap;
    Cs          mNotifyCs;
};
} // namespace fxb

#endif // __FXB_ADV_FILE_CHANGE_WATCHER_H__
