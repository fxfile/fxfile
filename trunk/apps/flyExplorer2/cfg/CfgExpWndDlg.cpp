//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpWndDlg.h"

#include "../rgc/FileDialogST.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgExpWndDlg::CfgExpWndDlg(xpr_size_t aIndex)
    : super(IDD_CFG_EXP_WND, XPR_NULL)
    , mIndex(aIndex)
{
}

void CfgExpWndDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_EXP_WND_TEXT_COLOR, mTextColorCtrl);
    DDX_Control(pDX, IDC_CFG_EXP_WND_BKGND_COLOR, mBkgndColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgExpWndDlg, super)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_CUSTOMIZE_FOLDER,   OnCustomizeFolder)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_BKGND_IMAGE_BROWSE, OnBkgndImageBrowse)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_INIT_FOLDER_BROWSE, OnInitFolderBrowse)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_INIT_FOLDER,        OnInitFolder)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_INIT_FOLDER_NONE,   OnInitFolderNone)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_INIT_LAST_FOLDER,   OnInitLastFolder)
    ON_BN_CLICKED(IDC_CFG_EXP_WND_NO_LAST_NET_FOLDER, OnNoLastNetFolder)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgExpWndDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_CUSTOMIZE_FOLDER))->SetCheck(gOpt->mExplorerCustomFolder[mIndex]);
    setActive(gOpt->mExplorerCustomFolder[mIndex]);

    SetDlgItemText(IDC_CFG_EXP_WND_BKGND_IMAGE_PATH, gOpt->mExplorerBkgndFile[mIndex]);

    mTextColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mTextColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mTextColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));
    mTextColorCtrl.SetColor(gOpt->mExplorerTextColor[mIndex]);

    mBkgndColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mBkgndColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mBkgndColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOW));
    mBkgndColorCtrl.SetColor(gOpt->mExplorerBkgndColor[mIndex]);

    OnCustomizeFolder();

    if (gOpt->mExplorerInitFolderType[mIndex] == INIT_TYPE_NONE)
        ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER_NONE))->SetCheck(XPR_TRUE);
    else if (gOpt->mExplorerInitFolderType[mIndex] == INIT_TYPE_INIT_FOLDER)
        ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER))->SetCheck(XPR_TRUE);
    else
        ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_LAST_FOLDER))->SetCheck(XPR_TRUE);

    ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_NO_LAST_NET_FOLDER))->SetCheck(gOpt->mExplorerNoNetLastFolder[mIndex]);

    SetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER_PATH, gOpt->mExplorerInitFolder[mIndex]);
    ::SHAutoComplete(GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER_PATH)->m_hWnd, SHACF_FILESYSTEM);
    OnInitFolder();

    ((CEdit *)GetDlgItem(IDC_CFG_EXP_WND_BKGND_IMAGE_PATH))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER_PATH))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_CFG_EXP_WND_CUSTOMIZE_FOLDER,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.check.customize_folder")));
    SetDlgItemText(IDC_CFG_EXP_WND_LABEL_BKGND_IMAGE,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.label.background_image")));
    SetDlgItemText(IDC_CFG_EXP_WND_LABEL_TEXT_COLOR,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.label.text_color")));
    SetDlgItemText(IDC_CFG_EXP_WND_LABEL_TEXT_COLOR_DESC,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.label.text_color_desc")));
    SetDlgItemText(IDC_CFG_EXP_WND_LABEL_BKGND_COLOR,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.label.background_color")));
    SetDlgItemText(IDC_CFG_EXP_WND_LABEL_BKGND_COLOR_DESC, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.label.background_color_desc")));
    SetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER_NONE,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.radio.init_folder_none")));
    SetDlgItemText(IDC_CFG_EXP_WND_INIT_LAST_FOLDER,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.radio.init_last_folder")));
    SetDlgItemText(IDC_CFG_EXP_WND_NO_LAST_NET_FOLDER,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.check.no_last_network_folder")));
    SetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.radio.init_folder")));

    return XPR_TRUE;
}

void CfgExpWndDlg::setActive(xpr_bool_t aActive)
{
    GetDlgItem(IDC_CFG_EXP_WND_LABEL_BKGND_IMAGE     )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_BKGND_IMAGE_PATH      )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_BKGND_IMAGE_BROWSE    )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_LABEL_TEXT_COLOR      )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_TEXT_COLOR            )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_LABEL_TEXT_COLOR_DESC )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_LABEL_BKGND_COLOR     )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_BKGND_COLOR           )->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_EXP_WND_LABEL_BKGND_COLOR_DESC)->EnableWindow(aActive);
}

LRESULT CfgExpWndDlg::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    setModified();
    return XPR_TRUE;
}

void CfgExpWndDlg::OnCustomizeFolder(void) 
{
    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_CUSTOMIZE_FOLDER))->GetCheck();
    setActive(sEnable);
    setModified();
}

void CfgExpWndDlg::OnBkgndImageBrowse(void) 
{
    xpr_tchar_t sFilter[0x1ff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.bmp; *.jpg; *.gif)\0*.bmp; *.jpg; *.gif\0%s (*.bmp)\0*.bmp\0%s (*.jpg)\0*.jpg\0%s (*.gif)\0*.gif\0\0"),
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all_formats")), 
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.bitmap_files")), 
        theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.jpeg_files")));

    CFileDialogST sFileDialog(XPR_TRUE, XPR_NULL, XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, this);
    if (sFileDialog.DoModal() == IDOK)
    {
        SetDlgItemText(IDC_CFG_EXP_WND_BKGND_IMAGE_PATH, sFileDialog.m_ofn.lpstrFile);
        setModified();
    }
}

void CfgExpWndDlg::OnInitFolderNone(void) 
{
    OnInitFolder();
    setModified();
}

void CfgExpWndDlg::OnInitLastFolder(void) 
{
    OnInitFolder();
    setModified();
}

void CfgExpWndDlg::OnNoLastNetFolder(void)
{
    setModified();
}

void CfgExpWndDlg::OnInitFolder(void) 
{
    xpr_bool_t sEnable = XPR_TRUE;
    xpr_bool_t sInitFolder = ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER))->GetCheck();
    if (XPR_IS_FALSE(sInitFolder))
        sEnable = XPR_FALSE;

    GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER_PATH  )->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER_BROWSE)->EnableWindow(sEnable);

    sEnable = XPR_TRUE;
    xpr_bool_t sLastFolder = ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_LAST_FOLDER))->GetCheck();
    if (XPR_IS_FALSE(sLastFolder))
        sEnable = XPR_FALSE;

    GetDlgItem(IDC_CFG_EXP_WND_NO_LAST_NET_FOLDER)->EnableWindow(sEnable);

    setModified();
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgExpWndDlg::OnInitFolderBrowse(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER_PATH, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.view.msg.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_tchar_t szNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, szNewPath);

        if (_tcsicmp(sOldPath, szNewPath) != 0)
        {
            sPath[0] = '\0';
            fxb::Pidl2Path(sFullPidl, sPath);
            SetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER_PATH, sPath);

            setModified();
        }
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

xpr_bool_t CfgExpWndDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_WND_BKGND_IMAGE_PATH:
        case IDC_CFG_EXP_WND_INIT_FOLDER_PATH:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgExpWndDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mExplorerCustomFolder[mIndex]    = ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_CUSTOMIZE_FOLDER))->GetCheck();
    gOpt->mExplorerBkgndColor[mIndex]      = mBkgndColorCtrl.GetColor();
    gOpt->mExplorerTextColor[mIndex]       = mTextColorCtrl.GetColor();
    gOpt->mExplorerNoNetLastFolder[mIndex] = ((CButton *)GetDlgItem(IDC_CFG_EXP_WND_NO_LAST_NET_FOLDER))->GetCheck();

    GetDlgItemText(IDC_CFG_EXP_WND_BKGND_IMAGE_PATH, gOpt->mExplorerBkgndFile[mIndex], XPR_MAX_PATH);

    if (gOpt->mExplorerBkgndColor[mIndex] == CLR_DEFAULT) gOpt->mExplorerBkgndColor[mIndex] = mBkgndColorCtrl.GetDefaultColor();
    if (gOpt->mExplorerTextColor[mIndex]  == CLR_DEFAULT) gOpt->mExplorerTextColor[mIndex] = mTextColorCtrl.GetDefaultColor();

    if (((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_FOLDER))->GetCheck())
        gOpt->mExplorerInitFolderType[mIndex] = INIT_TYPE_INIT_FOLDER;
    else if (((CButton *)GetDlgItem(IDC_CFG_EXP_WND_INIT_LAST_FOLDER))->GetCheck())
        gOpt->mExplorerInitFolderType[mIndex] = INIT_TYPE_LAST_FOLDER;
    else
        gOpt->mExplorerInitFolderType[mIndex] = INIT_TYPE_NONE;

    GetDlgItemText(IDC_CFG_EXP_WND_INIT_FOLDER_PATH, gOpt->mExplorerInitFolder[mIndex], XPR_MAX_PATH);

    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
    sOptionMgr.applyEtc(2);
}
