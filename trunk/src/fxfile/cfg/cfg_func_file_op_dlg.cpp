//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_func_file_op_dlg.h"

#include "../option.h"
#include "../resource.h"
#include "../explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgFuncFileOpDlg::CfgFuncFileOpDlg(void)
    : super(IDD_CFG_FUNC_FILE_OP, XPR_NULL)
{
}

void CfgFuncFileOpDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncFileOpDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncFileOpDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_FILE_OP_GROUP_EXTERNAL_PROGRAM,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_operation.group.external_program")));
    SetDlgItemText(IDC_CFG_FILE_OP_EXTERNAL_COPY,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_operation.check.external_copy")));
    SetDlgItemText(IDC_CFG_FILE_OP_EXTERNAL_MOVE,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_operation.check.external_move")));
    SetDlgItemText(IDC_CFG_FILE_OP_EXTERNAL_DELETE,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_operation.check.external_delete")));
    SetDlgItemText(IDC_CFG_FILE_OP_COMPLETED_FLASH_TASKBAR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_operation.check.flash_taskbar_when_completed")));

    return XPR_TRUE;
}

void CfgFuncFileOpDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_COMPLETED_FLASH_TASKBAR))->SetCheck(aConfig.mFileOpCompleteFlash);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_COPY          ))->SetCheck(aConfig.mExternalCopyFileOp);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_MOVE          ))->SetCheck(aConfig.mExternalMoveFileOp);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_DELETE        ))->SetCheck(aConfig.mExternalDeleteFileOp);
}

void CfgFuncFileOpDlg::onApply(Option::Config &aConfig)
{
    aConfig.mFileOpCompleteFlash  = ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_COMPLETED_FLASH_TASKBAR))->GetCheck();
    aConfig.mExternalCopyFileOp   = ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_COPY))->GetCheck();
    aConfig.mExternalMoveFileOp   = ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_MOVE))->GetCheck();
    aConfig.mExternalDeleteFileOp = ((CButton *)GetDlgItem(IDC_CFG_FILE_OP_EXTERNAL_DELETE))->GetCheck();
}
} // namespace cfg
} // namespace fxfile
