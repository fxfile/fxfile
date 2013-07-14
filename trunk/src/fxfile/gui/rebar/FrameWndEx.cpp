/////////////////////////////////////////////////////////////////////////////
// FrameWndEx.cpp: implementation of the frame classes.
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

#include "stdafx.h"
#include "FrameWndEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameWndBase out-of-line functions

CFrameWndBase::CFrameWndBase() : m_bInPlace( false )
{
}

CFrameWndBase::~CFrameWndBase()
{
}

BOOL CFrameWndBase::LoadFrame( UINT nIDResource, DWORD dwDefaultStyle,
                                        CWnd* pParentWnd, CCreateContext* pContext )
{
    if ( !CFrameWnd::LoadFrame( nIDResource, dwDefaultStyle, pParentWnd, pContext ) )
    {
        return FALSE;
    }

    m_wndMenuBar.SetObserver( dynamic_cast<CMenuBarObserver *>(this) );

    VERIFY( m_wndMenuBar.SetMenu( IDR_MAINFRAME ) );
    VERIFY( SetMenu( 0 ) );

    return TRUE;
}

int CFrameWndBase::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
    {
        return -1;
    }

    return 0;
}

int CFrameWndBase::CreateRebar( void )
{
    m_wndReBar.SetEdgeSize(0);

    if ( !m_wndReBar.Create( this, AFX_IDW_SIZABLEREBAR,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
            CBRS_ALIGN_TOP | CBRS_SIZE_DYNAMIC ) )
    {
		TRACE0("Failed to create rebar\n");
        return -1;
    }

    if ( !m_wndMenuBar.CreateEx( this,
            TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_CUSTOMERASE, // Toolbar styles
            WS_CHILD | WS_VISIBLE |                                             // Window styles
            CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,    // Control bar styles
            CRect( 0, 0, 0, 0 ), AFX_IDW_MENUBAR ) )
    {
		TRACE0("Failed to create menubar\n");
        return -1;
    }

    if ( !m_wndReBar.AddBar( &m_wndMenuBar, NULL, NULL, RBBS_BREAK | RBBS_USECHEVRON, _T("&Menubar"), false ) )
    {
		TRACE0("Failed to add rebar\n");
        return -1;
    }

    m_wndMenuBar.SetOwner( &m_wndReBar );
    m_wndMenuBar.UpdateMenuBar();

    m_wndReBar.createBands();

    return 0;
}

void CFrameWndBase::OnSysCommand( UINT nID, LPARAM lParam )
{
    if ( !m_wndMenuBar.FrameOnSysCommand( nID, lParam ) )
    {
        CFrameWnd::OnSysCommand( nID, lParam );
    }
}

LRESULT CFrameWndBase::OnMenuChar( UINT nChar, UINT nFlags, CMenu* pMenu )
{
    if ( m_wndMenuBar.FrameOnMenuChar( nChar, nFlags, pMenu ) )
    {
        return MAKELRESULT( 0, 1 );
    }

    return CFrameWnd::OnMenuChar( nChar, nFlags, pMenu );
}

BOOL CFrameWndBase::OnNcActivate( BOOL bActive )
{
    CFrameWnd* pFrame = GetTopLevelFrame();
    ASSERT_VALID( pFrame );

    m_wndMenuBar.FrameOnNcActivate(
        ( bActive || ( pFrame->m_nFlags & WF_STAYACTIVE ) ) && pFrame->IsWindowEnabled() );

    return CFrameWnd::OnNcActivate( bActive );
}

void CFrameWndBase::OnSettingChange( UINT uFlags, LPCTSTR lpszSection )
{
    CWinAppEx::GetInstance()->UpdateSystemParameters();    // reload cached metrics

    CFrameWnd::OnSettingChange( uFlags, lpszSection );
}

void CFrameWndBase::OnClose()
{

    CFrameWnd::OnClose();
}

void CFrameWndBase::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu )
{
    CFrameWnd::OnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );

    m_wndMenuBar.FrameOnInitMenuPopup( pPopupMenu, nIndex, bSysMenu );
}

void CFrameWndBase::OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu )
{
    CFrameWnd::OnMenuSelect( nItemID, nFlags, hSysMenu );

#if _MFC_VER == 0x0700
    m_wndMenuBar.FrameOnMenuSelect( nItemID, nFlags, ( ( CMenu* )hSysMenu )->GetSafeHmenu() );
#else
    m_wndMenuBar.FrameOnMenuSelect( nItemID, nFlags, hSysMenu );
#endif
}

void CFrameWndBase::OnLoadString( int nIndex, CString &strText )
{
    strText = fxfile::theApp.loadString( strText.GetBuffer() );
}

/////////////////////////////////////////////////////////////////////////////
// CFrameWndEx

IMPLEMENT_DYNCREATE( CFrameWndEx, CFrameWnd )

CFrameWndEx::CFrameWndEx()
{
}

CFrameWndEx::~CFrameWndEx()
{
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

void CFrameWndEx::DelayUpdateFrameMenu( HMENU hMenuAlt )
{
    // This function seems to be called by the framework only two times
    // when in-place activation begins and ends.

    if ( m_bInPlace )
    {
        VERIFY( SetMenu( 0 ) );
    }

    m_bInPlace = !m_bInPlace;

    CFrameWnd::DelayUpdateFrameMenu( hMenuAlt );
}

void CFrameWndEx::OnUpdateFrameMenu( HMENU hMenuAlt )
{
    CFrameWndBase::OnUpdateFrameMenu( hMenuAlt );

    //if ( hMenuAlt == 0 )
    //{
    //    // Attempt to get default menu from document
    //    CDocument* pDoc = GetActiveDocument();
    //    if ( pDoc != 0 )
    //    {
    //        hMenuAlt = pDoc->GetDefaultMenu();
    //    }
    //    // Use default menu stored in frame if none from document
    //    if ( hMenuAlt == 0 )
    //    {
    //        hMenuAlt = m_hMenuDefault;
    //    }
    //}

    //// Finally, set the menu
    //if ( m_bInPlace )
    //{
    //    VERIFY( ::SetMenu( m_hWnd, hMenuAlt ) );
    //}
    //else
    //{
    //    VERIFY( m_wndMenuBar.SetMenu( hMenuAlt ) );
    //}
}

/////////////////////////////////////////////////////////////////////////////
// CFrameWndEx message handlers

BEGIN_MESSAGE_MAP(CFrameWndBase, CFrameWnd)
    //{{AFX_MSG_MAP(CFrameWndBase)
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_MENUCHAR()
    ON_WM_NCACTIVATE()
    ON_WM_SETTINGCHANGE()
    ON_WM_CLOSE()
    ON_WM_INITMENUPOPUP()
    ON_WM_MENUSELECT()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFrameWndBase::PreTranslateMessage( MSG* pMsg )
{
    if (m_wndMenuBar.FramePreTranslateMessage( pMsg ))
        return TRUE;

    return CFrameWnd::PreTranslateMessage( pMsg );
}
