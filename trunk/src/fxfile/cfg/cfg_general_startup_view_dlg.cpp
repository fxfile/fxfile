//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_general_startup_view_dlg.h"

#include "../shell.h"
#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgGeneralStartupViewDlg::CfgGeneralStartupViewDlg(xpr_sint_t aViewIndex)
    : super(IDD_CFG_GENERAL_STARTUP_VIEW, XPR_NULL)
    , mViewIndex(aViewIndex)
{
}

void CfgGeneralStartupViewDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgGeneralStartupViewDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_STARTUP_VIEW_FOLDER_BROWSE, OnFolderBrowse)
END_MESSAGE_MAP()

xpr_bool_t CfgGeneralStartupViewDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_STARTUP_VIEW_FOLDER_BROWSE);

    ((CEdit *)GetDlgItem(IDC_CFG_STARTUP_VIEW_FOLDER))->LimitText(XPR_MAX_PATH);

    ::SHAutoComplete(GetDlgItem(IDC_CFG_STARTUP_VIEW_FOLDER)->m_hWnd, SHACF_FILESYSTEM);

    CComboBox *sStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP);
    sStartupComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.none")));
    sStartupComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.folder")));
    sStartupComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.last_folder")));
    sStartupComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.last_tab")));

    SetDlgItemText(IDC_CFG_STARTUP_VIEW_LABEL_STARTUP,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.label.startup")));
    SetDlgItemText(IDC_CFG_STARTUP_VIEW_LABEL_FOLDER,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.label.folder")));
    SetDlgItemText(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.check.no_last_network_folder")));

    return XPR_TRUE;
}

void CfgGeneralStartupViewDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgGeneralStartupViewDlg::onInit(Option::Config &aConfig)
{
    xpr_sint_t sCurSel = 2;
    switch (aConfig.mExplorerInitFolderType[mViewIndex])
    {
    case INIT_TYPE_NONE:        sCurSel = 0; break;
    case INIT_TYPE_INIT_FOLDER: sCurSel = 1; break;
    case INIT_TYPE_LAST_TAB:    sCurSel = 3; break;
    default:                    sCurSel = 2; break;
    }
    ((CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP))->SetCurSel(sCurSel);

    ((CButton *)GetDlgItem(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER))->SetCheck(aConfig.mExplorerNoNetLastFolder[mViewIndex]);

    SetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, aConfig.mExplorerInitFolder[mViewIndex]);
}

void CfgGeneralStartupViewDlg::onApply(Option::Config &aConfig)
{
    CComboBox *sStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP);

    xpr_sint_t sCurSel = sStartupComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 0:  aConfig.mExplorerInitFolderType[mViewIndex] = INIT_TYPE_NONE;        break;
    case 1:  aConfig.mExplorerInitFolderType[mViewIndex] = INIT_TYPE_INIT_FOLDER; break;
    case 3:  aConfig.mExplorerInitFolderType[mViewIndex] = INIT_TYPE_LAST_TAB;    break;
    default: aConfig.mExplorerInitFolderType[mViewIndex] = INIT_TYPE_LAST_FOLDER; break;
    }
    sStartupComboBox->SetCurSel(sCurSel);

    aConfig.mExplorerNoNetLastFolder[mViewIndex] = ((CButton *)GetDlgItem(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER))->GetCheck();

    GetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, aConfig.mExplorerInitFolder[mViewIndex], XPR_MAX_PATH);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgGeneralStartupViewDlg::OnFolderBrowse(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.msg.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_tchar_t szNewPath[XPR_MAX_PATH + 1] = {0};
        GetName(sFullPidl, SHGDN_FORPARSING, szNewPath);

        if (_tcsicmp(sOldPath, szNewPath) != 0)
        {
            sPath[0] = '\0';
            Pidl2Path(sFullPidl, sPath);
            SetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, sPath);

            setModified();
        }
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}
} // namespace cfg
} // namespace fxfile
