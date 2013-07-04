//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_USER_ENV_H__
#define __FXFILE_USER_ENV_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class UserEnv : public fxfile::base::Singleton<UserEnv>
{
    friend class fxfile::base::Singleton<UserEnv>;

protected: UserEnv(void);
public:   ~UserEnv(void);

public:
    enum WinVer
    {
        WinUnknown = 0,
        WinNT351,
        Win95orNT40,
        Win98,
        WinMe,
        Win2000,
        WinXP,
        WinXPx64,
        Win2003,
        Win2003R2,
        WinVista,
        Win2008,
        Win7,
        Win2008R2,
        Win8,
        Win2012,
        WinMore,
    };

    enum InetExpVer
    {
        InetExp50Under = 0,
        InetExp50More
    };

public:
    static DWORD getDllVersion(const xpr_tchar_t *aDllName);
    static void getDllVersion(const xpr_tchar_t *aDllName, LPDWORD aMajorVersion, LPDWORD aMinorVersion, LPDWORD aBuildNumber);

public:
    OSVERSIONINFOEX mOSVerInfo;
    InetExpVer      mInetExpVer;
    xpr::tstring    mShellVer;

protected:
    void getUserEnvironment(void);
};
} // namespace fxfile

#endif // __FXFILE_USER_ENV_H__
