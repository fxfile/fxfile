/////////////////////////////////////////////////////////////////////////////
// ToolBarEx.h: interface for the CToolBarEx class.
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

#ifndef __TOOLBAREX_H__
#define __TOOLBAREX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CustomizeDialog.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

struct TBBUTTONEX
{
    TBBUTTON tbinfo;
    bool     bInitiallyVisible;
};

class CToolBarEx : public CToolBar
{
    DECLARE_DYNAMIC( CToolBarEx );

    friend class CToolBarPopup;
    friend class CCustomizeDialog;

public:
    class Observer
    {
    public:
        virtual void onUpdatedToolbarSize(CToolBarEx &theToolBar) = 0;
    };

// Construction
public:
    CToolBarEx();
    ~CToolBarEx();

    void SetObserver( Observer *pObserver );

    BOOL Create( CWnd* pParentWnd,
                 DWORD dwStyle,
                 UINT nID = AFX_IDW_TOOLBAR,
                 DWORD dwExtendedStyle = TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS );

public:
    void SetButtons(int nCount, TBBUTTONEX* lpButtons);
    void SetDefaultButtons();

// Attributes
public:
    void SetTextType( int nTextType, bool bUpdate = true );
    int  GetTextType() const;
    void SetIconType( int nIconType, bool bUpdate = true );
    int  GetIconType() const;

// Operations
public:
    void SetIconSize(CSize szLargeIcon, CSize szSmallIcon);
    void SetImageList(UINT nIDSmallDisabled, UINT nIDSmallCold, UINT nIDSmallHot,
                      UINT nIDLargeDisabled, UINT nIDLargeCold, UINT nIDLargeHot,
                      int nIconTypeDefault, COLORREF clrMask = RGB(255,0,255));
    void GetImageList(CImageList *pImgList, BOOL bSmall = TRUE, int nImageType = TBG_HOT);

    void LoadState( LPCTSTR lpszProfileName );
    void SaveState( LPCTSTR lpszProfileName );

    void Customize();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarEx)
    protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    virtual BOOL HasButtonText( UINT nID );
    virtual BOOL HasButtonTip( UINT nID );
    virtual void GetButtonTextByCommand( UINT nID, CString& strText );
    virtual void GetButtonTip( UINT nID, CString& strTip );

public:
    virtual void Init();
    virtual bool IsTextTypeAvailable( int nTextType ) const;
    virtual bool IsIconTypeAvailable( int nIconType ) const;

    void UpdateToolbarSize();

// Implementation
protected:
    void ReloadButtons();
    BOOL GetButtonInfo( UINT nID, TBBUTTON& tbinfo );

    static LRESULT CALLBACK CBTProc( int nCode, WPARAM wParam, LPARAM lParam );

// Implementation data
protected:
    Observer *m_pObserver;

	typedef std::map<UINT, TBBUTTONEX *> ButtonMap;
	typedef std::deque<TBBUTTONEX *> ButtonDeque;

	ButtonMap m_mpButtons;
	ButtonDeque m_dqButtons;

    int             m_nTextType;
    int             m_nIconType;
    int             m_nTextTypeDefault;
    int             m_nIconTypeDefault;

    CSize           m_szLargeIcon;
    CSize           m_szSmallIcon;
    UINT            m_nIDSmallCold;
    UINT            m_nIDSmallHot;
    UINT            m_nIDSmallDisabled;
    UINT            m_nIDLargeCold;
    UINT            m_nIDLargeHot;
    UINT            m_nIDLargeDisabled;
    CImageList      m_imageListCold;
    CImageList      m_imageListHot;
    CImageList      m_imageListDisabled;
    COLORREF        m_clrMask;
    HGLOBAL         m_hToolbarData;

    static LPCTSTR              m_lpszStateInfoEntry;
    static CToolBarEx*          m_pToolBar;
    static CCustomizeDialog*    m_pCustomizeDlg;
    static HHOOK                m_hCBTHook;

// Generated message map functions
protected:
    //{{AFX_MSG(CToolBarEx)
    //}}AFX_MSG

    afx_msg LRESULT OnReBarChevronPushed( WPARAM wParam, LPARAM lParam );

    afx_msg void OnGetInfoTip( NMHDR* pNMHDR, LRESULT* pResult );

    // Toolbar customization
    afx_msg void OnBeginAdjust( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnEndAdjust( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnQueryInsert( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnQueryDelete( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnInitCustomize( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnGetButtonInfo( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnReset( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnToolBarChange( NMHDR* pNMHDR, LRESULT* pResult );

    // Saving and restoring toolbar
    afx_msg void OnSave( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnRestore( NMHDR* pNMHDR, LRESULT* pResult );
    DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////
// CToolBarPopup

class CToolBarPopup : public CWnd
{
    DECLARE_DYNAMIC( CToolBarPopup );

// Construction
public:
    CToolBarPopup( CToolBarEx* pToolBar );
    ~CToolBarPopup();

// Operations
public:
    bool ShowPopup( UINT nFlags, CPoint pt, CRect& rcExclude );

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarPopup)
    protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

// Implementation
protected:
    void SetPosition( UINT nFlags, CPoint pt, CRect& rcExclude );
    void Show( UINT nFlags, const CRect& rc );
    void OnKeyDown( UINT nChar );
    void DrawBorder( CDC* pDC );

    static LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );

// Implementation data
protected:
    CToolBarEx*     m_pToolBar;
    CToolBarCtrl    m_tbCtrl;
    bool            m_bOverTbCtrl;
    bool            m_bTextLabels;
    bool            m_bFlatMenu;

    static HHOOK            m_hKeyboardHook;
    static CToolBarPopup*   m_pPopup;

// Generated message map functions
protected:
    //{{AFX_MSG(CToolBarPopup)
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnNcPaint();
    //}}AFX_MSG
    afx_msg LRESULT OnPrint( WPARAM wParam, LPARAM lParam );
    afx_msg void OnEndModalLoop();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__MENUBAR_H__
