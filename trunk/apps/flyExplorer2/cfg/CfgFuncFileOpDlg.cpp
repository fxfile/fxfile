//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncFileOpDlg.h"

#include "../Option.h"
#include "../resource.h"
#include "../ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_COMPLETED_FLASH_TASKBAR))->SetCheck(gOpt->mFileOpCompleteFlash);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_COPY          ))->SetCheck(gOpt->mExternalCopyFileOp);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_MOVE          ))->SetCheck(gOpt->mExternalMoveFileOp);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_DELETE        ))->SetCheck(gOpt->mExternalDeleteFileOp);

    SetDlgItemText(IDC_CFG_FUNC_FILE_OP_GROUP_EXTERNAL_PROGRAM,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.operation.group.external_program")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_OP_EXTERNAL_COPY,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.operation.check.external_copy")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_OP_EXTERNAL_MOVE,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.operation.check.external_move")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_OP_EXTERNAL_DELETE,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.operation.check.external_delete")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_OP_COMPLETED_FLASH_TASKBAR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.operation.check.flash_taskbar_when_completed")));

    return XPR_TRUE;
}

void CfgFuncFileOpDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mFileOpCompleteFlash  = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_COMPLETED_FLASH_TASKBAR))->GetCheck();
    gOpt->mExternalCopyFileOp   = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_COPY))->GetCheck();
    gOpt->mExternalMoveFileOp   = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_MOVE))->GetCheck();
    gOpt->mExternalDeleteFileOp = ((CButton *)GetDlgItem(IDC_CFG_FUNC_FILE_OP_EXTERNAL_DELETE))->GetCheck();

    sOptionMgr.applyFolderCtrl(3, XPR_FALSE);
    sOptionMgr.applyExplorerView(3, XPR_FALSE);
    sOptionMgr.applyEtc(3);
}

xpr_bool_t CfgFuncFileOpDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_FILE_OP_COMPLETED_FLASH_TASKBAR:
        case IDC_CFG_FUNC_FILE_OP_EXTERNAL_COPY:
        case IDC_CFG_FUNC_FILE_OP_EXTERNAL_MOVE:
        case IDC_CFG_FUNC_FILE_OP_EXTERNAL_DELETE:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
    }

    return super::OnCommand(wParam, lParam);
}
