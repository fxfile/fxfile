//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "bookmark_wnd.h"

#include "option.h"
#include "main_frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
#define TOOLTIP_ID 100

BookmarkWnd::BookmarkWnd(void)
    : mBookmarkItem(XPR_NULL)
    , mBookmarkActive(XPR_FALSE)
    , mBookmarkColor(DEF_INFO_BAR_BOOKMARK_COLOR)
    , mCursor(XPR_NULL)
    , mIndex(-1)
{
}

BookmarkWnd::~BookmarkWnd(void)
{
    mTextFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(BookmarkWnd, CStatic)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONUP()
    ON_WM_NCHITTEST()
    ON_WM_LBUTTONDOWN()
    ON_WM_CREATE()
    ON_WM_CONTEXTMENU()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

xpr_sint_t BookmarkWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CStatic::OnCreate(lpCreateStruct) == -1)
        return -1;

    CFont sFont;
    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &sNonClientMetrics, 0);
    sNonClientMetrics.lfMenuFont.lfUnderline = XPR_TRUE;
    if (!sFont.CreateFontIndirect(&sNonClientMetrics.lfMenuFont))
        sFont.CreateStockObject(DEFAULT_GUI_FONT);
    SetFont(&sFont);

    mToolTipCtrl.Create(this);

    if (gOpt->mConfig.mBookmarkTooltip == XPR_TRUE)
    {
        CRect sRect;
        GetClientRect(&sRect);

        xpr_tchar_t sTip[XPR_MAX_PATH + 50] = {0};
        mBookmarkItem->getTooltip(sTip, XPR_TRUE);

        mToolTipCtrl.AddTool(this, sTip, sRect, TOOLTIP_ID);
        mToolTipCtrl.SetMaxTipWidth(500);
    }

    return 0;
}

void BookmarkWnd::PreSubclassWindow(void) 
{
    if (mBookmarkItem->mName.empty() == XPR_TRUE)
    {
        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
        GetWindowText(sName, XPR_MAX_PATH);

        mBookmarkItem->mName = sName;
    }

    // Adjust Child Window
    adjustWindow();

    mCursor = ::LoadCursor(XPR_NULL, IDC_HAND);

    CStatic::PreSubclassWindow();
}

void BookmarkWnd::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    mToolTipCtrl.Update();
}

xpr_bool_t BookmarkWnd::PreTranslateMessage(MSG* pMsg) 
{
    if (mToolTipCtrl.GetToolCount() > 0)
        mToolTipCtrl.RelayEvent(pMsg);

    return CStatic::PreTranslateMessage(pMsg);
}

void BookmarkWnd::setBookmarkColor(COLORREF aBookmarkColor)
{
    mBookmarkColor = aBookmarkColor;
}

HBRUSH BookmarkWnd::CtlColor(CDC* pDC, xpr_uint_t nCtlColor) 
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

    pDC->SetTextColor(mBookmarkColor);
    pDC->SetBkMode(TRANSPARENT);

    return (HBRUSH)GetStockObject(WHITE_BRUSH);//(HBRUSH)GetStockObject(NULL_BRUSH);
}

LRESULT BookmarkWnd::OnNcHitTest(CPoint aPoint) 
{
    return HTCLIENT;
}

void BookmarkWnd::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint) 
{
    mBookmarkActive = XPR_TRUE;
}

void BookmarkWnd::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint) 
{
    if (mBookmarkActive == XPR_TRUE)
        gotoBookmark();
}

void BookmarkWnd::OnMouseMove(xpr_uint_t nFlags, CPoint pt)
{
    CStatic::OnMouseMove(nFlags, pt);
}

xpr_bool_t BookmarkWnd::OnSetCursor(CWnd *aWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage) 
{
    if (mCursor != XPR_NULL)
    {
        ::SetCursor(mCursor);
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

void BookmarkWnd::SetFont(CFont *aFont)
{
    ASSERT(::IsWindow(GetSafeHwnd()));
    ASSERT(aFont != XPR_NULL);

    ShowWindow(SW_HIDE);

    LOGFONT sLogFont = {0};
    aFont->GetLogFont(&sLogFont);

    mTextFont.DeleteObject();
    mTextFont.CreateFontIndirect(&sLogFont);

    CStatic::SetFont(&mTextFont);

    adjustWindow();
    ShowWindow(SW_SHOW);
}

void BookmarkWnd::setBookmark(xpr_sint_t aIndex, BookmarkItem *aBookmarkItem)
{
    mIndex = aIndex;
    mBookmarkItem = aBookmarkItem;

    if (::IsWindow(GetSafeHwnd()) == XPR_TRUE)
    {
        ShowWindow(SW_HIDE);
        adjustWindow();
        mToolTipCtrl.UpdateTipText(mBookmarkItem->mName.c_str(), this, TOOLTIP_ID);
        ShowWindow(SW_SHOW);
    }
}

void BookmarkWnd::getBookmark(xpr_sint_t *aIndex, BookmarkItem **aBookmarkItem)
{
    if (aIndex        != XPR_NULL) *aIndex = mIndex;
    if (aBookmarkItem != XPR_NULL) *aBookmarkItem = mBookmarkItem;
}

void BookmarkWnd::SetWindowText(const xpr_tchar_t *aText)
{
    ASSERT(aText != XPR_NULL);

    if (::IsWindow(GetSafeHwnd()))
    {
        ShowWindow(SW_HIDE);
        CStatic::SetWindowText(aText);
        adjustWindow();
        ShowWindow(SW_SHOW);
    }
}

void BookmarkWnd::adjustWindow(void)
{
    ASSERT(::IsWindow(GetSafeHwnd()));

    CRect sWndRect;
    GetWindowRect(sWndRect);

    CWnd *sParentWnd = GetParent();
    if (sParentWnd != XPR_NULL)
        sParentWnd->ScreenToClient(sWndRect);

    CRect sClientRect;
    GetClientRect(sClientRect);

    xpr_sint_t sBorderWidth  = sWndRect.Width()  - sClientRect.Width();
    xpr_sint_t sBorderHeight = sWndRect.Height() - sClientRect.Height();

    CString strWndText;
    GetWindowText(strWndText);

    CDC *sDc = GetDC();
    CFont *sOldFont = sDc->SelectObject(&mTextFont);
    CSize szExtent = sDc->GetTextExtent(strWndText);
    sDc->SelectObject(sOldFont);
    ReleaseDC(sDc);

    sWndRect.bottom = sWndRect.top + szExtent.cy;
    sWndRect.right = sWndRect.left + szExtent.cx;

    MoveWindow(sWndRect.left, sWndRect.top, sWndRect.Width() + sBorderWidth, sWndRect.Height() + sBorderHeight);
}

void BookmarkWnd::gotoBookmark(void)
{
    gFrame->gotoBookmark(mIndex);
}

void BookmarkWnd::setTooltip(xpr_bool_t aPopup)
{
    mToolTipCtrl.DelTool(this, TOOLTIP_ID);

    if (aPopup == XPR_TRUE)
    {
        CRect sRect;
        GetClientRect(&sRect);

        xpr_tchar_t sTip[XPR_MAX_PATH + 50];
        _stprintf(sTip, XPR_STRING_LITERAL("%s: \"%s\""), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.path")), mBookmarkItem->mPath.c_str());
        if (mBookmarkItem->mHotKey > 0)
            _stprintf(sTip, XPR_STRING_LITERAL("%s\r\n%s: Ctrl+%c"), sTip, theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey")), LOWORD(mBookmarkItem->mHotKey));
        else
            _stprintf(sTip, XPR_STRING_LITERAL("%s\r\n%s: %s"), sTip, theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey")), theApp.loadString(XPR_STRING_LITERAL("bookmark.tooltip.hotkey.none")));

        mToolTipCtrl.AddTool(this, sTip, sRect, TOOLTIP_ID);
        mToolTipCtrl.SetMaxTipWidth(500);
    }
}
} // namespace fxfile
