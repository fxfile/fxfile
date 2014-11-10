//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_THREAD_H__
#define __FXFILE_THREAD_H__ 1
#pragma once

namespace fxfile
{
class Thread : public xpr::Thread
{
public:
    Thread(void);
    virtual ~Thread(void);

public:
    virtual xpr_rcode_t start(xpr::Thread::Runnable *aRunnable, xpr_size_t aStackSize = 0);

    virtual void stop(void);
    xpr_bool_t isStop(void) const;

protected:
    volatile xpr_bool_t mStop;
};

inline void Thread::stop(void)
{
    mStop = XPR_TRUE;
}

inline xpr_bool_t Thread::isStop(void) const
{
    return mStop;
}
} // namespace fxfile

#endif // __FXFILE_THREAD_H__
