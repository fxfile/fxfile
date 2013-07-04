/////////////////////////////////////////////////////////////////////////////
// FrameWndEx.h: interface for the frame classes.
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

#ifndef __FRAMEWNDEX_H__
#define __FRAMEWNDEX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "MenuBar.h"
#include "SizableReBar.h"
#include "WinAppEx.h"
#include "main_coolbar.h"
#include "main_menubar.h"

#define AFX_IDW_SIZABLEREBAR (AFX_IDW_CONTROLBAR_FIRST + 4)
#define AFX_IDW_MENUBAR      (AFX_IDW_CONTROLBAR_FIRST + 6)

/////////////////////////////////////////////////////////////////////////////
// CFrameWndBase

class CFrameWndBase : public CFrameWnd, public CMenuBarObserver
{
// Constructors
public:
    CFrameWndBase();
    ~CFrameWndBase();

// Attributes
public:
    fxfile::MainCoolBar m_wndReBar;
    fxfile::MainMenuBar m_wndMenuBar;

// Operations
public:
    int CreateRebar( void );

// Overrides
public:
    virtual BOOL LoadFrame( UINT nIDResource,
        DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
        CWnd* pParentWnd = 0, CCreateContext* pContext = 0 );

// Implementation data
protected:
    bool    m_bInPlace;

protected:
    virtual void OnLoadString( int nIndex, CString &strText );

protected:
    virtual BOOL PreTranslateMessage( MSG* pMsg );

// Generated message map functions
protected:
    //{{AFX_MSG(CFrameWndEx)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

// Message map functions
protected:
    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
    afx_msg LRESULT OnMenuChar( UINT nChar, UINT nFlags, CMenu* pMenu );
    afx_msg BOOL OnNcActivate( BOOL bActive );
    afx_msg void OnSettingChange( UINT uFlags, LPCTSTR lpszSection );
    afx_msg void OnClose();
    afx_msg void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );
    afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
};

/////////////////////////////////////////////////////////////////////////////
// CFrameWndEx frame

class CFrameWndEx : public CFrameWndBase
{
    DECLARE_DYNCREATE( CFrameWndEx )

// Constructors
public:
    CFrameWndEx();
    ~CFrameWndEx();

// Overrides
protected:
    virtual void DelayUpdateFrameMenu( HMENU hMenuAlt );
    virtual void OnUpdateFrameMenu( HMENU hMenuAlt );

// Generated message map functions
protected:
    //{{AFX_MSG(CFrameWndEx)
    //}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__FRAMEWNDEX_H__
