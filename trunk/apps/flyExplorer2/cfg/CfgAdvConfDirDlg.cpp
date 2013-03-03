//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvConfDirDlg.h"

#include "../resource.h"
#include "../ConfDir.h"

#include "SelFolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAdvConfDirDlg::CfgAdvConfDirDlg(void)
    : super(IDD_CFG_ADV_CONF_DIR, XPR_NULL)
{
}

void CfgAdvConfDirDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAdvConfDirDlg, super)
    ON_BN_CLICKED(IDC_CFG_CONF_DIR_CUSTOM_PATH_BROWSE, OnCustomPathBrowse)
    ON_BN_CLICKED(IDC_CFG_CONF_DIR_PROGRAM, OnConfDir)
    ON_BN_CLICKED(IDC_CFG_CONF_DIR_APPDATA, OnConfDir)
    ON_BN_CLICKED(IDC_CFG_CONF_DIR_CUSTOM,  OnConfDir)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvConfDirDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_CONF_DIR_CUSTOM_PATH_BROWSE);

    ((CEdit *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM_PATH))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_CFG_CONF_DIR_PROGRAM, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.conf_dir.radio.program")));
    SetDlgItemText(IDC_CFG_CONF_DIR_APPDATA, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.conf_dir.radio.appdata")));
    SetDlgItemText(IDC_CFG_CONF_DIR_CUSTOM,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.conf_dir.radio.custom")));

    return XPR_TRUE;
}

void CfgAdvConfDirDlg::onInit(Option::Config &aConfig)
{
    const xpr_tchar_t *sConfDir = ConfDir::instance().getConfDir();

    loadConfDir(sConfDir);
}

void CfgAdvConfDirDlg::loadConfDir(const xpr_tchar_t *aConfDir)
{
    ConfDir &sConfDir = ConfDir::instance();

    const xpr_tchar_t *sProgramConfDir;
    const xpr_tchar_t *sDefConfDir;

    sProgramConfDir = ConfDir::getProgramConfDir();
    sDefConfDir     = ConfDir::getDefConfDir();

    XPR_ASSERT(aConfDir        != XPR_NULL);
    XPR_ASSERT(sProgramConfDir != XPR_NULL);
    XPR_ASSERT(sDefConfDir     != XPR_NULL);

    if (_tcsicmp(aConfDir, sProgramConfDir) == 0)
    {
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_PROGRAM))->SetCheck(XPR_TRUE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_APPDATA))->SetCheck(XPR_FALSE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM ))->SetCheck(XPR_FALSE);
    }
    else if (_tcsicmp(aConfDir, sDefConfDir) == 0)
    {
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_PROGRAM))->SetCheck(XPR_FALSE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_APPDATA))->SetCheck(XPR_TRUE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM ))->SetCheck(XPR_FALSE);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_PROGRAM))->SetCheck(XPR_FALSE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_APPDATA))->SetCheck(XPR_FALSE);
        ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM ))->SetCheck(XPR_TRUE);

        SetDlgItemText(IDC_CFG_CONF_DIR_CUSTOM_PATH, aConfDir);
    }

    OnConfDir();
}

void CfgAdvConfDirDlg::onApply(Option::Config &aConfig)
{
    ConfDir &sConfDir = ConfDir::instance();

    sConfDir.setBackup();

    if (((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_PROGRAM))->GetCheck())
    {
        const xpr_tchar_t *sProgramConfDir = ConfDir::getProgramConfDir();

        sConfDir.setConfDir(sProgramConfDir);
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_APPDATA))->GetCheck())
    {
        const xpr_tchar_t *sDefConfDir = ConfDir::getDefConfDir();

        sConfDir.setConfDir(sDefConfDir);
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM))->GetCheck())
    {
        xpr_tchar_t sCustomConfDir[XPR_MAX_PATH + 1] = {0};
        GetDlgItemText(IDC_CFG_CONF_DIR_CUSTOM_PATH, sCustomConfDir, XPR_MAX_PATH);

        sConfDir.setConfDir(sCustomConfDir);
    }

    if (sConfDir.checkChangedConfDir() == XPR_TRUE)
    {
        if (sConfDir.moveToNewConfDir() == XPR_FALSE)
        {
            const xpr_tchar_t *sOldConfDir = sConfDir.getOldConfDir();

            sConfDir.setConfDir(sOldConfDir);

            loadConfDir(sOldConfDir);

            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.conf_dir.msg.cannot_set"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        }
    }
}

xpr_bool_t CfgAdvConfDirDlg::PreTranslateMessage(MSG* pMsg)
{
    return CDialog::PreTranslateMessage(pMsg);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgAdvConfDirDlg::OnCustomPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_CONF_DIR_CUSTOM_PATH, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.conf_dir.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
        {
            SetDlgItemText(IDC_CFG_CONF_DIR_CUSTOM_PATH, sNewPath);

            setModified();
        }
    }
}

void CfgAdvConfDirDlg::OnConfDir(void)
{
    xpr_bool_t sCustom = ((CButton *)GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM ))->GetCheck();

    GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM_PATH       )->EnableWindow(sCustom);
    GetDlgItem(IDC_CFG_CONF_DIR_CUSTOM_PATH_BROWSE)->EnableWindow(sCustom);
}
