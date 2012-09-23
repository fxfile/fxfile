//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv2 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "MenuEx.h"

#include "xpr/xpr_char.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace rgc
{
#define GAP                1
#define SEPARATOR_HEIGHT   5
#define MAX_MENU_WIDTH     350

class MenuEx::MenuData
{
    friend class MenuEx;

public:
    MenuData(void)
        : mAccelText(XPR_NULL)
        , mImageList(XPR_NULL), mImage(InvalidImageIndex)
    {
    }

    ~MenuData(void)
    {
        XPR_SAFE_DELETE_ARRAY(mAccelText);
        XPR_SAFE_DELETE(mImageList);
    }

public:
    void SetAccelText(const xpr_tchar_t *aAccelText)
    {
        if (aAccelText == XPR_NULL)
            return;

        XPR_SAFE_DELETE_ARRAY(mAccelText);

        xpr_size_t sAccelTextLength = _tcslen(aAccelText);
        if (sAccelTextLength > 0)
        {
            mAccelText = new xpr_tchar_t[sAccelTextLength + 1];
            if (mAccelText != XPR_NULL)
            {
                _tcscpy(mAccelText, aAccelText);
            }
        }
    }

protected:
    xpr_tchar_t *mAccelText;
    CImageList  *mImageList;
    xpr_sint_t   mImage;
};

MenuEx::MenuSet MenuEx::mAllSubMenus;

IMPLEMENT_DYNCREATE(MenuEx, CMenu)

MenuEx::MenuEx(void)
    : mIconX(16), mIconY(16)
{
}

MenuEx::~MenuEx(void)
{
    DestroyMenu();
}

BOOL MenuEx::IsMenu(CMenu *pSubMenu)
{
    if (pSubMenu == XPR_NULL)
        return FALSE;

    return IsMenu(pSubMenu->m_hMenu);
}

BOOL MenuEx::IsMenu(HMENU hSubMenu)
{
    if (mAllSubMenus.find(hSubMenu) == mAllSubMenus.end())
        return FALSE;

    return TRUE;
}

LRESULT MenuEx::OnMenuChar(UINT nChar, UINT nFlags, CMenu *pMenu)
{
    MenuEx *pMenuEx = dynamic_cast<MenuEx *>(pMenu);
    if (pMenuEx == XPR_NULL)
        return 0;

    if (XPR_TEST_BITS(nFlags, MF_POPUP))
    {
        xpr_tchar_t szCharWithAmpersand[3];
        _tcscpy(szCharWithAmpersand, _T("&&"));
        szCharWithAmpersand[1] = (TCHAR)xpr::toLower(nChar);

        xpr_sint_t i, sMenuCount;
        xpr_uint_t sState;
        CString sText;

        sMenuCount = (xpr_sint_t)pMenuEx->GetMenuItemCount();
        for (i = 0; i < sMenuCount; ++i)
        {
            sState = pMenuEx->GetMenuState(i, MF_BYPOSITION);
            if (XPR_TEST_BITS(sState, MF_DISABLED))
                continue;

            if (pMenuEx->GetMenuText(i, sText) == XPR_TRUE)
            {
                sText.MakeLower();
                if (sText.Find(szCharWithAmpersand) >= 0)
                {
                    return MAKELRESULT(i, MNC_EXECUTE); // MNC_EXECUTE - 2
                }
            }
        }
    }

    return 0;
}

BOOL MenuEx::DestroyMenu(void)
{
    HMENU sSubMenu;
    CMenu *sMenu;
    MenuEx *sMenuEx;
    MenuSet::iterator sSubMenuIterator;

    //
    // erase current menu
    //
    sSubMenuIterator = mAllSubMenus.find(m_hMenu);
    if (sSubMenuIterator != mAllSubMenus.end())
        mAllSubMenus.erase(sSubMenuIterator);

    //
    // clear all sub-menu
    //
    sSubMenuIterator = mSubMenus.begin();
    for (; sSubMenuIterator != mSubMenus.end(); ++sSubMenuIterator)
    {
        sSubMenu = *sSubMenuIterator;

        mAllSubMenus.erase(sSubMenu);

        sMenu = CMenu::FromHandle(sSubMenu);
        if (sMenu != XPR_NULL)
        {
            //sMenu->IsKindOf(RUNTIME_CLASS(MenuEx))

            sMenuEx = dynamic_cast<MenuEx *>(sMenu);
            if (sMenuEx != XPR_NULL)
            {
                XPR_SAFE_DELETE(sMenuEx);
            }
        }
    }

    mSubMenus.clear();

    //
    // clear menu item data
    //
    MenuData *sMenuData;
    MenuDataDeque::iterator sMenuDataIterator;

    sMenuDataIterator = mMenuDatas.begin();
    for (; sMenuDataIterator != mMenuDatas.end(); ++sMenuDataIterator)
    {
        sMenuData = *sMenuDataIterator;
        XPR_SAFE_DELETE(sMenuData);
    }

    mMenuDatas.clear();

	return CMenu::DestroyMenu();
}

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

static void DrawVistaStyle_MenuSel(CDC *pDC, CRect rc, COLORREF clr)
{
	COLORREF clrBegin[2];
	COLORREF clrEnd[2];

	COLORREF clrBorder = LightenColor(clr, 0.50);
	COLORREF clrLightenBorder = LightenColor(clrBorder, 0.20);

	clrBegin[0] = LightenColor(clr, 0.95);
	clrEnd[0]   = LightenColor(clr, 0.80);

	clrBegin[1] = LightenColor(clr, 0.70);
	clrEnd[1]   = LightenColor(clr, 0.80);

	CRect rcGradient(rc);
	rcGradient.DeflateRect(1,1,2,1);

	CRect rc0(rcGradient);
	CRect rc1(rcGradient);

	rc0.bottom = rc0.top + (rc0.Height()/2);
	rc1.top = rc0.bottom;

	DrawVertGradient(pDC, rc0, clrBegin[0], clrEnd[0]);
	DrawVertGradient(pDC, rc1, clrBegin[1], clrEnd[1]);

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

	COLORREF clrBorder = LightenColor(clr, 0.50);
	COLORREF clrLightBorder = LightenColor(clrBorder, 0.20);

	clrBegin[0] = LightenColor(clr, 0.95);
	clrEnd[0]   = LightenColor(clr, 0.80);

	clrBegin[1] = LightenColor(clr, 0.70);
	clrEnd[1]   = LightenColor(clr, 0.80);

	clrBegin[0] = LightenColor(clr, 0.95);
	clrEnd[0]   = LightenColor(clr, 0.80);

	clrBegin[1] = LightenColor(clr, 0.70);
	clrEnd[1]   = LightenColor(clr, 0.80);

	CRect rcGradient(rc);
	rcGradient.DeflateRect(1,1,2,1);

	CRect rc0(rcGradient);
	CRect rc1(rcGradient);

	rc0.bottom = rc0.top + (rc0.Height()/2);
	rc1.top = rc0.bottom;

	DrawVertGradient(pDC, rc0, clrBegin[0], clrEnd[0]);
	DrawVertGradient(pDC, rc1, clrBegin[1], clrEnd[1]);

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

void MenuEx::DrawItemOffice2007Style(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    CRect rc(lpDIS->rcItem);
    MenuData *pMenuData = GetMenuData(lpDIS->itemID);
	UINT state = lpDIS->itemState;

	COLORREF crBack   = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 240);
	COLORREF crBegin  = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd    = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += mIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + mIconX+4;
		DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);

		CRect rcSept(rc);
		rcSept.top  += rcSept.Height() >> 1;
		rcSept.left += mIconX + 8 + GAP;

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
            nIconNormal	= pMenuData->mImage;
			pImgList	= pMenuData->mImageList;
			GetMenuText(lpDIS->itemID, strText, MF_BYCOMMAND);
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
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
			rcIcon.right = rcIcon.left + mIconX + 4;
			DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - mIconY) / 2;
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
			CRect rc2(rc.left+2, rc.top+2+dy, rc.left+mIconX+1, rc.top+mIconY+2+dy);
			
			MenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + mIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

            if (pMenuData->mAccelText != XPR_NULL && pMenuData->mAccelText[0] != _T('\0'))
			{
				strLeft  = strText;
				strRight = pMenuData->mAccelText;
				rcText.right -= mIconX;
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

void MenuEx::DrawItemOffice2003Style(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
    MenuData *pMenuData = GetMenuData(lpDIS->itemID);
    UINT state = lpDIS->itemState;

	COLORREF crBack   = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 240);
	COLORREF crBegin  = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd    = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += mIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + mIconX+4;
		DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);

		CRect rcSept(rc);
		rcSept.top  += rcSept.Height() >> 1;
		rcSept.left += mIconX + 8 + GAP;

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
            nIconNormal	= pMenuData->mImage;
            pImgList	= pMenuData->mImageList;
			GetMenuText(lpDIS->itemID, strText, MF_BYCOMMAND);
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
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
			COLORREF clrAlpha = LightenColor(GetSysColor(COLOR_HIGHLIGHT), 0.80);

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
			rcIcon.right = rcIcon.left + mIconX + 4;
			DrawHorzGradient(pDC, rcIcon, crBegin, crEnd);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - mIconY) / 2;
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
			CRect rc2(rc.left+2, rc.top+2+dy, rc.left+mIconX+1, rc.top+mIconY+2+dy);
			
			MenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + mIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

            if (pMenuData->mAccelText != XPR_NULL && pMenuData->mAccelText[0] != _T('\0'))
			{
				strLeft  = strText;
				strRight = pMenuData->mAccelText;
				rcText.right -= mIconX;
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

void MenuEx::DrawItemStandardStyle(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);
	MenuData *pMenuData = GetMenuData(lpDIS->itemID);
    UINT state = lpDIS->itemState;

	COLORREF crBack = ::GetSysColor(COLOR_MENU);
	COLORREF crBegin  = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 220);
	COLORREF crEnd    = GetSysColor(COLOR_3DFACE);
	
	CRect rcR(rc);
	rcR.left += mIconX + 4;
	pDC->FillSolidRect(rcR, crBack);
	
	if (state & MF_SEPARATOR)
	{
		CRect rcIcon(rc);
		rcIcon.right = rcIcon.left + mIconX+4;
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
			nIconNormal	= pMenuData->mImage;
            pImgList	= pMenuData->mImageList;
            GetMenuText(lpDIS->itemID, strText, MF_BYCOMMAND);
			
			if (state & ODS_CHECKED && nIconNormal < 0)
			{
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
			rcIcon.right = rcIcon.left + mIconX + 4;
			pDC->FillSolidRect(rcIcon, crBack);
		}

		// Draw Icon
		int dy = (rc.Height() - 4 - mIconY) / 2;
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
			CRect rc2(rc.left+4, rc.top+2+dy, rc.left+mIconX+1, rc.top+mIconY+2+dy);
			
			MenuItemInfo info;
			info.fMask = MIIM_CHECKMARKS;
			::GetMenuItemInfo((HMENU)lpDIS->hwndItem, lpDIS->itemID, MF_BYCOMMAND, &info);

			if (state & ODS_CHECKED || info.hbmpUnchecked)
				Draw3DCheckmark(pDC, rc2, state & ODS_SELECTED, state & ODS_CHECKED ? info.hbmpChecked : info.hbmpUnchecked);
		}

		// Draw Text
		if (!strText.IsEmpty())
		{
			CRect rcText(rc.left + mIconX + 8 + GAP, rc.top+1, rc.right, rc.bottom);

			NONCLIENTMETRICS ncm = {0};
			ncm.cbSize = sizeof(NONCLIENTMETRICS);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); 

			CFont ftMenu;
			ftMenu.CreateFontIndirect(&ncm.lfMenuFont);
			CFont *pOldFont = pDC->SelectObject(&ftMenu);

			// Find Tab(\t)
			CString strLeft, strRight;
			strLeft.Empty(); strRight.Empty();

			if (pMenuData->mAccelText != XPR_NULL && pMenuData->mAccelText[0] != _T('\0'))
			{
				strLeft  = strText;
				strRight = pMenuData->mAccelText;
				rcText.right -= mIconX;
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

void MenuEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	if (g_lpSet->adv_bStandardMenu)
		DrawItemStandardStyle(lpDIS);
	else
		DrawItemOffice2007Style(lpDIS);
}

void MenuEx::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    MenuData *pMenuData = GetMenuData(lpMIS->itemID);
    UINT uState = GetMenuState(lpMIS->itemID, MF_BYCOMMAND);

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
        CString strText;
        GetMenuText(lpMIS->itemID, strText, MF_BYCOMMAND);

		xpr_tchar_t szMenuDispText[0xff] = {0};
        _tcscpy(szMenuDispText, strText.GetBuffer());
		if (pMenuData->mAccelText != XPR_NULL && pMenuData->mAccelText[0] != _T('\0'))
		{
			_stprintf(szMenuDispText + _tcslen(szMenuDispText), _T("\t%s"), pMenuData->mAccelText);
		}
		::GetTextExtentPoint32W(pDC->m_hDC, szMenuDispText, (int)_tcslen(szMenuDispText), &sz);

		pDC->SelectObject (pOldFont);
		pWnd->ReleaseDC(pDC);
		ftMenu.DeleteObject();

		lpMIS->itemWidth = mIconX + sz.cx + mIconX + GAP;
		lpMIS->itemWidth = min(lpMIS->itemWidth, MAX_MENU_WIDTH);

		lpMIS->itemHeight = max(GetSystemMetrics(SM_CYMENU), (mIconY+4));
	}
}

BOOL MenuEx::AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, LPCTSTR lpszNewItem, CImageList *aImageList, xpr_sint_t aImage)
{
    nFlags |= MF_OWNERDRAW;

	if (XPR_TEST_BITS(nFlags, MF_POPUP))
	{
		mAllSubMenus.insert((HMENU)nIDNewItem);
		mSubMenus.insert((HMENU)nIDNewItem);
	}

    // popup menu
    // separator
    // general menu

	MenuData *sMenuData = new MenuData;
	mMenuDatas[.Add(mdata);
	mdata->SetWideString(lpstrText);    //SK: modified for dynamic allocation

	mdata->menuIconNormal = nIconNormal;
	mdata->xoffset = -1;
	if (nIconNormal >= 0)
	{
		mdata->xoffset = 0;
		LoadFromToolBar((UINT)nIDNewItem, nIconNormal, mdata->xoffset);
		if (mdata->bitmap) mdata->bitmap->DeleteImageList();
		else mdata->bitmap = new(CImageList);
		mdata->bitmap->Create(mIconX, mIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
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

BOOL MenuEx::AppendODMenuA(LPCSTR lpstrText,UINT nFlags,UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	USES_CONVERSION;
	return AppendODMenuW(A2W(lpstrText),nFlags,nIDNewItem,il,xoffset);
}

BOOL MenuEx::AppendODMenuW(wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
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
	
	MenuExData *mdata = new MenuExData;
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

BOOL MenuEx::InsertODMenuA(UINT nPosition, LPCSTR lpstrText, UINT nFlags, UINT_PTR nIDNewItem, int nIconNormal)
{
	USES_CONVERSION;
	return InsertODMenuW(nPosition,A2W(lpstrText),nFlags,nIDNewItem,nIconNormal);
}


BOOL MenuEx::InsertODMenuW(UINT nPosition, wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, int nIconNormal)
{
	if (!(nFlags & MF_BYPOSITION))
	{
		int iPos =0;
		MenuEx* pMenu = FindMenuOption(nPosition, iPos);
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
	
	MenuExData *mdata = new MenuExData;
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
		mdata->bitmap->Create(mIconX, mIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
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

BOOL MenuEx::InsertODMenuA(UINT nPos, LPCSTR lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	USES_CONVERSION;
	return InsertODMenuW(nPos, A2W(lpstrText), nFlags, nIDNewItem, il, xoffset);
}

BOOL MenuEx::InsertODMenuW(UINT nPosition, wchar_t *lpstrText, UINT nFlags, UINT_PTR nIDNewItem, CImageList *il, int xoffset)
{
	if (!(nFlags & MF_BYPOSITION))
	{
		int iPos = 0;
		MenuEx* pMenu = FindMenuOption(nPosition, iPos);
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
	
	MenuExData *mdata = new MenuExData;
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

BOOL MenuEx::ModifyODMenuA(const char * lpstrText, UINT_PTR nIDNewItem, int nIconNormal)
{
	USES_CONVERSION;
	return ModifyODMenuW(A2W(lpstrText),nIDNewItem,nIconNormal);//SK: see MFC Tech Note 059
}

BOOL MenuEx::ModifyODMenuW(wchar_t *lpstrText, UINT_PTR nIDNewItem, int nIconNormal)
{
	int nLoc;
	MenuExData *mdata;
	
	// Find the old MenuExData structure:
	MenuEx *psubmenu = FindMenuOption((int)nIDNewItem,nLoc);
	if (psubmenu && nLoc>=0)
		mdata = psubmenu->m_MenuList[nLoc];
	else
	{
		// Create a new MenuExData structure:
		mdata = new MenuExData;
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

		mdata->bitmap->Create(mIconX, mIconY, ILC_COLORDDB | ILC_MASK, 1, 1);
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

BOOL MenuEx::ModifyODMenuA(const char * lpstrText,UINT_PTR nIDNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),nIDNewItem,il,xoffset);
}

BOOL MenuEx::ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nIDNewItem,CImageList *il,int xoffset)
{
  int nLoc;
  MenuExData *mdata;

  // Find the old MenuExData structure:
  MenuEx *psubmenu = FindMenuOption((int)nIDNewItem,nLoc);
  if(psubmenu && nLoc>=0)mdata = psubmenu->m_MenuList[nLoc];
  else{
  // Create a new MenuExData structure:
    mdata = new MenuExData;
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

BOOL MenuEx::ModifyODMenuA(const char * lpstrText,UINT_PTR nIDNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),nIDNewItem,bmp);
}

BOOL MenuEx::ModifyODMenuW(wchar_t *lpstrText,UINT_PTR nIDNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(mIconX,mIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return ModifyODMenuW(lpstrText,nIDNewItem,&temp,0);
  }
  return ModifyODMenuW(lpstrText,nIDNewItem,NULL,0);
}


BOOL MenuEx::ModifyODMenuA(const char *lpstrText,const char *OptionText,
                           int nIconNormal)
{
USES_CONVERSION;
return ModifyODMenuW(A2W(lpstrText),A2W(OptionText),nIconNormal);//SK: see MFC  Tech Note 059
}

BOOL MenuEx::ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,
                           int nIconNormal)
{
  MenuExData *mdata;

  // Find the old MenuExData structure:
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
      mdata->bitmap->Create(mIconX,mIconY,ILC_COLORDDB|ILC_MASK,1,1);
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

MenuExData *MenuEx::NewODMenu(UINT pos,UINT nFlags,UINT_PTR nIDNewItem,CString string)
{
  MenuExData *mdata;

  mdata = new MenuExData;
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

BOOL MenuEx::LoadToolbars(const UINT *arID,int n)
{
  ASSERT(arID);
  BOOL returnflag=TRUE;
  for(int i=0;i<n;++i){
    if(!LoadToolbar(arID[i]))returnflag=FALSE;
  }
  return(returnflag);
}

BOOL MenuEx::LoadToolbar(UINT nToolBar)
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

//BOOL MenuEx::LoadToolbar2(UINT nBitmapID, COLORREF clrMask, TBBUTTONEX *ptbe, int nCount)
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

BOOL MenuEx::LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset)
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
MenuExData *MenuEx::FindMenuItem(UINT nID)
{
	MenuExData *pData = NULL;

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
		MenuEx *pMenu = FindMenuOption(nID, loc);
		ASSERT(pMenu != this);
		if (loc >= 0)
			return pMenu->FindMenuItem(nID);
	}
	return pData;
}

MenuEx *MenuEx::FindMenuOption(int nId, int& nLoc)
{
	MenuEx *psubmenu,*pgoodmenu;

	int nCount = GetMenuItemCount();
	for (int i=0; i<nCount; ++i){
#ifdef _CPPRTTI 
		psubmenu = dynamic_cast<MenuEx *>(GetSubMenu(i));
#else
		psubmenu=(MenuEx *)GetSubMenu(i);
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

MenuExData *MenuEx::FindMenuOption(wchar_t *lpstrText)
{
	int i,j;
	MenuEx *psubmenu;
	MenuExData *pmenulist;

	int nCount = GetMenuItemCount();
	for (i=0; i<nCount; ++i)
	{
#ifdef _CPPRTTI 
		psubmenu = dynamic_cast<MenuEx *>(GetSubMenu(i));
#else
		psubmenu = (MenuEx *)GetSubMenu(i);
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


BOOL MenuEx::LoadMenu(int nResource)
{
	return MenuEx::LoadMenu(MAKEINTRESOURCE(nResource));
}

BOOL MenuEx::LoadMenu(LPCTSTR lpszResourceName)
{
	TRACE(_T("IMPORTANT:Use MenuEx::DestroyMenu to destroy Loaded Menu's\n"));
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
	
	CTypedPtrArray<CPtrArray, MenuEx*> m_Stack;    // Popup menu stack
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

			MenuEx* pSubMenu = new MenuEx;
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

void MenuEx::InsertSpaces(void)
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

void MenuEx::LoadCheckmarkBitmap(int unselect, int select)
{
	if(unselect > 0 && select > 0)
	{
		m_selectcheck = select;
		m_unselectcheck = unselect;
		
		if (checkmaps)	checkmaps->DeleteImageList();
		else			checkmaps = new(CImageList);
		checkmaps->Create(mIconX,mIconY,ILC_MASK,2,1);

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
BOOL MenuEx::GetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
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
		MenuEx* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->GetMenuText(uiLoc, string);
	}

	return bResult;
}

BOOL MenuEx::GetMenuText(UINT id, LPTSTR lpszString, UINT nFlags/*= MF_BYPOSITION*/)
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
		MenuEx* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->GetMenuText(uiLoc, lpszString);
	}

	return bResult;
}

BOOL MenuEx::SetMenuText(UINT id, CString& string, UINT nFlags/*= MF_BYPOSITION*/)
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
		MenuEx* pMenu = FindMenuOption(id,uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuText(uiLoc, string);
	}

	return bResult;
}

BOOL MenuEx::SetMenuText(UINT id, LPTSTR lpszString, UINT nFlags/* = MF_BYPOSITION*/)
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
		MenuEx* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuText(uiLoc, lpszString);
	}

	return bResult;
}

BOOL MenuEx::SetMenuAccelKey(UINT id, LPCTSTR lpcszAccelKey, UINT nFlags/* = MF_BYPOSITION*/)
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
		MenuEx* pMenu = FindMenuOption(id, uiLoc);
		if (NULL != pMenu)
			bResult = pMenu->SetMenuAccelKey(uiLoc, lpcszAccelKey);
	}

	return bResult;
}

void MenuEx::DrawRadioDot(CDC *pDC, int x, int y, COLORREF color)
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

void MenuEx::DrawCheckMark(CDC* pDC, int x, int y, COLORREF color)
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

MenuExData *MenuEx::FindMenuList(UINT_PTR nIDNewItem)
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

void MenuEx::InitializeMenuList(int value)
{
	INT_PTR nCount = m_MenuList.GetUpperBound();
	for(INT_PTR i=0; i<=nCount; ++i)
		m_MenuList[i]->syncflag=value;
}

void MenuEx::DeleteMenuList(void)
{
	for (int i=0; i<=m_MenuList.GetUpperBound(); ++i)
	{
		if (!m_MenuList[i]->syncflag)
			delete m_MenuList[i];
	}
}

void MenuEx::SynchronizeMenu(void)
{
	CTypedPtrArray<CPtrArray, MenuExData*> temp;
	MenuExData *mdata;
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

void MenuEx::UpdateMenu(CMenu *pmenu)
{
#ifdef _CPPRTTI 
	MenuEx *psubmenu = dynamic_cast<MenuEx *>(pmenu);
#else
	MenuEx *psubmenu = (MenuEx *)pmenu;
#endif
	
	if (psubmenu)
		psubmenu->SynchronizeMenu();
}

BOOL MenuEx::AddBitmapToImageList(CImageList *bmplist, UINT nResourceID)
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

void MenuEx::SetBitmapBackground(COLORREF color)
{
  m_bitmapBackground=color;
  m_bitmapBackgroundFlag=TRUE;
}

void MenuEx::UnSetBitmapBackground(void)
{
  m_bitmapBackgroundFlag=FALSE;
}

// Given a toolbar, append all the options from it to this menu
// Passed a ptr to the toolbar object and the toolbar ID
// Author : Robert Edward Caldecott
void MenuEx::AddFromToolBar(CToolBar* pToolBar, int nResourceID)
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
    MenuExData* pData;
    MenuEx *pSubMenu = FindMenuOption(nID, nLoc);
    if (pSubMenu && nLoc >= 0)pData = pSubMenu->m_MenuList[nLoc];
    else {
      // Create a new MenuExData structure
      pData = new MenuExData;
      m_MenuList.Add(pData);
    }
    // Set some default structure members
    pData->menuIconNormal = nResourceID;
    pData->nID = nID;
    pData->nFlags =  MF_BYCOMMAND | MF_OWNERDRAW;
    pData->xoffset = nImage;
    if (pData->bitmap)pData->bitmap->DeleteImageList();
    else pData->bitmap = new CImageList;
    pData->bitmap->Create(mIconX, mIconY,ILC_COLORDDB|ILC_MASK, 1, 1);

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

BOOL MenuEx::Draw3DCheckmark(CDC *dc, const CRect& rc, BOOL bSelected, HBITMAP hbmCheck)
{
	if (!hbmCheck)	DrawCheckMark(dc,rc.left+4,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	else			DrawRadioDot(dc,rc.left+5,rc.top+4,GetSysColor(COLOR_MENUTEXT));
	return TRUE;
}

HBITMAP MenuEx::LoadSysColorBitmap(int nResourceId)
{
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nResourceId), RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nResourceId), RT_BITMAP);
	if (hRsrc == NULL)
		return NULL;

	return AfxLoadSysColorBitmap(hInst, hRsrc, FALSE);
}

BOOL MenuEx::RemoveMenu(UINT uiId,UINT nFlags)
{
	if(MF_BYPOSITION&nFlags){
    UINT uint = GetMenuState(uiId,MF_BYPOSITION);
    if(uint&MF_SEPARATOR && !(uint&MF_POPUP)){
		  delete m_MenuList.GetAt(uiId);
  	  m_MenuList.RemoveAt(uiId);
    }
    else{
  		MenuEx* pSubMenu = (MenuEx*) GetSubMenu(uiId);
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
		MenuEx* pMenu = FindMenuOption(uiId,iPosition);
		if(pMenu)pMenu->RemoveMenu(iPosition,MF_BYPOSITION);
	}
	return CMenu::RemoveMenu(uiId,nFlags);
}

BOOL MenuEx::DeleteMenu(UINT uiId,UINT nFlags)
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
			MenuEx *pSubMenu = (MenuEx *)GetSubMenu(uiId);
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
		MenuEx* pMenu = FindMenuOption(uiId, iPosition);
		if (pMenu)
			pMenu->DeleteMenu(iPosition, MF_BYPOSITION);
	}
	return CMenu::DeleteMenu(uiId,nFlags);
}


BOOL MenuEx::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL MenuEx::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL MenuEx::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL MenuEx::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL MenuEx::AppendMenuA(UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return AppendMenuW(nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL MenuEx::AppendMenuW(UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(mIconX,mIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,&temp,0);
  }
  return AppendODMenuW(lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

BOOL MenuEx::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,int nIconNormal)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),nIconNormal);
}

BOOL MenuEx::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,int nIconNormal)
{
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,nIconNormal);
}

BOOL MenuEx::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CImageList *il,int xoffset)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),il,xoffset);
}

BOOL MenuEx::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CImageList *il,int xoffset)
{
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,il,xoffset);
}

BOOL MenuEx::InsertMenuA(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,const char *lpszNewItem,CBitmap *bmp)
{
USES_CONVERSION;
return InsertMenuW(nPosition,nFlags,nIDNewItem,A2W(lpszNewItem),bmp);
}

BOOL MenuEx::InsertMenuW(UINT nPosition,UINT nFlags,UINT_PTR nIDNewItem,wchar_t *lpszNewItem,CBitmap *bmp)
{
  if(bmp){
  	CImageList temp;
    temp.Create(mIconX,mIconY,ILC_COLORDDB|ILC_MASK,1,1);
    if(m_bitmapBackgroundFlag)temp.Add(bmp,m_bitmapBackground);
    else temp.Add(bmp,GetSysColor(COLOR_3DFACE));
    return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,&temp,0);
  }
  return InsertODMenuW(nPosition,lpszNewItem,nFlags,nIDNewItem,NULL,0);
}

//--------------------------------------------------------------------------
//[21.06.99 rj]
MenuEx* MenuEx::AppendODPopupMenuW(wchar_t *lpstrText)
{
	MenuEx* pSubMenu = new MenuEx;
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
MenuEx* MenuEx::AppendODPopupMenuA(LPCSTR lpstrText)
{
	USES_CONVERSION;
	return AppendODPopupMenuW(A2W(lpstrText));
}

BOOL MenuEx::ImageListDuplicate(CImageList *il,int xoffset,CImageList *newlist)
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


BOOL MenuEx::TrackPopupMenu(UINT nFlags, CPoint pt, CWnd* pWnd, LPCRECT lpRect /*= 0*/)
{
	return TrackPopupMenu(nFlags, pt.x, pt.y, pWnd, lpRect);
}

BOOL MenuEx::TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect /*= 0*/)
{
	if (!g_lpSet->adv_bAnimationMenu)
		nFlags |= 0x4000L;

	return CMenu::TrackPopupMenu(nFlags, x, y, pWnd, lpRect);
}
} // namespace rgc
