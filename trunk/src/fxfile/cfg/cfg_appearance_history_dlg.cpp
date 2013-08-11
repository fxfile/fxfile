//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_history_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceHistoryDlg::CfgAppearanceHistoryDlg(void)
    : super(IDD_CFG_FUNC_HISTORY, XPR_NULL)
{
}

void CfgAppearanceHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceHistoryDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceHistoryDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.history_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_HISTORY, MAX_HISTORY, DEF_HISTORY);
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_HISTORY_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.backward_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_BACKWARD, MAX_BACKWARD, DEF_BACKWARD);
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_BACKWARD_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.history_menu_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_HISTORY_MENU, MAX_HISTORY_MENU, DEF_HISTORY_MENU);
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_HISTORY_MENU_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.backward_menu_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_BACKWARD_MENU, MAX_BACKWARD_MENU, DEF_BACKWARD_MENU);
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_BACKWARD_MENU_COUNT_RANGE, sText);

    SetDlgItemText(IDC_CFG_HISTORY_SAVE_HISTROY,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.check.save_history")));
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_HISTORY_COUNT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.history_count")));
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_BACKWARD_COUNT,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.backward_count")));
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_HISTORY_MENU_COUNT,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.history_menu_count")));
    SetDlgItemText(IDC_CFG_HISTORY_LABEL_BACKWARD_MENU_COUNT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.history.label.backward_menu_count")));

    return XPR_TRUE;
}

void CfgAppearanceHistoryDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_HISTORY_SAVE_HISTROY))->SetCheck(aConfig.mSaveHistory);

    SetDlgItemInt(IDC_CFG_HISTORY_HISTORY_COUNT,       aConfig.mHistoryCount);
    SetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_COUNT,      aConfig.mBackwardCount);
    SetDlgItemInt(IDC_CFG_HISTORY_HISTORY_MENU_COUNT,  aConfig.mHistoryMenuCount);
    SetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_MENU_COUNT, aConfig.mBackwardMenuCount);
}

void CfgAppearanceHistoryDlg::onApply(Option::Config &aConfig)
{
    aConfig.mSaveHistory       = ((CButton *)GetDlgItem(IDC_CFG_HISTORY_SAVE_HISTROY))->GetCheck();
    aConfig.mHistoryCount      = GetDlgItemInt(IDC_CFG_HISTORY_HISTORY_COUNT);
    aConfig.mBackwardCount     = GetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_COUNT);
    aConfig.mHistoryMenuCount  = GetDlgItemInt(IDC_CFG_HISTORY_HISTORY_MENU_COUNT);
    aConfig.mBackwardMenuCount = GetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_MENU_COUNT);

    if (aConfig.mHistoryCount < MIN_HISTORY) aConfig.mHistoryCount = MIN_HISTORY;
    if (aConfig.mHistoryCount > MAX_HISTORY) aConfig.mHistoryCount = MAX_HISTORY;
    SetDlgItemInt(IDC_CFG_HISTORY_HISTORY_COUNT, aConfig.mHistoryCount);

    if (aConfig.mBackwardCount < MIN_BACKWARD) aConfig.mBackwardCount = MIN_BACKWARD;
    if (aConfig.mBackwardCount > MAX_BACKWARD) aConfig.mBackwardCount = MAX_BACKWARD;
    SetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_COUNT, aConfig.mBackwardCount);

    if (aConfig.mHistoryMenuCount < MIN_HISTORY_MENU) aConfig.mHistoryMenuCount = MIN_HISTORY_MENU;
    if (aConfig.mHistoryMenuCount > MAX_HISTORY_MENU) aConfig.mHistoryMenuCount = MAX_HISTORY_MENU;
    SetDlgItemInt(IDC_CFG_HISTORY_HISTORY_MENU_COUNT, aConfig.mHistoryMenuCount);

    if (aConfig.mBackwardMenuCount < MIN_BACKWARD_MENU) aConfig.mBackwardMenuCount = MIN_BACKWARD_MENU;
    if (aConfig.mBackwardMenuCount > MAX_BACKWARD_MENU) aConfig.mBackwardMenuCount = MAX_BACKWARD_MENU;
    SetDlgItemInt(IDC_CFG_HISTORY_BACKWARD_MENU_COUNT, aConfig.mBackwardMenuCount);
}
} // namespace cfg
} // namespace fxfile
