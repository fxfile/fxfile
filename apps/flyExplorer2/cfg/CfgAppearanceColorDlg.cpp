//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAppearanceColorDlg.h"
#include "CfgAppearanceColorViewDlg.h"

#include "../resource.h"
#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAppearanceColorDlg::CfgAppearanceColorDlg(void)
    : super(IDD_CFG_APPEARANCE_COLOR, XPR_NULL)
    , mOldViewIndex(-1)
{
    memset(&mViewDlg, 0, sizeof(mViewDlg));
}

void CfgAppearanceColorDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_COLOR_TAB,                      mTabCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_ACTIVED_VIEW_COLOR,       mActivedViewColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_INFO_BAR_BOOKMARK_COLOR,  mInfoBarBookmarkColorCtrl);
    DDX_Control(pDX, IDC_CFG_COLOR_PATH_BAR_HIGHLIGHT_COLOR, mPathBarHighlightColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgAppearanceColorDlg, super)
    ON_WM_DESTROY()
    ON_NOTIFY(TCN_SELCHANGE, IDC_CFG_COLOR_TAB, OnSelChangeTab)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceColorDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_COLOR_ACTIVED_VIEW_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_INFO_BAR_BOOKMARK_COLOR);
    addIgnoreApply(IDC_CFG_COLOR_PATH_BAR_HIGHLIGHT_COLOR);

    xpr_sint_t i;
    xpr::tstring sStringId;

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i] = new CfgAppearanceColorViewDlg(i);
        mViewDlg[i]->setObserver(mObserver);
        mViewDlg[i]->Create(mCfgIndex, this);
        mViewDlg[i]->ShowWindow(SW_HIDE);

        sStringId.format(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.tab.view%d"), i + 1);
        mTabCtrl.InsertItem(i, theApp.loadString(sStringId.c_str()));
    }

    showTabDlg(0);

    SetDlgItemText(IDC_CFG_COLOR_LABEL_ACTIVED_VIEW_COLOR,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.label.actived_view_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_INFO_BAR_BOOKMARK_COLOR,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.label.info_bar_bookmark_color")));
    SetDlgItemText(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.color.check.path_bar_highlight_color")));

    return XPR_TRUE;
}

void CfgAppearanceColorDlg::OnDestroy(void)
{
    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->DestroyWindow();
        XPR_SAFE_DELETE(mViewDlg[i]);
    }

    super::OnDestroy();
}

void CfgAppearanceColorDlg::onInit(Option::Config &aConfig)
{
    mActivedViewColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mActivedViewColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mActivedViewColorCtrl.SetDefaultColor(RGB(48,192,48));
    mActivedViewColorCtrl.SetColor(aConfig.mActiveViewColor);

    mInfoBarBookmarkColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mInfoBarBookmarkColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mInfoBarBookmarkColorCtrl.SetDefaultColor(RGB(0,0,255));
    mInfoBarBookmarkColorCtrl.SetColor(aConfig.mContentsBookmarkColor);

    mPathBarHighlightColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mPathBarHighlightColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mPathBarHighlightColorCtrl.SetDefaultColor(Option::getPathBarDefaultHighlightColor());
    mPathBarHighlightColorCtrl.SetColor(aConfig.mPathBarHighlightColor);

    ((CButton *)GetDlgItem(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR))->SetCheck(aConfig.mPathBarHighlight);

    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->onInit(aConfig);
    }
}

void CfgAppearanceColorDlg::onApply(Option::Config &aConfig)
{
    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mViewDlg[i]->onApply(aConfig);
    }

    aConfig.mActiveViewColor       = mActivedViewColorCtrl.GetColor();
    aConfig.mContentsBookmarkColor = mInfoBarBookmarkColorCtrl.GetColor();
    aConfig.mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetColor();
    aConfig.mPathBarHighlight      = ((CButton *)GetDlgItem(IDC_CFG_COLOR_LABEL_PATH_BAR_HIGHLIGHT_COLOR))->GetCheck();

    if (aConfig.mActiveViewColor       == CLR_DEFAULT) aConfig.mActiveViewColor       = mActivedViewColorCtrl.GetDefaultColor();
    if (aConfig.mContentsBookmarkColor == CLR_DEFAULT) aConfig.mContentsBookmarkColor = mInfoBarBookmarkColorCtrl.GetDefaultColor();
    if (aConfig.mPathBarHighlightColor == CLR_DEFAULT) aConfig.mPathBarHighlightColor = mPathBarHighlightColorCtrl.GetDefaultColor();
}

void CfgAppearanceColorDlg::OnSelChangeTab(NMHDR *aNMHDR, LRESULT *aResult)
{
    *aResult = 0;

    xpr_sint_t sViewIndex = mTabCtrl.GetCurSel();

    showTabDlg(sViewIndex);
}

void CfgAppearanceColorDlg::showTabDlg(xpr_sint_t aViewIndex)
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

LRESULT CfgAppearanceColorDlg::OnSelEndOK(WPARAM aWParam, LPARAM aLParam)
{
    setModified();

    return 0;
}
