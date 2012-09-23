//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "WorkingBar.h"

#include "Option.h"
#include "fxb/fxb_sys_img_list.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(WorkingBar, CSizingControlBarCF);

WorkingBar::WorkingBar(void)
    : mTabWnd(XPR_NULL)
    , mSearchCtrl(XPR_NULL)
{
}

WorkingBar::~WorkingBar(void)
{
}

BEGIN_MESSAGE_MAP(WorkingBar, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

xpr_sint_t WorkingBar::OnCreate(LPCREATESTRUCT lpcs) 
{
    if (super::OnCreate(lpcs) == -1)
        return -1;

    return 0;
}

xpr_bool_t WorkingBar::DestroyWindow(void)
{
    destroyChild();

    return super::DestroyWindow();
}

SearchResultCtrl *WorkingBar::getSearchResultCtrl(void) const
{
    return mSearchCtrl;
}

void WorkingBar::createChild(void)
{
    if (XPR_IS_NULL(mSearchCtrl))
    {
        mTabWnd = new CBCGTabWnd;
        if (XPR_IS_NULL(mTabWnd))
            return;

        if (mTabWnd->Create(CBCGTabWnd::STYLE_FLAT_SHARED_HORZ_SCROLL, CRect(0,0,0,0), this, 101) == XPR_NULL)
        {
            XPR_TRACE(XPR_STRING_LITERAL("Failed to create working tab window\n"));
            return;
        }

        fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

        mSearchCtrl = new SearchResultCtrl;
        if (XPR_IS_NULL(mSearchCtrl))
        {
            DESTROY_DELETE(mTabWnd);
            return;
        }

        if (mSearchCtrl->Create(WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS, CRect(0,0,0,0), mTabWnd, 1) == XPR_FALSE)
        {
            DESTROY_DELETE(mTabWnd);

            XPR_TRACE(XPR_STRING_LITERAL("Failed to create working search result window\n"));
            return;
        }

        mSearchCtrl->SetExtendedStyle(mSearchCtrl->GetExtendedStyle() | WS_EX_CLIENTEDGE);
        mSearchCtrl->SetImageList(&sSysImgListMgr.mSysImgList16, LVSIL_SMALL);
        mSearchCtrl->SetImageList(&sSysImgListMgr.mSysImgList32, LVSIL_NORMAL);

        mTabWnd->AddTab(mSearchCtrl, theApp.loadString(XPR_STRING_LITERAL("bar.search_result.title")), -1);
    }

    recalcLayout();

    mSearchCtrl->createAccelTable();
}

void WorkingBar::destroyChild(void)
{
    DESTROY_DELETE(mTabWnd);
    DESTROY_DELETE(mSearchCtrl);
}

void WorkingBar::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void WorkingBar::recalcLayout(void)
{
    if (XPR_IS_NOT_NULL(mTabWnd))
    {
        CRect sRect;
        GetClientRect(sRect);

        mTabWnd->SetWindowPos(XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height (), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
    }
}

void WorkingBar::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mSearchCtrl) && XPR_IS_NOT_NULL(mSearchCtrl->m_hWnd))
        mSearchCtrl->SetFocus();
}

void WorkingBar::OnWindowPosChanging(WINDOWPOS FAR *aWindowPos) 
{
    super::OnWindowPosChanging(aWindowPos);
}

xpr_bool_t WorkingBar::PreTranslateMessage(MSG *aMsg)
{
    return super::PreTranslateMessage(aMsg);
}

void WorkingBar::OnBarClose(void)
{
    if (XPR_IS_TRUE(gOpt->mSearchResultClearOnClose))
        mSearchCtrl->DeleteAllItems();
}
