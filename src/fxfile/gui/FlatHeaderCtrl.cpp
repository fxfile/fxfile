//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FlatHeaderCtrl.h"

#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ARROW_HEIGHT 6

FlatHeaderCtrl::FlatHeaderCtrl(void)
    : mCallDrawItem(XPR_FALSE)
    , mSortColumn(-1)
    , mBoldFont(XPR_FALSE)
    , mOffset(6)
    , mParentWnd(XPR_NULL)
    , mDragging(XPR_FALSE)
    , mTracking(XPR_FALSE)
    , mMouseHover(XPR_FALSE)
    , mMouseHoverColumn(-1)
{
}

FlatHeaderCtrl::~FlatHeaderCtrl(void)
{
}

BEGIN_MESSAGE_MAP(FlatHeaderCtrl, CHeaderCtrl)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_WINDOWPOSCHANGED()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHover)
    ON_MESSAGE(WM_MOUSELEAVE,   OnMouseLeave)
    ON_MESSAGE(WM_THEMECHANGED, OnThemeChanged)
    ON_NOTIFY_REFLECT(HDN_BEGINDRAG, OnHdnBegindrag)
    ON_NOTIFY_REFLECT(HDN_ENDDRAG,   OnHdnEnddrag)
END_MESSAGE_MAP()

void FlatHeaderCtrl::initHeader(xpr_bool_t aBoldFont)
{
    if (XPR_IS_NOT_NULL(mFont.m_hObject))
        mFont.DeleteObject();

    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(sNonClientMetrics), &sNonClientMetrics, 0);

    if (XPR_IS_TRUE(aBoldFont))
        sNonClientMetrics.lfMessageFont.lfWeight = FW_BOLD;

    mFont.CreateFontIndirect(&sNonClientMetrics.lfMessageFont);
    SetFont(&mFont);

    mBoldFont = aBoldFont;

    mXPTheme.Open(GetSafeHwnd(), XPR_WIDE_STRING_LITERAL("HEADER"));
}

xpr_sint_t FlatHeaderCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CHeaderCtrl::OnCreate(aCreateStruct) == -1)
        return -1;

    mParentWnd = CWnd::FromHandle(aCreateStruct->hwndParent);
    ASSERT(mParentWnd);

    resizeWindow();

    return 0;
}

void FlatHeaderCtrl::PreSubclassWindow(void)
{
    CHeaderCtrl::PreSubclassWindow();

    mParentWnd = GetParent();
    ASSERT(mParentWnd);

    resizeWindow();
}

xpr_sint_t FlatHeaderCtrl::setSortImage(xpr_sint_t aColumn, xpr_bool_t aAscending)
{
    xpr_sint_t aOldSortColumn = mSortColumn;

    mSortColumn = aColumn;
    mSortAscending = aAscending;

    HD_ITEM sHdItem = {0};
    sHdItem.mask = HDI_BITMAP | HDI_FORMAT;

    xpr_sint_t i;
    xpr_sint_t sCount = GetItemCount();

    for (i = 0; i < sCount; ++i)
    {
        GetItem(i, &sHdItem);

        sHdItem.fmt |= HDF_OWNERDRAW;
        SetItem(i, &sHdItem);
    }

    Invalidate();

    return aOldSortColumn;
}

void FlatHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT aDrawItemStruct)
{
    mCallDrawItem = XPR_TRUE;

    ASSERT(aDrawItemStruct != XPR_NULL);

    CDC        *sDC       = CDC::FromHandle(aDrawItemStruct->hDC);
    CRect       sItemRect = aDrawItemStruct->rcItem;
    xpr_uint_t  sState    = aDrawItemStruct->itemState;
    xpr_sint_t  sIndex    = aDrawItemStruct->itemID;

    if (XPR_IS_TRUE(mMouseHover))
    {
        if (sIndex == mMouseHoverColumn)
            sState |= ODS_HOTLIGHT;
    }

    drawItem(sDC, sItemRect, sState, sIndex);
}

void FlatHeaderCtrl::drawItem(CDC *aDC, CRect aItemRect, xpr_uint_t aState, xpr_sint_t aIndex) 
{
    CRect sIconRect;
    sIconRect = aItemRect;

    COLORREF sBtnFaceColor    = ::GetSysColor(COLOR_BTNFACE);
    COLORREF sBtnHilightColor = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF sBtnShadowColor  = ::GetSysColor(COLOR_BTNSHADOW);

    xpr_sint_t sSavedDC = aDC->SaveDC();

    HD_ITEM sHdItem = {0};
    xpr_tchar_t sText[XPR_MAX_PATH + 1];

    sHdItem.mask       = HDI_TEXT | HDI_FORMAT;
    sHdItem.pszText    = sText;
    sHdItem.cchTextMax = XPR_MAX_PATH;
    GetItem(aIndex, &sHdItem);

    xpr_uint_t sFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS;
    if (sHdItem.fmt & HDF_LEFT)        sFormat |= DT_LEFT;
    else if (sHdItem.fmt & HDF_CENTER) sFormat |= DT_CENTER;
    else if (sHdItem.fmt & HDF_RIGHT)  sFormat |= DT_RIGHT;

    // draw background
    if (mXPTheme.IsAppThemed() == XPR_TRUE)
    {
        xpr_sint_t sPartId = HP_HEADERITEM;
        xpr_sint_t sStateId = HIS_NORMAL;

        if (XPR_TEST_BITS(aState, ODS_HOTLIGHT)) sStateId = HIS_HOT;
        if (XPR_TEST_BITS(aState, ODS_SELECTED)) sStateId = HIS_PRESSED;

        mXPTheme.DrawBackground(*aDC, sPartId, sStateId, &aItemRect, XPR_NULL);
    }
    else
    {
        aDC->FillSolidRect(aItemRect, sBtnFaceColor);
        aDC->Draw3dRect(aItemRect, sBtnHilightColor, sBtnShadowColor);
    }

    aDC->SetBkMode(TRANSPARENT);

    // if mouse button is pressed
    if (XPR_TEST_BITS(aState, ODS_SELECTED))
    {
        aItemRect.left++;
        aItemRect.top += 2;
        aItemRect.right++;

        sIconRect.left++;
        sIconRect.top -= 2;
        sIconRect.right++;
    }

    if (aIndex == mSortColumn)
        aItemRect.right -= 3 * mOffset;

    aItemRect.left  += mOffset;
    aItemRect.right -= mOffset;

    // draw text
    CRect sTextRect(0,0,0,0);
    if (aItemRect.left < aItemRect.right)
    {
        if (XPR_IS_TRUE(mBoldFont))
        {
            CFont *sOldFont = aDC->SelectObject(&mFont);
            aDC->DrawText(sText, -1, &sTextRect, DT_SINGLELINE | DT_CALCRECT);
            aDC->DrawText(sText,-1,aItemRect, sFormat);
            aDC->SelectObject(sOldFont);
        }
        else
        {
            aDC->DrawText(sText, -1, &sTextRect, DT_SINGLELINE | DT_CALCRECT);
            aDC->DrawText(sText, -1, aItemRect, sFormat);
        }
    }

    if ((sHdItem.fmt & HDF_RIGHT) != HDF_RIGHT)
        sIconRect.right = aItemRect.left + sTextRect.Width() + mOffset * 4;

    // draw arrow
    if (aIndex == mSortColumn)
    {
        CBrush sBrush(sBtnShadowColor);
        POINT aPoints[3];

        if (XPR_IS_TRUE(mSortAscending))
        {
            aPoints[0].x = sIconRect.right - 2 * mOffset;         aPoints[0].y = (sIconRect.Height() - ARROW_HEIGHT) / 2;
            aPoints[1].x = sIconRect.right - 3 * mOffset / 2 + 2; aPoints[1].y = aPoints[0].y + ARROW_HEIGHT;
            aPoints[2].x = sIconRect.right - 5 * mOffset / 2 - 2; aPoints[2].y = aPoints[0].y + ARROW_HEIGHT;
        }
        else
        {
            aPoints[0].x = sIconRect.right - 3 * mOffset / 2 + 2; aPoints[0].y = (sIconRect.Height() - ARROW_HEIGHT) / 2 + 2;
            aPoints[1].x = sIconRect.right - 5 * mOffset / 2 - 1; aPoints[1].y = aPoints[0].y;
            aPoints[2].x = sIconRect.right - 2 * mOffset;         aPoints[2].y = aPoints[0].y + ARROW_HEIGHT - 1;
        }

        CRgn sRgn;
        sRgn.CreatePolygonRgn(aPoints, 3, ALTERNATE);
        aDC->FillRgn(&sRgn, &sBrush);
    }

    aDC->RestoreDC(sSavedDC);
}

void FlatHeaderCtrl::OnPaint(void)
{
    CPaintDC sPaintDC(this);
    CMemDC sMemDC(&sPaintDC);

    CRect sClientClient;
    GetClientRect(&sClientClient);

    COLORREF sBtnFaceColor = ::GetSysColor(COLOR_BTNFACE);
    sMemDC.FillSolidRect(sClientClient, sBtnFaceColor);

    mCallDrawItem = XPR_FALSE;

    CWnd::DefWindowProc(WM_PAINT, (WPARAM)sMemDC.m_hDC, 0);

    if (mXPTheme.IsAppThemed() == XPR_FALSE)
        drawFlatBorder(&sMemDC);
}

void FlatHeaderCtrl::drawFlatBorder(CDC *aDC)
{
    CRect sRect;
    GetWindowRect(&sRect);
    ScreenToClient(&sRect);

    COLORREF sBtnFaceColor    = ::GetSysColor(COLOR_BTNFACE);
    COLORREF sBtnHilightColor = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF sBtnShadowColor  = ::GetSysColor(COLOR_BTNSHADOW);

    CRect sItemRect;
    xpr_sint_t i, sCount, sTotal;
    xpr_sint_t *sItemOrders = XPR_NULL;

    HD_ITEM sHdItem = {0};
    sHdItem.fmt  = HDF_STRING | HDF_LEFT | HDF_OWNERDRAW;
    sHdItem.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT | HDI_ORDER;

    CFont *sOldFont = XPR_NULL;
    if (XPR_IS_FALSE(mCallDrawItem))
        sOldFont = aDC->SelectObject(GetFont());

    sTotal = 0;
    sCount = GetItemCount();
    if (sCount > 0)
    {
        sItemOrders = new xpr_sint_t[sCount];

        for (i = 0; i < sCount; ++i)
        {
            GetItem(i, &sHdItem);
            sItemOrders[sHdItem.iOrder] = i;

            sItemRect.left = sTotal;

            sTotal += sHdItem.cxy;

            sItemRect.right  = sTotal;
            sItemRect.top    = sRect.top;
            sItemRect.bottom = sRect.bottom;

            if (XPR_IS_FALSE(mCallDrawItem))
            {
                drawItem(aDC, sItemRect, 0, i);
            }
        }
    }

    if (XPR_IS_NOT_NULL(sOldFont))
        aDC->SelectObject(sOldFont);

    aDC->Draw3dRect(sRect, sBtnFaceColor, sBtnFaceColor);

    sRect.DeflateRect(1,1);
    aDC->Draw3dRect(sRect, sBtnFaceColor, sBtnFaceColor);

    sRect.InflateRect(1,1);
    aDC->Draw3dRect(sRect, sBtnHilightColor, sBtnShadowColor);

    CPen sHighlightPen(PS_SOLID, 1, sBtnHilightColor);
    CPen sShadowPen(PS_SOLID, 1, sBtnShadowColor);
    CPen sFacePen(PS_SOLID, 1, sBtnFaceColor);

    CPen *sOldPen = aDC->SelectObject(&sHighlightPen);

    xpr_sint_t cx = 0;
    for (i = 0; i < sCount; ++i)
    {
        sHdItem.fmt  = HDF_STRING | HDF_LEFT | HDF_OWNERDRAW;
        sHdItem.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT;
        GetItem(sItemOrders[i], &sHdItem);

        cx += sHdItem.cxy;

        aDC->SelectObject(&sShadowPen);
        aDC->MoveTo(cx, 2);
        aDC->LineTo(cx, sRect.bottom - 2);

        aDC->SelectObject(&sHighlightPen);
        aDC->MoveTo(cx + 1, 2);
        aDC->LineTo(cx + 1, sRect.bottom - 2);

        aDC->SelectObject(&sFacePen);
        aDC->MoveTo(cx - 1, 2);
        aDC->LineTo(cx - 1, sRect.bottom - 2);

        aDC->SelectObject(&sFacePen);
        aDC->MoveTo(cx - 2, 2);
        aDC->LineTo(cx - 2, sRect.bottom - 2);
    }

    CRect sFillRect = sRect;
    sFillRect.left += sTotal;
    sFillRect.DeflateRect(1,1,1,1);
    aDC->FillSolidRect(&sFillRect, sBtnFaceColor);

    aDC->SelectObject(sOldPen);

    XPR_SAFE_DELETE_ARRAY(sItemOrders);
    DELETE_OBJECT(sHighlightPen);
    DELETE_OBJECT(sShadowPen);
    DELETE_OBJECT(sFacePen);
}

void FlatHeaderCtrl::OnWindowPosChanged(WINDOWPOS FAR *aWindowPos)
{
    CHeaderCtrl::OnWindowPosChanged(aWindowPos);

    Invalidate(XPR_FALSE);
}

void FlatHeaderCtrl::resizeWindow(void)
{
    ASSERT_VALID(this);
    ASSERT_VALID(mParentWnd);

    CRect sParentRect;
    mParentWnd->GetClientRect(&sParentRect);

    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    SetWindowPos(XPR_NULL, 0, 0, sParentRect.Width(), sWindowRect.Height(), SWP_NOMOVE|SWP_FRAMECHANGED);
}

xpr_bool_t FlatHeaderCtrl::OnEraseBkgnd(CDC *aDC)
{
    return CHeaderCtrl::OnEraseBkgnd(aDC);
}

void FlatHeaderCtrl::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    if (XPR_IS_FALSE(mDragging))
    {
        if (aFlags == MK_LBUTTON)
            Invalidate(XPR_FALSE);

        if (XPR_IS_FALSE(mTracking))
        {
            TRACKMOUSEEVENT sTrackMouseEvent = {0};
            sTrackMouseEvent.cbSize      = sizeof(sTrackMouseEvent);
            sTrackMouseEvent.hwndTrack   = m_hWnd;
            sTrackMouseEvent.dwFlags     = TME_HOVER | TME_LEAVE;
            sTrackMouseEvent.dwHoverTime = 1;

            mTracking = _TrackMouseEvent(&sTrackMouseEvent) != XPR_FALSE;
        }

        if (XPR_IS_TRUE(mTracking))
        {
            xpr_sint_t sOldMouseHoverColumn = mMouseHoverColumn;
            mMouseHoverColumn = -1;

            xpr_sint_t i, sCount;
            CRect sItemRect;

            sCount = GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                GetItemRect(i, &sItemRect);
                if (sItemRect.PtInRect(aPoint) == XPR_TRUE)
                {
                    mMouseHoverColumn = i;
                    break;
                }
            }

            if (sOldMouseHoverColumn != mMouseHoverColumn)
                Invalidate(XPR_FALSE);
        }
    }

    CHeaderCtrl::OnMouseMove(aFlags, aPoint);
}

LRESULT FlatHeaderCtrl::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
    CPoint sPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    mMouseHover = XPR_TRUE;
    Invalidate();

    return 1;
}

LRESULT FlatHeaderCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    CPoint sPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    mTracking         = XPR_FALSE;
    mMouseHover       = XPR_FALSE;
    mMouseHoverColumn = -1;
    Invalidate(XPR_FALSE);

    return 0;
}

LRESULT FlatHeaderCtrl::OnThemeChanged(WPARAM wParam, LPARAM lParam)
{
    if (mXPTheme.IsAppThemed() == XPR_TRUE)
        mXPTheme.Close();

    mXPTheme.Open(GetSafeHwnd(), XPR_WIDE_STRING_LITERAL("HEADER"));

    Invalidate(XPR_FALSE);

    return 0;
}

void FlatHeaderCtrl::OnHdnBegindrag(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMHEADER sNmHeader = reinterpret_cast<LPNMHEADER>(aNmHdr);
    *aResult = 1;

    mDragging = XPR_FALSE;
}

void FlatHeaderCtrl::OnHdnEnddrag(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMHEADER sNmHeader = reinterpret_cast<LPNMHEADER>(aNmHdr);
    *aResult = 0;

    mDragging = XPR_FALSE;
}
