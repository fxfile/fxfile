//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncRenameDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncRenameDlg::CfgFuncRenameDlg(void)
    : super(IDD_CFG_FUNC_RENAME, XPR_NULL)
{
}

void CfgFuncRenameDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncRenameDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncRenameDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_EXT_DEFAULT                    ))->SetCheck((gOpt->mRenameExtType == RENAME_EXT_TYPE_DEFAULT) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_EXT_KEEP                       ))->SetCheck((gOpt->mRenameExtType == RENAME_EXT_TYPE_KEEP) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_EXT_SEL_EXCEPT_FOR_EXT         ))->SetCheck((gOpt->mRenameExtType == RENAME_EXT_TYPE_SEL_EXCEPT_FOR_EXT) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_DEFAULT          ))->SetCheck((gOpt->mSingleRenameType == SINGLE_RENAME_TYPE_DEFAULT) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_POPUP         ))->SetCheck((gOpt->mSingleRenameType == SINGLE_RENAME_TYPE_BY_POPUP) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_BATCH_RENAME  ))->SetCheck((gOpt->mSingleRenameType == SINGLE_RENAME_TYPE_BATCH_RENAME) ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BATCH_RENAME_ON_MULTI_SEL_ITEMS))->SetCheck(gOpt->mBatchRenameMultiSel);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BATCH_RENAME_WITH_FOLDER       ))->SetCheck(gOpt->mBatchRenameWithFolder);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BY_MOUSE                       ))->SetCheck(gOpt->mRenameByMouse);

    SetDlgItemText(IDC_CFG_FUNC_RENAME_GROUP_COMMAND,                   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.group.command")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_BATCH_RENAME_ON_MULTI_SEL_ITEMS, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.check.batch_reanme_on_multiple_selected_items")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_BATCH_RENAME_WITH_FOLDER,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.check.batch_reanme_with_folder")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_BY_MOUSE,                        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.check.rename_by_mouse")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_GROUP_SINGLE_RENAME,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.group.single_rename")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_DEFAULT,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.single_rename_default")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_POPUP,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.single_rename_by_popup")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_BATCH_RENAME,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.single_rename_by_batch_rename")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_GROUP_EXT,                       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.group.extension")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_EXT_DEFAULT,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.extension_default")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_EXT_KEEP,                        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.extension_keep")));
    SetDlgItemText(IDC_CFG_FUNC_RENAME_EXT_SEL_EXCEPT_FOR_EXT,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.rename.radio.extension_select_except_for_extension")));

    return XPR_TRUE;
}

void CfgFuncRenameDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mRenameExtType = RENAME_EXT_TYPE_DEFAULT;
    if (((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_EXT_KEEP))->GetCheck())
        gOpt->mRenameExtType = RENAME_EXT_TYPE_KEEP;
    else if (((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_EXT_SEL_EXCEPT_FOR_EXT))->GetCheck())
        gOpt->mRenameExtType = RENAME_EXT_TYPE_SEL_EXCEPT_FOR_EXT;

    gOpt->mSingleRenameType = SINGLE_RENAME_TYPE_DEFAULT;
    if (((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_POPUP))->GetCheck())
        gOpt->mSingleRenameType = SINGLE_RENAME_TYPE_BY_POPUP;
    else if (((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_BATCH_RENAME))->GetCheck())
        gOpt->mSingleRenameType = SINGLE_RENAME_TYPE_BATCH_RENAME;

    gOpt->mBatchRenameMultiSel   = ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BATCH_RENAME_ON_MULTI_SEL_ITEMS))->GetCheck();
    gOpt->mBatchRenameWithFolder = ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BATCH_RENAME_WITH_FOLDER))->GetCheck();
    gOpt->mRenameByMouse         = ((CButton *)GetDlgItem(IDC_CFG_FUNC_RENAME_BY_MOUSE))->GetCheck();

    sOptionMgr.applyFolderCtrl(0, XPR_FALSE);
    sOptionMgr.applyExplorerView(0, XPR_FALSE);
    sOptionMgr.applyEtc(0);
}

xpr_bool_t CfgFuncRenameDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_RENAME_EXT_DEFAULT:
        case IDC_CFG_FUNC_RENAME_EXT_KEEP:
        case IDC_CFG_FUNC_RENAME_EXT_SEL_EXCEPT_FOR_EXT:
        case IDC_CFG_FUNC_RENAME_SINGLE_RENAME_DEFAULT:
        case IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_POPUP:
        case IDC_CFG_FUNC_RENAME_SINGLE_RENAME_BY_BATCH_RENAME:
        case IDC_CFG_FUNC_RENAME_BATCH_RENAME_ON_MULTI_SEL_ITEMS:
        case IDC_CFG_FUNC_RENAME_BATCH_RENAME_WITH_FOLDER:
        case IDC_CFG_FUNC_RENAME_BY_MOUSE:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
