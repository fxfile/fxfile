//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpViewSetDlg.h"

#include "../rgc/FileDialogST.h"

#include "../Option.h"
#include "../resource.h"
#include "../ViewSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgExpViewSetDlg::CfgExpViewSetDlg(void)
    : super(IDD_CFG_EXP_VIEWSET, XPR_NULL)
{
}

void CfgExpViewSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgExpViewSetDlg, super)
    ON_BN_CLICKED(IDC_CFG_EXP_VIEW_CLEAN, OnClean)
    ON_BN_CLICKED(IDC_CFG_EXP_VIEW_INIT,  OnInit)
END_MESSAGE_MAP()

xpr_bool_t CfgExpViewSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_VIEW_STYLE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.view_style.large_icon")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.view_style.small_icon")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.view_style.list")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.view_style.detail")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.view_style.thumbnail")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort.name")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort.size")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort.type")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort.date")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort.attr")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT_ORDER);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort_order.ascending")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.sort_order.decending")));

    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_NONE              ))->SetCheck(gOpt->mExplorerSaveViewSet == 0);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT           ))->SetCheck(gOpt->mExplorerSaveViewSet == 1);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_SAME_BETWEEN_SPLIT))->SetCheck(gOpt->mExplorerSaveViewSet == 2);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_EACH_FOLDER       ))->SetCheck(gOpt->mExplorerSaveViewSet == 3);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_SAVE_VIEW_STYLE   ))->SetCheck(gOpt->mExplorerSaveViewStyle);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_NO_SORT           ))->SetCheck(gOpt->mExplorerNoSort);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_AUTO_COLUMN_WIDTH ))->SetCheck(gOpt->mExplorerAutoColumnWidth);
    ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_CLEAN_ON_EXIT     ))->SetCheck(gOpt->mExplorerExitVerifyViewSet);
    ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_VIEW_STYLE))->SetCurSel(gOpt->mExplorerDefaultViewStyle);
    ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT      ))->SetCurSel(gOpt->mExplorerDefaultSort);
    ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT_ORDER))->SetCurSel(gOpt->mExplorerDefaultSortOrder);

    SetDlgItemText(IDC_CFG_EXP_VIEW_GROUP_VIEW_SET,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.group.view_set")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_NONE,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.radio.none")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_SAVE_VIEW_STYLE,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.check.save_view_style")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_DEFAULT,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.radio.default")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_SAME_BETWEEN_SPLIT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.radio.same_between_split")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_EACH_FOLDER,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.radio.each_folder")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_LABEL_DEFAULT_VIEW_STYLE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.label.default_view_style")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_LABEL_DEFAULT_SORT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.label.default_sort")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_LABEL_DEFAULT_SORT_ORDER, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.label.default_sort_order")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_NO_SORT,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.check.no_sort")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_AUTO_COLUMN_WIDTH,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.check.automatic_column_width")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_CLEAN_ON_EXIT,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.check.clean_on_exit")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_CLEAN,                    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.button.clean")));
    SetDlgItemText(IDC_CFG_EXP_VIEW_INIT,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.button.initialize")));

    return XPR_TRUE;
}

void CfgExpViewSetDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mExplorerSaveViewSet = SAVE_VIEW_SET_DEFAULT;
    if (((CButton *)GetDlgItem(IDC_CFG_EXP_VIEW_NONE))->GetCheck())
        gOpt->mExplorerSaveViewSet = SAVE_VIEW_SET_NONE;
    else if (((CButton *)GetDlgItem(IDC_CFG_EXP_VIEW_SAME_BETWEEN_SPLIT))->GetCheck())
        gOpt->mExplorerSaveViewSet = SAVE_VIEW_SET_SAME_BETWEEN_SPLIT;
    else if (((CButton *)GetDlgItem(IDC_CFG_EXP_VIEW_EACH_FOLDER))->GetCheck())
        gOpt->mExplorerSaveViewSet = SAVE_VIEW_SET_EACH_FOLDER;

    gOpt->mExplorerNoSort            = ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_NO_SORT           ))->GetCheck();
    gOpt->mExplorerSaveViewStyle     = ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_SAVE_VIEW_STYLE   ))->GetCheck();
    gOpt->mExplorerAutoColumnWidth   = ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_AUTO_COLUMN_WIDTH ))->GetCheck();
    gOpt->mExplorerExitVerifyViewSet = ((CButton   *)GetDlgItem(IDC_CFG_EXP_VIEW_CLEAN_ON_EXIT     ))->GetCheck();
    gOpt->mExplorerDefaultViewStyle  = ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_VIEW_STYLE))->GetCurSel();
    gOpt->mExplorerDefaultSort       = ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT      ))->GetCurSel();
    gOpt->mExplorerDefaultSortOrder  = ((CComboBox *)GetDlgItem(IDC_CFG_EXP_VIEW_DEFAULT_SORT_ORDER))->GetCurSel();

    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgExpViewSetDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_VIEW_NONE:
        case IDC_CFG_EXP_VIEW_DEFAULT:
        case IDC_CFG_EXP_VIEW_SAME_BETWEEN_SPLIT:
        case IDC_CFG_EXP_VIEW_EACH_FOLDER:
        case IDC_CFG_EXP_VIEW_SAVE_VIEW_STYLE:
        case IDC_CFG_EXP_VIEW_NO_SORT:
        case IDC_CFG_EXP_VIEW_AUTO_COLUMN_WIDTH:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_VIEW_DEFAULT_VIEW_STYLE:
        case IDC_CFG_EXP_VIEW_DEFAULT_SORT:
        case IDC_CFG_EXP_VIEW_DEFAULT_SORT_ORDER:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgExpViewSetDlg::OnClean(void)
{
    ViewSet sViewSet;
    sViewSet.verify();

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.msg.confirm_initialize"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgExpViewSetDlg::OnInit(void)
{
    const xpr_tchar_t *sMsg;

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.msg.confirm_initialize"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    ViewSet sViewSet;
    sViewSet.clear();

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view_set.msg.completed_initialize"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}
