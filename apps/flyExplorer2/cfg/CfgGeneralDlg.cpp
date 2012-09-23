//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgGeneralDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgGeneralDlg::CfgGeneralDlg(void)
    : super(IDD_CFG_GENERAL, XPR_NULL)
{
}

void CfgGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgGeneralDlg, super)
    ON_BN_CLICKED(IDC_CFG_GENERAL_DEFAULT, OnDefault)
END_MESSAGE_MAP()

xpr_bool_t CfgGeneralDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.none")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.show_always")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.hide_known")));

    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK             ))->SetCheck((gOpt->mMouseClick == MOUSE_ONE_CLICK) ? XPR_TRUE : XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_DOUBLE_CLICK          ))->SetCheck((gOpt->mMouseClick == MOUSE_DOUBLE_CLICK) ? XPR_TRUE : XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH ))->SetCheck(gOpt->mAddressFullPath);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE      ))->SetCheck(gOpt->mShowHiddenAttribute);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE   ))->SetCheck(gOpt->mShowSystemAttribute);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP          ))->SetCheck(gOpt->mTooltip);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME))->SetCheck(gOpt->mTooltipWithFileName);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH       ))->SetCheck(gOpt->mTitleFullPath);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP       ))->SetCheck(gOpt->mNewItemDlg);

    sComboBox->SetCurSel(gOpt->mFileExtType);

    SetDlgItemText(IDC_CFG_GENERAL_GROUP_MOUSE_CLICK,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.group.mouse_click")));
    SetDlgItemText(IDC_CFG_GENERAL_ONE_CLICK,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.radio.one_click")));
    SetDlgItemText(IDC_CFG_GENERAL_DOUBLE_CLICK,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.radio.double_click")));
    SetDlgItemText(IDC_CFG_GENERAL_GROUP_GENERAL,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.group.general")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_hidden_file")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_protected_file")));
    SetDlgItemText(IDC_CFG_GENERAL_TITLE_FULL_PATH,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.display_full_path_on_title")));
    SetDlgItemText(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.display_full_path_on_address_bar")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_TOOLTIP,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_tooltip")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_tooltip_with_name")));
    SetDlgItemText(IDC_CFG_GENERAL_CREATE_BY_POPUP,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.create_by_popup")));
    SetDlgItemText(IDC_CFG_GENERAL_LABEL_EXTENSION,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.label.show_extension")));
    SetDlgItemText(IDC_CFG_GENERAL_DEFAULT,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.button.default")));

    return XPR_TRUE;
}

void CfgGeneralDlg::OnDefault(void) 
{
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK             ))->SetCheck(0);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_DOUBLE_CLICK          ))->SetCheck(1);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE      ))->SetCheck(0);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE   ))->SetCheck(0);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH       ))->SetCheck(0);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH ))->SetCheck(0);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP          ))->SetCheck(1);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME))->SetCheck(0);
    ((CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION             ))->SetCurSel(1);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP       ))->SetCheck(0);
}

void CfgGeneralDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    xpr_bool_t sOldShowHidden = gOpt->mShowHiddenAttribute;
    xpr_bool_t sOldShowSystem = gOpt->mShowSystemAttribute;

    gOpt->mMouseClick          = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK             ))->GetCheck() ? MOUSE_ONE_CLICK : MOUSE_DOUBLE_CLICK;
    gOpt->mAddressFullPath     = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH ))->GetCheck();
    gOpt->mShowHiddenAttribute = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE      ))->GetCheck();
    gOpt->mShowSystemAttribute = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE   ))->GetCheck();
    gOpt->mTooltip             = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP          ))->GetCheck();
    gOpt->mTooltipWithFileName = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME))->GetCheck();
    gOpt->mTitleFullPath       = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH       ))->GetCheck();
    gOpt->mFileExtType         = ((CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION             ))->GetCurSel();
    gOpt->mNewItemDlg          = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP       ))->GetCheck();

    sOptionMgr.setModifiedHidden((sOldShowHidden != gOpt->mShowHiddenAttribute));
    sOptionMgr.setModifiedSystem((sOldShowSystem != gOpt->mShowSystemAttribute));

    sOptionMgr.applyFolderCtrl(0, XPR_FALSE);
    sOptionMgr.applyExplorerView(0, XPR_FALSE);
    sOptionMgr.applyEtc(0);

    sOptionMgr.setModifiedHidden(XPR_FALSE);
    sOptionMgr.setModifiedSystem(XPR_FALSE);
}

xpr_bool_t CfgGeneralDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_GENERAL_DEFAULT:
        case IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH:
        case IDC_CFG_GENERAL_SHOW_HIDDEN_FILE:
        case IDC_CFG_GENERAL_SHOW_PROTECTED_FILE:
        case IDC_CFG_GENERAL_ONE_CLICK:
        case IDC_CFG_GENERAL_DOUBLE_CLICK:
        case IDC_CFG_GENERAL_SHOW_TOOLTIP:
        case IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME:
        case IDC_CFG_GENERAL_TITLE_FULL_PATH:
        case IDC_CFG_GENERAL_CREATE_BY_POPUP:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_GENERAL_EXTENSION:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
