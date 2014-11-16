//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "main_window.h"
#include "resource.h"

#include "update_checker.h"
#include "update_info_manager.h"
#include "conf_file_ex.h"
#include "env_path.h"
#include "program_opts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace upchecker
{
static const xpr_tchar_t kUpdateCheckUrl             [] = XPR_STRING_LITERAL("http://flychk.com/update/fxfile/upchecker_meta-data"); // TODO: fxfile.org change

static const xpr_tchar_t kConfFileName               [] = XPR_STRING_LITERAL("fxfile-upchecker.conf");

static const xpr_tchar_t kEnvAppDataName             [] = XPR_STRING_LITERAL("APPDATA");
static const xpr_tchar_t kEnvLocalAppDataName        [] = XPR_STRING_LITERAL("LOCALAPPDATA");
static const xpr_tchar_t kEnvConfHomeName            [] = XPR_STRING_LITERAL("FXFILE_CONF_HOME");

static const xpr_tchar_t kProgramDir                 [] = XPR_STRING_LITERAL("fxfile");
static const xpr_tchar_t kUpdateDir                  [] = XPR_STRING_LITERAL("update");
static const xpr_tchar_t kConfDir                    [] = XPR_STRING_LITERAL("conf");
static const xpr_tchar_t kLogDir                     [] = XPR_STRING_LITERAL("log");

// ex) %APPDATA%/fxfile/.fxfile
static const xpr_tchar_t kRootConfFileName           [] = XPR_STRING_LITERAL(".fxfile");
static const xpr_tchar_t kRootConfFileSection        [] = XPR_STRING_LITERAL(".fxfile");
static const xpr_tchar_t kRootConfFileKeyConfHome    [] = XPR_STRING_LITERAL("conf_home");

// user defined timer
enum
{
    kTimerIdCheckFirst = 100,
    kTimerIdCheck,
};

MainWindow::MainWindow(void)
    : mUpdateChecker(XPR_NULL)
    , mUpdateInfoManager(XPR_NULL)
{
}

MainWindow::~MainWindow(void)
{
    XPR_SAFE_DELETE(mUpdateChecker);
    XPR_SAFE_DELETE(mUpdateInfoManager);
}

xpr_bool_t MainWindow::registerWindowClass(void)
{
    xpr_bool_t sResult = FALSE;

    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    if (::GetClassInfo(sInstance, fxfile::base::kUpcheckerClassName, &sWndClass) == XPR_FALSE)
    {
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = fxfile::base::kUpcheckerClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return sResult;
}

BOOL MainWindow::Create(void)
{
    registerWindowClass();

    return CreateEx(0, fxfile::base::kUpcheckerClassName, fxfile::base::kUpcheckerClassName, 0, 0, 0, 0, 0, 0, 0);
}

BEGIN_MESSAGE_MAP(MainWindow, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_TIMER()

    ON_REGISTERED_MESSAGE(fxfile::base::WM_UPCHECKER_COMMAND_RELOAD_CONF, OnCommandReloadConf)
    ON_REGISTERED_MESSAGE(fxfile::base::WM_UPCHECKER_COMMAND_CHECK_NOW,   OnCommandCheckNow)
    ON_REGISTERED_MESSAGE(fxfile::base::WM_UPCHECKER_COMMAND_EXIT,        OnCommandExit)
END_MESSAGE_MAP()

int MainWindow::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CWnd::OnCreate(aCreateStruct) == -1)
        return -1;

    ProgramOpts sProgramOpts;
    sProgramOpts.parse();

    const xpr::string &sConfDir = sProgramOpts.getConfDir();
    if (sConfDir.empty() == XPR_TRUE)
    {
        return -1;
    }

    mConfHomeDir = sConfDir;

    xpr_bool_t  sResult;
    xpr::string sUpdateHomeDir;

    sResult = getUpdateHomeDir(sUpdateHomeDir);
    if (XPR_IS_FALSE(sResult))
    {
        return -1;
    }

    if (XPR_IS_TRUE(loadConfig()))
    {
        xpr_rcode_t sRcode;

        UpdateChecker::initStatic();

        mUpdateChecker = new UpdateChecker;
        mUpdateChecker->setDir(sUpdateHomeDir);
        mUpdateChecker->setUrl(kUpdateCheckUrl);

        mUpdateInfoManager = new UpdateInfoManager;
        mUpdateInfoManager->setUpdateHomeDir(sUpdateHomeDir.c_str());

        sRcode = mUpdateInfoManager->createUpdateInfo();
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
        }

        // check new version on program loading time
        SetTimer(kTimerIdCheckFirst, 0, XPR_NULL);
    }
    else
    {
    }

    return 0;
}

void MainWindow::OnDestroy(void)
{
    KillTimer(kTimerIdCheckFirst);
    KillTimer(kTimerIdCheck);

    XPR_SAFE_DELETE(mUpdateChecker);

    UpdateChecker::finalizeStatic();

    CWnd::OnDestroy();
}

xpr_bool_t MainWindow::getUpdateHomeDir(xpr::string &aUpdateHomeDir) const
{
    xpr_bool_t  sResult = XPR_FALSE;
    xpr::string sUpdateHomeDir;
    xpr_rcode_t sRcode;

    sRcode = xpr::getEnv(kEnvAppDataName, sUpdateHomeDir);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sUpdateHomeDir += XPR_FILE_SEPARATOR_STRING;
        sUpdateHomeDir += kProgramDir;
        sUpdateHomeDir += XPR_FILE_SEPARATOR_STRING;
        sUpdateHomeDir += kUpdateDir;

        sRcode = xpr::FileSys::mkdir_recursive(sUpdateHomeDir);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            aUpdateHomeDir = sUpdateHomeDir;

            return XPR_TRUE;
        }
    }

    sRcode = xpr::FileSys::getTempDir(sUpdateHomeDir);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sUpdateHomeDir += XPR_FILE_SEPARATOR_STRING;
        sUpdateHomeDir += kProgramDir;
        sUpdateHomeDir += XPR_FILE_SEPARATOR_STRING;
        sUpdateHomeDir += kUpdateDir;

        sRcode = xpr::FileSys::mkdir_recursive(sUpdateHomeDir);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            aUpdateHomeDir = sUpdateHomeDir;

            return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

void MainWindow::OnTimer(UINT_PTR aIdEvent)
{
    if (aIdEvent == kTimerIdCheckFirst ||
        aIdEvent == kTimerIdCheck)
    {
        xpr_bool_t  sNeedCheck = XPR_TRUE;
        xpr_rcode_t sRcode;
        xpr_sint_t  sMajorVer;
        xpr_sint_t  sMinorVer;
        xpr_sint_t  sPatchVer;
        xpr::string sCheckedVersion;
        xpr::string sDownloadedFilePath;
        xpr_size_t  sInputBytes;
        xpr_size_t  sOutputBytes;

        if (XPR_IS_TRUE(mUpdateChecker->isChecked()))
        {
            sNeedCheck = XPR_FALSE;
        }

        if (XPR_IS_TRUE(sNeedCheck))
        {
            mUpdateInfo.mStatus = kUpdateStatusCheckInProgress;

            sRcode = mUpdateInfoManager->writeUpdateInfo(mUpdateInfo);

            // check
            if (mUpdateChecker->check() == XPR_TRUE)
            {
                const MetaDataFile::MetaData &sMetaData = mUpdateChecker->getMetaData();

                _stscanf(sMetaData.mVersion.c_str(),
                    XPR_STRING_LITERAL("%d.%d.%d"),
                    &sMajorVer, &sMinorVer, &sPatchVer);

                mUpdateInfo.mStatus   = kUpdateStatusChecked;
                mUpdateInfo.mMajorVer = sMajorVer;
                mUpdateInfo.mMinorVer = sMinorVer;
                mUpdateInfo.mPatchVer = sPatchVer;

                sInputBytes = sMetaData.mNewUrl.length() * sizeof(xpr_tchar_t);
                sOutputBytes = XPR_MAX_URL_LENGTH * sizeof(xpr_wchar_t);
                XPR_TCS_TO_UTF16(sMetaData.mNewUrl.c_str(), &sInputBytes, mUpdateInfo.mNewUrl, &sOutputBytes);
                mUpdateInfo.mNewUrl[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

                sRcode = mUpdateInfoManager->writeUpdateInfo(mUpdateInfo);
                if (XPR_RCODE_IS_SUCCESS(sRcode))
                {
                    KillTimer(aIdEvent);
                    SetTimer(kTimerIdCheck, mConfig.mCheckPeriodTime, XPR_NULL);
                }
            }
            else
            {
                mUpdateInfo.mStatus = kUpdateStatusCheckFailed;

                sRcode = mUpdateInfoManager->writeUpdateInfo(mUpdateInfo);
                if (XPR_RCODE_IS_SUCCESS(sRcode))
                {
                    // recheck after 5 seconds
                    KillTimer(aIdEvent);
                    SetTimer(kTimerIdCheckFirst, 5000, XPR_NULL);
                }
            }
        }

        return;
    }

    CWnd::OnTimer(aIdEvent);
}

xpr_bool_t MainWindow::getConfHomeDir(xpr::string &aDir)
{
    xpr_rcode_t sRcode;
    xpr::string sConfPath;
    xpr::string sExeConfDir;
    xpr::string sEnvConfDir;
    xpr::string sAppDataConfDir;
    xpr::string sLocalAppDataConfDir;

    // [1] .fxfile of program directory
    sRcode = xpr::FileSys::getExeDir(sExeConfDir);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    sConfPath  = sExeConfDir;
    sConfPath += XPR_FILE_SEPARATOR_STRING;
    sConfPath += kRootConfFileName;

    if (XPR_IS_TRUE(xpr::FileSys::exist(sConfPath)))
    {
        if (XPR_IS_TRUE(getConfHomeDirFromConfFile(sConfPath, aDir)))
        {
            return XPR_TRUE;
        }
    }

    // [2] FXFILE_CONF_HOME environment varaible
    sRcode = xpr::getEnv(kEnvConfHomeName, sEnvConfDir);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sConfPath  = sEnvConfDir;
        sConfPath += XPR_FILE_SEPARATOR_STRING;
        sConfPath += kRootConfFileName;

        if (XPR_IS_TRUE(xpr::FileSys::exist(sConfPath)))
        {
            aDir = sEnvConfDir;

            return XPR_TRUE;
        }
    }

#if defined(XPR_CFG_OS_WINDOWS)
    // [3] %AppData%/fxfile/.fxfile
    sRcode = xpr::getEnv(kEnvAppDataName, sAppDataConfDir);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sConfPath  = sAppDataConfDir;
        sConfPath += XPR_FILE_SEPARATOR_STRING;
        sConfPath += kProgramDir;
        sConfPath += XPR_FILE_SEPARATOR_STRING;
        sConfPath += kRootConfFileName;

        if (XPR_IS_TRUE(xpr::FileSys::exist(sConfPath)))
        {
            if (XPR_IS_TRUE(getConfHomeDirFromConfFile(sConfPath, aDir)))
            {
                return XPR_TRUE;
            }
        }
    }

    // [4] %LocalAppData%/fxfile/.fxfile
    sRcode = xpr::getEnv(kEnvLocalAppDataName, sLocalAppDataConfDir);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sConfPath  = sLocalAppDataConfDir;
        sConfPath += XPR_FILE_SEPARATOR_STRING;
        sConfPath += kProgramDir;
        sConfPath += XPR_FILE_SEPARATOR_STRING;
        sConfPath += kRootConfFileName;

        if (XPR_IS_TRUE(xpr::FileSys::exist(sConfPath)))
        {
            if (XPR_IS_TRUE(getConfHomeDirFromConfFile(sConfPath, aDir)))
            {
                return XPR_TRUE;
            }
        }
    }
#else
#error Please, support for your OS support. <MainWindow.cpp>
#endif

    return XPR_TRUE;
}

xpr_bool_t MainWindow::getConfHomeDirFromConfFile(const xpr::string &aConfPath, xpr::string &aDir)
{
    fxfile::base::ConfFileEx sConfFile(aConfPath);
    if (XPR_IS_TRUE(sConfFile.load()))
    {
        fxfile::base::ConfFile::Section *sSection = sConfFile.findSection(kRootConfFileSection);
        if (XPR_IS_NOT_NULL(sSection))
        {
            const xpr_tchar_t *sValue = sConfFile.getValueS(sSection, kRootConfFileKeyConfHome, XPR_NULL);
            if (XPR_IS_NOT_NULL(sValue))
            {
                xpr::string sDir = sValue;

                fxfile::base::EnvPath::instance().resolve(sDir, aDir);

                xpr_size_t sLen = aDir.length();
                if (sLen > 1)
                {
                    if (aDir.back() == XPR_FILE_SEPARATOR)
                    {
                        aDir.erase(sLen - 1, 1);
                    }
                }

                return XPR_TRUE;
            }
        }
    }

    return XPR_FALSE;
}

xpr_bool_t MainWindow::loadConfig(void)
{
    xpr::string sConfPath(mConfHomeDir);
    sConfPath += XPR_FILE_SEPARATOR_STRING;
    sConfPath += kConfFileName;

    fxfile::base::ConfFileEx sConfFile(sConfPath);

    if (sConfFile.load() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    fxfile::base::ConfFile::Section *sSection = sConfFile.findSection(XPR_STRING_LITERAL("config"));
    if (XPR_IS_NULL(sSection))
    {
        return XPR_FALSE;
    }

    mConfig.mEnable          = sConfFile.getValueB(sSection, kUpcheckerConfKeyEnable,          XPR_FALSE);
    mConfig.mCheckPeriodTime = sConfFile.getValueI(sSection, kUpcheckerConfKeyCheckPeriodTime, 24 * 60 * 60 * 1000);

    return XPR_TRUE;
}

LRESULT MainWindow::OnCommandReloadConf(WPARAM wParam, LPARAM lParam)
{
    xpr_bool_t sResult = loadConfig();
    if (XPR_IS_TRUE(sResult))
    {
        if (XPR_IS_FALSE(mConfig.mEnable))
        {
            KillTimer(kTimerIdCheckFirst);
            KillTimer(kTimerIdCheck);

            return 0;
        }

        KillTimer(kTimerIdCheck);
        SetTimer(kTimerIdCheck, mConfig.mCheckPeriodTime, XPR_NULL);
    }

    return 0;
}

LRESULT MainWindow::OnCommandCheckNow(WPARAM wParam, LPARAM lParam)
{
    HWND sHwnd = (HWND)wParam;

    mUpdateChecker->check();

    return 0;
}

LRESULT MainWindow::OnCommandExit(WPARAM wParam, LPARAM lParam)
{
    PostMessage(WM_CLOSE);

    return 0;
}
} // namespace fxfile
} // namespace upchecker
