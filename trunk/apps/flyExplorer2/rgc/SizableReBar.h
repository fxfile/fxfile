/////////////////////////////////////////////////////////////////////////////
// SizableReBar.h: interface for the CSizableReBar class.
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

#ifndef __SIZABLEREBAR_H__
#define __SIZABLEREBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CSizableReBar

#define IDW_SIZABLEREBAR    ( AFX_IDW_CONTROLBAR_LAST - 1 )

class CSizableReBar : public CControlBar
{
    DECLARE_DYNAMIC( CSizableReBar );

// Construction
public:
    CSizableReBar();
    ~CSizableReBar();

// Operations
public:
    void SetEdgeSize( int nEdgeSize );

    bool Create( CWnd* pParentWnd, UINT nID, DWORD dwStyle );
    bool AddBar( CWnd* pBar, LPCTSTR pszText = 0, CBitmap* pbmp = 0,
        DWORD dwStyle = RBBS_FIXEDBMP, LPCTSTR lpszTitle = 0,
        bool bAlwaysVisible = true, bool bHasGripper = true );
    bool AddBar( CWnd* pBar, COLORREF clrFore, COLORREF clrBack, LPCTSTR pszText = 0,
        DWORD dwStyle = 0, LPCTSTR lpszTitle = 0,
        bool bAlwaysVisible = true, bool bHasGripper = true );
    bool SetBkImage( CBitmap* pbmp );

    bool Lock( bool bLock = true );
    bool IsLocked();

    void LoadState( LPCTSTR lpszProfileName );
    void SaveState( LPCTSTR lpszProfileName );

    CReBarCtrl& GetReBarCtrl();

public:
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSizableReBar)
    public:
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHandler );

// Implementation
protected:
    bool DoAddBar( CWnd* pBar, REBARBANDINFO* prbbi, LPCTSTR lpszTitle, bool bAlwaysVisiblem, bool bHasGripper );
    UINT GetSizingEdgeHitCode() const;
    bool GetSizingEdgeRect( const CRect& rcBar, CRect& rcEdge, UINT nHitTest ) const;
    void StartTracking( UINT nHitTest, CPoint pt );
    void StopTracking();
    void OnTrackUpdateSize( CPoint pt );

// Implementation data
protected:
    CReBarCtrl      m_rbCtrl;
    UINT            m_htEdge;
    CSize           m_szOld;
    CPoint          m_ptOld;
    bool            m_bTracking;
    int             m_cxEdge;
    int             m_cyEdge;
    HBITMAP         m_hbmBack;
    bool            m_bLocked;

    struct BARINFO
    {
        UINT    nID;
        TCHAR   szTitle[ MAX_PATH ];
        bool    bAlwaysVisible;
        bool    bHasGripper;
    };

    CArray< BARINFO, BARINFO& > m_aBars;

    class CFrameWndFriend : public CFrameWnd
    {
        friend CSizableReBar;
    };

    static int m_nStateInfoVersion;
    static LPCTSTR m_lpszStateInfoFormat;
    static LPCTSTR m_lpszStateInfoVersion;
    static LPCTSTR m_lpszStateInfoLocked;
    static LPCTSTR m_lpszStateInfoBand;

// Generated message map functions
protected:
    //{{AFX_MSG(CSizableReBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnNcPaint();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    afx_msg void OnRecalcParent();
    DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__SIZABLEREBAR_H__
