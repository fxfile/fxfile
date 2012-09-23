//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncDragDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncDragDlg::CfgFuncDragDlg(void)
    : super(IDD_CFG_FUNC_DRAG, XPR_NULL)
{
}

void CfgFuncDragDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncDragDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncDragDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FUNC_DRAG_DEFAULT_OPERATION);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.default_file_operation.default")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.default_file_operation.move")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.default_file_operation.copy")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.default_file_operation.shortcut")));

    xpr_sint_t sId;
    switch (gOpt->mDragType)
    {
    case DRAG_START_NONE:    sId = IDC_CFG_FUNC_DRAG_NO_DRAG;      break;
    case DRAG_START_CTRL:    sId = IDC_CFG_FUNC_DRAG_CTRL_DRAG;    break;
    case DRAG_START_DIST:    sId = IDC_CFG_FUNC_DRAG_DIST_DRAG;    break;

    default:
    case DRAG_START_DEFAULT: sId = IDC_CFG_FUNC_DRAG_DEFAULT_DRAG; break;
    }

    ((CButton *)GetDlgItem(sId))->SetCheck(XPR_TRUE);
    enableWindowDrag();

    switch (gOpt->mDragType)
    {
    case 0: sId = IDC_CFG_FUNC_DRAG_NO_DROP;      break;

    default:
    case 1: sId = IDC_CFG_FUNC_DRAG_DEFAULT_DROP; break;
    }

    ((CButton *)GetDlgItem(sId))->SetCheck(XPR_TRUE);

    ((CComboBox *)GetDlgItem(IDC_CFG_FUNC_DRAG_DEFAULT_OPERATION))->SetCurSel(gOpt->mDragDefaultFileOp);

    SetDlgItemInt(IDC_CFG_FUNC_DRAG_DIST_DRAG_VALUE,    gOpt->mDragDist);
    SetDlgItemInt(IDC_CFG_FUNC_DRAG_FOLDER_EXPAND_TIME, gOpt->mDragFolderTreeExpandTime);
    SetDlgItemInt(IDC_CFG_FUNC_DRAG_SCROLL_TIME,        gOpt->mDragScrollTime);

    ((CEdit *)GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG_VALUE   ))->LimitText(30);
    ((CEdit *)GetDlgItem(IDC_CFG_FUNC_DRAG_FOLDER_EXPAND_TIME))->LimitText(6);
    ((CEdit *)GetDlgItem(IDC_CFG_FUNC_DRAG_SCROLL_TIME       ))->LimitText(6);

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG_SPIN);
    sSpinCtrl->SetRange(MIN_DIST_DRAG, MAX_DIST_DRAG);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FUNC_DRAG_EXPAND_FOLDER_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_EXPAND_FOLDER_MSEC, MAX_EXPAND_FOLDER_MSEC);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FUNC_DRAG_SCROLL_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_DRAG_SCROLL_MSEC, MAX_DRAG_SCROLL_MSEC);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.drag_distance_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_DIST_DRAG, MAX_DIST_DRAG, DEF_DIST_DRAG);
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_DIST_DRAG_RANGE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.dragging_folder_expand_time_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_EXPAND_FOLDER_MSEC, MAX_EXPAND_FOLDER_MSEC, DEF_EXPAND_FOLDER_MSEC);
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_EXPAND_FOLDER_TIME_RANGE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.dragging_scroll_time_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_DRAG_SCROLL_MSEC, MAX_DRAG_SCROLL_MSEC, DEF_DRAG_SCROLL_MSEC);
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_SCROLL_TIME_RANGE, sText);

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DRAGGING_NO_CONTENTS))->SetCheck(gOpt->mDragNoContents);

    SetDlgItemText(IDC_CFG_FUNC_DRAG_GROUP_DRAG_START,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.group.drag_start")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_NO_DRAG,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drag_none")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_DEFAULT_DRAG,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drag_default")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_CTRL_DRAG,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drag_CTRL")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_DIST_DRAG,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drag_distance")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_GROUP_DRAGGING,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.group.dragging")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_FOLDER_EXPAND_TIME, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.dragging_folder_expand_time")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_SCROLL_TIME,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.dragging_scroll_time")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_DRAGGING_NO_CONTENTS,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.check.dragging_no_contents")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_GROUP_DROP,               theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.group.drop")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_NO_DROP,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drop_none")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_DEFAULT_DROP,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.radio.drop_default")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_GROUP_ETC,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.group.etc")));
    SetDlgItemText(IDC_CFG_FUNC_DRAG_LABEL_DEFAULT_OPERATION,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag.label.default_file_operation")));

    return XPR_TRUE;
}

void CfgFuncDragDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mDragType = DRAG_START_NONE;
    if (((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DEFAULT_DRAG))->GetCheck())
        gOpt->mDragType = DRAG_START_DEFAULT;
    else if (((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_CTRL_DRAG))->GetCheck())
        gOpt->mDragType = DRAG_START_CTRL;
    else if (((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG))->GetCheck())
        gOpt->mDragType = DRAG_START_DIST;

    gOpt->mDropType                 = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DEFAULT_DROP))->GetCheck() ? DROP_DEFAULT : DROP_NONE;
    gOpt->mDragDefaultFileOp        = ((CComboBox *)GetDlgItem(IDC_CFG_FUNC_DRAG_DEFAULT_OPERATION))->GetCurSel();
    gOpt->mDragDist                 = GetDlgItemInt(IDC_CFG_FUNC_DRAG_DIST_DRAG_VALUE, XPR_NULL, XPR_FALSE);
    gOpt->mDragFolderTreeExpandTime = GetDlgItemInt(IDC_CFG_FUNC_DRAG_FOLDER_EXPAND_TIME, XPR_NULL, XPR_FALSE);
    gOpt->mDragScrollTime           = GetDlgItemInt(IDC_CFG_FUNC_DRAG_SCROLL_TIME, XPR_NULL, XPR_FALSE);
    gOpt->mDragNoContents           = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DRAGGING_NO_CONTENTS))->GetCheck();

    if (gOpt->mDragDist < MIN_DIST_DRAG) gOpt->mDragDist = MIN_DIST_DRAG;
    if (gOpt->mDragDist > MAX_DIST_DRAG) gOpt->mDragDist = MAX_DIST_DRAG;

    if (gOpt->mDragFolderTreeExpandTime < MIN_EXPAND_FOLDER_MSEC) gOpt->mDragFolderTreeExpandTime = MIN_EXPAND_FOLDER_MSEC;
    if (gOpt->mDragFolderTreeExpandTime > MAX_EXPAND_FOLDER_MSEC) gOpt->mDragFolderTreeExpandTime = MAX_EXPAND_FOLDER_MSEC;

    if (gOpt->mDragScrollTime < MIN_DRAG_SCROLL_MSEC) gOpt->mDragScrollTime = MIN_DRAG_SCROLL_MSEC;
    if (gOpt->mDragScrollTime > MAX_DRAG_SCROLL_MSEC) gOpt->mDragScrollTime = MAX_DRAG_SCROLL_MSEC;

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}

xpr_bool_t CfgFuncDragDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_DRAG_NO_DRAG:
        case IDC_CFG_FUNC_DRAG_DEFAULT_DRAG:
        case IDC_CFG_FUNC_DRAG_CTRL_DRAG:
        case IDC_CFG_FUNC_DRAG_DIST_DRAG:
            enableWindowDrag();
            setModified();
            break;

        case IDC_CFG_FUNC_DRAG_DRAGGING_NO_CONTENTS:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_DRAG_DIST_DRAG_VALUE:
        case IDC_CFG_FUNC_DRAG_FOLDER_EXPAND_TIME:
        case IDC_CFG_FUNC_DRAG_SCROLL_TIME:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_DRAG_DEFAULT_OPERATION:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgFuncDragDlg::enableWindowDrag(void)
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG))->GetCheck();

    GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG_VALUE)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_FUNC_DRAG_DIST_DRAG_SPIN )->EnableWindow(sEnable);
}
