/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarCF          Version 2.42
// 
// Created: Dec 21, 1998        Last Modified: Feb 10, 2000
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2000 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "scbarcf.h"

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarCF

IMPLEMENT_DYNAMIC(CSizingControlBarCF, baseCSizingControlBarCF);

int CALLBACK EnumFontFamProc(ENUMLOGFONT FAR *lpelf,
                             NEWTEXTMETRIC FAR *lpntm,
                             int FontType,
                             LPARAM lParam)
{
    UNUSED_ALWAYS(lpelf);
    UNUSED_ALWAYS(lpntm);
    UNUSED_ALWAYS(FontType);
    UNUSED_ALWAYS(lParam);

    return 0;
}
 
CSizingControlBarCF::CSizingControlBarCF()
{
    m_bActive = FALSE;

    CDC dc;
    dc.CreateCompatibleDC(NULL);

    m_sFontFace = (::EnumFontFamilies(dc.m_hDC,
        _T("Tahoma"), (FONTENUMPROC) EnumFontFamProc, 0) == 0) ?
        _T("Tahoma") : _T("Arial");

    dc.DeleteDC();
    
	m_cyGripper = 17;
}

BEGIN_MESSAGE_MAP(CSizingControlBarCF, baseCSizingControlBarCF)
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

void CSizingControlBarCF::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    baseCSizingControlBarCF::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

    if (!HasGripper())
        return;

    BOOL bNeedPaint = FALSE;

    CWnd* pFocus = GetFocus();
    BOOL bActiveOld = m_bActive;

    m_bActive = (pFocus->GetSafeHwnd() && IsChild(pFocus));

    if (m_bActive != bActiveOld)
        bNeedPaint = TRUE;

    if (bNeedPaint)
        SendMessage(WM_NCPAINT);
}

// gradient defines (if not already defined)
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION     27
#define COLOR_GRADIENTINACTIVECAPTION   28
#define SPI_GETGRADIENTCAPTIONS         0x1008
#endif

void CSizingControlBarCF::NcPaintGripper(CDC* pDC, CRect rcClient)
{
	if (!HasGripper())
		return;
	
	// compute the caption rectangle
	BOOL bHorz = IsHorzDocked();
	CRect rcGrip = rcClient;
	CRect rcBtn = m_biHide.GetRect();
	if (bHorz)
	{   // right side gripper
		rcGrip.left -= m_cyGripper + 1;
		rcGrip.right = rcGrip.left + m_cyGripper - 1;
//		rcGrip.top = rcBtn.bottom + 3;
	}
	else
	{   // gripper at top
		rcGrip.top -= m_cyGripper + 1;
		rcGrip.bottom = rcGrip.top + m_cyGripper - 1;
//		rcGrip.right = rcBtn.left - 3;
	}
	rcGrip.InflateRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
	
	COLORREF clrCptn = m_bActive ? ::GetSysColor(COLOR_ACTIVECAPTION) : ::GetSysColor(COLOR_3DSHADOW);
	pDC->FillSolidRect(&rcGrip, clrCptn);
	if (!m_bActive)
	{
		CRect rc2(rcGrip);
		rc2.DeflateRect(1,1,1,1);
		pDC->FillSolidRect(rc2, ::GetSysColor(COLOR_BTNFACE));
	}

	// draw the caption text - first select a font
	CFont font;
	int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
	int pointsize = MulDiv(85, 96, ppi); // 8.5 points at 96 ppi
	
	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	ncm.lfMessageFont.lfHeight = -MulDiv(9, GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72);
	BOOL bFont = font.CreateFontIndirect(&ncm.lfMessageFont); //.CreatePointFont(pointsize, m_sFontFace);

	if (bFont)
	{
		// get the text color
		COLORREF clrCptnText = m_bActive ? ::GetSysColor(COLOR_CAPTIONTEXT) : ::GetSysColor(COLOR_BTNTEXT);
		
		int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		COLORREF clrOldText = pDC->SetTextColor(clrCptnText);
		
		LOGFONT lf;
		if (bHorz)
		{
			// rotate text 90 degrees CCW if horizontally docked
			font.GetLogFont(&lf);
			font.DeleteObject();
			lf.lfEscapement = 900;
			font.CreateFontIndirect(&lf);
		}
        
		CFont* pOldFont = pDC->SelectObject(&font);
		CString sTitle;
		GetWindowText(sTitle);
		
//    CPoint ptOrg = bHorz ?
//          CPoint(rcGrip.left - 1, rcGrip.bottom - 3) :
//            CPoint(rcGrip.left + 3, rcGrip.top - 1);

		int nFormat;
		if (bHorz)
		{
			nFormat = DT_BOTTOM | DT_SINGLELINE;
			rcGrip.left += 2;
			rcGrip.bottom += 12;
			rcGrip.top += rcBtn.bottom - 5;
		}
		else
		{
			nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
			rcGrip.left += 3;
			rcGrip.right = rcBtn.left - 1;
		}
		pDC->DrawText(sTitle, rcGrip, nFormat);

//        pDC->ExtTextOut(ptOrg.x, ptOrg.y, ETO_CLIPPED, rcGrip, sTitle, NULL);
		
		pDC->SelectObject(pOldFont);
		pDC->SetBkMode(nOldBkMode);
		pDC->SetTextColor(clrOldText);
	}
	
	// draw the button
	m_biHide.Paint(pDC, m_bActive);
}

LRESULT CSizingControlBarCF::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = baseCSizingControlBarCF::OnSetText(wParam, lParam);

    SendMessage(WM_NCPAINT);

    return lResult;
}
