/////////////////////////////////////////////////////////////////////////////
// Preview.h: interface for the print preview classes.
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

#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FrameWndEx.h"
#include "ToolBarEx.h"

#include <afxpriv.h>

/////////////////////////////////////////////////////////////////////////////
// CPreviewToolBar

class CPreviewToolBar : public CToolBarEx
{
    DECLARE_DYNCREATE( CPreviewToolBar )

// Overrides
public:
    virtual void Init();
    virtual bool IsTextOptionAvailable( int nTextType ) const;
    virtual bool IsIconOptionAvailable( int nIconType ) const;

protected:
    virtual BOOL HasButtonText( UINT nID );
};

/////////////////////////////////////////////////////////////////////////////
// CPreviewViewEx

class CPreviewViewEx : public CPreviewView
{
	DECLARE_DYNCREATE( CPreviewViewEx )

// Constructors
public:
    CPreviewViewEx();

// Overrides
public:
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

// Implementation
protected:
    class CViewFriend : public CView
    {
        friend CPreviewViewEx;
    };

// Implementation data
protected:
    CSizableReBar*  m_pReBar;
    bool            m_bTempReBar;
    bool            m_bLocked;
    CToolBarEx*     m_pPreviewToolBar;

// Generated message map functions
protected:
    //{{AFX_MSG(CPreviewViewEx)
    afx_msg void OnPreviewClose();
    afx_msg void OnPreviewPrint();
    afx_msg void OnUpdateNumPageChange( CCmdUI* pCmdUI );
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPreviewableView

template< class TBase >
class CPreviewableView : public TBase
{
// Constructors
public:
    CPreviewableView();
    ~CPreviewableView();

// Implementation
protected:
    bool DoPrintPreview( CView* pPrintView, CRuntimeClass* pPreviewToolBarClass,
        CRuntimeClass* pPreviewViewClass, CPrintPreviewState* pState );

    static BOOL CALLBACK PreviewCloseProc( CFrameWnd* pFrameWnd );

// Implementation
protected:
    class CViewFriend : public CView
    {
        friend CPreviewableView< TBase >;
    };

    class CPreviewViewExFriend : public CPreviewViewEx
    {
        friend CPreviewableView< TBase >;
    };

// Message map functions
protected:
    afx_msg void OnFilePrintPreview();
};

/////////////////////////////////////////////////////////////////////////////
// CPreviewableView out-of-line functions

template< class TBase >
CPreviewableView< TBase >::CPreviewableView()
{
}

template< class TBase >
CPreviewableView< TBase >::~CPreviewableView()
{
}

template< class TBase >
void CPreviewableView< TBase >::OnFilePrintPreview()
{
    CPrintPreviewState* pState = new CPrintPreviewState;
    pState->lpfnCloseProc = PreviewCloseProc;

    if ( !DoPrintPreview( this,
            RUNTIME_CLASS( CPreviewToolBar ),
            RUNTIME_CLASS( CPreviewViewEx ), pState ) )
    {
        AfxMessageBox( AFX_IDP_COMMAND_FAILURE );
        delete pState;      // preview failed to initialize, delete State now
    }
}

template< class TBase >
bool CPreviewableView< TBase >::DoPrintPreview( CView* pPrintView,
                                                CRuntimeClass* pPreviewToolBarClass,
                                                CRuntimeClass* pPreviewViewClass,
                                                CPrintPreviewState* pState )
{
    // This code is based on CView::DoPrintPreview().
    // See MFC source code for the original version.

    ASSERT_VALID( pPrintView );
    ASSERT( pState != 0 );

#if _MFC_VER >= 0x0700
    CWnd* pMainWnd = GetParentFrame();
    if ( DYNAMIC_DOWNCAST( CFrameWnd, pMainWnd ) == 0 )
    {
        // If it's not a frame, we'll try the main window
        pMainWnd = AfxGetMainWnd();
    }

    CFrameWnd* pParent = STATIC_DOWNCAST( CFrameWnd, pMainWnd );
    ASSERT_VALID( pParent );
#else
    CFrameWnd* pParent = STATIC_DOWNCAST( CFrameWnd, AfxGetMainWnd() );
    ASSERT_VALID( pParent );
#endif

    CCreateContext context;
    context.m_pCurrentFrame = pParent;
    context.m_pCurrentDoc   = CView::GetDocument();
    context.m_pLastView     = this;

    // Create the preview view object
    CPreviewViewExFriend* pView = ( CPreviewViewExFriend* )pPreviewViewClass->CreateObject();
    ASSERT( pView != 0 );
    ASSERT_KINDOF( CPreviewViewEx, pView );

    pView->m_pToolBar = 0;

    CWnd* pWnd = pParent->GetDlgItem( IDW_SIZABLEREBAR );
    if ( pWnd != 0 )
    {
        if ( !pWnd->IsKindOf( RUNTIME_CLASS( CSizableReBar ) ) )
        {
            ASSERT( false );
            delete pView;
            return false;
        }

        pView->m_pReBar = ( CSizableReBar* )pWnd;
        pView->m_bTempReBar = false;
    }
    else
    {
        pView->m_pReBar = new CSizableReBar;
        pView->m_bTempReBar = true;

        if ( !pView->m_pReBar->Create( pParent, IDW_SIZABLEREBAR,
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
                CBRS_ALIGN_TOP | CBRS_SIZE_DYNAMIC ) )
        {
            delete pView->m_pReBar;
            pView->m_pReBar = 0;
            delete pView;
            return false;
        }

        pView->m_pReBar->m_bAutoDelete = TRUE;
    }

    pView->m_pPreviewState = pState;    // save pointer

    // Take over Frame Window
    pParent->OnSetPreviewMode( TRUE, pState );

    // Create print preview toolbar
    pView->m_pPreviewToolBar = ( CToolBarEx* )pPreviewToolBarClass->CreateObject();
    ASSERT( pView->m_pPreviewToolBar != 0 );
    ASSERT_KINDOF( CToolBarEx, pView->m_pPreviewToolBar );

    if ( !pView->m_pPreviewToolBar->Create( pView->m_pReBar,
            WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_FLYBY,
            AFX_IDW_PREVIEW_BAR ) )
    {
        pParent->OnSetPreviewMode( FALSE, pState ); // restore Frame Window
        delete pView->m_pPreviewToolBar;            // not autodestruct yet
        pView->m_pPreviewToolBar = 0;
        pView->m_pPreviewState   = 0;               // do not delete state structure
        delete pView;
        return false;
    }

    VERIFY( pView->m_pReBar->AddBar( pView->m_pPreviewToolBar, 0, 0, RBBS_FIXEDBMP ) );

    pView->m_bLocked = pView->m_pReBar->Lock();
    pView->m_pReBar->EnableContextMenu( false );

    pView->m_pPreviewToolBar->m_bAutoDelete = TRUE;    // automatic cleanup
    pView->m_pPreviewToolBar->ModifyStyle( 0, CCS_ADJUSTABLE );
    pView->m_pPreviewToolBar->Init();

    // Create the preview view as a child of the App Main Window.  This
    // is a sibling of this view if this is an SDI app.  This is NOT a sibling
    // if this is an MDI app.
    if ( !pView->Create( 0, 0, AFX_WS_DEFAULT_VIEW,
            CRect( 0, 0, 0, 0 ), pParent, AFX_IDW_PANE_FIRST, &context ) )
    {
        pParent->OnSetPreviewMode( FALSE, pState ); // restore Frame Window
        pView->m_pPreviewState = 0;     // do not delete state structure
        delete pView;
        return false;
    }

    // Preview window shown now
    pState->pViewActiveOld = pParent->GetActiveView();
    CView* pActiveView = pParent->GetActiveFrame()->GetActiveView();
    if ( pActiveView != 0 )
    {
        ( ( CViewFriend* )pActiveView )->OnActivateView( FALSE, pActiveView, pActiveView );
    }

    if ( !pView->SetPrintView( pPrintView ) )
    {
        pView->OnPreviewClose();
        return true;            // signal that OnEndPrintPreview was called
    }

    pParent->SetActiveView( pView );    // set active view - even for MDI

    // Update toolbar and redraw everything
    pView->m_pPreviewToolBar->SendMessage( WM_IDLEUPDATECMDUI, ( WPARAM )TRUE );
    pParent->RecalcLayout();            // position and size everything
    pParent->UpdateWindow();

    return true;
}

template< class TBase >
BOOL CALLBACK CPreviewableView< TBase >::PreviewCloseProc( CFrameWnd* pFrameWnd )
{
    ASSERT_VALID( pFrameWnd );

    CPreviewViewExFriend* pView = ( CPreviewViewExFriend* )pFrameWnd->GetDlgItem( AFX_IDW_PANE_FIRST );
    ASSERT_KINDOF( CPreviewViewEx, pView );

    pView->OnPreviewClose();
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
#endif  // !__PREVIEW_H__
