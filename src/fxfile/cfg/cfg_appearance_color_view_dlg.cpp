//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_color_view_dlg.h"

#include "gui/FileDialogST.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceColorViewDlg::CfgAppearanceColorViewDlg(xpr_sint_t aViewIndex)
    : super(IDD_CFG_APPEARANCE_COLOR_VIEW, XPR_NULL)
    , mViewIndex(aViewIndex)
{
}

void CfgAppearanceColorViewDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_COLOR_FILE_LIST_TEXT_CUSTOM_COLOR,          mFileListTextCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FILE_LIST_BKGND_CUSTOM_COLOR,         mFileListBkgndCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_TEXT_CUSTOM_COLOR,        mFolderTreeTextCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_BKGND_CUSTOM_COLOR,       mFolderTreeBkgndCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT_COLOR, mFolderTreeInactiveHighlightColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgAppearanceColorViewDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_BROWSE, OnFileListBkgndImageBrowse)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceColorViewDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_TEXT_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_BKGND_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_TEXT_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_BKGND_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_BROWSE);

    ((CEdit *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH))->LimitText(XPR_MAX_PATH);

    CComboBox *sFileListTextColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    sFileListTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.default")));
    sFileListTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.custom")));
    sFileListTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.filtering")));

    CComboBox *sFileListBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    sFileListBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.default")));
    sFileListBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.custom")));
    sFileListBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.filtering")));

    CComboBox *sFolderTreeTextColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    sFolderTreeTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.default")));
    sFolderTreeTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.custom")));
    sFolderTreeTextColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.filtering")));

    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);
    sFolderTreeBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.default")));
    sFolderTreeBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.custom")));
    sFolderTreeBkgndColorComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.filtering")));

    SetDlgItemText(IDC_CFG_COLOR_LABEL_FILE_LIST_TEXT_COLOR,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.file_list_text_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FILE_LIST_BKGND_COLOR,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.file_list_background_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FOLDER_TREE_TEXT_COLOR,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.folder_tree_text_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FOLDER_TREE_BKGND_COLOR,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.folder_tree_background_color")));
    SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.check.file_list_background_image")));
    SetDlgItemText(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.check.folder_tree_inactive_highlight")));

    return XPR_TRUE;
}

void CfgAppearanceColorViewDlg::OnDestroy(void)
{

    super::OnDestroy();
}

void CfgAppearanceColorViewDlg::onInit(Option::Config &aConfig)
{
    CComboBox *sFileListTextColorComboBox    = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    CComboBox *sFileListBkgndColorComboBox   = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    CComboBox *sFolderTreeTextColorComboBox  = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);

    mFileListTextCustomColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFileListTextCustomColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFileListTextCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));
    mFileListTextCustomColorCtrl.SetColor(aConfig.mExplorerTextColor[mViewIndex]);

    mFileListBkgndCustomColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFileListBkgndCustomColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFileListBkgndCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));
    mFileListBkgndCustomColorCtrl.SetColor(aConfig.mExplorerBkgndColor[mViewIndex]);

    mFolderTreeTextCustomColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeTextCustomColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeTextCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));
    mFolderTreeTextCustomColorCtrl.SetColor(aConfig.mFolderTreeTextColor[mViewIndex]);

    mFolderTreeBkgndCustomColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeBkgndCustomColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeBkgndCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));
    mFolderTreeBkgndCustomColorCtrl.SetColor(aConfig.mFolderTreeBkgndColor[mViewIndex]);

    mFolderTreeInactiveHighlightColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeInactiveHighlightColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeInactiveHighlightColorCtrl.SetDefaultColor(::GetSysColor(COLOR_HIGHLIGHT));
    mFolderTreeInactiveHighlightColorCtrl.SetColor(aConfig.mFolderTreeHighlightColor[mViewIndex]);

    xpr_sint_t sCurSel;

    sCurSel = 0;
    switch (aConfig.mExplorerTextColorType[mViewIndex])
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFileListTextColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aConfig.mExplorerBkgndColorType[mViewIndex])
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFileListBkgndColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aConfig.mFolderTreeTextColorType[mViewIndex])
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFolderTreeTextColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aConfig.mFolderTreeBkgndColorType[mViewIndex])
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFolderTreeBkgndColorComboBox->SetCurSel(sCurSel);

    ((CButton *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT))->SetCheck(aConfig.mFolderTreeHighlight[mViewIndex]);
    ((CButton *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE         ))->SetCheck(aConfig.mExplorerBkgndImage[mViewIndex]);

    SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, aConfig.mExplorerBkgndImagePath[mViewIndex]);
}

void CfgAppearanceColorViewDlg::onApply(Option::Config &aConfig)
{
    xpr_sint_t sCurSel;
    CComboBox *sFileListTextColorComboBox    = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    CComboBox *sFileListBkgndColorComboBox   = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    CComboBox *sFolderTreeTextColorComboBox  = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);

    sCurSel = sFileListTextColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aConfig.mExplorerTextColorType[mViewIndex] = COLOR_TYPE_CUSTOM;    break;
    case 2:  aConfig.mExplorerTextColorType[mViewIndex] = COLOR_TYPE_FILTERING; break;
    default: aConfig.mExplorerTextColorType[mViewIndex] = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFileListBkgndColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aConfig.mExplorerBkgndColorType[mViewIndex] = COLOR_TYPE_CUSTOM;    break;
    case 2:  aConfig.mExplorerBkgndColorType[mViewIndex] = COLOR_TYPE_FILTERING; break;
    default: aConfig.mExplorerBkgndColorType[mViewIndex] = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFolderTreeTextColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aConfig.mFolderTreeTextColorType[mViewIndex] = COLOR_TYPE_CUSTOM;    break;
    case 2:  aConfig.mFolderTreeTextColorType[mViewIndex] = COLOR_TYPE_FILTERING; break;
    default: aConfig.mFolderTreeTextColorType[mViewIndex] = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFolderTreeBkgndColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aConfig.mFolderTreeBkgndColorType[mViewIndex] = COLOR_TYPE_CUSTOM;    break;
    case 2:  aConfig.mFolderTreeBkgndColorType[mViewIndex] = COLOR_TYPE_FILTERING; break;
    default: aConfig.mFolderTreeBkgndColorType[mViewIndex] = COLOR_TYPE_DEFAULT;   break;
    }

    aConfig.mExplorerTextColor[mViewIndex]        = mFileListTextCustomColorCtrl.GetColor();
    aConfig.mExplorerBkgndColor[mViewIndex]       = mFileListBkgndCustomColorCtrl.GetColor();
    aConfig.mFolderTreeTextColor[mViewIndex]      = mFolderTreeTextCustomColorCtrl.GetColor();
    aConfig.mFolderTreeBkgndColor[mViewIndex]     = mFolderTreeBkgndCustomColorCtrl.GetColor();
    aConfig.mFolderTreeHighlightColor[mViewIndex] = mFolderTreeInactiveHighlightColorCtrl.GetColor();
    aConfig.mFolderTreeHighlight[mViewIndex]      = ((CButton *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT))->GetCheck();
    aConfig.mExplorerBkgndImage[mViewIndex]       = ((CButton *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE         ))->GetCheck();

    GetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, aConfig.mExplorerBkgndImagePath[mViewIndex], XPR_MAX_PATH);

    if (aConfig.mExplorerTextColor[mViewIndex]        == CLR_DEFAULT) aConfig.mExplorerTextColor[mViewIndex]        = mFileListTextCustomColorCtrl.GetDefaultColor();
    if (aConfig.mExplorerBkgndColor[mViewIndex]       == CLR_DEFAULT) aConfig.mExplorerBkgndColor[mViewIndex]       = mFileListBkgndCustomColorCtrl.GetDefaultColor();
    if (aConfig.mFolderTreeTextColor[mViewIndex]      == CLR_DEFAULT) aConfig.mFolderTreeTextColor[mViewIndex]      = mFolderTreeTextCustomColorCtrl.GetDefaultColor();
    if (aConfig.mFolderTreeBkgndColor[mViewIndex]     == CLR_DEFAULT) aConfig.mFolderTreeBkgndColor[mViewIndex]     = mFolderTreeBkgndCustomColorCtrl.GetDefaultColor();
    if (aConfig.mFolderTreeHighlightColor[mViewIndex] == CLR_DEFAULT) aConfig.mFolderTreeHighlightColor[mViewIndex] = mFolderTreeInactiveHighlightColorCtrl.GetDefaultColor();
}

LRESULT CfgAppearanceColorViewDlg::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    setModified();

    return XPR_TRUE;
}

void CfgAppearanceColorViewDlg::OnFileListBkgndImageBrowse(void)
{
    xpr_tchar_t sFilter[0x1ff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.bmp; *.jpg; *.gif)\0*.bmp; *.jpg; *.gif\0%s (*.bmp)\0*.bmp\0%s (*.jpg)\0*.jpg\0%s (*.gif)\0*.gif\0\0"),
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all_formats")), 
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.bitmap_files")), 
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.jpeg_files")));

    CFileDialogST sFileDialog(XPR_TRUE, XPR_NULL, XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, this);
    if (sFileDialog.DoModal() == IDOK)
    {
        SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, sFileDialog.m_ofn.lpstrFile);
        setModified();
    }
}
} // namespace cfg
} // namespace fxfile
