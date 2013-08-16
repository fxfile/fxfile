//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_replace_dlg.h"

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
BatchRenameTabReplaceDlg::BatchRenameTabReplaceDlg(void)
    : super(IDD_BATCH_RENAME_TAB_REPLACE)
    , mDlgState(XPR_NULL)
{
}

void BatchRenameTabReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabReplaceDlg, super)
    ON_BN_CLICKED(IDC_BATCH_RENAME_REPLACE_APPLY, OnApply)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabReplaceDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_REPLACE_OLD,   sizeResize, sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_REPLACE_NEW,   sizeResize, sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_REPLACE_APPLY, sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_RENAME_REPLACE_REPEAT_SPIN);
    sSpinCtrl->SetRange32(0, ksint32max);

    ((CEdit *)GetDlgItem(IDC_BATCH_RENAME_REPLACE_OLD))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_BATCH_RENAME_REPLACE_NEW))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_BATCH_RENAME_REPLACE_LABEL_OLD,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace.label.old")));
    SetDlgItemText(IDC_BATCH_RENAME_REPLACE_LABEL_NEW,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace.label.new")));
    SetDlgItemText(IDC_BATCH_RENAME_REPLACE_LABEL_REPEAT, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace.label.repeat")));
    SetDlgItemText(IDC_BATCH_RENAME_REPLACE_NOCASE,       gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace.check.no_case")));
    SetDlgItemText(IDC_BATCH_RENAME_REPLACE_APPLY,        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace.button.apply")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("Rename2"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Old"), IDC_BATCH_RENAME_REPLACE_OLD);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("New"), IDC_BATCH_RENAME_REPLACE_NEW);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Repeat"),  IDC_BATCH_RENAME_REPLACE_REPEAT);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("No Case"), IDC_BATCH_RENAME_REPLACE_NOCASE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void BatchRenameTabReplaceDlg::OnApply(void) 
{
    CComboBox *sComboBoxOld = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_REPLACE_OLD);
    CComboBox *sComboBoxNew = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_REPLACE_NEW);

    DlgState::insertComboEditString(sComboBoxOld);
    DlgState::insertComboEditString(sComboBoxNew);

    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 2);
}

xpr_bool_t BatchRenameTabReplaceDlg::PreTranslateMessage(MSG* pMsg) 
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
        case IDC_BATCH_RENAME_REPLACE_OLD:
        case IDC_BATCH_RENAME_REPLACE_NEW:
        case IDC_BATCH_RENAME_REPLACE_REPEAT:
        case IDC_BATCH_RENAME_REPLACE_REPEAT_SPIN:
        case IDC_BATCH_RENAME_REPLACE_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

void BatchRenameTabReplaceDlg::OnDestroy(void) 
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
