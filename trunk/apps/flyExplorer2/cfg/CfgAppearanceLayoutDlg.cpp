//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAppearanceLayoutDlg.h"

#include "../resource.h"
#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAppearanceLayoutDlg::CfgAppearanceLayoutDlg(void)
    : super(IDD_CFG_APPEARANCE_LAYOUT, XPR_NULL)
{
}

void CfgAppearanceLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceLayoutDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_LAYOUT_SHOW_PATH_BAR,           OnPathBar)
    ON_BN_CLICKED(IDC_CFG_LAYOUT_SHOW_EACH_DRIVE_BAR,     OnDriveBar)
    ON_BN_CLICKED(IDC_CFG_LAYOUT_SHOW_INFO_BAR,           OnInfoBar)
    ON_BN_CLICKED(IDC_CFG_LAYOUT_INFO_BAR_STANDARD_STYLE, OnInfoBar)
    ON_BN_CLICKED(IDC_CFG_LAYOUT_INFO_BAR_BASIC_STYLE,    OnInfoBar)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceLayoutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_LAYOUT_VIEW_SPLIT_RESIZE_BY_RATIO,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.view_split_resize_by_ratio")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_ADDRESS_BAR,               theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_address_bar")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_PATH_BAR,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_path_bar")));
    SetDlgItemText(IDC_CFG_LAYOUT_PATH_BAR_ICON,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.path_bar_icon")));
    SetDlgItemText(IDC_CFG_LAYOUT_PATH_BAR_REAL_PATH,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.path_bar_real_path")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_EACH_DRIVE_BAR,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_each_drive_bar")));
    SetDlgItemText(IDC_CFG_LAYOUT_DRIVE_BAR_LEFT_SIDE,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.drive_bar_left_side")));
    SetDlgItemText(IDC_CFG_LAYOUT_DRIVE_BAR_SHORT_TEXT,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.drive_bar_short_text")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_ACTIVATE_BAR,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_activate_bar")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_INFO_BAR,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_info_bar")));
    SetDlgItemText(IDC_CFG_LAYOUT_GROUP_INFO_BAR_STYLE_OPTION,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.group.info_bar_style")));
    SetDlgItemText(IDC_CFG_LAYOUT_INFO_BAR_STANDARD_STYLE,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.radio.info_bar_standard_style")));
    SetDlgItemText(IDC_CFG_LAYOUT_INFO_BAR_BASIC_STYLE,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.radio.info_bar_basic_style")));
    SetDlgItemText(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.info_bar_no_display_selected_file_info")));
    SetDlgItemText(IDC_CFG_LAYOUT_INFO_BAR_SHOW_BOOKMARK,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.info_bar_show_bookmark")));
    SetDlgItemText(IDC_CFG_LAYOUT_INFO_BAR_SHOW_ATTR_ARHS_STYLE,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.info_bar_show_arhs_attribute_style")));
    SetDlgItemText(IDC_CFG_LAYOUT_SHOW_STATUS_BAR,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.layout.check.show_status_bar")));

    return XPR_TRUE;
}

void CfgAppearanceLayoutDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgAppearanceLayoutDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_VIEW_SPLIT_RESIZE_BY_RATIO    ))->SetCheck(aConfig.mViewSplitByRatio);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_ADDRESS_BAR              ))->SetCheck(aConfig.mShowAddressBar);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_PATH_BAR                 ))->SetCheck(aConfig.mShowPathBar);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_ICON                 ))->SetCheck(aConfig.mPathBarIcon);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_REAL_PATH            ))->SetCheck(aConfig.mPathBarRealPath);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_EACH_DRIVE_BAR           ))->SetCheck(aConfig.mShowEachDriveBar);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_LEFT_SIDE           ))->SetCheck(aConfig.mDriveBarLeftSide);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_SHORT_TEXT          ))->SetCheck(aConfig.mDriveBarShortText);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_ACTIVATE_BAR             ))->SetCheck(aConfig.mShowActivateBar);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_INFO_BAR                 ))->SetCheck(aConfig.mShowInfoBar);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_BASIC_STYLE          ))->SetCheck((aConfig.mContentsStyle == CONTENTS_BASIC) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_STANDARD_STYLE       ))->SetCheck((aConfig.mContentsStyle == CONTENTS_EXPLORER) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO))->SetCheck(aConfig.mContentsNoDispSelFileInfo);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_BOOKMARK        ))->SetCheck(aConfig.mContentsBookmark);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_ATTR_ARHS_STYLE ))->SetCheck(aConfig.mContentsARHSAttribute);
    ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_STATUS_BAR               ))->SetCheck(aConfig.mShowStatusBar);

    OnPathBar();
    OnDriveBar();
    OnInfoBar();
}

void CfgAppearanceLayoutDlg::onApply(Option::Config &aConfig)
{
    if (((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_BASIC_STYLE))->GetCheck())
        aConfig.mContentsStyle = CONTENTS_BASIC;
    else
        aConfig.mContentsStyle = CONTENTS_EXPLORER;

    aConfig.mViewSplitByRatio          = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_VIEW_SPLIT_RESIZE_BY_RATIO    ))->GetCheck();
    aConfig.mShowAddressBar            = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_ADDRESS_BAR              ))->GetCheck();
    aConfig.mShowPathBar               = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_PATH_BAR                 ))->GetCheck();
    aConfig.mPathBarIcon               = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_ICON                 ))->GetCheck();
    aConfig.mPathBarRealPath           = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_REAL_PATH            ))->GetCheck();
    aConfig.mShowEachDriveBar          = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_EACH_DRIVE_BAR           ))->GetCheck();
    aConfig.mDriveBarLeftSide          = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_LEFT_SIDE           ))->GetCheck();
    aConfig.mDriveBarShortText         = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_SHORT_TEXT          ))->GetCheck();
    aConfig.mShowActivateBar           = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_ACTIVATE_BAR             ))->GetCheck();
    aConfig.mShowInfoBar               = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_INFO_BAR                 ))->GetCheck();
    aConfig.mContentsNoDispSelFileInfo = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO))->GetCheck();
    aConfig.mContentsBookmark          = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_BOOKMARK        ))->GetCheck();
    aConfig.mContentsARHSAttribute     = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_ATTR_ARHS_STYLE ))->GetCheck();
    aConfig.mShowStatusBar             = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_STATUS_BAR               ))->GetCheck();
}

void CfgAppearanceLayoutDlg::OnPathBar(void)
{
    xpr_bool_t sShowPathBar = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_PATH_BAR))->GetCheck();
    GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_ICON     )->EnableWindow(sShowPathBar);
    GetDlgItem(IDC_CFG_LAYOUT_PATH_BAR_REAL_PATH)->EnableWindow(sShowPathBar);
}

void CfgAppearanceLayoutDlg::OnDriveBar(void)
{
    xpr_bool_t sShowDriveBar = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_EACH_DRIVE_BAR))->GetCheck();
    GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_LEFT_SIDE )->EnableWindow(sShowDriveBar);
    GetDlgItem(IDC_CFG_LAYOUT_DRIVE_BAR_SHORT_TEXT)->EnableWindow(sShowDriveBar);
}

void CfgAppearanceLayoutDlg::OnInfoBar(void)
{
    xpr_bool_t sShowInfoBar = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_SHOW_INFO_BAR))->GetCheck();

    GetDlgItem(IDC_CFG_LAYOUT_GROUP_INFO_BAR_STYLE_OPTION   )->EnableWindow(sShowInfoBar);
    GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_BASIC_STYLE          )->EnableWindow(sShowInfoBar);
    GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_STANDARD_STYLE       )->EnableWindow(sShowInfoBar);
    GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_BOOKMARK        )->EnableWindow(sShowInfoBar);
    GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_SHOW_ATTR_ARHS_STYLE )->EnableWindow(sShowInfoBar);

    if (XPR_IS_FALSE(sShowInfoBar))
    {
        GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO)->EnableWindow(XPR_FALSE);
    }
    else
    {
        xpr_bool_t sStandardStyle = ((CButton *)GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_STANDARD_STYLE))->GetCheck();
        if (XPR_IS_FALSE(sStandardStyle))
        {
            GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO)->EnableWindow(XPR_FALSE);
        }
        else
        {
            GetDlgItem(IDC_CFG_LAYOUT_INFO_BAR_NO_DISP_SEL_FILE_INFO)->EnableWindow(XPR_TRUE);
        }
    }
}
