//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_folder_layout_dlg.h"

#include "gui/FileDialogST.h"

#include "../option.h"
#include "../resource.h"
#include "../folder_layout_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceFolderLayoutDlg::CfgAppearanceFolderLayoutDlg(void)
    : super(IDD_CFG_APPEARANCE_FOLDER_LAYOUT, XPR_NULL)
{
}

void CfgAppearanceFolderLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceFolderLayoutDlg, super)
    ON_BN_CLICKED(IDC_CFG_FOLDERLAYOUT_CLEAN, OnClean)
    ON_BN_CLICKED(IDC_CFG_FOLDERLAYOUT_INIT,  OnInit)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceFolderLayoutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_FOLDERLAYOUT_CLEAN);
    addIgnoreApply(IDC_CFG_FOLDERLAYOUT_INIT);

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_VIEW_STYLE);
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.extra_large_icons")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.large_icons")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.medium_icons")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.small_icons")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.list")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.details")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.view_style.thumbnail")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT);
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort.name")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort.size")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort.type")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort.date")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort.attr")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT_ORDER);
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort_order.ascending")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.sort_order.decending")));

    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_GROUP_FOLDER_LAYOUT,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.group.folder_layout")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_NONE,                     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.radio.none")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_DEFAULT,                  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.radio.default")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_EACH_FOLDER,              gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.radio.each_folder")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_LABEL_DEFAULT_VIEW_STYLE, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.label.default_view_style")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_LABEL_DEFAULT_SORT,       gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.label.default_sort")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_LABEL_DEFAULT_SORT_ORDER, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.label.default_sort_order")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_NO_SORT,                  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.check.no_sort")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_AUTO_COLUMN_WIDTH,        gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.check.automatic_column_width")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_CLEAN_ON_EXIT,            gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.check.clean_on_exit")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_CLEAN,                    gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.button.clean")));
    SetDlgItemText(IDC_CFG_FOLDERLAYOUT_INIT,                     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.button.reset")));

    return XPR_TRUE;
}

void CfgAppearanceFolderLayoutDlg::onInit(const Option::Config &aConfig)
{
    xpr_sint_t sCurSelForViewStyle = getCurSelFromViewStyle(aConfig.mFileListDefaultViewStyle);

    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_NONE              ))->SetCheck(aConfig.mFileListSaveFolderLayout == SAVE_FOLDER_LAYOUT_NONE);
    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT           ))->SetCheck(aConfig.mFileListSaveFolderLayout == SAVE_FOLDER_LAYOUT_DEFAULT);
    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_EACH_FOLDER       ))->SetCheck(aConfig.mFileListSaveFolderLayout == SAVE_FOLDER_LAYOUT_EACH_FOLDER);
    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_NO_SORT           ))->SetCheck(aConfig.mFileListNoSort);
    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_AUTO_COLUMN_WIDTH ))->SetCheck(aConfig.mFileListAutoColumnWidth);
    ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_CLEAN_ON_EXIT     ))->SetCheck(aConfig.mFileListExitVerifyFolderLayout);
    ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_VIEW_STYLE))->SetCurSel(sCurSelForViewStyle);
    ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT      ))->SetCurSel(aConfig.mFileListDefaultSort);
    ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT_ORDER))->SetCurSel(aConfig.mFileListDefaultSortOrder);
}

void CfgAppearanceFolderLayoutDlg::onApply(Option::Config &aConfig)
{
    aConfig.mFileListSaveFolderLayout = SAVE_FOLDER_LAYOUT_DEFAULT;
    if (((CButton *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_NONE))->GetCheck())
        aConfig.mFileListSaveFolderLayout = SAVE_FOLDER_LAYOUT_NONE;
    else if (((CButton *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_EACH_FOLDER))->GetCheck())
        aConfig.mFileListSaveFolderLayout = SAVE_FOLDER_LAYOUT_EACH_FOLDER;

    xpr_sint_t sCurSelForViewStyle = ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_VIEW_STYLE))->GetCurSel();

    aConfig.mFileListNoSort                 = ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_NO_SORT           ))->GetCheck();
    aConfig.mFileListAutoColumnWidth        = ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_AUTO_COLUMN_WIDTH ))->GetCheck();
    aConfig.mFileListExitVerifyFolderLayout = ((CButton   *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_CLEAN_ON_EXIT     ))->GetCheck();
    aConfig.mFileListDefaultViewStyle       = getViewStyleFromCurSel(sCurSelForViewStyle);
    aConfig.mFileListDefaultSort            = ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT      ))->GetCurSel();
    aConfig.mFileListDefaultSortOrder       = ((CComboBox *)GetDlgItem(IDC_CFG_FOLDERLAYOUT_DEFAULT_SORT_ORDER))->GetCurSel();
}

xpr_sint_t CfgAppearanceFolderLayoutDlg::getCurSelFromViewStyle(xpr_sint_t aViewStyle)
{
    xpr_sint_t sCurSel;

    switch (aViewStyle)
    {
    case VIEW_STYLE_EXTRA_LARGE_ICONS: sCurSel =  0; break;
    case VIEW_STYLE_LARGE_ICONS:       sCurSel =  1; break;
    case VIEW_STYLE_MEDIUM_ICONS:      sCurSel =  2; break;
    case VIEW_STYLE_SMALL_ICONS:       sCurSel =  3; break;
    case VIEW_STYLE_LIST:              sCurSel =  4; break;
    case VIEW_STYLE_DETAILS:           sCurSel =  5; break;
    case VIEW_STYLE_THUMBNAIL:         sCurSel =  6; break;
    default:                           sCurSel = -1; break;
    }

    return sCurSel;
}

xpr_sint_t CfgAppearanceFolderLayoutDlg::getViewStyleFromCurSel(xpr_sint_t aCurSel)
{
    xpr_sint_t sViewStyle;

    switch (aCurSel)
    {
    case 0:  sViewStyle = VIEW_STYLE_EXTRA_LARGE_ICONS; break;
    case 1:  sViewStyle = VIEW_STYLE_LARGE_ICONS;       break;
    case 2:  sViewStyle = VIEW_STYLE_MEDIUM_ICONS;      break;
    case 3:  sViewStyle = VIEW_STYLE_SMALL_ICONS;       break;
    case 4:  sViewStyle = VIEW_STYLE_LIST;              break;
    case 5:  sViewStyle = VIEW_STYLE_DETAILS;           break;
    case 6:  sViewStyle = VIEW_STYLE_THUMBNAIL;         break;
    default: sViewStyle = VIEW_STYLE_DETAILS;           break;
    }

    return sViewStyle;
}

void CfgAppearanceFolderLayoutDlg::OnClean(void)
{
    FolderLayoutManager &sFolderLayoutManager = SingletonManager::get<FolderLayoutManager>();
    sFolderLayoutManager.verify();

    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.msg.completed_clean"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgAppearanceFolderLayoutDlg::OnInit(void)
{
    const xpr_tchar_t *sMsg;

    sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.msg.confirm_reset"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    FolderLayoutManager &sFolderLayoutManager = SingletonManager::get<FolderLayoutManager>();
    sFolderLayoutManager.clear();

    sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.folder_layout.msg.completed_reset"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}
} // namespace cfg
} // namespace fxfile
