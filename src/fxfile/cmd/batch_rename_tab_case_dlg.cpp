//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_case_dlg.h"

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
BatchRenameTabCaseDlg::BatchRenameTabCaseDlg(void)
    : super(IDD_BATCH_RENAME_TAB_CASE)
    , mDlgState(XPR_NULL)
{
}

void BatchRenameTabCaseDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabCaseDlg, super)
    ON_BN_CLICKED(IDC_BATCH_RENAME_CASE_APPLY, OnApply)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabCaseDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_CASE_APPLY, sizeRepos, sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_CASE_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.type.only_filename")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.type.only_extension")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.type.full_filename")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_CASE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.case.lowercase")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.case.uppercase")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.case.uppercase_first_character")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.case.uppercase_first_character_of_each_word")));

    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_CASE_TYPE))->SetCurSel(0);
    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_CASE))->SetCurSel(0);

    ((CEdit *)GetDlgItem(IDC_BATCH_RENAME_SKIP_SPEC_CHAR))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_BATCH_RENAME_CASE_LABEL_TYPE,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.label.type")));
    SetDlgItemText(IDC_BATCH_RENAME_CASE_LABEL_CASE,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.label.case")));
    SetDlgItemText(IDC_BATCH_RENAME_CASE_LABEL_SKIP_SPEC_CHAR, theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.label.skip_special_characters")));
    SetDlgItemText(IDC_BATCH_RENAME_CASE_APPLY,                theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case.button.apply")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("Rename5"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"),         IDC_BATCH_RENAME_CASE_TYPE);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Case"),         IDC_BATCH_RENAME_CASE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("SkipSpecChar"), IDC_BATCH_RENAME_SKIP_SPEC_CHAR);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void BatchRenameTabCaseDlg::OnApply(void) 
{
    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 5);
}

xpr_bool_t BatchRenameTabCaseDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0)
    {
        xpr_sint_t sId = ::GetDlgCtrlID(pMsg->hwnd);
        switch (sId)
        {
        case IDC_BATCH_RENAME_CASE_TYPE:
        case IDC_BATCH_RENAME_CASE:
        case IDC_BATCH_RENAME_CASE_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

void BatchRenameTabCaseDlg::OnDestroy(void) 
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
