//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "GoPathDlg.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GoPathDlg::GoPathDlg(void)
    : super(IDD_GO_PATH, XPR_NULL)
    , mUrl(XPR_FALSE)
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
END_MESSAGE_MAP()

xpr_bool_t GoPathDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("GoPath"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Path"), mComboBox.GetDlgCtrlID());
        mDlgState->setCheckBox(XPR_STRING_LITERAL("URL"), IDC_GO_PATH_URL);
        mDlgState->load();
    }

    if (mComboBox.GetCount() > 0)
        mComboBox.SetCurSel(0);

    mComboBox.LimitText(XPR_MAX_PATH);

    COMBOBOXINFO sComboBoxInfo = {0};
    sComboBoxInfo.cbSize = sizeof(COMBOBOXINFO);
    mComboBox.GetComboBoxInfo(&sComboBoxInfo);

    ::SHAutoComplete(sComboBoxInfo.hwndItem, SHACF_FILESYSTEM);

    mUrl = ((CButton *)GetDlgItem(IDC_GO_PATH_URL))->GetCheck();

    GetDlgItem(IDC_GO_PATH_EXP1_PATH)->EnableWindow(mPidl1 ? XPR_TRUE : XPR_FALSE);
    GetDlgItem(IDC_GO_PATH_EXP2_PATH)->EnableWindow(mPidl2 ? XPR_TRUE : XPR_FALSE);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.go_path.title")));
    SetDlgItemText(IDC_GO_PATH_LABEL_INPUT, theApp.loadString(XPR_STRING_LITERAL("popup.go_path.label.input")));
    SetDlgItemText(IDC_GO_PATH_EXP1_PATH,   theApp.loadString(XPR_STRING_LITERAL("popup.go_path.button.window_1_folder")));
    SetDlgItemText(IDC_GO_PATH_EXP2_PATH,   theApp.loadString(XPR_STRING_LITERAL("popup.go_path.button.window_2_folder")));
    SetDlgItemText(IDC_GO_PATH_URL,         theApp.loadString(XPR_STRING_LITERAL("popup.go_path.check.internet_address_compatible")));
    SetDlgItemText(IDOK,                    theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void GoPathDlg::OnOK(void) 
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    mComboBox.GetWindowText(sPath, XPR_MAX_PATH);

    // Save Command
    DlgState::insertComboEditString(&mComboBox, XPR_FALSE);

    _tcscpy(mPath, sPath);
    mUrl = ((CButton *)GetDlgItem(IDC_GO_PATH_URL))->GetCheck();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    super::OnOK();
}

void GoPathDlg::setExpPath(LPITEMIDLIST aPidl1, LPITEMIDLIST aPidl2)
{
    mPidl1 = fxb::CopyItemIDList(aPidl1);
    mPidl2 = fxb::CopyItemIDList(aPidl2);
}

void GoPathDlg::getPath(xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        _tcscpy(aPath, mPath);
}

xpr_bool_t GoPathDlg::isEnableUrl(void)
{
    return mUrl;
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

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.go_path.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;

    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t szNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, szNewPath);

        if (_tcsicmp(sOldPath, szNewPath))
        {
            fxb::GetName(sFullPidl, SHGDN_FORPARSING, sPath);
            mComboBox.SetWindowText(sPath);
        }
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void GoPathDlg::OnBnClickedExp1Path(void)
{
    std::tstring strPath;
    if (fxb::IsParentVirtualItem(mPidl1) == XPR_TRUE)
        fxb::GetFullPath(mPidl1, strPath);
    else
        fxb::GetName(mPidl1, SHGDN_FORPARSING, strPath);

    mComboBox.SetWindowText(strPath.c_str());
}

void GoPathDlg::OnBnClickedExp2Path(void)
{
    std::tstring strPath;
    if (fxb::IsParentVirtualItem(mPidl2) == XPR_TRUE)
        fxb::GetFullPath(mPidl2, strPath);
    else
        fxb::GetName(mPidl2, SHGDN_FORPARSING, strPath);

    mComboBox.SetWindowText(strPath.c_str());
}
