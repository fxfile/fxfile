//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncHistoryDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncHistoryDlg::CfgFuncHistoryDlg(void)
    : super(IDD_CFG_FUNC_HISTORY, XPR_NULL)
{
}

void CfgFuncHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncHistoryDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncHistoryDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_HISTORY_SAVE_HISTROY))->SetCheck(gOpt->mSaveHistory);

    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_COUNT,       gOpt->mHistoryCount);
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_COUNT,      gOpt->mBackwardCount);
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_MENU_COUNT,  gOpt->mHistoryMenuCount);
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_MENU_COUNT, gOpt->mBackwardMenuCount);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.history_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_HISTORY, MAX_HISTORY, DEF_HISTORY);
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_HISTORY_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.backward_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_BACKWARD, MAX_BACKWARD, DEF_BACKWARD);
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_BACKWARD_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.history_menu_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_HISTORY_MENU, MAX_HISTORY_MENU, DEF_HISTORY_MENU);
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_HISTORY_MENU_COUNT_RANGE, sText);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.backward_menu_count_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_BACKWARD_MENU, MAX_BACKWARD_MENU, DEF_BACKWARD_MENU);
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_BACKWARD_MENU_COUNT_RANGE, sText);

    SetDlgItemText(IDC_CFG_FUNC_HISTORY_SAVE_HISTROY,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.history.check.save_history")));
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_HISTORY_COUNT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.history_count")));
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_BACKWARD_COUNT,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.backward_count")));
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_HISTORY_MENU_COUNT,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.history_menu_count")));
    SetDlgItemText(IDC_CFG_FUNC_HISTORY_LABEL_BACKWARD_MENU_COUNT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.history.label.backward_menu_count")));

    return XPR_TRUE;
}

xpr_bool_t CfgFuncHistoryDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_HISTORY_SAVE_HISTROY:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_HISTORY_HISTORY_COUNT:
        case IDC_CFG_FUNC_HISTORY_BACKWARD_COUNT:
        case IDC_CFG_FUNC_HISTORY_HISTORY_MENU_COUNT:
        case IDC_CFG_FUNC_HISTORY_BACKWARD_MENU_COUNT:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgFuncHistoryDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mSaveHistory       = ((CButton *)GetDlgItem(IDC_CFG_FUNC_HISTORY_SAVE_HISTROY))->GetCheck();
    gOpt->mHistoryCount      = GetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_COUNT);
    gOpt->mBackwardCount     = GetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_COUNT);
    gOpt->mHistoryMenuCount  = GetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_MENU_COUNT);
    gOpt->mBackwardMenuCount = GetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_MENU_COUNT);

    if (gOpt->mHistoryCount < MIN_HISTORY) gOpt->mHistoryCount = MIN_HISTORY;
    if (gOpt->mHistoryCount > MAX_HISTORY) gOpt->mHistoryCount = MAX_HISTORY;
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_COUNT, gOpt->mHistoryCount);

    if (gOpt->mBackwardCount < MIN_BACKWARD) gOpt->mBackwardCount = MIN_BACKWARD;
    if (gOpt->mBackwardCount > MAX_BACKWARD) gOpt->mBackwardCount = MAX_BACKWARD;
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_COUNT, gOpt->mBackwardCount);

    if (gOpt->mHistoryMenuCount < MIN_HISTORY_MENU) gOpt->mHistoryMenuCount = MIN_HISTORY_MENU;
    if (gOpt->mHistoryMenuCount > MAX_HISTORY_MENU) gOpt->mHistoryMenuCount = MAX_HISTORY_MENU;
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_HISTORY_MENU_COUNT, gOpt->mHistoryMenuCount);

    if (gOpt->mBackwardMenuCount < MIN_BACKWARD_MENU) gOpt->mBackwardMenuCount = MIN_BACKWARD_MENU;
    if (gOpt->mBackwardMenuCount > MAX_BACKWARD_MENU) gOpt->mBackwardMenuCount = MAX_BACKWARD_MENU;
    SetDlgItemInt(IDC_CFG_FUNC_HISTORY_BACKWARD_MENU_COUNT, gOpt->mBackwardMenuCount);

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}
