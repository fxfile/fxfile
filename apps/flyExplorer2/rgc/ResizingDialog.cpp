// A flicker-free resizing dialog class
// Copyright (c) 1999 Andy Brown <andy@mirage.dabsol.co.uk>
// You may do whatever you like with this file, I just don't care.


#include "stdafx.h"
#include "ResizingDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CResizingDialog, CDialog)
// used if OEMRESOURCE not defined in precompiled headers

#ifndef OBM_SIZE
#define OBM_SIZE 32766
#endif

CResizingDialog::CResizingDialog(const UINT resID, CWnd *pParent)
: CDialog(resID, pParent)
{
	//{{AFX_DATA_INIT(CResizingDialog)
	//}}AFX_DATA_INIT

	// allow all sizing
	m_xAllow = sizeResize;
	m_yAllow = sizeResize;

	// not initialized yet
	m_bInited = false;
}

BEGIN_MESSAGE_MAP(CResizingDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CResizingDialog::OnInitDialog() 
{
	// call the base class
	CDialog::OnInitDialog();

	CRect rcIcon,rcDialogClient;

	// get the dialog size
	GetClientRect(rcDialogClient);
	GetWindowRect(m_rcDialog);

	m_MinSize.x = m_rcDialog.Width();
	m_MinSize.y = m_rcDialog.Height();

	// set up the size icon
	m_wndSizeGrip.Create(this, rcDialogClient, m_idSizeIcon);

	// make it auto-position
	AddControl(m_idSizeIcon, sizeRepos, sizeRepos, FALSE);

	// all done
	m_bInited = true;
	return TRUE;
}

void CResizingDialog::OnSize(UINT nType, int cx, int cy)
{
	// call the base class
	CDialog::OnSize(nType,cx,cy);

	CRect rect;
	HDWP hdwp;
	std::vector<CItem>::iterator it;

	// start deferring window pos
	hdwp = BeginDeferWindowPos(20);

	if (!m_Items.empty())
	{
		// get the new size
		GetWindowRect(rect);

		// children can resize themselves
		for (it=m_Items.begin(); it!=m_Items.end(); it++)
			it->OnSize(hdwp, m_rcDialog, rect, this);
	}

	OnSizeItem(nType, cx, cy, hdwp);

	// do the deferred window position change
	EndDeferWindowPos(hdwp);

	// remember new size
	m_rcDialog = rect;
}

void CResizingDialog::OnSizeItem(UINT nType, int cx, int cy, HDWP hdwp)
{
}

void CResizingDialog::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	if (m_bInited)
	{
		lpMMI->ptMinTrackSize = m_MinSize;

		if (m_xAllow == sizeNone)
			lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x = m_MinSize.x;

		if (m_yAllow == sizeNone)
			lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y = m_MinSize.y;
	}
}

BOOL CResizingDialog::OnEraseBkgnd(CDC *pDC)
{
	std::vector<CItem>::const_iterator it;

	for (it=m_Items.begin(); it!=m_Items.end(); it++)
	{
		// skip over the size icon if it's been hidden
		if (it->m_resID == m_idSizeIcon && !m_wndSizeGrip.IsWindowVisible())
		{
			m_wndSizeGrip.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			continue;
		}

		if (it->m_bFlickerFree)
			pDC->ExcludeClipRect(it->m_rcControl);
	}

	CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}

void CResizingDialog::AddControl(const UINT resID, const eSizeType xsize, const eSizeType ysize, const bool bFlickerFree)
{
	CItem item;

	GetDlgItem(resID)->GetWindowRect(item.m_rcControl);
	ScreenToClient(item.m_rcControl);

	item.m_bFlickerFree = bFlickerFree;
	item.m_resID = resID;
	item.m_xSize = xsize;
	item.m_ySize = ysize;

	if (xsize == sizeRelative)
		item.m_xRatio = (double)item.m_rcControl.left / ((double)m_rcDialog.Width() - (double)item.m_rcControl.left);

	if (ysize == sizeRelative)
		item.m_yRatio = (double)item.m_rcControl.top / ((double)m_rcDialog.Height() - (double)item.m_rcControl.top);

	m_Items.push_back(item);
}

void CResizingDialog::AllowSizing(const eSizeType xsize, const eSizeType ysize)
{
	m_xAllow = xsize;
	m_yAllow = ysize;
}

void CResizingDialog::HideSizeIcon(void)
{
	m_wndSizeGrip.ShowWindow(SW_HIDE);
}

CResizingDialog::CItem::CItem()
{
}

CResizingDialog::CItem::CItem(const CItem& src)
{
	Assign(src);
}

CResizingDialog::CItem& CResizingDialog::CItem::operator=(const CItem& src)
{
	Assign(src);
	return *this;
}

void CResizingDialog::CItem::Assign(const CItem& src)
{
	m_rcControl = src.m_rcControl;
	m_resID = src.m_resID;
	m_xSize = src.m_xSize;
	m_ySize = src.m_ySize;
	m_bFlickerFree = src.m_bFlickerFree;
	m_xRatio = src.m_xRatio;
	m_yRatio = src.m_yRatio;
}

void CResizingDialog::CItem::OnSize(HDWP hdwp, const CRect& rcParentOld, const CRect& rcParentNew, CWnd *pDlg)
{
	CSize diff;
	CRect rcControl;
	CWnd *pWnd;
	int   newpos,newsize;

	pWnd = pDlg->GetDlgItem(m_resID);

	diff.cx = rcParentNew.Width()  - rcParentOld.Width();
	diff.cy = rcParentNew.Height() - rcParentOld.Height();

	rcControl = m_rcControl;

	switch (m_xSize)
	{
	case sizeResize:
		rcControl.right += diff.cx;
		break;

	case sizeRepos:
		rcControl.left  += diff.cx;
		rcControl.right += diff.cx;
		break;

	case sizeRelative:
		newpos = (int)((m_xRatio*(double)rcParentNew.Width()) / (1.0+m_xRatio));
		newsize = rcControl.Width();
		rcControl.left = newpos;
		rcControl.right = newpos+newsize;
		break;
	}

	switch (m_ySize)
	{
	case sizeResize:
		rcControl.bottom += diff.cy;
		break;

	case sizeRepos:
		rcControl.top += diff.cy;
		rcControl.bottom += diff.cy;
		break;

	case sizeRelative:
		newpos = (int)((m_yRatio * (double)rcParentNew.Height()) / (1.0 + m_yRatio));
		newsize = rcControl.Height();
		rcControl.top = newpos;
		rcControl.bottom = newpos + newsize;
		break;
	}

	if (rcControl != m_rcControl)
	{
		DeferWindowPos(hdwp, *pWnd, NULL, rcControl.left, rcControl.top, rcControl.Width(), rcControl.Height(), SWP_NOZORDER);
		m_rcControl = rcControl;
	}
}

LRESULT CResizingDialog::OnNcHitTest(CPoint point) 
{
	CRect rSizeRect ;
	m_wndSizeGrip.GetWindowRect(&rSizeRect) ;

	// Test to see if the cursor is within the 'gripper'
	// bitmap, and tell the system that the user is over
	// the lower right-hand corner if it is.
	if (rSizeRect.PtInRect(point))
	{
		return HTBOTTOMRIGHT ;
	}
	else
	{
		return CDialog::OnNcHitTest(point) ;
	}
}
