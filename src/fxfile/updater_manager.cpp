//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "updater_manager.h"
#include "conf_dir.h"
#include "updater_def.h"
#include "app_ver.h"

#include "conf_file_ex.h"

#include <atlbase.h> // for CRegKey

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace
{
const xpr_tchar_t kUpdaterProgramDebug          [] = XPR_STRING_LITERAL("updater\\fxfile-updater_dbg.exe");
const xpr_tchar_t kUpdaterProgram               [] = XPR_STRING_LITERAL("updater\\fxfile-updater.exe");
const xpr_tchar_t kUpdaterSection               [] = XPR_STRING_LITERAL("config");
const xpr_tchar_t kUpdaterRegKey                [] = XPR_STRING_LITERAL("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
const xpr_tchar_t kUpdaterRegValueNameWinStartup[] = XPR_STRING_LITERAL("fxfile-updater");
} // namespace anonymous

static HWND findUpdaterWindow(void)
{
    return ::FindWindow(fxfile::base::kUpdaterClassName, XPR_NULL);
}

void UpdaterManager::writeUpdaterConf(Option::Config &aConfig)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    ConfDir &sConfDir = ConfDir::instance();
    sConfDir.getSavePath(ConfDir::TypeUpdater, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile updater configuration file"));

    ConfFile::Section *sSection;

    sSection = sConfFile.addSection(kUpdaterSection);
    XPR_ASSERT(sSection != XPR_NULL);

    xpr_tchar_t sAppVer[0xff] = {0};
    getAppVer(sAppVer);

    sConfFile.setValueB(sSection, kUpdaterConfKeyEnable,        aConfig.mUpdateCheckEnable);
    sConfFile.setValueS(sSection, kUpdaterConfKeyCurVer,        sAppVer);
    sConfFile.setValueB(sSection, kUpdaterConfKeyCheckMinorVer, aConfig.mUpdateCheckMinorVer);

    sConfFile.save(xpr::CharSetUtf8);
}

void UpdaterManager::startupProcess(void)
{
    HWND sHwnd = findUpdaterWindow();
    if (XPR_IS_NULL(sHwnd))
    {
        xpr::string sPath;
        xpr::FileSys::getExeDir(sPath);

        sPath += XPR_STRING_LITERAL("\\");

#ifdef XPR_CFG_BUILD_DEBUG
        sPath += kUpdaterProgramDebug;
#else
        sPath += kUpdaterProgram;
#endif

        ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sPath.c_str(), XPR_NULL, XPR_NULL, 0);
    }
    else
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPDATER_COMMAND_RELOAD_CONF, 0, 0);
    }
}

void UpdaterManager::shutdownProcess(void)
{
    HWND sHwnd = findUpdaterWindow();
    if (XPR_IS_NOT_NULL(sHwnd))
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPDATER_COMMAND_RELOAD_CONF, 0, 0);
        ::PostMessage(sHwnd, fxfile::base::WM_UPDATER_COMMAND_EXIT ,       0, 0);
    }
}

void UpdaterManager::checkNow(void)
{
    HWND sHwnd = findUpdaterWindow();
    if (XPR_IS_NOT_NULL(sHwnd))
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPDATER_COMMAND_CHECK_NOW, 0, 0);
    }
    else
    {
        writeUpdaterConf(gOpt->mConfig);

        startupProcess();
    }
}

void UpdaterManager::registerWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kUpdaterRegKey) == ERROR_SUCCESS)
    {
        xpr::string sPath;
        xpr::FileSys::getExeDir(sPath);

        sPath += XPR_FILE_SEPARATOR_STRING;

#ifdef XPR_CFG_BUILD_DEBUG
        sPath += kUpdaterProgramDebug;
#else
        sPath += kUpdaterProgram;
#endif

        sRegKey.SetStringValue(kUpdaterRegValueNameWinStartup, sPath.c_str());
    }

    sRegKey.Close();
}

void UpdaterManager::unregisterWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kUpdaterRegKey) == ERROR_SUCCESS)
    {
        sRegKey.DeleteValue(kUpdaterRegValueNameWinStartup);
    }

    sRegKey.Close();
}

xpr_bool_t UpdaterManager::getUpdateInfo(fxfile::base::UpdateInfo &aUpdateInfo)
{

    return XPR_TRUE;
}
} // namespace fxfile
