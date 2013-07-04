//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "sel_filter_dlg.h"

#include "resource.h"
#include "dlg_state_manager.h"
#include "dlg_state.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
SelFilterDlg::SelFilterDlg(xpr_bool_t aSelect)
    : super(IDD_SEL_FILTER, XPR_NULL)
    , mSelect(aSelect)
    , mOnlySel(XPR_FALSE)
    , mDlgState(XPR_NULL)
{
}

void SelFilterDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEL_FILTER, mComboBox);
}

BEGIN_MESSAGE_MAP(SelFilterDlg, super)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_SEL_FILTER, OnSelchangeFilter)
END_MESSAGE_MAP()

xpr_bool_t SelFilterDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mComboBox.LimitText(XPR_MAX_PATH);

    if (mSelect == XPR_TRUE)
    {
        SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.select_by_filter.title")));
        SetDlgItemText(IDC_SEL_FILTER_DESC,     theApp.loadString(XPR_STRING_LITERAL("popup.select_by_filter.label.desc")));
        SetDlgItemText(IDC_SEL_FILTER_SEL_ONLY, theApp.loadString(XPR_STRING_LITERAL("popup.select_by_filter.check.select_only")));
    }
    else
    {
        GetDlgItem(IDC_SEL_FILTER_SEL_ONLY)->ShowWindow(SW_HIDE);

        SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.deselect_by_filter.title")));
        SetDlgItemText(IDC_SEL_FILTER_DESC, theApp.loadString(XPR_STRING_LITERAL("popup.deselect_by_filter.label.desc")));
    }

    SetDlgItemText(IDOK,     theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL, theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    // dialog state
    const xpr_tchar_t *sSection = (mSelect == XPR_TRUE) ? XPR_STRING_LITERAL("SelFilter") : XPR_STRING_LITERAL("UnSelFilter"); 

    xpr::tstring sPrevFilter;
    xpr_bool_t sOnlySel = XPR_FALSE;

    mDlgState = DlgStateManager::instance().getDlgState(sSection);
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->load();

        sPrevFilter = mDlgState->getStateS(XPR_STRING_LITERAL("Filter"), XPR_STRING_LITERAL(""));

        if (mSelect == XPR_TRUE)
        {
            sOnlySel = mDlgState->getStateB(XPR_STRING_LITERAL("Only Sel"), XPR_FALSE);
        }
    }

    ((CButton *)GetDlgItem(IDC_SEL_FILTER_SEL_ONLY))->SetCheck(sOnlySel);

    FilterMgr &sFilterMgr = FilterMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    FilterItem *sFilterItem;

    sCount = sFilterMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = sFilterMgr.getFilter(i);
        if (sFilterItem == XPR_NULL)
            continue;

        sIndex = mComboBox.AddString(sFilterItem->mName.c_str());
        mComboBox.SetItemData(sIndex, (DWORD_PTR)sFilterItem);

        if (_tcsicmp(sFilterItem->mName.c_str(), sPrevFilter.c_str()) == 0)
            mComboBox.SetCurSel(sIndex);
    }

    if (mComboBox.GetCurSel() == CB_ERR)
        mComboBox.SetCurSel(0);

    OnSelchangeFilter();

    return XPR_TRUE;
}

void SelFilterDlg::OnDestroy(void) 
{
    super::OnDestroy();

}

void SelFilterDlg::OnOK(void) 
{
    mOnlySel = ((CButton *)GetDlgItem(IDC_SEL_FILTER_SEL_ONLY))->GetCheck();

    xpr_sint_t sIndex = mComboBox.GetCurSel();
    if (sIndex == CB_ERR)
    {
        const xpr_tchar_t *sMsg = XPR_NULL;
        if (mSelect == XPR_TRUE)
            sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.select_by_filter.msg.select"));
        else
            sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.deselect_by_filter.msg.select"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        mComboBox.SetFocus();
        return;
    }

    mFilterItem = (FilterItem *)mComboBox.GetItemData(sIndex);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->setStateS(XPR_STRING_LITERAL("Filter"), mFilterItem->mName.c_str());
        if (mSelect == XPR_TRUE)
            mDlgState->setStateB(XPR_STRING_LITERAL("Only Sel"), mOnlySel);
        mDlgState->save();
    }

    super::OnOK();
}

void SelFilterDlg::OnSelchangeFilter(void)
{
    xpr::tstring sExts;

    xpr_sint_t sIndex = mComboBox.GetCurSel();
    if (sIndex != CB_ERR)
    {
        FilterItem *sFilterItem = (FilterItem *)mComboBox.GetItemData(sIndex);
        if (sFilterItem != XPR_NULL)
            sExts = sFilterItem->mExts;
    }

    SetDlgItemText(IDC_SEL_FILTER_STATUS, sExts.c_str());
}
} // namespace cmd
} // namespace fxfile
