//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "single_process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace upchecker
{
namespace
{
const xpr_tchar_t kSingleProcessMutexName[] = XPR_STRING_LITERAL("fxfile-upchecker - single process");
const xpr_uint_t  WM_SINGLE_PROCESS         = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-upchecker - single process"));
} // anonymous namespace

HANDLE SingleProcess::mSingleProcessMutex = XPR_NULL;

xpr_bool_t SingleProcess::check(void)
{
    HANDLE sSingleProcessMutex;

    sSingleProcessMutex = ::OpenMutex(MUTEX_ALL_ACCESS, XPR_FALSE, kSingleProcessMutexName);
    if (XPR_IS_NOT_NULL(sSingleProcessMutex))
    {
        return XPR_FALSE;
    }

    CLOSE_HANDLE(sSingleProcessMutex);

    return XPR_TRUE;
}

void SingleProcess::lock(void)
{
    if (XPR_IS_NULL(mSingleProcessMutex))
    {
        mSingleProcessMutex = ::CreateMutex(XPR_NULL, XPR_FALSE, kSingleProcessMutexName);
    }
}

void SingleProcess::unlock(void)
{
    CLOSE_HANDLE(mSingleProcessMutex);
}

xpr_uint_t SingleProcess::getMsg(void)
{
    return WM_SINGLE_PROCESS;
}

void SingleProcess::postMsg(void)
{
    ::PostMessage(HWND_BROADCAST, WM_SINGLE_PROCESS, 0, 0);
}
} // namespace upchecker
} // namespace fxfile
