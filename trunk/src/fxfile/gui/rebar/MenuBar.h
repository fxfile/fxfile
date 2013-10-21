/////////////////////////////////////////////////////////////////////////////
// MenuBar.h: interface for the CMenuBar class.
//
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001-2002 by Nikolay Denisov. All rights reserved.
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
// Please email bug reports, bug fixes, enhancements, requests and
// comments to: acnick@mail.lanck.net
/////////////////////////////////////////////////////////////////////////////

#ifndef __MENUBAR_H__
#define __MENUBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GlobalData.h"

#include <afxtempl.h>

class BCMenu;

/////////////////////////////////////////////////////////////////////////////
// CMenuBarButtonXxx - helper classes

class CMenuBarButton
{
// Construction
public:
    CMenuBarButton();
    virtual ~CMenuBarButton();

// Operations
public:
    void DrawButton( CDC* pDC, bool bFrameActive );
    bool HitTest( CPoint pt ) const;
    void SetMDIChild( HWND hWndMDIChild );
    void SetMenuBarRect( LPCRECT lpRect );
    void SetTheme( HTHEME hTheme );
    bool HideButton( bool bHide );
    bool PushButton( bool bPush );
    bool IsPushed() const;

    static CSize GetButtonSize();

// Overrides
public:
    virtual CRect GetButtonRect() const = 0;
    virtual UINT GetSysCommandID() const = 0;
    virtual bool IsEnabled() const;
    virtual bool IsVisible() const;

// Implementation
protected:
    DWORD GetMDIChildStyle() const;

// Implementation data
protected:
    HWND    m_hWndMDIChild;
    CRect   m_rcMenuBar;
    bool    m_bHidden;
    bool    m_bPushed;
    HTHEME  m_hTheme;
};

class CMenuBarButtonMin : public CMenuBarButton
{
// Overrides
public:
    virtual CRect GetButtonRect() const;
    virtual UINT GetSysCommandID() const;
    virtual bool IsEnabled() const;
    virtual bool IsVisible() const;
};

class CMenuBarButtonMax : public CMenuBarButton
{
// Overrides
public:
    virtual CRect GetButtonRect() const;
    virtual UINT GetSysCommandID() const;
    virtual bool IsEnabled() const;
    virtual bool IsVisible() const;
};

class CMenuBarButtonClose : public CMenuBarButton
{
// Overrides
public:
    virtual CRect GetButtonRect() const;
    virtual UINT GetSysCommandID() const;
};

/////////////////////////////////////////////////////////////////////////////
// CMenuBarObserver

class CMenuBarObserver
{
public:
    virtual void OnLoadString( int nIndex, CString &strText ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CMenuBar

class CFrameWndBase;

class CMenuBar : public CToolBar
{
    DECLARE_DYNAMIC( CMenuBar );

    friend CFrameWndBase;

// Construction
public:
    CMenuBar();
    ~CMenuBar();

// Operations
public:
    void SetObserver( CMenuBarObserver *pObserver );

    bool SetMenu( UINT nID );
    bool ReloadMenu();
	CMenu* GetMenu() const;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMenuBar)
    //}}AFX_VIRTUAL

    virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler );
    virtual INT_PTR OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;

// Implementation
protected:
    CReBarCtrl& GetParentReBarCtrl() const;
    int GetParentBandIndex() const;
    void SetButtonWidth( UINT nID, int nWidth );
    void UpdateMenuBar();
    void RepositionSysButtons( CRect rcMenuBar );
    void EnterTrackingMode( int nItem );
    void TrackChevronMenu( CRect& rcChevron, int nItem );
    void TrackPopupMenu();
    void ContinueTracking( bool bSelectFirst );
    void ExitTrackingMode();
    void ShowChevronMenu( int nItem );
    bool IsItemClipped( int nItem ) const;
    bool IsOverChevron( CPoint pt ) const;
    bool OnButtonDown( UINT nFlags, CPoint pt, bool bLeft );

// Implementation
protected:
    void HookMessageProc( UINT message, WPARAM wParam, LPARAM lParam );
    bool FrameOnSysCommand( UINT nID, LPARAM lParam );
    bool FrameOnMenuChar( UINT nChar, UINT nFlags, CMenu* pMenu );
    void FrameOnNcActivate( BOOL bActive );
    void FrameOnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );
    void FrameOnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
    BOOL FramePreTranslateMessage( MSG *pMsg );
    void OpenTheme();
    void CloseTheme();

// Static members
protected:
    static HHOOK m_hMsgHook;
    static CMenuBar* m_pMenuBar;
    static LRESULT CALLBACK MessageProc( int code, WPARAM wParam, LPARAM lParam );

// Implementation data
protected:
    CMenuBarObserver *m_pObserver;

    CFont   m_fontMenu;
    CPoint  m_ptMouseLast;

    HWND    m_hWndMDIChild;
    HWND    m_hWndOldFocus;
    UINT    m_nResourceID;
    BCMenu* m_pMenu;
    BCMenu* m_pMenuTracking;

    int     m_nItem;
    bool    m_bItemTracking;
    bool    m_bItemDropped;
    bool    m_bPrimaryMenu;
    bool    m_bSubmenuItem;
    bool    m_bEscape;
    bool    m_bContinue;
    bool    m_bSelectFirst;
    bool    m_bButtonCapture;
    bool    m_bFrameActive;

    CTypedPtrArray< CPtrArray, CMenuBarButton* > m_aMenuBarButtons;

    HTHEME  m_hTheme;

// Generated message map functions
protected:
    //{{AFX_MSG(CMenuBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    //}}AFX_MSG

    afx_msg LRESULT OnShowPopupMenu( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnReBarChildSize( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnReBarChevronPushed( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );

    afx_msg void OnUpdateMenuButton( CCmdUI* pCmdUI );

    // Toolbar control notifications
    afx_msg void OnDropDown( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnHotItemChange( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );
    DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__MENUBAR_H__
