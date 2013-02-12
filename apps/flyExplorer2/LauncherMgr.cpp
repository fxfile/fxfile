//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "LauncherMgr.h"

#include <atlbase.h> // for CRegKey

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kLauncherProgramDebug[] = XPR_STRING_LITERAL("fxLauncher_dbg.exe");
static const xpr_tchar_t kLauncherProgram[]      = XPR_STRING_LITERAL("fxLauncher.exe");
static const xpr_tchar_t kLauncherWindowClass[]  = XPR_STRING_LITERAL("fxLauncher");

static const xpr_uint_t WM_LAUNCHER_COMMAND_LOAD   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Load"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_TRAY   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Tray"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_HOTKEY = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: HotKey"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_EXIT   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Exit"));

static const xpr_tchar_t kKeyName[] = XPR_STRING_LITERAL("Software\\Microsoft\\Windows\\CurrentVersion\\Run");

void LauncherMgr::startLauncher(xpr_ushort_t aGlobalHotKey, xpr_bool_t aShowTray)
{
    HWND sHwnd = ::FindWindow(kLauncherWindowClass, XPR_NULL);
    if (XPR_IS_NULL(sHwnd))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetModuleDir(sPath, XPR_MAX_PATH);

        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

#ifdef XPR_CFG_BUILD_DEBUG
        _tcscat(sPath, kLauncherProgramDebug);
#else
        _tcscat(sPath, kLauncherProgram);
#endif

        xpr_tchar_t szParameters[0xff] = {0};
        DWORD dwGlobalHotKey = aGlobalHotKey;
        _stprintf(szParameters, XPR_STRING_LITERAL("/load=%d, /tray=%d /hotkey=%d"), XPR_TRUE, aShowTray, dwGlobalHotKey);

        ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sPath, szParameters, XPR_NULL, 0);
    }
    else
    {
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_LOAD,   (WPARAM)XPR_TRUE,      0);
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_HOTKEY, (WPARAM)aGlobalHotKey, 0);
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_TRAY,   (WPARAM)aShowTray,     0);
    }
}

void LauncherMgr::stopLauncher(xpr_ushort_t aGlobalHotKey, xpr_bool_t aShowTray)
{
    HWND sHwnd = ::FindWindow(kLauncherWindowClass, XPR_NULL);
    if (XPR_IS_NOT_NULL(sHwnd))
    {
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_LOAD,   (WPARAM)XPR_FALSE,     0);
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_HOTKEY, (WPARAM)aGlobalHotKey, 0);
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_TRAY,   (WPARAM)aShowTray,     0);
        ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_EXIT,   0,                     0);
    }
}

void LauncherMgr::registerWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kKeyName) == ERROR_SUCCESS)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetModuleDir(sPath, XPR_MAX_PATH);

        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

#ifdef XPR_CFG_BUILD_DEBUG
        _tcscat(sPath, kLauncherProgramDebug);
#else
        _tcscat(sPath, kLauncherProgram);
#endif

        sRegKey.SetStringValue(XPR_STRING_LITERAL("fxLauncher"), sPath);
    }

    sRegKey.Close();
}

void LauncherMgr::unregisterWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kKeyName) == ERROR_SUCCESS)
    {
        sRegKey.DeleteValue(XPR_STRING_LITERAL("fxLauncher"));
    }

    sRegKey.Close();
}
