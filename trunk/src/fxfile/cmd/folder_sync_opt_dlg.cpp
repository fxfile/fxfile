//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_sync_opt_dlg.h"

#include "sync_dirs.h"
#include "size_format.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
FolderSyncOptDlg::FolderSyncOptDlg(void)
    : super(IDD_FOLDER_SYNC_OPT)
    , mDirection(SyncDirectionNone)
    , mFiles1(0), mFiles2(0)
    , mSize1(0i64), mSize2(0i64)
{
}

FolderSyncOptDlg::~FolderSyncOptDlg(void)
{
}

void FolderSyncOptDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FOLDER_SYNC_OPTION_DIRECTION, mDirectionComboBox);
}

BEGIN_MESSAGE_MAP(FolderSyncOptDlg, super)
    ON_CBN_SELCHANGE(IDC_FOLDER_SYNC_OPTION_DIRECTION, OnSelchangeDirection)
    ON_BN_CLICKED(IDOK,                            OnOK)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_OPTION_TO_RIGHT, OnToLeftRight)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_OPTION_TO_LEFT,  OnToLeftRight)
END_MESSAGE_MAP()

xpr_bool_t FolderSyncOptDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDirectionComboBox.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.right")));
    mDirectionComboBox.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.left")));
    mDirectionComboBox.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.bi-drectional")));
    mDirectionComboBox.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.none")));
    mDirectionComboBox.SetCurSel(0);

    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_PATH1, mPath1.c_str());
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_PATH2, mPath2.c_str());

    xpr_tchar_t sText[0xff] = {0};
    xpr_tchar_t sFilesFmt[0xff] = {0};
    xpr_tchar_t sSizeFmt[0xff] = {0};

    SizeFormat::getFormatedNumber(mFiles1, sFilesFmt, 0xfe);
    SizeFormat::getFormatedNumber(mSize1,  sSizeFmt,  0xfe);
    _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync_option.check.to_right"), XPR_STRING_LITERAL("%s,%s")), sFilesFmt, sSizeFmt);
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_TO_RIGHT, sText);

    SizeFormat::getFormatedNumber(mFiles2, sFilesFmt, 0xfe);
    SizeFormat::getFormatedNumber(mSize2,  sSizeFmt,  0xfe);
    _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync_option.check.to_left"), XPR_STRING_LITERAL("%s,%s")), sFilesFmt, sSizeFmt);
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_TO_LEFT, sText);

    OnSelchangeDirection();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.title")));
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_LABEL_DIRECTION, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.label.direction")));
    SetDlgItemText(IDOK,                                   gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                               gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void FolderSyncOptDlg::setPath(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL)
        return;

    if (aIndex == 0) mPath1 = aPath;
    if (aIndex == 1) mPath2 = aPath;
}

void FolderSyncOptDlg::setFiles(xpr_sint_t aIndex, xpr_sint_t aFiles, xpr_sint64_t aSize)
{
    if (aIndex == 0) { mFiles1 = aFiles; mSize1 = aSize; }
    if (aIndex == 1) { mFiles2 = aFiles; mSize2 = aSize; }
}

void FolderSyncOptDlg::OnSelchangeDirection(void)
{
    xpr_uint_t sDirection = SyncDirectionNone;

    xpr_sint_t sCurSel = mDirectionComboBox.GetCurSel();
    switch (sCurSel)
    {
    case 0: sDirection = SyncDirectionToRight;     break;
    case 1: sDirection = SyncDirectionToLeft;      break;
    case 2: sDirection = SyncDirectionBidirection; break;
    }

    ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_RIGHT))->SetCheck(XPR_TEST_BITS(sDirection, SyncDirectionToRight));
    ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_LEFT ))->SetCheck(XPR_TEST_BITS(sDirection, SyncDirectionToLeft));
}

void FolderSyncOptDlg::OnOK(void)
{
    xpr_sint_t sCurSel = mDirectionComboBox.GetCurSel();
    switch (sCurSel)
    {
    case 0: mDirection = SyncDirectionToRight;     break;
    case 1: mDirection = SyncDirectionToLeft;      break;
    case 2: mDirection = SyncDirectionBidirection; break;
    }

    super::OnOK();
}

xpr_uint_t FolderSyncOptDlg::getDirection(void)
{
    return mDirection;
}

void FolderSyncOptDlg::OnToLeftRight(void)
{
    xpr_bool_t sToLeft  = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_LEFT ))->GetCheck();
    xpr_bool_t sToRight = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_RIGHT))->GetCheck();

    xpr_sint_t sCurSel = 3;
    if (sToLeft == XPR_TRUE && sToRight == XPR_TRUE)
        sCurSel = 2;
    else if (sToLeft == XPR_TRUE) sCurSel = 1;
    else if (sToRight == XPR_TRUE) sCurSel = 0;

    mDirectionComboBox.SetCurSel(sCurSel);
}
} // namespace cmd
} // namespace fxfile
