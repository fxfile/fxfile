//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_change_watcher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
static const DWORD kIdleTime = 20;

FileChangeWatcher::FileChangeWatcher(void)
    : mEvent(XPR_NULL)
{
}

FileChangeWatcher::~FileChangeWatcher(void)
{
    destroy();

    CLOSE_HANDLE(mEvent);
}

xpr_bool_t FileChangeWatcher::create(void)
{
    CLOSE_HANDLE(mEvent);

    mEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    if (XPR_IS_NULL(mEvent))
        return XPR_FALSE;

    return Start();
}

void FileChangeWatcher::destroy(void)
{
    unregisterAllWatches();

    Stop();

    CLOSE_HANDLE(mEvent);
}

FileChangeWatcher::WatchId FileChangeWatcher::registerWatch(WatchItem *aWatchItem)
{
    if (XPR_IS_NULL(aWatchItem))
        return InvalidWatchId;

    if (XPR_IS_NULL(aWatchItem->mHwnd) || aWatchItem->mMsg == 0 || aWatchItem->mPath.empty() == true)
        return InvalidWatchId;

    if (IsExistFile(aWatchItem->mPath) == XPR_FALSE)
        return InvalidWatchId;

    DWORD sAttributes;
    sAttributes = ::GetFileAttributes(aWatchItem->mPath.c_str());

    if (!XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return InvalidWatchId;

    WatchItem *sNewWatchItem = new WatchItem;
    if (XPR_IS_NULL(sNewWatchItem))
        return InvalidWatchId;

    *sNewWatchItem = *aWatchItem;

    HANDLE sNotifyHandle;
    const DWORD sNotifyFilter = 
        FILE_NOTIFY_CHANGE_FILE_NAME   |
        FILE_NOTIFY_CHANGE_DIR_NAME    |
        FILE_NOTIFY_CHANGE_ATTRIBUTES  |
        FILE_NOTIFY_CHANGE_SIZE        |
        FILE_NOTIFY_CHANGE_LAST_WRITE  |
        FILE_NOTIFY_CHANGE_SECURITY;

    sNotifyHandle = ::FindFirstChangeNotification(
        sNewWatchItem->mPath.c_str(),
        sNewWatchItem->mSubPath,
        sNotifyFilter);

    if (sNotifyHandle == INVALID_HANDLE_VALUE)
    {
        XPR_SAFE_DELETE(sNewWatchItem);
        return InvalidWatchId;
    }

    if (IsRunning() == XPR_FALSE)
        create();

    WatchId sWatchId = (WatchId)sNotifyHandle;

    {
        CsLocker sLocker(mCs);

        mWatchMap[sNotifyHandle] = sNewWatchItem;

        SetEvent(mEvent);
    }

    return sWatchId;
}

FileChangeWatcher::WatchId FileChangeWatcher::modifyWatch(WatchId aWatchId, WatchItem *aWatchItem)
{
    unregisterWatch(aWatchId);
    return registerWatch(aWatchItem);
}

void FileChangeWatcher::unregisterWatch(WatchId aWatchId)
{
    HANDLE sNotifyHandle = (HANDLE)aWatchId;
    WatchItem *sWatchItem;

    {
        CsLocker sLocker(mCs);

        WatchMap::iterator sIterator = mWatchMap.find(sNotifyHandle);
        if (sIterator == mWatchMap.end())
            return;

        sWatchItem = sIterator->second;
        XPR_SAFE_DELETE(sWatchItem);

        mWatchMap.erase(sIterator);

        SetEvent(mEvent);
    }

    ::FindCloseChangeNotification(sNotifyHandle);
}

void FileChangeWatcher::unregisterAllWatches(void)
{
    CsLocker sLocker(mCs);

    HANDLE sNotifyHandle;
    WatchItem *sWatchItem;

    WatchMap::iterator sIterator = mWatchMap.begin();
    for (; sIterator != mWatchMap.end(); ++sIterator)
    {
        sNotifyHandle = sIterator->first;
        sWatchItem = sIterator->second;

        ::FindCloseChangeNotification(sNotifyHandle);
        XPR_SAFE_DELETE(sWatchItem);
    }

    mWatchMap.clear();

    SetEvent(mEvent);
}

xpr_bool_t FileChangeWatcher::OnPreEntry(void)
{
    return XPR_TRUE;
}

unsigned FileChangeWatcher::OnEntryProc(void)
{
    typedef std::vector<HANDLE> HandleVector;
    HandleVector sNotifyHandles;

    typedef std::set<HANDLE> SignalSet;
    SignalSet sSignalSet;

    HANDLE sChangedNotify;
    HANDLE sNotifyHandle;
    HANDLE *sHandles;
    xpr_size_t sCount;
    xpr_sint_t sIndex;
    xpr_bool_t sTimeout;
    xpr_bool_t sResetTimer;
    xpr_bool_t sResetHandle;
    xpr_sint_t sNotifyResult;

    DWORD sIdle;
    DWORD sResult;

    while (IsStop() == XPR_FALSE)
    {
        sIdle = kIdleTime;

        sResult = ::WaitForSingleObject(mEvent, sIdle);
        if (sResult == WAIT_OBJECT_0)
        {
            CsLocker sLocker(mCs);

            sNotifyHandles.clear();
            sSignalSet.clear();

            sNotifyHandles.push_back(mEvent);

            WatchMap::iterator sIterator = mWatchMap.begin();
            for (; sIterator != mWatchMap.end(); ++sIterator)
            {
                sNotifyHandles.push_back(sIterator->first);

                if (sIterator->second->mTime != 0i64)
                    sSignalSet.insert(sIterator->first);
            }

            ::ResetEvent(mEvent);
        }

        if (sNotifyHandles.empty())
            continue;

        sIdle = 0;

        sCount = sNotifyHandles.size();
        sHandles = &sNotifyHandles[0];

        while (IsStop() == XPR_FALSE)
        {
            sTimeout = XPR_FALSE;

            sResult = ::WaitForMultipleObjects((DWORD)sCount, sHandles, XPR_FALSE, kIdleTime);
            if (!XPR_IS_RANGE(WAIT_OBJECT_0, sResult, WAIT_OBJECT_0+sCount))
            {
                if (sResult == WAIT_FAILED)
                    break;

                if (sSignalSet.empty())
                    continue;

                sTimeout = XPR_TRUE;
            }

            if (sResult == WAIT_OBJECT_0)
                break;

            sIndex = -1;
            sChangedNotify = XPR_NULL;

            if (XPR_IS_FALSE(sTimeout))
            {
                sIndex = sResult - WAIT_OBJECT_0;
                sChangedNotify = sHandles[sIndex];

                ::FindNextChangeNotification(sChangedNotify);

                if (sSignalSet.find(sChangedNotify) == sSignalSet.end())
                    sSignalSet.insert(sChangedNotify);
            }

            //
            // Update timer or notify message for all events, if all the events are timeout.
            //

            sResetHandle = XPR_FALSE;

            SignalSet::iterator sIterator = sSignalSet.begin();
            while (sIterator != sSignalSet.end())
            {
                sNotifyHandle = *sIterator;
                sResetTimer = (sNotifyHandle == sChangedNotify);

                sNotifyResult = delayNotify(sNotifyHandle, sResetTimer);

                if (sNotifyResult == -1)
                    sResetHandle = XPR_TRUE;

                if (sNotifyResult == 1)
                {
                    sIterator = sSignalSet.erase(sIterator);
                    continue;
                }

                sIterator++;
            }

            if (XPR_IS_TRUE(sResetHandle))
            {
                ::SetEvent(mEvent);
                break;
            }
        }
    }

    sNotifyHandles.clear();
    sSignalSet.clear();

    return 0;
}

xpr_bool_t FileChangeWatcher::delayNotify(HANDLE aNotifyHandle, xpr_bool_t aResetTimer)
{
    if (XPR_IS_NULL(aNotifyHandle))
        return 0;

    CsLocker sLocker(mCs);

    WatchMap::iterator sIterator = mWatchMap.find(aNotifyHandle);
    if (sIterator == mWatchMap.end())
        return -1;

    WatchItem *sWatchItem = sIterator->second;
    if (sWatchItem->mTime == 0i64 || aResetTimer)
    {
        // reset timer
        sWatchItem->mTime = xpr::timer_ms();
    }
    else
    {
        xpr_sint64_t sDiff = xpr::timer_ms() - sWatchItem->mTime;
        if (sDiff >= 1000i64)
        {
            sWatchItem->mTime = 0i64;

            ::PostMessage(
                sWatchItem->mHwnd,
                sWatchItem->mMsg,
                (WPARAM)aNotifyHandle,
                sWatchItem->mParam);

            return 1;
        }
    }

    return 0;
}
} // namespace fxb
