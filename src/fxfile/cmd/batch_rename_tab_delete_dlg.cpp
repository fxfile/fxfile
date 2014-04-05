//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_delete_dlg.h"
#include "batch_rename.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchRenameTabDeleteDlg::BatchRenameTabDeleteDlg(void)
    : super(IDD_BATCH_RENAME_TAB_DELETE)
    , mDlgState(XPR_NULL)
{
}

void BatchRenameTabDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabDeleteDlg, super)
    ON_BN_CLICKED(IDC_BATCH_RENAME_DELETE_APPLY, OnApply)
    ON_CBN_SELCHANGE(IDC_BATCH_RENAME_DELETE_TYPE, OnSelchangeDeleteSelection)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabDeleteDlg::OnInitDialog(void) 
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
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.head")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.tail")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.from_begin")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.type.from_end")));

    ((CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_DELETE_POS_SPIN   ))->SetRange(FXFILE_BATCH_RENAME_DELETE_POS_MIN,    FXFILE_BATCH_RENAME_DELETE_POS_MAX);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_DELETE_LENGTH_SPIN))->SetRange(FXFILE_BATCH_RENAME_DELETE_LENGTH_MIN, FXFILE_BATCH_RENAME_DELETE_LENGTH_MAX);

    SetDlgItemInt(IDC_BATCH_RENAME_DELETE_POS,    FXFILE_BATCH_RENAME_DELETE_POS_DEF);
    SetDlgItemInt(IDC_BATCH_RENAME_DELETE_LENGTH, FXFILE_BATCH_RENAME_DELETE_LENGTH_DEF);

    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE))->SetCurSel(2);

    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_TYPE,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.type")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_POS,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.position")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_LABEL_LENGTH, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.label.length")));
    SetDlgItemText(IDC_BATCH_RENAME_DELETE_APPLY,        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete.button.apply")));

    // Load Dialog State
    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchRenameDelete"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Pos"),    IDC_BATCH_RENAME_DELETE_POS);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Length"), IDC_BATCH_RENAME_DELETE_LENGTH);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"),   IDC_BATCH_RENAME_DELETE_TYPE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void BatchRenameTabDeleteDlg::OnApply(void) 
{
    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 4);
}

void BatchRenameTabDeleteDlg::OnSelchangeDeleteSelection(void) 
{
    enableWindowDelete();
}

void BatchRenameTabDeleteDlg::enableWindowDelete(void)
{
    xpr_sint_t sType = ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE))->GetCurSel();

    xpr_bool_t sEnable = (sType == 2) || (sType == 3);
    GetDlgItem(IDC_BATCH_RENAME_DELETE_POS)->EnableWindow(sEnable);
    GetDlgItem(IDC_BATCH_RENAME_DELETE_POS_SPIN)->EnableWindow(sEnable);
}

xpr_bool_t BatchRenameTabDeleteDlg::PreTranslateMessage(MSG* pMsg) 
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

void BatchRenameTabDeleteDlg::OnDestroy(void) 
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
