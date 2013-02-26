//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAppearanceViewSetDlg.h"

#include "../rgc/FileDialogST.h"

#include "../Option.h"
#include "../resource.h"
#include "../ViewSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAppearanceViewSetDlg::CfgAppearanceViewSetDlg(void)
    : super(IDD_CFG_GENERAL_VIEWSET, XPR_NULL)
{
}

void CfgAppearanceViewSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceViewSetDlg, super)
    ON_BN_CLICKED(IDC_CFG_VIEWSET_CLEAN, OnClean)
    ON_BN_CLICKED(IDC_CFG_VIEWSET_INIT,  OnInit)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceViewSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_VIEWSET_CLEAN);
    addIgnoreApply(IDC_CFG_VIEWSET_INIT);

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_VIEW_STYLE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.view_style.large_icon")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.view_style.small_icon")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.view_style.list")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.view_style.detail")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.view_style.thumbnail")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort.name")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort.size")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort.type")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort.date")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort.attr")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT_ORDER);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort_order.ascending")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.sort_order.decending")));

    SetDlgItemText(IDC_CFG_VIEWSET_GROUP_VIEW_SET,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.group.view_set")));
    SetDlgItemText(IDC_CFG_VIEWSET_NONE,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.radio.none")));
    SetDlgItemText(IDC_CFG_VIEWSET_SAVE_VIEW_STYLE,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.check.save_view_style")));
    SetDlgItemText(IDC_CFG_VIEWSET_DEFAULT,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.radio.default")));
    SetDlgItemText(IDC_CFG_VIEWSET_SAME_BETWEEN_SPLIT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.radio.same_between_split")));
    SetDlgItemText(IDC_CFG_VIEWSET_EACH_FOLDER,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.radio.each_folder")));
    SetDlgItemText(IDC_CFG_VIEWSET_LABEL_DEFAULT_VIEW_STYLE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.label.default_view_style")));
    SetDlgItemText(IDC_CFG_VIEWSET_LABEL_DEFAULT_SORT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.label.default_sort")));
    SetDlgItemText(IDC_CFG_VIEWSET_LABEL_DEFAULT_SORT_ORDER, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.label.default_sort_order")));
    SetDlgItemText(IDC_CFG_VIEWSET_NO_SORT,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.check.no_sort")));
    SetDlgItemText(IDC_CFG_VIEWSET_AUTO_COLUMN_WIDTH,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.check.automatic_column_width")));
    SetDlgItemText(IDC_CFG_VIEWSET_CLEAN_ON_EXIT,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.check.clean_on_exit")));
    SetDlgItemText(IDC_CFG_VIEWSET_CLEAN,                    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.button.clean")));
    SetDlgItemText(IDC_CFG_VIEWSET_INIT,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.button.initialize")));

    return XPR_TRUE;
}

void CfgAppearanceViewSetDlg::onInit(Option::Config &aConfig)
{
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_NONE              ))->SetCheck(aConfig.mExplorerSaveViewSet == 0);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT           ))->SetCheck(aConfig.mExplorerSaveViewSet == 1);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_SAME_BETWEEN_SPLIT))->SetCheck(aConfig.mExplorerSaveViewSet == 2);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_EACH_FOLDER       ))->SetCheck(aConfig.mExplorerSaveViewSet == 3);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_SAVE_VIEW_STYLE   ))->SetCheck(aConfig.mExplorerSaveViewStyle);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_NO_SORT           ))->SetCheck(aConfig.mExplorerNoSort);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_AUTO_COLUMN_WIDTH ))->SetCheck(aConfig.mExplorerAutoColumnWidth);
    ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_CLEAN_ON_EXIT     ))->SetCheck(aConfig.mExplorerExitVerifyViewSet);
    ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_VIEW_STYLE))->SetCurSel(aConfig.mExplorerDefaultViewStyle);
    ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT      ))->SetCurSel(aConfig.mExplorerDefaultSort);
    ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT_ORDER))->SetCurSel(aConfig.mExplorerDefaultSortOrder);
}

void CfgAppearanceViewSetDlg::onApply(Option::Config &aConfig)
{
    aConfig.mExplorerSaveViewSet = SAVE_VIEW_SET_DEFAULT;
    if (((CButton *)GetDlgItem(IDC_CFG_VIEWSET_NONE))->GetCheck())
        aConfig.mExplorerSaveViewSet = SAVE_VIEW_SET_NONE;
    else if (((CButton *)GetDlgItem(IDC_CFG_VIEWSET_SAME_BETWEEN_SPLIT))->GetCheck())
        aConfig.mExplorerSaveViewSet = SAVE_VIEW_SET_ALL_OF_SAME;
    else if (((CButton *)GetDlgItem(IDC_CFG_VIEWSET_EACH_FOLDER))->GetCheck())
        aConfig.mExplorerSaveViewSet = SAVE_VIEW_SET_EACH_FOLDER;

    aConfig.mExplorerNoSort            = ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_NO_SORT           ))->GetCheck();
    aConfig.mExplorerSaveViewStyle     = ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_SAVE_VIEW_STYLE   ))->GetCheck();
    aConfig.mExplorerAutoColumnWidth   = ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_AUTO_COLUMN_WIDTH ))->GetCheck();
    aConfig.mExplorerExitVerifyViewSet = ((CButton   *)GetDlgItem(IDC_CFG_VIEWSET_CLEAN_ON_EXIT     ))->GetCheck();
    aConfig.mExplorerDefaultViewStyle  = ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_VIEW_STYLE))->GetCurSel();
    aConfig.mExplorerDefaultSort       = ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT      ))->GetCurSel();
    aConfig.mExplorerDefaultSortOrder  = ((CComboBox *)GetDlgItem(IDC_CFG_VIEWSET_DEFAULT_SORT_ORDER))->GetCurSel();
}

void CfgAppearanceViewSetDlg::OnClean(void)
{
    ViewSetMgr &sViewSetMgr = ViewSetMgr::instance();
    sViewSetMgr.verify();

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.msg.completed_clean"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgAppearanceViewSetDlg::OnInit(void)
{
    const xpr_tchar_t *sMsg;

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.msg.confirm_initialize"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    ViewSetMgr &sViewSetMgr = ViewSetMgr::instance();
    sViewSetMgr.clear();

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.view_set.msg.completed_initialize"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}
