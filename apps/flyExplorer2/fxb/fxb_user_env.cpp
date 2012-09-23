//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_user_env.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
#define PACK_VERSION(major,minor) MAKELONG(minor,major)

UserEnv::UserEnv(void)
{
    getUserEnvironment();
}

UserEnv::~UserEnv(void)
{
}

void UserEnv::getUserEnvironment(void)
{
    //
    // step 1. get windows version
    //

    // reference:
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms724833(v=vs.85).aspx

    mOSVerInfo.dwOSVersionInfoSize = sizeof(mOSVerInfo);
    ::GetVersionEx((LPOSVERSIONINFO)&mOSVerInfo);

    mWinVer = WinUnknown;
    xpr_bool_t sWorkstation = (mOSVerInfo.wProductType == VER_NT_WORKSTATION) ? XPR_TRUE : XPR_FALSE;

    switch (mOSVerInfo.dwMajorVersion)
    {
    case 3:
        {
            switch (mOSVerInfo.dwMinorVersion)
            {
            case 51: mWinVer = WinNT351; break;
            }
            break;
        }

    case 4:
        {
            switch (mOSVerInfo.dwMinorVersion)
            {
            case  0: mWinVer = Win95orNT40; break;
            case 10: mWinVer = Win98;       break;
            case 90: mWinVer = WinMe;       break;
            }
            break;
        }

    case 5:
        {
            if (mOSVerInfo.dwMinorVersion == 2)
            {
                SYSTEM_INFO si = {0};
                ::GetSystemInfo(&si);

                if (sWorkstation == XPR_TRUE && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                {
                    mWinVer = WinXPx64;
                }
                else
                {
                    if (GetSystemMetrics(SM_SERVERR2) != 0)
                    {
                        mWinVer = Win2003R2;
                    }
                    else
                    {
                        mWinVer = Win2003;
                    }
                }
            }

            else
            {
                switch (mOSVerInfo.dwMinorVersion)
                {
                case 0: mWinVer = Win2000; break;
                case 1: mWinVer = WinXP;   break;
                }
            }

            break;
        }

    case 6:
        {
            if (sWorkstation == XPR_TRUE)
            {
                switch (mOSVerInfo.dwMinorVersion)
                {
                case 0:  mWinVer = WinVista; break;
                case 1:  mWinVer = Win7;     break;
                case 2:  mWinVer = Win8;     break;
                default: mWinVer = WinMore;  break;
                }
            }
            else
            {
                switch (mOSVerInfo.dwMinorVersion)
                {
                case 0:  mWinVer = Win2008;   break;
                case 1:  mWinVer = Win2008R2; break;
                case 2:  mWinVer = Win2012;   break;
                default: mWinVer = WinMore;   break;
                }
            }

            break;
        }

    default:
        {
            if (mOSVerInfo.dwMajorVersion > 6)
            {
                mWinVer = WinMore;
            }

            break;
        }
    }

    mPlatformNT = (mOSVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) ? XPR_TRUE : XPR_FALSE;

    //
    // step 2. get internet explorer version
    //
    DWORD sComctl32DllVer = getDllVersion(XPR_STRING_LITERAL("Comctl32.dll"));
    DWORD sShlwapiDllVer  = getDllVersion(XPR_STRING_LITERAL("Shlwapi.dll"));
    DWORD sShell32DllVer  = getDllVersion(XPR_STRING_LITERAL("Shell32.dll"));

    if (sShell32DllVer > PACK_VERSION(6,1))
    {
        mShellVer = XPR_STRING_LITERAL("6.X");
        mInetExpVer = InetExp50More;
    }
    else if (sShell32DllVer == PACK_VERSION(6,1))
    {
        mShellVer = XPR_STRING_LITERAL("6.1");
        mInetExpVer = InetExp50More;
    }
    else if (sComctl32DllVer >= PACK_VERSION(6,0))
    {
        mShellVer = XPR_STRING_LITERAL("6.0");
        mInetExpVer = InetExp50More;
    }
    else if (sComctl32DllVer >= PACK_VERSION(5,81))
    {
        mShellVer = XPR_STRING_LITERAL("5.81");
        mInetExpVer = InetExp50More;
    }
    else if (sComctl32DllVer >= PACK_VERSION(5,8))
    {
        mShellVer = XPR_STRING_LITERAL("5.8");
        mInetExpVer = InetExp50More;
    }
    else if (sShlwapiDllVer >= PACK_VERSION(5,0))
    {
        mShellVer = XPR_STRING_LITERAL("5.0");
        mInetExpVer = InetExp50More;
    }
    else if (sShell32DllVer >= PACK_VERSION(4,72))
    {
        mShellVer = XPR_STRING_LITERAL("4.72");
        mInetExpVer = InetExp50Under;
    }
    else if (sShell32DllVer >= PACK_VERSION(4,71))
    {
        mShellVer = XPR_STRING_LITERAL("4.71");
        mInetExpVer = InetExp50Under;
    }
    else if (sShell32DllVer >= PACK_VERSION(4,7))
    {
        mShellVer = XPR_STRING_LITERAL("4.7");
        mInetExpVer = InetExp50Under;
    }
    else if (sShell32DllVer >= PACK_VERSION(4,0))
    {
        mShellVer = XPR_STRING_LITERAL("4.0");
        mInetExpVer = InetExp50Under;
    }
    else
    {
        mShellVer = XPR_STRING_LITERAL("Unknown");
        mInetExpVer = InetExp50Under;
    }
}

DWORD UserEnv::getDllVersion(const xpr_tchar_t *aDllName)
{
    HINSTANCE sInstance;
    DWORD sVersion = 0;

    sInstance = ::LoadLibrary(aDllName);
    if (XPR_IS_NOT_NULL(sInstance))
    {
        DLLGETVERSIONPROC sDllGetVersionFunc = (DLLGETVERSIONPROC)::GetProcAddress(sInstance, "DllGetVersion");
        if (XPR_IS_NOT_NULL(sDllGetVersionFunc))
        {
            DLLVERSIONINFO sDllVersionInfo = {0};
            sDllVersionInfo.cbSize = sizeof(sDllVersionInfo);

            HRESULT sHResult = (*sDllGetVersionFunc)(&sDllVersionInfo);

            if (SUCCEEDED(sHResult))
                sVersion = PACK_VERSION(sDllVersionInfo.dwMajorVersion, sDllVersionInfo.dwMinorVersion);
        }

        ::FreeLibrary(sInstance);
    }

    return sVersion;
}

void UserEnv::getDllVersion(const xpr_tchar_t *aDllName, LPDWORD aMajorVersion, LPDWORD aMinorVersion, LPDWORD aBuildNumber)
{
    HINSTANCE sInstance;
    DWORD sVersion = 0;

    sInstance = ::LoadLibrary(aDllName);
    if (XPR_IS_NOT_NULL(sInstance))
    {
        DLLGETVERSIONPROC sDllGetVersionFunc = (DLLGETVERSIONPROC)::GetProcAddress(sInstance, "DllGetVersion");
        if (XPR_IS_NOT_NULL(sDllGetVersionFunc))
        {
            DLLVERSIONINFO sDllVersionInfo = {0};
            sDllVersionInfo.cbSize = sizeof(sDllVersionInfo);

            HRESULT sHResult = (*sDllGetVersionFunc)(&sDllVersionInfo);

            if (SUCCEEDED(sHResult))
            {
                if (XPR_IS_NOT_NULL(aMajorVersion))
                    *aMajorVersion = sDllVersionInfo.dwMajorVersion;

                if (XPR_IS_NOT_NULL(aMinorVersion))
                    *aMinorVersion = sDllVersionInfo.dwMinorVersion;

                if (XPR_IS_NOT_NULL(aBuildNumber))
                    *aBuildNumber = sDllVersionInfo.dwBuildNumber;
            }
        }

        ::FreeLibrary(sInstance);
    }
}
} // namespace fxb
