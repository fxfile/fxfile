//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "sel_folder_dlg.h"

#include "resource.h"
#include "resource.h"
#include "dlg_state.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SelFolderDlg::SelFolderDlg(void)
    : super(IDD_SEL_FOLDER, XPR_NULL)
{
}

void SelFolderDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FOLDER, mComboBox);
}

BEGIN_MESSAGE_MAP(SelFolderDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
END_MESSAGE_MAP()

xpr_bool_t SelFolderDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mComboBox.LimitText(XPR_MAX_PATH);

    COMBOBOXINFO sComboBoxInfo = {0};
    sComboBoxInfo.cbSize = sizeof(COMBOBOXINFO);
    mComboBox.GetComboBoxInfo(&sComboBoxInfo);

    ::SHAutoComplete(sComboBoxInfo.hwndItem, SHACF_FILESYSTEM);

    if (mFolder.empty() == false)
        mComboBox.SetWindowText(mFolder.c_str());

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.select_folder.title")));
    SetDlgItemText(IDC_SEL_FOLDER_DESC, theApp.loadString(XPR_STRING_LITERAL("popup.select_folder.label.desc")));

    return XPR_TRUE;
}

void SelFolderDlg::OnDestroy(void) 
{
    super::OnDestroy();

}

void SelFolderDlg::setFolder(const xpr_tchar_t *aFolder)
{
    if (aFolder != XPR_NULL)
        mFolder = aFolder;
}

const xpr_tchar_t *SelFolderDlg::getSelFolder(void)
{
    return mFolder.c_str();
}

void SelFolderDlg::OnOK(void) 
{
    xpr_tchar_t sFolder[XPR_MAX_PATH + 1] = {0};
    if (mComboBox.GetWindowText(sFolder, XPR_MAX_PATH) <= 0)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.select_folder.msg.wrong_name"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        mComboBox.SetFocus();
        return;
    }

    DlgState::insertComboEditString(&mComboBox, XPR_FALSE);

    mFolder = sFolder;

    super::OnOK();
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void SelFolderDlg::OnBrowse(void)
{
    xpr_tchar_t sFolder[XPR_MAX_PATH + 1] = {0};
    mComboBox.GetWindowText(sFolder, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sFolder);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.select_folder.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
        {
            mComboBox.SetWindowText(sNewPath);
            DlgState::insertComboEditString(&mComboBox, XPR_FALSE);
        }
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}
