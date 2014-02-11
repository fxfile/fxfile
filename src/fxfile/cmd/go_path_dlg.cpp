//
// Copyright (c) 2001-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "go_path_dlg.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
GoPathDlg::GoPathDlg(void)
    : super(IDD_GO_PATH, XPR_NULL)
    , mEnableFile(XPR_FALSE), mEnableUrl(XPR_FALSE), mNewTab(XPR_FALSE)
    , mPidl1(XPR_NULL), mPidl2(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mPath[0] = 0;
}

GoPathDlg::~GoPathDlg(void)
{
    COM_FREE(mPidl1);
    COM_FREE(mPidl2);
}

void GoPathDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GO_PATH_PATH, mComboBox);
}

BEGIN_MESSAGE_MAP(GoPathDlg, super)
    ON_BN_CLICKED(IDC_GO_PATH_PATH_BROWSE, OnGoPathBrowse)
    ON_BN_CLICKED(IDC_GO_PATH_EXP1_PATH,   OnBnClickedExp1Path)
    ON_BN_CLICKED(IDC_GO_PATH_EXP2_PATH,   OnBnClickedExp2Path)
    ON_BN_CLICKED(IDC_NEW_TAB,             OnNewTab)
END_MESSAGE_MAP()

xpr_bool_t GoPathDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("GoPath"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Path"), mComboBox.GetDlgCtrlID());
        mDlgState->setCheckBox    (XPR_STRING_LITERAL("File"), IDC_GO_PATH_FILE);
        mDlgState->setCheckBox    (XPR_STRING_LITERAL("URL"),  IDC_GO_PATH_URL);
        mDlgState->load();
    }

    if (mComboBox.GetCount() > 0)
        mComboBox.SetCurSel(0);

    mComboBox.LimitText(XPR_MAX_PATH);

    COMBOBOXINFO sComboBoxInfo = {0};
    sComboBoxInfo.cbSize = sizeof(COMBOBOXINFO);
    mComboBox.GetComboBoxInfo(&sComboBoxInfo);

    ::SHAutoComplete(sComboBoxInfo.hwndItem, SHACF_FILESYSTEM);

    mEnableFile = ((CButton *)GetDlgItem(IDC_GO_PATH_FILE))->GetCheck();
    mEnableUrl  = ((CButton *)GetDlgItem(IDC_GO_PATH_URL ))->GetCheck();

    GetDlgItem(IDC_GO_PATH_EXP1_PATH)->EnableWindow(mPidl1 ? XPR_TRUE : XPR_FALSE);
    GetDlgItem(IDC_GO_PATH_EXP2_PATH)->EnableWindow(mPidl2 ? XPR_TRUE : XPR_FALSE);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.go_path.title")));
    SetDlgItemText(IDC_GO_PATH_LABEL_INPUT, gApp.loadString(XPR_STRING_LITERAL("popup.go_path.label.input")));
    SetDlgItemText(IDC_GO_PATH_EXP1_PATH,   gApp.loadString(XPR_STRING_LITERAL("popup.go_path.button.window_1_folder")));
    SetDlgItemText(IDC_GO_PATH_EXP2_PATH,   gApp.loadString(XPR_STRING_LITERAL("popup.go_path.button.window_2_folder")));
    SetDlgItemText(IDC_GO_PATH_FILE,        gApp.loadString(XPR_STRING_LITERAL("popup.go_path.check.file_execution")));
    SetDlgItemText(IDC_GO_PATH_URL,         gApp.loadString(XPR_STRING_LITERAL("popup.go_path.check.internet_address_compatible")));
    SetDlgItemText(IDC_NEW_TAB,             gApp.loadString(XPR_STRING_LITERAL("popup.go_path.button.new_tab")));
    SetDlgItemText(IDOK,                    gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void GoPathDlg::OnOK(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    mComboBox.GetWindowText(sPath, XPR_MAX_PATH);

    // Save Command
    DlgState::insertComboEditString(&mComboBox, XPR_FALSE);

    _tcscpy(mPath, sPath);

    mEnableFile = ((CButton *)GetDlgItem(IDC_GO_PATH_FILE))->GetCheck();
    mEnableUrl  = ((CButton *)GetDlgItem(IDC_GO_PATH_URL ))->GetCheck();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    super::OnOK();
}

void GoPathDlg::OnNewTab(void)
{
    mNewTab = XPR_TRUE;

    OnOK();
}

void GoPathDlg::setExpPath(LPITEMIDLIST aPidl1, LPITEMIDLIST aPidl2)
{
    mPidl1 = fxfile::base::Pidl::clone(aPidl1);
    mPidl2 = fxfile::base::Pidl::clone(aPidl2);
}

void GoPathDlg::getPath(xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        _tcscpy(aPath, mPath);
}

xpr_bool_t GoPathDlg::isEnableFile(void) const
{
    return mEnableFile;
}

xpr_bool_t GoPathDlg::isEnableUrl(void) const
{
    return mEnableUrl;
}

xpr_bool_t GoPathDlg::isNewTab(void) const
{
    return mNewTab;
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void GoPathDlg::OnGoPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    mComboBox.GetWindowText(sPath, XPR_MAX_PATH);

    if (_tcslen(sPath) == 2 && sPath[1] == XPR_STRING_LITERAL(':'))
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

    LPITEMIDLIST sOldFullPidl = Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.go_path.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;

    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t szNewPath[XPR_MAX_PATH + 1] = {0};
        GetName(sFullPidl, SHGDN_FORPARSING, szNewPath);

        if (_tcsicmp(sOldPath, szNewPath))
        {
            GetName(sFullPidl, SHGDN_FORPARSING, sPath);
            mComboBox.SetWindowText(sPath);
        }
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void GoPathDlg::OnBnClickedExp1Path(void)
{
    xpr::tstring strPath;
    if (IsParentVirtualItem(mPidl1) == XPR_TRUE)
        GetFullPath(mPidl1, strPath);
    else
        GetName(mPidl1, SHGDN_FORPARSING, strPath);

    mComboBox.SetWindowText(strPath.c_str());
}

void GoPathDlg::OnBnClickedExp2Path(void)
{
    xpr::tstring strPath;
    if (IsParentVirtualItem(mPidl2) == XPR_TRUE)
        GetFullPath(mPidl2, strPath);
    else
        GetName(mPidl2, SHGDN_FORPARSING, strPath);

    mComboBox.SetWindowText(strPath.c_str());
}
} // namespace cmd
} // namespace fxfile
