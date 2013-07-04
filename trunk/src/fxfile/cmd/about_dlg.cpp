//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "about_dlg.h"

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
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

AboutDlg::AboutDlg(void)
    : super(IDD_ABOUT)
    , mUpdateInfoManager(XPR_NULL)
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FXFILE, mProgramWnd);
}

BEGIN_MESSAGE_MAP(AboutDlg, super)
    ON_WM_LBUTTONDOWN()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_ABOUT_BUG,    OnBug)
    ON_BN_CLICKED(IDC_ABOUT_SYSTEM, OnSystem)
    ON_BN_CLICKED(IDC_ABOUT_UPDATE, OnUpdate)
END_MESSAGE_MAP()

xpr_bool_t AboutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    UserEnv &sUserEnv = UserEnv::instance();

    // Get User Name
    xpr_tchar_t sUserName[0xff] = {0};
    DWORD dwUserNameLength = 0xfe;
    GetUserName(sUserName, &dwUserNameLength);

    SetDlgItemText(IDC_ABOUT_NAME, sUserName);

    // Get Version - Windows, Shell
    CString sWindows;
    sWindows.Format(XPR_STRING_LITERAL("Microsoft (R) Windows %d.%d build %d"), 
        LOWORD(sUserEnv.mOSVerInfo.dwMajorVersion),
        LOWORD(sUserEnv.mOSVerInfo.dwMinorVersion),
        LOWORD(sUserEnv.mOSVerInfo.dwBuildNumber));
    SetDlgItemText(IDC_ABOUT_WINDOWS, sWindows);

    // fxfile
    LOGFONT sLogFont = {0};
    mProgramWnd.GetFont()->GetLogFont(&sLogFont);
    sLogFont.lfWeight = FW_BOLD;
    mBoldFont.CreateFontIndirect(&sLogFont);
    mProgramWnd.SetFont(&mBoldFont);

    // Version
    xpr_tchar_t sVersion[0xff] = {0};
    getFullAppVer(sVersion);
    SetDlgItemText(IDC_ABOUT_VERSION, sVersion);

    // Get Version - Module, Dll
    xpr_sint_t i = 0;
    CListCtrl *sListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST);
    sListCtrl->SetExtendedStyle(sListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    sListCtrl->InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.about.module")),  LVCFMT_LEFT,  208);
    sListCtrl->InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.about.version")), LVCFMT_RIGHT, 100);

    xpr_tchar_t sGflVersion[0xff] = {0};
#ifdef XPR_CFG_UNICODE
    xpr_size_t sInputBytes = strlen(gflGetVersion());
    xpr_size_t sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
    XPR_MBS_TO_TCS(gflGetVersion(), &sInputBytes, sGflVersion, &sOutputBytes);
    sGflVersion[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
#else
    strcpy(sGflVersion, gflGetVersion());
#endif

    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("gfl SDK"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, sGflVersion, 0, 0, 0, 0);

    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("Windows Shell"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, sUserEnv.mShellVer.c_str(), 0, 0, 0, 0);

    xpr_tchar_t szText[0xff];
    DWORD sMajorVersion, sMinorVersion, sBuildNumber;
    UserEnv::getDllVersion(XPR_STRING_LITERAL("comctl32.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("comctl32.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    UserEnv::getDllVersion(XPR_STRING_LITERAL("shlwapi.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("shlwapi.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    UserEnv::getDllVersion(XPR_STRING_LITERAL("shell32.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("shell32.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.about.title")));
    SetDlgItemText(IDC_ABOUT_LABEL_ACCEPT, theApp.loadString(XPR_STRING_LITERAL("popup.about.label.accept")));
    SetDlgItemText(IDOK,                   theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDC_ABOUT_SYSTEM,       theApp.loadString(XPR_STRING_LITERAL("popup.about.button.system_info")));
    SetDlgItemText(IDC_ABOUT_BUG,          theApp.loadString(XPR_STRING_LITERAL("popup.about.button.bug_report")));
    SetDlgItemText(IDC_ABOUT_UPDATE,       theApp.loadString(XPR_STRING_LITERAL("popup.about.button.update")));

    // disable update button
    GetDlgItem(IDC_ABOUT_UPDATE)->ShowWindow(SW_HIDE);

    // start update check timer
    SetTimer(kTimerIdUpdateCheckFirst, 0, XPR_NULL);

    return XPR_TRUE;
}

void AboutDlg::OnLButtonDown(xpr_uint_t nFlags, CPoint point) 
{
    //PostMessage(WM_NCLBUTTONDOWN, HTCAPTION);
    super::OnLButtonDown(nFlags, point);
}

void AboutDlg::OnDestroy(void) 
{
    // stop update check timer
    KillTimer(kTimerIdUpdateCheckFirst);
    KillTimer(kTimerIdUpdateCheck);

    XPR_SAFE_DELETE(mUpdateInfoManager);

    super::OnDestroy();

    mBoldFont.DeleteObject();
}

void AboutDlg::OnTimer(xpr_uint_t aIdEvent)
{
    if (aIdEvent == kTimerIdUpdateCheckFirst ||
        aIdEvent == kTimerIdUpdateCheck)
    {
        if (aIdEvent == kTimerIdUpdateCheckFirst)
        {
            KillTimer(aIdEvent);
            SetTimer(kTimerIdUpdateCheck, 300, XPR_NULL);
        }

        xpr_rcode_t sRcode;
        xpr_tchar_t sStatus[0xff];
        xpr_tchar_t sCheckedVersion[0xff];
        UpdateInfo  sUpdateInfo = {0};

        if (XPR_IS_NULL(mUpdateInfoManager))
        {
            mUpdateInfoManager = new UpdateInfoManager;
            mUpdateInfoManager->setUpdateHomeDir(XPR_STRING_LITERAL("C:\\Users\\flychk\\AppData\\Roaming\\fxfile\\update"));

            sRcode = mUpdateInfoManager->openUpdateInfo();
            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                XPR_SAFE_DELETE(mUpdateInfoManager);
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
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, theApp.loadString(XPR_STRING_LITERAL("popup.about.label.update.checking")));
                    break;

                case kUpdateStatusCheckFailed:
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, theApp.loadString(XPR_STRING_LITERAL("popup.about.label.update.check_failed")));
                    break;

                case kUpdateStatusExistNewVer:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.label.update.checked"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;

                case kUpdateStatusLastestVer:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.label.update.latest_version"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);

                    KillTimer(aIdEvent);
                    break;

                case kUpdateStatusDownloading:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.label.update.downloading"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;

                case kUpdateStatusDownloaded:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.label.update.downloaded"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    GetDlgItem(IDC_ABOUT_UPDATE)->ShowWindow(SW_SHOW);

                    KillTimer(aIdEvent);
                    break;

                case kUpdateStatusDownloadFailed:
                    UpdateInfoManager::getCheckedVersion(sUpdateInfo, sCheckedVersion, XPR_COUNT_OF(sCheckedVersion) - 1);

                    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.label.update.download_failed"), XPR_STRING_LITERAL("%s")), sCheckedVersion);
                    SetDlgItemText(IDC_ABOUT_UPDATE_INFO, sStatus);
                    break;
                }
            }
        }
    }

    CWnd::OnTimer(aIdEvent);
}

void AboutDlg::OnBug(void) 
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
    _stprintf(sUrl, XPR_STRING_LITERAL("mailto:flychk@flychk.com?subject=fxfile v%s - bug report"), sVersion);
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sUrl, XPR_NULL, XPR_NULL, 0);
}

void AboutDlg::OnSystem(void)
{
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), XPR_STRING_LITERAL("msinfo32.exe"), XPR_NULL, XPR_NULL, 0);
}

void AboutDlg::OnUpdate(void)
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
                _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.about.msg.installer_execute_error"), XPR_STRING_LITERAL("%d")), sErrorCode);
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
