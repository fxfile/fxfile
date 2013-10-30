//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "about_tab_info_dlg.h"

#include "user_env.h"
#include "winapi_ex.h"

#include "app_ver.h"
#include "option.h"
#include "resource.h"

#include "gfl/libgfl.h"

#include <atlbase.h>

#include "updater_def.h"
#include "update_info_manager.h"
#include "updater_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace cmd
{
// user defined timer
enum
{
    kTimerIdUpdateCheckFirst = 100,
    kTimerIdUpdateCheck,
};

AboutTabInfoDlg::AboutTabInfoDlg(void)
    : super(IDD_ABOUT_TAB_ABOUT)
    , mUpdateInfoManager(XPR_NULL)
{
}

void AboutTabInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(AboutTabInfoDlg, super)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_ABOUT_REPORT, OnReport)
    ON_BN_CLICKED(IDC_ABOUT_SYSTEM, OnSystem)
    ON_BN_CLICKED(IDC_ABOUT_UPDATE, OnUpdate)
END_MESSAGE_MAP()

xpr_bool_t AboutTabInfoDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    UserEnv &sUserEnv = UserEnv::instance();

    // set program name and version
    xpr::tstring sProgram;
    xpr_tchar_t sVersion[0xff] = {0};

    getFullAppVer(sVersion);
    sProgram.format(XPR_STRING_LITERAL("fxfile %s"), sVersion);

    SetDlgItemText(IDC_ABOUT_PROGRAM, sProgram.c_str());

    // set windows version
    CString sWindows;
    sWindows.Format(XPR_STRING_LITERAL("Microsoft (R) Windows %d.%d build %d"), 
        LOWORD(sUserEnv.mOSVerInfo.dwMajorVersion),
        LOWORD(sUserEnv.mOSVerInfo.dwMinorVersion),
        LOWORD(sUserEnv.mOSVerInfo.dwBuildNumber));
    SetDlgItemText(IDC_ABOUT_WINDOWS, sWindows);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.title")));
    SetDlgItemText(IDC_ABOUT_LABEL_ACCEPT, gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.label.accept")));
    SetDlgItemText(IDOK,                   gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDC_ABOUT_SYSTEM,       gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.button.system_info")));
    SetDlgItemText(IDC_ABOUT_REPORT,       gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.button.report")));
    SetDlgItemText(IDC_ABOUT_UPDATE,       gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.button.update")));

    // disable update button
    GetDlgItem(IDC_ABOUT_UPDATE)->EnableWindow(XPR_FALSE);

    if (XPR_IS_TRUE(gOpt->mConfig.mUpdateCheckEnable))
    {
        // start update check timer
        SetTimer(kTimerIdUpdateCheckFirst, 0, XPR_NULL);
    }
    else
    {
        GetDlgItem(IDC_ABOUT_UPDATE_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_ABOUT_UPDATE     )->ShowWindow(SW_HIDE);
    }

    return XPR_TRUE;
}

void AboutTabInfoDlg::OnDestroy(void) 
{
    // stop update check timer
    KillTimer(kTimerIdUpdateCheckFirst);
    KillTimer(kTimerIdUpdateCheck);

    XPR_SAFE_DELETE(mUpdateInfoManager);

    super::OnDestroy();
}

void AboutTabInfoDlg::OnTimer(UINT_PTR aIdEvent)
{
    if (aIdEvent == kTimerIdUpdateCheckFirst ||
        aIdEvent == kTimerIdUpdateCheck)
    {
        if (aIdEvent == kTimerIdUpdateCheckFirst)
        {
            KillTimer(aIdEvent);
            SetTimer(kTimerIdUpdateCheck, 300, XPR_NULL);
        }

        xpr_rcode_t  sRcode;
        xpr_tchar_t  sStatus[0xff];
        xpr_tchar_t  sCheckedVersion[0xff];
        UpdateInfo   sUpdateInfo = {0};
        xpr::tstring sUpdateHomeDir;

        if (XPR_IS_NULL(mUpdateInfoManager))
        {
            if (UpdateInfoManager::getDefaultUpdateHomeDir(sUpdateHomeDir) == XPR_FALSE)
            {
                return;
            }

            mUpdateInfoManager = new UpdateInfoManager;
            mUpdateInfoManager->setUpdateHomeDir(sUpdateHomeDir);

            sRcode = mUpdateInfoManager->openUpdateInfo();
            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                XPR_SAFE_DELETE(mUpdateInfoManager);

                UpdaterManager::checkNow();
                return;
            }
        }

        if (XPR_IS_NOT_NULL(mUpdateInfoManager))
        {
            sRcode = mUpdateInfoManager->readUpdateInfo(sUpdateInfo);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                switch (sUpdateInfo.mStatus)
                {
                case kUpdateStatusCheckInProgress:
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.checking")));
                    break;

                case kUpdateStatusCheckFailed:
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.check_failed")));
                    break;

                case kUpdateStatusExistNewVer:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.checked"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;

                case kUpdateStatusLastestVer:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.latest_version"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);

                    KillTimer(aIdEvent);
                    break;

                case kUpdateStatusDownloading:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.downloading"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;

                case kUpdateStatusDownloaded:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.downloaded"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    GetDlgItem(IDC_ABOUT_UPDATE)->ShowWindow(SW_SHOW);

                    KillTimer(aIdEvent);
                    break;

                case kUpdateStatusDownloadFailed:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.label.update.download_failed"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;
                }
            }
        }
    }

    CWnd::OnTimer(aIdEvent);
}

void AboutTabInfoDlg::OnReport(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetModulePath(sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = XPR_FALSE;
    xpr_tchar_t sVersion[0xff];
    void *sBlock = new xpr_char_t[5120];
    if (GetFileVersionInfo(sPath, 0, 5120, sBlock))
    {
        struct LangAndCodePage
        {
            WORD wLanguage;
            WORD wCodePage;
        } *sTranslate;

        xpr_uint_t sTranslateLength = 0;
        if (VerQueryValue(sBlock,
                          XPR_STRING_LITERAL("\\VarFileInfo\\Translation"),
                          (LPVOID*)&sTranslate,
                          &sTranslateLength))
        {
            xpr_tchar_t sSubBlock[0xff] = {0};
            void *sBuffer = XPR_NULL;
            xpr_uint_t sBytes = 0;
            xpr_sint_t i, sCount;

            sCount = sTranslateLength / sizeof(LangAndCodePage);
            for (i = 0; i < sCount; ++i)
            {
                _stprintf(sSubBlock, XPR_STRING_LITERAL("\\StringFileInfo\\%04x%04x\\FileVersion"),
                    sTranslate[i].wLanguage, sTranslate[i].wCodePage);

                if (VerQueryValue(sBlock, sSubBlock, &sBuffer, &sBytes))
                {
                    _tcscpy(sVersion, (xpr_tchar_t *)sBuffer);
                    sResult = XPR_TRUE;
                    break;
                }
            }
        }
    }

    XPR_SAFE_DELETE_ARRAY(sBlock);

    if (sResult == XPR_FALSE)
        _stprintf(sVersion, XPR_STRING_LITERAL("1"));

    xpr_tchar_t sUrl[XPR_MAX_URL_LENGTH+50] = {0};
    _stprintf(sUrl, XPR_STRING_LITERAL("mailto:flychk@flychk.com?subject=fxfile v%s - report an issue"), sVersion);
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sUrl, XPR_NULL, XPR_NULL, 0);
}

void AboutTabInfoDlg::OnSystem(void)
{
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), XPR_STRING_LITERAL("msinfo32.exe"), XPR_NULL, XPR_NULL, 0);
}

void AboutTabInfoDlg::OnUpdate(void)
{
    xpr_rcode_t sRcode;
    UpdateInfo  sUpdateInfo = {0};
    xpr_tchar_t sDownloadedFilePath[XPR_MAX_PATH + 1] = {0};

    sRcode = mUpdateInfoManager->readUpdateInfo(sUpdateInfo);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        if (sUpdateInfo.mStatus == kUpdateStatusDownloaded)
        {
            UpdateInfoManager::getDownloadedFilePath(sUpdateInfo, sDownloadedFilePath, XPR_MAX_PATH);

            HINSTANCE sInstance = ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sDownloadedFilePath, XPR_NULL, XPR_NULL, SW_SHOW);
            if ((xpr_sint_t)(xpr_sintptr_t)sInstance < 32)
            {
                xpr_sint_t sErrorCode = (xpr_sint_t)(xpr_sintptr_t)sInstance;

                xpr_tchar_t sMsg[0xff] = {0};
                _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.about.tab.info.msg.installer_execute_error"), XPR_STRING_LITERAL("%d")), sErrorCode);
                ::MessageBox(XPR_NULL, sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                if (XPR_IS_TRUE(gOpt->mConfig.mUpdateCheckEnable))
                {
                    // restart update checker program
                    UpdaterManager::shutdownProcess();
                    UpdaterManager::startupProcess();
                }
            }
        }
    }
}
} // namespace cmd
} // namespace fxfile
