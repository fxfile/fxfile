//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_PATTERN_H__
#define __FXFILE_BASE_PATTERN_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_thread_sync.h"

namespace fxfile
{
namespace base
{
class Command
{
public:
    virtual ~Command(void) {}
    virtual void execute(void) = 0;

protected:
    Command(void) {}
};

template <typename T>
class Singleton
{
public:
    static T& instance(void)
    {
        xpr::MutexGuard sLockGuard(mMutex);

        if (mSingleInstance.get() == XPR_NULL)
        {
            mSingleInstance.reset(new T);
        }

        return *mSingleInstance;
    }

    static xpr_bool_t isInstance(void)
    {
        xpr::MutexGuard sLockGuard(mMutex);

        return (mSingleInstance.get() == XPR_NULL) ? XPR_FALSE : XPR_TRUE;
    }

    virtual ~Singleton(void) {}

protected:
    Singleton(void) {}

private:
    static std::auto_ptr<T> mSingleInstance;
    static xpr::Mutex mMutex;
};

template <typename T> std::auto_ptr<T> Singleton<T>::mSingleInstance;
template <typename T> xpr::Mutex Singleton<T>::mMutex;
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_PATTERN_H__
