//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_func_file_scrap_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgFuncFileScrapDlg::CfgFuncFileScrapDlg(void)
    : super(IDD_CFG_FUNC_FILE_SCRAP, XPR_NULL)
{
}

void CfgFuncFileScrapDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncFileScrapDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncFileScrapDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_FILE_SCRAP_SHOW_POPUP, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_scrap.show_on_shell_popup_menu")));
    SetDlgItemText(IDC_CFG_FILE_SCRAP_SAVE_SCRAP, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.file_scrap.save_file_scrap")));

    return XPR_TRUE;
}

void CfgFuncFileScrapDlg::onInit(const Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_FILE_SCRAP_SHOW_POPUP))->SetCheck(aConfig.mFileScrapContextMenu);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_SCRAP_SAVE_SCRAP))->SetCheck(aConfig.mFileScrapSave);
}

void CfgFuncFileScrapDlg::onApply(Option::Config &aConfig)
{
    aConfig.mFileScrapContextMenu = ((CButton *)GetDlgItem(IDC_CFG_FILE_SCRAP_SHOW_POPUP))->GetCheck();
    aConfig.mFileScrapSave        = ((CButton *)GetDlgItem(IDC_CFG_FILE_SCRAP_SAVE_SCRAP))->GetCheck();
}
} // namespace cfg
} // namespace fxfile
