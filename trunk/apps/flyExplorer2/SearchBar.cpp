//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SearchBar.h"

#include "MainFrame.h"
#include "ExplorerView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(SearchBar, CSizingControlBarCF);

SearchBar::SearchBar(void)
    : mSearchDlg(XPR_NULL)
{
}

SearchBar::~SearchBar(void)
{
}

BEGIN_MESSAGE_MAP(SearchBar, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

xpr_sint_t SearchBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    return 0;
}

xpr_bool_t SearchBar::DestroyWindow(void)
{
    destroyChild();

    return super::DestroyWindow();
}

void SearchBar::createChild(void)
{
    if (XPR_IS_NULL(mSearchDlg))
    {
        mSearchDlg = new SearchDlg;
        if (XPR_IS_NULL(mSearchDlg))
            return;

        if (mSearchDlg->Create(this) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(mSearchDlg);
            return;
        }

        mSearchDlg->ShowWindow(SW_SHOW);
    }

    mSearchDlg->createAccelTable();
}

void SearchBar::destroyChild(void)
{
    if (XPR_IS_NULL(mSearchDlg))
        return;

    DESTROY_DELETE(mSearchDlg);
}

void SearchBar::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy) 
{
    super::OnSize(aType, cx, cy);

    if (XPR_IS_NOT_NULL(mSearchDlg) && XPR_IS_NOT_NULL(mSearchDlg->m_hWnd))
        mSearchDlg->MoveWindow(0, 0, cx, cy);
}

void SearchBar::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mSearchDlg) && XPR_IS_NOT_NULL(mSearchDlg->m_hWnd))
        mSearchDlg->SetFocus();
}

SearchDlg *SearchBar::getSearchDlg(void) const
{
    return mSearchDlg;
}

SearchResultCtrl *SearchBar::getSearchResultCtrl(void) const
{
    if (XPR_IS_NULL(mSearchDlg))
        return XPR_NULL;

    return mSearchDlg->getSearchResultCtrl();
}

xpr_bool_t SearchBar::PreTranslateMessage(MSG *aMsg)
{
    return super::PreTranslateMessage(aMsg);
}
