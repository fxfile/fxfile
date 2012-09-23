//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFldWndDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFldWndDlg::CfgFldWndDlg(xpr_size_t aIndex)
    : super(IDD_CFG_FLD_WND, XPR_NULL)
    , mIndex(aIndex)
{
}

void CfgFldWndDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_FLD_WND_TEXT_COLOR,               mTextColorCtrl);
    DDX_Control(pDX, IDC_CFG_FLD_WND_BKGND_COLOR,              mBkgndColorCtrl);
    DDX_Control(pDX, IDC_CFG_FLD_WND_INACTIVE_HIGHLIGHT_COLOR, mHighlightColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgFldWndDlg, super)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
    ON_BN_CLICKED(IDC_CFG_FLD_WND_CUSTOM_NONE,     OnCustomFolder)
    ON_BN_CLICKED(IDC_CFG_FLD_WND_CUSTOM_EXPLORER, OnCustomFolder)
    ON_BN_CLICKED(IDC_CFG_FLD_WND_CUSTOM_FOLDER,   OnCustomFolder)
END_MESSAGE_MAP()

xpr_bool_t CfgFldWndDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_NONE    ))->SetCheck(gOpt->mFolderTreeCustomFolder[mIndex] == FOLDER_TREE_CUSTOM_NONE);
    ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_EXPLORER))->SetCheck(gOpt->mFolderTreeCustomFolder[mIndex] == FOLDER_TREE_CUSTOM_EXPLORER);
    ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_FOLDER  ))->SetCheck(gOpt->mFolderTreeCustomFolder[mIndex] == FOLDER_TREE_CUSTOM_USER_DEFINED);

    mTextColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mTextColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mTextColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));
    mTextColorCtrl.SetColor(gOpt->mFolderTreeTextColor[mIndex]);

    mBkgndColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mBkgndColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mBkgndColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));
    mBkgndColorCtrl.SetColor(gOpt->mFolderTreeBkgndColor[mIndex]);

    mHighlightColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mHighlightColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mHighlightColorCtrl.SetDefaultColor(::GetSysColor(COLOR_HIGHLIGHT));
    mHighlightColorCtrl.SetColor(gOpt->mFolderTreeHighlightColor[mIndex]);

    ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_INACTIVE_HIGHLIGHT))->SetCheck(gOpt->mFolderTreeHighlight[mIndex]);

    OnCustomFolder();

    SetDlgItemText(IDC_CFG_FLD_WND_GROUP_CUSTOM,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.group.custom")));
    SetDlgItemText(IDC_CFG_FLD_WND_CUSTOM_NONE,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.radio.custom_none")));
    SetDlgItemText(IDC_CFG_FLD_WND_CUSTOM_EXPLORER,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.radio.custom_explorer")));
    SetDlgItemText(IDC_CFG_FLD_WND_CUSTOM_FOLDER,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.radio.custom_folder")));
    SetDlgItemText(IDC_CFG_FLD_WND_LABEL_TEXT_COLOR,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.label.text_color")));
    SetDlgItemText(IDC_CFG_FLD_WND_LABEL_BKGND_COLOR,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.label.background_color")));
    SetDlgItemText(IDC_CFG_FLD_WND_INACTIVE_HIGHLIGHT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.folder_pane.view.check.inactive_highlight")));

    return XPR_TRUE;
}

void CfgFldWndDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mFolderTreeCustomFolder[mIndex] = FOLDER_TREE_CUSTOM_NONE;
    if (((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_EXPLORER))->GetCheck()) gOpt->mFolderTreeCustomFolder[mIndex] = FOLDER_TREE_CUSTOM_EXPLORER;
    if (((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_FOLDER))->GetCheck())   gOpt->mFolderTreeCustomFolder[mIndex] = FOLDER_TREE_CUSTOM_USER_DEFINED;

    gOpt->mFolderTreeBkgndColor[mIndex]     = mBkgndColorCtrl.GetColor();
    gOpt->mFolderTreeTextColor[mIndex]      = mTextColorCtrl.GetColor();
    gOpt->mFolderTreeHighlightColor[mIndex] = mHighlightColorCtrl.GetColor();

    if (gOpt->mFolderTreeBkgndColor[mIndex]     == CLR_DEFAULT) gOpt->mFolderTreeBkgndColor[mIndex]     = mBkgndColorCtrl.GetDefaultColor();
    if (gOpt->mFolderTreeTextColor[mIndex]      == CLR_DEFAULT) gOpt->mFolderTreeTextColor[mIndex]      = mTextColorCtrl.GetDefaultColor();
    if (gOpt->mFolderTreeHighlightColor[mIndex] == CLR_DEFAULT) gOpt->mFolderTreeHighlightColor[mIndex] = mHighlightColorCtrl.GetDefaultColor();

    gOpt->mFolderTreeHighlight[mIndex] = ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_INACTIVE_HIGHLIGHT_COLOR))->GetCheck();

    sOptionMgr.applyFolderCtrl(0, XPR_FALSE);
    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgFldWndDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FLD_WND_INACTIVE_HIGHLIGHT_COLOR:
        case IDC_CFG_FLD_WND_CUSTOM_NONE:
        case IDC_CFG_FLD_WND_CUSTOM_EXPLORER:
        case IDC_CFG_FLD_WND_CUSTOM_FOLDER:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

LRESULT CfgFldWndDlg::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    setModified();
    return XPR_TRUE;
}

void CfgFldWndDlg::OnCustomFolder(void)
{
    xpr_bool_t sCustomFolder = ((CButton *)GetDlgItem(IDC_CFG_FLD_WND_CUSTOM_FOLDER))->GetCheck();

    mBkgndColorCtrl.EnableWindow(sCustomFolder);
    mTextColorCtrl.EnableWindow(sCustomFolder);
}
