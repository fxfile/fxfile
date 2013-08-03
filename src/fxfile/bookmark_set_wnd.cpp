//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "bookmark_set_wnd.h"

#include "gui/MemDC.h"

#include "option.h"
#include "bookmark_wnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("BookmarkSetWnd");
static const xpr_sint_t OFFSET_MARGIN = 5;
static const xpr_sint_t OFFSET_HEIGHT = 2;

//
// control id
//
enum
{
    CTRL_ID_BOOKMARK_WND = 50,
};

BookmarkSetWnd::BookmarkSetWnd(void)
    : mBookmarkWnd(XPR_NULL)
    , mBookmarkCount(0)
    , mScrollDelta(0)
{
}

BookmarkSetWnd::~BookmarkSetWnd(void)
{
}

xpr_bool_t BookmarkSetWnd::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_VSCROLL;

    WNDCLASS sWndClass;
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

        if (!AfxRegisterClass(&sWndClass))
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return super::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(BookmarkSetWnd, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETTINGCHANGE()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

xpr_sint_t BookmarkSetWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void BookmarkSetWnd::OnDestroy(void) 
{
    super::OnDestroy();

    if (mBookmarkWnd != XPR_NULL)
    {
        xpr_sint_t i;
        for (i = 0; i < mBookmarkCount; ++i)
        {
            DESTROY_WINDOW(mBookmarkWnd[i]);
        }
        XPR_SAFE_DELETE_ARRAY(mBookmarkWnd);
    }

    mBookmarkCount = 0;
}

void BookmarkSetWnd::OnSettingChange(xpr_uint_t uFlags, const xpr_tchar_t *lpszSection) 
{
    super::OnSettingChange(uFlags, lpszSection);

}

void BookmarkSetWnd::OnPaint(void) 
{
    CPaintDC sPaintDc(this); // device context for painting

    CRect sClientRect;
    GetClientRect(sClientRect);

    CMemDC sMemDc(&sPaintDc);
    sMemDc.FillSolidRect(&sClientRect, RGB(255,255,255));
}

xpr_bool_t BookmarkSetWnd::OnEraseBkgnd(CDC* pDC) 
{
    return XPR_TRUE;
    //return super::OnEraseBkgnd(pDC);
}

void BookmarkSetWnd::updateBookmark(void)
{
    SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), XPR_NULL);

    // delete all the bookmark
    if (mBookmarkWnd != XPR_NULL)
    {
        xpr_sint_t i;
        for (i = 0; i < mBookmarkCount; ++i)
        {
            DESTROY_WINDOW(mBookmarkWnd[i]);
        }

        XPR_SAFE_DELETE_ARRAY(mBookmarkWnd);
    }

    mBookmarkWnd = XPR_NULL;
    mBookmarkCount = 0;

    if (gOpt->mConfig.mContentsStyle != CONTENTS_EXPLORER)
        return;

    if (gOpt->mConfig.mContentsBookmark == XPR_FALSE)
        return;

    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sCount;
    xpr_sint_t sCountExceptForSeparator;
    BookmarkItem *sBookmarkItem;

    sCountExceptForSeparator = 0;

    sCount = sBookmarkMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = sBookmarkMgr.getBookmark(i);
        if (sBookmarkItem == XPR_NULL)
            continue;

        if (sBookmarkItem->isSeparator() == XPR_TRUE)
            continue;

        sCountExceptForSeparator++;
    }

    if (sCountExceptForSeparator <= 0)
        return;

    CRect sRect(10,0,200,0);
    CClientDC sClientDc(this);
    CSize sTextSize = sClientDc.GetTextExtent(XPR_STRING_LITERAL("test"));

    sRect.bottom += OFFSET_MARGIN;

    mBookmarkCount = sCountExceptForSeparator;
    mBookmarkWnd = new BookmarkWnd[mBookmarkCount];

    xpr_sint_t j = 0;
    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = sBookmarkMgr.getBookmark(i);
        if (sBookmarkItem == XPR_NULL)
            continue;

        if (sBookmarkItem->isSeparator() == XPR_TRUE)
            continue;

        sRect.top    = sRect.bottom + OFFSET_HEIGHT;
        sRect.bottom = sRect.top + sTextSize.cy;

        const xpr_tchar_t *sText = sBookmarkItem->mName.c_str();
        if (sText == XPR_NULL || _tcslen(sText) == 0)
            sText = theApp.loadString(XPR_STRING_LITERAL("bookmark.no_name"));

        mBookmarkWnd[j].setBookmark(i, sBookmarkItem);
        mBookmarkWnd[j].Create(sText, WS_CHILD | WS_VISIBLE | SS_LEFT, sRect, this, CTRL_ID_BOOKMARK_WND + j);
        mBookmarkWnd[j].adjustWindow();
        mBookmarkWnd[j].Invalidate(XPR_TRUE);
        j++;
    }

    recalcLayout();
}

void BookmarkSetWnd::enableBookmark(xpr_bool_t aEnable)
{
    if (aEnable == XPR_TRUE)
        SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), XPR_NULL);

    ShowWindow((aEnable == XPR_TRUE) ? SW_SHOW : SW_HIDE);
}

void BookmarkSetWnd::setBookmarkOption(COLORREF aColor, xpr_bool_t aPopup)
{
    if (mBookmarkWnd == XPR_NULL)
        return;

    xpr_sint_t i;
    for (i = 0; i < mBookmarkCount; ++i)
    {
        if (mBookmarkWnd[i].m_hWnd != XPR_NULL)
        {
            mBookmarkWnd[i].setBookmarkColor(aColor);
            mBookmarkWnd[i].setTooltip(aPopup);
        }
    }
}

void BookmarkSetWnd::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(nType, cx, cy);

    recalcLayout();
}

void BookmarkSetWnd::recalcLayout(void)
{
    CRect sRect;
    GetClientRect(&sRect);

    CClientDC sClientDc(this);
    CSize sTextSize = sClientDc.GetTextExtent(XPR_STRING_LITERAL("test"));

    SCROLLINFO sScrollInfo = {0};
    sScrollInfo.cbSize = sizeof(SCROLLINFO);
    sScrollInfo.fMask = SIF_PAGE;
    GetScrollInfo(SB_VERT, &sScrollInfo);

    xpr_sint_t sOldPage = sScrollInfo.nPage;

    sScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
    sScrollInfo.nMax  = (OFFSET_MARGIN * 2) + ((sTextSize.cy + OFFSET_HEIGHT) * mBookmarkCount);
    sScrollInfo.nPage = sRect.Height();
    SetScrollInfo(SB_VERT, &sScrollInfo);

    sScrollInfo.fMask = SIF_POS;
    GetScrollInfo(SB_VERT, &sScrollInfo);

    if (mScrollDelta != 0 && sOldPage != sScrollInfo.nPage)
    {
        ScrollWindow(0, -(mScrollDelta + sScrollInfo.nPos), XPR_NULL, XPR_NULL);
        UpdateWindow();

        mScrollDelta -= mScrollDelta + sScrollInfo.nPos;
    }

    if (sOldPage == sScrollInfo.nPage)
        mScrollDelta = 0;
}

void BookmarkSetWnd::OnVScroll(xpr_uint_t nSBCode, xpr_uint_t nPos, CScrollBar* pScrollBar)
{
    CClientDC sClientDc(this);
    CSize sTextSize = sClientDc.GetTextExtent(XPR_STRING_LITERAL("test"));
    sTextSize.cy += OFFSET_HEIGHT;

    SCROLLINFO sScrollInfo = {0};
    sScrollInfo.cbSize = sizeof(sScrollInfo);
    sScrollInfo.fMask = SIF_ALL;
    GetScrollInfo(SB_VERT, &sScrollInfo);

    xpr_sint_t sOldPos = sScrollInfo.nPos;

    switch (nSBCode)
    {
    case SB_PAGEUP:        sScrollInfo.nPos -= sScrollInfo.nPage;  break;
    case SB_PAGEDOWN:      sScrollInfo.nPos += sScrollInfo.nPage;  break;
    case SB_LINEUP:        sScrollInfo.nPos -= (long)sTextSize.cy; break;
    case SB_LINEDOWN:      sScrollInfo.nPos += (long)sTextSize.cy; break;
    case SB_THUMBPOSITION: sScrollInfo.nPos = nPos;                break;
    case SB_THUMBTRACK:    sScrollInfo.nPos = nPos;                break;
    }

    sScrollInfo.fMask = SIF_POS;
    SetScrollInfo(SB_VERT, &sScrollInfo, XPR_TRUE);
    GetScrollInfo(SB_VERT, &sScrollInfo);

    if (sScrollInfo.nPos != sOldPos)
    {
        ScrollWindow(0, sOldPos - sScrollInfo.nPos, XPR_NULL, XPR_NULL);
        UpdateWindow();

        mScrollDelta += sOldPos - sScrollInfo.nPos;
    }

    super::OnVScroll(nSBCode, nPos, pScrollBar);
}
} // namespace fxfile
