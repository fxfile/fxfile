//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "RenTabDlg3.h"

#include "resource.h"
#include "DlgState.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RenTabDlg3::RenTabDlg3(void)
    : super(IDD_RENAME_4)
{
}

void RenTabDlg3::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(RenTabDlg3, super)
    ON_BN_CLICKED(IDC_BATCH_RENAME_DELETE_APPLY, OnApply)
    ON_CBN_SELCHANGE(IDC_BATCH_RENAME_DELETE_TYPE, OnSelchangeDeleteSelection)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t RenTabDlg3::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_DELETE_APPLY, sizeRepos, sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.head")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.tail")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.from_begin")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.from_end")));

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_RENAME_DELETE_POS_SPIN);
    sSpinCtrl->SetRange(0, XPR_MAX_PATH);
    sSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_RENAME_DELETE_LENGTH_SPIN);
    sSpinCtrl->SetRange(1, XPR_MAX_PATH);

    SetDlgItemInt(IDC_BATCH_RENAME_DELETE_LENGTH, 1, XPR_FALSE);

    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE))->SetCurSel(2);

    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_TYPE,   theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.type")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_POS,    theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.position")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_LENGTH, theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.length")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_APPLY,        theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.button.apply")));

    // Load Dialog State
    mState.setDialog(this);
    mState.setSection(XPR_STRING_LITERAL("Rename4"));
    mState.setEditCtrl(XPR_STRING_LITERAL("Pos"),    IDC_BATCH_RENAME_DELETE_POS);
    mState.setEditCtrl(XPR_STRING_LITERAL("Length"), IDC_BATCH_RENAME_DELETE_LENGTH);
    mState.setComboBox(XPR_STRING_LITERAL("Type"),   IDC_BATCH_RENAME_DELETE_TYPE);
    mState.load();

    return XPR_TRUE;
}

void RenTabDlg3::OnApply(void) 
{
    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 4);
}

void RenTabDlg3::OnSelchangeDeleteSelection(void) 
{
    enableWindowDelete();
}

void RenTabDlg3::enableWindowDelete(void)
{
    xpr_sint_t sType = ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE))->GetCurSel();

    xpr_bool_t sEnable = (sType == 2) || (sType == 3);
    GetDlgItem(IDC_BATCH_RENAME_DELETE_POS)->EnableWindow(sEnable);
    GetDlgItem(IDC_BATCH_RENAME_DELETE_POS_SPIN)->EnableWindow(sEnable);
}

xpr_bool_t RenTabDlg3::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0)
    {
        xpr_sint_t sId = ::GetDlgCtrlID(pMsg->hwnd);
        switch (sId)
        {
        case IDC_BATCH_RENAME_DELETE_TYPE:
        case IDC_BATCH_RENAME_DELETE_POS:
        case IDC_BATCH_RENAME_DELETE_POS_SPIN:
        case IDC_BATCH_RENAME_DELETE_LENGTH:
        case IDC_BATCH_RENAME_DELETE_LENGTH_SPIN:
        case IDC_BATCH_RENAME_DELETE_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

void RenTabDlg3::OnDestroy() 
{
    super::OnDestroy();

    // Save Dialog State
    mState.reset();
    mState.save();
}
