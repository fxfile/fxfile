//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_dlg.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceDlg::CfgAppearanceDlg(void)
    : super(IDD_CFG_APPEARANCE, XPR_NULL)
{
}

void CfgAppearanceDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceDlg, super)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_APPEARANCE_MENU_ANIMATION,                    gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.check.menu_animation")));
    SetDlgItemText(IDC_CFG_APPEARANCE_STANDARD_MENU_STYLE,               gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.check.standard_style_menu")));
    SetDlgItemText(IDC_CFG_APPEARANCE_SINGLE_VIEW_SPLIT_AS_ACTIVED_VIEW, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.check.single_view_split_as_actived_view")));
    SetDlgItemText(IDC_CFG_APPEARANCE_VIEW_SPLIT_REOPEN_LAST_FOLDER,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.check.reopen_last_folder_when_changing_view_split")));

    return XPR_TRUE;
}

void CfgAppearanceDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgAppearanceDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_MENU_ANIMATION                   ))->SetCheck(aConfig.mAnimationMenu);
    ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_STANDARD_MENU_STYLE              ))->SetCheck(aConfig.mStandardMenu);
    ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_SINGLE_VIEW_SPLIT_AS_ACTIVED_VIEW))->SetCheck(aConfig.mSingleViewSplitAsActivedView);
    ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_VIEW_SPLIT_REOPEN_LAST_FOLDER    ))->SetCheck(aConfig.mViewSplitReopenLastFolder);
}

void CfgAppearanceDlg::onApply(Option::Config &aConfig)
{
    aConfig.mAnimationMenu                = ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_MENU_ANIMATION                         ))->GetCheck();
    aConfig.mStandardMenu                 = ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_STANDARD_MENU_STYLE                    ))->GetCheck();
    aConfig.mSingleViewSplitAsActivedView = ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_SINGLE_VIEW_SPLIT_AS_ACTIVED_VIEW      ))->GetCheck();
    aConfig.mViewSplitReopenLastFolder    = ((CButton *)GetDlgItem(IDC_CFG_APPEARANCE_VIEW_SPLIT_REOPEN_LAST_FOLDER          ))->GetCheck();
}
} // namespace cfg
} // namespace fxfile
