// An Internet Explorer Style Options Panel
// Copyright (c) 2000 Andy Brown <andy@mirage.dabsol.co.uk>
// You may do whatever you like with this file, I just don't care.


#if !defined(AFX_OPTIONSPANEL_H__3DCE226A_4298_4CEE_89B8_87A162E7A1CE__INCLUDED_)
#define AFX_OPTIONSPANEL_H__3DCE226A_4298_4CEE_89B8_87A162E7A1CE__INCLUDED_
#pragma once


#include <vector>
#include "xptheme.h"
// notification structure

typedef struct tagNMOPTIONSPANEL
{
	NMHDR hdr;
	int		idButton;
	BOOL	bSelected;
} NMOPTIONSPANEL;


// the WM_NOTIFY code

#define OPN_SELCHANGE 0

// options panel class

class OptionsPanel : public CButton
{
protected:
// item class

	class CItem
	{
	public:
		enum eType
		{
			typeGroup,
			typeCheck,
			typeRadio
		} m_Type;

		DWORD		m_dwFlags;
		int			m_nLevel;
		int			m_nID;
		int			m_nImage;
		CString m_strText;
		CRect		m_rcItem;

	protected:
		void Assign(const CItem& src);

	public:
		CItem();
		CItem(const CItem& src);

		CItem& operator=(const CItem& src);
	};

	enum
	{
		idVertScroll=1000
	};

// data members

	std::vector<CItem> m_Items;
	CScrollBar				 m_wndVertScroll;
	CFont							 m_NormalFont;
	CFont							 m_BoldFont;
	CPen							 m_FocusPen;
	CPen							 m_BackPen;
	CImageList				 m_ImageList;
	BOOL							 m_bInited;
	int								 m_BoldFontOffset;
	int								 m_NormalFontOffset;
	int								 m_Top;
	CItem							*m_pFocus;

	CXPTheme m_xpButton;

public:
// enumerations

	enum
	{
		itemChecked=0x1,
		itemBold=0x2
	};

	enum
	{
		groupBold=0x1
	};

protected:
	//{{AFX_MSG(OptionsPanel)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClicked();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void AddCheckOrRadio(const CItem::eType type,const CString& strText,const DWORD dwFlags,const int nLevel,const int nID);
	void Initialize(CDC *pDC);
	void CalculateOneItemRect(CDC *pDC,CItem& item,int& top);
	void CalculateItemRects(CDC *pDC);
	void SetupScrollBars(void);
	void DrawRadio(CDC *pDC,const CItem& radio,const CRect& rcClient);
	void DrawCheck(CDC *pDC,const CItem& check,const CRect& rcClient);
	void DrawGroup(CDC *pDC,const CItem& group,const CRect& rcClient);
	void DrawCheckOrRadio(CDC *pDC,const CItem& button,const int nChecked,const int nClear,const CRect& rcClient);
	void GetFontOffset(CDC *pDC,CFont& font,int& offset) const;
	void DisplaceItemRects(const int disp);
	void DrawBackRect(CDC *pDC,const CItem& item,const CRect& rcClient);
	void DrawSingleItem(CDC *pDC,const CItem& item,const CRect& rcClient);
	CItem *GetItemAtCursor(void);
	void GetAdjustedClientRect(CRect& rcClient) const;
	void ToggleState(CDC *pDC,CItem *pItem,const CRect& rcClient);
	std::vector<CItem>::iterator GetIterator(CItem *pItem);
	void CursorUp(void);
	void CursorDown(void);
	void PageUp(void);
	void PageDown(void);
	void CursorHome(void);
	void CursorEnd(void);
	void EnsureVisible(const CRect& rcClient,const int nItem);
	void SelectItem(CItem *pItem);
	void NotifyParentSelChange(const CItem& item) const;

public:
	OptionsPanel();
	virtual ~OptionsPanel();

	void SetImageList(CImageList& ImgList);
	void AddGroup(const CString& strText,const DWORD dwFlags,const int nLevel,const int nImage);
	void AddGroup(const UINT resID,const DWORD dwFlags,const int nLevel,const int nImage);
	void AddCheck(const CString& strText,const DWORD dwFlags,const int nLevel,const int nID);
	void AddCheck(const UINT resID,const DWORD dwFlags,const int nLevel);
	void AddRadio(const CString& strText,const DWORD dwFlags,const int nLevel,const int nID);
	void AddRadio(const UINT resID,const DWORD dwFlags,const int nLevel);
	BOOL GetCheckState(const int nID) const;
	BOOL SetCheckState(const int nID,const BOOL bChecked=TRUE);

	//{{AFX_VIRTUAL(OptionsPanel)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	//}}AFX_VIRTUAL
};


//{{AFX_INSERT_LOCATION}}


#endif // !defined(AFX_OPTIONSPANEL_H__3DCE226A_4298_4CEE_89B8_87A162E7A1CE__INCLUDED_)
