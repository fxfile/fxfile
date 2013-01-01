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
#include "SearchResultPaneObserver.h"
#include "SearchResultCtrl.h"
#include "CtrlId.h"

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
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mSearchResultCtrl(XPR_NULL)
    , mStatusBar(XPR_NULL)
{
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

    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    //Check weather the class is registerd already
    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        //If not then we have to register the new class
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = kClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

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

void SearchResultPane::setObserver(SearchResultPaneObserver *aObserver)
{
    mObserver = aObserver;
}

void SearchResultPane::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

xpr_sint_t SearchResultPane::getViewIndex(void) const
{
    return mViewIndex;
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
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        xpr_sint_t   sResultFileCount = 0;
        xpr_sint_t   sResultDirCount  = 0;
        xpr_uint64_t sResultTotalSize = 0;
        aSearchResultCtrl.getResultInfo(sResultFileCount, sResultDirCount, sResultTotalSize);

        xpr_tchar_t sStatusText0[0xff] = {0};
        xpr_tchar_t sStatusText1[0xff] = {0};

        _stprintf(
            sStatusText0,
            theApp.loadFormatString(XPR_STRING_LITERAL("search_result.status.pane1.file_folder"),
            XPR_STRING_LITERAL("%d,%d,%d")),
            sResultDirCount + sResultFileCount,
            sResultDirCount,
            sResultFileCount);

        fxb::SizeFormat::getDefSizeFormat(sResultTotalSize, sStatusText1, XPR_COUNT_OF(sStatusText1) - 1);

        mStatusBar->setPaneText(0, sStatusText0);
        mStatusBar->setPaneText(1, sStatusText1);
    }
}

void SearchResultPane::onSetFocus(SearchResultCtrl &aSearchResultCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSetFocus(*this);
    }
}
