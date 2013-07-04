/////////////////////////////////////////////////////////////////////////////
// WinAppEx.h: interface for the CWinAppEx class.
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

#ifndef __WINAPPEX_H__
#define __WINAPPEX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GlobalData.h" // CMapUIntToInt and CMapIntToInt

/////////////////////////////////////////////////////////////////////////////
// CWinAppEx

const CSize szMenuIcon( 16, 16 );

class CWinAppEx : public CWinApp
{
    DECLARE_DYNAMIC( CWinAppEx );

// Enums
public:
    enum EOs
    {
        osWin95,
        osWin98,
        osWinNT,
        osWin2K,
        osWinXP,
        osUnknown,
    };

// Construction
public:
	CWinAppEx( LPCTSTR lpszAppName = 0 );
	~CWinAppEx();

// Operations
public:
    static CWinAppEx* GetInstance();
    static BOOL TrackPopupMenuEx( HMENU hMenu, UINT nFlags,
        int x, int y, CWnd* pWnd, LPTPMPARAMS lptpm = 0 );

// System-wide info
    void UpdateSystemParameters();

    EOs GetOs() const;

    bool IsWin50() const;
    bool IsWinNT() const;
    bool IsWin2K() const;
    bool IsWinXP() const;

    bool GetMenuAnimation() const;
    bool GetMenuUnderlines() const;
    bool GetMenuFade() const;
    bool GetFlatMenu() const;

    const NONCLIENTMETRICS& GetNonClientMetrics() const;

// Menu icon cache related stuff
    void LoadStdProfileSettings( UINT nMaxMRU = _AFX_MRU_COUNT );

    void SetBitmappedMenus( bool bBitmappedMenus );
    bool GetBitmappedMenus() const;

    void AddMenuIcon( CMapUIntToInt& mapIDToImage, UINT nBitmapID, COLORREF clrMask = RGB( 255, 0, 255 ) );
    void AddMenuIcon( UINT nID, HICON hIcon );
    void AddMenuIcon( UINT nID, LPCTSTR lpszFileName );
    void ReplaceMenuIcon( UINT nID, HICON hIcon );
    void RemoveMenuIcon( UINT nID );

    void SetMenuIcons( CMenu* pMenu, bool bSubmenus = false );

    CImageList* GetMenuImageList();
    int GetMenuImage( UINT nID ) const;

// Implementation data
protected:
    EOs                 m_eOs;
    NONCLIENTMETRICS    m_info;
    BOOL                m_bMenuAnimation;   // W98/W2K
    BOOL                m_bMenuUnderlines;  // W98/W2K
    BOOL                m_bMenuFade;        // W2K
    BOOL                m_bUIEffects;       // W2K
    BOOL                m_bFlatMenu;        // XP

    bool                m_bBitmappedMenus;
    CImageList          m_imageList;
    CMapUIntToInt       m_mapIDToImage;
    CMapIntToInt        m_mapFileIcons;
};

/////////////////////////////////////////////////////////////////////////////
// CWinAppEx inline functions

inline CWinAppEx* CWinAppEx::GetInstance()
    { return STATIC_DOWNCAST( CWinAppEx, AfxGetApp() ); }
inline CWinAppEx::EOs CWinAppEx::GetOs() const
    { return m_eOs; }
inline bool CWinAppEx::IsWin50() const
    { return ( m_eOs == osWin98 || m_eOs == osWin2K || m_eOs == osWinXP ); }
inline bool CWinAppEx::IsWinNT() const
    { return ( m_eOs == osWinNT || m_eOs == osWin2K || m_eOs == osWinXP ); }
inline bool CWinAppEx::IsWin2K() const
    { return ( m_eOs == osWin2K || m_eOs == osWinXP ); }
inline bool CWinAppEx::IsWinXP() const
    { return ( m_eOs == osWinXP ); }
inline const NONCLIENTMETRICS& CWinAppEx::GetNonClientMetrics() const
    { return m_info; }

/////////////////////////////////////////////////////////////////////////////
#endif  // !__WINAPPEX_H__
