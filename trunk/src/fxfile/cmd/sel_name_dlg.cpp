//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "sel_name_dlg.h"

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
SelNameDlg::SelNameDlg(xpr_bool_t aSelect)
    : super(IDD_SEL_NAME, XPR_NULL)
    , mSelect(aSelect)
    , mOnlySel(XPR_FALSE)
    , mDlgState(XPR_NULL)
{
}

void SelNameDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEL_NAME, mComboBox);
}

BEGIN_MESSAGE_MAP(SelNameDlg, super)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t SelNameDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mComboBox.LimitText(XPR_MAX_PATH);

    if (mSelect == XPR_TRUE)
    {
        SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.select_by_name.title")));
        SetDlgItemText(IDC_SEL_NAME_DESC,     gApp.loadString(XPR_STRING_LITERAL("popup.select_by_name.label.desc")));
        SetDlgItemText(IDC_SEL_NAME_SEL_ONLY, gApp.loadString(XPR_STRING_LITERAL("popup.select_by_name.check.select_only")));
    }
    else
    {
        GetDlgItem(IDC_SEL_NAME_SEL_ONLY)->ShowWindow(SW_HIDE);

        SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.deselect_by_name.title")));
        SetDlgItemText(IDC_SEL_NAME_DESC, gApp.loadString(XPR_STRING_LITERAL("popup.deselect_by_name.label.desc")));
    }

    SetDlgItemText(IDOK,     gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL, gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    // dialog state
    const xpr_tchar_t *sSection = (mSelect == XPR_TRUE) ? XPR_STRING_LITERAL("SelName") : XPR_STRING_LITERAL("UnSelName"); 

    xpr_bool_t sOnlySel = XPR_FALSE;

    mDlgState = DlgStateManager::instance().getDlgState(sSection);
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Name"), mComboBox.GetDlgCtrlID());
        mDlgState->load();
    }

    if (mSelect == XPR_TRUE)
    {
        ((CButton *)GetDlgItem(IDC_SEL_NAME_SEL_ONLY))->SetCheck(sOnlySel);
    }

    if (mComboBox.GetCount() > 0)
        mComboBox.SetCurSel(0);

    return XPR_TRUE;
}

void SelNameDlg::OnDestroy(void) 
{
    super::OnDestroy();

}

void SelNameDlg::OnOK(void) 
{
    mOnlySel = ((CButton *)GetDlgItem(IDC_SEL_NAME_SEL_ONLY))->GetCheck();

    if (mComboBox.GetWindowText(mName, XPR_MAX_PATH) <= 0)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        if (mSelect == XPR_TRUE)
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.select_by_filter.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
        else
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.deselect_by_filter.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        mComboBox.SetFocus();
        return;
    }

    // dialog state
    DlgState::insertComboEditString(&mComboBox, XPR_FALSE);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        if (mSelect == XPR_TRUE)
            mDlgState->setStateB(XPR_STRING_LITERAL("Only Sel"), mOnlySel);
        mDlgState->save();
    }

    super::OnOK();
}
} // namespace cmd
} // namespace fxfile
