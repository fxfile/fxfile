//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SINGLETON_MANAGER_H__
#define __FXFILE_SINGLETON_MANAGER_H__ 1
#pragma once

#include "xpr_mutex.h"

namespace fxfile
{
class SingletonManager;

class SingletonInstance
{
    friend class SingletonManager;

protected:
    SingletonInstance(void) {}
    virtual ~SingletonInstance(void) {}
};

class SingletonManager
{
public:
    template <typename T>
    static T &get(void)
    {
        // TODO: change to hash code in C++11
        const xpr_char_t *sTypeName = typeid(T).name();

        xpr::MutexGuard sLockGuard(mMutex);

        InstanceMap::iterator sIterator = mInstanceMap.find(sTypeName);
        if (sIterator != mInstanceMap.end())
        {
            return *((T *)sIterator->second);
        }

        T *sInstance = new T;

        mInstanceMap[sTypeName] = sInstance;

        return *sInstance;
    }

    template <typename T>
    static xpr_bool_t &exist(void)
    {
        const xpr_char_t *sTypeName = typeid(T).name();

        xpr::MutexGuard sLockGuard(mMutex);

        if (mInstanceMap.find(sTypeName) == mInstanceMap.end())
        {
            return XPR_FALSE;
        }

        return XPR_TRUE;
    }

    static void clean(void)
    {
        SingletonInstance     *sSingletonInstance;
        InstanceMap::iterator  sIterator;

        xpr::MutexGuard sLockGuard(mMutex);

        FXFILE_STL_FOR_EACH(sIterator, mInstanceMap)
        {
            sSingletonInstance = sIterator->second;
            XPR_SAFE_DELETE(sSingletonInstance);
        }

        mInstanceMap.clear();
    }

protected:
    typedef std::tr1::unordered_map<xpr::string, SingletonInstance *> InstanceMap;
    static InstanceMap mInstanceMap;
    static xpr::Mutex  mMutex;
};
} // namespace fxfile

#endif // __FXFILE_SINGLETON_MANAGER_H__
