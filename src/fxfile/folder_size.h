//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FOLDER_SIZE_H__
#define __FXFILE_FOLDER_SIZE_H__ 1
#pragma once

#include "xpr_thread_sync.h"

namespace fxfile
{
class FolderSize
{
public:
    FolderSize(void);
    virtual ~FolderSize(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    // asynchronize
    void addFolderSize(const xpr_tchar_t *aDir);
    void stop(DWORD atopMillseconds = INFINITE);

    // synchronize
    static xpr_uint64_t getFolderSize(const xpr_tchar_t *aDir);

protected:
    static unsigned __stdcall FolderSizeProc(void *aParam);
    DWORD OnFolderSize(void);

protected:
    HWND       mHwnd;
    xpr_uint_t mMsg;

    typedef std::deque<xpr::tstring> DirDeque;
    DirDeque   mDirDeque;
    xpr::Mutex mMutex;

    HANDLE   mThread;
    HANDLE   mEvent;
    HANDLE   mStop;
    unsigned mThreadId;
};

xpr_uint64_t getFolderSize(const xpr_tchar_t *aFolder);
} // namespace fxfile

#endif // __FXFILE_FOLDER_SIZE_H__
