//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "upchecker_manager.h"
#include "conf_dir.h"
#include "upchecker_def.h"
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
const xpr_tchar_t kUpcheckerProgramDebug          [] = XPR_STRING_LITERAL("upchecker\\fxfile-upchecker_dbg.exe");
const xpr_tchar_t kUpcheckerProgram               [] = XPR_STRING_LITERAL("upchecker\\fxfile-upchecker.exe");
const xpr_tchar_t kUpcheckerProgramOptionConfDir  [] = XPR_STRING_LITERAL("--conf_dir");
const xpr_tchar_t kUpcheckerSection               [] = XPR_STRING_LITERAL("config");
const xpr_tchar_t kUpcheckerRegKey                [] = XPR_STRING_LITERAL("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
const xpr_tchar_t kUpcheckerRegValueNameWinStartup[] = XPR_STRING_LITERAL("fxfile-upchecker");
} // namespace anonymous

static HWND findUpcheckerWindow(void)
{
    return ::FindWindow(fxfile::base::kUpcheckerClassName, XPR_NULL);
}

void UpcheckerManager::writeConfFile(Option::Config &aConfig)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    ConfDir &sConfDir = ConfDir::instance();
    sConfDir.getSavePath(ConfDir::TypeUpchecker, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile update checker configuration file"));

    ConfFile::Section *sSection;

    sSection = sConfFile.addSection(kUpcheckerSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sConfFile.setValueB(sSection, kUpcheckerConfKeyEnable, aConfig.mUpdateCheckEnable);

    sConfFile.save(xpr::CharSetUtf8);
}

void UpcheckerManager::startupProcess(void)
{
    HWND sHwnd = findUpcheckerWindow();
    if (XPR_IS_NULL(sHwnd))
    {
        xpr::string sPath;

        xpr::FileSys::getExeDir(sPath);

        sPath += XPR_STRING_LITERAL("\\");

#ifdef XPR_CFG_BUILD_DEBUG
        sPath += kUpcheckerProgramDebug;
#else
        sPath += kUpcheckerProgram;
#endif

        ConfDir &sConfDir = ConfDir::instance();
        const xpr_tchar_t *sLoadConfDir = sConfDir.getConfDir();

        xpr::string sParameters;
        sParameters.format(XPR_STRING_LITERAL("%s=%s"), kUpcheckerProgramOptionConfDir, sLoadConfDir);

        ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sPath.c_str(), sParameters.c_str(), XPR_NULL, 0);
    }
    else
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPCHECKER_COMMAND_RELOAD_CONF, 0, 0);
    }
}

void UpcheckerManager::shutdownProcess(void)
{
    HWND sHwnd = findUpcheckerWindow();
    if (XPR_IS_NOT_NULL(sHwnd))
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPCHECKER_COMMAND_RELOAD_CONF, 0, 0);
        ::PostMessage(sHwnd, fxfile::base::WM_UPCHECKER_COMMAND_EXIT ,       0, 0);
    }
}

void UpcheckerManager::checkNow(void)
{
    HWND sHwnd = findUpcheckerWindow();
    if (XPR_IS_NOT_NULL(sHwnd))
    {
        ::PostMessage(sHwnd, fxfile::base::WM_UPCHECKER_COMMAND_CHECK_NOW, 0, 0);
    }
    else
    {
        writeConfFile(gOpt->mConfig);

        startupProcess();
    }
}

void UpcheckerManager::registerWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kUpcheckerRegKey) == ERROR_SUCCESS)
    {
        xpr::string sPath;
        xpr::FileSys::getExeDir(sPath);
        sPath += XPR_FILE_SEPARATOR_STRING;

#ifdef XPR_CFG_BUILD_DEBUG
        sPath += kUpcheckerProgramDebug;
#else
        sPath += kUpcheckerProgram;
#endif

        ConfDir &sConfDir = ConfDir::instance();
        const xpr_tchar_t *sLoadConfDir = sConfDir.getConfDir();

        xpr::string sParameters;
        sParameters.format(XPR_STRING_LITERAL("%s=\"%s\""), kUpcheckerProgramOptionConfDir, sLoadConfDir);

        xpr::string sValue;
        sValue += XPR_STRING_LITERAL("\"");
        sValue += sPath;
        sValue += XPR_STRING_LITERAL("\" ");
        sValue += sParameters;

        sRegKey.SetStringValue(kUpcheckerRegValueNameWinStartup, sValue.c_str());
    }

    sRegKey.Close();
}

void UpcheckerManager::unregisterWinStartup(void)
{
    CRegKey sRegKey;
    if (sRegKey.Open(HKEY_CURRENT_USER, kUpcheckerRegKey) == ERROR_SUCCESS)
    {
        sRegKey.DeleteValue(kUpcheckerRegValueNameWinStartup);
    }

    sRegKey.Close();
}

xpr_bool_t UpcheckerManager::getUpdateInfo(fxfile::base::UpdateInfo &aUpdateInfo)
{

    return XPR_TRUE;
}
} // namespace fxfile
