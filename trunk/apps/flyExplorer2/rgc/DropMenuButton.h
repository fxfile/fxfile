#if !defined(AFX_DropMenuButton_H__3A90681F_CE5F_11D3_808C_005004D6CF90__INCLUDED_)
#define AFX_DropMenuButton_H__3A90681F_CE5F_11D3_808C_005004D6CF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xptheme.h"

class BCMenu;

class CDropMenuButton : public CButton
{
public:
    CDropMenuButton();
    BOOL SetIDs(UINT nID1, UINT m_nID2);
    BOOL AddMenuItem(UINT nMenuId,const CString strMenu, UINT nMenuFlags);
    BOOL LoadMenu(int nResource, int nSubMenu = -1);
	BOOL SetMenu(BCMenu *pMenu);
    BOOL SetButtonImage(UINT nResourceId, COLORREF crMask);
    void SetSplit(BOOL b) { m_bSplit = b && (m_bMenuLoaded || m_nID2); }
    void SetAsDefaultBtn(BOOL b) { m_bDefaultBtn = b; }
    void ClearButtonPushed();

// Attributes
protected:
    BCMenu      *m_pMenu;
	int			m_nSubMenu;

    CBitmap     m_btnImage;
    CImageList  m_IL;
    BOOL        m_Initialized;
    BOOL        m_bPushed;
    BOOL        m_bMenuPushed;
    BOOL        m_bMenuLoaded;
    BOOL        m_bLoaded;
    BOOL        m_bDefaultBtn;
    BOOL        m_bActAsDefaultBtn;
    BOOL        m_bSplit;
    BITMAP      m_bm;
    UINT        m_nID1;
    UINT        m_nID2;
    COLORREF    m_crMask;
    HBITMAP     m_hbmpDisabled;    
    BOOL        m_bOverControl;    // is the mouse over us?
    BOOL        m_bTracking;    // are we tracking the mouse

	CXPTheme m_xpTheme;

// Operations
protected:
    void DrawArrow(CDC* pDC,CPoint ArrowTip);
    BOOL HitMenuBtn(CPoint point);
    BOOL OnMenuBtn();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDropMenuButton)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CDropMenuButton();

    // Generated message map functions
protected:
    //{{AFX_MSG(CDropMenuButton)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSysColorChange();
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();
    afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DropMenuButton_H__3A90681F_CE5F_11D3_808C_005004D6CF90__INCLUDED_)