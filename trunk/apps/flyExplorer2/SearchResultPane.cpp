//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SearchResultPane.h"
#include "TabPaneObserver.h"
#include "SearchResultCtrl.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_size_format.h"

#include "rgc/StatusBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("SearchResultPane");
static const xpr_size_t  kPaneWidth[] = { 150, 90 };

//
// control id
//
enum
{
    CTRL_ID_SEARCH_RESULT = 50,
    CTRL_ID_STATUS_BAR,
};

//
// pane id
//
enum
{
    kPaneIdNormal = 10,
    kPaneIdSize,
};

SearchResultPane::SearchResultPane(void)
    : TabPane(TabTypeSearchResult)
    , mSubPaneId(TabPane::InvalidId)
    , mSearchResultCtrl(XPR_NULL)
    , mStatusBar(XPR_NULL)
{
    mStatusText0[0] = 0;
    mStatusText1[0] = 0;

    registerWindowClass(kClassName);
}

SearchResultPane::~SearchResultPane(void)
{
}

xpr_bool_t SearchResultPane::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(SearchResultPane, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

xpr_sint_t SearchResultPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    mSearchResultCtrl = new SearchResultCtrl;
    if (XPR_IS_NOT_NULL(mSearchResultCtrl))
    {
        mSearchResultCtrl->setObserver(dynamic_cast<SearchResultCtrlObserver *>(this));

        if (mSearchResultCtrl->Create(this, CTRL_ID_SEARCH_RESULT, CRect(0, 0, 0, 0)) == XPR_TRUE)
        {
            fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();
            mSearchResultCtrl->SetImageList(&sSysImgListMgr.mSysImgList16, LVSIL_SMALL);
            mSearchResultCtrl->SetImageList(&sSysImgListMgr.mSysImgList32, LVSIL_NORMAL);
        }
        else
        {
            XPR_SAFE_DELETE(mSearchResultCtrl);
        }
    }

    mStatusBar = new StatusBar;
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        if (mStatusBar->Create(this, CTRL_ID_STATUS_BAR, CRect(0, 0, 0, 0)) == XPR_TRUE)
        {
            mStatusBar->addPane(kPaneIdNormal, XPR_NULL);
            mStatusBar->addPane(kPaneIdSize,   XPR_NULL);

            mStatusBar->setPaneSize(0, kPaneWidth[0], kPaneWidth[0]);
            mStatusBar->setPaneSize(1, kPaneWidth[1], kPaneWidth[1]);
        }
        else
        {
            XPR_SAFE_DELETE(mStatusBar);
        }
    }

    if (XPR_IS_NULL(mSearchResultCtrl) || XPR_IS_NULL(mStatusBar))
    {
        DESTROY_DELETE(mSearchResultCtrl);
        DESTROY_DELETE(mStatusBar);

        return -1;
    }

    return 0;
}

void SearchResultPane::OnDestroy(void)
{
    DESTROY_DELETE(mSearchResultCtrl);
    DESTROY_DELETE(mStatusBar);

    super::OnDestroy();
}

CWnd * SearchResultPane::newSubPane(xpr_uint_t aId)
{
    return mSearchResultCtrl;
}

CWnd * SearchResultPane::getSubPane(xpr_uint_t aId) const
{
    return mSearchResultCtrl;
}

xpr_size_t SearchResultPane::getSubPaneCount(void) const
{
    return 1;
}

xpr_uint_t SearchResultPane::getCurSubPaneId(void) const
{
    return mSubPaneId;
}

xpr_uint_t SearchResultPane::setCurSubPane(xpr_uint_t aId)
{
    mSubPaneId = aId;

    return mSubPaneId;
}

void SearchResultPane::destroySubPane(xpr_uint_t aId)
{
}

void SearchResultPane::destroySubPane(void)
{
}

StatusBar *SearchResultPane::getStatusBar(void) const
{
    return mStatusBar;
}

const xpr_tchar_t *SearchResultPane::getStatusPaneText(xpr_sint_t aIndex) const
{
    if (aIndex == 1)
        return mStatusText1;

    return mStatusText0;
}

void SearchResultPane::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void SearchResultPane::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mSearchResultCtrl))
    {
        mSearchResultCtrl->SetFocus();
    }
}

void SearchResultPane::recalcLayout(void)
{
    CRect sRect;
    GetClientRect(&sRect);

    HDWP sHdwp = ::BeginDeferWindowPos(2);

    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        CRect sStatusBarRect(sRect);
        sStatusBarRect.top = sStatusBarRect.bottom - mStatusBar->getDefaultHeight();

        ::DeferWindowPos(sHdwp, *mStatusBar, XPR_NULL, sStatusBarRect.left, sStatusBarRect.top, sStatusBarRect.Width(), sStatusBarRect.Height(), SWP_NOZORDER);

        sRect.bottom -= sStatusBarRect.Height();
    }

    if (XPR_IS_NOT_NULL(mSearchResultCtrl))
    {
        ::DeferWindowPos(sHdwp, *mSearchResultCtrl, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
    }

    ::EndDeferWindowPos(sHdwp);
}

SearchResultCtrl *SearchResultPane::getSearchResultCtrl(void) const
{
    return mSearchResultCtrl;
}

void SearchResultPane::onStartSearch(SearchResultCtrl &aSearchResultCtrl)
{
}

void SearchResultPane::onEndSearch(SearchResultCtrl &aSearchResultCtrl)
{
    onUpdatedResultInfo(aSearchResultCtrl);
}

void SearchResultPane::onUpdatedResultInfo(SearchResultCtrl &aSearchResultCtrl)
{
    xpr_sint_t   sResultFileCount  = 0;
    xpr_sint_t   sResultDirCount   = 0;
    xpr_sint_t   sResultTotalCount = 0;
    xpr_uint64_t sResultTotalSize  = 0;
    aSearchResultCtrl.getResultInfo(sResultFileCount, sResultDirCount, sResultTotalSize);
    sResultTotalCount = sResultFileCount + sResultDirCount;

    xpr_tchar_t sResultFileCountText[0xff] = {0};
    xpr_tchar_t sResultDirCountText [0xff] = {0};
    xpr_tchar_t sResultTotalCountText[0xff] = {0};
    fxb::GetFormatedNumber(sResultFileCount,  sResultFileCountText,  XPR_COUNT_OF(sResultFileCountText)  - 1);
    fxb::GetFormatedNumber(sResultDirCount,   sResultDirCountText,   XPR_COUNT_OF(sResultDirCountText)   - 1);
    fxb::GetFormatedNumber(sResultTotalCount, sResultTotalCountText, XPR_COUNT_OF(sResultTotalCountText) - 1);

    _stprintf(
        mStatusText0,
        theApp.loadFormatString(XPR_STRING_LITERAL("search_result.status.pane1.file_folder"),
        XPR_STRING_LITERAL("%s,%s,%s")),
        sResultTotalCountText,
        sResultDirCountText,
        sResultFileCountText);

    fxb::SizeFormat::getDefSizeFormat(sResultTotalSize, mStatusText1, XPR_COUNT_OF(mStatusText1) - 1);

    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        mStatusBar->setPaneText(0, mStatusText0);
        mStatusBar->setPaneText(1, mStatusText1);
    }
}

void SearchResultPane::onSetFocus(SearchResultCtrl &aSearchResultCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSetFocus(*this);
    }
}

void SearchResultPane::onMoveFocus(SearchResultCtrl &aSearchResultCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onMoveFocus(*this, 1);
    }
}

xpr_bool_t SearchResultPane::onOpenFolder(SearchResultCtrl &aSearchResultCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    if (XPR_IS_NULL(mObserver))
        return XPR_FALSE;

    return mObserver->onOpenFolder(*this, aDir, aSelPath);
}

xpr_bool_t SearchResultPane::onOpenFolder(SearchResultCtrl &aSearchResultCtrl, LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(mObserver))
        return XPR_FALSE;

    return mObserver->onOpenFolder(*this, aFullPidl);
}

void SearchResultPane::setChangedOption(Option &aOption)
{
}
