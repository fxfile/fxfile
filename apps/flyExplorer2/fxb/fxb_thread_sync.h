//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_THREAD_SYNC_H__
#define __FXB_THREAD_SYNC_H__
#pragma once

namespace fxb
{
class Cs
{
public:
    Cs(void);
    ~Cs(void);

public:
    void lock(void);
    void unlock(void);

private:
    CRITICAL_SECTION mCs;
};

class Mutex
{
public:
    Mutex(void);
    ~Mutex(void);

public:
    void lock(void);
    void unlock(void);

private:
    HANDLE mMutex;
};

class CsLocker
{
public:
    CsLocker(Cs &aCs);
    ~CsLocker(void);

private:
    Cs &mCs;
};

class MutexLocker
{
public:
    MutexLocker(Mutex &aMutex);
    ~MutexLocker(void);

private:
    Mutex &mMutex;
};
} // namespace fxb

#endif // __FXB_THREAD_SYNC_H__
