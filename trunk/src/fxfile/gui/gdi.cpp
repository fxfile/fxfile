//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "gdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COLORREF CalculateColor(COLORREF aFrontColor, COLORREF aBackColor, xpr_sint_t aAlpha)
{
    xpr_float_t sFrontRed   = GetRValue(aFrontColor);
    xpr_float_t sFrontGreen = GetGValue(aFrontColor);
    xpr_float_t sFrontBlue  = GetBValue(aFrontColor);
    xpr_float_t sBackRed    = GetRValue(aBackColor);
    xpr_float_t sBackGreen  = GetGValue(aBackColor);
    xpr_float_t sBackBlue   = GetBValue(aBackColor);

    xpr_float_t r = sFrontRed   * aAlpha / 255 + sBackRed   * ((xpr_float_t)(255 - aAlpha) / 255);
    xpr_float_t g = sFrontGreen * aAlpha / 255 + sBackGreen * ((xpr_float_t)(255 - aAlpha) / 255);
    xpr_float_t b = sFrontBlue  * aAlpha / 255 + sBackBlue  * ((xpr_float_t)(255 - aAlpha) / 255);

    return (RGB((xpr_byte_t)r, (xpr_byte_t)g, (xpr_byte_t)b));
}

void DrawHorzGradient(CDC *aDC, CRect aRect, COLORREF aBeginColor, COLORREF aEndColor)
{
    xpr_sint_t r1 = GetRValue(aBeginColor); 
    xpr_sint_t g1 = GetGValue(aBeginColor);
    xpr_sint_t b1 = GetBValue(aBeginColor);
    xpr_sint_t r2 = GetRValue(aEndColor);
    xpr_sint_t g2 = GetGValue(aEndColor);
    xpr_sint_t b2 = GetBValue(aEndColor);

    CRect sRect2;
    xpr_sint_t i;
    xpr_sint_t r, g, b;
    HBRUSH sBrush;

    for (i = 0; i < aRect.right; ++i) // loop to create the gradient
    {
        r = r1 + (i * (r2-r1) / aRect.right);
        g = g1 + (i * (g2-g1) / aRect.right);
        b = b1 + (i * (b2-b1) / aRect.right);
        sBrush = ::CreateSolidBrush(RGB(r, g, b));

        sRect2.SetRect(aRect.left + i, aRect.top, aRect.left + i + 1, aRect.bottom + 1);
        FillRect(aDC->m_hDC, sRect2, sBrush);

        ::DeleteObject(sBrush);
    }
}

void DrawVertGradient(CDC *aDC, CRect aRect, COLORREF aBeginColor, COLORREF aEndColor)
{
    xpr_sint_t r1 = GetRValue(aBeginColor);
    xpr_sint_t g1 = GetGValue(aBeginColor);
    xpr_sint_t b1 = GetBValue(aBeginColor);
    xpr_sint_t r2 = GetRValue(aEndColor);
    xpr_sint_t g2 = GetGValue(aEndColor);
    xpr_sint_t b2 = GetBValue(aEndColor);

    CRect sRect2;
    xpr_sint_t i;
    xpr_sint_t r, g, b;
    HBRUSH sBrush;

    for (i = 0; i < aRect.Height(); ++i) // loop to create the gradient
    {
        r = r1 + (i * (r2-r1) / aRect.Height());
        g = g1 + (i * (g2-g1) / aRect.Height());
        b = b1 + (i * (b2-b1) / aRect.Height());
        sBrush = CreateSolidBrush(RGB(r, g, b));

        sRect2.SetRect(aRect.left, aRect.top + i, aRect.right + 1, aRect.top + i + 1);
        FillRect(aDC->m_hDC, sRect2, sBrush);

        ::DeleteObject(sBrush);
    }
}

COLORREF LightenColor(COLORREF aRgb, xpr_double_t aFactor)
{
    if (aFactor > 0.0 && aFactor <= 1.0)
    {
        xpr_byte_t r = GetRValue(aRgb);
        xpr_byte_t g = GetGValue(aRgb);
        xpr_byte_t b = GetBValue(aRgb);

        xpr_byte_t sLightR = (xpr_byte_t)((aFactor * (255 - r)) + r);
        xpr_byte_t sLightG = (xpr_byte_t)((aFactor * (255 - g)) + g);
        xpr_byte_t sLightB = (xpr_byte_t)((aFactor * (255 - b)) + b);

        aRgb = RGB(sLightR, sLightG, sLightB);
    }

    return aRgb;
}

xpr_bool_t DuplicateImgList(CImageList *aSrcImgList, CImageList *aDstImgList)
{
    if (XPR_IS_NULL(aSrcImgList) || XPR_IS_NULL(aDstImgList))
        return XPR_FALSE;

    HIMAGELIST sImgList = ImageList_Duplicate(aSrcImgList->m_hImageList);
    if (XPR_IS_NULL(sImgList))
        return XPR_FALSE;

    if (aDstImgList->m_hImageList)
        aDstImgList->DeleteImageList();

    return aDstImgList->Attach(sImgList);
}

xpr_bool_t LoadImgList(CImageList *aImgList, HBITMAP aBitmap, CSize aIconSize, xpr_sint_t aOverlayCount)
{
    if (XPR_IS_NULL(aImgList) || XPR_IS_NULL(aBitmap))
        return XPR_FALSE;

    CBitmap sBitmap;
    sBitmap.Attach(aBitmap);

    if (XPR_IS_NOT_NULL(aImgList))
    {
        if (aImgList->m_hImageList)
            aImgList->DeleteImageList();

        aImgList->Create(aIconSize.cx, aIconSize.cy, ILC_COLOR16 | ILC_MASK, -1, -1);
        aImgList->Add(&sBitmap, RGB(255,0,255));
    }

    sBitmap.DeleteObject();

    xpr_sint_t i;
    for (i = 0; i < aOverlayCount; ++i)
        aImgList->SetOverlayImage(i, i+1);

    return XPR_TRUE;
}

xpr_bool_t LoadImgList(CImageList *aImgList, xpr_uint_t aBitmapId, CSize aIconSize, xpr_sint_t aOverlayCount)
{
    if (XPR_IS_NULL(aImgList) || aBitmapId <= 0)
        return XPR_FALSE;

    HBITMAP sBitmap = (HBITMAP)::LoadImage(
        AfxGetApp()->m_hInstance,
        MAKEINTRESOURCE(aBitmapId),
        IMAGE_BITMAP,
        0,
        0,
        LR_DEFAULTSIZE);

    if (XPR_IS_NULL(sBitmap))
        return XPR_FALSE;

    return LoadImgList(aImgList, sBitmap, aIconSize, aOverlayCount);
}

xpr_bool_t LoadImgList(CImageList *aImgList, const xpr_tchar_t *aPath, CSize aIconSize, xpr_sint_t aOverlayCount)
{
    if (XPR_IS_NULL(aImgList) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    HBITMAP sBitmap = (HBITMAP)::LoadImage(
        XPR_NULL,
        aPath,
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE);

    if (XPR_IS_NULL(sBitmap))
        return XPR_FALSE;

    return LoadImgList(aImgList, sBitmap, aIconSize, aOverlayCount);
}

HFONT CreateFont(const xpr_tchar_t *aFaceName,
                 xpr_sint_t         aHeight,
                 xpr_bool_t         aBold,
                 xpr_bool_t         aItalic,
                 xpr_bool_t         aUnderLine,
                 xpr_bool_t         aStrikeOut,
                 xpr_sint_t         aWidth)
{
    if (XPR_IS_NULL(aFaceName))
        return XPR_NULL;

    if (_tcslen(aFaceName) >= LF_FACESIZE)
        return XPR_NULL;

    LOGFONT sLogFont = {0};
    sLogFont.lfHeight    = aHeight;
    sLogFont.lfWidth     = (aWidth == -1) ? 0 : aWidth;
    sLogFont.lfWeight    = XPR_IS_TRUE(aBold) ? FW_BOLD : FW_NORMAL;
    sLogFont.lfItalic    = aItalic;
    sLogFont.lfUnderline = aUnderLine;
    sLogFont.lfStrikeOut = aStrikeOut;
    sLogFont.lfCharSet   = DEFAULT_CHARSET;
    _tcscpy(sLogFont.lfFaceName, aFaceName);

    return ::CreateFontIndirect(&sLogFont);
}


xpr_bool_t FontDlgToString(const xpr_tchar_t *aFaceName,
                           xpr_sint_t         aSize,
                           xpr_sint_t         aWeight,
                           xpr_bool_t         aItalic,
                           xpr_bool_t         aStrikeOut,
                           xpr_bool_t         aUnderline,
                           xpr_tchar_t       *aFontText)
{
    if (XPR_IS_NULL(aFaceName) || XPR_IS_NULL(aFontText))
        return XPR_FALSE;

    _stprintf(aFontText,
              XPR_STRING_LITERAL("%s,%d,%d,%d,%d,%d"),
              aFaceName,
              aSize / 10,
              aWeight,
              aItalic,
              aStrikeOut,
              aUnderline);

    return XPR_TRUE;
}

xpr_bool_t StringToLogFont(const xpr_tchar_t *aFontText, LOGFONT &aLogFont)
{
    if (XPR_IS_NULL(aFontText))
        return XPR_FALSE;

    xpr_tchar_t sFontText[0xff] = {0};
    _tcscpy(sFontText, aFontText);

    memset(&aLogFont, 0, sizeof(LOGFONT));

    xpr_sint_t i;
    xpr_tchar_t *sSplit = sFontText;
    xpr_tchar_t *sSplit2;
    for (i = 0; i <= 5; ++i)
    {
        if (*sSplit == XPR_STRING_LITERAL('\0'))
            break;

        sSplit2 = _tcschr(sSplit, XPR_STRING_LITERAL(','));
        if (XPR_IS_NOT_NULL(sSplit2))
            *sSplit2 = XPR_STRING_LITERAL('\0');

        switch (i)
        {
        case 0: _tcscpy(aLogFont.lfFaceName, sSplit);                               break;
        case 1: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfHeight);     break;
        case 2: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfWeight);     break;
        case 3: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfItalic);     break;
        case 4: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfStrikeOut);  break;
        case 5: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfUnderline);  break;
        }

        sSplit += _tcslen(sSplit) + 1;
    }

    CClientDC sClientDC(CWnd::GetDesktopWindow());
    aLogFont.lfCharSet = DEFAULT_CHARSET;
    aLogFont.lfHeight  = -MulDiv(aLogFont.lfHeight, sClientDC.GetDeviceCaps(LOGPIXELSY), 72);
    if (aLogFont.lfWeight    == 1) aLogFont.lfWeight    = FW_BOLD;
    if (aLogFont.lfStrikeOut != 0) aLogFont.lfStrikeOut = XPR_TRUE;
    if (aLogFont.lfUnderline != 0) aLogFont.lfUnderline = XPR_TRUE;

    return XPR_TRUE;
}
