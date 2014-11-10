//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "thread.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
Thread::Thread(void)
    : mStop(XPR_FALSE)
{
}

Thread::~Thread(void)
{
    stop();
    join();
}

xpr_rcode_t Thread::start(xpr::Thread::Runnable *aRunnable, xpr_size_t aStackSize)
{
    if (isRunning() == XPR_FALSE)
    {
        mStop = XPR_FALSE;
    }

    return xpr::Thread::start(aRunnable);
}
} // namespace fxfile
