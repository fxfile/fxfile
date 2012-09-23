//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SchLocEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SchLocEditDlg::SchLocEditDlg(void)
    : super(IDD_SEARCH_LOC_EDIT, XPR_NULL)
    , mInclude(XPR_TRUE)
    , mSubFolder(XPR_FALSE)
    , mSchUserLocPathSet(XPR_NULL)
{
}

SchLocEditDlg::~SchLocEditDlg(void)
{
}

void SchLocEditDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SchLocEditDlg, super)
    ON_BN_CLICKED(IDC_SEARCH_LOC_EDIT_INCLUDE, OnIncludeExclude)
    ON_BN_CLICKED(IDC_SEARCH_LOC_EDIT_EXCLUDE, OnIncludeExclude)
    ON_BN_CLICKED(IDC_SEARCH_LOC_EDIT_PATH_BROWSE, OnPathBrowse)
END_MESSAGE_MAP()

xpr_bool_t SchLocEditDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CEdit *)GetDlgItem(IDC_SEARCH_LOC_EDIT_PATH))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_SEARCH_LOC_EDIT_PATH, mPath.c_str());

    ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_INCLUDE))->SetCheck( mInclude);
    ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_EXCLUDE))->SetCheck(!mInclude);
    ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_SUBFOLDER))->SetCheck(mSubFolder);

    ::SHAutoComplete(GetDlgItem(IDC_SEARCH_LOC_EDIT_PATH)->m_hWnd, SHACF_FILESYSTEM);

    OnIncludeExclude();

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.title")));
    SetDlgItemText(IDC_SEARCH_LOC_EDIT_INCLUDE,    theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.check.include")));
    SetDlgItemText(IDC_SEARCH_LOC_EDIT_EXCLUDE,    theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.check.exclude")));
    SetDlgItemText(IDC_SEARCH_LOC_EDIT_LABEL_PATH, theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.label.exclude_path")));
    SetDlgItemText(IDC_SEARCH_LOC_EDIT_SUBFOLDER,  theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.check.exclude_sub-folder")));
    SetDlgItemText(IDOK,                           theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void SchLocEditDlg::OnOK(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_SEARCH_LOC_EDIT_PATH, sPath, XPR_MAX_PATH);

    if (_tcslen(sPath) == 2)
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

    if (mSchUserLocPathSet != XPR_NULL)
    {
        xpr_tchar_t sPathUpper[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sPathUpper, sPath);
        _tcsupr(sPathUpper);

        if (mSchUserLocPathSet->find(sPathUpper) != mSchUserLocPathSet->end())
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.msg.duplicated_path"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_SEARCH_LOC_EDIT_PATH)->SetFocus();
            return;
        }
    }

    xpr_bool_t sInclude   = ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_INCLUDE))->GetCheck();
    xpr_bool_t sSubFolder = ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_SUBFOLDER))->GetCheck();

    mPath      = sPath;
    mInclude   = sInclude;
    mSubFolder = sSubFolder;

    super::OnOK();
}

const xpr_tchar_t *SchLocEditDlg::getPath(void) const
{
    return mPath.c_str();
}

xpr_bool_t SchLocEditDlg::isInclude(void) const
{
    return mInclude;
}

xpr_bool_t SchLocEditDlg::isSubFolder(void) const
{
    return mSubFolder;
}

void SchLocEditDlg::setPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPath = aPath;
}

void SchLocEditDlg::setPath(LPITEMIDLIST aFullPidl)
{
    if (aFullPidl == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

    mPath = sPath;
}

void SchLocEditDlg::setInclude(xpr_bool_t aInclude)
{
    mInclude = aInclude;
}

void SchLocEditDlg::setSubFolder(xpr_bool_t aSubFolder)
{
    mSubFolder = aSubFolder;
}

void SchLocEditDlg::setPathSet(fxb::SchUserLocPathSet *aSchUserLocPathSet)
{
    mSchUserLocPathSet = aSchUserLocPathSet;
}

void SchLocEditDlg::OnIncludeExclude(void)
{
    xpr_bool_t sInclude = ((CButton *)GetDlgItem(IDC_SEARCH_LOC_EDIT_INCLUDE))->GetCheck();
    GetDlgItem(IDC_SEARCH_LOC_EDIT_SUBFOLDER)->EnableWindow(sInclude);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void SchLocEditDlg::OnPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_SEARCH_LOC_EDIT_PATH, sPath, XPR_MAX_PATH);

    if (_tcslen(sPath) == 2)
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

    LPITEMIDLIST sOldFullPidl = fxb::SHGetPidlFromPath(sPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location_edit.exclude_path_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;

    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sPath);
        SetDlgItemText(IDC_SEARCH_LOC_EDIT_PATH, sPath);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}
