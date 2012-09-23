//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TipDlg.h"

#include "Option.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_TIP 100

TipDlg::TipDlg(void)
    : super(IDD_TIP, XPR_NULL)
    , mIndex(-1)
{
}

void TipDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TipDlg, super)
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_TIP_NEXT, OnNext)
    ON_BN_CLICKED(IDC_TIP_NO_STARTUP, OnTipStartup)
END_MESSAGE_MAP()

xpr_bool_t TipDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    xpr_bool_t sNoStartupTip = !gOpt->mTipOfTheToday;
    ((CButton *)GetDlgItem(IDC_TIP_NO_STARTUP))->SetCheck(sNoStartupTip);

    xpr_sint_t i;
    xpr_tchar_t sStringId[0xff] = {0};
    const xpr_tchar_t *sString = XPR_NULL;

    for (i = 1; i <= MAX_TIP ; ++i)
    {
        _stprintf(sStringId, XPR_STRING_LITERAL("popup.tip_of_today.tip%d"), i);

        sString = theApp.loadString(sStringId, XPR_TRUE);
        if (sString != XPR_NULL)
        {
            mTipDeque.push_back(sString);
        }
        else
        {
            break;
        }
    }

    srand((xpr_uint_t)time(XPR_NULL));

    if (mTipDeque.empty() == false)
        mIndex = rand() % mTipDeque.size();

    return XPR_TRUE;
}

void TipDlg::OnPaint(void) 
{
    CPaintDC sPaintDc(this);

    CRect sRect;
    GetClientRect(&sRect);
    sRect.DeflateRect(12, 12, 12, 12);
    sRect.bottom -= 34;

    CRect sRect2(sRect);
    sRect2.right = 70;
    sPaintDc.FillSolidRect(sRect2, RGB(128,128,128));

    HICON sIcon = theApp.LoadIcon(IDI_TIP);
    if (sIcon != XPR_NULL)
    {
        CPoint sIconPoint(sRect2.left + ((sRect2.Width() - 32) / 2), 25);
        ::DrawIconEx(sPaintDc.m_hDC, sIconPoint.x, sIconPoint.y, sIcon, 32, 32, 0, XPR_NULL, DI_NORMAL);
        DESTROY_ICON(sIcon);
    }

    CRect sRect3(sRect);
    sRect3.left = 70;
    sPaintDc.FillSolidRect(sRect3, RGB(255,255,255));

    CPen sPen(PS_SOLID, 1, RGB(200,200,200));
    CPen *sOldPen = sPaintDc.SelectObject(&sPen);
    sPaintDc.MoveTo(sRect2.right, 60);
    sPaintDc.LineTo(sRect.right, 60);
    sPaintDc.SelectObject(sOldPen);

    CRect sTitleRect(sRect);
    sTitleRect.left = sRect2.right + 10;
    sTitleRect.bottom = 60;

    CFont sTitleFont, sTextFont;
    LOGFONT sLogFont = {0};
    GetFont()->GetLogFont(&sLogFont);
    sTextFont.CreateFontIndirect(&sLogFont);
    sTitleFont.CreatePointFont(200, XPR_STRING_LITERAL("Times New Roman"));
    sTitleFont.GetLogFont(&sLogFont);
    sLogFont.lfWeight = FW_BOLD;
    sLogFont.lfItalic = XPR_TRUE;
    sLogFont.lfHeight = -MulDiv(18, GetDeviceCaps(sPaintDc.m_hDC, LOGPIXELSY), 72);
    sTitleFont.DeleteObject();
    sTitleFont.CreateFontIndirect(&sLogFont);

    sPaintDc.SetTextColor(RGB(0,0,0));
    CFont *sOldFont = sPaintDc.SelectObject(&sTitleFont);
    xpr_tchar_t sTipOfTodayText[] = XPR_STRING_LITERAL("tip of the today");
    sPaintDc.DrawText(sTipOfTodayText, (xpr_sint_t)_tcslen(sTipOfTodayText), sTitleRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    sPaintDc.SelectObject(sOldFont);

    CRect sTextRect;
    sTextRect.left   = sTitleRect.left;
    sTextRect.top    = sTitleRect.bottom + 15;
    sTextRect.right  = sRect.right - 10;
    sTextRect.bottom = sRect.bottom - 10;

    const xpr_tchar_t *sText = XPR_NULL;
    if (XPR_STL_IS_INDEXABLE(mIndex, mTipDeque))
    {
        sText = mTipDeque[mIndex].c_str();
    }

    if (sText != XPR_NULL)
    {
        sOldFont = sPaintDc.SelectObject(&sTextFont);
        sPaintDc.DrawText(sText, (xpr_sint_t)_tcslen(sText), sTextRect, DT_LEFT | DT_WORDBREAK);
        sPaintDc.SelectObject(sOldFont);
    }
}

void TipDlg::OnNext(void)
{
    if (mTipDeque.empty() == false)
    {
        mIndex = ++mIndex % mTipDeque.size();
        Invalidate(XPR_FALSE);
    }
}

void TipDlg::OnDestroy(void) 
{
    super::OnDestroy();

}

void TipDlg::OnTipStartup(void) 
{
    gOpt->mTipOfTheToday = !((CButton *)GetDlgItem(IDC_TIP_NO_STARTUP))->GetCheck();
}
