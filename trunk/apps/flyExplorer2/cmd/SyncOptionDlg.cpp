//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SyncOptionDlg.h"

#include "fxb/fxb_sync_dirs.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SyncOptionDlg::SyncOptionDlg(void)
    : super(IDD_FOLDER_SYNC_OPTION)
    , mDirection(fxb::SyncDirectionNone)
    , mFiles1(0), mFiles2(0)
    , mSize1(0i64), mSize2(0i64)
{
}

SyncOptionDlg::~SyncOptionDlg(void)
{
}

void SyncOptionDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FOLDER_SYNC_OPTION_DIRECTION, mDirectionComboBox);
}

BEGIN_MESSAGE_MAP(SyncOptionDlg, super)
    ON_CBN_SELCHANGE(IDC_FOLDER_SYNC_OPTION_DIRECTION, OnSelchangeDirection)
    ON_BN_CLICKED(IDOK,                            OnOK)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_OPTION_TO_RIGHT, OnToLeftRight)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_OPTION_TO_LEFT,  OnToLeftRight)
END_MESSAGE_MAP()

xpr_bool_t SyncOptionDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDirectionComboBox.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.right")));
    mDirectionComboBox.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.left")));
    mDirectionComboBox.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.bi-drectional")));
    mDirectionComboBox.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.folder_sync_option.direction.none")));
    mDirectionComboBox.SetCurSel(0);

    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_PATH1, mPath1.c_str());
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_PATH2, mPath2.c_str());

    xpr_tchar_t sText[0xff] = {0};
    xpr_tchar_t sFilesFmt[0xff] = {0};
    xpr_tchar_t sSizeFmt[0xff] = {0};

    fxb::GetFormatedNumber(mFiles1, sFilesFmt, 0xfe);
    fxb::GetFormatedNumber(mSize1,  sSizeFmt,  0xfe);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync_option.check.to_right"), XPR_STRING_LITERAL("%s,%s")), sFilesFmt, sSizeFmt);
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_TO_RIGHT, sText);

    fxb::GetFormatedNumber(mFiles2, sFilesFmt, 0xfe);
    fxb::GetFormatedNumber(mSize2,  sSizeFmt,  0xfe);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync_option.check.to_left"), XPR_STRING_LITERAL("%s,%s")), sFilesFmt, sSizeFmt);
    SetDlgItemText(IDC_FOLDER_SYNC_OPTION_TO_LEFT, sText);

    OnSelchangeDirection();

    return XPR_TRUE;
}

void SyncOptionDlg::setPath(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL)
        return;

    if (aIndex == 0) mPath1 = aPath;
    if (aIndex == 1) mPath2 = aPath;
}

void SyncOptionDlg::setFiles(xpr_sint_t aIndex, xpr_sint_t aFiles, xpr_sint64_t aSize)
{
    if (aIndex == 0) { mFiles1 = aFiles; mSize1 = aSize; }
    if (aIndex == 1) { mFiles2 = aFiles; mSize2 = aSize; }
}

void SyncOptionDlg::OnSelchangeDirection(void)
{
    xpr_uint_t sDirection = fxb::SyncDirectionNone;

    xpr_sint_t sCurSel = mDirectionComboBox.GetCurSel();
    switch (sCurSel)
    {
    case 0: sDirection = fxb::SyncDirectionToRight;     break;
    case 1: sDirection = fxb::SyncDirectionToLeft;      break;
    case 2: sDirection = fxb::SyncDirectionBidirection; break;
    }

    ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_RIGHT))->SetCheck(XPR_TEST_BITS(sDirection, fxb::SyncDirectionToRight));
    ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_OPTION_TO_LEFT ))->SetCheck(XPR_TEST_BITS(sDirection, fxb::SyncDirectionToLeft));
}

void SyncOptionDlg::OnOK(void)
{
    xpr_sint_t sCurSel = mDirectionComboBox.GetCurSel();
    switch (sCurSel)
    {
    case 0: mDirection = fxb::SyncDirectionToRight;     break;
    case 1: mDirection = fxb::SyncDirectionToLeft;      break;
    case 2: mDirection = fxb::SyncDirectionBidirection; break;
    }

    super::OnOK();
}

xpr_uint_t SyncOptionDlg::getDirection(void)
{
    return mDirection;
}

void SyncOptionDlg::OnToLeftRight(void)
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
