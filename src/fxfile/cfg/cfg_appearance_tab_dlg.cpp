//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_tab_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceTabDlg::CfgAppearanceTabDlg(void)
    : super(IDD_CFG_APPEARANCE_TAB, XPR_NULL)
{
}

void CfgAppearanceTabDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceTabDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceTabDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_TAB_SWITCH_TO_NEW_TAB,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.switch_to_new_tab")));
    SetDlgItemText(IDC_CFG_TAB_CLOSE_ON_DOUBLE_CLICK, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.close_on_double_click")));
    SetDlgItemText(IDC_CFG_TAB_AUTO_FIT,              gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.auto_fit")));
    SetDlgItemText(IDC_CFG_TAB_SHOW_ONE_TAB,          gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.show_one_tab")));
    SetDlgItemText(IDC_CFG_TAB_CONFIRM_TO_CLOSE,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.confirm_to_close")));
    SetDlgItemText(IDC_CFG_TAB_CONFIRM_TO_CLOSE_ALL,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.confirm_to_close_all")));
    SetDlgItemText(IDC_CFG_TAB_NEAR_OPEN_NEW_TAB,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.near_open_new_tab")));
    SetDlgItemText(IDC_CFG_TAB_ICON,                  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.icon")));
    SetDlgItemText(IDC_CFG_TAB_SHOW_DRIVE,            gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.show_drive")));
    SetDlgItemText(IDC_CFG_TAB_SHOW_NEW_BUTTON,       gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.show_new_button")));
    SetDlgItemText(IDC_CFG_TAB_DRAG_MOVE,             gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.tab.check.drag_move")));

    return XPR_TRUE;
}

void CfgAppearanceTabDlg::onInit(const Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_TAB_SWITCH_TO_NEW_TAB    ))->SetCheck(aConfig.mTabSwitchToNewTab);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_CLOSE_ON_DOUBLE_CLICK))->SetCheck(aConfig.mTabCloseOnDoubleClick);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_AUTO_FIT             ))->SetCheck(aConfig.mTabAutoFit);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_ONE_TAB         ))->SetCheck(aConfig.mTabShowOneTab);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_CONFIRM_TO_CLOSE     ))->SetCheck(aConfig.mTabConfirmToClose);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_CONFIRM_TO_CLOSE_ALL ))->SetCheck(aConfig.mTabConfirmToCloseAll);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_NEAR_OPEN_NEW_TAB    ))->SetCheck(aConfig.mTabNearOpenNewTab);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_ICON                 ))->SetCheck(aConfig.mTabIcon);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_DRIVE           ))->SetCheck(aConfig.mTabShowDrive);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_NEW_BUTTON      ))->SetCheck(aConfig.mTabShowNewButton);
    ((CButton *)GetDlgItem(IDC_CFG_TAB_DRAG_MOVE            ))->SetCheck(aConfig.mTabDragMove);
}

void CfgAppearanceTabDlg::onApply(Option::Config &aConfig)
{
    aConfig.mTabSwitchToNewTab     = ((CButton *)GetDlgItem(IDC_CFG_TAB_SWITCH_TO_NEW_TAB    ))->GetCheck();
    aConfig.mTabCloseOnDoubleClick = ((CButton *)GetDlgItem(IDC_CFG_TAB_CLOSE_ON_DOUBLE_CLICK))->GetCheck();
    aConfig.mTabAutoFit            = ((CButton *)GetDlgItem(IDC_CFG_TAB_AUTO_FIT             ))->GetCheck();
    aConfig.mTabShowOneTab         = ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_ONE_TAB         ))->GetCheck();
    aConfig.mTabConfirmToClose     = ((CButton *)GetDlgItem(IDC_CFG_TAB_CONFIRM_TO_CLOSE     ))->GetCheck();
    aConfig.mTabConfirmToCloseAll  = ((CButton *)GetDlgItem(IDC_CFG_TAB_CONFIRM_TO_CLOSE_ALL ))->GetCheck();
    aConfig.mTabNearOpenNewTab     = ((CButton *)GetDlgItem(IDC_CFG_TAB_NEAR_OPEN_NEW_TAB    ))->GetCheck();
    aConfig.mTabIcon               = ((CButton *)GetDlgItem(IDC_CFG_TAB_ICON                 ))->GetCheck();
    aConfig.mTabShowDrive          = ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_DRIVE           ))->GetCheck();
    aConfig.mTabShowNewButton      = ((CButton *)GetDlgItem(IDC_CFG_TAB_SHOW_NEW_BUTTON      ))->GetCheck();
    aConfig.mTabDragMove           = ((CButton *)GetDlgItem(IDC_CFG_TAB_DRAG_MOVE            ))->GetCheck();
}
} // namespace cfg
} // namespace fxfile
