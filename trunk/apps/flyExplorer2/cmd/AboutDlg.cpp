//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "AboutDlg.h"

#include "AppVer.h"
#include "Option.h"
#include "resource.h"

#include "gfl/libgfl.h"

#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AboutDlg::AboutDlg(void)
    : super(IDD_ABOUT)
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FLYEXPLORER, mProgramWnd);
}

BEGIN_MESSAGE_MAP(AboutDlg, super)
    ON_WM_LBUTTONDOWN()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_ABOUT_BUG,    OnBug)
    ON_BN_CLICKED(IDC_ABOUT_SYSTEM, OnSystem)
END_MESSAGE_MAP()

xpr_bool_t AboutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    fxb::UserEnv &sUserEnv = fxb::UserEnv::instance();

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

    // flyExplorer
    LOGFONT sLogFont = {0};
    mProgramWnd.GetFont()->GetLogFont(&sLogFont);
    sLogFont.lfWeight = FW_BOLD;
    mBoldFont.CreateFontIndirect(&sLogFont);
    mProgramWnd.SetFont(&mBoldFont);

    // Version
    xpr_tchar_t sVersion[0xff] = {0};
    _stprintf(sVersion, XPR_STRING_LITERAL("version %d.%d.%d %s"), MAJOR_VER, MINOR_VER, PATCH_VER,
#ifdef XPR_CFG_UNICODE
        XPR_STRING_LITERAL("Unicode")
#else
        XPR_STRING_LITERAL("Ansicode")
#endif
        );
    _tcscat(sVersion, XPR_STRING_LITERAL(", alpha3"));
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
    fxb::UserEnv::getDllVersion(XPR_STRING_LITERAL("comctl32.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("comctl32.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    fxb::UserEnv::getDllVersion(XPR_STRING_LITERAL("shlwapi.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("shlwapi.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    fxb::UserEnv::getDllVersion(XPR_STRING_LITERAL("shell32.dll"), &sMajorVersion, &sMinorVersion, &sBuildNumber);
    _stprintf(szText, XPR_STRING_LITERAL("%d.%d.%d"), sMajorVersion, sMinorVersion, sBuildNumber);
    sListCtrl->InsertItem(i, XPR_STRING_LITERAL("shell32.dll"));
    sListCtrl->SetItem(i++, 1, LVIF_TEXT, szText, 0, 0, 0, 0);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.about.title")));
    SetDlgItemText(IDC_ABOUT_LABEL_ACCEPT, theApp.loadString(XPR_STRING_LITERAL("popup.about.label.accept")));
    SetDlgItemText(IDOK,                   theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDC_ABOUT_SYSTEM,       theApp.loadString(XPR_STRING_LITERAL("popup.about.button.system_info")));
    SetDlgItemText(IDC_ABOUT_BUG,          theApp.loadString(XPR_STRING_LITERAL("popup.about.button.bug_report")));

    return XPR_TRUE;
}

void AboutDlg::OnLButtonDown(xpr_uint_t nFlags, CPoint point) 
{
    //PostMessage(WM_NCLBUTTONDOWN, HTCAPTION);
    super::OnLButtonDown(nFlags, point);
}

void AboutDlg::OnDestroy(void) 
{
    super::OnDestroy();
    mBoldFont.DeleteObject();
}

void AboutDlg::OnBug(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetModulePath(sPath, XPR_MAX_PATH);

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
    _stprintf(sUrl, XPR_STRING_LITERAL("mailto:flychk@flychk.com?subject=flyExplorer v%s - bug report"), sVersion);
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sUrl, XPR_NULL, XPR_NULL, 0);
}

void AboutDlg::OnSystem(void)
{
    ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), XPR_STRING_LITERAL("msinfo32.exe"), XPR_NULL, XPR_NULL, 0);
}
