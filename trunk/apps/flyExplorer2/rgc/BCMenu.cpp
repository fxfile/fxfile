//*************************************************************************
// BCMenu.cpp : implementation file
// Version : 2.63
// Date : December 2, 1999
// Author : Brent Corkum
// Email :  corkum@rocscience.com
// Latest Version : http://www.rocscience.com/~corkum/BCMenu.html
// 
// Bug Fixes and portions of code supplied by:
//
// Ben Ashley,Girish Bharadwaj,Jean-Edouard Lachand-Robert,
// Robert Edward Caldecott,Kenny Goers,Leonardo Zide,
// Stefan Kuhr,Reiner Jung,Martin Vladic,Kim Yoo Chul,
// Oz Solomonovich
//
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of
// your applications (Freeware,Shareware,Commercial). All I ask is
// that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also
// wouldn't mind if you sent me a copy of your application since I
// like to play with new stuff.
//********************************************************************
//////////////////////////////////////////////////////////////////////
//
// modified by flychk (2003.9.14)
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"        // Standard windows header file
#include "BCMenu.h"        // BCMenu class declaration
#include <afxpriv.h>       //SK: makes A2W and other spiffy AFX macros work

#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GAP 1
#ifndef OBM_CHECK
#define OBM_CHECK 32760 // from winuser.h
#endif

#if _MFC_VER <0x400
#error This code does not work on Versions of MFC prior to 4.0
#endif

//----------------------------------------------------------------------
// BCMenuData
//----------------------------------------------------------------------

BCMenuData::~BCMenuData()
{
	if (bitmap)
		delete bitmap;
	
	delete[] m_szMenuText; //Need not check for NULL because ANSI X3J16 allows "delete NULL"
}

void BCMenuData::SetWideString(const wchar_t *szWideString)
{
	delete[] m_szMenuText; //Need not check for NULL because ANSI X3J16 allows "delete NULL"
	
	if (szWideString)
    {
		m_szMenuText = new wchar_t[sizeof(wchar_t)*(wcslen(szWideString)+1)];
		if (m_szMenuText)
			wcscpy(m_szMenuText, szWideString);
    }
	else
		m_szMenuText = NULL;//set to NULL so we need not bother about dangling non-NULL Ptrs
}

void BCMenuData::SetAnsiString(LPCSTR szAnsiString)
{
	USES_CONVERSION;
	SetWideString(A2W(szAnsiString));  //SK:  see MFC Tech Note 059
}

//returns the menu text in ANSI or UNICODE
//depending on the MFC-Version we are using
CString BCMenuData::GetString(void)
{
	CString strText;
	if (m_szMenuText)
    {
#ifdef UNICODE
		strText = m_szMenuText;
#else
		USES_CONVERSION;
		strText = W2A(m_szMenuText);     // MFC Tech Note 059
#endif    
    }
	return strText;
}

//----------------------------------------------------------------------
// BCMenu
//----------------------------------------------------------------------

#define SEPARATOR_HEIGHT	5
#define MAX_MENU_WIDTH		350

CTypedPtrArray<CPtrArray, HMENU> BCMenu::m_AllSubMenus;  // Stores list of all sub-menus

xpr_bool_t BCMenu::mStandardMenuStyle = XPR_FALSE;
xpr_bool_t BCMenu::mAnimationMenu     = XPR_FALSE;

IMPLEMENT_DYNCREATE(BCMenu, CMenu)

BCMenu::BCMenu()
{
	m_bDynIcons = FALSE;     // O.S. - no dynamic icons by default
	m_nIconX = 16;            // Icon sizes default to 16 x 16
	m_nIconY = 16;            // ...
	m_selectcheck = -1;
	m_unselectcheck = -1;
	checkmaps = NULL;
	checkmapsshare = FALSE;
	// set the color used for the transparent background in all bitmaps
	m_bitmapBackground = RGB(192,192,192); //gray
	m_bitmapBackgroundFlag = FALSE;

//	m_nItemHeight = -1;
}

BCMenu::~BCMenu()
{
	DestroyMenu();
}

BOOL BCMenu::IsMenu(CMenu *submenu)
{
	INT_PTR m;
	INT_PTR numSubMenus = m_AllSubMenus.GetUpperBound();
	for (m=0; m<=numSubMenus; ++m)
	{
		if (submenu->m_hMenu == m_AllSubMenus[m])
			return (TRUE);
	}
	return (FALSE);
}

BOOL BCMenu::IsMenu(HMENU submenu)
{
	INT_PTR m;
	INT_PTR numSubMenus = m_AllSubMenus.GetUpperBound();
	for (m=0; m<=numSubMenus; ++m)
	{
		if (submenu == m_AllSubMenus[m])
			return(TRUE);
	}
	return(FALSE);
}

BOOL BCMenu::DestroyMenu()
{
	// Destroy Sub menus:
	INT_PTR m, n;
	INT_PTR nAllSubMenus = m_AllSubMenus.GetUpperBound();
	for (n = nAllSubMenus; n >= 0; n--)
	{
		if (m_AllSubMenus[n] == m_hMenu)
			m_AllSubMenus.RemoveAt(n);
	}

	INT_PTR nSubMenus = m_SubMenus.GetUpperBound();
	for (m = nSubMenus; m >= 0; m--)
	{
		nAllSubMenus = m_AllSubMenus.GetUpperBound();
		for (n = nAllSubMenus; n >= 0; n--)
		{
			if (m_AllSubMenus[n] == m_SubMenus[m])
				m_AllSubMenus.RemoveAt(n);
		}

		CMenu *ptr=FromHandle(m_SubMenus[m]);
		BOOL flag=ptr->IsKindOf(RUNTIME_CLASS( BCMenu ));
		if(flag)delete((BCMenu *)ptr);
	}
	m_SubMenus.RemoveAll();
	
	// Destroy menu data
	INT_PTR nItems = m_MenuList.GetUpperBound();
	for (m = 0; m <= nItems; m++)
		delete (m_MenuList[m]);

	m_MenuList.RemoveAll();
	if (checkmaps && !checkmapsshare)
	{
		delete checkmaps;
		checkmaps = NULL;
	}

	return CMenu::DestroyMenu();
}


///////////////////////////////////////////////////////////////////////////
//
// BCMenu message handlers


/*
==========================================================================
void BCMenu::DrawItem(LPDRAWITEMSTRUCT)
---------------------------------------

Called by the framework when a particular item needs to be drawn.  We
overide this to draw the menu item in a custom-fashion, including icons
and the 3D rectangle bar.
==========================================================================
*/
/*
void DrawGradient(CDC*pDC, CRect rc)
{
	COLORREF clrWin  = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF clrBack = GetSysColor(COLOR_3DFACE);
	
	TRIVERTEX vert[2];
	vert[0].x       =  rc.left;
	vert[0].y       =  rc.top;
	vert[0].Red     =  (GetRValue(clrWin)) << 8;
	vert[0].Green   =  (GetGValue(clrWin)) << 8;
	vert[0].Blue    =  (GetBValue(clrWin)) << 8;
	vert[0].Alpha   =  0x0000;
	
	vert[1].x       =  rc.right;
	vert[1].y       =  rc.bottom;
	vert[1].Red     =  (GetRValue(clrBack)) << 8;
	vert[1].Green   =  (GetGValue(clrBack)) << 8;
	vert[1].Blue    =  (GetBValue(clrBack)) << 8;
	vert[1].Alpha   =  0x0000;
	
	GRADIENT_RECT grc;
	grc.UpperLeft  = 0;
	grc.LowerRight = 1;
	
	GradientFill(pDC->m_hDC, vert, 2, &grc, 1, GRADIENT_FILL_RECT_H);
}
*/

void BCMenu::setStandardMenuStyle(xpr_bool_t aStandardMenuStyle)
{
    mStandardMenuStyle = aStandardMenuStyle;
}

void BCMenu::setAnimationMenu(xpr_bool_t aAnimationMenu)
{
    mAnimationMenu = aAnimationMenu;
}

static void DrawVistaStyle_MenuSel(CDC *pDC, CRect rc, COLORREF clr)
{
	COLORREF clrBegin[2];
	COLORREF clrEnd[2];

    COLORREF clrBorder = fxb::LightenColor(clr, 0.50);
	COLORREF clrLightenBorder = fxb::LightenColor(clrBorder, 0.20);

	clrBegin[0] = fxb::LightenColor(clr, 0.95);
	clrEnd[0]   = fxb::LightenColor(clr, 0.80);

	clrBegin[1] = fxb::LightenColor(clr, 0.70);
	clrEnd[1]   = fxb::LightenColor(clr, 0.80);

	CRect rcGradient(rc);
	rcGradient.DeflateRect(1,1,2,1);

	CRect rc0(rcGradient);
	CRect rc1(rcGradient);

	rc0.bottom = rc0.top + (rc0.Height()/2);
	rc1.top = rc0.bottom;

	fxb::DrawVertGradient(pDC, rc0, clrBegin[0], clrEnd[0]);
	fxb::DrawVertGradient(pDC, rc1, clrBegin[1], clrEnd[1]);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, clrBorder);

	CPen *pOldPen = pDC->SelectObject(&pen);

	CPoint pt1(rc.left, rc.top);
	CPoint pt2(rc.right, rc.top);
	CPoint pt3(rc.right, rc.bottom);
	CPoint pt4(rc.left, rc.bottom);

	pDC->MoveTo(pt1.x+2, pt1.y);
	pDC->LineTo(pt2.x-2, pt2.y);

	pDC->MoveTo(pt2.x-1, pt2.y+2);
	pDC->LineTo(pt3.x-1, pt3.y-2);

	pDC->MoveTo(pt4.x+2, pt4.y-1);
	pDC->LineTo(pt3.x-2, pt3.y-1);

	pDC->MoveTo(pt1.x, pt1.y+2);
	pDC->LineTo(pt4.x, pt4.y-2);

	pDC->SetPixel(pt1.x+1,   pt1.y+1,   clrBorder);
	pDC->SetPixel(pt2.x-1-1, pt2.y+1,   clrBorder);
	pDC->SetPixel(pt3.x-1-1, pt3.y-1-1, clrBorder);
	pDC->SetPixel(pt4.x+1,   pt4.y-1-1, clrBorder);

	pDC->SetPixel(pt1.x+2,   pt1.y,     clrLightenBorder);
	pDC->SetPixel(pt1.x,     pt1.y+2,   clrLightenBorder);
	pDC->SetPixel(pt2.x-1-2, pt2.y,     clrLightenBorder);
	pDC->SetPixel(pt2.x-1,   pt2.y+2,   clrLightenBorder);
	pDC->SetPixel(pt3.x-1,   pt3.y-1-2, clrLightenBorder);
	pDC->SetPixel(pt3.x-1-2, pt3.y-1,   clrLightenBorder);
	pDC->SetPixel(pt4.x+2,   pt4.y-1,   clrLightenBorder);
	pDC->SetPixel(pt4.x,     pt4.y-1-2, clrLightenBorder);

	pDC->SelectObject(pOldPen);
}

static void DrawOffice2007Style_MenuSel(CDC *pDC, CRect rc, COLORREF clr)
{
	COLORREF clrBegin[2];
	COLORREF clrEnd[2];

	COLORREF clrBorder = fxb::LightenColor(clr, 0.50);
	COLORREF clrLightBorder = fxb::LightenColor(clrBorder, 0.20);

	clrBegin[0] = fxb::LightenColor(clr, 0.95);
	clrEnd[0]   = fxb::LightenColor(clr, 0.80);

	clrBegin[1] = fxb::LightenColor(clr, 0.70);
	clrEnd[1]   = fxb::LightenColor(clr, 0.80);

	clrBegin[0] = fxb::LightenColor(clr, 0.95);
	clrEnd[0]   = fxb::LightenColor(clr, 0.80);

	clrBegin[1] = fxb::LightenColor(clr, 0.70);
	clrEnd[1]   = fxb::LightenColor(clr, 0.80);

	CRect rcGradient(rc);
	rcGradient.DeflateRect(1,1,2,1);

	CRect rc0(rcGradient);
	CRect rc1(rcGradient);

	rc0.bottom = rc0.top + (rc0.Height()/2);
	rc1.top = rc0.bottom;

	fxb::DrawVertGradient(pDC, rc0, clrBegin[0], clrEnd[0]);
	fxb::DrawVertGradient(pDC, rc1, clrBegin[1], clrEnd[1]);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, clrBorder);

	CPen *pOldPen = pDC->SelectObject(&pen);

	CPoint pt1(rc.left, rc.top);
	CPoint pt2(rc.right, rc.top);
	CPoint pt3(rc.right, rc.bottom);
	CPoint pt4(rc.left, rc.bottom);

	//
	// box style
	//
	//pDC->MoveTo(pt1.x, pt1.y);
	//pDC->LineTo(pt2.x, pt2.y);

	//pDC->MoveTo(pt2.x-1, pt2.y);
	//pDC->LineTo(pt3.x-1, pt3.y);

	//pDC->MoveTo(pt4.x, pt4.y-1);
	//pDC->LineTo(pt3.x, pt3.y-1);

	//pDC->MoveTo(pt1.x, pt1.y);
	//pDC->LineTo(pt4.x, pt4.y);

	//
	// round box style
	//
	pDC->MoveTo(pt1.x+2, pt1.y);
	pDC->LineTo(pt2.x-2, pt2.y);

	pDC->MoveTo(pt2.x-1, pt2.y+2);
	pDC->LineTo(pt3.x-1, pt3.y-2);

	pDC->MoveTo(pt4.x+2, pt4.y-1);
	pDC->LineTo(pt3.x-2, pt3.y-1);

	pDC->MoveTo(pt1.x, pt1.y+2);
	pDC->LineTo(pt4.x, pt4.y-2);

	pDC->SetPixel(pt1.x+1,   pt1.y+1,   clrBorder);
	pDC->SetPixel(pt2.x-1-1, pt2.y+1,   clrBorder);
	pDC->SetPixel(pt3.x-1-1, pt3.y-1-1, clrBorder);
	pDC->SetPixel(pt4.x+1,   pt4.y-1-1, clrBorder);

	pDC->SetPixel(pt1.x+2,   pt1.y,     clrLightBorder);
	pDC->SetPixel(pt1.x,     pt1.y+2,   clrLightBorder);
	pDC->SetPixel(pt2.x-1-2, pt2.y,     clrLightBorder);
	pDC->SetPixel(pt2.x-1,   pt2.y+2,   clrLightBorder);
	pDC->SetPixel(pt3.x-1,   pt3.y-1-2, clrLightBorder);
	pDC->SetPixel(pt3.x-1-2, pt3.y-1,   clrLightBorder);
	pDC->SetPixel(pt4.x+2,   pt4.y-1,   clrLightBorder);
	pDC->SetPixel(pt4.x,     pt4.y-1-2, clrLightBorder);

	pDC->SelectObject(pOldPen);
}

void BCMenu::DrawItemOffice2007Style(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
	BCMenuData *pMenuData = (BCMenuData *)lpDIS->itemData;
	UINT state = pMenuData->nFlags;

	COLORREF crBack   = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 240);
	COLORREF crBegin  = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd    = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += m_nIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + m_nIconX+4;
		fxb::DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);

		CRect rcSept(rc);
		rcSept.top  += rcSept.Height() >> 1;
		rcSept.left += m_nIconX + 8 + GAP;

		pDC->DrawEdge(rcSept, EDGE_ETCHED, BF_TOP);
	}
	else
	{
		CBrush brSelect;
		brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));

		// Get State(up, down, focused, disabled)
		CString strText;
		int nIconNormal = -1, xoffset = -1;
		CImageList *pImgList = NULL;

		BOOL standardflag = FALSE, selectedflag = FALSE, disableflag = FALSE;
		BOOL checkflag = FALSE;

		state = lpDIS->itemState;
		if (lpDIS->itemData != NULL)
		{
			nIconNormal	= pMenuData->menuIconNormal;
			xoffset		= pMenuData->xoffset;
			pImgList	= pMenuData->bitmap;
			strText		= pMenuData->GetString();
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
				if (state & ODS_SELECTED && m_selectcheck > 0)
					checkflag = TRUE;
				else if (m_unselectcheck > 0)
					checkflag = TRUE;
			}
			else if (nIconNormal != -1)
			{
				standardflag = TRUE;
				if (state & ODS_SELECTED && !(state & ODS_GRAYED))
					selectedflag = TRUE;
				else if (state & ODS_GRAYED)
					disableflag = TRUE;
			}
		}
		else
			strText.Empty();

		// Draw Selection Box
		if (state & ODS_SELECTED)
		{
			if ((state & ODS_GRAYED) != ODS_GRAYED)
			{
				DrawOffice2007Style_MenuSel(pDC, rc, GetSysColor(COLOR_HIGHLIGHT));
			}
			else
			{
				DrawOffice2007Style_MenuSel(pDC, rc, GetSysColor(COLOR_3DSHADOW));
			}
		}
		else
		{
			CRect rcIcon(rc);
			rcIcon.right = rcIcon.left + m_nIconX + 4;
			fxb::DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - m_nIconY) / 2;
		dy = dy < 0 ? 0 : dy;
		if (nIconNormal >= 0 && pImgList)
		{
			CPoint ptImage(rc.left+2, rc.top+2+dy);

			if (pImgList && pImgList->m_hImageList && pImgList->GetImageCount() > 0)
			{
				HICON hIcon = pImgList->ExtractIcon(0);
				if (hIcon)
				{
					DrawIconEx(pDC->m_hDC, ptImage.x, ptImage.y, hIcon, 16, 16, 0, NULL, DI_NORMAL);
					::DestroyIcon(hIcon);
				}
			}
		}

		// Draw Check or Radio Mark
		if (nIconNormal < 0 && state & ODS_CHECKED && !checkflag)
		{
			CRect rc2(rc.left+2, rc.top+2+dy, rc.left+m_nIconX+1, rc.top+m_nIconY+2+dy);
			
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + m_nIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

			if (pMenuData->strAccelKey.GetLength() > 0)
			{
				strLeft  = strText;
				strRight = pMenuData->strAccelKey;
				rcText.right -= m_nIconX;
			}
			else
				strLeft = strText;
			
			int nOldMode = pDC->GetBkMode();
			pDC->SetBkMode(TRANSPARENT);
			
			// Draw the text in the correct colour:
			UINT nFormat  = DT_LEFT  | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
			UINT nFormat2 = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

			COLORREF crText;
			if (lpDIS->itemState & ODS_GRAYED)
				crText = GetSysColor(COLOR_3DSHADOW);
			else
				crText = GetSysColor(COLOR_MENUTEXT);
			pDC->SetTextColor(crText);

			pDC->DrawText(strLeft, rcText, nFormat);
			if (strRight.GetLength() > 0)
				pDC->DrawText(strRight, rcText, nFormat2);

			pDC->SelectObject(pOldFont);
			ftMenu.DeleteObject();

			// Restore Background Mode
			pDC->SetBkMode(nOldMode);
		}
		
		brSelect.DeleteObject();
	}
}

void BCMenu::DrawItemOffice2003Style(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
	BCMenuData *pMenuData = (BCMenuData *)lpDIS->itemData;
	UINT state = pMenuData->nFlags;

	COLORREF crBack  = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 240);
	COLORREF crBegin = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd   = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += m_nIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + m_nIconX+4;
		fxb::DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);

		CRect rcSept(rc);
		rcSept.top  += rcSept.Height() >> 1;
		rcSept.left += m_nIconX + 8 + GAP;

		pDC->DrawEdge(rcSept, EDGE_ETCHED, BF_TOP);
	}
	else
	{
		CBrush brSelect;
		brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));

		// Get State(up, down, focused, disabled)
		CString strText;
		int nIconNormal = -1, xoffset = -1;
		CImageList *pImgList = NULL;

		BOOL standardflag = FALSE, selectedflag = FALSE, disableflag = FALSE;
		BOOL checkflag = FALSE;

		state = lpDIS->itemState;
		if (lpDIS->itemData != NULL)
		{
			nIconNormal	= pMenuData->menuIconNormal;
			xoffset		= pMenuData->xoffset;
			pImgList	= pMenuData->bitmap;
			strText		= pMenuData->GetString();
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
				if (state & ODS_SELECTED && m_selectcheck > 0)
					checkflag = TRUE;
				else if (m_unselectcheck > 0)
					checkflag = TRUE;
			}
			else if (nIconNormal != -1)
			{
				standardflag = TRUE;
				if (state & ODS_SELECTED && !(state & ODS_GRAYED))
					selectedflag = TRUE;
				else if (state & ODS_GRAYED)
					disableflag = TRUE;
			}
		}
		else
			strText.Empty();

		// Draw Selection Box
		if (state & ODS_SELECTED)
		{
			COLORREF clrAlpha = fxb::LightenColor(GetSysColor(COLOR_HIGHLIGHT), 0.80);

			if ((state & ODS_GRAYED) != ODS_GRAYED)
			{
				CRect rcSel(rc);
				pDC->FillRect(rcSel, &brSelect);

				rcSel.DeflateRect(1, 1, 1, 1);
				pDC->FillSolidRect(rcSel, clrAlpha);
			}
			else
			{
				CRect rcSel(rc);
				pDC->FillSolidRect(rcSel, clrAlpha);
			}
		}
		else
		{
			CRect rcIcon(rc);
			rcIcon.right = rcIcon.left + m_nIconX + 4;
			fxb::DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - m_nIconY) / 2;
		dy = dy < 0 ? 0 : dy;
		if (nIconNormal >= 0 && pImgList)
		{
			CPoint ptImage(rc.left+2, rc.top+2+dy);

			if (pImgList && pImgList->m_hImageList && pImgList->GetImageCount() > 0)
			{
				HICON hIcon = pImgList->ExtractIcon(0);
				if (hIcon)
				{
					DrawIconEx(pDC->m_hDC, ptImage.x, ptImage.y, hIcon, 16, 16, 0, NULL, DI_NORMAL);
					::DestroyIcon(hIcon);
				}
			}
		}

		// Draw Check or Radio Mark
		if (nIconNormal < 0 && state & ODS_CHECKED && !checkflag)
		{
			CRect rc2(rc.left+2, rc.top+2+dy, rc.left+m_nIconX+1, rc.top+m_nIconY+2+dy);
			
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + m_nIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

			if (pMenuData->strAccelKey.GetLength() > 0)
			{
				strLeft  = strText;
				strRight = pMenuData->strAccelKey;
				rcText.right -= m_nIconX;
			}
			else
				strLeft = strText;
			
			int nOldMode = pDC->GetBkMode();
			pDC->SetBkMode(TRANSPARENT);
			
			// Draw the text in the correct colour:
			UINT nFormat  = DT_LEFT  | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
			UINT nFormat2 = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

			COLORREF crText;
			if (lpDIS->itemState & ODS_GRAYED)
				crText = GetSysColor(COLOR_3DSHADOW);
			else
				crText = GetSysColor(COLOR_MENUTEXT);
			pDC->SetTextColor(crText);

			pDC->DrawText(strLeft, rcText, nFormat);
			if (strRight.GetLength() > 0)
				pDC->DrawText(strRight, rcText, nFormat2);

			pDC->SelectObject(pOldFont);
			ftMenu.DeleteObject();

			// Restore Background Mode
			pDC->SetBkMode(nOldMode);
		}
		
		brSelect.DeleteObject();
	}
}

void BCMenu::DrawItemStandardStyle(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
	BCMenuData *pMenuData = (BCMenuData *)lpDIS->itemData;
	UINT state = (((BCMenuData *)(lpDIS->itemData))->nFlags);

	COLORREF crBack  = ::GetSysColor(COLOR_MENU);
	COLORREF crBegin = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd   = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += m_nIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + m_nIconX+4;
		pDC->FillSolidRect(rcIcon, crBack);

		CRect rcSept(rc);
		rcSept.top  += rcSept.Height() >> 1;

		pDC->DrawEdge(rcSept, EDGE_ETCHED, BF_TOP);
	}
	else
	{
		CBrush brSelect;
		brSelect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));

		// Get State(up, down, focused, disabled)
		CString strText;
		int nIconNormal = -1, xoffset = -1;
		CImageList *pImgList = NULL;

		BOOL standardflag = FALSE, selectedflag = FALSE, disableflag = FALSE;
		BOOL checkflag = FALSE;

		state = lpDIS->itemState;
		if (lpDIS->itemData != NULL)
		{
			nIconNormal	= pMenuData->menuIconNormal;
			xoffset		= pMenuData->xoffset;
			pImgList	= pMenuData->bitmap;
			strText		= pMenuData->GetString();
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
				if (state & ODS_SELECTED && m_selectcheck > 0)
					checkflag = TRUE;
				else if (m_unselectcheck > 0)
					checkflag = TRUE;
			}
			else if (nIconNormal != -1)
			{
				standardflag = TRUE;
				if (state & ODS_SELECTED && !(state & ODS_GRAYED))
					selectedflag = TRUE;
				else if (state & ODS_GRAYED)
					disableflag = TRUE;
			}
		}
		else
			strText.Empty();

		// Draw Selection Box
		if (state & ODS_SELECTED)
		{
			if ((state & ODS_GRAYED) != ODS_GRAYED)
			{
				CRect rcSel(rc);
				pDC->FillRect(rcSel, &brSelect);
			}
			else
			{
				CRect rcSel(rc);
				pDC->FillRect(rcSel, &brSelect);
			}
		}
		else
		{
			CRect rcIcon(rc);
			rcIcon.right = rcIcon.left + m_nIconX + 4;
			pDC->FillSolidRect(rcIcon, crBack);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - m_nIconY) / 2;
		dy = dy < 0 ? 0 : dy;
		if (nIconNormal >= 0 && pImgList)
		{
			CPoint ptImage(rc.left+2, rc.top+2+dy);

			if (pImgList && pImgList->m_hImageList && pImgList->GetImageCount() > 0)
			{
				HICON hIcon = pImgList->ExtractIcon(0);
				if (hIcon)
				{
					DrawIconEx(pDC->m_hDC, ptImage.x, ptImage.y, hIcon, 16, 16, 0, NULL, DI_NORMAL);
					::DestroyIcon(hIcon);
				}
			}
		}

		// Draw Check or Radio Mark
		if (nIconNormal < 0 && state & ODS_CHECKED && !checkflag)
		{
			CRect rc2(rc.left+4, rc.top+2+dy, rc.left+m_nIconX+1, rc.top+m_nIconY+2+dy);
			
			CMenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + m_nIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

			if (pMenuData->strAccelKey.GetLength() > 0)
			{
				strLeft  = strText;
				strRight = pMenuData->strAccelKey;
				rcText.right -= m_nIconX;
			}
			else
				strLeft = strText;
			
			int nOldMode = pDC->GetBkMode();
			pDC->SetBkMode(TRANSPARENT);
			
			// Draw the text in the correct colour:
			UINT nFormat  = DT_LEFT  | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
			UINT nFormat2 = DT_RIGHT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

			COLORREF crText;
			if (lpDIS->itemState & ODS_GRAYED)
				crText = RGB(140, 140, 140);
			else
			{
				if (state & ODS_SELECTED)
					crText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
				else
					crText = ::GetSysColor(COLOR_MENUTEXT);
			}
			pDC->SetTextColor(crText);

			pDC->DrawText(strLeft, rcText, nFormat);
			if (strRight.GetLength() > 0)
				pDC->DrawText(strRight, rcText, nFormat2);

			pDC->SelectObject(pOldFont);
			ftMenu.DeleteObject();

			// Restore Background Mode
			pDC->SetBkMode(nOldMode);
		}
		
		brSelect.DeleteObject();
	}
}

void BCMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (mStandardMenuStyle == XPR_TRUE)
		DrawItemStandardStyle(lpDIS);
	else
		DrawItemOffice2007Style(lpDIS);
}

void BCMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	BCMenuData *pMenuData = (BCMenuData *)lpMIS->itemData;
	UINT uState = pMenuData->nFlags;

	if (uState & MF_SEPARATOR)
	{
		lpMIS->itemWidth  = 0;
		lpMIS->itemHeight = SEPARATOR_HEIGHT;
	}
	else
	{
		CWnd *pWnd = AfxGetMainWnd();
		CDC *pDC = pWnd->GetDC();

		NONCLIENTMETRICS ncm = {0};
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

		CFont ftMenu;
		ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
		CFont *pOldFont = pDC->SelectObject(&ftMenu);
        
		SIZE sz = {0};
		static wchar_t wszMenuDispText[0xff] = {0};
		wcscpy(wszMenuDispText, pMenuData->GetWideString());
		if (pMenuData->strAccelKey.GetLength() > 0)
		{
			LPTSTR lpszAccelKey = pMenuData->strAccelKey.GetBuffer();

			USES_CONVERSION;
			swprintf(wszMenuDispText+wcslen(wszMenuDispText), L"\t%s", T2W(lpszAccelKey));
		}
		::GetTextExtentPoint32W(pDC->m_hDC, wszMenuDispText, (int)wcslen(wszMenuDispText), &sz);

		pDC->SelectObject (pOldFont);
		pWnd->ReleaseDC(pDC);
		ftMenu.DeleteObject();

		lpMIS->itemWidth = m_nIconX + sz.cx + m_nIconX + GAP;
		lpMIS->itemWidth = min(lpMIS->itemWidth, MAX_MENU_WIDTH);

		lpMIS->itemHeight = max(GetSystemMetrics(SM_CYMENU), (m_nIconY+4));
	}
}

int BCMenu::GetMenuHeight()
{
	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_FTYPE;

	int cyMenu = GetSystemMetrics(SM_CYMENU);
	int nDefaultHeight = cyMenu > (m_nIconY+4) ? cyMenu : (m_nIconY+4);

	int nHeight = 0;
	int nCount = GetMenuItemCount();
	for (int i=0; i<nCount; i++)
	{
		GetMenuItemInfo(i, &mii, TRUE);
		nHeight += (mii.fType & MFT_SEPARATOR) ? SEPARATOR_HEIGHT : nDefaultHeight;
	}

	return nHeight;
}
/*
void BCMenu::SetMenuItemHeight(int nItemHeight)
{
	m_nItemHeight = nItemHeight;
}
*/
void BCMenu::SetIconSize (int width, int height)
{
	m_nIconX = width;
	m_nIconY = height;
}

BOOL BCMenu::AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT_PTR nIDNewItem, int nIconNormal)
{
	USES_CONVERSION;
	return AppendODMenuW(A2W(lpstrText), nFlags, nIDNewItem, nIconNormal);//SK: See MFC Tech Note 059
}


BOOL BCMenu::AppendODMenuW(wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, int nIconNormal)
{
	// Add the MF_OWNERDRAW flag if not specified:
	if (!nIDNewItem)
	{
		if (nFlags & MF_BYPOSITION) nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
		else nFlags = MF_SEPARATOR | MF_OWNERDRAW;
	}
	else if (!(nFlags & MF_OWNERDRAW))
		nFlags |= MF_OWNERDRAW;
	
	if (nFlags & MF_POPUP)
	{
		m_AllSubMenus.Add((HMENU)nIDNewItem);
		m_SubMenus.Add((HMENU)nIDNewItem);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.Add(mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation

	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset = -1;
	if (nIconNormal >= 0)
	{
		mdata->xoffset = 0;
		LoadFromToolBar((UINT)nIDNewItem, nIconNormal, mdata->xoffset);
		if (mdata->bitmap) mdata->bitmap->DeleteImageList();
		else mdata->bitmap = new(CImageList);
		mdata->bitmap->Create(m_nIconX, m_nIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
		if (!AddBitmapToImageList(mdata->bitmap, nIconNormal))
		{
			mdata->bitmap->DeleteImageList();
			delete mdata->bitmap;
			mdata->bitmap = NULL;
			mdata->menuIconNormal = nIconNormal = -1;
			mdata->xoffset = -1;
		}
	}
	mdata->nFlags = nFlags;
	mdata->nID = nIDNewItem;
	return (CMenu::AppendMenu(nFlags, nIDNewItem, (LPCTSTR)mdata));
}

BOOL BCMenu::AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	USES_CONVERSION;
	return AppendODMenuW(A2W(lpstrText),nFlags,nIDNewItem,il,xoffset);
}

BOOL BCMenu::AppendODMenuW(wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	// Add the MF_OWNERDRAW flag if not specified:
	if (!nIDNewItem)
	{
		if (nFlags & MF_BYPOSITION)	nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
		else nFlags = MF_SEPARATOR | MF_OWNERDRAW;
	}
	else if (!(nFlags & MF_OWNERDRAW))
		nFlags |= MF_OWNERDRAW;
	
	if (nFlags & MF_POPUP)
	{
		m_AllSubMenus.Add((HMENU)nIDNewItem);
		m_SubMenus.Add((HMENU)nIDNewItem);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.Add(mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	if (il)
	{
		mdata->menuIconNormal = 0;
		mdata->xoffset = 0;
		if (mdata->bitmap)
			mdata->bitmap->DeleteImageList();
		else
			mdata->bitmap = new CImageList;
		ImageListDuplicate(il, xoffset, mdata->bitmap);
	}
	else
	{
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
	}
	mdata->nFlags = nFlags;
	mdata->nID = nIDNewItem;
	return (CMenu::AppendMenu(nFlags, nIDNewItem, (LPCTSTR)mdata));
}

BOOL BCMenu::InsertODMenuA(UINT nPosition, LPCSTR lpstrText, UINT nFlags, UINT_PTR nIDNewItem, int nIconNormal)
{
	USES_CONVERSION;
	return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nIDNewItem,nIconNormal);
}


BOOL BCMenu::InsertODMenuW(UINT nPosition, wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, int nIconNormal)
{
	if (!(nFlags & MF_BYPOSITION))
	{
		int iPos =0;
		BCMenu* pMenu = FindMenuOption(nPosition, iPos);
		if (pMenu)
			return pMenu->InsertODMenuW(iPos, lpstrText, nFlags | MF_BYPOSITION, nIDNewItem, nIconNormal);
		else
			return FALSE;
	}
	
	if (!nIDNewItem) nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
	else if (!(nFlags & MF_OWNERDRAW)) nFlags |= MF_OWNERDRAW;
	
	if (nFlags & MF_POPUP)
	{
		m_AllSubMenus.Add((HMENU)nIDNewItem);
		m_SubMenus.InsertAt(nPosition, (HMENU)nIDNewItem);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition, mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset = -1;
	if (nIconNormal >= 0)
	{
		mdata->xoffset = 0;
		LoadFromToolBar((UINT)nIDNewItem, nIconNormal, mdata->xoffset);
		if (mdata->bitmap) mdata->bitmap->DeleteImageList();
		else mdata->bitmap = new CImageList;
		mdata->bitmap->Create(m_nIconX, m_nIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
		if (!AddBitmapToImageList(mdata->bitmap,nIconNormal))
		{
			mdata->bitmap->DeleteImageList();
			delete mdata->bitmap;
			mdata->bitmap = NULL;
			mdata->menuIconNormal = nIconNormal = -1;
			mdata->xoffset = -1;
		}
	}
	mdata->nFlags = nFlags;
	mdata->nID = nIDNewItem;
	return CMenu::InsertMenu(nPosition,nFlags,nIDNewItem,(LPCTSTR)mdata);
}

BOOL BCMenu::InsertODMenuA(UINT nPos, LPCSTR lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	USES_CONVERSION;
	return InsertODMenuW(nPos, A2W(lpstrText), nFlags, nIDNewItem, il, xoffset);
}

BOOL BCMenu::InsertODMenuW(UINT nPosition, wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	if (!(nFlags & MF_BYPOSITION))
	{
		int iPos = 0;
		BCMenu* pMenu = FindMenuOption(nPosition, iPos);
		if(pMenu)
			return pMenu->InsertODMenuW(iPos, lpstrText, nFlags | MF_BYPOSITION, nIDNewItem, il, xoffset);
		else
			return FALSE;
	}
	
	if (!nIDNewItem) nFlags = MF_SEPARATOR | MF_OWNERDRAW | MF_BYPOSITION;
	else if (!(nFlags & MF_OWNERDRAW)) nFlags |= MF_OWNERDRAW;
	
	if (nFlags & MF_POPUP)
	{
		m_AllSubMenus.Add((HMENU)nIDNewItem);
		m_SubMenus.InsertAt(nPosition,(HMENU)nIDNewItem);
	}
	
	BCMenuData *mdata = new BCMenuData;
	m_MenuList.InsertAt(nPosition, mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation
	
	if (il)
	{
		mdata->menuIconNormal = 0;
		mdata->xoffset = 0;
		if (mdata->bitmap) mdata->bitmap->DeleteImageList();
		else mdata->bitmap = new(CImageList);
		ImageListDuplicate(il, xoffset, mdata->bitmap);
	}
	else
	{
		mdata->menuIconNormal = -1;
		mdata->xoffset = -1;
	}
	mdata->nFlags = nFlags;
	mdata->nID = nIDNewItem;
	return CMenu::InsertMenu(nPosition, nFlags, nIDNewItem, (LPCTSTR)mdata);
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText, UINT_PTR nIDNewItem, int nIconNormal)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nIDNewItem,nIconNormal);//SK: see MFC Tech Note 059
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText, UINT_PTR nIDNewItem, int nIconNormal)
{
	int nLoc;
	BCMenuData *mdata;
	
	// Find the old BCMenuData structure:
	BCMenu *psubmenu = FindMenuOption((int)nIDNewItem,nLoc);
	if (psubmenu && nLoc>=0)
		mdata = psubmenu->m_MenuList[nLoc];
	else
	{
		// Create a new BCMenuData structure:
		mdata = new BCMenuData;
		m_MenuList.Add(mdata);
	}
	
	ASSERT(mdata);
	
	if (lpstrText)
		mdata->SetWideString(lpstrText);  //SK: modified for dynamic allocation

	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset = -1;
	
	if (nIconNormal >= 0)
	{
		mdata->xoffset = 0;
		LoadFromToolBar((UINT)nIDNewItem, nIconNormal, mdata->xoffset);

		if (mdata->bitmap)
			mdata->bitmap->DeleteImageList();
		else
			mdata->bitmap = new CImageList;

		mdata->bitmap->Create(m_nIconX, m_nIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
		if (!AddBitmapToImageList(mdata->bitmap, nIconNormal))
		{
			mdata->bitmap->DeleteImageList();
			delete mdata->bitmap;
			mdata->bitmap = NULL;
			mdata->menuIconNormal = nIconNormal = -1;
			mdata->xoffset = -1;
		}
	}

	mdata->nFlags = MF_BYCOMMAND | MF_OWNERDRAW;
	mdata->nID = nIDNewItem;

	return (CMenu::ModifyMenu((UINT)nIDNewItem,mdata->nFlags,nIDNewItem,(LPCTSTR)mdata));
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT_PTR nIDNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),nIDNewItem,il,xoffset);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nIDNewItem,CImageList *il,int xoffset)
{
  int nLoc;
  BCMenuData *mdata;

  // Find the old BCMenuData structure:
  BCMenu *psubmenu = FindMenuOption((int)nIDNewItem,nLoc);
  if(psubmenu && nLoc>=0)mdata = psubmenu->m_MenuList[nLoc];
  else{
  // Create a new BCMenuData structure:
    mdata = new BCMenuData;
    m_MenuList.Add(mdata);
  }

  ASSERT(mdata);
  if(lpstrText)
     mdata->SetWideString(lpstrText);  //SK: modified for dynamic allocation
  if(il){
    mdata->menuIconNormal = 0;
    mdata->xoffset=0;
    if(mdata->bitmap)mdata->bitmap->DeleteImageList();
    else mdata->bitmap=new(CImageList);
    ImageListDuplicate(il,xoffset,mdata->bitmap);
  }
  else{
    mdata->menuIconNormal = -1;
    mdata->xoffset = -1;
  }
  mdata->nFlags = MF_BYCOMMAND | MF_OWNERDRAW;
  mdata->nID = nIDNewItem;
  return (CMenu::ModifyMenu((UINT)nIDNewItem,mdata->nFlags,nIDNewItem,(LPCTSTR)mdata));
}

BOOL BCMenu::ModifyODMenuA(const char * lpstrText,UINT_PTR nIDNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),nIDNewItem,bmp);
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nIDNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(m_nIconX,m_nIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return ModifyODMenuW(lpstrText,nIDNewItem,&temp,0);
  }
  return ModifyODMenuW(lpstrText,nIDNewItem,NULL,0);
}


BOOL BCMenu::ModifyODMenuA(const char *lpstrText,const char *OptionText,
                           int nIconNormal)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),A2W(OptionText),nIconNormal);//SK: see MFC  Tech Note 059
}

BOOL BCMenu::ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,
                           int nIconNormal)
{
  BCMenuData *mdata;

  // Find the old BCMenuData structure:
  mdata=FindMenuOption(OptionText);
  if(mdata){
    if(lpstrText)
        mdata->SetWideString(lpstrText);//SK: modified for dynamic allocation
    mdata->menuIconNormal = nIconNormal;
    mdata->xoffset=-1;
    if(nIconNormal>=0){
      mdata->xoffset=0;
      if(mdata->bitmap)mdata->bitmap->DeleteImageList();
      else mdata->bitmap=new(CImageList);
      mdata->bitmap->Create(m_nIconX,m_nIconY,ILC_COLORDDB|ILC_MASK,1,1);
	    if(!AddBitmapToImageList(mdata->bitmap,nIconNormal)){
				mdata->bitmap->DeleteImageList();
				delete mdata->bitmap;
				mdata->bitmap=NULL;
				mdata->menuIconNormal = nIconNormal = -1;
				mdata->xoffset = -1;
			}
    }
    return(TRUE);
  }
  return(FALSE);
}

BCMenuData *BCMenu::NewODMenu(UINT pos,UINT nFlags,UINT_PTR nIDNewItem,CString string)
{
  BCMenuData *mdata;

  mdata = new BCMenuData;
  mdata->menuIconNormal = -1;
  mdata->xoffset=-1;
  #ifdef UNICODE
  mdata->SetWideString((LPCTSTR)string);//SK: modified for dynamic allocation
  #else
  mdata->SetAnsiString(string);
  #endif
  mdata->nFlags = nFlags;
  mdata->nID = nIDNewItem;

  if(nFlags & MF_POPUP)m_AllSubMenus.Add((HMENU)nIDNewItem);
  
  if (nFlags&MF_OWNERDRAW){
    ASSERT(!(nFlags&MF_STRING));
    ModifyMenu(pos,nFlags,nIDNewItem,(LPCTSTR)mdata);
  }
  else if (nFlags&MF_STRING){
    ASSERT(!(nFlags&MF_OWNERDRAW));
    ModifyMenu(pos,nFlags,nIDNewItem,mdata->GetString());
  }
  else{
    ASSERT(nFlags&MF_SEPARATOR);
    ModifyMenu(pos,nFlags,nIDNewItem);
  }

  return(mdata);
};

BOOL BCMenu::LoadToolbars(const UINT *arID,int n)
{
  ASSERT(arID);
  BOOL returnflag=TRUE;
  for(int i=0;i<n;++i){
    if(!LoadToolbar(arID[i]))returnflag=FALSE;
  }
  return(returnflag);
}

BOOL BCMenu::LoadToolbar(UINT nToolBar)
{
	UINT nID;
	BOOL returnflag = FALSE;
	CToolBar bar;

	bar.Create(AfxGetMainWnd());
	if (bar.LoadToolBar(nToolBar))
	{
		for (int i=0; i<bar.GetCount(); ++i)
		{
			nID = bar.GetItemID(i); 
			if (nID && GetMenuState(nID, MF_BYCOMMAND) !=0xFFFFFFFF)
				ModifyODMenu(NULL, nID, nToolBar);
		}
		returnflag = TRUE;
	}
	return returnflag;
}

//BOOL BCMenu::LoadToolbar2(UINT nBitmapID, COLORREF clrMask, TBBUTTONEX *ptbe, int nCount)
//{
//	return TRUE;
//
//	CImageList ImgList;
//	HIMAGELIST hImageList;
//	hImageList = ImageList_LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nBitmapID),
//		16, 0, clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION);
//	if (!hImageList)
//		return FALSE;
//
//	ImgList.Attach(hImageList);
//
//	UINT nID;
//	int nOffset;
//	for (int i=0; i<nCount; i++)
//	{
//		nID = ptbe[i].tbinfo.idCommand;
//		nOffset = ptbe[i].tbinfo.iBitmap;
//		if (nID == 0) // seperator
//			continue;
//
//		ModifyODMenu(NULL, nID, &ImgList, nOffset);
//	}
//
//	return TRUE;
//}

BOOL BCMenu::LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset)
{
	int xset, offset;
	UINT nStyle;
	BOOL returnflag = FALSE;
	CToolBar bar;
	
	bar.Create(AfxGetMainWnd());
	if (bar.LoadToolBar(nToolBar))
	{
		offset = bar.CommandToIndex(nID);
		if (offset >= 0)
		{
			bar.GetButtonInfo(offset, nID, nStyle, xset);
			if (xset > 0) xoffset = xset;
			returnflag = TRUE;
		}
	}
	return returnflag;
}

// O.S.
BCMenuData *BCMenu::FindMenuItem(UINT nID)
{
	BCMenuData *pData = NULL;

	INT_PTR nBound = m_MenuList.GetUpperBound();
	for (int i=0; i<=nBound; i++)
	{
		if (m_MenuList[i]->nID == nID)
		{
			pData = m_MenuList[i];
			break;
		}
	}

	if (!pData)
	{
		int loc;
		BCMenu *pMenu = FindMenuOption(nID, loc);
		ASSERT(pMenu != this);
		if (loc >= 0)
			return pMenu->FindMenuItem(nID);
	}
	return pData;
}

BCMenu *BCMenu::FindMenuOption(int nId, int& nLoc)
{
	BCMenu *psubmenu,*pgoodmenu;

	int nCount = GetMenuItemCount();
	for (int i=0; i<nCount; ++i){
#ifdef _CPPRTTI 
		psubmenu = dynamic_cast<BCMenu *>(GetSubMenu(i));
#else
		psubmenu=(BCMenu *)GetSubMenu(i);
#endif
		if (psubmenu)
		{
			pgoodmenu = psubmenu->FindMenuOption(nId, nLoc);
			if (pgoodmenu)
				return pgoodmenu;
		}
		else if (nId == (int)GetMenuItemID(i))
		{
			nLoc = i;
			return this;
		}
	}
	nLoc = -1;
	return NULL;
}

BCMenuData *BCMenu::FindMenuOption(wchar_t *lpstrText)
{
	int i,j;
	BCMenu *psubmenu;
	BCMenuData *pmenulist;

	int nCount = GetMenuItemCount();
	for (i=0; i<nCount; ++i)
	{
#ifdef _CPPRTTI 
		psubmenu = dynamic_cast<BCMenu *>(GetSubMenu(i));
#else
		psubmenu = (BCMenu *)GetSubMenu(i);
#endif
		if (psubmenu)
		{
			pmenulist = psubmenu->FindMenuOption(lpstrText);
			if (pmenulist)
				return pmenulist;
		}
		else
		{
			const wchar_t *szWide;//SK: we use const to prevent misuse of this Ptr
			for (j=0; j<=m_MenuList.GetUpperBound(); ++j)
			{
				szWide = m_MenuList[j]->GetWideString ();
				if (szWide && !wcscmp(lpstrText, szWide))//SK: modified for dynamic allocation
					return m_MenuList[j];
			}
		}
	}
	return NULL;
}


BOOL BCMenu::LoadMenu(int nResource)
{
	return BCMenu::LoadMenu(MAKEINTRESOURCE(nResource));
}

BOOL BCMenu::LoadMenu(LPCTSTR lpszResourceName)
{
	TRACE(_T("IMPORTANT:Use BCMenu::DestroyMenu to destroy Loaded Menu's\n"));
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);
	
	// Find the Menu Resource:
	HINSTANCE m_hInst = AfxFindResourceHandle(lpszResourceName,RT_MENU);
	HRSRC hRsrc = ::FindResource(m_hInst,lpszResourceName,RT_MENU);
	if(hRsrc == NULL)
		return FALSE;
	
	// Load the Menu Resource:
	HGLOBAL hGlobal = LoadResource(m_hInst, hRsrc);
	if(hGlobal == NULL)
		return FALSE;
	
	// Attempt to create us as a menu...
	if (!CMenu::CreateMenu())
		return FALSE;
	
	// Get Item template Header, and calculate offset of MENUITEMTEMPLATES
	MENUITEMTEMPLATEHEADER *pTpHdr = (MENUITEMTEMPLATEHEADER*)LockResource(hGlobal);
	BYTE *pTp = (BYTE *)pTpHdr + (sizeof(MENUITEMTEMPLATEHEADER) + pTpHdr->offset);
	
	// Variables needed during processing of Menu Item Templates:
	INT_PTR	j = 0;
	WORD	dwFlags = 0;              // Flags of the Menu Item
	WORD	dwID = 0;              // ID of the Menu Item
	UINT	uFlags;                  // Actual Flags.
	LPWSTR	lpwszCaption = NULL;
	int		nLen = 0;                // Length of caption
	
	CTypedPtrArray<CPtrArray, BCMenu*> m_Stack;    // Popup menu stack
	CArray<BOOL, BOOL> m_StackEnd;    // Popup menu stack
	m_Stack.Add(this);                  // Add it to this...
	m_StackEnd.Add(FALSE);
	
	do
	{
		// Obtain Flags and (if necessary), the ID...
		memcpy(&dwFlags, pTp, sizeof(WORD)); // Obtain Flags
		pTp += sizeof(WORD);
		if (!(dwFlags & MF_POPUP))
		{
			memcpy(&dwID, pTp, sizeof(WORD)); // Obtain ID
			pTp+=sizeof(WORD);
		}
		else
			dwID = 0;
		
		uFlags = (UINT)dwFlags; // Remove MF_END from the flags that will
		if(uFlags & MF_END) // be passed to the Append(OD)Menu functions.
			uFlags -= MF_END;
		
		// Obtain Caption (and length)
		nLen = 0;
		lpwszCaption = new WCHAR[wcslen((LPWSTR)pTp)+1];
		wcscpy(lpwszCaption, (LPWSTR)pTp);
		pTp = &pTp[(wcslen((LPWSTR)pTp)+1) * sizeof(WCHAR)]; //modified SK
		
		// Handle popup menus first....
		
		// WideCharToMultiByte
		if (dwFlags & MF_POPUP)
		{
			if(dwFlags & MF_END)
				m_StackEnd.SetAt(m_Stack.GetUpperBound(), TRUE);

			BCMenu* pSubMenu = new BCMenu;
			pSubMenu->m_unselectcheck	= m_unselectcheck;
			pSubMenu->m_selectcheck		= m_selectcheck;
			pSubMenu->checkmaps			= checkmaps;
			pSubMenu->checkmapsshare	= TRUE;
			pSubMenu->CreatePopupMenu();
			
			// Append it to the top of the stack:
			m_Stack[m_Stack.GetUpperBound()]->AppendODMenuW(lpwszCaption, uFlags, (UINT_PTR)pSubMenu->m_hMenu, -1);
			m_Stack.Add(pSubMenu);
			m_StackEnd.Add(FALSE);
		}
		else
		{
			m_Stack[m_Stack.GetUpperBound()]->AppendODMenuW(lpwszCaption, uFlags, dwID, -1);
			
			if (dwFlags & MF_END)
				m_StackEnd.SetAt(m_Stack.GetUpperBound(), TRUE);

			j = m_Stack.GetUpperBound();
			while (j >= 0 && m_StackEnd.GetAt(j))
			{
				m_Stack[m_Stack.GetUpperBound()]->InsertSpaces();
				m_Stack.RemoveAt(j);
				m_StackEnd.RemoveAt(j);
				--j;
			}
		}
		
		delete[] lpwszCaption;
	}
	while (m_Stack.GetUpperBound() != -1);

	CString str;
	for (int i=0; i<(int)GetMenuItemCount(); ++i)
	{
		str = m_MenuList[i]->GetString();
		if (GetSubMenu(i))
		{
			m_MenuList[i]->nFlags = MF_POPUP | MF_BYPOSITION;
			ModifyMenu(i, MF_POPUP | MF_BYPOSITION, (UINT_PTR)GetSubMenu(i)->m_hMenu, str);
		}
		else
		{
			m_MenuList[i]->nFlags = MF_STRING | MF_BYPOSITION;
			ModifyMenu(i, MF_STRING | MF_BYPOSITION, m_MenuList[i]->nID, str);
		}
	}
	
	return (TRUE);
}

void BCMenu::InsertSpaces(void)
{
	int i, j, nCount, maxlength;
	CString string, newstring;
	CSize t;
	
	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof (NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

	CFont ftMenu;
	ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
	
	CWnd *pWnd = CWnd::GetDesktopWindow();
	CDC *pDC = pWnd->GetDC();
	CFont* pOldFont = pDC->SelectObject(&ftMenu);
	
	nCount = GetMenuItemCount();
	maxlength = -1;
	for (i=0; i<nCount; ++i)
	{
		string = m_MenuList[i]->GetString();
		j = string.Find((char)9);
		
		newstring.Empty();
		if (j != -1)	newstring = string.Left(j);
		else			newstring = string;

		newstring+=_T(" ");//SK: modified for Unicode correctness. 
		LPCTSTR lpstrText = (LPCTSTR)newstring;
		t = pDC->GetTextExtent(lpstrText, (int)_tcslen(lpstrText));
		if (t.cx > maxlength)
			maxlength = t.cx;
	}
	
	for(i=0; i<nCount; ++i)
	{
		string = m_MenuList[i]->GetString();
		j = string.Find((char)9);
		if (j != -1)
		{
			newstring.Empty();
			newstring = string.Left(j);
			LPCTSTR lpstrText = (LPCTSTR)(newstring);
			t = pDC->GetTextExtent(lpstrText, (int)_tcslen(lpstrText));
			while (t.cx < maxlength)
			{
				newstring += _T(' ');//SK: modified for Unicode correctness
				LPCTSTR lpstrText = (LPCTSTR)(newstring);
				t = pDC->GetTextExtent(lpstrText, (int)_tcslen(lpstrText));
			}
			newstring+=string.Mid(j);
#ifdef UNICODE
			m_MenuList[i]->SetWideString(newstring);//SK: modified for dynamic allocation
#else
			m_MenuList[i]->SetAnsiString(newstring);
#endif
		}
	}

	pDC->SelectObject(pOldFont);              // Select old font in
	pWnd->ReleaseDC(pDC);       // Release the DC
	ftMenu.DeleteObject();
}

void BCMenu::LoadCheckmarkBitmap(int unselect, int select)
{
	if(unselect > 0 && select > 0)
	{
		m_selectcheck = select;
		m_unselectcheck = unselect;
		
		if (checkmaps)	checkmaps->DeleteImageList();
		else			checkmaps = new(CImageList);
		checkmaps->Create(m_nIconX,m_nIconY,ILC_MASK,2,1);

		BOOL flag1 = AddBitmapToImageList(checkmaps, unselect);
		BOOL flag2 = AddBitmapToImageList(checkmaps, select);
		if (!flag1 || !flag2)
		{
			checkmaps->DeleteImageList();
			delete checkmaps;
			checkmaps=NULL;
		}
	}
}

//--------------------------------------------------------------------------
//[18.06.99 rj]
BOOL BCMenu::GetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
{
	BOOL bResult = FALSE;
	
	if (MF_BYPOSITION & nFlags)
	{
		INT_PTR numMenuItems = m_MenuList.GetUpperBound();
		if (numMenuItems != -1 && id <= (UINT)numMenuItems && m_MenuList[id])
		{
			string = m_MenuList[id]->GetString();
			bResult = TRUE;
		}
	}
	else
	{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->GetMenuText(uiLoc, string);
	}

	return bResult;
}

BOOL BCMenu::GetMenuText(UINT id, LPTSTR lpszString, UINT nFlags/*= MF_BYPOSITION*/)
{
	BOOL bResult = FALSE;
	
	if (MF_BYPOSITION & nFlags)
	{
		INT_PTR numMenuItems = m_MenuList.GetUpperBound();
		if (numMenuItems != -1 && id <= (UINT)numMenuItems)
		{
			_tcscpy(lpszString, m_MenuList[id]->GetString());
			bResult = TRUE;
		}
	}
	else
	{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->GetMenuText(uiLoc, lpszString);
	}

	return bResult;
}

BOOL BCMenu::SetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
{
	BOOL bResult = FALSE;
	
	if (MF_BYPOSITION & nFlags)
	{
		INT_PTR numMenuItems = m_MenuList.GetUpperBound();
		if (numMenuItems != -1 && id <= (UINT)numMenuItems)
		{
#ifdef _UNICODE
			m_MenuList[id]->SetWideString(string);
#else
			m_MenuList[id]->SetAnsiString(string);
#endif
			bResult = TRUE;
		}
	}
	else
	{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id,uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuText(uiLoc, string);
	}

	return bResult;
}

BOOL BCMenu::SetMenuText(UINT id, LPTSTR lpszString, UINT nFlags/* = MF_BYPOSITION*/)
{
	BOOL bResult = FALSE;
	
	if (MF_BYPOSITION & nFlags)
	{
		INT_PTR numMenuItems = m_MenuList.GetUpperBound();
		if (numMenuItems != -1 && id <= (UINT)numMenuItems)
		{
#ifdef _UNICODE
			m_MenuList[id]->SetWideString(lpszString);
#else
			m_MenuList[id]->SetAnsiString(lpszString);
#endif
			bResult = TRUE;
		}
	}
	else
	{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuText(uiLoc, lpszString);
	}

	return bResult;
}

BOOL BCMenu::SetMenuAccelKey(UINT id, LPCTSTR lpcszAccelKey, UINT nFlags/* = MF_BYPOSITION*/)
{
	BOOL bResult = FALSE;
	
	if (MF_BYPOSITION & nFlags)
	{
		INT_PTR numMenuItems = m_MenuList.GetUpperBound();
		if (numMenuItems != -1 && id <= (UINT)numMenuItems)
		{
			m_MenuList[id]->strAccelKey = lpcszAccelKey;
			bResult = TRUE;
		}
	}
	else
	{
		int uiLoc;
		BCMenu* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuAccelKey(uiLoc, lpcszAccelKey);
	}

	return bResult;
}

void BCMenu::DrawRadioDot(CDC *pDC, int x, int y, COLORREF color)
{
	CRect rcDot(x,y,x+7,y+7);
	
	CBrush brush;
	brush.CreateSolidBrush(color);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, color);
	CPen *pOldPen = pDC->SelectObject(&pen);

	pDC->Ellipse(&rcDot);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	
	pen.DeleteObject();
	brush.DeleteObject();
}

void BCMenu::DrawCheckMark(CDC* pDC, int x, int y, COLORREF color)
{
	pDC->SetPixel(x,y+2,color);
	pDC->SetPixel(x,y+3,color);
	pDC->SetPixel(x,y+4,color);
	
	pDC->SetPixel(x+1,y+3,color);
	pDC->SetPixel(x+1,y+4,color);
	pDC->SetPixel(x+1,y+5,color);
	
	pDC->SetPixel(x+2,y+4,color);
	pDC->SetPixel(x+2,y+5,color);
	pDC->SetPixel(x+2,y+6,color);
	
	pDC->SetPixel(x+3,y+3,color);
	pDC->SetPixel(x+3,y+4,color);
	pDC->SetPixel(x+3,y+5,color);
	
	pDC->SetPixel(x+4,y+2,color);
	pDC->SetPixel(x+4,y+3,color);
	pDC->SetPixel(x+4,y+4,color);
	
	pDC->SetPixel(x+5,y+1,color);
	pDC->SetPixel(x+5,y+2,color);
	pDC->SetPixel(x+5,y+3,color);
	
	pDC->SetPixel(x+6,y,color);
	pDC->SetPixel(x+6,y+1,color);
	pDC->SetPixel(x+6,y+2,color);
}

BCMenuData *BCMenu::FindMenuList(UINT_PTR nIDNewItem)
{
	INT_PTR nCount = m_MenuList.GetUpperBound();
	for(INT_PTR i=0; i<=nCount; ++i)
	{
		if (m_MenuList[i]->nID == nIDNewItem && !m_MenuList[i]->syncflag)
		{
			m_MenuList[i]->syncflag=1;
			return (m_MenuList[i]);
		}
	}
	return (NULL);
}

void BCMenu::InitializeMenuList(int value)
{
	INT_PTR nCount = m_MenuList.GetUpperBound();
	for(INT_PTR i=0; i<=nCount; ++i)
		m_MenuList[i]->syncflag=value;
}

void BCMenu::DeleteMenuList(void)
{
	for (int i=0; i<=m_MenuList.GetUpperBound(); ++i)
	{
		if (!m_MenuList[i]->syncflag)
			delete m_MenuList[i];
	}
}

void BCMenu::SynchronizeMenu(void)
{
	CTypedPtrArray<CPtrArray, BCMenuData*> temp;
	BCMenuData *mdata;
	CString string;
	UINT_PTR submenu;
	UINT nID=0, state, j;
	
	InitializeMenuList(0);
	for (j=0;j<GetMenuItemCount();++j)
	{
		mdata = NULL;
		state = GetMenuState(j, MF_BYPOSITION);
		if (state & MF_POPUP)
		{
			submenu = (UINT_PTR)GetSubMenu(j)->m_hMenu;
			mdata = FindMenuList(submenu);
			GetMenuString(j, string, MF_BYPOSITION);

			if (!mdata)
				mdata = NewODMenu(j, (state & 0xFF) | MF_BYPOSITION | MF_POPUP | MF_OWNERDRAW, submenu, string);
			else if(string.GetLength()>0)
			{
#ifdef UNICODE
				mdata->SetWideString(string);  //SK: modified for dynamic allocation
#else
				mdata->SetAnsiString(string);
#endif
			}
		}
		else if (state & MF_SEPARATOR)
		{
			mdata = FindMenuList(0);
			if (!mdata)
				mdata = NewODMenu(j, state | MF_BYPOSITION | MF_SEPARATOR | MF_OWNERDRAW, 0, _T(""));//SK: modified for Unicode correctness
			else
				ModifyMenu(j,mdata->nFlags,nID,(LPCTSTR)mdata);
		}
		else
		{
			nID = GetMenuItemID(j);
			mdata = FindMenuList(nID);
			GetMenuString(j, string, MF_BYPOSITION);
			if(!mdata)
				mdata=NewODMenu(j, state | MF_BYPOSITION | MF_OWNERDRAW, nID, string);
			else
			{
				mdata->nFlags = state | MF_BYPOSITION | MF_OWNERDRAW;
				if (string.GetLength() > 0)
				{
#ifdef UNICODE
					mdata->SetWideString(string);//SK: modified for dynamic allocation
#else
					mdata->SetAnsiString(string);
#endif
				}
				
				ModifyMenu(j, mdata->nFlags, nID, (LPCTSTR)mdata);
			}
		}
		
		if (mdata)
			temp.Add(mdata);
	}

	DeleteMenuList();
	m_MenuList.RemoveAll();
	m_MenuList.Append(temp);
	temp.RemoveAll(); 
}

void BCMenu::UpdateMenu(CMenu *pmenu)
{
#ifdef _CPPRTTI 
	BCMenu *psubmenu = dynamic_cast<BCMenu *>(pmenu);
#else
	BCMenu *psubmenu = (BCMenu *)pmenu;
#endif
	
	if (psubmenu)
		psubmenu->SynchronizeMenu();
}

LRESULT BCMenu::FindKeyboardShortcut(UINT nChar, UINT nFlags, CMenu *pMenu)
{
#ifdef _CPPRTTI 
	BCMenu *pBCMenu = dynamic_cast<BCMenu *>(pMenu);
#else
	BCMenu *pBCMenu = (BCMenu *)pMenu;
#endif

	if (pBCMenu && nFlags & MF_POPUP)
	{
		CString key(_T('&'), 2); //SK: modified for Unicode correctness
		key.SetAt(1, (TCHAR)nChar);
		key.MakeLower();

		UINT nState;
		CString menutext;
		int menusize = (int)pBCMenu->GetMenuItemCount();
		if (menusize != (pBCMenu->m_MenuList.GetUpperBound()+1))
			pBCMenu->SynchronizeMenu();

		for (int i=0; i<menusize; ++i)
		{
			nState = pBCMenu->GetMenuState(i, MF_BYPOSITION);
			if ((nState & MF_DISABLED) != MF_DISABLED && pBCMenu->GetMenuText(i,menutext))
			{
				menutext.MakeLower();
				if (menutext.Find(key) >= 0)
				{
					return (MAKELRESULT(i, MNC_EXECUTE)); // MNC_EXECUTE - 2
				}
			}
		}
	}

	return(0);
}

BOOL BCMenu::AddBitmapToImageList(CImageList *bmplist, UINT nResourceID)
{
	// O.S.
	if (m_bDynIcons)
	{
		bmplist->Add((HICON)(uintptr_t)nResourceID);
		return TRUE;
	}
	
	BOOL bResult = FALSE;
	HBITMAP hbmp = LoadSysColorBitmap(nResourceID);
	if (hbmp)
	{
		CBitmap bmp;
		bmp.Attach(hbmp);
		
		if (m_bitmapBackgroundFlag)
			bmplist->Add(&bmp, m_bitmapBackground);
		else
			bmplist->Add(&bmp, GetSysColor(COLOR_3DFACE));

		bmp.Detach();
		DeleteObject(hbmp);
		bResult = TRUE;
	}
	
	return bResult;
}

void BCMenu::SetBitmapBackground(COLORREF color)
{
  m_bitmapBackground=color;
  m_bitmapBackgroundFlag=TRUE;
}

void BCMenu::UnSetBitmapBackground(void)
{
  m_bitmapBackgroundFlag=FALSE;
}

// Given a toolbar, append all the options from it to this menu
// Passed a ptr to the toolbar object and the toolbar ID
// Author : Robert Edward Caldecott
void BCMenu::AddFromToolBar(CToolBar* pToolBar, int nResourceID)
{
  for (int i = 0; i < pToolBar->GetCount(); i++) {
    UINT nID = pToolBar->GetItemID(i);
    // See if this toolbar option
    // appears as a command on this
    // menu or is a separator
    if (nID == 0 || GetMenuState(nID, MF_BYCOMMAND) == 0xFFFFFFFF)
      continue; // Item doesn't exist
    UINT nStyle;
    int nImage;
    // Get the toolbar button info
    pToolBar->GetButtonInfo(i, nID, nStyle, nImage);
    // OK, we have the command ID of the toolbar
    // option, and the tollbar bitmap offset
    int nLoc;
    BCMenuData* pData;
    BCMenu *pSubMenu = FindMenuOption(nID, nLoc);
    if (pSubMenu && nLoc >= 0)pData = pSubMenu->m_MenuList[nLoc];
    else {
      // Create a new BCMenuData structure
      pData = new BCMenuData;
      m_MenuList.Add(pData);
    }
    // Set some default structure members
    pData->menuIconNormal = nResourceID;
    pData->nID = nID;
    pData->nFlags =  MF_BYCOMMAND | MF_OWNERDRAW;
    pData->xoffset = nImage;
    if (pData->bitmap)pData->bitmap->DeleteImageList();
    else pData->bitmap = new CImageList;
    pData->bitmap->Create(m_nIconX, m_nIconY,ILC_COLORDDB|ILC_MASK, 1, 1);

    if(!AddBitmapToImageList(pData->bitmap, nResourceID)){
			pData->bitmap->DeleteImageList();
			delete pData->bitmap;
			pData->bitmap=NULL;
      pData->menuIconNormal = -1;
      pData->xoffset = -1;
    }

    // Modify our menu
    ModifyMenu(nID,pData->nFlags,nID,(LPCTSTR)pData);
  }
}

BOOL BCMenu::Draw3DCheckmark(CDC *dc, const CRect& rc, BOOL bSelected, HBITMAP hbmCheck)
{
	if (!hbmCheck)	DrawCheckMark(dc,rc.left+4,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	else			DrawRadioDot(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	return TRUE;
}

HBITMAP BCMenu::LoadSysColorBitmap(int nResourceId)
{
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResourceId), RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nResourceId), RT_BITMAP);
	if (hRsrc == NULL)
		return NULL;

	return AfxLoadSysColorBitmap(hInst, hRsrc, FALSE);
}

BOOL BCMenu::RemoveMenu(UINT uiId,UINT nFlags)
{
	if(MF_BYPOSITION&nFlags){
    UINT uint = GetMenuState(uiId,MF_BYPOSITION);
    if(uint&MF_SEPARATOR && !(uint&MF_POPUP)){
		  delete m_MenuList.GetAt(uiId);
  	  m_MenuList.RemoveAt(uiId);
    }
    else{
  		BCMenu* pSubMenu = (BCMenu*) GetSubMenu(uiId);
	  	if(NULL==pSubMenu){
		  	UINT uiCommandId = GetMenuItemID(uiId);
			  for(int i=0;i<m_MenuList.GetSize(); i++){
				  if(m_MenuList[i]->nID==uiCommandId){
  					delete m_MenuList.GetAt(i);
	  				m_MenuList.RemoveAt(i);
		  			break;
			  	}
			  }
		  }
		  else{
        INT_PTR numSubMenus = m_SubMenus.GetUpperBound();
        for(INT_PTR m = numSubMenus; m >= 0; m--){
          if(m_SubMenus[m]==pSubMenu->m_hMenu){
            INT_PTR numAllSubMenus = m_AllSubMenus.GetUpperBound();
            for(INT_PTR n = numAllSubMenus; n>= 0; n--){
              if(m_AllSubMenus[n]==m_SubMenus[m])m_AllSubMenus.RemoveAt(n);
            }
	  				m_SubMenus.RemoveAt(m);
          }
        }
        INT_PTR num = pSubMenu->GetMenuItemCount();
		INT_PTR i;
        for(i=num-1;i>=0;--i)pSubMenu->RemoveMenu((UINT)i,MF_BYPOSITION);
  			for(i=m_MenuList.GetUpperBound();i>=0;i--){
	  			if(m_MenuList[i]->nID==(UINT_PTR)pSubMenu->m_hMenu){
		  			delete m_MenuList.GetAt(i);
			  		m_MenuList.RemoveAt(i);
				  	break;
				  }
			  }
			  delete pSubMenu;
      }
		}
	}
	else{
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(uiId,iPosition);
		if(pMenu)pMenu->RemoveMenu(iPosition,MF_BYPOSITION);
	}
	return CMenu::RemoveMenu(uiId,nFlags);
}

BOOL BCMenu::DeleteMenu(UINT uiId,UINT nFlags)
{
	if (MF_BYPOSITION & nFlags)
	{
		UINT uint = GetMenuState(uiId, MF_BYPOSITION);
		if (uint & MF_SEPARATOR && !(uint & MF_POPUP))
		{
			delete m_MenuList.GetAt(uiId);
			m_MenuList.RemoveAt(uiId);
		}
		else
		{
			BCMenu *pSubMenu = (BCMenu *)GetSubMenu(uiId);
			if (NULL == pSubMenu)
			{
				UINT uiCommandId = GetMenuItemID(uiId);
				for (int i=0; i<m_MenuList.GetSize(); i++)
				{
					if (m_MenuList[i]->nID == uiCommandId)
					{
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
			}
			else
			{
				INT_PTR numSubMenus = m_SubMenus.GetUpperBound();
				for (INT_PTR m=numSubMenus; m>=0; m--)
				{
					if (m_SubMenus[m] == pSubMenu->m_hMenu)
					{
						INT_PTR numAllSubMenus = m_AllSubMenus.GetUpperBound();
						for(INT_PTR n=numAllSubMenus; n>=0; n--)
						{
							if (m_AllSubMenus[n] == m_SubMenus[m])
								m_AllSubMenus.RemoveAt(n);
						}
						m_SubMenus.RemoveAt(m);
					}
				}
				INT_PTR i;
				INT_PTR num = pSubMenu->GetMenuItemCount();
				for (i=num-1; i>=0; --i)
					pSubMenu->DeleteMenu((UINT)i, MF_BYPOSITION);
				for (i=m_MenuList.GetUpperBound(); i>=0; i--)
				{
					if (m_MenuList[i]->nID == (UINT_PTR)pSubMenu->m_hMenu)
					{
						delete m_MenuList.GetAt(i);
						m_MenuList.RemoveAt(i);
						break;
					}
				}
				delete pSubMenu;
			}
		}
	}
	else
	{
		int iPosition =0;
		BCMenu* pMenu = FindMenuOption(uiId, iPosition);
		if (pMenu)
			pMenu->DeleteMenu(iPosition, MF_BYPOSITION);
	}
	return CMenu::DeleteMenu(uiId,nFlags);
}


BOOL BCMenu::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL BCMenu::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL BCMenu::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL BCMenu::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(m_nIconX,m_nIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,&temp,0);
  }
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL BCMenu::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL BCMenu::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(m_nIconX,m_nIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,&temp,0);
  }
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

//--------------------------------------------------------------------------
//[21.06.99 rj]
BCMenu* BCMenu::AppendODPopupMenuW(wchar_t *lpstrText)
{
	BCMenu* pSubMenu = new BCMenu;
	pSubMenu->m_unselectcheck=m_unselectcheck;
	pSubMenu->m_selectcheck=m_selectcheck;
	pSubMenu->checkmaps=checkmaps;
	pSubMenu->checkmapsshare=TRUE;
	pSubMenu->CreatePopupMenu();
	AppendODMenuW(lpstrText,MF_POPUP,(UINT_PTR)pSubMenu->m_hMenu, -1);
	return pSubMenu;
}

//--------------------------------------------------------------------------
//[21.06.99 rj]
BCMenu* BCMenu::AppendODPopupMenuA(LPCSTR lpstrText)
{
	USES_CONVERSION;
	return AppendODPopupMenuW(A2W(lpstrText));
}

BOOL BCMenu::ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist)
{
  if (il == NULL||newlist==NULL||xoffset<0) return FALSE;
  HICON hIcon = il->ExtractIcon(xoffset);
  int cx, cy;
  ImageList_GetIconSize(il->m_hImageList, &cx, &cy);
  newlist->Create(cx,cy,ILC_COLOR32|ILC_MASK,1,1);
  newlist->Add(hIcon);
  ::DestroyIcon(hIcon);
  return TRUE;
}

//*************************************************************************


BOOL BCMenu::TrackPopupMenu(UINT nFlags, CPoint pt, CWnd* pWnd, LPCRECT lpRect /*= 0*/)
{
	return TrackPopupMenu(nFlags, pt.x, pt.y, pWnd, lpRect);
}

BOOL BCMenu::TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect /*= 0*/)
{
	if (mAnimationMenu == XPR_FALSE)
		nFlags |= 0x4000L;

	return CMenu::TrackPopupMenu(nFlags, x, y, pWnd, lpRect);
}
