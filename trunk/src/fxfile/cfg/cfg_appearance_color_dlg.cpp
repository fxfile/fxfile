//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_color_dlg.h"

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
CfgAppearanceColorDlg::CfgAppearanceColorDlg(void)
    : super(IDD_CFG_APPEARANCE_COLOR, XPR_NULL)
    , mOldViewIndex(-1)
{
}

void CfgAppearanceColorDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_COLOR_FILE_LIST_TEXT_CUSTOM_COLOR,          mFileListTextCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FILE_LIST_BKGND_CUSTOM_COLOR,         mFileListBkgndCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_TEXT_CUSTOM_COLOR,        mFolderTreeTextCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_BKGND_CUSTOM_COLOR,       mFolderTreeBkgndCustomColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT_COLOR, mFolderTreeInactiveHighlightColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_ACTIVED_VIEW_COLOR,                   mActivedViewColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_INFO_BAR_BOOKMARK_COLOR,              mInfoBarBookmarkColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_PATH_BAR_HIGHLIGHT_COLOR,             mPathBarHighlightColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgAppearanceColorDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_COLOR_APPLY_ALL,                    OnApplyAll)
    ON_BN_CLICKED(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_BROWSE, OnFileListBkgndImageBrowse)
    ON_CBN_SELCHANGE(IDC_CFG_COLOR_VIEW,                      OnSelChangeView)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceColorDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_TEXT_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_BKGND_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_TEXT_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_BKGND_CUSTOM_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_BROWSE);
    addIgnoreApply(IDC_CFG_COLOR_ACTIVED_VIEW_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_INFO_BAR_BOOKMARK_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_PATH_BAR_HIGHLIGHT_COLOR);

    ((CEdit *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH))->LimitText(XPR_MAX_PATH);

    CComboBox *sFileListTextColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    sFileListTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.default")));
    sFileListTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.custom")));
    sFileListTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_text_color.filtering")));

    CComboBox *sFileListBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    sFileListBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.default")));
    sFileListBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.custom")));
    sFileListBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.file_list_background_color.filtering")));

    CComboBox *sFolderTreeTextColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    sFolderTreeTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.default")));
    sFolderTreeTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.custom")));
    sFolderTreeTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_text_color.filtering")));
    sFolderTreeTextColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.type.file_list")));

    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);
    sFolderTreeBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.default")));
    sFolderTreeBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.custom")));
    sFolderTreeBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.folder_tree_background_color.filtering")));
    sFolderTreeBkgndColorComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.type.file_list")));

    mFileListTextCustomColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFileListTextCustomColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFileListTextCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));

    mFileListBkgndCustomColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFileListBkgndCustomColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFileListBkgndCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));

    mFolderTreeTextCustomColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeTextCustomColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeTextCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));

    mFolderTreeBkgndCustomColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeBkgndCustomColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeBkgndCustomColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));

    mFolderTreeInactiveHighlightColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mFolderTreeInactiveHighlightColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mFolderTreeInactiveHighlightColorCtrl.SetDefaultColor(::GetSysColor(COLOR_HIGHLIGHT));

    mActivedViewColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mActivedViewColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mActivedViewColorCtrl.SetDefaultColor(DEF_ACTIVED_VIEW_COLOR);

    mInfoBarBookmarkColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mInfoBarBookmarkColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mInfoBarBookmarkColorCtrl.SetDefaultColor(DEF_INFO_BAR_BOOKMARK_COLOR);

    mPathBarHighlightColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mPathBarHighlightColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mPathBarHighlightColorCtrl.SetDefaultColor(DEF_PATH_BAR_HIGHLIGHT_COLOR);

    SetDlgItemText(IDC_CFG_COLOR_APPLY_ALL,                      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.button.apply_all")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FILE_LIST_TEXT_COLOR,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.file_list_text_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FILE_LIST_BKGND_COLOR,    gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.file_list_background_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FOLDER_TREE_TEXT_COLOR,   gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.folder_tree_text_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_FOLDER_TREE_BKGND_COLOR,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.label.folder_tree_background_color")));
    SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE,          gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.check.file_list_background_image")));
    SetDlgItemText(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.view.check.folder_tree_inactive_highlight")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_ACTIVED_VIEW_COLOR,       gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.label.actived_view_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_INFO_BAR_BOOKMARK_COLOR,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.label.info_bar_bookmark_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.check.path_bar_highlight_color")));

    return XPR_TRUE;
}

void CfgAppearanceColorDlg::OnDestroy(void)
{
    CComboBox *sViewColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sViewColorComboBox != XPR_NULL);

    xpr_sint_t i, sCount;
    ViewColor *sViewColor;

    sCount = sViewColorComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sViewColor = (ViewColor *)sViewColorComboBox->GetItemData(i);
        XPR_SAFE_DELETE(sViewColor);
    }

    sViewColorComboBox->ResetContent();

    super::OnDestroy();
}

void CfgAppearanceColorDlg::onInit(Option::Config &aConfig)
{
    xpr_sint_t i;
    xpr::tstring sStringId;
    xpr::tstring sViewText;
    ViewColor *pViewColor;
    CComboBox *sViewColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sViewColorComboBox != XPR_NULL);

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        pViewColor = new ViewColor;
        pViewColor->mExplorerTextColorType    = aConfig.mExplorerTextColorType[i];
        pViewColor->mExplorerTextColor        = aConfig.mExplorerTextColor[i];
        pViewColor->mExplorerBkgndColorType   = aConfig.mExplorerBkgndColorType[i];
        pViewColor->mExplorerBkgndColor       = aConfig.mExplorerBkgndColor[i];
        pViewColor->mFolderTreeTextColorType  = aConfig.mFolderTreeTextColorType[i];
        pViewColor->mFolderTreeTextColor      = aConfig.mFolderTreeTextColor[i];
        pViewColor->mFolderTreeBkgndColorType = aConfig.mFolderTreeBkgndColorType[i];
        pViewColor->mFolderTreeBkgndColor     = aConfig.mFolderTreeBkgndColor[i];
        pViewColor->mExplorerBkgndImage       = aConfig.mExplorerBkgndImage[i];
        pViewColor->mExplorerBkgndImagePath   = aConfig.mExplorerBkgndImagePath[i];
        pViewColor->mFolderTreeHighlightColor = aConfig.mFolderTreeHighlightColor[i];
        pViewColor->mFolderTreeHighlight      = aConfig.mFolderTreeHighlight[i];

        sStringId.format(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.combo.view%d"), i + 1);

        sViewColorComboBox->AddString(gApp.loadString(sStringId.c_str()));
        sViewColorComboBox->SetItemData(i, (DWORD_PTR)pViewColor);
    }

    sViewColorComboBox->SetCurSel(0);
    OnSelChangeView();

    mActivedViewColorCtrl.SetColor(aConfig.mActiveViewColor);
    mInfoBarBookmarkColorCtrl.SetColor(aConfig.mContentsBookmarkColor);
    mPathBarHighlightColorCtrl.SetColor(aConfig.mPathBarHighlightColor);
    ((CButton *)GetDlgItem(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR))->SetCheck(aConfig.mPathBarHighlight);
}

void CfgAppearanceColorDlg::onApply(Option::Config &aConfig)
{
    saveViewColor();

    xpr_sint_t i, sCount;
    ViewColor *sViewColor;
    CComboBox *sViewColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sViewColorComboBox != XPR_NULL);

    sCount = sViewColorComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sViewColor = (ViewColor *)sViewColorComboBox->GetItemData(i);

        XPR_ASSERT(sViewColor != XPR_NULL);

        aConfig.mExplorerTextColorType[i]    = sViewColor->mExplorerTextColorType;
        aConfig.mExplorerTextColor[i]        = sViewColor->mExplorerTextColor;
        aConfig.mExplorerBkgndColorType[i]   = sViewColor->mExplorerBkgndColorType;
        aConfig.mExplorerBkgndColor[i]       = sViewColor->mExplorerBkgndColor;
        aConfig.mFolderTreeTextColorType[i]  = sViewColor->mFolderTreeTextColorType;
        aConfig.mFolderTreeTextColor[i]      = sViewColor->mFolderTreeTextColor;
        aConfig.mFolderTreeBkgndColorType[i] = sViewColor->mFolderTreeBkgndColorType;
        aConfig.mFolderTreeBkgndColor[i]     = sViewColor->mFolderTreeBkgndColor;

        aConfig.mExplorerBkgndImage[i]       = sViewColor->mExplorerBkgndImage;
        _tcscpy(aConfig.mExplorerBkgndImagePath[i], sViewColor->mExplorerBkgndImagePath.c_str());

        aConfig.mFolderTreeHighlightColor[i] = sViewColor->mFolderTreeHighlightColor;
        aConfig.mFolderTreeHighlight[i]      = sViewColor->mFolderTreeHighlight;
    }

    aConfig.mActiveViewColor       = mActivedViewColorCtrl.GetColor();
    aConfig.mContentsBookmarkColor = mInfoBarBookmarkColorCtrl.GetColor();
    aConfig.mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetColor();
    aConfig.mPathBarHighlight      = ((CButton *)GetDlgItem(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR))->GetCheck();

    if (aConfig.mActiveViewColor       == CLR_DEFAULT) aConfig.mActiveViewColor       = mActivedViewColorCtrl.GetDefaultColor();
    if (aConfig.mContentsBookmarkColor == CLR_DEFAULT) aConfig.mContentsBookmarkColor = mInfoBarBookmarkColorCtrl.GetDefaultColor();
    if (aConfig.mPathBarHighlightColor == CLR_DEFAULT) aConfig.mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetDefaultColor();
}

void CfgAppearanceColorDlg::OnApplyAll(void)
{
    saveViewColor();

    CComboBox *sViewColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sViewColorComboBox != XPR_NULL);

    xpr_sint_t i, sCount;
    xpr_sint_t sCurSel;
    ViewColor *sCurViewColor;
    ViewColor *sViewColor;

    sCurSel = sViewColorComboBox->GetCurSel();
    sCurViewColor = (ViewColor *)sViewColorComboBox->GetItemData(sCurSel);

    XPR_ASSERT(sCurViewColor != XPR_NULL);

    sCount = sViewColorComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sCurSel)
        {
            continue;
        }

        sViewColor = (ViewColor *)sViewColorComboBox->GetItemData(i);

        XPR_ASSERT(sViewColor != XPR_NULL);

        *sViewColor = *sCurViewColor;
    }
}

void CfgAppearanceColorDlg::OnSelChangeView(void)
{
    CComboBox *sColorViewComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sColorViewComboBox != XPR_NULL);

    if (mOldViewIndex != -1)
    {
        ViewColor *sOldViewColor = (ViewColor *)sColorViewComboBox->GetItemData(mOldViewIndex);

        XPR_ASSERT(sOldViewColor != XPR_NULL);

        saveViewColor(*sOldViewColor);
    }

    xpr_sint_t sViewIndex = sColorViewComboBox->GetCurSel();
    ViewColor *sViewColor = (ViewColor *)sColorViewComboBox->GetItemData(sViewIndex);

    XPR_ASSERT(sViewColor != XPR_NULL);

    if (mOldViewIndex != sViewIndex)
    {
        loadViewColor(*sViewColor);

        mOldViewIndex = sViewIndex;
    }
}

void CfgAppearanceColorDlg::loadViewColor(const ViewColor &aViewColor)
{
    CComboBox *sFileListTextColorComboBox    = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    CComboBox *sFileListBkgndColorComboBox   = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    CComboBox *sFolderTreeTextColorComboBox  = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);

    XPR_ASSERT(sFileListTextColorComboBox    != XPR_NULL);
    XPR_ASSERT(sFileListBkgndColorComboBox   != XPR_NULL);
    XPR_ASSERT(sFolderTreeTextColorComboBox  != XPR_NULL);
    XPR_ASSERT(sFolderTreeBkgndColorComboBox != XPR_NULL);

    mFileListTextCustomColorCtrl.SetColor(aViewColor.mExplorerTextColor);
    mFileListBkgndCustomColorCtrl.SetColor(aViewColor.mExplorerBkgndColor);
    mFolderTreeTextCustomColorCtrl.SetColor(aViewColor.mFolderTreeTextColor);
    mFolderTreeBkgndCustomColorCtrl.SetColor(aViewColor.mFolderTreeBkgndColor);
    mFolderTreeInactiveHighlightColorCtrl.SetColor(aViewColor.mFolderTreeHighlightColor);

    xpr_sint_t sCurSel;

    sCurSel = 0;
    switch (aViewColor.mExplorerTextColorType)
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFileListTextColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aViewColor.mExplorerBkgndColorType)
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    default:                   sCurSel = 0; break;
    }
    sFileListBkgndColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aViewColor.mFolderTreeTextColorType)
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    case COLOR_TYPE_FILE_LIST: sCurSel = 3; break;
    default:                   sCurSel = 0; break;
    }
    sFolderTreeTextColorComboBox->SetCurSel(sCurSel);

    sCurSel = 0;
    switch (aViewColor.mFolderTreeBkgndColorType)
    {
    case COLOR_TYPE_CUSTOM:    sCurSel = 1; break;
    case COLOR_TYPE_FILTERING: sCurSel = 2; break;
    case COLOR_TYPE_FILE_LIST: sCurSel = 3; break;
    default:                   sCurSel = 0; break;
    }
    sFolderTreeBkgndColorComboBox->SetCurSel(sCurSel);

    ((CButton *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT))->SetCheck(aViewColor.mFolderTreeHighlight);
    ((CButton *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE         ))->SetCheck(aViewColor.mExplorerBkgndImage);

    SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, aViewColor.mExplorerBkgndImagePath.c_str());
}

void CfgAppearanceColorDlg::saveViewColor(ViewColor &aViewColor)
{
    xpr_sint_t sCurSel;
    CComboBox *sFileListTextColorComboBox    = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_TEXT_COLOR);
    CComboBox *sFileListBkgndColorComboBox   = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_COLOR);
    CComboBox *sFolderTreeTextColorComboBox  = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_TEXT_COLOR);
    CComboBox *sFolderTreeBkgndColorComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_BKGND_COLOR);

    sCurSel = sFileListTextColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aViewColor.mExplorerTextColorType = COLOR_TYPE_CUSTOM;    break;
    case 2:  aViewColor.mExplorerTextColorType = COLOR_TYPE_FILTERING; break;
    default: aViewColor.mExplorerTextColorType = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFileListBkgndColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aViewColor.mExplorerBkgndColorType = COLOR_TYPE_CUSTOM;    break;
    case 2:  aViewColor.mExplorerBkgndColorType = COLOR_TYPE_FILTERING; break;
    default: aViewColor.mExplorerBkgndColorType = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFolderTreeTextColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aViewColor.mFolderTreeTextColorType = COLOR_TYPE_CUSTOM;    break;
    case 2:  aViewColor.mFolderTreeTextColorType = COLOR_TYPE_FILTERING; break;
    case 3:  aViewColor.mFolderTreeTextColorType = COLOR_TYPE_FILE_LIST; break;
    default: aViewColor.mFolderTreeTextColorType = COLOR_TYPE_DEFAULT;   break;
    }

    sCurSel = sFolderTreeBkgndColorComboBox->GetCurSel();
    switch (sCurSel)
    {
    case 1:  aViewColor.mFolderTreeBkgndColorType = COLOR_TYPE_CUSTOM;    break;
    case 2:  aViewColor.mFolderTreeBkgndColorType = COLOR_TYPE_FILTERING; break;
    case 3:  aViewColor.mFolderTreeBkgndColorType = COLOR_TYPE_FILE_LIST; break;
    default: aViewColor.mFolderTreeBkgndColorType = COLOR_TYPE_DEFAULT;   break;
    }

    aViewColor.mExplorerTextColor        = mFileListTextCustomColorCtrl.GetColor();
    aViewColor.mExplorerBkgndColor       = mFileListBkgndCustomColorCtrl.GetColor();
    aViewColor.mFolderTreeTextColor      = mFolderTreeTextCustomColorCtrl.GetColor();
    aViewColor.mFolderTreeBkgndColor     = mFolderTreeBkgndCustomColorCtrl.GetColor();
    aViewColor.mFolderTreeHighlightColor = mFolderTreeInactiveHighlightColorCtrl.GetColor();
    aViewColor.mFolderTreeHighlight      = ((CButton *)GetDlgItem(IDC_CFG_COLOR_FOLDER_TREE_INACTIVE_HIGHLIGHT))->GetCheck();
    aViewColor.mExplorerBkgndImage       = ((CButton *)GetDlgItem(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE         ))->GetCheck();

    xpr_tchar_t sExplorerBkgndImagePath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, sExplorerBkgndImagePath, XPR_MAX_PATH);
    aViewColor.mExplorerBkgndImagePath = sExplorerBkgndImagePath;

    if (aViewColor.mExplorerTextColor        == CLR_DEFAULT) aViewColor.mExplorerTextColor        = mFileListTextCustomColorCtrl.GetDefaultColor();
    if (aViewColor.mExplorerBkgndColor       == CLR_DEFAULT) aViewColor.mExplorerBkgndColor       = mFileListBkgndCustomColorCtrl.GetDefaultColor();
    if (aViewColor.mFolderTreeTextColor      == CLR_DEFAULT) aViewColor.mFolderTreeTextColor      = mFolderTreeTextCustomColorCtrl.GetDefaultColor();
    if (aViewColor.mFolderTreeBkgndColor     == CLR_DEFAULT) aViewColor.mFolderTreeBkgndColor     = mFolderTreeBkgndCustomColorCtrl.GetDefaultColor();
    if (aViewColor.mFolderTreeHighlightColor == CLR_DEFAULT) aViewColor.mFolderTreeHighlightColor = mFolderTreeInactiveHighlightColorCtrl.GetDefaultColor();
}

void CfgAppearanceColorDlg::saveViewColor(void)
{
    CComboBox *sColorViewComboBox = (CComboBox *)GetDlgItem(IDC_CFG_COLOR_VIEW);

    XPR_ASSERT(sColorViewComboBox != XPR_NULL);

    xpr_sint_t sViewIndex = sColorViewComboBox->GetCurSel();
    ViewColor *sViewColor = (ViewColor *)sColorViewComboBox->GetItemData(sViewIndex);

    XPR_ASSERT(sViewColor != XPR_NULL);

    saveViewColor(*sViewColor);
}

void CfgAppearanceColorDlg::OnFileListBkgndImageBrowse(void)
{
    xpr_tchar_t sFilter[0x1ff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.bmp; *.jpg; *.gif)\0*.bmp; *.jpg; *.gif\0%s (*.bmp)\0*.bmp\0%s (*.jpg)\0*.jpg\0%s (*.gif)\0*.gif\0\0"),
        gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all_formats")), 
        gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.bitmap_files")), 
        gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.jpeg_files")));

    CFileDialogST sFileDialog(XPR_TRUE, XPR_NULL, XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, this);
    if (sFileDialog.DoModal() == IDOK)
    {
        SetDlgItemText(IDC_CFG_COLOR_FILE_LIST_BKGND_IMAGE_PATH, sFileDialog.m_ofn.lpstrFile);
        setModified();
    }
}

LRESULT CfgAppearanceColorDlg::OnSelEndOK(WPARAM aWParam, LPARAM aLParam)
{
    setModified();

    return 0;
}
} // namespace cfg
} // namespace fxfile
