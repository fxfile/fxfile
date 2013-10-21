//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "adv_file_change_watcher.h"

#include "functors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
const DWORD kIdleTime = 20;
const xpr_size_t kBufferSize = sizeof(FILE_NOTIFY_INFORMATION) * 1024;

struct AdvFileChangeWatcher::AdvWatchOverlapped : public OVERLAPPED
{
    xpr_byte_t      mBuffer[kBufferSize];
    DriveWatchItem *mDriveWatchItem;
    xpr::tstring    mOldFileName;
    xpr::tstring    mRootPath;
};

enum AdvFileChangeWatcher::TaskCommand
{
    TaskCommandNone,
    TaskCommandRegister,
    TaskCommandModify,
    TaskCommandUnregister,
    TaskCommandUnregisterAll,
};

struct AdvFileChangeWatcher::Task
{
    Task(void)
        : mTaskCommand(TaskCommandNone)
        , mOldAdvWatchId(InvalidAdvWatchId)
        , mAdvWatchItem(XPR_NULL)
    {
    }

    ~Task(void)
    {
        XPR_SAFE_DELETE(mAdvWatchItem);
    }

    TaskCommand   mTaskCommand;
    AdvWatchId    mOldAdvWatchId;
    AdvWatchItem *mAdvWatchItem;
};

AdvFileChangeWatcher::AdvWatchItem::AdvWatchItem(void)
{
    clear();
    mAdvWatchId = 0;
}

AdvFileChangeWatcher::AdvWatchItem::~AdvWatchItem(void)
{
    clear();
}

void AdvFileChangeWatcher::AdvWatchItem::clear(void)
{
    mHwnd = XPR_NULL;
    mMsg = XPR_NULL;
    mPath.clear();
    mSubPath = XPR_FALSE;
    mParam = XPR_NULL;
}

AdvFileChangeWatcher::AdvWatchItem& AdvFileChangeWatcher::AdvWatchItem::operator = (const AdvWatchItem &aAdvWatchItem)
{
    mHwnd       = aAdvWatchItem.mHwnd;
    mMsg        = aAdvWatchItem.mMsg;
    mPath       = aAdvWatchItem.mPath;
    mSubPath    = aAdvWatchItem.mSubPath;
    mParam      = aAdvWatchItem.mParam;
    mAdvWatchId = aAdvWatchItem.mAdvWatchId;

    return *this;
}

xpr_bool_t AdvFileChangeWatcher::AdvWatchItem::isValidate(void)
{
    if (XPR_IS_NULL(mHwnd) || mMsg == 0 || mPath.empty())
        return XPR_FALSE;

    if (IsExistFile(mPath) == XPR_FALSE || IsFileSystemFolder(mPath) == XPR_FALSE)
        return XPR_FALSE;

    if (*mPath.rbegin() == XPR_STRING_LITERAL('\\'))
        mPath.erase(mPath.length()-1);

    return XPR_TRUE;
}

class AdvFileChangeWatcher::DriveWatchItem
{
public:
    DriveWatchItem(void)
        : mDirectory(XPR_NULL)
        , mCompletionPort(XPR_NULL)
        , mOverlapped(XPR_NULL)
    {
    }

    ~DriveWatchItem(void)
    {
        unregisterAllWatches();

        CLOSE_HANDLE(mCompletionPort);
        CLOSE_HANDLE(mDirectory);

        XPR_SAFE_DELETE(mOverlapped);
    }

    AdvWatchId registerWatch(const xpr::tstring &aRootPath, AdvWatchItem *aNewAdvWatchItem)
    {
        if (aRootPath.empty() == XPR_TRUE || XPR_IS_NULL(aNewAdvWatchItem))
            return InvalidAdvWatchId;

        if (XPR_IS_NULL(mDirectory))
        {
            mRootPath = aRootPath;

            xpr::tstring sDrivePath(aRootPath);
            sDrivePath += XPR_STRING_LITERAL('\\');

            mDirectory = ::CreateFile(
                sDrivePath.c_str(),
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                XPR_NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                XPR_NULL);

            if (mDirectory == INVALID_HANDLE_VALUE)
            {
                mDirectory = XPR_NULL;
                return InvalidAdvWatchId;
            }

            mCompletionPort = ::CreateIoCompletionPort(
                mDirectory,
                mCompletionPort,
                (ULONG_PTR)aNewAdvWatchItem,
                0);

            if (XPR_IS_NULL(mCompletionPort))
            {
                CLOSE_HANDLE(mDirectory);
                return InvalidAdvWatchId;
            }
        }

        aNewAdvWatchItem->mAdvWatchId = (AdvWatchId)aNewAdvWatchItem;

        mWatchList.push_back(aNewAdvWatchItem);
        mIdWatchMap[aNewAdvWatchItem->mAdvWatchId] = aNewAdvWatchItem;

        return aNewAdvWatchItem->mAdvWatchId;
    }

    void unregisterWatch(AdvWatchId aAdvWatchId)
    {
        IdWatchMap::iterator sIterator = mIdWatchMap.find(aAdvWatchId);
        if (sIterator == mIdWatchMap.end())
            return;

        AdvWatchItem *sAdvWatchItem = sIterator->second;

        mWatchList.remove(sAdvWatchItem);
        mIdWatchMap.erase(sIterator);

        XPR_SAFE_DELETE(sAdvWatchItem);
    }

    void unregisterAllWatches(void)
    {
        WatchList::iterator sIterator;
        AdvWatchItem *sAdvWatchItem;

        sIterator = mWatchList.begin();
        for (; sIterator != mWatchList.end(); ++sIterator)
        {
            sAdvWatchItem = *sIterator;
            XPR_SAFE_DELETE(sAdvWatchItem);
        }

        mWatchList.clear();
        mIdWatchMap.clear();
    }

    AdvWatchItem *getRegisteredItem(AdvWatchId aAdvWatchId)
    {
        IdWatchMap::iterator sIterator = mIdWatchMap.find(aAdvWatchId);
        if (sIterator == mIdWatchMap.end())
            return XPR_NULL;

        return sIterator->second;
    }

    xpr_size_t getRegisteredCount(void)
    {
        return mWatchList.size();
    }

    xpr_bool_t readDirectoryChanges(void)
    {
        if (XPR_IS_NULL(mOverlapped))
        {
            mOverlapped = new AdvWatchOverlapped;
            mOverlapped->hEvent          = XPR_NULL;
            mOverlapped->mDriveWatchItem = this;
            mOverlapped->mRootPath       = mRootPath;
        }

        // if notify filter contains 'FILE_NOTIFY_CHANGE_LAST_ACCESS',
        // then it don't contain because occur too many events.

        const DWORD sNotifyFilter = 
            FILE_NOTIFY_CHANGE_FILE_NAME   |
            FILE_NOTIFY_CHANGE_DIR_NAME    |
            FILE_NOTIFY_CHANGE_ATTRIBUTES  |
            FILE_NOTIFY_CHANGE_SIZE        |
            FILE_NOTIFY_CHANGE_LAST_WRITE  |
            //FILE_NOTIFY_CHANGE_LAST_ACCESS |
            FILE_NOTIFY_CHANGE_CREATION;//    |
        //FILE_NOTIFY_CHANGE_SECURITY;

        DWORD sBytesReturned = 0;
        DWORD sBufferSize = static_cast<DWORD>(kBufferSize);

        xpr_bool_t sResult = ::ReadDirectoryChangesW(
            mDirectory,
            mOverlapped->mBuffer,
            sBufferSize, 
            XPR_TRUE,
            sNotifyFilter,
            &sBytesReturned,
            mOverlapped,
            XPR_NULL);

        return sResult;
    }

    LPOVERLAPPED getQueuedCompletionStatus(DWORD aMilliseconds)
    {
        DWORD sNumberOfBytes;
        ULONG_PTR sCompletionKey;
        LPOVERLAPPED sOverlapped;

        xpr_bool_t sResult = ::GetQueuedCompletionStatus(
            mCompletionPort,
            &sNumberOfBytes,
            &sCompletionKey,
            &sOverlapped,
            aMilliseconds);

        if (XPR_IS_FALSE(sResult))
            return XPR_NULL;

        return sOverlapped;
    }

    void extractFileName(const FILE_NOTIFY_INFORMATION *aFileNotifyInfo, xpr::tstring &aFileName)
    {
        xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
        xpr_size_t sInputBytes = aFileNotifyInfo->FileNameLength * sizeof(xpr_wchar_t);
        xpr_size_t sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
        XPR_UTF16_TO_TCS(aFileNotifyInfo->FileName, &sInputBytes, sFileName, &sOutputBytes);
        sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        aFileName = sFileName;
    }

    void OnCompletionRoutine(LPOVERLAPPED aOverlapped, NotifyList &aNotifyList)
    {
        AdvFileChangeWatcher::AdvWatchOverlapped *sAdvWatchOverlapped = reinterpret_cast<AdvFileChangeWatcher::AdvWatchOverlapped *>(aOverlapped);

        FILE_NOTIFY_INFORMATION *sFileNotifyInformation = XPR_NULL;
        xpr_size_t sOffset = 0;

        do
        {
            sFileNotifyInformation = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&sAdvWatchOverlapped->mBuffer[sOffset]);
            sOffset += sFileNotifyInformation->NextEntryOffset;

            OnFileChanged(sAdvWatchOverlapped, sFileNotifyInformation, aNotifyList);

        } while (sFileNotifyInformation->NextEntryOffset != 0);

        readDirectoryChanges();
    }

    void OnFileChanged(AdvWatchOverlapped *aAdvWatchOverlapped, FILE_NOTIFY_INFORMATION *aFileNotifyInformation, NotifyList &aNotifyList)
    {
        if (XPR_IS_NULL(aAdvWatchOverlapped) || XPR_IS_NULL(aFileNotifyInformation))
            return;

        if (aFileNotifyInformation->Action == FILE_ACTION_RENAMED_OLD_NAME)
        {
            extractFileName(aFileNotifyInformation, aAdvWatchOverlapped->mOldFileName);
            return;
        }

        Event sEvent = EventNone;

        switch (aFileNotifyInformation->Action)
        {
        case FILE_ACTION_ADDED:            sEvent = EventCreated;  break;
        case FILE_ACTION_MODIFIED:         sEvent = EventModified; break;
        case FILE_ACTION_REMOVED:          sEvent = EventDeleted;  break;
        case FILE_ACTION_RENAMED_NEW_NAME: sEvent = EventRenamed;  break;

        default:
            return;
        }

        const xpr::tstring &sRootPath = aAdvWatchOverlapped->mRootPath;
        xpr_size_t sRootPathLen = sRootPath.length();

        xpr::tstring sFileName;
        extractFileName(aFileNotifyInformation, sFileName);

        xpr::tstring sDir = sRootPath;
        xpr_size_t sFind = sFileName.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::tstring::npos)
        {
            sDir += XPR_STRING_LITERAL('\\');
            sDir += sFileName.substr(0, sFind);
            sFileName.erase(0, sFind+1);
        }

        DriveWatchItem *sDriveWatchItem = aAdvWatchOverlapped->mDriveWatchItem;
        AdvWatchItem *sTargetAdvWatchItem = XPR_NULL;

        {
            DriveWatchItem::WatchList::iterator sIterator;
            AdvWatchItem *sAdvWatchItem;

            sIterator = sDriveWatchItem->mWatchList.begin();
            for (; sIterator != sDriveWatchItem->mWatchList.end(); ++sIterator)
            {
                sAdvWatchItem = *sIterator;
                if (XPR_IS_NULL(sAdvWatchItem))
                    continue;

                if (sDir.length() != sAdvWatchItem->mPath.length())
                    continue;

                if (!_tcsicmp(sDir.c_str(), sAdvWatchItem->mPath.c_str()))
                {
                    sTargetAdvWatchItem = sAdvWatchItem;
                    break;
                }
            }
        }

        if (XPR_IS_NULL(sTargetAdvWatchItem))
            return;

        NotifyInfo *sNotifyInfo = new NotifyInfo;
        if (XPR_IS_NULL(sNotifyInfo))
            return;

        sNotifyInfo->mHwnd        = sTargetAdvWatchItem->mHwnd;
        sNotifyInfo->mMsg         = sTargetAdvWatchItem->mMsg;
        sNotifyInfo->mAdvWatchId  = sTargetAdvWatchItem->mAdvWatchId;
        sNotifyInfo->mEvent       = sEvent;
        sNotifyInfo->mDir         = sDir;
        sNotifyInfo->mFileName    = sFileName;
        sNotifyInfo->mTime        = xpr::timer_ms();
        sNotifyInfo->mNotifyCount = 1;

        if (sEvent == EventRenamed)
        {
            sNotifyInfo->mOldDir = sRootPath;
            sFind = aAdvWatchOverlapped->mOldFileName.rfind(XPR_STRING_LITERAL('\\'));
            if (sFind != xpr::tstring::npos)
            {
                sNotifyInfo->mOldDir += XPR_STRING_LITERAL('\\');
                sNotifyInfo->mOldDir += aAdvWatchOverlapped->mOldFileName.substr(0, sFind);
                sNotifyInfo->mOldFileName = aAdvWatchOverlapped->mOldFileName.substr(sFind+1);
            }
        }

        aAdvWatchOverlapped->mOldFileName.clear();

        aNotifyList.push_back(sNotifyInfo);
    }

public:
    xpr::tstring        mRootPath;
    HANDLE              mDirectory;
    HANDLE              mCompletionPort;
    AdvWatchOverlapped *mOverlapped;

    HANDLE              mStopEvent;
    HANDLE              mClosedEvent;

    typedef std::list<AdvWatchItem *> WatchList;
    typedef std::map<AdvWatchId, AdvWatchItem *> IdWatchMap;

    WatchList           mWatchList;
    IdWatchMap          mIdWatchMap;
};

//
// AdvFileChangeWatcher
//
AdvFileChangeWatcher::AdvFileChangeWatcher(void)
    : mTaskEvent(XPR_NULL)
    , mTaskAddEvent(XPR_NULL)
    , mNotifyEvent(XPR_NULL)
{
}

AdvFileChangeWatcher::~AdvFileChangeWatcher(void)
{
    destroy();

    CLOSE_HANDLE(mTaskEvent);
    CLOSE_HANDLE(mTaskAddEvent);
    CLOSE_HANDLE(mNotifyEvent);
}

xpr_bool_t AdvFileChangeWatcher::create(void)
{
    CLOSE_HANDLE(mTaskEvent);
    CLOSE_HANDLE(mTaskAddEvent);
    CLOSE_HANDLE(mNotifyEvent);

    mTaskEvent    = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mTaskAddEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mNotifyEvent  = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);

    if (XPR_IS_NULL(mTaskEvent) || XPR_IS_NULL(mTaskAddEvent) || XPR_IS_NULL(mNotifyEvent))
    {
        destroy();
        return XPR_FALSE;
    }

    mMonitorThread.SetThreadHandler(dynamic_cast<ThreadHandler *>(this));
    mNotifyThread.SetThreadHandler(dynamic_cast<ThreadHandler *>(this));

    if (mMonitorThread.Start() == XPR_FALSE)
    {
        destroy();
        return XPR_FALSE;
    }

    if (mNotifyThread.Start() == XPR_FALSE)
    {
        destroy();
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

void AdvFileChangeWatcher::destroy(void)
{
    unregisterAllTasks();

    mNotifyThread.Stop();
    mMonitorThread.Stop();

    clearTasks(mTaskList);
    clearNotifies(mNotifyList);
    clearNotifies(mNotifyMap);

    CLOSE_HANDLE(mTaskEvent);
    CLOSE_HANDLE(mTaskAddEvent);
    CLOSE_HANDLE(mNotifyEvent);
}

AdvFileChangeWatcher::AdvWatchId AdvFileChangeWatcher::registerWatch(AdvWatchItem *aAdvWatchItem)
{
    if (XPR_IS_NULL(aAdvWatchItem))
        return InvalidAdvWatchId;

    if (aAdvWatchItem->isValidate() == XPR_FALSE)
        return InvalidAdvWatchId;

    {
        xpr::MutexGuard sLockGuard(mTaskMutex);

        Task *sTask = new Task;
        sTask->mTaskCommand  = TaskCommandRegister;
        sTask->mAdvWatchItem = aAdvWatchItem;

        aAdvWatchItem->mAdvWatchId = (AdvWatchId)aAdvWatchItem;

        mTaskList.push_back(sTask);

        if (mMonitorThread.IsRunning() == XPR_FALSE)
            create();

        ::SetEvent(mTaskEvent);
    }

    return aAdvWatchItem->mAdvWatchId;
}

AdvFileChangeWatcher::AdvWatchId AdvFileChangeWatcher::modifyWatch(AdvWatchId aOldAdvWatchId, AdvWatchItem *aAdvWatchItem)
{
    if (XPR_IS_NULL(aAdvWatchItem))
        return InvalidAdvWatchId;

    if (aOldAdvWatchId == InvalidAdvWatchId)
        return registerWatch(aAdvWatchItem);

    if (mMonitorThread.IsRunning() == XPR_FALSE)
        return InvalidAdvWatchId;

    {
        xpr::MutexGuard sLockGuard(mTaskMutex);

        Task *sTask = new Task;
        sTask->mTaskCommand   = TaskCommandModify;
        sTask->mOldAdvWatchId = aOldAdvWatchId;
        sTask->mAdvWatchItem  = aAdvWatchItem;

        aAdvWatchItem->mAdvWatchId = (AdvWatchId)aAdvWatchItem;

        mTaskList.push_back(sTask);

        ::SetEvent(mTaskEvent);
    }

    return aAdvWatchItem->mAdvWatchId;
}

void AdvFileChangeWatcher::unregisterWatch(AdvWatchId aAdvWatchId)
{
    if (aAdvWatchId == InvalidAdvWatchId)
        return;

    if (mMonitorThread.IsRunning() == XPR_FALSE)
        return;

    {
        xpr::MutexGuard sLockGuard(mTaskMutex);

        Task *sTask = new Task;
        sTask->mTaskCommand   = TaskCommandUnregister;
        sTask->mOldAdvWatchId = aAdvWatchId;

        mTaskList.push_back(sTask);

        ::SetEvent(mTaskEvent);
    }
}

void AdvFileChangeWatcher::unregisterAllWatches(void)
{
    if (!mMonitorThread.IsRunning())
        return;

    {
        xpr::MutexGuard sLockGuard(mTaskMutex);

        Task *sTask = new Task;
        sTask->mTaskCommand = TaskCommandUnregisterAll;

        mTaskList.push_back(sTask);

        ::SetEvent(mTaskEvent);
    }
}

xpr_bool_t AdvFileChangeWatcher::getRootPath(const xpr::tstring &aPath, xpr::tstring &aRootPath)
{
    aRootPath.clear();

    if (aPath[1] == XPR_STRING_LITERAL(':'))
    {
        // drive path (local drive or network drive)
        aRootPath = aPath.substr(0, 2);
    }
    else if (_tcsncmp(aPath.c_str(), XPR_STRING_LITERAL("\\\\"), 2) == 0)
    {
        // network shared-folder path
        xpr_size_t sFind = aPath.find(XPR_STRING_LITERAL('\\'), 2);
        if (sFind != xpr::tstring::npos)
            aRootPath = aPath.substr(0, sFind);
    }

    return aRootPath.empty() ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t AdvFileChangeWatcher::OnThreadPreEntry(Thread &theThread)
{
    return XPR_TRUE;
}

unsigned AdvFileChangeWatcher::OnThreadEntryProc(Thread &theThread)
{
    if (theThread == mMonitorThread)
        return OnMonitorThreadEntryProc();

    if (theThread == mNotifyThread)
        return OnNotifyThreadEntryProc();

    return 0;
}

unsigned AdvFileChangeWatcher::OnMonitorThreadEntryProc(void)
{
    DriveWatchMap::iterator sIterator;
    DriveWatchItem *sDriveWatchItem;
    LPOVERLAPPED sOverlapped;
    TaskList sTaskList;
    NotifyList sNotifyList;

    DWORD sResult;

    while (!mMonitorThread.IsStop())
    {
        sResult = ::WaitForSingleObject(mTaskEvent, kIdleTime);
        if (sResult == WAIT_OBJECT_0)
        {
            {
                xpr::MutexGuard sLockGuard(mTaskMutex);

                mTaskList.swap(sTaskList);
                ::ResetEvent(mTaskEvent);
            }

            if (sTaskList.empty() == false)
            {
                processTasks(sTaskList);
                clearTasks(sTaskList);
            }
        }
        else if (sResult == WAIT_TIMEOUT)
        {
            sIterator = mDriveWatchMap.begin();
            for (; sIterator != mDriveWatchMap.end(); ++sIterator)
            {
                if (mMonitorThread.IsStop())
                    break;

                sDriveWatchItem = sIterator->second;
                if (XPR_IS_NULL(sDriveWatchItem))
                    continue;

                if (::WaitForSingleObject(mTaskEvent, 0) == WAIT_OBJECT_0)
                    break;

                sOverlapped = sDriveWatchItem->getQueuedCompletionStatus(10);
                if (XPR_IS_NOT_NULL(sOverlapped))
                {
                    sDriveWatchItem->OnCompletionRoutine(sOverlapped, sNotifyList);

                    if (sNotifyList.empty() == false)
                    {
                        xpr::MutexGuard sLockGuard(mNotifyMutex);

                        mNotifyList.insert(mNotifyList.begin(), sNotifyList.begin(), sNotifyList.end());
                        sNotifyList.clear();

                        ::SetEvent(mNotifyEvent);
                    }
                }
            }
        }
    }

    unregisterAllTasks();

    {
        xpr::MutexGuard sLockGuard(mTaskMutex);
        clearTasks(sTaskList);
    }

    {
        xpr::MutexGuard sLockGuard(mNotifyMutex);
        clearNotifies(mNotifyList);
    }

    return 0;
}

void AdvFileChangeWatcher::processTasks(TaskList &aTaskList)
{
    TaskList::iterator sIterator;

    sIterator = aTaskList.begin();
    for (; sIterator != aTaskList.end(); ++sIterator)
    {
        Task *sTask = *sIterator;

        switch (sTask->mTaskCommand)
        {
        case TaskCommandRegister:      registerTask(*sTask);   break;
        case TaskCommandModify:        modifyTask(*sTask);     break;
        case TaskCommandUnregister:    unregisterTask(*sTask); break;
        case TaskCommandUnregisterAll: unregisterAllTasks();   break;
        }
    }
}

void AdvFileChangeWatcher::registerTask(Task &aTask)
{
    xpr::tstring sRootPath;
    if (!getRootPath(aTask.mAdvWatchItem->mPath, sRootPath))
        return;

    xpr_bool_t sResult = XPR_FALSE;
    DriveWatchItem *sDriveWatchItem = XPR_NULL;

    DriveWatchMap::iterator sIterator = mDriveWatchMap.find(sRootPath);
    if (sIterator == mDriveWatchMap.end())
    {
        sDriveWatchItem = new DriveWatchItem;
        if (XPR_IS_NOT_NULL(sDriveWatchItem))
            mDriveWatchMap[sRootPath] = sDriveWatchItem;
    }
    else
    {
        sDriveWatchItem = sIterator->second;
        if (XPR_IS_NULL(sDriveWatchItem))
            mDriveWatchMap.erase(sIterator);
    }

    if (XPR_IS_NOT_NULL(sDriveWatchItem))
    {
        AdvWatchId sNewAdvWatchId;
        sNewAdvWatchId = sDriveWatchItem->registerWatch(sRootPath, aTask.mAdvWatchItem);
        if (sNewAdvWatchId != InvalidAdvWatchId)
        {
            if (sDriveWatchItem->getRegisteredCount() == 1)
            {
                sDriveWatchItem->readDirectoryChanges();
            }

            sResult = XPR_TRUE;
        }
        else
        {
            XPR_SAFE_DELETE(sDriveWatchItem);
            mDriveWatchMap.erase(sRootPath);
            return;
        }
    }

    if (XPR_IS_FALSE(sResult))
        return;

    if (XPR_IS_NOT_NULL(sDriveWatchItem))
        mIdDriveWatchMap[aTask.mAdvWatchItem->mAdvWatchId] = sDriveWatchItem;

    aTask.mAdvWatchItem = XPR_NULL;
}

void AdvFileChangeWatcher::modifyTask(Task &aTask)
{
    DriveWatchItem *sOldDriveWatchItem = XPR_NULL;
    DriveWatchItem *sNewDriveWatchItem = XPR_NULL;

    IdDriveWatchMap::iterator sOldIdIterator = mIdDriveWatchMap.find(aTask.mOldAdvWatchId);
    if (sOldIdIterator != mIdDriveWatchMap.end())
        sOldDriveWatchItem = sOldIdIterator->second;

    AdvWatchId sNewAdvWatchId = InvalidAdvWatchId;

    xpr_bool_t sResult = aTask.mAdvWatchItem->isValidate();

    // compare
    if (XPR_IS_NOT_NULL(sOldDriveWatchItem))
    {
        AdvWatchItem *sOldAdvWatchItem = sOldDriveWatchItem->getRegisteredItem(aTask.mOldAdvWatchId);
        if (XPR_IS_NOT_NULL(sOldAdvWatchItem) && XPR_IS_NOT_NULL(aTask.mAdvWatchItem))
        {
            if (sOldAdvWatchItem->mPath == aTask.mAdvWatchItem->mPath)
            {
                if (sOldAdvWatchItem->mAdvWatchId != aTask.mAdvWatchItem->mAdvWatchId)
                {
                    // new item change
                    sOldDriveWatchItem->unregisterWatch(aTask.mOldAdvWatchId);
                    mIdDriveWatchMap.erase(sOldIdIterator);

                    xpr::tstring sRootPath;
                    if (getRootPath(aTask.mAdvWatchItem->mPath, sRootPath))
                    {
                        sOldDriveWatchItem->registerWatch(sRootPath, aTask.mAdvWatchItem);

                        mIdDriveWatchMap[aTask.mAdvWatchItem->mAdvWatchId] = sOldDriveWatchItem;

                        aTask.mAdvWatchItem = XPR_NULL;
                    }
                }

                return;
            }
        }
    }

    // old
    if (XPR_IS_NOT_NULL(sOldDriveWatchItem))
    {
        sOldDriveWatchItem->unregisterWatch(aTask.mOldAdvWatchId);
    }

    // new
    {
        xpr::tstring sRootPath;
        if (getRootPath(aTask.mAdvWatchItem->mPath, sRootPath))
        {
            DriveWatchMap::iterator sIterator = mDriveWatchMap.find(sRootPath);
            if (sIterator == mDriveWatchMap.end())
            {
                sNewDriveWatchItem = new DriveWatchItem;
                if (XPR_IS_NOT_NULL(sNewDriveWatchItem))
                    mDriveWatchMap[sRootPath] = sNewDriveWatchItem;
            }
            else
            {
                sNewDriveWatchItem = sIterator->second;
                if (XPR_IS_NULL(sNewDriveWatchItem))
                    mDriveWatchMap.erase(sIterator);
            }

            sNewAdvWatchId = sNewDriveWatchItem->registerWatch(sRootPath, aTask.mAdvWatchItem);
            if (sNewAdvWatchId != InvalidAdvWatchId)
            {
                if (sNewDriveWatchItem->getRegisteredCount() == 1)
                {
                    sNewDriveWatchItem->readDirectoryChanges();
                }
            }
            else
            {
                XPR_SAFE_DELETE(sNewDriveWatchItem);
                mDriveWatchMap.erase(sRootPath);
            }
        }
    }

    if (XPR_IS_NOT_NULL(sOldDriveWatchItem))
    {
        xpr_size_t nOldRegisteredCount = sOldDriveWatchItem->getRegisteredCount();
        if (nOldRegisteredCount == 0)
        {
            mDriveWatchMap.erase(sOldDriveWatchItem->mRootPath);
            XPR_SAFE_DELETE(sOldDriveWatchItem);
        }
    }

    if (sOldIdIterator != mIdDriveWatchMap.end())
        mIdDriveWatchMap.erase(sOldIdIterator);

    if (XPR_IS_NOT_NULL(sNewDriveWatchItem) && sNewAdvWatchId != InvalidAdvWatchId)
        mIdDriveWatchMap[aTask.mAdvWatchItem->mAdvWatchId] = sNewDriveWatchItem;

    if (sNewAdvWatchId != InvalidAdvWatchId)
        aTask.mAdvWatchItem = XPR_NULL;
}

void AdvFileChangeWatcher::unregisterTask(Task &aTask)
{
    IdDriveWatchMap::iterator sIterator = mIdDriveWatchMap.find(aTask.mOldAdvWatchId);
    if (sIterator == mIdDriveWatchMap.end())
        return;

    DriveWatchItem *sDriveWatchItem = sIterator->second;
    if (XPR_IS_NULL(sDriveWatchItem))
    {
        mIdDriveWatchMap.erase(sIterator);
        return;
    }

    sDriveWatchItem->unregisterWatch(aTask.mOldAdvWatchId);

    xpr_size_t sRegisteredCount = sDriveWatchItem->getRegisteredCount();
    if (sRegisteredCount == 0)
    {
        mDriveWatchMap.erase(sDriveWatchItem->mRootPath);
        XPR_SAFE_DELETE(sDriveWatchItem);
    }

    mIdDriveWatchMap.erase(sIterator);
}

void AdvFileChangeWatcher::unregisterAllTasks(void)
{
    DriveWatchMap::iterator sIterator;
    DriveWatchItem *sDriveWatchItem;

    sIterator = mDriveWatchMap.begin();
    for (; sIterator != mDriveWatchMap.end(); ++sIterator)
    {
        sDriveWatchItem = sIterator->second;
        XPR_SAFE_DELETE(sDriveWatchItem);
    }

    mDriveWatchMap.clear();
    mIdDriveWatchMap.clear();
}

void AdvFileChangeWatcher::clearTasks(TaskList &aTaskList)
{
    Task *sTask;
    TaskList::iterator sIterator;

    sIterator = aTaskList.begin();
    for (; sIterator != aTaskList.end(); ++sIterator)
    {
        sTask = *sIterator;
        XPR_SAFE_DELETE(sTask);
    }

    aTaskList.clear();
}

unsigned AdvFileChangeWatcher::OnNotifyThreadEntryProc(void)
{
    DWORD sResult;
    NotifyList sNotifyList;

    NotifyInfo *sNotifyInfo;
    NotifyList::iterator sIterator;
    NotifyMap::iterator sNotifyIterator;
    xpr_sint64_t sTime, sDiff;
    xpr_size_t sNotifyCount;

    while (mNotifyThread.IsStop() == XPR_FALSE)
    {
        sResult = ::WaitForSingleObject(mNotifyEvent, kIdleTime);
        if (sResult == WAIT_OBJECT_0)
        {
            {
                xpr::MutexGuard sLockGuard(mNotifyMutex);

                sNotifyList.swap(mNotifyList);
                ::ResetEvent(mNotifyEvent);
            }

            if (sNotifyList.empty() == false)
            {
                sIterator = sNotifyList.begin();
                for (; sIterator != sNotifyList.end(); ++sIterator)
                {
                    sNotifyInfo = *sIterator;

                    sNotifyIterator = mNotifyMap.find(sNotifyInfo->mAdvWatchId);
                    if (sNotifyIterator == mNotifyMap.end())
                    {
                        NotifyList sNotifyList;
                        sNotifyList.push_back(sNotifyInfo);

                        mNotifyMap[sNotifyInfo->mAdvWatchId] = sNotifyList;
                    }
                    else
                    {
                        sNotifyIterator->second.push_back(sNotifyInfo);
                    }
                }

                sNotifyList.clear();
            }
        }
        else if (sResult == WAIT_TIMEOUT)
        {
            if (mNotifyMap.empty())
                continue;

            sTime = xpr::timer_ms();

            sNotifyIterator = mNotifyMap.begin();
            while (sNotifyIterator != mNotifyMap.end())
            {
                AdvWatchId sAdvWatchId = sNotifyIterator->first;
                NotifyList &sNotifyList = sNotifyIterator->second;

                if (sNotifyList.empty() == true)
                {
                    sNotifyIterator = mNotifyMap.erase(sNotifyIterator);
                    continue;
                }

                sNotifyInfo = *sNotifyList.rbegin();

                sDiff = sTime - sNotifyInfo->mTime;
                if (sDiff >= 1000)
                {
                    sNotifyCount = sNotifyList.size();
                    if (sNotifyCount > 3)
                    {
                        sNotifyInfo->mEvent       = EventUpdateDir;
                        sNotifyInfo->mNotifyCount = sNotifyCount;
                        sNotifyInfo->mFileName.clear();

                        if (::PostMessage(sNotifyInfo->mHwnd, sNotifyInfo->mMsg, (WPARAM)sNotifyInfo, XPR_NULL) == XPR_TRUE)
                        {
                            sNotifyList.remove(sNotifyInfo);
                        }
                    }
                    else
                    {
                        sIterator = sNotifyList.begin();
                        while (sIterator != sNotifyList.end())
                        {
                            sNotifyInfo = *sIterator;

                            if (::PostMessage(sNotifyInfo->mHwnd, sNotifyInfo->mMsg, (WPARAM)sNotifyInfo, XPR_NULL) == XPR_TRUE)
                            {
                                sIterator = sNotifyList.erase(sIterator);
                                continue;
                            }

                            sIterator++;
                        }
                    }

                    clearNotifies(sNotifyList);
                }

                if (sNotifyList.empty())
                {
                    sNotifyIterator = mNotifyMap.erase(sNotifyIterator);
                    continue;
                }

                sNotifyIterator++;
            }
        }
    }

    clearNotifies(mNotifyMap);

    return 0;
}

void AdvFileChangeWatcher::clearNotifies(NotifyList &aNotifyList)
{
    clear(aNotifyList);
}

void AdvFileChangeWatcher::clearNotifies(NotifyMap &aNotifyMap)
{
    NotifyMap::iterator sIterator;

    sIterator = aNotifyMap.begin();
    for (; sIterator != aNotifyMap.end(); ++sIterator)
    {
        NotifyList &sNotifyList = sIterator->second;
        clearNotifies(sNotifyList);
    }

    aNotifyMap.clear();
}
} // namespace fxfile
