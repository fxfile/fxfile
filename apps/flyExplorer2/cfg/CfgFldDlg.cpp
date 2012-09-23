//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFldDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFldDlg::CfgFldDlg(void)
    : super(IDD_CFG_FLD, XPR_NULL)
{
}

void CfgFldDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFldDlg, super)
    ON_BN_CLICKED(IDC_CFG_FLD_CUSTOM_FONT_TEXT_BROWSE, OnCustomFontTextBrowse)
    ON_BN_CLICKED(IDC_CFG_FLD_ITEM_HEIGHT,             OnItemHeight)
    ON_BN_CLICKED(IDC_CFG_FLD_EXPLORE_DELAY,           OnExploreDelay)
END_MESSAGE_MAP()

xpr_bool_t CfgFldDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CSpinButtonCtrl *sSpinCtrl;

    ((CEdit *)GetDlgItem(IDC_CFG_FLD_CUSTOM_FONT_TEXT))->LimitText(MAX_FONT_TEXT);

    ((CButton *)GetDlgItem(IDC_CFG_FLD_CUSTOM_FONT))->SetCheck(gOpt->mFolderTreeCustomFont);
    ((CButton *)GetDlgItem(IDC_CFG_FLD_INIT_NO_EXPAND))->SetCheck(gOpt->mFolderTreeInitNoExpand);
    ((CButton *)GetDlgItem(IDC_CFG_FLD_SINGLE_FOLDER_PANE_LINKAGE))->SetCheck(gOpt->mSingleFolderTreeLinkage);

    SetDlgItemText(IDC_CFG_FLD_CUSTOM_FONT_TEXT, gOpt->mFolderTreeCustomFontText);

    ((CButton *)GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT))->SetCheck(gOpt->mFolderTreeIsItemHeight ? 1 : 0);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT_SIZE_SPIN);
    sSpinCtrl->SetRange(MIN_FOLDER_TREE_HEIGHT, MAX_FOLDER_TREE_HEIGHT);
    SetDlgItemInt(IDC_CFG_FLD_ITEM_HEIGHT_SIZE, gOpt->mFolderTreeItemHeight, XPR_TRUE);
    OnItemHeight();

    ((CButton *)GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY))->SetCheck(gOpt->mFolderTreeSelDelay);
    SetDlgItemInt(IDC_CFG_FLD_EXPLORE_DELAY_TIME, gOpt->mFolderTreeSelDelayTime);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY_TIME_SPIN);
    sSpinCtrl->SetRange(MIN_FLD_SEL_DELAY_MSEC, MAX_FLD_SEL_DELAY_MSEC);
    OnExploreDelay();

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText,
        theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.label.explore_delay_time_range"), XPR_STRING_LITERAL("%d,%d,%d")),
        MIN_FLD_SEL_DELAY_MSEC, MAX_FLD_SEL_DELAY_MSEC, DEF_FLD_SEL_DELAY_MSEC);
    SetDlgItemText(IDC_CFG_FLD_LABEL_EXPLORE_DELAY_TIME_RANGE, sText);

    SetDlgItemText(IDC_CFG_FLD_CUSTOM_FONT,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.check.custom_font")));
    SetDlgItemText(IDC_CFG_FLD_ITEM_HEIGHT,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.check.tree_item_height")));
    SetDlgItemText(IDC_CFG_FLD_INIT_NO_EXPAND,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.check.init_no_expand")));
    SetDlgItemText(IDC_CFG_FLD_EXPLORE_DELAY,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.check.explore_delay")));
    SetDlgItemText(IDC_CFG_FLD_SINGLE_FOLDER_PANE_LINKAGE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.check.single_folder_pane_linkage")));

    return XPR_TRUE;
}

void CfgFldDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mFolderTreeCustomFont    = ((CButton *)GetDlgItem(IDC_CFG_FLD_CUSTOM_FONT   ))->GetCheck();
    gOpt->mFolderTreeInitNoExpand  = ((CButton *)GetDlgItem(IDC_CFG_FLD_INIT_NO_EXPAND))->GetCheck();
    gOpt->mFolderTreeIsItemHeight  = ((CButton *)GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT   ))->GetCheck();
    gOpt->mFolderTreeSelDelay      = ((CButton *)GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY ))->GetCheck();
    gOpt->mSingleFolderTreeLinkage = ((CButton *)GetDlgItem(IDC_CFG_FLD_SINGLE_FOLDER_PANE_LINKAGE))->GetCheck();
    gOpt->mFolderTreeItemHeight    = GetDlgItemInt(IDC_CFG_FLD_ITEM_HEIGHT_SIZE);
    gOpt->mFolderTreeSelDelayTime  = GetDlgItemInt(IDC_CFG_FLD_EXPLORE_DELAY_TIME);

    if (gOpt->mFolderTreeItemHeight < MIN_FOLDER_TREE_HEIGHT) gOpt->mFolderTreeItemHeight = MIN_FOLDER_TREE_HEIGHT;
    if (gOpt->mFolderTreeItemHeight > MAX_FOLDER_TREE_HEIGHT) gOpt->mFolderTreeItemHeight = MAX_FOLDER_TREE_HEIGHT;

    if (gOpt->mFolderTreeSelDelayTime < MIN_FLD_SEL_DELAY_MSEC) gOpt->mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;
    if (gOpt->mFolderTreeSelDelayTime > MAX_FLD_SEL_DELAY_MSEC) gOpt->mFolderTreeSelDelayTime = DEF_FLD_SEL_DELAY_MSEC;

    GetDlgItemText(IDC_CFG_FLD_CUSTOM_FONT_TEXT, gOpt->mFolderTreeCustomFontText, MAX_FONT_TEXT);

    sOptionMgr.applyFolderCtrl(0, XPR_FALSE);
    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgFldDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FLD_CUSTOM_FONT:
        case IDC_CFG_FLD_ITEM_HEIGHT:
        case IDC_CFG_FLD_INIT_NO_EXPAND:
        case IDC_CFG_FLD_EXPLORE_DELAY:
        case IDC_CFG_FLD_SINGLE_FOLDER_PANE_LINKAGE:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_FLD_CUSTOM_FONT_TEXT:
        case IDC_CFG_FLD_ITEM_HEIGHT_SIZE:
        case IDC_CFG_FLD_EXPLORE_DELAY_TIME:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgFldDlg::OnCustomFontTextBrowse(void) 
{
    xpr_tchar_t sFont[MAX_FONT_TEXT + 1] = {0};
    GetDlgItemText(IDC_CFG_FLD_CUSTOM_FONT_TEXT, sFont, MAX_FONT_TEXT);

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

        SetDlgItemText(IDC_CFG_FLD_CUSTOM_FONT_TEXT, sText);

        setModified();
    }
}

void CfgFldDlg::OnItemHeight(void) 
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT))->GetCheck();
    GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT_SIZE)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_FLD_ITEM_HEIGHT_SIZE_SPIN)->EnableWindow(sEnable);
}

void CfgFldDlg::OnExploreDelay(void)
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY))->GetCheck();
    GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY_TIME)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_FLD_EXPLORE_DELAY_TIME_SPIN)->EnableWindow(sEnable);
}
