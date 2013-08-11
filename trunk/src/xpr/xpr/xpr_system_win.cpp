//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_system.h"
#include "xpr_rcode.h"
#include "xpr_char.h"

namespace xpr
{
static SystemInfo gSystemInfo;

XPR_INLINE xpr_rcode_t initOsVer(xpr_uint_t &aOsVer);

XPR_DL_API xpr_rcode_t initSystemInfo(void)
{
    xpr_rcode_t sRcode = initOsVer(gSystemInfo.mOsVer);
    if (sRcode != XPR_RCODE_SUCCESS)
        return sRcode;

    return XPR_RCODE_SUCCESS;
}

// reference:
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms724833(v=vs.85).aspx
XPR_INLINE xpr_rcode_t initOsVer(xpr_uint_t &aOsVer)
{
    OSVERSIONINFOEX sOsVerInfo = {0,};
    sOsVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    xpr_bool_t sResult = ::GetVersionEx((LPOSVERSIONINFO)&sOsVerInfo);
    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    xpr_uint_t sWinVer = kOsVerWinUnknown;
    xpr_bool_t sWorkstation = (sOsVerInfo.wProductType == VER_NT_WORKSTATION) ? XPR_TRUE : XPR_FALSE;
    xpr_uint_t sServicePack = 0;

    if (sOsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 9x
    {
        if (sOsVerInfo.dwMajorVersion < 4)
        {
            sWinVer = kOsVerWinUnknown;
        }
        else if (sOsVerInfo.dwMajorVersion == 4)
        {
            if (sOsVerInfo.dwMinorVersion < 10)
            {
                if (sOsVerInfo.szCSDVersion[1] == 'B' || sOsVerInfo.szCSDVersion[1] == 'C')
                    sWinVer = kOsVerWin95Osr2;
                else
                    sWinVer = kOsVerWin95;
            }
            else if (sOsVerInfo.dwMinorVersion == 10)
            {
                if (sOsVerInfo.szCSDVersion[1] == 'A')
                    sWinVer = kOsVerWin98SE;
                else
                    sWinVer = kOsVerWin98;
            }
            else if (sOsVerInfo.dwMinorVersion == 90)
            {
                sWinVer = kOsVerWinMe;
            }
            else
            {
                sWinVer = kOsVerWinMe;
            }
        }
    }
    else if (sOsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        xpr_char_t *sDesc = sOsVerInfo.szCSDVersion;
        while (*sDesc != 0 && isDigit((xpr_char_t)(*sDesc)) != XPR_TRUE)
            sDesc++;

        if (*sDesc != 0)
        {
            sServicePack = _ttoi(sDesc);
        }

        if (sOsVerInfo.dwMajorVersion == 3)
        {
            if (sOsVerInfo.dwMinorVersion < 50)
                sWinVer = kOsVerWinUnknown;
            else if (sOsVerInfo.dwMinorVersion == 50)
                sWinVer = kOsVerWinNT35;
            else
                sWinVer = kOsVerWinNT351;
        }
        else if (sOsVerInfo.dwMajorVersion == 4)
        {
             if (sServicePack < 2)
                 sWinVer = kOsVerWinNT4;
             else
             {
                 switch (sServicePack)
                 {
                 case 2:  sWinVer = kOsVerWinNT4Sp2; break;
                 case 3:  sWinVer = kOsVerWinNT4Sp3; break;
                 case 4:  sWinVer = kOsVerWinNT4Sp4; break;
                 case 5:  sWinVer = kOsVerWinNT4Sp5; break;
                 default: sWinVer = kOsVerWinNT4Sp6; break;
                 }
             }
        }
        else if (sOsVerInfo.dwMajorVersion == 5)
        {
            if (sOsVerInfo.dwMinorVersion == 0)
            {
                switch (sServicePack)
                {
                case 0:  sWinVer = kOsVerWin2000;    break;
                case 1:  sWinVer = kOsVerWin2000Sp1; break;
                case 2:  sWinVer = kOsVerWin2000Sp2; break;
                case 3:  sWinVer = kOsVerWin2000Sp3; break;
                default: sWinVer = kOsVerWin2000Sp4; break;
                }
            }
            else if (sOsVerInfo.dwMinorVersion == 1)
            {
                switch (sServicePack)
                {
                case 0:  sWinVer = kOsVerWinXP;    break;
                case 1:  sWinVer = kOsVerWinXPSp1; break;
                case 2:  sWinVer = kOsVerWinXPSp2; break;
                default: sWinVer = kOsVerWinXPSp3; break;
                }
            }
            else if (sOsVerInfo.dwMinorVersion == 2)
            {
                SYSTEM_INFO si = {0};
                ::GetSystemInfo(&si);

                if (sWorkstation == XPR_TRUE && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                {
                    sWinVer = kOsVerWinXPx64;
                }
                else
                {
                    if (GetSystemMetrics(SM_SERVERR2) != 0)
                    {
                        sWinVer = kOsVerWin2003R2;
                    }
                    else
                    {
                        sWinVer = kOsVerWin2003;
                    }
                }
            }
            else
            {
                sWinVer = kOsVerWinXP;
            }
        }
        else if (sOsVerInfo.dwMajorVersion == 6)
        {
            if (sWorkstation == XPR_TRUE)
            {
                if (sOsVerInfo.dwMinorVersion == 0)
                {
                    switch (sServicePack)
                    {
                    case 0:  sWinVer = kOsVerWinVista;    break;
                    case 1:  sWinVer = kOsVerWinVistaSp1; break;
                    case 2:  sWinVer = kOsVerWinVistaSp2; break;
                    default: sWinVer = kOsVerWinVistaSp3; break;
                    }
                }
                else if (sOsVerInfo.dwMinorVersion == 1)
                {
                    switch (sServicePack)
                    {
                    case 0:  sWinVer = kOsVerWin7;    break;
                    case 1:  sWinVer = kOsVerWin7Sp1; break;
                    default: sWinVer = kOsVerWin7Sp2; break;
                    }
                }
                else if (sOsVerInfo.dwMinorVersion == 2)
                {
                    sWinVer = kOsVerWin8;
                }
                else
                {
                    sWinVer = kOsVerWinHigher;
                }
            }
            else
            {
                switch (sOsVerInfo.dwMinorVersion)
                {
                case 0:  sWinVer = kOsVerWin2008;   break;
                case 1:  sWinVer = kOsVerWin2008R2; break;
                case 2:  sWinVer = kOsVerWin2012;   break;
                default: sWinVer = kOsVerWinHigher; break;
                }
            }
        }
        else
        {
            sWinVer = kOsVerWinHigher;
        }
    }
    else
    {
        sWinVer = kOsVerWinUnknown;
    }

    aOsVer = sWinVer;

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API SystemInfo *getSystemInfo(void)
{
    return &gSystemInfo;
}

XPR_DL_API xpr_uint_t getOsVer(void)
{
    return gSystemInfo.mOsVer;
}
} // namespace xpr
