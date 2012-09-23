//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_thread_sync.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
Cs::Cs(void)
{
    ::InitializeCriticalSection(&mCs);
}

Cs::~Cs(void)
{
    ::DeleteCriticalSection(&mCs);
}

void Cs::lock(void)
{
    ::EnterCriticalSection(&mCs);
}

void Cs::unlock(void)
{
    ::LeaveCriticalSection(&mCs);
}

Mutex::Mutex(void)
{
    mMutex = ::CreateMutex(XPR_NULL, XPR_FALSE, XPR_NULL);
}

Mutex::~Mutex(void)
{
    if (mMutex != XPR_NULL)
        ::CloseHandle(mMutex);
}

void Mutex::lock(void)
{
    if (mMutex != XPR_NULL)
        ::WaitForSingleObject(mMutex, INFINITE);
}

void Mutex::unlock(void)
{
    if (mMutex != XPR_NULL)
        ::ReleaseMutex(mMutex);
}

CsLocker::CsLocker(Cs &aCs)
    : mCs(aCs)
{
    mCs.lock();
}

CsLocker::~CsLocker(void)
{
    mCs.unlock();
}

MutexLocker::MutexLocker(Mutex &aMutex)
    : mMutex(aMutex)
{
    mMutex.lock();
}

MutexLocker::~MutexLocker(void)
{
    mMutex.unlock();
}
} // namespace fxb
