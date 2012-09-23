//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgDispDlg.h"

#include "../Option.h"
#include "../resource.h"
#include "../PathBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgDispDlg::CfgDispDlg(void)
    : super(IDD_CFG_DISP, XPR_NULL)
{
}

void CfgDispDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_DISP_ACTIVE_VIEW_COLOR,       mActivateColorCtrl);
    DDX_Control(pDX, IDC_CFG_DISP_PATHBAR_HIGHLIGHT_COLOR, mPathBarHighlightColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgDispDlg, super)
    ON_BN_CLICKED(IDC_CFG_DISP_CUSTOM_FONT_TEXT_BROWSE, OnCustomFontTextBrowse)
    ON_BN_CLICKED(IDC_CFG_DISP_PATHBAR_HIGHLIGHT,       OnPathBarHighlight)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgDispDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    //GetSysColor(COLOR_ACTIVECAPTION)
    mActivateColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mActivateColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mActivateColorCtrl.SetDefaultColor(RGB(48,192,48));
    mActivateColorCtrl.SetColor(gOpt->mActiveViewColor);

    mPathBarHighlightColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mPathBarHighlightColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mPathBarHighlightColorCtrl.SetDefaultColor(PathBar::getDefaultHighlightColor());
    mPathBarHighlightColorCtrl.SetColor(gOpt->mPathBarHighlightColor);

    ((CButton *)GetDlgItem(IDC_CFG_DISP_CUSTOM_FONT        ))->SetCheck(gOpt->mCustomFont);
    ((CButton *)GetDlgItem(IDC_CFG_DISP_MENU_ANIMATION     ))->SetCheck(gOpt->mAnimationMenu);
    ((CButton *)GetDlgItem(IDC_CFG_DISP_STANDARD_MENU_STYLE))->SetCheck(gOpt->mStandardMenu);
    ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_REAL_PATH  ))->SetCheck(gOpt->mPathBarRealPath);
    ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_ICON       ))->SetCheck(gOpt->mPathBarIcon);
    ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_HIGHLIGHT  ))->SetCheck(gOpt->mPathBarHighlight);

    SetDlgItemText(IDC_CFG_DISP_CUSTOM_FONT_TEXT, gOpt->mCustomFontText);

    ((CEdit *)GetDlgItem(IDC_CFG_DISP_CUSTOM_FONT_TEXT))->LimitText(MAX_FONT_TEXT);

    OnPathBarHighlight();

    SetDlgItemText(IDC_CFG_DISP_CUSTOM_FONT,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.custom_font")));
    SetDlgItemText(IDC_CFG_DISP_MENU_ANIMATION,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.menu_animation")));
    SetDlgItemText(IDC_CFG_DISP_STANDARD_MENU_STYLE,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.standard_style_menu")));
    SetDlgItemText(IDC_CFG_DISP_LABEL_ACTIVE_VIEW_COLOR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.label.active_color")));
    SetDlgItemText(IDC_CFG_DISP_GROUP_PATHBAR,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.group.pathbar")));
    SetDlgItemText(IDC_CFG_DISP_PATHBAR_ICON,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.pathbar_icon")));
    SetDlgItemText(IDC_CFG_DISP_PATHBAR_REAL_PATH,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.pathbar_real_path")));
    SetDlgItemText(IDC_CFG_DISP_PATHBAR_HIGHLIGHT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.check.pathbar_highlight")));

    return XPR_TRUE;
}

void CfgDispDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mActiveViewColor       = mActivateColorCtrl.GetColor();
    gOpt->mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetColor();

    if (gOpt->mActiveViewColor       == CLR_DEFAULT) gOpt->mActiveViewColor       = mActivateColorCtrl.GetDefaultColor();
    if (gOpt->mPathBarHighlightColor == CLR_DEFAULT) gOpt->mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetDefaultColor();

    gOpt->mCustomFont       = ((CButton *)GetDlgItem(IDC_CFG_DISP_CUSTOM_FONT        ))->GetCheck();
    gOpt->mAnimationMenu    = ((CButton *)GetDlgItem(IDC_CFG_DISP_MENU_ANIMATION     ))->GetCheck();
    gOpt->mStandardMenu     = ((CButton *)GetDlgItem(IDC_CFG_DISP_STANDARD_MENU_STYLE))->GetCheck();
    gOpt->mPathBarRealPath  = ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_REAL_PATH  ))->GetCheck();
    gOpt->mPathBarIcon      = ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_ICON       ))->GetCheck();
    gOpt->mPathBarHighlight = ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_HIGHLIGHT  ))->GetCheck();

    GetDlgItemText(IDC_CFG_DISP_CUSTOM_FONT_TEXT, gOpt->mCustomFontText, MAX_FONT_TEXT);

    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

LRESULT CfgDispDlg::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    setModified();
    return XPR_TRUE;
}

xpr_bool_t CfgDispDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_DISP_CUSTOM_FONT:
        case IDC_CFG_DISP_MENU_ANIMATION:
        case IDC_CFG_DISP_STANDARD_MENU_STYLE:
        case IDC_CFG_DISP_PATHBAR_REAL_PATH:
        case IDC_CFG_DISP_PATHBAR_ICON:
        case IDC_CFG_DISP_PATHBAR_HIGHLIGHT:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_DISP_CUSTOM_FONT_TEXT:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgDispDlg::OnCustomFontTextBrowse(void) 
{
    xpr_tchar_t sFont[MAX_FONT_TEXT + 1] = {0};
    GetDlgItemText(IDC_CFG_DISP_CUSTOM_FONT_TEXT, sFont, MAX_FONT_TEXT);

    LOGFONT sLogFont = {0};
    OptionMgr::instance().StringToLogFont(sFont, sLogFont);

    CFontDialog sDlg;
    sDlg.m_cf.Flags &= ~CF_EFFECTS;
    sDlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    memcpy(sDlg.m_cf.lpLogFont, &sLogFont, sizeof(LOGFONT));
    if (sDlg.DoModal() == IDOK)
    {
        xpr_tchar_t sText[MAX_FONT_TEXT + 1] = {0};
        OptionMgr::instance().FontDlgToString(sDlg, sText);

        SetDlgItemText(IDC_CFG_DISP_CUSTOM_FONT_TEXT, sText);

        setModified();
    }
}

void CfgDispDlg::OnPathBarHighlight(void)
{
    xpr_bool_t sHighlight = ((CButton *)GetDlgItem(IDC_CFG_DISP_PATHBAR_HIGHLIGHT))->GetCheck();
    mPathBarHighlightColorCtrl.EnableWindow(sHighlight);
}
