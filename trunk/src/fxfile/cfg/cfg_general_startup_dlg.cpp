//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_general_startup_dlg.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgGeneralStartupDlg::CfgGeneralStartupDlg(void)
    : super(IDD_CFG_GENERAL_STARTUP, XPR_NULL)
    , mOldViewIndex(-1)
{
}

void CfgGeneralStartupDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgGeneralStartupDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_STARTUP_APPLY_ALL,          OnApplyAll)
    ON_BN_CLICKED(IDC_CFG_STARTUP_VIEW_FOLDER_BROWSE, OnFolderBrowse)
    ON_CBN_SELCHANGE(IDC_CFG_STARTUP_VIEW, OnSelChangeView)
END_MESSAGE_MAP()

xpr_bool_t CfgGeneralStartupDlg::OnInitDialog(void)
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_STARTUP_VIEW_FOLDER_BROWSE);

    ((CEdit *)GetDlgItem(IDC_CFG_STARTUP_VIEW_FOLDER))->LimitText(XPR_MAX_PATH);

    ::SHAutoComplete(GetDlgItem(IDC_CFG_STARTUP_VIEW_FOLDER)->m_hWnd, SHACF_FILESYSTEM);

    CComboBox *sStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP);
    sStartupComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.none")));
    sStartupComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.folder")));
    sStartupComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.last_folder")));
    sStartupComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.startup.last_tab")));

    SetDlgItemText(IDC_CFG_STARTUP_APPLY_ALL,                   gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.button.apply_all")));
    SetDlgItemText(IDC_CFG_STARTUP_VIEW_LABEL_STARTUP,          gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.label.startup")));
    SetDlgItemText(IDC_CFG_STARTUP_VIEW_LABEL_FOLDER,           gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.label.folder")));
    SetDlgItemText(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.check.no_last_network_folder")));

    return XPR_TRUE;
}

void CfgGeneralStartupDlg::OnDestroy(void)
{
    CComboBox *sViewStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sViewStartupComboBox != XPR_NULL);

    xpr_sint_t i, sCount;
    ViewStartup *sViewStartup;

    sCount = sViewStartupComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sViewStartup = (ViewStartup *)sViewStartupComboBox->GetItemData(i);
        XPR_SAFE_DELETE(sViewStartup);
    }

    sViewStartupComboBox->ResetContent();

    super::OnDestroy();
}

void CfgGeneralStartupDlg::onInit(Option::Config &aConfig)
{
    CComboBox *sViewStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sViewStartupComboBox != XPR_NULL);

    xpr_sint_t i;
    xpr::tstring sStringId;
    xpr::tstring sViewText;
    ViewStartup *pViewStartup;

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        pViewStartup = new ViewStartup;
        pViewStartup->mExplorerInitFolderType  = aConfig.mExplorerInitFolderType[i];
        pViewStartup->mExplorerNoNetLastFolder = aConfig.mExplorerNoNetLastFolder[i];
        pViewStartup->mExplorerInitFolder      = aConfig.mExplorerInitFolder[i];

        sStringId.format(XPR_STRING_LITERAL("popup.cfg.body.general.startup.combo.view%d"), i + 1);

        sViewStartupComboBox->AddString(gApp.loadString(sStringId.c_str()));
        sViewStartupComboBox->SetItemData(i, (DWORD_PTR)pViewStartup);
    }

    sViewStartupComboBox->SetCurSel(0);
    OnSelChangeView();
}

void CfgGeneralStartupDlg::onApply(Option::Config &aConfig)
{
    saveViewStartup();

    xpr_sint_t i, sCount;
    ViewStartup *sViewStartup;
    CComboBox *sViewStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sViewStartupComboBox != XPR_NULL);

    sCount = sViewStartupComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sViewStartup = (ViewStartup *)sViewStartupComboBox->GetItemData(i);

        XPR_ASSERT(sViewStartup != XPR_NULL);

        aConfig.mExplorerInitFolderType[i]  = sViewStartup->mExplorerInitFolderType;
        aConfig.mExplorerNoNetLastFolder[i] = sViewStartup->mExplorerNoNetLastFolder;
        _tcscpy(aConfig.mExplorerInitFolder[i], sViewStartup->mExplorerInitFolder.c_str());
    }
}

void CfgGeneralStartupDlg::OnApplyAll(void)
{
    saveViewStartup();

    CComboBox *sViewStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sViewStartupComboBox != XPR_NULL);

    xpr_sint_t i, sCount;
    xpr_sint_t sCurSel;
    ViewStartup *sCurViewStartup;
    ViewStartup *sViewStartup;

    sCurSel = sViewStartupComboBox->GetCurSel();
    sCurViewStartup = (ViewStartup *)sViewStartupComboBox->GetItemData(sCurSel);

    XPR_ASSERT(sCurViewStartup != XPR_NULL);

    sCount = sViewStartupComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sCurSel)
        {
            continue;
        }

        sViewStartup = (ViewStartup *)sViewStartupComboBox->GetItemData(i);

        XPR_ASSERT(sViewStartup != XPR_NULL);

        *sViewStartup = *sCurViewStartup;
    }
}

void CfgGeneralStartupDlg::OnSelChangeView(void)
{
    CComboBox *sColorViewComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sColorViewComboBox != XPR_NULL);

    if (mOldViewIndex != -1)
    {
        ViewStartup *sOldViewStartup = (ViewStartup *)sColorViewComboBox->GetItemData(mOldViewIndex);

        XPR_ASSERT(sOldViewStartup != XPR_NULL);

        saveViewStartup(*sOldViewStartup);
    }

    xpr_sint_t sViewIndex = sColorViewComboBox->GetCurSel();
    ViewStartup *sViewStartup = (ViewStartup *)sColorViewComboBox->GetItemData(sViewIndex);

    XPR_ASSERT(sViewStartup != XPR_NULL);

    if (mOldViewIndex != sViewIndex)
    {
        loadViewStartup(*sViewStartup);

        mOldViewIndex = sViewIndex;
    }
}

void CfgGeneralStartupDlg::loadViewStartup(const ViewStartup &aViewStartup)
{
    xpr_sint_t sCurSel = 2;
    switch (aViewStartup.mExplorerInitFolderType)
    {
    case INIT_TYPE_NONE:        sCurSel = 0; break;
    case INIT_TYPE_INIT_FOLDER: sCurSel = 1; break;
    case INIT_TYPE_LAST_TAB:    sCurSel = 3; break;
    default:                    sCurSel = 2; break;
    }
    ((CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP))->SetCurSel(sCurSel);

    ((CButton *)GetDlgItem(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER))->SetCheck(aViewStartup.mExplorerNoNetLastFolder);

    SetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, aViewStartup.mExplorerInitFolder.c_str());
}

void CfgGeneralStartupDlg::saveViewStartup(ViewStartup &aViewStartup)
{
    CComboBox *sStartupComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW_STARTUP);

    xpr_sint_t sCurSel = sStartupComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 0:  aViewStartup.mExplorerInitFolderType = INIT_TYPE_NONE;        break;
    case 1:  aViewStartup.mExplorerInitFolderType = INIT_TYPE_INIT_FOLDER; break;
    case 3:  aViewStartup.mExplorerInitFolderType = INIT_TYPE_LAST_TAB;    break;
    default: aViewStartup.mExplorerInitFolderType = INIT_TYPE_LAST_FOLDER; break;
    }
    sStartupComboBox->SetCurSel(sCurSel);

    aViewStartup.mExplorerNoNetLastFolder = ((CButton *)GetDlgItem(IDC_CFG_STARTUP_VIEW_NO_LAST_NETWORK_FOLDER))->GetCheck();

    xpr_tchar_t sExplorerInitFolder[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, sExplorerInitFolder, XPR_MAX_PATH);
    aViewStartup.mExplorerInitFolder = sExplorerInitFolder;
}

void CfgGeneralStartupDlg::saveViewStartup(void)
{
    CComboBox *sColorViewComboBox = (CComboBox *)GetDlgItem(IDC_CFG_STARTUP_VIEW);

    XPR_ASSERT(sColorViewComboBox != XPR_NULL);

    xpr_sint_t sViewIndex = sColorViewComboBox->GetCurSel();
    ViewStartup *sViewStartup = (ViewStartup *)sColorViewComboBox->GetItemData(sViewIndex);

    XPR_ASSERT(sViewStartup != XPR_NULL);

    saveViewStartup(*sViewStartup);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgGeneralStartupDlg::OnFolderBrowse(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_STARTUP_VIEW_FOLDER, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.startup.view.msg.folder_browse.title"));
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
