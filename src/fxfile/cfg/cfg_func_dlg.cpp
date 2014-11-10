//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_func_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
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

    SetDlgItemText(IDC_CFG_FUNC_DRIVE_LAST_FOLDER,         gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.last_folder_for_each_drive")));
    SetDlgItemText(IDC_CFG_FUNC_DRIVE_SHIFT_KEY,           gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.go_drive_with_shift_key")));
    SetDlgItemText(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.internet_address_compatible_on_address_bar")));
    SetDlgItemText(IDC_CFG_FUNC_SHELL_NEW,                 gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.new_items_such_as_windows_explorer")));
    SetDlgItemText(IDC_CFG_FUNC_FILELIST_CUR_DIR,          gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.default_current_dir_for_file_list")));
    SetDlgItemText(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.real_path_on_working_folder")));
    SetDlgItemText(IDC_CFG_FUNC_EDIT_AFTER_CREATING_TEXT,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.check.edit_after_creating_text_file")));
    SetDlgItemText(IDC_CFG_FUNC_LABEL_NAME_COPY_SEPARATOR, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.label.separator_on_name_copy")));

    return XPR_TRUE;
}

void CfgFuncDlg::onInit(const Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_LAST_FOLDER       ))->SetCheck(aConfig.mDriveLastFolder);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_SHIFT_KEY         ))->SetCheck(aConfig.mDriveShiftKey);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE    ))->SetCheck(aConfig.mAddressBarUrl);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_SHELL_NEW               ))->SetCheck(aConfig.mShellNewMenu);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILELIST_CUR_DIR        ))->SetCheck(aConfig.mFileListCurDir);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH))->SetCheck(aConfig.mWorkingFolderRealPath);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_EDIT_AFTER_CREATING_TEXT))->SetCheck(aConfig.mFileListCreateAndEditText);

    SetDlgItemText(IDC_CFG_FUNC_NAME_COPY_SEPARATOR, aConfig.mClipboardSeparator);
    ((CEdit *)GetDlgItem(IDC_CFG_FUNC_NAME_COPY_SEPARATOR))->LimitText(MAX_CLIP_SEPARATOR);
}

void CfgFuncDlg::onApply(Option::Config &aConfig)
{
    aConfig.mDriveLastFolder           = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_LAST_FOLDER       ))->GetCheck();
    aConfig.mDriveShiftKey             = ((CButton *)GetDlgItem(IDC_CFG_FUNC_DRIVE_SHIFT_KEY         ))->GetCheck();
    aConfig.mAddressBarUrl             = ((CButton *)GetDlgItem(IDC_CFG_FUNC_INET_ADDR_COMPATIBLE    ))->GetCheck();
    aConfig.mShellNewMenu              = ((CButton *)GetDlgItem(IDC_CFG_FUNC_SHELL_NEW               ))->GetCheck();
    aConfig.mFileListCurDir            = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILELIST_CUR_DIR        ))->GetCheck();
    aConfig.mWorkingFolderRealPath     = ((CButton *)GetDlgItem(IDC_CFG_FUNC_WORKING_FOLDER_REAL_PATH))->GetCheck();
    aConfig.mFileListCreateAndEditText = ((CButton *)GetDlgItem(IDC_CFG_FUNC_EDIT_AFTER_CREATING_TEXT))->GetCheck();

    GetDlgItemText(IDC_CFG_FUNC_NAME_COPY_SEPARATOR, aConfig.mClipboardSeparator, MAX_CLIP_SEPARATOR);
    if (_tcslen(aConfig.mClipboardSeparator) <= 0)
        _tcscpy(aConfig.mClipboardSeparator, XPR_STRING_LITERAL("\\r\\n"));
}
} // namespace cfg
} // namespace fxfile
