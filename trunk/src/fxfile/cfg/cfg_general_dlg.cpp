//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_general_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgGeneralDlg::CfgGeneralDlg(void)
    : super(IDD_CFG_GENERAL, XPR_NULL)
{
}

void CfgGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgGeneralDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_GENERAL_DEFAULT, OnDefault)
END_MESSAGE_MAP()

xpr_bool_t CfgGeneralDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.none")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.show_always")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.extension.hide_known")));

    SetDlgItemText(IDC_CFG_GENERAL_GROUP_MOUSE_CLICK,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.group.mouse_click")));
    SetDlgItemText(IDC_CFG_GENERAL_ONE_CLICK,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.radio.one_click")));
    SetDlgItemText(IDC_CFG_GENERAL_DOUBLE_CLICK,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.radio.double_click")));
    SetDlgItemText(IDC_CFG_GENERAL_GROUP_GENERAL,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.group.general")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_hidden_file")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_protected_file")));
    SetDlgItemText(IDC_CFG_GENERAL_TITLE_FULL_PATH,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.display_full_path_on_title")));
    SetDlgItemText(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.display_full_path_on_address_bar")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_TOOLTIP,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_tooltip")));
    SetDlgItemText(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.show_tooltip_with_name")));
    SetDlgItemText(IDC_CFG_GENERAL_CREATE_BY_POPUP,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.create_by_popup")));
    SetDlgItemText(IDC_CFG_GENERAL_GO_UP_AND_SEL_CHILD_ITEM, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.check.go_up_and_select_the_child_item")));
    SetDlgItemText(IDC_CFG_GENERAL_LABEL_EXTENSION,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.label.show_extension")));
    SetDlgItemText(IDC_CFG_GENERAL_DEFAULT,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.general.button.default")));

    return XPR_TRUE;
}

void CfgGeneralDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgGeneralDlg::OnDefault(void) 
{
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK               ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_DOUBLE_CLICK            ))->SetCheck(XPR_TRUE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE        ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE     ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH         ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH   ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP            ))->SetCheck(XPR_TRUE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME  ))->SetCheck(XPR_FALSE);
    ((CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION               ))->SetCurSel(1);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP         ))->SetCheck(XPR_FALSE);
    ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_GO_UP_AND_SEL_CHILD_ITEM))->SetCheck(XPR_TRUE);
}

void CfgGeneralDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK               ))->SetCheck((aConfig.mMouseClick == MOUSE_ONE_CLICK) ? XPR_TRUE : XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_DOUBLE_CLICK            ))->SetCheck((aConfig.mMouseClick == MOUSE_DOUBLE_CLICK) ? XPR_TRUE : XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH   ))->SetCheck(aConfig.mAddressFullPath);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE        ))->SetCheck(aConfig.mShowHiddenAttribute);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE     ))->SetCheck(aConfig.mShowSystemAttribute);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP            ))->SetCheck(aConfig.mTooltip);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME  ))->SetCheck(aConfig.mTooltipWithFileName);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH         ))->SetCheck(aConfig.mTitleFullPath);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP         ))->SetCheck(aConfig.mNewItemDlg);
    ((CButton *)GetDlgItem(IDC_CFG_GENERAL_GO_UP_AND_SEL_CHILD_ITEM))->SetCheck(aConfig.mExplorerGoUpSelSubFolder);

    ((CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION))->SetCurSel(aConfig.mFileExtType);
}

void CfgGeneralDlg::onApply(Option::Config &aConfig)
{
    aConfig.mMouseClick               = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ONE_CLICK               ))->GetCheck() ? MOUSE_ONE_CLICK : MOUSE_DOUBLE_CLICK;
    aConfig.mAddressFullPath          = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_ADDRESS_BAR_FULL_PATH   ))->GetCheck();
    aConfig.mShowHiddenAttribute      = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_HIDDEN_FILE        ))->GetCheck();
    aConfig.mShowSystemAttribute      = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_PROTECTED_FILE     ))->GetCheck();
    aConfig.mTooltip                  = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP            ))->GetCheck();
    aConfig.mTooltipWithFileName      = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_SHOW_TOOLTIP_WITH_NAME  ))->GetCheck();
    aConfig.mTitleFullPath            = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_TITLE_FULL_PATH         ))->GetCheck();
    aConfig.mFileExtType              = ((CComboBox *)GetDlgItem(IDC_CFG_GENERAL_EXTENSION               ))->GetCurSel();
    aConfig.mNewItemDlg               = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_CREATE_BY_POPUP         ))->GetCheck();
    aConfig.mExplorerGoUpSelSubFolder = ((CButton   *)GetDlgItem(IDC_CFG_GENERAL_GO_UP_AND_SEL_CHILD_ITEM))->GetCheck();
}
} // namespace cfg
} // namespace fxfile
