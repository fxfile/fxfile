//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_font_dlg.h"

#include "../gui/gdi.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceFontDlg::CfgAppearanceFontDlg(void)
    : super(IDD_CFG_APPEARANCE_FONT, XPR_NULL)
{
}

void CfgAppearanceFontDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceFontDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_FONT_CUSTOM_FONT_TEXT_BROWSE,             OnCustomFontTextBrowse)
    ON_BN_CLICKED(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT_BROWSE, OnFolderTreeCustomFontTextBrowse)
    ON_BN_CLICKED(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT,             OnFolderTreeItemHeight)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceFontDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_FONT_CUSTOM_FONT_TEXT_BROWSE);
    addIgnoreApply(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT_BROWSE);

    CSpinButtonCtrl *sSpinCtrl;

    ((CEdit *)GetDlgItem(IDC_CFG_FONT_CUSTOM_FONT_TEXT            ))->LimitText(MAX_FONT_TEXT);
    ((CEdit *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT))->LimitText(MAX_FONT_TEXT);

    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT_SIZE_SPIN);
    sSpinCtrl->SetRange(MIN_FOLDER_TREE_HEIGHT, MAX_FOLDER_TREE_HEIGHT);

    SetDlgItemText(IDC_CFG_FONT_CUSTOM_FONT,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.font.check.custom_font")));
    SetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.font.check.folder_tree_custom_font")));
    SetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.font.check.folder_tree_item_height")));

    return XPR_TRUE;
}

void CfgAppearanceFontDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgAppearanceFontDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_FONT_CUSTOM_FONT            ))->SetCheck(aConfig.mCustomFont);
    ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT))->SetCheck(aConfig.mFolderTreeCustomFont);
    ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT))->SetCheck(aConfig.mFolderTreeIsItemHeight);
    ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT))->SetCheck(aConfig.mFolderTreeIsItemHeight ? 1 : 0);

    SetDlgItemText(IDC_CFG_FONT_CUSTOM_FONT_TEXT,             aConfig.mCustomFontText);
    SetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT, aConfig.mFolderTreeCustomFontText);
    SetDlgItemInt (IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT_SIZE, aConfig.mFolderTreeItemHeight, XPR_TRUE);
    OnFolderTreeItemHeight();
}

void CfgAppearanceFontDlg::onApply(Option::Config &aConfig)
{
    aConfig.mCustomFont             = ((CButton *)GetDlgItem(IDC_CFG_FONT_CUSTOM_FONT            ))->GetCheck();
    aConfig.mFolderTreeCustomFont   = ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT))->GetCheck();
    aConfig.mFolderTreeIsItemHeight = ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT))->GetCheck();
    aConfig.mFolderTreeItemHeight   = GetDlgItemInt(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT_SIZE);

    GetDlgItemText(IDC_CFG_FONT_CUSTOM_FONT_TEXT,             aConfig.mCustomFontText,           MAX_FONT_TEXT);
    GetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT, aConfig.mFolderTreeCustomFontText, MAX_FONT_TEXT);

    if (aConfig.mFolderTreeItemHeight < MIN_FOLDER_TREE_HEIGHT) aConfig.mFolderTreeItemHeight = MIN_FOLDER_TREE_HEIGHT;
    if (aConfig.mFolderTreeItemHeight > MAX_FOLDER_TREE_HEIGHT) aConfig.mFolderTreeItemHeight = MAX_FOLDER_TREE_HEIGHT;
}

void CfgAppearanceFontDlg::OnCustomFontTextBrowse(void) 
{
    xpr_tchar_t sFont[MAX_FONT_TEXT + 1] = {0};
    GetDlgItemText(IDC_CFG_FONT_CUSTOM_FONT_TEXT, sFont, MAX_FONT_TEXT);

    LOGFONT sLogFont = {0};
    StringToLogFont(sFont, sLogFont);

    CFontDialog sDlg;
    sDlg.m_cf.Flags &= ~CF_EFFECTS;
    sDlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    memcpy(sDlg.m_cf.lpLogFont, &sLogFont, sizeof(LOGFONT));
    if (sDlg.DoModal() == IDOK)
    {
        xpr_tchar_t sText[MAX_FONT_TEXT + 1] = {0};
        FontDlgToString(sDlg.GetFaceName(), sDlg.GetSize(), sDlg.GetWeight(), sDlg.IsItalic(), sDlg.IsStrikeOut(), sDlg.IsUnderline(), sText);

        SetDlgItemText(IDC_CFG_FONT_CUSTOM_FONT_TEXT, sText);

        setModified();
    }
}

void CfgAppearanceFontDlg::OnFolderTreeCustomFontTextBrowse(void) 
{
    xpr_tchar_t sFont[MAX_FONT_TEXT + 1] = {0};
    GetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT, sFont, MAX_FONT_TEXT);

    LOGFONT sLogFont = {0};
    StringToLogFont(sFont, sLogFont);

    CFontDialog sDlg;
    sDlg.m_cf.Flags &= ~CF_EFFECTS;
    sDlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    memcpy(sDlg.m_cf.lpLogFont, &sLogFont, sizeof(LOGFONT));
    if (sDlg.DoModal() == IDOK)
    {
        xpr_tchar_t sText[MAX_FONT_TEXT + 1] = {0};
        FontDlgToString(sDlg.GetFaceName(), sDlg.GetSize(), sDlg.GetWeight(), sDlg.IsItalic(), sDlg.IsStrikeOut(), sDlg.IsUnderline(), sText);

        SetDlgItemText(IDC_CFG_FONT_FOLDER_TREE_CUSTOM_FONT_TEXT, sText);

        setModified();
    }
}

void CfgAppearanceFontDlg::OnFolderTreeItemHeight(void) 
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT))->GetCheck();

    GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT_SIZE     )->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_FONT_FOLDER_TREE_ITEM_HEIGHT_SIZE_SPIN)->EnableWindow(sEnable);
}
} // namespace cfg
} // namespace fxfile
