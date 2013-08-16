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
RenameTipDlg::RenameTipDlg(void)
    : super(IDD_RENAME_TIP, XPR_NULL)
{
}

void RenameTipDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(RenameTipDlg, super)
    ON_WM_CREATE()
END_MESSAGE_MAP()

xpr_bool_t RenameTipDlg::Create(CWnd* pParentWnd)
{
    return super::Create(IDD_RENAME_TIP, pParentWnd);
}

xpr_sint_t RenameTipDlg::OnCreate(LPCREATESTRUCT lpcs)
{
    if (super::OnCreate(lpcs) == -1)
        return -1;

    return 0;
}

xpr_bool_t RenameTipDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.title")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_NUMBER_DIGIT,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.number_digit")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_NUMBER_DIGIT_ZERO, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.number_digit_with_zero_filled")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_ORG_FILENAME,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.original_filename")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_FULL_FILENAME,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.full_filename")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_EXT,               gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.extension")));

    SetDlgItemText(IDC_RENAME_TIP_LABEL_TODAY_DATE,        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.today_date")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_YEAR,              gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_year")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MONTH,             gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_month")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_DAY,               gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_day")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_TIME,              gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_time")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_HOUR,              gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MINUTE,            gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_minute")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_SECOND,            gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.current_second")));

    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_DATE,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.date")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_YEAR,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.year")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_MONTH,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.month")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_DAY,       gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.day")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_TIME,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.time")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_AM_PM,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.am_pm")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_12HOUR,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.12-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_24HOUR,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.24-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_MINUTE,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.minute")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_CREATED_SECOND,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_created_time.second")));

    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_DATE,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.date")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_YEAR,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.year")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_MONTH,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.month")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_DAY,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.day")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_TIME,     gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.time")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_AM_PM,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.am_pm")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_12HOUR,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.12-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_24HOUR,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.24-hour")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_MINUTE,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.minute")));
    SetDlgItemText(IDC_RENAME_TIP_LABEL_MODIFIED_SECOND,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_tip.label.file_modified_time.second")));

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
