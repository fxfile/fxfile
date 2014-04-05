//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tip_dlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchRenameTipDlg::BatchRenameTipDlg(void)
    : super(IDD_BATCH_RENAME_TIP, XPR_NULL)
{
}

void BatchRenameTipDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTipDlg, super)
    ON_WM_CREATE()
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTipDlg::Create(CWnd* pParentWnd)
{
    return super::Create(IDD_RENAME_TIP, pParentWnd);
}

xpr_sint_t BatchRenameTipDlg::OnCreate(LPCREATESTRUCT lpcs)
{
    if (super::OnCreate(lpcs) == -1)
        return -1;

    return 0;
}

xpr_bool_t BatchRenameTipDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.title")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_NUMBERING,          gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.numbering")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_ORG_FILENAME,       gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.original_filename")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_FULL_FILENAME,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.full_filename")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_ONLY_FILENAME,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.filename")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_EXT,                gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.extension")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CURRENT_TIME,       gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_time")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_FILE_CREATED_TIME,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_FILE_MODIFIED_TIME, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time")));
    SetDlgItemText(IDC_RENAME_TIP_GROUP_TIME_FORMAT,        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.group.time_format")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_YEAR,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.year")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_MONTH,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.month")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_DAY,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.day")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_AM_PM,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.am_pm")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_12HOUR, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.12-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_24HOUR, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.24-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_MINUTE, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.minute")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME_FORMAT_SECOND, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.time_format.second")));

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
