// bcgtabwnd.cpp : implementation file
//

#include "stdafx.h"
#include "bcgtabwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT BCGM_CHANGE_ACTIVE_TAB = ::RegisterWindowMessage (_T("BCGM_ONCHANGE_ACTIVE_TAB"));
UINT BCGM_ON_HSCROLL		= ::RegisterWindowMessage (_T("BCGM_ON_HSCROLL"));

/////////////////////////////////////////////////////////////////////////////
// CBCGTabInfo

class CBCGTabInfo : public CObject
{
	friend class CBCGTabWnd;

	CBCGTabInfo(const CString& strText, const UINT uiIcon, CWnd *pWnd)
		: m_pWnd (pWnd), m_uiIcon (uiIcon)
	{
		m_strText = m_strDisplayedText = strText;
		m_rect.SetRectEmpty ();
		m_nFullWidth = 0;
	}

	CString		m_strText;
	CString		m_strDisplayedText;
	const UINT	m_uiIcon;
	CRect		m_rect;
	CWnd*		m_pWnd;
	int			m_nFullWidth;
};

/////////////////////////////////////////////////////////////////////////////
// CBCGTabWnd

#define TAB_BORDER_SIZE		0
#define TEXT_MARGIN			4
#define IMAGE_MARGIN		4
#define MIN_SROLL_WIDTH		(::GetSystemMetrics (SM_CXHSCROLL) * 2)
#define SPLITTER_WIDTH		5
#define TABS_FONT			_T("Arial")

CBCGTabWnd::CBCGTabWnd()
{
	m_iTabsNum = 0;
	m_iActiveTab = -1;
	m_sizeImage = CSize (0, 0);
	m_bFlat = FALSE;
	m_bSharedScroll = FALSE;
	m_rectTabsArea.SetRectEmpty ();
	m_rectWndArea.SetRectEmpty ();
	m_nTabsHorzOffset = 0;
	m_nTabsHorzOffsetMax = 0;
	m_nTabsTotalWidth = 0;
	m_nHorzScrollWidth = 0;
	m_nScrollBarRight = 0;
	m_rectTabSplitter.SetRectEmpty ();
	m_bTrackSplitter = FALSE;

	m_clrActiveTabBk = (COLORREF) -1;
	m_clrActiveTabFg = (COLORREF) -1;
	
	m_nTabsHeight = 0;
}
//***************************************************************************************
CBCGTabWnd::~CBCGTabWnd()
{
}

BEGIN_MESSAGE_MAP(CBCGTabWnd, CWnd)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_HSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

BOOL CBCGTabWnd::Create (Style style, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_bFlat = (style == STYLE_FLAT) || (style == STYLE_FLAT_SHARED_HORZ_SCROLL);
	m_bSharedScroll = style == STYLE_FLAT_SHARED_HORZ_SCROLL;

	if (!m_bFlat && m_bSharedScroll)
	{
		//--------------------------------------
		// Only flat tab has a shared scrollbar!
		//--------------------------------------
		ASSERT (FALSE);
		m_bSharedScroll = FALSE;
	}

	return CWnd::Create (NULL, _T(""), WS_CHILD | WS_VISIBLE, rect,
		pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CBCGTabWnd message handlers

void CBCGTabWnd::OnDestroy() 
{
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_pWnd->DestroyWindow ();
		delete pTab;
	}

	CWnd::OnDestroy();
}
//***************************************************************************************
void CBCGTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectClient;
	GetClientRect (&rectClient);

	CBrush brBtnFace;
	brBtnFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	CBrush* pOldBrush = dc.SelectObject (&brBtnFace);
	ASSERT (pOldBrush != NULL);

	CPen penDrak (PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
	CPen* pOldPen = (CPen*) dc.SelectObject (&penDrak);
	ASSERT(pOldPen != NULL);

	CRect rectTabs = rectClient;
	rectTabs.top = m_rectTabsArea.top - TAB_BORDER_SIZE;
	dc.FillRect (rectTabs, &brBtnFace);

	CRect rectFrame = rectClient;

	if (m_bFlat)
	{
		//---------------------------
		// Draw line bellow the tabs:
		//---------------------------
		dc.MoveTo (rectFrame.left, m_rectTabsArea.top);
		dc.LineTo (rectFrame.right, m_rectTabsArea.top);
	}
	else
	{
		rectFrame.bottom = m_rectTabsArea.top;
	}

	//-----------------------------------------------------
	// Draw wide 3-dimensional frame around the Tabs area:
	//-----------------------------------------------------
//	dc.Draw3dRect (&rectFrame, clrBtnHilite, clrBtnDkShadow);
	
	rectFrame.DeflateRect (1, 1);
	CRect rc2(rectFrame);
	rc2.bottom += 2;
	dc.Draw3dRect (&rc2, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_BTNSHADOW));

	rectFrame.DeflateRect (1, 1);

	if (rectFrame.Width () > 0 && rectFrame.Height () > 0)
	{
		dc.PatBlt (rectFrame.left, rectFrame.top, TAB_BORDER_SIZE, rectFrame.Height (), PATCOPY);
		dc.PatBlt (rectFrame.left, rectFrame.top, rectFrame.Width (), TAB_BORDER_SIZE, PATCOPY);
		dc.PatBlt (rectFrame.right - TAB_BORDER_SIZE, rectFrame.top, TAB_BORDER_SIZE, rectFrame.Height (), PATCOPY);
		dc.PatBlt (rectFrame.left, rectFrame.bottom - TAB_BORDER_SIZE, rectFrame.Width (), TAB_BORDER_SIZE, PATCOPY);

		rectFrame.DeflateRect (TAB_BORDER_SIZE - 2, TAB_BORDER_SIZE - 2);

		if (rectFrame.Width () > 0 && rectFrame.Height () > 0)
		{
			CRect rc2(rectFrame);
			rc2.bottom += 10;
			dc.Draw3dRect (&rc2, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
		}
	}

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);
	::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
   
	CFont ftRegular;
	ftRegular.CreateFontIndirect(&info.lfMenuFont);

	CFont* pOldFont = dc.SelectObject (m_bFlat ?	&m_fntTabs : 
													&ftRegular);
	ASSERT(pOldFont != NULL);

	dc.SetBkMode (TRANSPARENT);
	dc.SetTextColor (::GetSysColor(COLOR_BTNTEXT));

	if (m_rectTabsArea.Width () > 2 * TAB_BORDER_SIZE + 1 &&
		m_rectTabsArea.Height () > 2 * TAB_BORDER_SIZE + 1)
	{
		//-----------
		// Draw tabs:
		//-----------
		if (m_bFlat)
		{
			CRgn rgn;
			rgn.CreateRectRgnIndirect (m_rectTabsArea);

			dc.SelectClipRgn (&rgn);
		}

		for (int i = 0; i < m_iTabsNum; i ++)
		{
			CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
			ASSERT_VALID (pTab);

			if (m_bFlat)
			{
				if (i != m_iActiveTab)	// Draw active button last
				{
					DrawFlatTab (&dc, pTab, FALSE);
				}
			}
			else
			{
				Draw3DTab (&dc, pTab, i == m_iActiveTab);
			}
		}

		if (m_bFlat && m_iActiveTab >= 0)
		{
			//-----------------
			// Draw active tab:
			//-----------------
			CBCGTabInfo* pTabActive = (CBCGTabInfo*) m_arTabs [m_iActiveTab];
			ASSERT_VALID (pTabActive);

			dc.SelectObject (&m_brActiveTab);
			dc.SelectObject (&m_fntTabsBold);
			dc.SetTextColor (GetActiveTabTextColor ());

			DrawFlatTab (&dc, pTabActive, TRUE);

			//---------------------------------
			// Draw line bellow the active tab:
			//---------------------------------
			CPen penLight (PS_SOLID, 1, GetActiveTabColor ());
			dc.SelectObject (&penLight);

			dc.MoveTo (pTabActive->m_rect.left + 1, pTabActive->m_rect.top);
			dc.LineTo (pTabActive->m_rect.right, pTabActive->m_rect.top);
		}

		if (m_bFlat)
		{
			dc.SelectClipRgn (NULL);
		}
	}

	if (!m_rectTabSplitter.IsRectEmpty ())
	{
		dc.FillRect (m_rectTabSplitter, &brBtnFace);

		CRect rectTabSplitter = m_rectTabSplitter;

		dc.Draw3dRect (rectTabSplitter, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_BTNSHADOW));
		rectTabSplitter.DeflateRect (1, 1);
		dc.Draw3dRect (rectTabSplitter, ::GetSysColor(COLOR_BTNHIGHLIGHT), ::GetSysColor(COLOR_BTNSHADOW));
	}
	
	dc.SelectObject (pOldFont);
	dc.SelectObject (pOldBrush);
	dc.SelectObject (pOldPen);
}
//***************************************************************************************
void CBCGTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	int nTabsAreaWidth = cx - 4 * ::GetSystemMetrics (SM_CXVSCROLL) 
							- 2 * TAB_BORDER_SIZE;

	if (nTabsAreaWidth <= MIN_SROLL_WIDTH)
	{
		m_nHorzScrollWidth = 0;
	}
	else if (nTabsAreaWidth / 2 > MIN_SROLL_WIDTH)
	{
		m_nHorzScrollWidth = nTabsAreaWidth / 2;
	}
	else
	{
		m_nHorzScrollWidth = nTabsAreaWidth; 
	}

	RecalcLayout ();
	SynchronizeScrollBar ();
}
//***************************************************************************************
void CBCGTabWnd::AddTab (CWnd* pNewWnd, LPCTSTR lpszName, UINT uiImageId)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pNewWnd);
	ASSERT (pNewWnd->GetParent()->GetSafeHwnd () == GetSafeHwnd ());
	ASSERT (lpszName != NULL);

	if (m_bFlat)
	{
		ASSERT (uiImageId == -1);
		uiImageId = (UINT) -1;
	}
	else if (m_sizeImage != CSize (0, 0))
	{
		ASSERT (uiImageId != (UINT) -1);
	}

	m_arTabs.SetAtGrow (m_iTabsNum ++, 
		new CBCGTabInfo (lpszName, uiImageId, pNewWnd));

	if (!m_bFlat)
	{
		CRect rectEmpty (0, 0, 0, 0);
		m_ToolTip.AddTool (this, lpszName, &rectEmpty, m_iTabsNum);
	}

	RecalcLayout ();

	if (m_iTabsNum == 1)
	{
		//----------------------------------------
		// First tab automatically becames active:
		//----------------------------------------
		SetActiveTab (0);
	}

	if (GetActiveWnd () != pNewWnd)
	{
		pNewWnd->ShowWindow (SW_HIDE);
	}
}
//***************************************************************************************
void CBCGTabWnd::AddTab (CWnd* pTabWnd, UINT uiResTabLabel, UINT uiImageId)
{
	CString strLabel;
	strLabel.LoadString (uiResTabLabel);

	AddTab (pTabWnd, strLabel, uiImageId);
}
//***************************************************************************************
BOOL CBCGTabWnd::RemoveTab (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("RemoveTab: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	if (m_iTabsNum == 1)
	{
		RemoveAllTabs ();
		return TRUE;
	}

	CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [iTab];
	ASSERT_VALID (pTab);

	//----------------------------
	// Detach tab from collection:
	//----------------------------
	m_arTabs.RemoveAt (iTab);
	m_iTabsNum --;

	//-----------------------------------
	// Destroy tab window and delete tab:
	//-----------------------------------
	ASSERT_VALID (pTab->m_pWnd);
	pTab->m_pWnd->DestroyWindow ();
	delete pTab;

	int iActiveTab = -1;
	if (m_iActiveTab >= iTab)
	{
		iActiveTab = max (0, min (m_iTabsNum, m_iActiveTab - 1));
		m_iActiveTab = -1;
	}

	RecalcLayout ();
	SetActiveTab (iActiveTab);
	GetParent ()->SendMessage (BCGM_CHANGE_ACTIVE_TAB, m_iActiveTab);

	return TRUE;
}
//***************************************************************************************
void CBCGTabWnd::RemoveAllTabs ()
{
	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_pWnd->DestroyWindow ();
		delete pTab;
	}

	m_arTabs.RemoveAll ();

	m_iTabsNum = 0;
	m_iActiveTab = -1;

	RecalcLayout ();
}
//***************************************************************************************
BOOL CBCGTabWnd::SetActiveTab (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("SetActiveTab: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	if (m_iActiveTab == iTab)	// Already active, do nothing
	{
		return TRUE;
	}

	if (m_iActiveTab != -1)
	{
		//--------------------
		// Hide active window:
		//--------------------
		GetActiveWnd()->ShowWindow (SW_HIDE);
	}

	m_iActiveTab = iTab;
	
	//------------------------
	// Show new active window:
	//------------------------
	HideActiveWindowHorzScrollBar ();
	GetActiveWnd ()->ShowWindow (SW_SHOW);

	//----------------------------------------------------------------------
	// Small trick: to adjust active window scroll sizes, I should change an
	// active window size twice (+1 pixel and -1 pixel):
	//----------------------------------------------------------------------
	GetActiveWnd ()->SetWindowPos (NULL,
			-1, -1,
			m_rectWndArea.Width (), m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	GetActiveWnd ()->SetWindowPos (NULL,
			-1, -1,
			m_rectWndArea.Width () - 1, m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	EnsureVisible (m_iActiveTab);

	if (m_bFlat)
	{
		SynchronizeScrollBar ();
	}

	//-------------
	// Redraw tabs:
	//-------------
	Invalidate ();
	UpdateWindow ();

	GetActiveWnd ()->SetFocus ();
	return TRUE;
}
//***************************************************************************************
void CBCGTabWnd::AdjustTabs ()
{
	m_nTabsTotalWidth = 0;

	if (m_iTabsNum == 0)
	{
		return;
	}

	//-------------------------
	// Define tab's full width:
	//-------------------------
	CClientDC dc (this);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);
	::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
   
	CFont ftRegular;
	ftRegular.CreateFontIndirect(&info.lfMenuFont);

	CFont* pOldFont = dc.SelectObject (m_bFlat ? 
		&m_fntTabsBold : &ftRegular);
	ASSERT(pOldFont != NULL);

	CRect rectClient;
	GetClientRect (&rectClient);

	//----------------------------------------------
	// First, try set all tabs in its original size:
	//----------------------------------------------
	int i;
	int x = m_rectTabsArea.left - m_nTabsHorzOffset;
	for (i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_nFullWidth = m_sizeImage.cx + IMAGE_MARGIN +
				dc.GetTextExtent (pTab->m_strText).cx + 2 * TEXT_MARGIN;

		pTab->m_rect = CRect (CPoint (x, m_rectTabsArea.top),
						CSize (pTab->m_nFullWidth, m_rectTabsArea.Height () - 2));

		if (!m_bFlat)
		{
			m_ToolTip.SetToolRect (this, i + 1, CRect (0, 0, 0, 0));
		}

		x += pTab->m_rect.Width () + 1;
		m_nTabsTotalWidth += pTab->m_rect.Width () + 1;

		if (m_bFlat)
		{
			//--------------------------------------------
			// In the flat mode tab is overlapped by next:
			//--------------------------------------------
			pTab->m_rect.right += m_nTabsHeight / 2;
		}

		pTab->m_strDisplayedText = pTab->m_strText;
	}

	if (m_bFlat || x < m_rectTabsArea.right)
	{
		m_nTabsTotalWidth += m_nTabsHeight / 2;
		dc.SelectObject(pOldFont);
		return;
	}

	//-----------------------------------------
	// Not enouth space to show the whole text.
	//-----------------------------------------
	int nTabWidth = m_rectTabsArea.Width () / m_iTabsNum;

	//------------------------------------
	// May be it's too wide for some tabs?
	//------------------------------------
	int nRest = 0;
	int nCuttedTabsNum = m_iTabsNum;

	for (i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_nFullWidth < nTabWidth)
		{
			nRest += nTabWidth - pTab->m_nFullWidth;
			nCuttedTabsNum --;
		}
	}

	if (nCuttedTabsNum <= 0)
	{
		ASSERT (FALSE);	// Somethin wrong
		dc.SelectObject(pOldFont);
		return;
	}

	nTabWidth += nRest / nCuttedTabsNum;

	//----------------------------------
	// Last pass: set actual rectangles:
	//----------------------------------
	x = m_rectTabsArea.left;
	for (i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		BOOL bIsTrucncated = pTab->m_nFullWidth > nTabWidth;
		int nCurrTabWidth = (bIsTrucncated) ? nTabWidth : pTab->m_nFullWidth;

		if (bIsTrucncated)
		{
			TruncateText (&dc, pTab->m_strDisplayedText, 
				nCurrTabWidth - TEXT_MARGIN - IMAGE_MARGIN - m_sizeImage.cx);
		}

		if (nTabWidth < m_sizeImage.cx + IMAGE_MARGIN)
		{
			// Too narrow!
			nCurrTabWidth = (m_rectTabsArea.Width () + TAB_BORDER_SIZE * 2) / m_iTabsNum;
			pTab->m_strDisplayedText.Empty ();
		}
		else
		{
			if (pTab->m_strDisplayedText.IsEmpty ())
			{
				nCurrTabWidth = m_sizeImage.cx + 2 * TEXT_MARGIN;
			}
		}

		pTab->m_rect = CRect (CPoint (x, m_rectTabsArea.top),
						CSize (nCurrTabWidth, m_rectTabsArea.Height ()));

		if (!m_bFlat)
		{
			pTab->m_rect.bottom -= 2;
			m_ToolTip.SetToolRect (this, i + 1, 
				bIsTrucncated ? pTab->m_rect : CRect (0, 0, 0, 0));
		}

		x += nCurrTabWidth + 1;
	}

	dc.SelectObject(pOldFont);
}
//***************************************************************************************
void CBCGTabWnd::Draw3DTab (CDC* pDC, CBCGTabInfo* pTab, BOOL bActive)
{
	ASSERT_VALID (pTab);
	ASSERT_VALID (pDC);

	const int iVertOffset = 2;
	const int iHorzOffset = 2;

	CRect rectTab = pTab->m_rect;
	
	if (!bActive)
	{
		rectTab.bottom -= iVertOffset;
	}

	CPen penLight (PS_SOLID, 1, ::GetSysColor (COLOR_3DHILIGHT));

	CPen* pOldPen = (CPen*) pDC->SelectObject (&penLight);
	ASSERT(pOldPen != NULL);

	pDC->MoveTo(rectTab.left, rectTab.top);
	pDC->LineTo(rectTab.left, rectTab.bottom - iVertOffset);

	pDC->SelectStockObject(BLACK_PEN);

	pDC->LineTo(rectTab.left + iHorzOffset, rectTab.bottom);
	pDC->LineTo(rectTab.right - iHorzOffset, rectTab.bottom);
	pDC->LineTo(rectTab.right, rectTab.bottom - iVertOffset);
	pDC->LineTo(rectTab.right, rectTab.top - 1);

	CPen penDkGray (PS_SOLID, 1, ::GetSysColor (COLOR_3DSHADOW));
	pDC->SelectObject(&penDkGray);

	pDC->MoveTo(rectTab.left + iHorzOffset + 1, rectTab.bottom - 1);
	pDC->LineTo(rectTab.right - iHorzOffset, rectTab.bottom - 1);
	pDC->LineTo(rectTab.right - 1, rectTab.bottom - iVertOffset);
	pDC->LineTo(rectTab.right - 1, rectTab.top - 1);

	if (bActive)
	{
		CBrush brBtnFace;
		brBtnFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
		pDC->FillRect (CRect (CPoint (rectTab.left, rectTab.top - 3), 
							CSize (rectTab.Width (), 3)), &brBtnFace);
	}

	if (m_sizeImage.cx + IMAGE_MARGIN <= rectTab.Width ())
	{
		if (m_Images.GetSafeHandle () != NULL)
		{
			// Draw the tab's image:
			CRect rectImage = rectTab;

			rectImage.top += (rectTab.Height () - m_sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + m_sizeImage.cy;

			rectImage.left += IMAGE_MARGIN;
			rectImage.right = rectImage.left + m_sizeImage.cx;

			m_Images.Draw (pDC, pTab->m_uiIcon, rectImage.TopLeft (), ILD_TRANSPARENT);
		}

		// Finally, draw the tab's text
		CRect rcText = rectTab;
		rcText.left += m_sizeImage.cx + 2 * TEXT_MARGIN;

		pDC->DrawText (pTab->m_strDisplayedText, 
							rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}

	pDC->SelectObject (pOldPen);
}
//***************************************************************************************
void CBCGTabWnd::DrawFlatTab (CDC* pDC, CBCGTabInfo* pTab, BOOL /*bActive*/)
{
	ASSERT_VALID (pTab);
	ASSERT_VALID (pDC);

	CRect rectTab = pTab->m_rect;
	rectTab.bottom --;

	//----------------
	// Draw tab edges:
	//----------------
	#define POINTS_NUM	4
	POINT pts [POINTS_NUM];

	pts [0].x = rectTab.left;
	pts [0].y = rectTab.top;

	pts [1].x = rectTab.left + m_nTabsHeight / 2;
	pts [1].y = rectTab.bottom;

	pts [2].x = rectTab.right - m_nTabsHeight / 2;
	pts [2].y = rectTab.bottom;

	pts [3].x = rectTab.right;
	pts [3].y = rectTab.top;

	pDC->Polygon (pts, POINTS_NUM);

	//---------------
	// Draw tab text:
	//---------------
	pDC->DrawText (pTab->m_strDisplayedText, 
					rectTab, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}
//***************************************************************************************
void CBCGTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_rectTabSplitter.PtInRect (point))
	{
		m_bTrackSplitter = TRUE;
		SetCapture ();
		return;
	}

	CWnd* pWndParent = GetParent ();
	ASSERT_VALID (pWndParent);

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_rect.PtInRect (point))
		{
			if (i != m_iActiveTab && m_rectTabsArea.PtInRect (point))
			{
				SetActiveTab (i);
				pWndParent->SendMessage (BCGM_CHANGE_ACTIVE_TAB, i);
			}

			return;
		}
	}

	CWnd* pWndTarget = FindTargetWnd (point);
	if (pWndTarget == NULL)
	{
		return;
	}

	ASSERT_VALID (pWndTarget);

	MapWindowPoints (pWndTarget, &point, 1);
	pWndTarget->SendMessage (WM_LBUTTONDOWN, nFlags, 
							MAKELPARAM (point.x, point.y));
}
//***************************************************************************************
void CBCGTabWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDblClk(nFlags, point);

	CWnd* pWndTarget = FindTargetWnd (point);
	if (pWndTarget == NULL)
	{
		return;
	}

	ASSERT_VALID (pWndTarget);

	MapWindowPoints (pWndTarget, &point, 1);
	pWndTarget->SendMessage (WM_LBUTTONDBLCLK, nFlags, 
							MAKELPARAM (point.x, point.y));
}
//****************************************************************************************
int CBCGTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bFlat)
	{
		//-------------------
		// Create scrollbars:
		//-------------------
		CRect rectDummy (0, 0, 0, 0);
		m_wndScrollTabs.Create (WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy,
			this, (UINT) -1);

		if (m_bSharedScroll)
		{
			m_wndScrollWnd.Create (WS_CHILD | WS_VISIBLE | SBS_HORZ, rectDummy,
				this, (UINT) -1);
		}

		//---------------------
		// Create active brush:
		//---------------------
		m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());
	}
	else
	{
		//---------------------------------------
		// Text may be truncated. Create tooltip.
		//---------------------------------------
		m_ToolTip.Create (this, TTS_ALWAYSTIP);
		m_ToolTip.Activate (TRUE);
		m_ToolTip.BringWindowToTop ();
	}

	SetTabsHeight ();
	return 0;
}
//***************************************************************************************
BOOL CBCGTabWnd::SetImageList (UINT uiID, int cx, COLORREF clrTransp)
{
	if (m_bFlat)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (!m_Images.Create (uiID, cx, 0, clrTransp))
	{
		return FALSE;
	}

	IMAGEINFO info;
	m_Images.GetImageInfo (0, &info);

	CRect rectImage = info.rcImage;
	m_sizeImage = rectImage.Size ();

	SetTabsHeight ();
	return TRUE;
}
//***************************************************************************************
BOOL CBCGTabWnd::OnEraseBkgnd(CDC* pDC)
{
	if (m_iTabsNum == 0)
	{
		CBrush brBtnFace;
		brBtnFace.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));

		CRect rectClient;
		GetClientRect (rectClient);
		pDC->FillRect (rectClient, &brBtnFace);
	}

	return TRUE;
}
//****************************************************************************************
BOOL CBCGTabWnd::PreTranslateMessage(MSG* pMsg) 
{
   	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (m_iActiveTab != -1 &&
			::GetAsyncKeyState (VK_CONTROL) & 0x8000)	// Ctrl is pressed
		{
			switch (pMsg->wParam)
			{
			case VK_NEXT:
				SetActiveTab (m_iActiveTab == m_iTabsNum - 1 ? 0 : m_iActiveTab + 1);
				GetActiveWnd ()->SetFocus ();
				GetParent ()->SendMessage (BCGM_CHANGE_ACTIVE_TAB, m_iActiveTab);
				break;

			case VK_PRIOR:
				SetActiveTab (m_iActiveTab == 0 ? m_iTabsNum - 1 : m_iActiveTab - 1);
				GetActiveWnd ()->SetFocus ();
				GetParent ()->SendMessage (BCGM_CHANGE_ACTIVE_TAB, m_iActiveTab);
				break;
			}
		}
		// Continue....

	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_MOUSEMOVE:
		if (!m_bFlat)
		{
			m_ToolTip.RelayEvent(pMsg);
		}
		break;
	}

	return CWnd::PreTranslateMessage(pMsg);
}
//****************************************************************************************
CWnd* CBCGTabWnd::GetTabWnd (int iTab) const
{
	if (iTab >= 0 && iTab < m_iTabsNum)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [iTab];
		ASSERT_VALID (pTab);

		return pTab->m_pWnd;
	}
	else
	{
		return NULL;
	}
}
//******************************************************************************************
CWnd* CBCGTabWnd::GetActiveWnd () const
{
	return m_iActiveTab == -1 ? 
		NULL : 
		((CBCGTabInfo*) m_arTabs [m_iActiveTab])->m_pWnd;
}
//******************************************************************************************
void CBCGTabWnd::TruncateText (CDC* pDC, CString& strText, int nMaxWidth)
//--------------
// By Alan Fritz
//--------------
{
	ASSERT_VALID (pDC);

	if (strText.IsEmpty ())
	{
		return;
	}

	CString strFirstChar = strText.Left (1);
	if (pDC->GetTextExtent (strFirstChar).cx > nMaxWidth)
	{
		// Even first char can't be displayed!
		strText.Empty ();
		return;
	}

	const CString strEllipses (_T("..."));
	CString strNewText (strText);

	while (strText.GetLength () > 1 &&
		pDC->GetTextExtent (strNewText).cx > nMaxWidth)
	{
		strText = strText.Left (strText.GetLength () - 1);
		strNewText = strText + strEllipses;
	}

	if (strText.GetLength () == 1)
	{
		// Start remove dots from ellipses...
		while (!strNewText.IsEmpty () &&
			pDC->GetTextExtent (strNewText).cx > nMaxWidth)
		{
			strNewText = strNewText.Left (strNewText.GetLength () - 1);
		}
	}

	strText = strNewText;
}
//******************************************************************************************
void CBCGTabWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!m_bFlat)
	{
		CWnd::OnHScroll (nSBCode, nPos, pScrollBar);
		return;
	}

	if (pScrollBar->GetSafeHwnd () == m_wndScrollWnd.GetSafeHwnd ())
	{
		static BOOL bInsideScroll = FALSE;

		if (m_iActiveTab != -1 && !bInsideScroll)
		{
			//----------------------------------------------------------------------
			// MSDN CWnd::OnHScroll
			//----------------------------------------------------------------------
			int nMinPos;
			int nMaxPos;
			pScrollBar->GetScrollRange(&nMinPos, &nMaxPos); 
			nMaxPos = pScrollBar->GetScrollLimit();
			
			// Get the current position of scroll box.
			int nCurPos = pScrollBar->GetScrollPos();
			int nDeltaX = nPos - nCurPos;
			
			// Determine the new position of scroll box.
			switch (nSBCode)
			{
			case SB_LEFT:  nCurPos = nMinPos; break;
			case SB_RIGHT: nCurPos = nMaxPos; break;
			case SB_ENDSCROLL: break;
			
			case SB_LINELEFT: 
				if (nCurPos > nMinPos)
					nCurPos--;
				break;
			
			case SB_LINERIGHT:
				if (nCurPos < nMaxPos)
					nCurPos++;
				break;
			
			case SB_PAGELEFT:
				{
					SCROLLINFO si = {0};
					pScrollBar->GetScrollInfo(&si, SIF_ALL);
					
					if (nCurPos > nMinPos)
						nCurPos = max(nMinPos, nCurPos - (int)si.nPage);
				}
				break;

			case SB_PAGERIGHT:
				{
					SCROLLINFO si = {0};
					pScrollBar->GetScrollInfo(&si, SIF_ALL);
					
					if (nCurPos < nMaxPos)
						nCurPos = min(nMaxPos, nCurPos + (int)si.nPage);
				}
				break;
				
			case SB_THUMBPOSITION: nCurPos = nPos; break;
			case SB_THUMBTRACK:    nCurPos = nPos; break;
			}

			nPos = nCurPos;
			//----------------------------------------------------------------------

			CWnd* pWndActive = GetActiveWnd ();
			ASSERT_VALID(pWndActive);
			WPARAM wParam = MAKEWPARAM(nSBCode, nPos);

			//----------------------------------
			// Pass scroll to the active window:
			//----------------------------------
			bInsideScroll = TRUE;
			if (pWndActive->IsKindOf(RUNTIME_CLASS(CListCtrl)) && nSBCode == SB_THUMBTRACK)
				((CListCtrl *)pWndActive)->Scroll(CSize(nDeltaX, 0));
			else
				pWndActive->SendMessage(WM_HSCROLL, wParam);
			bInsideScroll = FALSE;

			m_wndScrollWnd.SetScrollPos(pWndActive->GetScrollPos(SB_HORZ));

			HideActiveWindowHorzScrollBar();
			GetParent()->SendMessage(BCGM_ON_HSCROLL, wParam);
		}
		return;
	}

	if (pScrollBar->GetSafeHwnd () != m_wndScrollTabs.GetSafeHwnd ())
	{
		CWnd::OnHScroll (nSBCode, nPos, pScrollBar);
		return;
	}

	int nPrevOffset = m_nTabsHorzOffset;
	const int nScrollOffset = 20;

	switch (nSBCode)
	{
	case SB_LINELEFT:
		m_nTabsHorzOffset -= nScrollOffset;
		break;

	case SB_LINERIGHT:
		m_nTabsHorzOffset += nScrollOffset;
		break;

	default:
		AdjustTabsScroll ();
		return;
	}

	m_nTabsHorzOffset = min (max (0, m_nTabsHorzOffset), m_nTabsHorzOffsetMax);
	if (nPrevOffset != m_nTabsHorzOffset)
	{
		AdjustTabs ();
		InvalidateRect (m_rectTabsArea);
		UpdateWindow ();

		if (m_nTabsHorzOffset == 0)
		{
			m_wndScrollTabs.EnableScrollBar ();
			m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_LTUP);
		}
		else if (m_nTabsHorzOffset == m_nTabsHorzOffsetMax)
		{
			m_wndScrollTabs.EnableScrollBar ();
			m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_RTDN);
		}
	}
}
//******************************************************************************************
CWnd* CBCGTabWnd::FindTargetWnd (const CPoint& point)
{
	if (point.y < m_nTabsHeight)
	{
		return NULL;
	}

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		if (pTab->m_rect.PtInRect (point))
		{
			return NULL;
		}
	}

	CWnd* pWndParent = GetParent ();
	ASSERT_VALID (pWndParent);

	return pWndParent;
}
//************************************************************************************
void CBCGTabWnd::AdjustTabsScroll ()
{
	ASSERT_VALID (this);

	if (!m_bFlat)
	{
		m_nTabsHorzOffset = 0;
		return;
	}

	if (m_iTabsNum == 0)
	{
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_BOTH);
		m_nTabsHorzOffsetMax = 0;
		m_nTabsHorzOffset = 0;
		return;
	}

	int nPrevHorzOffset = m_nTabsHorzOffset;

	m_nTabsHorzOffsetMax = max (0, m_nTabsTotalWidth - m_rectTabsArea.Width ());
	m_nTabsHorzOffset = max (0, min (m_nTabsHorzOffset, m_nTabsHorzOffsetMax));

	m_wndScrollTabs.EnableScrollBar ();
	if (m_nTabsHorzOffset <= 0)
	{
		m_nTabsHorzOffset = 0;
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_LTUP);
	}

	if (m_nTabsHorzOffset >= m_nTabsHorzOffsetMax)
	{
		m_nTabsHorzOffset = m_nTabsHorzOffsetMax;
		m_wndScrollTabs.EnableScrollBar (ESB_DISABLE_RTDN);
	}

	if (nPrevHorzOffset != m_nTabsHorzOffset)
	{
		AdjustTabs ();
		InvalidateRect (m_rectTabsArea);
		UpdateWindow ();
	}
}
//*************************************************************************************
void CBCGTabWnd::RecalcLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	ASSERT_VALID (this);

	CRect rectClient;
	GetClientRect (rectClient);

	m_rectTabsArea = rectClient;
	m_rectTabsArea.DeflateRect (TAB_BORDER_SIZE, 0);

	if (m_bFlat)
	{
		m_rectTabsArea.bottom -= TAB_BORDER_SIZE - 1;
		m_rectTabsArea.top = m_rectTabsArea.bottom - m_nTabsHeight;

		int nScrollBarWidth = ::GetSystemMetrics (SM_CXHSCROLL) * 2;
		int nScrollBarHeight = m_rectTabsArea.Height () - 3;

		m_rectTabsArea.left += nScrollBarWidth + 1;
		m_rectTabsArea.right -= TAB_BORDER_SIZE;

		if (m_rectTabsArea.right < m_rectTabsArea.left)
		{
			nScrollBarWidth = 0;
			m_rectTabsArea.left = rectClient.left + TAB_BORDER_SIZE + 1;
			m_rectTabsArea.right = rectClient.right - TAB_BORDER_SIZE - 1;
		}

		if (m_rectTabsArea.top - TAB_BORDER_SIZE < rectClient.top)
		{
			nScrollBarHeight = 0;
			m_rectTabsArea.left = 0;
			m_rectTabsArea.right = 0;
		}

		ASSERT (m_wndScrollTabs.GetSafeHwnd () != NULL);

		m_wndScrollTabs.SetWindowPos (NULL,
			rectClient.left + TAB_BORDER_SIZE + 1, m_rectTabsArea.top + 1,
			nScrollBarWidth, nScrollBarHeight,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOREDRAW);
		
	}
	else
	{
		m_rectTabsArea.top = m_rectTabsArea.bottom - m_nTabsHeight;
		m_rectTabsArea.right -= 2 * TAB_BORDER_SIZE;
	}

	m_nScrollBarRight = m_rectTabsArea.right - ::GetSystemMetrics (SM_CXVSCROLL);

	m_rectWndArea = rectClient;
	m_rectWndArea.DeflateRect (TAB_BORDER_SIZE + 1, TAB_BORDER_SIZE + 1);
	
	if (m_bFlat)
	{
		m_rectWndArea.bottom = m_rectTabsArea.top;
	}
	else
	{
		m_rectWndArea.bottom = m_rectTabsArea.top - 2 * TAB_BORDER_SIZE + 1;
	}

	for (int i = 0; i < m_iTabsNum; i ++)
	{
		CBCGTabInfo* pTab = (CBCGTabInfo*) m_arTabs [i];
		ASSERT_VALID (pTab);

		pTab->m_pWnd->SetWindowPos (NULL,
			m_rectWndArea.left, m_rectWndArea.top,
			m_rectWndArea.Width () - 1, m_rectWndArea.Height (),
			SWP_NOACTIVATE | SWP_NOZORDER);
	}

	AdjustWndScroll ();
	AdjustTabs ();
	AdjustTabsScroll ();

	Invalidate ();
	UpdateWindow ();
}
//*************************************************************************************
void CBCGTabWnd::AdjustWndScroll ()
{
	if (!m_bSharedScroll)
	{
		return;
	}

	ASSERT_VALID (this);

	CRect rectScroll = m_rectTabsArea;

	if (m_nHorzScrollWidth >= MIN_SROLL_WIDTH)
	{
		rectScroll.right = m_nScrollBarRight;
		rectScroll.left = rectScroll.right - m_nHorzScrollWidth;
		rectScroll.bottom -= 2;

		m_rectTabSplitter = rectScroll;
		m_rectTabSplitter.top ++;
		m_rectTabSplitter.right = rectScroll.left;
		m_rectTabSplitter.left = m_rectTabSplitter.right - SPLITTER_WIDTH;

		m_rectTabsArea.right = m_rectTabSplitter.left;
		
		ASSERT (!m_rectTabSplitter.IsRectEmpty ());
	}
	else
	{
		rectScroll.SetRectEmpty ();
		m_rectTabSplitter.SetRectEmpty ();
	}

	m_wndScrollWnd.SetWindowPos (NULL,
		rectScroll.left, rectScroll.top,
		rectScroll.Width (), rectScroll.Height (),
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS);
}
//***************************************************************************************
BOOL CBCGTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bFlat && !m_rectTabSplitter.IsRectEmpty ())
	{
		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ScreenToClient (&ptCursor);

		if (m_rectTabSplitter.PtInRect (ptCursor))
		{
			HCURSOR hCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE));
			::SetCursor(hCursor);
			if (hCursor) ::DestroyCursor(hCursor);
			return TRUE;
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
//***************************************************************************************
void CBCGTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTrackSplitter)
	{
		m_bTrackSplitter = FALSE;
		ReleaseCapture ();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}
//***************************************************************************************
void CBCGTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTrackSplitter)
	{
		int nSplitterLeftPrev = m_rectTabSplitter.left;

		m_nHorzScrollWidth = min (
			m_nScrollBarRight - m_rectTabsArea.left - SPLITTER_WIDTH, 
			m_nScrollBarRight - point.x);

		m_nHorzScrollWidth = max (MIN_SROLL_WIDTH, m_nHorzScrollWidth);
		AdjustWndScroll ();

		if (m_rectTabSplitter.left > nSplitterLeftPrev)
		{
			CRect rect = m_rectTabSplitter;
			rect.left = nSplitterLeftPrev - 20;
			rect.right = m_rectTabSplitter.left;
			rect.InflateRect (0, TAB_BORDER_SIZE);

			InvalidateRect (rect);
		}

		CRect rectTabSplitter = m_rectTabSplitter;
		rectTabSplitter.InflateRect (0, TAB_BORDER_SIZE);

		InvalidateRect (rectTabSplitter);
		UpdateWindow ();
		AdjustTabsScroll ();
	}
	
	CWnd::OnMouseMove(nFlags, point);
}
//***************************************************************************************
void CBCGTabWnd::OnCancelMode() 
{
	CWnd::OnCancelMode();
	
	if (m_bTrackSplitter)
	{
		m_bTrackSplitter = FALSE;
		ReleaseCapture ();
	}
}
//***********************************************************************************
void CBCGTabWnd::SetActiveTabTextColor (COLORREF clr)
{
	ASSERT (m_bFlat);
	m_clrActiveTabFg = clr;
}
//***********************************************************************************
void CBCGTabWnd::SetActiveTabColor (COLORREF clr)
{
	ASSERT (m_bFlat);

	m_clrActiveTabBk = clr;

	if (m_brActiveTab.GetSafeHandle () != NULL)
	{
		m_brActiveTab.DeleteObject ();
	}

	m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());
}
//**********************************************************************************
void CBCGTabWnd::OnSysColorChange() 
{
	CWnd::OnSysColorChange();

	if (m_bFlat && m_clrActiveTabFg == (COLORREF) -1)
	{
		if (m_brActiveTab.GetSafeHandle () != NULL)
		{
			m_brActiveTab.DeleteObject ();
		}

		m_brActiveTab.CreateSolidBrush (GetActiveTabColor ());

		Invalidate ();
		UpdateWindow ();
	}
}
//***********************************************************************************
BOOL CBCGTabWnd::SynchronizeScrollBar (SCROLLINFO* pScrollInfo/* = NULL*/)
{
	if (!m_bSharedScroll)
		return FALSE;

	ASSERT_VALID (this);

	SCROLLINFO scrollInfo = {0};
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_ALL;

	CWnd* pWndActive = GetActiveWnd();

	if (pScrollInfo != NULL)
		scrollInfo = *pScrollInfo;
	else if (pWndActive != NULL)
	{
		if (!pWndActive->GetScrollInfo( SB_HORZ, &scrollInfo) ||
			scrollInfo.nMin + (int) scrollInfo.nPage >= scrollInfo.nMax)
		{
			m_wndScrollWnd.EnableScrollBar(ESB_DISABLE_BOTH);
			return TRUE;
		}
	}

	m_wndScrollWnd.EnableScrollBar(ESB_ENABLE_BOTH);
	m_wndScrollWnd.SetScrollInfo(&scrollInfo);

	HideActiveWindowHorzScrollBar();
	return TRUE;
}
//*************************************************************************************
void CBCGTabWnd::HideActiveWindowHorzScrollBar ()
{
	CWnd* pWnd = GetActiveWnd ();
	if (pWnd == NULL)
	{
		return;
	}

	ASSERT_VALID (pWnd);

	pWnd->ShowScrollBar(SB_HORZ, FALSE);
//	pWnd->ModifyStyle (WS_HSCROLL, 0, SWP_DRAWFRAME);
}
//************************************************************************************
void CBCGTabWnd::SetTabsHeight ()
{
	if (m_bFlat)
	{
		m_nTabsHeight = ::GetSystemMetrics (SM_CYHSCROLL) + TEXT_MARGIN / 2;

		CFont* pDefaultFont = 
			CFont::FromHandle ((HFONT) ::GetStockObject (DEFAULT_GUI_FONT));
		ASSERT_VALID (pDefaultFont);

		if (pDefaultFont != NULL)	// Just to be relaxed....
		{
			LOGFONT lfDefault;
			pDefaultFont->GetLogFont (&lfDefault);

			LOGFONT lf;
			memset (&lf, 0, sizeof (LOGFONT));

			lf.lfCharSet = lfDefault.lfCharSet;
			lf.lfHeight = lfDefault.lfHeight;
			_tcscpy (lf.lfFaceName, TABS_FONT);

			CClientDC dc (this);

			TEXTMETRIC tm;

			do
			{
				m_fntTabs.DeleteObject ();
				m_fntTabs.CreateFontIndirect (&lf);

				CFont* pFont = dc.SelectObject (&m_fntTabs);
				ASSERT (pFont != NULL);

				dc.GetTextMetrics (&tm);
				dc.SelectObject (pFont);

				if (tm.tmHeight + TEXT_MARGIN / 2 <= m_nTabsHeight)
				{
					break;
				}

				//------------------
				// Try smaller font:
				//------------------
				if (lf.lfHeight < 0)
				{
					lf.lfHeight ++;
				}
				else
				{
					lf.lfHeight --;
				}
			}
			while (lf.lfHeight != 0);

			//------------------
			// Create bold font:
			//------------------
			lf.lfWeight = FW_BOLD;
			m_fntTabsBold.DeleteObject ();
			m_fntTabsBold.CreateFontIndirect (&lf);
		}
	}
	else
	{
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);
		::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
   
		CFont ftRegular;
		ftRegular.CreateFontIndirect(&info.lfMenuFont);

		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(&ftRegular);

		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm);
		dc.SelectObject(pOldFont);

		int nTextHeight = tm.tmHeight + 2;

		m_nTabsHeight = (max (m_sizeImage.cy + 2 * IMAGE_MARGIN,
			nTextHeight + TEXT_MARGIN / 2));
	}
}
//*************************************************************************************
void CBCGTabWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);
	
	//-----------------------------------------------------------------
	// In the flat modetabs height should be same as scroll bar height
	//-----------------------------------------------------------------
	if (m_bFlat)
	{
		SetTabsHeight ();
		RecalcLayout ();
		SynchronizeScrollBar ();
	}
}
//*************************************************************************************
BOOL CBCGTabWnd::EnsureVisible (int iTab)
{
	if (iTab < 0 || iTab >= m_iTabsNum)
	{
		TRACE(_T("EnsureVisible: illegal tab number %d\n"), iTab);
		return FALSE;
	}

	//---------------------------------------------------------
	// Be sure, that active tab is visible (not out of scroll):
	//---------------------------------------------------------
	if (m_bFlat && m_rectTabsArea.Width () > 0)
	{
		CRect rectActiveTab = ((CBCGTabInfo*) m_arTabs [m_iActiveTab])->m_rect;
		BOOL bAdjustTabs = FALSE;

		if (rectActiveTab.left < m_rectTabsArea.left)
		{
			m_nTabsHorzOffset -= (m_rectTabsArea.left - rectActiveTab.left);
			bAdjustTabs = TRUE;
		}
		else if (rectActiveTab.right > m_rectTabsArea.right)
		{
			m_nTabsHorzOffset += (rectActiveTab.right - m_rectTabsArea.right);
			bAdjustTabs = TRUE;
		}

		if (bAdjustTabs)
		{
			AdjustTabs ();
			AdjustTabsScroll ();
		}
	}

	return TRUE;
}
//**********************************************************************************
BOOL CBCGTabWnd::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CWnd::OnNotify (wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && !m_bFlat)
	{
		m_ToolTip.SetWindowPos (&wndTop, -1, -1, -1, -1,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}
