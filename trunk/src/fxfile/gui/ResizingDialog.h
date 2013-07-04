// A flicker-free resizing dialog class
// Copyright (c) 1999 Andy Brown <andy@mirage.dabsol.co.uk>
// You may do whatever you like with this file, I just don't care.


#if !defined(AFX_RESIZINGDIALOG_H__6907518B_528A_4FF8_9D41_E87796B543AF__INCLUDED_)
#define AFX_RESIZINGDIALOG_H__6907518B_528A_4FF8_9D41_E87796B543AF__INCLUDED_
#pragma once


#include <vector>
#include "SizeGripWnd.h"

// resizing dialog class
class CResizingDialog : public CDialog
{
public:
	DECLARE_DYNCREATE(CResizingDialog)
	CResizingDialog(const UINT resID = 0,CWnd *pParent = NULL);
	// possible sizing types
	
	enum eSizeType
	{
		sizeNone,                           // do nothing
		sizeResize,                         // proportional expand/contract
		sizeRepos,                          // maintain distance from top/left
		sizeRelative                        // proportional distance from sides
	};
	
protected:
	// id for the size icon - change if you get a clash with any of your controls
	
	enum { m_idSizeIcon=0x4545 };
	
	// contained class to hold item state
	
	class CItem
	{
	public:
		UINT      m_resID;                  // resource ID
		eSizeType m_xSize;                  // x sizing option
		eSizeType m_ySize;                  // y sizing option
		CRect     m_rcControl;              // last size
		bool      m_bFlickerFree;           // flicker-free move?
		double    m_xRatio;                 // x ratio (for relative)
		double    m_yRatio;                 // y ratio (for relative)
		
	protected:
		void Assign(const CItem& src);
		
	public:
		CItem();
		CItem(const CItem& src);
		
		void OnSize(HDWP hdwp,const CRect& rcParentOld,const CRect& rcParentNew,CWnd *pDlg);
		
		CItem& operator=(const CItem& src);
	};
	
	// data members
	
	std::vector<CItem>	m_Items;           // array of controlled items
	CRect				m_rcDialog;        // last dialog size
	CPoint				m_MinSize;         // smallest size allowed
	eSizeType			m_xAllow;          // horizontal sizing allowed
	eSizeType			m_yAllow;          // vertical sizing allowed
	SizeGripWnd		m_wndSizeGrip;     // size icon window
	bool				m_bInited;         // set after initialize
	
protected:
	//{{AFX_MSG(CResizingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnSizeItem(UINT nType, int cx, int cy, HDWP hdwp);

	void AddControl(const UINT resID,const eSizeType xsize,const eSizeType ysize,const bool bFlickerFree=true);
	void AllowSizing(const eSizeType xsize,const eSizeType ysize);
	void HideSizeIcon(void);
	
public:
	
	//{{AFX_DATA(CResizingDialog)
	//}}AFX_DATA
	
	//{{AFX_VIRTUAL(CResizingDialog)
	//}}AFX_VIRTUAL
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_RESIZINGDIALOG_H__6907518B_528A_4FF8_9D41_E87796B543AF__INCLUDED_)
