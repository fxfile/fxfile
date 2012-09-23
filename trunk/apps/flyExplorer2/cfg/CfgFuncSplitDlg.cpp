//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncSplitDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncSplitDlg::CfgFuncSplitDlg(void)
    : super(IDD_CFG_FUNC_SPLIT, XPR_NULL)
{
}

void CfgFuncSplitDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncSplitDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncSplitDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_FOCUS_MOVE_BY_TAB                            ))->SetCheck(gOpt->mSplitViewTabKey);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_RESIZE_BY_RATIO                              ))->SetCheck(gOpt->mViewSplitByRatio);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_SINGLE_SPLIT_VIEW_CHANGE_THROUGH_ACTIVED_VIEW))->SetCheck(gOpt->mSingleViewActivatePath);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_SPLIT_LAST_FOLDER                            ))->SetCheck(gOpt->mSplitLastFolder);

    SetDlgItemText(IDC_CFG_FUNC_SPLIT_FOCUS_MOVE_BY_TAB,                             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.split_view.check.focus_move_by_TAB_key")));
    SetDlgItemText(IDC_CFG_FUNC_SPLIT_RESIZE_BY_RATIO,                               theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.split_view.check.resize_by_ratio")));
    SetDlgItemText(IDC_CFG_FUNC_SPLIT_SINGLE_SPLIT_VIEW_CHANGE_THROUGH_ACTIVED_VIEW, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.split_view.check.single_split_view_change_through_actived_view")));
    SetDlgItemText(IDC_CFG_FUNC_SPLIT_SPLIT_LAST_FOLDER,                             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.split_view.check.last_folder_when_re-split_view")));

    return XPR_TRUE;
}

xpr_bool_t CfgFuncSplitDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_SPLIT_FOCUS_MOVE_BY_TAB:
        case IDC_CFG_FUNC_SPLIT_RESIZE_BY_RATIO:
        case IDC_CFG_FUNC_SPLIT_SINGLE_SPLIT_VIEW_CHANGE_THROUGH_ACTIVED_VIEW:
        case IDC_CFG_FUNC_SPLIT_SPLIT_LAST_FOLDER:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
    }

    return super::OnCommand(wParam, lParam);
}

void CfgFuncSplitDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mSplitViewTabKey        = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_FOCUS_MOVE_BY_TAB))->GetCheck();
    gOpt->mViewSplitByRatio       = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_RESIZE_BY_RATIO))->GetCheck();
    gOpt->mSingleViewActivatePath = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_SINGLE_SPLIT_VIEW_CHANGE_THROUGH_ACTIVED_VIEW))->GetCheck();
    gOpt->mSplitLastFolder        = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SPLIT_SPLIT_LAST_FOLDER))->GetCheck();

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}
