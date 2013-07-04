//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_insert_dlg.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
BatchRenameTabInsertDlg::BatchRenameTabInsertDlg(void)
    : super(IDD_BATCH_RENAME_TAB_INSERT)
    , mDlgState(XPR_NULL)
{
}

void BatchRenameTabInsertDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabInsertDlg, super)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_BATCH_RENAME_INSERT_TYPE, OnSelchangeInsertSelection)
    ON_BN_CLICKED(IDC_BATCH_RENAME_INSERT_APPLY, OnApply)
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabInsertDlg::OnInitDialog() 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_INSERT_STRING, sizeResize, sizeNone);
    AddControl(IDC_BATCH_RENAME_INSERT_APPLY,  sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_INSERT_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.type.head")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.type.tail")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.type.from_begin")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.type.from_end")));

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_RENAME_INSERT_POS_SPIN);
    sSpinCtrl->SetRange(0, XPR_MAX_PATH);

    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_INSERT_TYPE))->SetCurSel(2);

    SetDlgItemText(IDC_BATCH_RENAME_INSERT_LABEL_TYPE,   theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.label.type")));
    SetDlgItemText(IDC_BATCH_RENAME_INSERT_LABEL_POS,    theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.label.position")));
    SetDlgItemText(IDC_BATCH_RENAME_INSERT_LABEL_STRING, theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.label.string")));
    SetDlgItemText(IDC_BATCH_RENAME_INSERT_APPLY,        theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert.button.apply")));

    // Load Dialog State
    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("Rename3"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Pos"),        IDC_BATCH_RENAME_INSERT_POS);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Insert"), IDC_BATCH_RENAME_INSERT_STRING);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"),       IDC_BATCH_RENAME_INSERT_TYPE);
        mDlgState->load();
    }

    enableWindowInsert();

    return XPR_TRUE;
}

void BatchRenameTabInsertDlg::OnApply(void) 
{
    CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_INSERT_STRING);

    DlgState::insertComboEditString(sComboBox);

    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 3);
}

void BatchRenameTabInsertDlg::OnSelchangeInsertSelection(void) 
{
    enableWindowInsert();
}

void BatchRenameTabInsertDlg::enableWindowInsert(void)
{
    xpr_sint_t sType = ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_INSERT_TYPE))->GetCurSel();

    xpr_bool_t sEnable = (sType == 2) || (sType == 3);
    GetDlgItem(IDC_BATCH_RENAME_INSERT_POS)->EnableWindow(sEnable);
    GetDlgItem(IDC_BATCH_RENAME_INSERT_POS_SPIN)->EnableWindow(sEnable);
}

xpr_bool_t BatchRenameTabInsertDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0)
    {
        HWND sHwnd = pMsg->hwnd;

        xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
        ::GetClassName(::GetParent(sHwnd), sClassName, MAX_CLASS_NAME);
        if (_tcsicmp(sClassName, XPR_STRING_LITERAL("ComboBox")) == 0)
            sHwnd = ::GetParent(sHwnd);

        xpr_sint_t sId = ::GetDlgCtrlID(sHwnd);
        switch (sId)
        {
        case IDC_BATCH_RENAME_INSERT_TYPE:
        case IDC_BATCH_RENAME_INSERT_POS:
        case IDC_BATCH_RENAME_INSERT_POS_SPIN:
        case IDC_BATCH_RENAME_INSERT_STRING:
        case IDC_BATCH_RENAME_INSERT_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

void BatchRenameTabInsertDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}
} // namespace cmd
} // namespace fxfile
