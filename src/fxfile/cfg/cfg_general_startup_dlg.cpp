//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_general_startup_dlg.h"
#include "cfg_general_startup_view_dlg.h"

#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cfg
{
CfgGeneralStartupDlg::CfgGeneralStartupDlg(void)
    : super(IDD_CFG_GENERAL_STARTUP, XPR_NULL)
    , mOldViewIndex(-1)
{
    memset(&mViewDlg, 0, sizeof(mViewDlg));
}

void CfgGeneralStartupDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_STARTUP_TAB, mTabCtrl);
}

BEGIN_MESSAGE_MAP(CfgGeneralStartupDlg, super)
    ON_WM_DESTROY()
    ON_NOTIFY(TCN_SELCHANGE, IDC_CFG_STARTUP_TAB, OnSelChangeTab)
END_MESSAGE_MAP()

xpr_bool_t CfgGeneralStartupDlg::OnInitDialog(void)
{
    super::OnInitDialog();

    xpr_sint_t i;
    xpr::tstring sStringId;

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i] = new CfgGeneralStartupViewDlg(i);
        mViewDlg[i]->setObserver(mObserver);
        mViewDlg[i]->Create(mCfgIndex, this);
        mViewDlg[i]->ShowWindow(SW_HIDE);

        sStringId.format(XPR_STRING_LITERAL("popup.cfg.body.general.startup.tab.view%d"), i + 1);
        mTabCtrl.InsertItem(i, theApp.loadString(sStringId.c_str()));
    }

    showTabDlg(0);

    return XPR_TRUE;
}

void CfgGeneralStartupDlg::OnDestroy(void)
{
    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->DestroyWindow();
        XPR_SAFE_DELETE(mViewDlg[i]);
    }

    super::OnDestroy();
}

void CfgGeneralStartupDlg::onInit(Option::Config &aConfig)
{
    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->onInit(aConfig);
    }
}

void CfgGeneralStartupDlg::onApply(Option::Config &aConfig)
{
    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->onApply(aConfig);
    }
}

void CfgGeneralStartupDlg::OnSelChangeTab(NMHDR *aNMHDR, LRESULT *aResult)
{
    *aResult = 0;

    xpr_sint_t sViewIndex = mTabCtrl.GetCurSel();

    showTabDlg(sViewIndex);
}

void CfgGeneralStartupDlg::showTabDlg(xpr_sint_t aViewIndex)
{
    xpr_sint_t sViewIndex = mTabCtrl.GetCurSel();

    if (mOldViewIndex != -1)
    {
        if (XPR_IS_NOT_NULL(mViewDlg[mOldViewIndex]))
        {
            mViewDlg[mOldViewIndex]->ShowWindow(SW_HIDE);
        }
    }

    if (XPR_IS_RANGE(0, aViewIndex, MAX_VIEW_SPLIT - 1))
    {
        CRect sTabRect(0,0,0,0);
        mTabCtrl.GetWindowRect(&sTabRect);
        ScreenToClient(&sTabRect);

        CRect sItemRect(0,0,0,0);
        mTabCtrl.GetItemRect(0, &sItemRect);

        sTabRect.DeflateRect(3, sItemRect.Height()+5, 3, 3);

        mViewDlg[aViewIndex]->MoveWindow(sTabRect);
        mViewDlg[aViewIndex]->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        mViewDlg[aViewIndex]->ShowWindow(SW_SHOW);
        mViewDlg[aViewIndex]->SetFocus();
        UpdateWindow();

        mTabCtrl.SetCurSel(aViewIndex);
    }

    mOldViewIndex = aViewIndex;
}
} // namespace cfg
} // namespace fxfile
