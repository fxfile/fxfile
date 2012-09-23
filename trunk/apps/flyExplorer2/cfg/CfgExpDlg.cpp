//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpDlg.h"

#include "../rgc/FileDialogST.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgExpDlg::CfgExpDlg(void)
    : super(IDD_CFG_EXP, XPR_NULL)
{
}

void CfgExpDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgExpDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgExpDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_EXP_UP_AND_SEL_ITEM))->SetCheck(gOpt->mExplorerGoUpSelSubFolder);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_EDIT_AFTER_CREATING_TEXT))->SetCheck(gOpt->mExplorerCreateAndEditText);

    SetDlgItemText(IDC_CFG_EXP_UP_AND_SEL_ITEM,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.check.go_up_and_select_child_item")));
    SetDlgItemText(IDC_CFG_EXP_EDIT_AFTER_CREATING_TEXT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.check.edit_after_creating_text_file")));

    return XPR_TRUE;
}

void CfgExpDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mExplorerGoUpSelSubFolder  = ((CButton *)GetDlgItem(IDC_CFG_EXP_UP_AND_SEL_ITEM))->GetCheck();
    gOpt->mExplorerCreateAndEditText = ((CButton *)GetDlgItem(IDC_CFG_EXP_EDIT_AFTER_CREATING_TEXT))->GetCheck();

    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgExpDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_UP_AND_SEL_ITEM:
        case IDC_CFG_EXP_EDIT_AFTER_CREATING_TEXT:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
