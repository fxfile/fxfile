//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_PATTERN_H__
#define __XPR_PATTERN_H__
#pragma once

#include "xpr_types.h"
#include "xpr_thread_sync.h"

namespace xpr
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
        MutexGuard sLockGuard(mMutex);

        if (mSingleInstance.get() == 0)
            mSingleInstance.reset(new T);

        return *mSingleInstance;
    }

    static xpr_bool_t isInstance(void)
    {
        MutexGuard sLockGuard(mMutex);

        return (mSingleInstance.get() == 0) ? XPR_FALSE : XPR_TRUE;
    }

    virtual ~Singleton(void) {}

protected:
    Singleton(void) {}

private:
    static std::auto_ptr<T> mSingleInstance;
    static Mutex mMutex;
};

template <typename T> std::auto_ptr<T> Singleton<T>::mSingleInstance;
template <typename T> Mutex Singleton<T>::mMutex;
} // namespace xpr

#endif // __XPR_PATTERN_H__
