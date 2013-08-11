//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TextProgressCtrl.h"

#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TextProgressCtrl::TextProgressCtrl(void)
    : mMin(0), mMax(100), mPos(0)
    , mShowText(XPR_TRUE)
    , mBoldFont(XPR_FALSE)
{
    mProgressColor     = ::GetSysColor(COLOR_HIGHLIGHT);
    mBackColor         = ::GetSysColor(COLOR_WINDOW);
    mTextProgressColor = ::GetSysColor(COLOR_HIGHLIGHT);
    mTextBackColor     = ::GetSysColor(COLOR_WINDOW);
}

TextProgressCtrl::~TextProgressCtrl(void)
{
}

BEGIN_MESSAGE_MAP(TextProgressCtrl, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

xpr_sint_t TextProgressCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    setBoldFont(mBoldFont);

    DWORD sClassStyle = ::GetClassLongPtr(m_hWnd, GCL_STYLE);
    ::SetClassLongPtr(m_hWnd, GCL_STYLE, sClassStyle | CS_DBLCLKS);

    return 0;
}

void TextProgressCtrl::setBoldFont(xpr_bool_t aBold)
{
    if (XPR_IS_NOT_NULL(mTextFont.m_hObject))
        mTextFont.DeleteObject();

    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &sNonClientMetrics, 0);

    if (XPR_IS_TRUE(aBold))
        sNonClientMetrics.lfMenuFont.lfWeight = FW_BOLD;

    if (mTextFont.CreateFontIndirect(&sNonClientMetrics.lfMenuFont) == XPR_FALSE)
        mTextFont.CreateStockObject(DEFAULT_GUI_FONT);

    if (::IsWindow(m_hWnd) == XPR_TRUE)
        SetFont(&mTextFont);

    mBoldFont = aBold;
}

void TextProgressCtrl::OnDestroy(void)
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mTextFont.m_hObject))
        mTextFont.DeleteObject();
}

void TextProgressCtrl::setProgressColor(COLORREF aColor)
{
    mProgressColor = aColor;
}

void TextProgressCtrl::setBackColor(COLORREF aColor)
{
    mBackColor = aColor;
}

void TextProgressCtrl::setTextProgressColor(COLORREF aColor)
{
    mTextProgressColor = aColor;
}

void TextProgressCtrl::setTextBackColor(COLORREF aColor)
{
    mTextBackColor = aColor;
}

void TextProgressCtrl::setShowText(xpr_bool_t aShow)
{ 
    mShowText = aShow;
}

void TextProgressCtrl::setRange(xpr_sint_t aMin, xpr_sint_t aMax)
{
    if (aMin > aMax)
        return;

    mMin = aMin;
    mMax = aMax;
}

xpr_sint_t TextProgressCtrl::setPos(xpr_sint_t aPos)
{
    if (mPos == aPos)
        return mPos;

    xpr_sint_t sOldPos = mPos;
    mPos = aPos;

    if (::IsWindow(m_hWnd) == XPR_TRUE)
        RedrawWindow();

    return sOldPos;
}

void TextProgressCtrl::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

}

void TextProgressCtrl::OnNcPaint(void)
{
}

void TextProgressCtrl::OnPaint(void)
{
    CPaintDC sPaintDC(this);

    CMemDC sMemDC(&sPaintDC);

    CRect sClientRect;
    GetClientRect(sClientRect);

    xpr_double_t sFraction = 0.0;
    if ((mMax - mMin) > 0)
        sFraction = (xpr_double_t)(mPos - mMin) / ((xpr_double_t)(mMax - mMin));

    CRect sProgressRect(sClientRect);
    sProgressRect.right = sProgressRect.left + (xpr_sint_t)(sProgressRect.Width() * sFraction);

    CRect sBackRect(sClientRect);
    sBackRect.left = sProgressRect.right;

    sMemDC.FillSolidRect(sProgressRect, mProgressColor);
    sMemDC.FillSolidRect(sBackRect, mBackColor);

    if (XPR_IS_TRUE(mShowText))
    {
        xpr_tchar_t sText[0xff] = {0};
        _stprintf(sText, XPR_STRING_LITERAL("%d%%"), (xpr_sint_t)(sFraction * 100.0));

        CFont *sOldFont = sMemDC.SelectObject(&mTextFont);
        sMemDC.SetBkMode(TRANSPARENT);

        CRgn sRgn;
        sRgn.CreateRectRgn(sProgressRect.left, sProgressRect.top, sProgressRect.right, sProgressRect.bottom);
        sMemDC.SelectClipRgn(&sRgn);
        sMemDC.SetTextColor(mTextBackColor);

        sMemDC.DrawText(sText, sClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        sRgn.DeleteObject();
        sRgn.CreateRectRgn(sBackRect.left, sBackRect.top, sBackRect.right, sBackRect.bottom);
        sMemDC.SelectClipRgn(&sRgn);
        sMemDC.SetTextColor(mTextProgressColor);

        sMemDC.DrawText(sText, sClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        sMemDC.SelectObject(sOldFont);
    }
}

xpr_bool_t TextProgressCtrl::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE;
}

void TextProgressCtrl::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint)
{
    GetParent()->SendMessage(WM_PROGRESS_DBLCK, (WPARAM)aFlags, (LPARAM)&aPoint);

    super::OnLButtonDblClk(aFlags, aPoint);
}
