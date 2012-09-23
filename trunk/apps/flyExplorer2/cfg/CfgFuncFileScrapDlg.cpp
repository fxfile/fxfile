//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncFileScrapDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncFileScrapDlg::CfgFuncFileScrapDlg(void)
    : super(IDD_CFG_FUNC_FILE_SCRAP, XPR_NULL)
{
}

void CfgFuncFileScrapDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncFileScrapDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncFileScrapDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_SCRAP_SHOW_POPUP))->SetCheck(gOpt->mFileScrapContextMenu);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_SCRAP_SAVE_SCRAP))->SetCheck(gOpt->mFileScrapSave);

    SetDlgItemText(IDC_CFG_FUNC_FILE_SCRAP_SHOW_POPUP, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.scrap.show_on_shell_popup_menu")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_SCRAP_SAVE_SCRAP, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.scrap.save_file_scrap")));

    return XPR_TRUE;
}

xpr_bool_t CfgFuncFileScrapDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_FILE_SCRAP_SHOW_POPUP:
        case IDC_CFG_FUNC_FILE_SCRAP_SAVE_SCRAP:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
    }

    return super::OnCommand(wParam, lParam);
}

void CfgFuncFileScrapDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mFileScrapContextMenu = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_SCRAP_SHOW_POPUP))->GetCheck();
    gOpt->mFileScrapSave        = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_SCRAP_SAVE_SCRAP))->GetCheck();

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}
