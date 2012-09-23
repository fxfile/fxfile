//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "Print.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CRect kMargin(200, 150, 200, 150);

Print::Print(void)
{
}

Print::~Print(void)
{
}

xpr_bool_t Print::onPreparePrinting(CPrintInfo *aPrintInfo)
{
    mRatioX     = 0;
    mPaperRect  = CRect(0,0,0,0);
    mPageRect   = CRect(0,0,0,0);
    mMarginRect = kMargin;

    return XPR_TRUE;
}

void Print::onBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    ASSERT(aDC != XPR_NULL && aPrintInfo != XPR_NULL);

    mPaperRect = getPaperRect(aDC);
    mPageRect  = getPageRect();
    mRatioX    = getTextRatioX(aDC);
}

void Print::onPrint(CDC *aDC, CPrintInfo *aPrintInfo)
{
#ifdef XPR_CFG_BUILD_DEBUG

    drawRect(aDC, mPaperRect, RGB(0,0,0));
    drawRect(aDC, mPageRect, RGB(0,0,255));

#endif
}

void Print::onEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
}

#ifdef XPR_CFG_BUILD_DEBUG
void Print::drawRect(CDC *aDC, CRect aRect, COLORREF aColor)
{
    UNUSED_ALWAYS(aDC);
    UNUSED_ALWAYS(aRect);
    UNUSED_ALWAYS(aColor);

#if 0
    CPen sPen;
    sPen.CreatePen(PS_SOLID, 3, aColor);
    CPen *sOldPen = aDC->SelectObject(&sPen);
    aDC->Rectangle(aRect);
    aDC->SelectObject(sOldPen);
#endif
}
#endif // XPR_CFG_BUILD_DEBUG

CRect Print::getPaperRect(CDC *aDC)
{
    ASSERT(aDC != XPR_NULL);

    CSize aPaperSize = CSize(aDC->GetDeviceCaps(HORZRES), aDC->GetDeviceCaps(VERTRES));

    return CRect(0, 0, aPaperSize.cx, aPaperSize.cy);
}

CRect Print::getPageRect(void)
{
    ASSERT(mPaperRect != CRect(0,0,0,0));

    CRect sRect = mPaperRect;
    sRect.DeflateRect(mMarginRect.left, mMarginRect.top, mMarginRect.right, mMarginRect.bottom);

    return sRect;
}

CSize Print::getCharSize(CDC *aDC, CFont *aFont)
{
    ASSERT(aDC != XPR_NULL && aFont != XPR_NULL);

    CFont *sOldFont = aDC->SelectObject(aFont);

    CSize sCharSize = aDC->GetTextExtent(XPR_STRING_LITERAL("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"),52);
    sCharSize.cx /= 52;

    aDC->SelectObject(sOldFont);

    return sCharSize;
}

xpr_float_t Print::getTextRatioX(CDC *aDC)
{
    ASSERT(aDC != XPR_NULL);

    CClientDC sScreenDC(XPR_NULL);

    TEXTMETRIC sSrcTextMetrics, sDstTextMetrics;
    sScreenDC.GetTextMetrics(&sSrcTextMetrics);
    aDC->GetTextMetrics(&sDstTextMetrics);

    //return ((xpr_float_t)sDstTextMetrics.tmAveCharWidth) / ((xpr_float_t)sSrcTextMetrics.tmAveCharWidth);
    return ((xpr_float_t)sDstTextMetrics.tmDigitizedAspectX) / ((xpr_float_t)sSrcTextMetrics.tmDigitizedAspectX);
}

CFont *Print::createFont(CDC *aDC, CString aName, xpr_sint_t aPoints, xpr_bool_t aBold, xpr_bool_t aItalic)
{
    ASSERT(aDC != XPR_NULL);

    if (XPR_IS_NULL(aDC) || aPoints < 0)
        return XPR_NULL;

    CClientDC sScreenDC(XPR_NULL);

    CFont *sCurFont = sScreenDC.GetCurrentFont();

    LOGFONT sLogFont = {0};
    sCurFont->GetLogFont(&sLogFont);

    TEXTMETRIC sTextMetrics = {0};
    sScreenDC.GetTextMetrics(&sTextMetrics);

    // get paper, screen ppi (pixels per inch)
    CSize sPaperSize(aDC->GetDeviceCaps(LOGPIXELSX), aDC->GetDeviceCaps(LOGPIXELSY));
    CSize sScreenSize(sScreenDC.GetDeviceCaps(LOGPIXELSX), sScreenDC.GetDeviceCaps(LOGPIXELSY));

    if (aPoints == 0)
        aPoints = MulDiv((sTextMetrics.tmHeight - sTextMetrics.tmInternalLeading), 72, sScreenSize.cy);

    xpr_sint_t sHeight = -MulDiv(aPoints, sPaperSize.cy, 72);

    HFONT sFontHandle = fxb::CreateFont(aName, sHeight, aBold, aItalic);
    if (XPR_IS_NULL(sFontHandle))
        return XPR_NULL;

    CFont *sFont = new CFont;
    sFont->Attach(sFontHandle);

    return sFont;
}

void Print::setAppName(const CString &aName)
{
    mAppName = aName;
}

void Print::setDocTitle(const CString &aName)
{
    mDocName = aName;
}

void Print::setMarginPixel(CRect aMarginRect)
{
    mMarginRect = aMarginRect;
}

void Print::getMarginPixel(CRect &aMarginRect)
{
    aMarginRect = mMarginRect;
}
