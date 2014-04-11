//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_size.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
void getRecursiveFolderSize(const xpr_tchar_t *aFolder, WIN32_FIND_DATA &aWin32FindData, xpr_uint64_t &aFolderSize)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sPath, XPR_STRING_LITERAL("%s\\*.*"), aFolder);

    HANDLE sFind = ::FindFirstFile(sPath, &aWin32FindData);
    if (sFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (_tcscmp(aWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(aWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
                continue;

            if (!XPR_TEST_BITS(aWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                aFolderSize += aWin32FindData.nFileSizeLow;
                aFolderSize += (xpr_uint64_t)aWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;
            }

            if (XPR_TEST_BITS(aWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s"), aFolder, aWin32FindData.cFileName);
                getRecursiveFolderSize(sPath, aWin32FindData, aFolderSize);
            }
        }
        while (FindNextFile(sFind, &aWin32FindData) == XPR_TRUE);

        ::FindClose(sFind);
    }
}
}

FolderSize::FolderSize(void)
    : mHwnd(XPR_NULL)
    , mMsg(XPR_NULL)
    , mThread(XPR_NULL)
    , mEvent(XPR_NULL)
    , mStop(XPR_NULL)
    , mThreadId(0)
{
}

FolderSize::~FolderSize(void)
{
    stop();

    CLOSE_HANDLE(mThread);
    CLOSE_HANDLE(mEvent);
    CLOSE_HANDLE(mStop);

    mDirDeque.clear();
}

void FolderSize::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void FolderSize::addFolderSize(const xpr_tchar_t *aDir)
{
    if (XPR_IS_NULL(aDir))
        return;

    if (IsExistFile(aDir) == XPR_FALSE)
        return;

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mDirDeque.push_back(aDir);

        ::SetEvent(mEvent);
    }
}

void FolderSize::stop(DWORD aStopMillseconds)
{
    ::SetEvent(mStop);

    if (::WaitForSingleObject(mThread, aStopMillseconds) == WAIT_TIMEOUT)
        ::TerminateThread(mThread, 0);

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mDirDeque.clear();
    }
}

unsigned __stdcall FolderSize::FolderSizeProc(void *aParam)
{
    DWORD sExitCode = 0;

    FolderSize *sFolderSize = (FolderSize *)aParam;
    if (XPR_IS_NOT_NULL(sFolderSize))
        sExitCode = sFolderSize->OnFolderSize();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD FolderSize::OnFolderSize(void)
{
    HANDLE sEvents[2];
    sEvents[0] = mEvent;
    sEvents[1] = mStop;

    DWORD sWait;
    xpr::string sDir;
    WIN32_FIND_DATA sWin32FindData;
    xpr_uint64_t sFolderSize;

    xpr_tchar_t *sParamFolder;
    xpr_uint64_t *sParamFolderSize;
    xpr_bool_t sPosted;

    while (true)
    {
        sWait = ::WaitForMultipleObjects(2, sEvents, XPR_FALSE, 100);

        if (sWait == WAIT_TIMEOUT)
            continue;

        if (sWait == WAIT_OBJECT_0+1)
            break;

        if (sWait == WAIT_OBJECT_0)
        {
            {
                xpr::MutexGuard sLockGuard(mMutex);
                if (mDirDeque.empty())
                    continue;

                sDir = mDirDeque.front();
                mDirDeque.pop_front();
            }

            sFolderSize = 0;
            getRecursiveFolderSize(sDir.c_str(), sWin32FindData, sFolderSize);

            sParamFolder = new xpr_tchar_t[sDir.length()+1];
            sParamFolderSize = new xpr_uint64_t;

            sPosted = XPR_FALSE;
            if (XPR_IS_NOT_NULL(sParamFolder) && XPR_IS_NOT_NULL(sParamFolderSize))
            {
                _tcscpy(sParamFolder, sDir.c_str());
                *sParamFolderSize = sFolderSize;

                sPosted = ::PostMessage(mHwnd, mMsg, (WPARAM)sParamFolder, (LPARAM)sParamFolderSize);
            }

            if (XPR_IS_FALSE(sPosted))
            {
                XPR_SAFE_DELETE_ARRAY(sParamFolder);
                XPR_SAFE_DELETE(sParamFolderSize);
            }
        }
    }

    return 0;
}

xpr_uint64_t FolderSize::getFolderSize(const xpr_tchar_t *aDir)
{
    if (XPR_IS_NULL(aDir))
        return 0;

    if (IsExistFile(aDir) == XPR_FALSE)
        return 0;

    WIN32_FIND_DATA sWin32FindData = {0};
    xpr_uint64_t sFolderSize = 0;

    getRecursiveFolderSize(aDir, sWin32FindData, sFolderSize);

    return sFolderSize;
}
} // namespace fxfile
