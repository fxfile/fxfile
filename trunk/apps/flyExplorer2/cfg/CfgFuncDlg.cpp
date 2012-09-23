//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncDlg::CfgFuncDlg(void)
    : super(IDD_CFG_FUNC, XPR_NULL)
{
}

void CfgFuncDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_LAST_FOLDER       ))->SetCheck(gOpt->mDriveLastFolder);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_SHIFT_KEY         ))->SetCheck(gOpt->mDriveShiftKey);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE    ))->SetCheck(gOpt->mAddressBarUrl);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_CLEAR_SEARCH_RESULT     ))->SetCheck(gOpt->mSearchResultClearOnClose);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SHELL_NEW               ))->SetCheck(gOpt->mShellNewMenu);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILELIST_CUR_DIR        ))->SetCheck(gOpt->mFileListCurDir);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH))->SetCheck(gOpt->mWorkingFolderRealPath);

    SetDlgItemText(IDC_CFG_FUNC_NAME_COPY_SEPARATOR, gOpt->mClipboardSeparator);
    ((CEdit *)GetDlgItem(IDC_CFG_FUNC_NAME_COPY_SEPARATOR))->LimitText(MAX_CLIP_SEPARATOR);

    SetDlgItemText(IDC_CFG_FUNC_DRIVE_LAST_FOLDER,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.last_folder_for_each_drive")));
    SetDlgItemText(IDC_CFG_FUNC_DRIVE_SHIFT_KEY,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.go_drive_with_shift_key")));
    SetDlgItemText(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.internet_address_compatible_on_address_bar")));
    SetDlgItemText(IDC_CFG_FUNC_CLEAR_SEARCH_RESULT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.clear_search_result_on_exit_working_bar")));
    SetDlgItemText(IDC_CFG_FUNC_SHELL_NEW,                 theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.new_items_such_as_windows_explorer")));
    SetDlgItemText(IDC_CFG_FUNC_FILELIST_CUR_DIR,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.default_current_dir_for_file_list")));
    SetDlgItemText(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.real_path_on_working_folder")));
    SetDlgItemText(IDC_CFG_FUNC_LABEL_NAME_COPY_SEPARATOR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.label.separator_on_name_copy")));

    return XPR_TRUE;
}

void CfgFuncDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mDriveLastFolder          = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_LAST_FOLDER       ))->GetCheck();
    gOpt->mDriveShiftKey            = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_SHIFT_KEY         ))->GetCheck();
    gOpt->mAddressBarUrl            = ((CButton *)GetDlgItem(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE    ))->GetCheck();
    gOpt->mSearchResultClearOnClose = ((CButton *)GetDlgItem(IDC_CFG_FUNC_CLEAR_SEARCH_RESULT     ))->GetCheck();
    gOpt->mShellNewMenu             = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SHELL_NEW               ))->GetCheck();
    gOpt->mFileListCurDir           = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILELIST_CUR_DIR        ))->GetCheck();
    gOpt->mWorkingFolderRealPath    = ((CButton *)GetDlgItem(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH))->GetCheck();

    GetDlgItemText(IDC_CFG_FUNC_NAME_COPY_SEPARATOR, gOpt->mClipboardSeparator, MAX_CLIP_SEPARATOR);
    if (_tcslen(gOpt->mClipboardSeparator) <= 0)
        _tcscpy(gOpt->mClipboardSeparator, XPR_STRING_LITERAL("\\r\\n"));

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}

xpr_bool_t CfgFuncDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_DRIVE_LAST_FOLDER:
        case IDC_CFG_FUNC_DRIVE_SHIFT_KEY:
        case IDC_CFG_FUNC_INET_ADDR_COMPATIBLE:
        case IDC_CFG_FUNC_CLEAR_SEARCH_RESULT:
        case IDC_CFG_FUNC_SHELL_NEW:
        case IDC_CFG_FUNC_FILELIST_CUR_DIR:
        case IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_NAME_COPY_SEPARATOR:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
