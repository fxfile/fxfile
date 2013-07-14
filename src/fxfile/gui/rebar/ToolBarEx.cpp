/////////////////////////////////////////////////////////////////////////////
// ToolBarEx.cpp: implementation of the CToolBarEx class.
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
#include "Resource.h"
#include "CustomizeDialog.h"
#include "SizableReBar.h"
#include "ToolBarEx.h"
#include "WinAppEx.h"

#include <afxpriv.h>    // ON_MESSAGE_VOID()

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

LPCTSTR             CToolBarEx::m_lpszStateInfoEntry = _T("ToolbarStateInfo");
CToolBarEx*         CToolBarEx::m_pToolBar           = 0;
CCustomizeDialog*   CToolBarEx::m_pCustomizeDlg      = 0;
HHOOK               CToolBarEx::m_hCBTHook           = 0;

IMPLEMENT_DYNAMIC( CToolBarEx, CToolBar )

CToolBarEx::CToolBarEx()
{
    m_pObserver = NULL;

    m_szLargeIcon = CSize(22,22);
    m_szSmallIcon = CSize(16,16);
    m_clrMask = RGB(255,0,255);

    m_nTextType = TBT_RIGHT;
    m_nIconType = TBI_SMALL;
    m_nTextTypeDefault = TBT_RIGHT;
    m_nIconTypeDefault = TBI_SMALL;
}

CToolBarEx::~CToolBarEx()
{
    m_dqButtons.clear();
    m_mpButtons.clear();
}

void CToolBarEx::SetObserver(Observer *pObserver)
{
    m_pObserver = pObserver;
}

BOOL CToolBarEx::Create( CWnd* pParentWnd,
                         DWORD dwStyle,
                         UINT nID /*=AFX_IDW_TOOLBAR*/,
                         DWORD dwExtendedStyle /*= TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS*/ )
{
    dwStyle &= ~CBRS_TOOLTIPS;
    DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
    if (!CreateEx(pParentWnd, dwCtrlStyle, dwStyle, CRect(0,0,0,0), nID))
        return FALSE;

    GetToolBarCtrl().SetExtendedStyle(dwExtendedStyle);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Attributes

void CToolBarEx::SetTextType( int nTextType, bool bUpdate /*=true*/ )
{
    ASSERT( ::IsWindow( m_hWnd ) );
    ASSERT( GetStyle() & TBSTYLE_TOOLTIPS );
    ASSERT( !( GetBarStyle() & CBRS_TOOLTIPS ) );
    ASSERT( IsTextTypeAvailable( nTextType ) );

    m_nTextType = nTextType;

    // Modify toolbar style according to new text options
    ModifyStyle(
        ( nTextType == TBT_RIGHT ) ? 0 : TBSTYLE_LIST,
        ( nTextType == TBT_RIGHT ) ? TBSTYLE_LIST : 0 );

    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    DWORD dwStyleEx = tbCtrl.GetExtendedStyle();
    tbCtrl.SetExtendedStyle(
        ( nTextType == TBT_RIGHT ) ?
            ( dwStyleEx |  TBSTYLE_EX_MIXEDBUTTONS ) :
            ( dwStyleEx & ~TBSTYLE_EX_MIXEDBUTTONS ) );
    VERIFY( tbCtrl.SetMaxTextRows(
        ( nTextType == TBT_NONE ) ? 0 : 1 ) );

    if ( nTextType == TBT_TEXT )
    {
        VERIFY( tbCtrl.SetButtonWidth( 0, 76 ) );
    }

    // Modify all (even currently hidden ones) buttons in internal cache
	TBBUTTONEX *sTbButtonEx;
	ButtonDeque::iterator sIterator;

	sIterator = m_dqButtons.begin();
	for (; sIterator != m_dqButtons.end(); ++sIterator)
	{
		sTbButtonEx = *sIterator;

        if ( !( sTbButtonEx->tbinfo.fsStyle & TBSTYLE_SEP ) )
        {
            CString strButtonText;
            GetButtonTextByCommand( sTbButtonEx->tbinfo.idCommand, strButtonText );
            CString strToAdd( strButtonText, strButtonText.GetLength() + 1 );
            sTbButtonEx->tbinfo.iString = tbCtrl.AddStrings( strToAdd );

            switch ( nTextType )
            {
                case TBT_TEXT:
                    sTbButtonEx->tbinfo.fsStyle &= ~( TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT );
                    break;

                case TBT_RIGHT:
                    sTbButtonEx->tbinfo.fsStyle |= ( TBSTYLE_AUTOSIZE |
                        ( HasButtonText( sTbButtonEx->tbinfo.idCommand ) ? BTNS_SHOWTEXT : 0 ) );
                    break;

                case TBT_NONE:
                    sTbButtonEx->tbinfo.fsStyle &= ~BTNS_SHOWTEXT;
                    sTbButtonEx->tbinfo.fsStyle |= TBSTYLE_AUTOSIZE;
                    break;
            }
        }
    }

    // If requested, reflect changes immediately
    if ( bUpdate )
    {
        ReloadButtons();
        UpdateToolbarSize();
    }
}

int CToolBarEx::GetTextType() const
{
    return m_nTextType;
}

void CToolBarEx::SetIconType( int nIconType, bool bUpdate /*=true*/ )
{
    ASSERT( ::IsWindow( m_hWnd ) );
    ASSERT( IsIconTypeAvailable( nIconType ) );

    m_nIconType = nIconType;

    // Set image list(s) and icon size for selected icon options
    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    tbCtrl.SetImageList( 0 );
    tbCtrl.SetHotImageList( 0 );
    tbCtrl.SetDisabledImageList( 0 );

    CSize szIcon = ( nIconType == TBI_SMALL ) ? m_szSmallIcon : m_szLargeIcon;
    VERIFY( tbCtrl.SetBitmapSize( szIcon ) );

    UINT nIDCold     = ( nIconType == TBI_SMALL ) ? m_nIDSmallCold     : m_nIDLargeCold;
    UINT nIDHot      = ( nIconType == TBI_SMALL ) ? m_nIDSmallHot      : m_nIDLargeHot;
    UINT nIDDisabled = ( nIconType == TBI_SMALL ) ? m_nIDSmallDisabled : m_nIDLargeDisabled;
    ASSERT( nIDCold != ( UINT )-1 );    // at least there must be "cold" imagelist

    LPCTSTR lpResName = MAKEINTRESOURCE( nIDCold );
    HINSTANCE hInst = AfxFindResourceHandle( lpResName, RT_BITMAP );

    m_imageListCold.DeleteImageList();
    VERIFY( m_imageListCold.Attach( ImageList_LoadImage( hInst, lpResName,
        szIcon.cx, 0, m_clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION ) ) );
    tbCtrl.SetImageList( &m_imageListCold );

    if ( nIDHot != ( UINT )-1 ) // "hot" imagelist is optional
    {
        lpResName = MAKEINTRESOURCE( nIDHot );
        hInst = AfxFindResourceHandle( lpResName, RT_BITMAP );

        m_imageListHot.DeleteImageList();
        VERIFY( m_imageListHot.Attach( ImageList_LoadImage( hInst, lpResName,
            szIcon.cx, 0, m_clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION ) ) );
        tbCtrl.SetHotImageList( &m_imageListHot );
    }

    if ( nIDDisabled != ( UINT )-1 ) // "disabled" imagelist is optional
    {
        lpResName = MAKEINTRESOURCE( nIDDisabled );
        hInst = AfxFindResourceHandle( lpResName, RT_BITMAP );

        m_imageListDisabled.DeleteImageList();
        VERIFY( m_imageListDisabled.Attach( ImageList_LoadImage( hInst, lpResName,
            szIcon.cx, 0, m_clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION ) ) );
        tbCtrl.SetDisabledImageList( &m_imageListDisabled );
    }

    // If requested, reflect changes immediately
    if ( bUpdate )
    {
        ReloadButtons();
        UpdateToolbarSize();
    }
}

int CToolBarEx::GetIconType() const
{
    return m_nIconType;
}

/////////////////////////////////////////////////////////////////////////////
// Operations

void CToolBarEx::SetIconSize(CSize szLargeIcon, CSize szSmallIcon)
{
    m_szLargeIcon = szLargeIcon;
    m_szSmallIcon = szSmallIcon;
}

void CToolBarEx::SetImageList(UINT nIDSmallDisabled, UINT nIDSmallCold, UINT nIDSmallHot,
                              UINT nIDLargeDisabled, UINT nIDLargeCold, UINT nIDLargeHot,
                              int nIconTypeDefault, COLORREF clrMask /*= RGB(255,0,255)*/)
{
    m_nIDSmallCold     = nIDSmallCold;
    m_nIDSmallHot      = nIDSmallHot;
    m_nIDSmallDisabled = nIDSmallDisabled;
    m_nIDLargeCold     = nIDLargeCold;
    m_nIDLargeHot      = nIDLargeHot;
    m_nIDLargeDisabled = nIDLargeDisabled;
    m_clrMask          = clrMask;

    m_nIconTypeDefault = nIconTypeDefault;    // to be used on reset
    SetIconType( m_nIconTypeDefault );        // apply new options
}

void CToolBarEx::GetImageList(CImageList *pImgList, BOOL bSmall/* = TRUE*/, int nImageType/* = TBG_HOT*/)
{
    UINT nID = bSmall ? m_nIDSmallHot : m_nIDLargeHot;
    CSize szIcon = bSmall ? m_szSmallIcon : m_szLargeIcon;

    HIMAGELIST hImageList;
    hImageList = ImageList_LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nID),
        szIcon.cx, 0, m_clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION);

    if (hImageList)
    {
        if (pImgList->m_hImageList)
            pImgList->DeleteImageList();

        pImgList->Attach(hImageList);
    }
}

void CToolBarEx::SetDefaultButtons()
{
    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    while (tbCtrl.DeleteButton(0));

    TBBUTTONEX *sTbButtonEx;
    ButtonDeque::iterator sIterator;

    sIterator = m_dqButtons.begin();
    for (; sIterator!=m_dqButtons.end(); ++sIterator)
    {
        sTbButtonEx = *sIterator;

        if (sTbButtonEx->bInitiallyVisible)
            tbCtrl.AddButtons(1, &sTbButtonEx->tbinfo);
    }

    SetTextType(TBT_RIGHT);
}

void CToolBarEx::SetButtons(int nCount, TBBUTTONEX* lpButtons)
{
    m_dqButtons.clear();
    m_mpButtons.clear();

    int i;
    for (i = 0; i < nCount; ++i)
    {
        m_dqButtons.push_back(&lpButtons[i]);
        m_mpButtons[lpButtons[i].tbinfo.idCommand] = &lpButtons[i];
    }
}

void CToolBarEx::LoadState( LPCTSTR lpszProfileName )
{
    CString strSubKey;
    strSubKey.Format( _T("Software\\%s\\%s\\%s"),
        AfxGetApp()->m_pszRegistryKey,
        AfxGetApp()->m_pszProfileName,
        lpszProfileName );

    GetToolBarCtrl().RestoreState( HKEY_CURRENT_USER,
        strSubKey, m_lpszStateInfoEntry );

// It was found out that TB_SAVERESTORE causes TBN_BEGINADJUST
// and TBN_ENDADJUST to be sent correspondingly at the beggining
// and the end of save/restore process.  So, the following
// call is redundant and therefore was commented out.

//    UpdateToolbarSize();
}

void CToolBarEx::SaveState( LPCTSTR lpszProfileName )
{
    CString strSubKey;
    strSubKey.Format( _T("Software\\%s\\%s\\%s"),
        AfxGetApp()->m_pszRegistryKey,
        AfxGetApp()->m_pszProfileName,
        lpszProfileName );

    m_hToolbarData = 0;
    GetToolBarCtrl().SaveState( HKEY_CURRENT_USER,
        strSubKey, m_lpszStateInfoEntry );
    if ( m_hToolbarData != 0 )
    {
        ::GlobalFree( m_hToolbarData );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

LRESULT CToolBarEx::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    // Special handling for certain messages (forwarding to owner/parent)
    switch ( message )
    {
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
            {
                UpdateToolbarSize();
                break;
            }
    }

    return CToolBar::WindowProc( message, wParam, lParam );
}

BOOL CToolBarEx::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
    NMHDR* pNMHDR = ( NMHDR* )lParam;
    if ( ( pNMHDR->code == TTN_NEEDTEXTA ) ||
         ( pNMHDR->code == TTN_NEEDTEXTW ) )
    {
        // If button doesn't have a tip, ignore notification
        *pResult = HasButtonTip( (UINT)pNMHDR->idFrom ) ? Default() : 0;
        return TRUE;
    }

    return CToolBar::OnNotify( wParam, lParam, pResult );
}

//LRESULT CToolBarEx::DoCustomDraw( NMHDR* pNMHDR, CWnd* /*pToolBar*/ )
//{
//    LPNMTBCUSTOMDRAW lpNMCustomDraw = ( LPNMTBCUSTOMDRAW )pNMHDR;
//
//    switch ( lpNMCustomDraw->nmcd.dwDrawStage )
//    {
//        case CDDS_PREPAINT:
//            return CDRF_NOTIFYITEMDRAW;
//
//        case CDDS_ITEMPREPAINT:
//        {
//            UINT uItemState = lpNMCustomDraw->nmcd.uItemState;
//            if ( ( uItemState & CDIS_CHECKED ) && ( uItemState & CDIS_HOT ) )
//            {
//                // I personally don't like when background of checked item
//                // which is currently hot is drawn using dither brush
//                //lpNMCustomDraw->hbrMonoDither = 0;
//            }
//            // fall through...
//        }
//        default:
//            return CDRF_DODEFAULT;
//    }
//}

BOOL CToolBarEx::HasButtonText( UINT /*nID*/ )
{
    return TRUE;
}

BOOL CToolBarEx::HasButtonTip( UINT nID )
{
    switch ( m_nTextType )
    {
        case TBT_TEXT:
            return FALSE;
        case TBT_RIGHT:
            return !HasButtonText( nID );
        case TBT_NONE:
            return TRUE;
        default:
            ASSERT( FALSE );
            return FALSE;
    }
}

void CToolBarEx::GetButtonTextByCommand( UINT nID, CString& strText )
{
    CString strFull;
    VERIFY( strFull.LoadString( nID ) );
    VERIFY( AfxExtractSubString( strText, strFull, 1, _T('\n') ) );
}

void CToolBarEx::GetButtonTip( UINT nID, CString& strTip )
{
    GetButtonTextByCommand( nID, strTip );
}

void CToolBarEx::Init()
{
    ASSERT( false );    // must be overridden
}

bool CToolBarEx::IsTextTypeAvailable( int /*nTextType*/ ) const
{
    return true;
}

bool CToolBarEx::IsIconTypeAvailable( int /*nIconType*/ ) const
{
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

void CToolBarEx::ReloadButtons()
{
    // Reload buttons from internal cache
    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    for ( int nIndex = 0, nButtons = tbCtrl.GetButtonCount(); nIndex < nButtons; nIndex++ )
    {
        TBBUTTON tbinfo;
        VERIFY( tbCtrl.GetButton( 0, &tbinfo ) );
        VERIFY( GetButtonInfo( tbinfo.idCommand, tbinfo ) );
        VERIFY( tbCtrl.DeleteButton( 0 ) );
        VERIFY( tbCtrl.AddButtons( 1, &tbinfo ) );
    }
}

void CToolBarEx::UpdateToolbarSize()
{
    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    tbCtrl.AutoSize();

    if (m_pObserver != NULL)
        m_pObserver->onUpdatedToolbarSize(*this);

    Invalidate();   // visual feedback
}

BOOL CToolBarEx::GetButtonInfo( UINT nID, TBBUTTON& tbinfo )
{
    if ( tbinfo.fsStyle & TBSTYLE_SEP )
    {
        return TRUE;
    }

    ButtonMap::iterator sIterator = m_mpButtons.find(nID);
    if (sIterator == m_mpButtons.end())
        return FALSE;

    tbinfo = sIterator->second->tbinfo;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// CToolBarEx message handlers

BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
    //{{AFX_MSG_MAP(CToolBarEx)
    //}}AFX_MSG_MAP

    ON_MESSAGE( WM_REBAR_CHEVRONPUSHED, OnReBarChevronPushed )

    ON_NOTIFY_REFLECT( TBN_GETINFOTIP, OnGetInfoTip )

    // Toolbar customization
    ON_NOTIFY_REFLECT( TBN_BEGINADJUST, OnBeginAdjust )
    ON_NOTIFY_REFLECT( TBN_ENDADJUST, OnEndAdjust )
    ON_NOTIFY_REFLECT( TBN_QUERYINSERT, OnQueryInsert )
    ON_NOTIFY_REFLECT( TBN_QUERYDELETE, OnQueryDelete )
    ON_NOTIFY_REFLECT( TBN_INITCUSTOMIZE, OnInitCustomize )
    ON_NOTIFY_REFLECT( TBN_GETBUTTONINFO, OnGetButtonInfo )
    ON_NOTIFY_REFLECT( TBN_RESET, OnReset )
    ON_NOTIFY_REFLECT( TBN_TOOLBARCHANGE, OnToolBarChange )

    // Saving and restoring toolbar
    ON_NOTIFY_REFLECT( TBN_SAVE, OnSave )
    ON_NOTIFY_REFLECT( TBN_RESTORE, OnRestore )
END_MESSAGE_MAP()

void CToolBarEx::Customize()
{
    GetToolBarCtrl().Customize();
}

LRESULT CToolBarEx::OnReBarChevronPushed( WPARAM wParam, LPARAM /*lParam*/ )
{
    CRect rcChevron( ( LPCRECT )wParam );

    CToolBarPopup menu( this );
    menu.ShowPopup( TPM_LEFTALIGN | TPM_VERTICAL |
            ( CWinAppEx::GetInstance()->IsWin50() ? TPM_VERPOSANIMATION : 0 ),
        CPoint( rcChevron.left, rcChevron.bottom ), rcChevron );

    return 0L;
}

void CToolBarEx::OnGetInfoTip( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTBGETINFOTIP* lptbgit = ( NMTBGETINFOTIP* )pNMHDR;
    CString strTip;
    GetButtonTip( lptbgit->iItem, strTip );
    _tcsncpy( lptbgit->pszText, strTip, lptbgit->cchTextMax );

    *pResult = 0;
}

LRESULT CALLBACK CToolBarEx::CBTProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    ASSERT( m_pToolBar != 0 );
    HWND hWnd = ( HWND )wParam;

    switch ( nCode )
    {
        case HCBT_CREATEWND:
            if ( m_pCustomizeDlg == 0 )
            {
                // This is where we have a good chance to subclass standard
                // "Customize toolbar" dialog in order to add to it some of
                // our new features (ie. text and icon options selectors)
                TCHAR szClassName[ 7 ];
                ::GetClassName( hWnd, szClassName, sizeof( szClassName ) / sizeof( szClassName[ 0 ] ) );
                if ( !lstrcmp( _T("#32770"), szClassName ) )
                {
                    m_pCustomizeDlg = new CCustomizeDialog( m_pToolBar );
                    VERIFY( m_pCustomizeDlg->SubclassWindow( hWnd ) );
                    return 0;
                }
            }
            break;

        case HCBT_DESTROYWND:
            if ( ( m_pCustomizeDlg != 0 ) && ( m_pCustomizeDlg->m_hWnd == hWnd ) )
            {
                m_pCustomizeDlg = 0;
                return 0;
            }
            break;
    }

    return ::CallNextHookEx( m_hCBTHook, nCode, wParam, lParam );
}

void CToolBarEx::OnBeginAdjust( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    m_pToolBar = this;
    m_hCBTHook = ::SetWindowsHookEx( WH_CBT, CBTProc, 0, ::GetCurrentThreadId() );
    ASSERT( m_hCBTHook != 0 );

    *pResult = 0;
}

void CToolBarEx::OnEndAdjust( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    VERIFY( ::UnhookWindowsHookEx( m_hCBTHook ) );
    m_hCBTHook = 0;
    m_pToolBar = 0;

    ReloadButtons();
    UpdateToolbarSize();

    *pResult = 0;
}

void CToolBarEx::OnQueryInsert( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    *pResult = TRUE;    // otherwise Customize dialog will not be shown
}

void CToolBarEx::OnQueryDelete( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    *pResult = TRUE;    // why not? :)
}

void CToolBarEx::OnInitCustomize( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    *pResult = TBNRF_HIDEHELP;      // no help available yet
}

void CToolBarEx::OnGetButtonInfo( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTOOLBAR* lpnmtb = ( NMTOOLBAR* )pNMHDR;
    *pResult = FALSE;

    int nItem = lpnmtb->iItem;
    if (!FXFILE_STL_IS_INDEXABLE(nItem, m_dqButtons))
        return;

    lpnmtb->tbButton = m_dqButtons[nItem]->tbinfo;
    if (!XPR_TEST_BITS(lpnmtb->tbButton.fsStyle, TBSTYLE_SEP))
    {
        CString strText;
        ButtonMap::iterator sIterator;

        sIterator = m_mpButtons.find(lpnmtb->tbButton.idCommand);
        if (sIterator != m_mpButtons.end())
            strText.LoadString((UINT)sIterator->second->tbinfo.iString);

        _tcscpy(lpnmtb->pszText, strText);
    }

    *pResult = TRUE;
}

void CToolBarEx::OnReset( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    // Delete all buttons
    CToolBarCtrl& tbCtrl = GetToolBarCtrl();
    while ( tbCtrl.DeleteButton( 0 ) );

    // Load buttons from internal cache
    TBBUTTONEX *sTbButtonEx;
    ButtonDeque::iterator sIterator;

    sIterator = m_dqButtons.begin();
    for (; sIterator != m_dqButtons.end(); ++sIterator)
    {
        sTbButtonEx = *sIterator;

        if (XPR_IS_TRUE(sTbButtonEx->bInitiallyVisible))
            tbCtrl.AddButtons(1, &sTbButtonEx->tbinfo);
    }

    // Set default text & icon options
    SetIconType( m_nIconTypeDefault, false );
    SetTextType( m_nTextTypeDefault, false );
    ReloadButtons();
    UpdateToolbarSize();

    // Synchronize with Customize dialog
    ASSERT( m_pCustomizeDlg != 0 );
    m_pCustomizeDlg->SetTextType( m_nIconTypeDefault, true );
    m_pCustomizeDlg->SetIconType( m_nTextTypeDefault, true );

    *pResult = 0;
}

void CToolBarEx::OnToolBarChange( NMHDR* /*pNMHDR*/, LRESULT* pResult )
{
    UpdateToolbarSize();

    *pResult = 0;
}

void CToolBarEx::OnSave( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTBSAVE* lpnmtb = ( NMTBSAVE* )pNMHDR;
    if ( lpnmtb->iItem == -1 )
    {
        lpnmtb->cbData += sizeof( DWORD ) * 2;

        m_hToolbarData = ::GlobalAlloc( GMEM_FIXED, lpnmtb->cbData );
        ASSERT( m_hToolbarData != 0 );

        lpnmtb->pData    = ( LPDWORD )m_hToolbarData;
        lpnmtb->pCurrent = lpnmtb->pData;

        *lpnmtb->pCurrent++ = ( DWORD )m_nTextType;
        *lpnmtb->pCurrent++ = ( DWORD )m_nIconType;
    }

    *pResult = 0;
}

void CToolBarEx::OnRestore( NMHDR* pNMHDR, LRESULT* pResult )
{
    NMTBRESTORE* lpnmtb = ( NMTBRESTORE* )pNMHDR;
    if ( lpnmtb->iItem == -1 )
    {
        lpnmtb->cButtons = ( lpnmtb->cbData - sizeof( DWORD ) * 2 ) / lpnmtb->cbBytesPerRecord;
        lpnmtb->pCurrent = lpnmtb->pData;

        SetTextType( ( int )*lpnmtb->pCurrent++, false );
        SetIconType( ( int )*lpnmtb->pCurrent++, false );

        if ( lpnmtb->cButtons == 0 )
        {
            ReloadButtons();
            UpdateToolbarSize();

            *pResult = 1;
            return;
        }
    }
    else
    {
        VERIFY( GetButtonInfo( lpnmtb->tbButton.idCommand, lpnmtb->tbButton ) );
    }

    *pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CToolBarPopup

HHOOK           CToolBarPopup::m_hKeyboardHook  = 0;
CToolBarPopup*  CToolBarPopup::m_pPopup         = 0;

IMPLEMENT_DYNAMIC( CToolBarPopup, CWnd )

CToolBarPopup::CToolBarPopup( CToolBarEx* pToolBar )
{
    ASSERT_VALID( pToolBar );

    m_pToolBar    = pToolBar;
    m_bOverTbCtrl = false;
    m_bTextLabels = false;

    CWinAppEx* pApp = CWinAppEx::GetInstance();
    m_bFlatMenu = ( pApp->IsWinXP() && pApp->GetFlatMenu() );
}

CToolBarPopup::~CToolBarPopup()
{
}

/////////////////////////////////////////////////////////////////////////////
// Operations

bool CToolBarPopup::ShowPopup( UINT nFlags, CPoint pt, CRect& rcExclude )
{
    CWinAppEx* pApp = CWinAppEx::GetInstance();
    CString strWndClass = AfxRegisterWndClass( CS_SAVEBITS | ( pApp->IsWinXP() ? CS_DROPSHADOW : 0 ),
        ::LoadCursor( 0, IDC_ARROW ), ( HBRUSH )( COLOR_MENU + 1 ), 0 );

    if ( !CreateEx( m_bFlatMenu ? ( WS_EX_TOOLWINDOW | WS_EX_TOPMOST ) : WS_EX_PALETTEWINDOW, strWndClass, 0,
            WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED | ( m_bFlatMenu ? WS_BORDER : ( WS_DLGFRAME | WS_CLIPCHILDREN ) ),
            CRect( 0, 0, 0, 0 ), 0, 0 ) )
    {
        return false;
    }

    SetPosition( nFlags, pt, rcExclude );
    SetCapture();
    SendMessage( WM_SETCURSOR, ( WPARAM )m_hWnd, MAKELPARAM( HTCLIENT, 0 ) );

    m_pPopup = this;
    m_hKeyboardHook = ::SetWindowsHookEx( WH_KEYBOARD, KeyboardProc, 0, ::GetCurrentThreadId() );
    ASSERT( m_hKeyboardHook != 0 );

    // Emulate menu loop
    RunModalLoop( MLF_NOKICKIDLE );

    VERIFY( ::UnhookWindowsHookEx( m_hKeyboardHook ) );
    m_hKeyboardHook = 0;
    m_pPopup = 0;

    DestroyWindow();

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

void CToolBarPopup::SetPosition( UINT nFlags, CPoint pt, CRect& rcExclude )
{
    // The main purpose of this functions is to find proper position of
    // the popup window, so that we neither exceed screen dimensions nor
    // intersect rcExclude rectangle.  The function also takes into account
    // control flags specified by the user.

    // Calc initial position
    CRect rc;
    m_tbCtrl.GetWindowRect( rc );

    CalcWindowRect( rc, CWnd::adjustBorder );
    rc.OffsetRect( -rc.TopLeft() );
    rc.OffsetRect(
        ( nFlags & TPM_CENTERALIGN ) ? ( pt.x - rc.Width() / 2 ) :
        ( nFlags & TPM_RIGHTALIGN  ) ? ( pt.x - rc.Width() ) : pt.x, 0 );
    rc.OffsetRect( 0,
        ( nFlags & TPM_VCENTERALIGN ) ? ( pt.y - rc.Height() / 2 ) :
        ( nFlags & TPM_BOTTOMALIGN  ) ? ( pt.y - rc.Height() ) : pt.y );

    // Make sure we don't exceed screen dimensions
    CRect rcDesktop;
    GetDesktopWindow()->GetWindowRect( rcDesktop );

    rc.OffsetRect(
        min( rcDesktop.right  - rc.right,  0 ),
        min( rcDesktop.bottom - rc.bottom, 0 ) );
    rc.OffsetRect(
        max( rcDesktop.left   - rc.left,   0 ),
        max( rcDesktop.top    - rc.top,    0 ) );

    // Make sure we don't intersect rcExclude rectangle
    CRect rcTemp;
    if ( !rcExclude.IsRectEmpty() && rcTemp.IntersectRect( rc, rcExclude ) )
    {
        if ( nFlags & TPM_VERTICAL )
        {
            CRect rcUp( rc );
            int nUp = rc.bottom - rcExclude.top;
            rcUp.OffsetRect( 0, -nUp );

            CRect rcDown( rc );
            int nDown = rcExclude.bottom - rc.top;
            rcDown.OffsetRect( 0, nDown );

            bool bUp = false;
            if ( ( rcUp.top >= rcDesktop.top ) && ( rcDown.bottom <= rcDesktop.bottom ) )
            {
                bUp = ( nUp < nDown );
            }
            else if ( rcUp.top >= rcDesktop.top )
            {
                bUp = true;
            }
            else if ( rcDown.bottom <= rcDesktop.bottom )
            {
                bUp = false;
            }
            else
            {
                ASSERT( false );    // this case is not supported yet
            }

            rc = bUp ? rcUp : rcDown;
            nFlags &= ~( TPM_HORPOSANIMATION | TPM_HORNEGANIMATION | TPM_VERPOSANIMATION | TPM_VERNEGANIMATION );
            nFlags |=  ( bUp ? TPM_VERNEGANIMATION : TPM_VERPOSANIMATION );
        }
        else
        {
            CRect rcLeft( rc );
            int nLeft = rc.right - rcExclude.left;
            rcLeft.OffsetRect( -nLeft, 0 );

            CRect rcRight( rc );
            int nRight = rcExclude.right - rc.left;
            rcRight.OffsetRect( nRight, 0 );

            bool bLeft = false;
            if ( ( rcLeft.left >= rcDesktop.top ) && ( rcRight.right <= rcDesktop.right ) )
            {
                bLeft = ( nLeft < nRight );
            }
            else if ( rcLeft.left >= rcDesktop.left )
            {
                bLeft = true;
            }
            else if ( rcRight.right <= rcDesktop.right )
            {
                bLeft = false;
            }
            else
            {
                ASSERT( false );    // this case is not supported yet
            }

            rc = bLeft ? rcLeft : rcRight;
            nFlags &= ~( TPM_HORPOSANIMATION | TPM_HORNEGANIMATION | TPM_VERPOSANIMATION | TPM_VERNEGANIMATION );
            nFlags |=  ( bLeft ? TPM_HORNEGANIMATION : TPM_HORPOSANIMATION );
        }
    }

    Show( nFlags, rc );
}

void CToolBarPopup::Show( UINT nFlags, const CRect& rc )
{
    // On W98/W2K systems the menu animation feature is supported.
    // This function tries to mimic this feature conformably to
    // toolbar popup window.

    // Position window on the screen
    SetWindowPos( &wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(),
        SWP_NOACTIVATE );

    CWinAppEx* pApp = CWinAppEx::GetInstance();
    if ( pApp->IsWin50() && !( nFlags & TPM_NOANIMATION ) ) // W98/W2K specific (menu animation)
    {
        if ( pApp->GetMenuAnimation() )
        {
            DWORD dwAnimationFlags = AW_SLIDE |
                ( ( nFlags & TPM_HORPOSANIMATION ) ? AW_HOR_POSITIVE : 0 ) |
                ( ( nFlags & TPM_HORNEGANIMATION ) ? AW_HOR_NEGATIVE : 0 ) |
                ( ( nFlags & TPM_VERPOSANIMATION ) ? AW_VER_POSITIVE : 0 ) |
                ( ( nFlags & TPM_VERNEGANIMATION ) ? AW_VER_NEGATIVE : 0 );
            if ( dwAnimationFlags == AW_SLIDE )
            {
                // If none of TPM_*ANIMATION flags is set, set default animation
                dwAnimationFlags |= AW_HOR_POSITIVE | AW_VER_POSITIVE;
            }

            if ( pApp->IsWin2K() && pApp->GetMenuFade() )   // W2K specific (fade effect)
            {
                dwAnimationFlags = AW_BLEND;
            }

            VERIFY( ::AnimateWindowWin50( m_hWnd, 200, dwAnimationFlags ) );
            return;
        }
    }

    // The animation feature is whether turned off or unsupported
    // on this system.  Therefore, just show window without activation.
    ShowWindow( SW_SHOWNOACTIVATE );
}

void CToolBarPopup::OnKeyDown( UINT nChar )
{
    switch ( nChar )
    {
        case VK_ESCAPE: // dismiss menu
        case VK_MENU:
            SendMessage( WM_TB_ENDMODALLOOP );
            break;

        case VK_UP:     // select next/prev button
        case VK_DOWN:
        {
            int nLastItem = m_tbCtrl.GetButtonCount() - 1;
            int nHotItem  = m_tbCtrl.GetHotItem();

            for ( int nIndex = 0; nIndex <= nLastItem; nIndex++ )
            {
                if ( nHotItem >= 0 )
                {
                    nHotItem = ( nChar == VK_UP ) ?
                        ( nHotItem ? nHotItem - 1 : nLastItem ) :
                        ( nHotItem == nLastItem ? 0 : nHotItem + 1 );
                }
                else
                {
                    nHotItem = ( nChar == VK_UP ) ?
                        nLastItem : 0;
                }

                m_tbCtrl.SetHotItem( nHotItem );
                if ( m_tbCtrl.GetHotItem() == nHotItem )
                {
                    break;
                }
            }
            break;
        }
        case VK_RETURN: // issue command associated with selected button
        {
            int nHotItem = m_tbCtrl.GetHotItem();
            if ( nHotItem >= 0 )
            {
                TBBUTTON tbinfo;
                VERIFY( m_tbCtrl.GetButton( nHotItem, &tbinfo ) );
                if ( tbinfo.idCommand != 0 )
                {
                    SendMessage( WM_COMMAND, tbinfo.idCommand );
                    break;
                }
            }

            SendMessage( WM_TB_ENDMODALLOOP );
            break;
        }
        default:
            break;
    }
}

LRESULT CALLBACK CToolBarPopup::KeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
    ASSERT( m_pPopup != 0 );

    if ( code == HC_ACTION )
    {
        CWnd* pCapture = GetCapture();
        if ( ( pCapture == m_pPopup ) ||
             ( pCapture == &m_pPopup->m_tbCtrl ) )
        {
            if ( !( HIWORD( lParam ) & KF_UP ) )
            {
                m_pPopup->OnKeyDown( (UINT)wParam );
            }

            return 1;
        }
    }

    return ::CallNextHookEx( m_hKeyboardHook, code, wParam, lParam );
}

void CToolBarPopup::DrawBorder( CDC* pDC )
{
    CRect rcWindow;
    GetWindowRect( rcWindow );
    CRect rcClient;
    GetClientRect( rcClient );
    ClientToScreen( rcClient );
    rcClient.OffsetRect( -rcWindow.TopLeft() );
    rcWindow.OffsetRect( -rcWindow.TopLeft() );
    pDC->ExcludeClipRect( rcClient );
    pDC->FillSolidRect( rcWindow, ::GetSysColor( COLOR_3DSHADOW ) );
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

BOOL CToolBarPopup::OnCommand( WPARAM wParam, LPARAM /*lParam*/ )
{
    // Dismiss menu
    SendMessage( WM_TB_ENDMODALLOOP );

    // Forward command to the original toolbar window
    m_pToolBar->PostMessage( WM_COMMAND, LOWORD( wParam ), 0 );

    return TRUE;    // command was processed
}

BOOL CToolBarPopup::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
    NMHDR* pNMHDR = ( NMHDR* )lParam;
    if ( pNMHDR->hwndFrom == m_tbCtrl.m_hWnd )
    {
        // Handle certain notifications from embedded toolbar control
        switch ( pNMHDR->code )
        {
            case NM_RELEASEDCAPTURE:
                SetCapture();
                *pResult = 0;
                break;

            case NM_CUSTOMDRAW:
            {
                CWinAppEx* pApp = CWinAppEx::GetInstance();
                if ( pApp->IsWinXP() )
                {
                    LPNMTBCUSTOMDRAW lpNMCustomDraw = ( LPNMTBCUSTOMDRAW )pNMHDR;
                    *pResult = CDRF_DODEFAULT;  // by default

                    switch ( lpNMCustomDraw->nmcd.dwDrawStage )
                    {
                        case CDDS_PREPAINT:
                            *pResult = CDRF_NOTIFYITEMDRAW;
                            break;

                        case CDDS_ITEMPREPAINT:
                        {
                            *pResult |= TBCDRF_NOEDGES | TBCDRF_NOOFFSET;
                            UINT uItemState = lpNMCustomDraw->nmcd.uItemState;
                            if ( !( uItemState & CDIS_DISABLED ) && ( uItemState & ( CDIS_HOT | CDIS_SELECTED ) ) )
                            {
                                lpNMCustomDraw->clrHighlightHotTrack = ::GetSysColor( pApp->GetFlatMenu() ? COLOR_MENUHILIGHT : COLOR_HIGHLIGHT );
                                lpNMCustomDraw->clrText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
                                *pResult |= TBCDRF_HILITEHOTTRACK;
                            }
                            else
                            {
                                if ( uItemState & CDIS_DISABLED )
                                {
                                    lpNMCustomDraw->clrText = ::GetSysColor( COLOR_GRAYTEXT );
                                }
                                else if ( uItemState & CDIS_CHECKED )
                                {
                                    lpNMCustomDraw->clrText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
                                    lpNMCustomDraw->clrBtnFace = ::GetSysColor( pApp->GetFlatMenu() ? COLOR_MENUHILIGHT : COLOR_HIGHLIGHT );
                                }
                                else
                                {
                                    lpNMCustomDraw->clrText = ::GetSysColor( COLOR_MENUTEXT );
                                }
                            }
                            break;
                        }
                    }
                }
                //else
                //{
                //    *pResult = m_pToolBar->DoCustomDraw( pNMHDR, &m_tbCtrl );
                //}
                break;
            }
            case TBN_GETINFOTIP:
                if ( !m_bTextLabels )
                {
                    NMTBGETINFOTIP* lptbgit = ( NMTBGETINFOTIP* )pNMHDR;
                    CString strTip;
                    m_pToolBar->GetButtonTip( lptbgit->iItem, strTip );
                    _tcsncpy( lptbgit->pszText, strTip, lptbgit->cchTextMax );
                }
                *pResult = 0;
                break;

            default:    // forward message to the parent of the original toolbar
                *pResult = m_pToolBar->GetParent()->SendMessage( WM_NOTIFY, wParam, lParam );
                break;
        }

        return TRUE;
    }

    return CWnd::OnNotify( wParam, lParam, pResult );
}

LRESULT CToolBarPopup::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    if ( WM_MOUSEFIRST <= message && message <= WM_MOUSELAST )
    {
        DWORD dwPos = ::GetMessagePos();
        CPoint ptScreen( LOWORD( dwPos ), HIWORD( dwPos ) );
        CWnd* pWnd = WindowFromPoint( ptScreen );
        if ( pWnd != 0 )
        {
            CPoint ptClient( ptScreen );
            pWnd->ScreenToClient( &ptClient );

            switch ( message )
            {
                case WM_MOUSEMOVE:
                {
                    // Check if hot item should be changed
                    bool bOverTbCtrl = ( pWnd == &m_tbCtrl );
                    if ( bOverTbCtrl )
                    {
                        int nHit = m_tbCtrl.HitTest( &ptClient );
                        m_tbCtrl.SetHotItem( nHit );

                        // Let tooltip control process mouse event
                        CToolTipCtrl* pTtCtrl = m_tbCtrl.GetToolTips();
                        if ( pTtCtrl != 0 )
                        {
                            MSG msg;
                            msg.hwnd    = m_tbCtrl.m_hWnd;
                            msg.message = WM_MOUSEMOVE;
                            msg.wParam  = wParam;
                            msg.lParam  = MAKELPARAM( ptClient.x, ptClient.y );
                            msg.pt      = ptScreen;
                            msg.time    = ::GetMessageTime();
                            pTtCtrl->RelayEvent( &msg );
                        }
                    }
                    else if ( m_bOverTbCtrl )
                    {
                        m_tbCtrl.SetHotItem( -1 );
                    }

                    m_bOverTbCtrl = bOverTbCtrl;
                    return 0L;
                }
                case WM_LBUTTONDOWN:
                    if ( pWnd != this )
                    {
                        // Dismiss menu if user has clicked outside the window
                        if ( pWnd != &m_tbCtrl )
                        {
                            SendMessage( WM_TB_ENDMODALLOOP );
                        }

                        // Forward this mouse event to the window that was clicked
                        LPARAM nPosition = MAKELPARAM( ptScreen.x, ptScreen.y );
                        WPARAM nHitTest  = pWnd->SendMessage( WM_NCHITTEST, 0, nPosition );
                        if ( nHitTest == HTCLIENT )
                        {
                            nPosition = MAKELPARAM( ptClient.x, ptClient.y );
                            pWnd->PostMessage( WM_LBUTTONDOWN, wParam, nPosition );
                        }
                        else
                        {
                            pWnd->PostMessage( WM_NCLBUTTONDOWN, nHitTest, nPosition );
                        }
                    }
                    return 0L;

                default:
                    break;
            }
        }
    }

    return CWnd::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////
// CToolBarPopup message handlers

BEGIN_MESSAGE_MAP(CToolBarPopup, CWnd)
    //{{AFX_MSG_MAP(CToolBarPopup)
    ON_WM_CAPTURECHANGED()
    ON_WM_CREATE()
    ON_WM_MOUSEACTIVATE()
    ON_WM_NCPAINT()
    //}}AFX_MSG_MAP
    ON_MESSAGE( WM_PRINT, OnPrint )
    ON_MESSAGE_VOID( WM_TB_ENDMODALLOOP, OnEndModalLoop )
END_MESSAGE_MAP()

void CToolBarPopup::OnCaptureChanged( CWnd* pWnd )
{
    if ( ( pWnd != this ) && ( pWnd != &m_tbCtrl ) && ContinueModal() )
    {
        PostMessage( WM_TB_ENDMODALLOOP );  // dismiss menu
    }

    CWnd::OnCaptureChanged( pWnd );
}

int CToolBarPopup::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
    {
        return -1;
    }

    // Create embedded toolbar control
    if ( !m_tbCtrl.Create(
            TBSTYLE_FLAT | TBSTYLE_WRAPABLE | TBSTYLE_LIST | TBSTYLE_TOOLTIPS |
            CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE |
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            CRect( 0, 0, 0, 0 ), this, m_pToolBar->GetDlgCtrlID() ) )
    {
        return -1;
    }

    m_tbCtrl.SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS );
    VERIFY( m_tbCtrl.GetToolTips()->ModifyStyle( 0, TTS_ALWAYSTIP ) );

    if ( CWinAppEx::GetInstance()->IsWinXP() )
    {
        HRESULT hr = ::SetWindowThemeXP( m_tbCtrl.m_hWnd, L"", L"" );
        ASSERT( SUCCEEDED( hr ) );
    }

    // There could be only two modes, depending on text options
    // of the original toolbar: with or without text labels.
    int nTextType = m_pToolBar->GetTextType();
    m_bTextLabels =
        ( nTextType == TBT_TEXT ) ||
        ( nTextType == TBT_RIGHT );

    // Copy all required information from the original toolbar
    CToolBarCtrl& tbCtrl = m_pToolBar->GetToolBarCtrl();
    m_tbCtrl.SetImageList( tbCtrl.GetImageList() );
    m_tbCtrl.SetHotImageList( tbCtrl.GetHotImageList() );
    m_tbCtrl.SetDisabledImageList( tbCtrl.GetDisabledImageList() );

    CRect rcItem, rcClient;
    tbCtrl.GetClientRect( rcClient );

    TBBUTTON tbinfo;
    int nMaxWidth = 0;
    int nButtons = tbCtrl.GetButtonCount();
    int nIndex;
    for ( nIndex = 0; nIndex < nButtons; nIndex++ )
    {
        tbCtrl.GetItemRect( nIndex, rcItem );
        if ( rcItem.right > rcClient.right )
        {
            VERIFY( tbCtrl.GetButton( nIndex, &tbinfo ) );
            if ( !( tbinfo.fsStyle & TBSTYLE_SEP ) && !( tbinfo.fsState & TBSTATE_HIDDEN ) )
            {
                CString strButtonText;
                m_pToolBar->GetButtonTextByCommand( tbinfo.idCommand, strButtonText );
                CString strToAdd( strButtonText, strButtonText.GetLength() + 1 );
                tbinfo.iString  = m_tbCtrl.AddStrings( strToAdd );
                tbinfo.fsStyle |= TBSTYLE_AUTOSIZE | ( m_bTextLabels ? BTNS_SHOWTEXT : 0 );
                VERIFY( m_tbCtrl.AddButtons( 1, &tbinfo ) );
                VERIFY( m_tbCtrl.GetItemRect( m_tbCtrl.CommandToIndex( tbinfo.idCommand ), rcItem ) );
                nMaxWidth = max( nMaxWidth, rcItem.Width() );
            }
        }
    }

    nButtons = m_tbCtrl.GetButtonCount();
    if ( nButtons == 0 )
    {
        ASSERT( false );    // this should never happen
        return -1;
    }

    if ( m_bTextLabels )
    {
        TBBUTTONINFO tbbi;
        tbbi.cbSize = sizeof( tbbi );
        tbbi.dwMask = TBIF_SIZE | TBIF_STYLE;
        for ( nIndex = 0; nIndex < nButtons; nIndex++ )
        {
            VERIFY( m_tbCtrl.GetButton( nIndex, &tbinfo ) );
            tbbi.cx      = ( WORD )nMaxWidth;
            tbbi.fsStyle = ( BYTE )( tbinfo.fsStyle & ~TBSTYLE_AUTOSIZE );
            VERIFY( m_tbCtrl.SetButtonInfo( tbinfo.idCommand, &tbbi ) );
        }
    }

    m_tbCtrl.AutoSize();

    // Calc toolbar size
    if ( nButtons > 1 )
    {
        m_tbCtrl.SetRows( nButtons, m_bTextLabels, rcClient );
    }
    else
    {
        VERIFY( m_tbCtrl.GetItemRect( 0, rcClient ) );
    }

    m_tbCtrl.MoveWindow( rcClient );

    return 0;
}

int CToolBarPopup::OnMouseActivate( CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/ )
{
    return MA_NOACTIVATEANDEAT; // just in case
}

void CToolBarPopup::OnNcPaint()
{
    if ( m_bFlatMenu )
    {
        CWindowDC dc( this );
        DrawBorder( &dc );
        ReleaseDC( &dc );
    }
    else
    {
        CWnd::OnNcPaint();
    }
}

LRESULT CToolBarPopup::OnPrint( WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult = DefWindowProc( WM_PRINT, wParam, lParam );

    if ( m_bFlatMenu && ( lParam & PRF_NONCLIENT ) )
    {
        CDC* pDC = CDC::FromHandle( ( HDC )wParam );
        int nSavedDC = pDC->SaveDC();
        DrawBorder( pDC );
        pDC->RestoreDC( nSavedDC );
    }

    return lResult;
}

void CToolBarPopup::OnEndModalLoop()
{
    EndModalLoop( 0 );
}
