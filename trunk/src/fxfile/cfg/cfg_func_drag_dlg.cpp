//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_func_drag_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cfg
{
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
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_DRAG_DROP_DEFAULT_OPERATION);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.default_file_operation.default")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.default_file_operation.move")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.default_file_operation.copy")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.default_file_operation.shortcut")));

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG_SPIN);
    sSpinCtrl->SetRange(MIN_DIST_DRAG, MAX_DIST_DRAG);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_DRAG_DROP_EXPAND_FOLDER_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_EXPAND_FOLDER_MSEC, MAX_EXPAND_FOLDER_MSEC);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_DRAG_DROP_SCROLL_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_DRAG_SCROLL_MSEC, MAX_DRAG_SCROLL_MSEC);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.drag_distance_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_DIST_DRAG, MAX_DIST_DRAG, DEF_DIST_DRAG);
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_DIST_DRAG_RANGE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.dragging_folder_expand_time_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_EXPAND_FOLDER_MSEC, MAX_EXPAND_FOLDER_MSEC, DEF_EXPAND_FOLDER_MSEC);
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_EXPAND_FOLDER_TIME_RANGE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.dragging_scroll_time_range"), XPR_STRING_LITERAL("%d,%d,%d")), MIN_DRAG_SCROLL_MSEC, MAX_DRAG_SCROLL_MSEC, DEF_DRAG_SCROLL_MSEC);
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_SCROLL_TIME_RANGE, sText);

    SetDlgItemText(IDC_CFG_DRAG_DROP_GROUP_DRAG_START,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.group.drag_start")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_NO_DRAG,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drag_none")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_DEFAULT_DRAG,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drag_default")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_CTRL_DRAG,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drag_CTRL")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_DIST_DRAG,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drag_distance")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_GROUP_DRAGGING,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.group.dragging")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_FOLDER_EXPAND_TIME, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.dragging_folder_expand_time")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_SCROLL_TIME,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.dragging_scroll_time")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_DRAGGING_NO_CONTENTS,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.check.dragging_no_contents")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_GROUP_DROP,               theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.group.drop")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_NO_DROP,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drop_none")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_DEFAULT_DROP,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.radio.drop_default")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_GROUP_ETC,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.group.etc")));
    SetDlgItemText(IDC_CFG_DRAG_DROP_LABEL_DEFAULT_OPERATION,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.drag_drop.label.default_file_operation")));

    return XPR_TRUE;
}

void CfgFuncDragDlg::onInit(Option::Config &aConfig)
{
    xpr_sint_t sId;
    switch (aConfig.mDragType)
    {
    case DRAG_START_NONE:    sId = IDC_CFG_DRAG_DROP_NO_DRAG;      break;
    case DRAG_START_CTRL:    sId = IDC_CFG_DRAG_DROP_CTRL_DRAG;    break;
    case DRAG_START_DIST:    sId = IDC_CFG_DRAG_DROP_DIST_DRAG;    break;

    default:
    case DRAG_START_DEFAULT: sId = IDC_CFG_DRAG_DROP_DEFAULT_DRAG; break;
    }

    ((CButton *)GetDlgItem(sId))->SetCheck(XPR_TRUE);
    enableWindowDrag();

    switch (aConfig.mDragType)
    {
    case 0: sId = IDC_CFG_DRAG_DROP_NO_DROP;      break;

    default:
    case 1: sId = IDC_CFG_DRAG_DROP_DEFAULT_DROP; break;
    }

    ((CButton *)GetDlgItem(sId))->SetCheck(XPR_TRUE);

    ((CComboBox *)GetDlgItem(IDC_CFG_DRAG_DROP_DEFAULT_OPERATION))->SetCurSel(aConfig.mDragDefaultFileOp);

    SetDlgItemInt(IDC_CFG_DRAG_DROP_DIST_DRAG_VALUE,    aConfig.mDragDist);
    SetDlgItemInt(IDC_CFG_DRAG_DROP_FOLDER_EXPAND_TIME, aConfig.mDragFolderTreeExpandTime);
    SetDlgItemInt(IDC_CFG_DRAG_DROP_SCROLL_TIME,        aConfig.mDragScrollTime);

    ((CEdit *)GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG_VALUE   ))->LimitText(30);
    ((CEdit *)GetDlgItem(IDC_CFG_DRAG_DROP_FOLDER_EXPAND_TIME))->LimitText(6);
    ((CEdit *)GetDlgItem(IDC_CFG_DRAG_DROP_SCROLL_TIME       ))->LimitText(6);

    ((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DRAGGING_NO_CONTENTS))->SetCheck(aConfig.mDragNoContents);
}

void CfgFuncDragDlg::onApply(Option::Config &aConfig)
{
    aConfig.mDragType = DRAG_START_NONE;
    if (((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DEFAULT_DRAG))->GetCheck())
        aConfig.mDragType = DRAG_START_DEFAULT;
    else if (((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_CTRL_DRAG))->GetCheck())
        aConfig.mDragType = DRAG_START_CTRL;
    else if (((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG))->GetCheck())
        aConfig.mDragType = DRAG_START_DIST;

    aConfig.mDropType                 = ((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DEFAULT_DROP))->GetCheck() ? DROP_DEFAULT : DROP_NONE;
    aConfig.mDragDefaultFileOp        = ((CComboBox *)GetDlgItem(IDC_CFG_DRAG_DROP_DEFAULT_OPERATION))->GetCurSel();
    aConfig.mDragDist                 = GetDlgItemInt(IDC_CFG_DRAG_DROP_DIST_DRAG_VALUE, XPR_NULL, XPR_FALSE);
    aConfig.mDragFolderTreeExpandTime = GetDlgItemInt(IDC_CFG_DRAG_DROP_FOLDER_EXPAND_TIME, XPR_NULL, XPR_FALSE);
    aConfig.mDragScrollTime           = GetDlgItemInt(IDC_CFG_DRAG_DROP_SCROLL_TIME, XPR_NULL, XPR_FALSE);
    aConfig.mDragNoContents           = ((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DRAGGING_NO_CONTENTS))->GetCheck();

    if (aConfig.mDragDist < MIN_DIST_DRAG) aConfig.mDragDist = MIN_DIST_DRAG;
    if (aConfig.mDragDist > MAX_DIST_DRAG) aConfig.mDragDist = MAX_DIST_DRAG;

    if (aConfig.mDragFolderTreeExpandTime < MIN_EXPAND_FOLDER_MSEC) aConfig.mDragFolderTreeExpandTime = MIN_EXPAND_FOLDER_MSEC;
    if (aConfig.mDragFolderTreeExpandTime > MAX_EXPAND_FOLDER_MSEC) aConfig.mDragFolderTreeExpandTime = MAX_EXPAND_FOLDER_MSEC;

    if (aConfig.mDragScrollTime < MIN_DRAG_SCROLL_MSEC) aConfig.mDragScrollTime = MIN_DRAG_SCROLL_MSEC;
    if (aConfig.mDragScrollTime > MAX_DRAG_SCROLL_MSEC) aConfig.mDragScrollTime = MAX_DRAG_SCROLL_MSEC;
}

void CfgFuncDragDlg::enableWindowDrag(void)
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG))->GetCheck();

    GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG_VALUE)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_DRAG_DROP_DIST_DRAG_SPIN )->EnableWindow(sEnable);
}
} // namespace cfg
} // namespace fxfile
