//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvPathDlg.h"

#include "../resource.h"
#include "../CfgPath.h"

#include "SelFolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAdvPathDlg::CfgAdvPathDlg(void)
    : super(IDD_CFG_ADV_PATH, XPR_NULL)
{
}

void CfgAdvPathDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAdvPathDlg, super)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE, OnCustomPathBrowse)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_PROGRAM, OnCfgPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_APPDATA, OnCfgPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_CUSTOM,  OnCfgPath)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvPathDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE);

    ((CEdit *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_CFG_CFG_PATH_PROGRAM, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.program")));
    SetDlgItemText(IDC_CFG_CFG_PATH_APPDATA, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.appdata")));
    SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.custom")));

    return XPR_TRUE;
}

void CfgAdvPathDlg::onInit(Option::Config &aConfig)
{
    CfgPath &sCfgPath = CfgPath::instance();

    const xpr_tchar_t *sDir;
    const xpr_tchar_t *sDefDir;

    sDir = sCfgPath.getRootDir();
    XPR_ASSERT(sDir != XPR_NULL);

    sDefDir = CfgPath::getDefRootDir();
    XPR_ASSERT(sDefDir != XPR_NULL);

    if (_tcsicmp(sDir, XPR_STRING_LITERAL("%flyExplorer%")) == 0)
    {
        ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM))->SetCheck(XPR_TRUE);
    }
    else if (_tcsicmp(sDir, sDefDir) == 0)
    {
        ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA))->SetCheck(XPR_TRUE);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM))->SetCheck(XPR_TRUE);
        SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sDir);
    }

    OnCfgPath();
}

void CfgAdvPathDlg::onApply(Option::Config &aConfig)
{
    CfgPath &sCfgPath = CfgPath::instance();

    sCfgPath.setBackup(XPR_TRUE);

    if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM))->GetCheck())
    {
        sCfgPath.setRootDir(XPR_STRING_LITERAL("%flyExplorer%"));
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA))->GetCheck())
    {
        const xpr_tchar_t *sDefDir = CfgPath::getDefRootDir();

        sCfgPath.setRootDir(sDefDir);
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM))->GetCheck())
    {
        xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
        GetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sDir, XPR_MAX_PATH);

        sCfgPath.setRootDir(sDir);
    }

    sCfgPath.setBackup(XPR_FALSE);

    if (sCfgPath.checkChangedCfgPath() == XPR_TRUE)
    {
        sCfgPath.moveToNewCfgPath();
    }
}

xpr_bool_t CfgAdvPathDlg::PreTranslateMessage(MSG* pMsg)
{
    return CDialog::PreTranslateMessage(pMsg);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgAdvPathDlg::OnCustomPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
        {
            SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sNewPath);

            setModified();
        }
    }
}

void CfgAdvPathDlg::OnCfgPath(void)
{
    xpr_bool_t sProgram = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM))->GetCheck();
    xpr_bool_t sAppData = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA))->GetCheck();
    xpr_bool_t sCustom  = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM ))->GetCheck();

    GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH       )->EnableWindow(sCustom);
    GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE)->EnableWindow(sCustom);
}
