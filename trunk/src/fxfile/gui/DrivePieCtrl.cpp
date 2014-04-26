//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DrivePieCtrl.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_MARGIN     5

const COLORREF FREE_RGB  = RGB(255, 0, 255);
const COLORREF USED_RGB  = RGB(0, 0, 255);
const COLORREF WHITE_RGB = RGB(255, 255, 255);

DrivePieCtrl::DrivePieCtrl(void)
    : mTotalSize(0), mUsedSize(0), mFreeSize(0)
{
}

DrivePieCtrl::~DrivePieCtrl(void)
{
}

BEGIN_MESSAGE_MAP(DrivePieCtrl, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void DrivePieCtrl::OnPaint(void)
{
    CPaintDC sPaintDC(this);

    drawDrivePie(sPaintDC);
    drawLegends(sPaintDC);
}

void DrivePieCtrl::setString(const xpr_tchar_t *aTotalSizeText, const xpr_tchar_t *aUsedSizeText, const xpr_tchar_t *aFreeSizeText)
{
    mTotalSizeText.clear();
    mUsedSizeText.clear();
    mFreeSizeText.clear();

    if (XPR_IS_NOT_NULL(aTotalSizeText)) mTotalSizeText = aTotalSizeText;
    if (XPR_IS_NOT_NULL(aUsedSizeText )) mUsedSizeText  = aUsedSizeText;
    if (XPR_IS_NOT_NULL(aFreeSizeText )) mFreeSizeText  = aFreeSizeText;
}

void DrivePieCtrl::setDrive(const xpr_tchar_t *aDrive)
{
    _tcscpy(mDrive, aDrive);

    getDriveSpace();

    Invalidate();
}

void DrivePieCtrl::drawDrivePie(CDC &aDC)
{
    if (mDrive[0] == '\0')
        return;

    CRect sClientRect;
    GetClientRect(&sClientRect);
    sClientRect.DeflateRect(1, 1);
    aDC.FillSolidRect(&sClientRect, WHITE_RGB);

    if (mTotalSize == 0)
        return;

    sClientRect.top    = sClientRect.top    + /*sClientRect.Height()*2/5*/ + DEFAULT_MARGIN;
    sClientRect.right  = sClientRect.right  - DEFAULT_MARGIN;
    sClientRect.left   = sClientRect.left   + DEFAULT_MARGIN;
    sClientRect.bottom = sClientRect.bottom - DEFAULT_MARGIN;

    CPen *sOldPen;
    CBrush *sOldBrush;
    COLORREF sColors[2] = { USED_RGB, FREE_RGB };

    // draw pie
    xpr_sint_t sCenterX, sCenterY, sRadius, sNewX, sNewY, sLastX, sLastY;
    sCenterX = sClientRect.left + (xpr_sint_t)(sClientRect.Width() / 3.2);
    sCenterY = sClientRect.top + sClientRect.Height() / 2;

    sRadius = min(sClientRect.Height(), sClientRect.Width()) / 2;

    sLastX = sCenterX + sRadius;
    sLastY = sCenterY;

    CRect sPieRect;
    sPieRect.top    = sCenterY - sRadius;
    sPieRect.left   = sCenterX - sRadius;
    sPieRect.bottom = sCenterY + sRadius;
    sPieRect.right  = sCenterX + sRadius;

    xpr_double_t sSlice[2];
    sSlice[0] = (xpr_double_t)(mUsedSize * 1.0 / mTotalSize);
    sSlice[1] = (xpr_double_t)(mFreeSize * 1.0 / mTotalSize);

    xpr_sint_t i;
    xpr_double_t sDegree = 0, sDataSum = 0;
    for (i = 0; i < 2; ++i)
    {
        if (sSlice[i] == 0.f)
            break;

        sDataSum += sSlice[i];
        sDegree = sDataSum * 2.0 * XPR_MATH_PI;

        sNewX = sCenterX + (xpr_sint_t)((xpr_double_t)sRadius * cos(sDegree));
        sNewY = sCenterY - (xpr_sint_t)((xpr_double_t)sRadius * sin(sDegree));

        CPoint sPoint1(sLastX, sLastY);
        CPoint sPoint2(sNewX, sNewY);

        CBrush sBrush(sColors[i]);
        CPen sPiePen(PS_SOLID, 1, sColors[i]);
        sOldPen = aDC.SelectObject(&sPiePen);
        sOldBrush = aDC.SelectObject(&sBrush);

        aDC.Pie(sPieRect, sPoint1, sPoint2);

        aDC.SelectObject(sOldBrush);
        aDC.SelectObject(sOldPen);

        sLastX = sNewX;
        sLastY = sNewY;
    }
}

void DrivePieCtrl::getDriveSpace(void)
{
    ULARGE_INTEGER sUsedSize  = {0};
    ULARGE_INTEGER sTotalSize = {0};
    ULARGE_INTEGER sFreeSize  = {0};
    ULARGE_INTEGER sTempSize  = {0};

    TRY
    {
        ::GetDiskFreeSpaceEx(mDrive, &sTempSize, &sTotalSize, &sFreeSize);
        sUsedSize.QuadPart = sTotalSize.QuadPart - sFreeSize.QuadPart;
    }
    END_TRY

    mTotalSize = sTotalSize.QuadPart;
    mFreeSize  = sFreeSize.QuadPart;
    mUsedSize  = sUsedSize.QuadPart;
}

void DrivePieCtrl::drawLegends(CDC &aDC)
{
    if (mDrive[0] == '\0')
        return;

    if (mTotalSize == 0)
        return;

    CRect sClientRect;
    GetClientRect(&sClientRect);
    sClientRect.top    = sClientRect.top + DEFAULT_MARGIN;
    sClientRect.right  = sClientRect.right - DEFAULT_MARGIN;
    sClientRect.left   = sClientRect.left + (xpr_sint_t)(sClientRect.Width()/1.8) + DEFAULT_MARGIN;
    sClientRect.bottom = sClientRect.bottom - sClientRect.Height()*3/5 - DEFAULT_MARGIN;

    CRect sLine1Rect, sLine2Rect, sLine3Rect;
    sLine1Rect = sLine2Rect = sLine3Rect = sClientRect;
    sLine1Rect.bottom = sClientRect.top + sClientRect.Height()/3;
    sLine2Rect.top = sLine1Rect.bottom + DEFAULT_MARGIN;
    sLine2Rect.bottom = sClientRect.bottom - sClientRect.Height()/3 + DEFAULT_MARGIN/2;
    sLine3Rect.top = sLine2Rect.bottom + DEFAULT_MARGIN;

    xpr::string sTitle;
    CFont *sFont = GetParent()->GetFont();
    CFont *sOldFont = aDC.SelectObject(sFont);

    if (mTotalSize > 1024.0*1024.0*1024)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f GB"), mTotalSizeText.c_str(), mTotalSize/1024.0/1024.0/1024.0);
    else if (mTotalSize > 1024.0*1024.0)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f MB"), mTotalSizeText.c_str(), mTotalSize/1024.0/1024.0);
    else
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f KB"), mTotalSizeText.c_str(), mTotalSize/1024.0);
    aDC.TextOut(sLine1Rect.left + DEFAULT_MARGIN, sLine1Rect.top + DEFAULT_MARGIN, sTitle.c_str());

    if (mUsedSize > 1024.0*1024.0*1024)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f GB"), mUsedSizeText.c_str(), mUsedSize/1024.0/1024.0/1024.0);
    else if (mUsedSize > 1024.0*1024.0)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f MB"), mUsedSizeText.c_str(), mUsedSize/1024.0/1024.0);
    else
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f KB"), mUsedSizeText.c_str(), mUsedSize/1024.0);
    aDC.TextOut(sLine2Rect.left + DEFAULT_MARGIN + sLine2Rect.Width()/5, sLine2Rect.top, sTitle.c_str());

    if (mFreeSize > 1024.0*1024.0*1024)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f GB"), mFreeSizeText.c_str(), mFreeSize/1024.0/1024.0/1024.0);
    else if (mFreeSize > 1024.0*1024.0)
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f MB"), mFreeSizeText.c_str(), mFreeSize/1024.0/1024.0);
    else
        sTitle.format(XPR_STRING_LITERAL("%s: %2.2f KB"), mFreeSizeText.c_str(), mFreeSize/1024.0);
    aDC.TextOut(sLine3Rect.left + DEFAULT_MARGIN + sLine3Rect.Width()/5, sLine3Rect.top - DEFAULT_MARGIN, sTitle.c_str());

    aDC.SelectObject(sOldFont);

    CBrush sUsedBrush, sFreeBrush;
    CRect sUsedRect, sFreeRect;
    sUsedRect.top    = sLine2Rect.top;
    sUsedRect.left   = sLine2Rect.left + DEFAULT_MARGIN * 2;
    sUsedRect.right  = sUsedRect.left  + DEFAULT_MARGIN * 3 - 2;
    sUsedRect.bottom = sUsedRect.top   + DEFAULT_MARGIN * 3 - 2;
    sUsedBrush.CreateSolidBrush(USED_RGB);
    aDC.FillRect(&sUsedRect, &sUsedBrush);

    sFreeRect.top    = sLine3Rect.top  - DEFAULT_MARGIN;
    sFreeRect.left   = sLine3Rect.left + DEFAULT_MARGIN * 2;
    sFreeRect.right  = sFreeRect.left  + DEFAULT_MARGIN * 3 - 2;
    sFreeRect.bottom = sFreeRect.top   + DEFAULT_MARGIN * 3 - 2;
    sFreeBrush.CreateSolidBrush(FREE_RGB);
    aDC.FillRect(&sFreeRect, &sFreeBrush);
}
