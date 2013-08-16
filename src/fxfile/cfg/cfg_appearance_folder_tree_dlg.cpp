//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_folder_tree_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceFolderTreeDlg::CfgAppearanceFolderTreeDlg(void)
    : super(IDD_CFG_FUNC_FOLDER_TREE, XPR_NULL)
{
}

void CfgAppearanceFolderTreeDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceFolderTreeDlg, super)
    ON_BN_CLICKED(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY, OnExploreDelay)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceFolderTreeDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CSpinButtonCtrl *sSpinCtrl;

    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_FLD_SEL_DELAY_MSEC, MAX_FLD_SEL_DELAY_MSEC);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText,
        gApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_tree.label.explore_delay_time_range"), XPR_STRING_LITERAL("%d,%d,%d")),
        MIN_FLD_SEL_DELAY_MSEC, MAX_FLD_SEL_DELAY_MSEC, DEF_FLD_SEL_DELAY_MSEC);
    SetDlgItemText(IDC_CFG_FOLDER_TREE_LABEL_EXPLORE_DELAY_TIME_RANGE, sText);

    SetDlgItemText(IDC_CFG_FOLDER_TREE_INIT_NO_EXPAND,             gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_tree.check.init_no_expand")));
    SetDlgItemText(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY,              gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_tree.check.explore_delay")));
    SetDlgItemText(IDC_CFG_FOLDER_TREE_SINGLE_FOLDER_PANE_LINKAGE, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_tree.check.single_folder_pane_linkage")));

    return XPR_TRUE;
}

void CfgAppearanceFolderTreeDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_INIT_NO_EXPAND            ))->SetCheck(aConfig.mFolderTreeInitNoExpand);
    ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_SINGLE_FOLDER_PANE_LINKAGE))->SetCheck(aConfig.mSingleFolderTreeLinkage);

    ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY))->SetCheck(aConfig.mFolderTreeSelDelay);
    SetDlgItemInt(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY_TIME, aConfig.mFolderTreeSelDelayTime);

    OnExploreDelay();
}

void CfgAppearanceFolderTreeDlg::onApply(Option::Config &aConfig)
{
    aConfig.mFolderTreeInitNoExpand  = ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_INIT_NO_EXPAND))->GetCheck();
    aConfig.mFolderTreeSelDelay      = ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY ))->GetCheck();
    aConfig.mSingleFolderTreeLinkage = ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_SINGLE_FOLDER_PANE_LINKAGE))->GetCheck();
    aConfig.mFolderTreeSelDelayTime  = GetDlgItemInt(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY_TIME);

    if (aConfig.mFolderTreeSelDelayTime < MIN_FLD_SEL_DELAY_MSEC) aConfig.mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;
    if (aConfig.mFolderTreeSelDelayTime > MAX_FLD_SEL_DELAY_MSEC) aConfig.mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;
}

void CfgAppearanceFolderTreeDlg::OnExploreDelay(void)
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY))->GetCheck();
    GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY_TIME)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_FOLDER_TREE_EXPLORE_DELAY_TIME_SPIN)->EnableWindow(sEnable);
}
} // namespace cfg
} // namespace fxfile
