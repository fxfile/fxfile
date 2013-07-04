/////////////////////////////////////////////////////////////////////////////
// CustomizeDialog.cpp: implementation of the CCustomizeDialog class.
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
#include "CustomizeDialog.h"

#include "../resource.h"
#include "GlobalData.h"
#include "ToolBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

COptionsDialog::COptionsDialog( int nTextType, int nIconType )
{
    //{{AFX_DATA_INIT(COptionsDialog)
    //}}AFX_DATA_INIT

    m_nTextType = nTextType;
    m_nIconType = nIconType;
}


void COptionsDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    //{{AFX_DATA_MAP(COptionsDialog)
    DDX_Control(pDX, IDC_CUSTOM_TOOLBAR_TEXT_OPTION, m_cbTextType);
    DDX_Control(pDX, IDC_CUSTOM_TOOLBAR_ICON_OPTION, m_cbIconType);
    //}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// Operations

bool COptionsDialog::SelectTextType( int nTextType )
{
    for ( int nIndex = 0; nIndex < m_cbTextType.GetCount(); nIndex++ )
    {
        if ( nTextType == ( int )m_cbTextType.GetItemData( nIndex ) )
        {
            m_cbTextType.SetCurSel( nIndex );
            m_nTextType = nTextType;
            return true;
        }
    }

    return false;
}

bool COptionsDialog::SelectIconType( int nIconType )
{
    for ( int nIndex = 0; nIndex < m_cbIconType.GetCount(); nIndex++ )
    {
        if ( nIconType == ( int )m_cbIconType.GetItemData( nIndex ) )
        {
            m_cbIconType.SetCurSel( nIndex );
            m_nIconType = nIconType;
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

CCustomizeDialog* COptionsDialog::GetCustomizeDialog() const
{
    return STATIC_DOWNCAST( CCustomizeDialog, GetParent() );
}

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
    //{{AFX_MSG_MAP(COptionsDialog)
    ON_CBN_SELENDOK(IDC_CUSTOM_TOOLBAR_TEXT_OPTION, OnTextType)
    ON_CBN_SELENDOK(IDC_CUSTOM_TOOLBAR_ICON_OPTION, OnIconType)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COptionsDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CCustomizeDialog* pCustomizeDialog = GetCustomizeDialog();

    pCustomizeDialog->AddTextType(m_cbTextType, TBT_TEXT,  fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.text.show")));
    pCustomizeDialog->AddTextType(m_cbTextType, TBT_RIGHT, fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.text.show_on_right")));
    pCustomizeDialog->AddTextType(m_cbTextType, TBT_NONE,  fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.text.none")));
    VERIFY(SelectTextType(m_nTextType));

    pCustomizeDialog->AddIconType(m_cbIconType, TBI_SMALL, fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.icon.small")));
    pCustomizeDialog->AddIconType(m_cbIconType, TBI_LARGE, fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.icon.large")));
    VERIFY(SelectIconType(m_nIconType));

    SetDlgItemText(IDC_CUSTOM_TOOLBAR_LABEL_TEXT_OPTION, fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.label.text_option")));
    SetDlgItemText(IDC_CUSTOM_TOOLBAR_LABEL_ICON_OPTION, fxfile::theApp.loadString(XPR_STRING_LITERAL("popup.custom_toolbar.label.icon_option")));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::OnTextType()
{
    int nSel = m_cbTextType.GetCurSel();
    ASSERT( nSel != CB_ERR );
    m_nTextType = ( int )m_cbTextType.GetItemData( nSel );
    GetCustomizeDialog()->SetTextType( m_nTextType, false );
}

void COptionsDialog::OnIconType()
{
    int nSel = m_cbIconType.GetCurSel();
    ASSERT( nSel != CB_ERR );
    m_nIconType = ( int )m_cbIconType.GetItemData( nSel );
    GetCustomizeDialog()->SetIconType( m_nIconType, false );
}

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDialog dialog

#define IDC_LB_AVAILABLE    0x00C9  // determined with Spy++
#define IDC_LB_CURRENT      0x00CB

static const int cxPadding = 3;
static const int cyPadding = 3;

IMPLEMENT_DYNAMIC( CCustomizeDialog, CWnd )

CCustomizeDialog::CCustomizeDialog( CToolBarEx* pToolBar )
    : m_dlgOptions( pToolBar->m_nTextType, pToolBar->m_nIconType )
{
    m_pToolBar = pToolBar;
}

/////////////////////////////////////////////////////////////////////////////
// Operations

void CCustomizeDialog::SetTextType( int nTextType, bool bInDialog )
{
    if ( bInDialog )
    {
        VERIFY( m_dlgOptions.SelectTextType( nTextType ) );
    }
    else
    {
        m_pToolBar->SetTextType( nTextType );
    }
}

void CCustomizeDialog::SetIconType( int nIconType, bool bInDialog )
{
    if ( bInDialog )
    {
        VERIFY( m_dlgOptions.SelectIconType( nIconType ) );
    }
    else
    {
        m_pToolBar->SetIconType( nIconType );
    }

    int nHeight = GetButtonSize().cy;

    CWnd* pWnd = GetDlgItem( IDC_LB_AVAILABLE );
    if ( pWnd != 0 )
    {
        pWnd->SendMessage( LB_SETITEMHEIGHT, 0, nHeight );
        pWnd->Invalidate();
    }

    pWnd = GetDlgItem( IDC_LB_CURRENT );
    if ( pWnd != 0 )
    {
        pWnd->SendMessage( LB_SETITEMHEIGHT, 0, nHeight );
        pWnd->Invalidate();
    }
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

void CCustomizeDialog::PostNcDestroy()
{
    delete this;
}

/////////////////////////////////////////////////////////////////////////////
// Implementation

void CCustomizeDialog::AddTextType( CComboBox& cbTextOptions, int nTextType, LPCTSTR lpcszString )
{
    if ( m_pToolBar->IsTextTypeAvailable( nTextType ) )
    {
        int nItem = cbTextOptions.AddString( lpcszString );
        ASSERT( nItem >= 0 );
        cbTextOptions.SetItemData( nItem, ( DWORD )nTextType );
    }
}

void CCustomizeDialog::AddIconType( CComboBox& cbIconOptions, int nIconType, LPCTSTR lpcszString )
{
    if ( m_pToolBar->IsIconTypeAvailable( nIconType ) )
    {
        int nItem = cbIconOptions.AddString( lpcszString );
        ASSERT( nItem >= 0 );
        cbIconOptions.SetItemData( nItem, ( DWORD )nIconType );
    }
}

CSize CCustomizeDialog::GetButtonSize() const
{
    CSize szImage = ( m_pToolBar->GetIconType() == TBI_SMALL ) ?
        m_pToolBar->m_szSmallIcon : m_pToolBar->m_szLargeIcon;
    return szImage + CSize( cxPadding * 2, cyPadding * 2 );
}

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDialog message handlers

BEGIN_MESSAGE_MAP(CCustomizeDialog, CWnd)
    //{{AFX_MSG_MAP(CCustomizeDialog)
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_CTLCOLOR()
    //}}AFX_MSG_MAP
    ON_MESSAGE( WM_INITDIALOG, OnInitDialog )
END_MESSAGE_MAP()

LRESULT CCustomizeDialog::OnInitDialog( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
    CRect rcClient;
    GetClientRect( rcClient );
    CRect rcWindow;
    GetWindowRect( rcWindow );

    // Create Options dialog as a child window of Customize Toolbar dialog
    VERIFY( m_dlgOptions.Create( IDD_CUSTOM_TOOLBAR, this ) );

    CRect rcDlg;
    m_dlgOptions.GetWindowRect( rcDlg );

    // Place Options dialog at the bottom
    VERIFY( m_dlgOptions.SetWindowPos( 0, 0, rcClient.Height(), rcClient.Width(), rcDlg.Height(),
        SWP_NOZORDER | SWP_SHOWWINDOW ) );

    // Increase height of Customize Toolbar dialog accordingly
    rcWindow.bottom += rcDlg.Height();
    VERIFY( SetWindowPos( 0, 0, 0, rcWindow.Width(), rcWindow.Height(),
        SWP_NOZORDER | SWP_NOMOVE ) );

    LRESULT sResult = Default();

    RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);

    return sResult;
}

void CCustomizeDialog::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    if ( lpDrawItemStruct->CtlType == ODT_LISTBOX )
    {
        if ( ( nIDCtl == IDC_LB_AVAILABLE ) || ( nIDCtl == IDC_LB_CURRENT ) )
        {
            CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
            int nSavedDC = pDC->SaveDC();
            ASSERT( nSavedDC );

            // Get all needed info about the item being drawn
            CRect rcItem( &lpDrawItemStruct->rcItem );
            int nItem  = ( SHORT )HIWORD( lpDrawItemStruct->itemData );
            int nImage = ( SHORT )LOWORD( lpDrawItemStruct->itemData );
            int nIndex = ( int )lpDrawItemStruct->itemID;
            bool bSelected  = ( lpDrawItemStruct->itemState & ODS_SELECTED ) != 0;
            bool bFocused   = ( lpDrawItemStruct->itemState & ODS_FOCUS    ) != 0;
            bool bSeparator = ( ( nItem < 0 ) || ( nItem > (int)m_pToolBar->m_dqButtons.size() ) );

            // Calculate rectangles for image, text and focus frame
            CSize szButton = GetButtonSize();
            CRect rcImage( rcItem );
            rcImage.right = rcImage.left + szButton.cx;
            CRect rcText( rcItem );
            rcText.left  += szButton.cx + 5;
            rcText.right -= 2;
            CRect rcLabel( rcItem );
            rcLabel.left += szButton.cx + 1;

            // Cache system colors
            COLORREF clrText     = ::GetSysColor( bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT );
            COLORREF clrBack     = ::GetSysColor( bSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE );
            COLORREF clrGrayText = ::GetSysColor( COLOR_GRAYTEXT );
            COLORREF clrBtnFace  = ::GetSysColor( COLOR_BTNFACE );
            COLORREF clr3dShadow = ::GetSysColor( COLOR_3DSHADOW );
            COLORREF clr3dHilite = ::GetSysColor( COLOR_3DHILIGHT );

            pDC->FillSolidRect( rcImage, clrBtnFace );
            pDC->FillSolidRect( rcLabel, clrBack );

            // Draw focus rect, if needed
            if ( bFocused )
            {
                CBrush brush( RGB(
                    255 - GetRValue( clrBack ),
                    255 - GetGValue( clrBack ),
                    255 - GetBValue( clrBack ) ) );
                rcLabel.DeflateRect( 2, 2 );
                pDC->FrameRect( rcLabel, &brush );
                VERIFY( brush.DeleteObject() );
            }

            // Get item text to be drawn
            CString strText;
            if ( !bSeparator )
            {
                UINT nID = m_pToolBar->m_dqButtons[ nItem ]->tbinfo.idCommand;
                m_pToolBar->GetButtonTextByCommand( nID, strText );
            }
            else
            {
                //VERIFY( strText.LoadString( IDS_SEPARATOR ) );
            }

            // Prepare to draw item text
            int nFormat = DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
            pDC->SetBkMode( TRANSPARENT );
            pDC->SetTextColor( clrText );

            if ( !bSeparator )
            {
                // Draw item image, if any
                if ( nImage >= 0 )
                {
                    if ( bSelected )
                    {
                        pDC->Draw3dRect( rcImage, clr3dHilite, clr3dShadow );
                    }

                    CToolBarCtrl& tbCtrl = m_pToolBar->GetToolBarCtrl();
                    CImageList* pImageList = tbCtrl.GetHotImageList();
                    if ( !bSelected || !pImageList )
                    {
                        pImageList = tbCtrl.GetImageList();
                    }

                    rcImage.DeflateRect( cxPadding, cyPadding );
                    VERIFY( pImageList->Draw( pDC, nImage, rcImage.TopLeft(),
                        ILD_NORMAL | ILD_TRANSPARENT ) );
                }

                if ( m_pToolBar->m_dqButtons[ nItem ]->tbinfo.fsStyle & TBSTYLE_DROPDOWN )
                {
                    // This button is drop-down button.  To indicate this simple fact
                    // we draw an arrow on right like one that submenu items have.

                    CRect rcArrow( rcText );
                    rcArrow.left = rcArrow.right - ::GetSystemMetrics( SM_CXMENUCHECK );
                    rcText.right -= rcArrow.Width();

                    CBrush brush( clrText );
                    CBrush* pOldBrush = pDC->SelectObject( &brush );

                    CDC dc;
                    VERIFY( dc.CreateCompatibleDC( pDC ) );

                    CBitmap bmpArrow;
                    VERIFY( bmpArrow.CreateCompatibleBitmap( pDC, rcArrow.Width(), rcArrow.Height() ) );

                    CBitmap* pOldBitmap = dc.SelectObject( &bmpArrow );
                    CRect rc( 0, 0, rcArrow.Width(), rcArrow.Height() );
                    VERIFY( dc.DrawFrameControl( rc, DFC_MENU, DFCS_MENUARROW ) );
                    VERIFY( pDC->BitBlt( rcArrow.left, rcArrow.top, rcArrow.Width(), rcArrow.Height(),
                        &dc, 0, 0, ROP_PSDPxax ) );

                    dc.SelectObject( pOldBitmap );
                    VERIFY( bmpArrow.DeleteObject() );

                    pDC->SelectObject( pOldBrush );
                    VERIFY( brush.DeleteObject() );
                }
            }
            else
            {
                if ( ( nIDCtl == IDC_LB_CURRENT ) &&
                     ( nIndex == SendDlgItemMessage( nIDCtl, LB_GETCOUNT ) - 1 ) )
                {
                    // The last item in the list of current buttons is a separator
                    // which cannot be removed (guess what is the purpose of it?).
                    // To indicate this fact we draw it as disabled item.

                    if ( bSelected )
                    {
                        pDC->SetTextColor( clrGrayText );
                    }
                    else
                    {
                        CRect rcTemp( rcText );
                        rcTemp.OffsetRect( 1, 1 );
                        pDC->SetTextColor( clr3dHilite );
                        pDC->DrawText( strText, rcTemp, nFormat );
                        pDC->SetTextColor( clr3dShadow );
                    }
                }
                else
                {
                    // Draw the separator bar in the middle
                    rcImage.DeflateRect( cxPadding, cyPadding );
                    int y = ( rcImage.top + rcImage.bottom ) / 2;
                    rcImage.top    = y - 1;
                    rcImage.bottom = y + 1;

                    pDC->Draw3dRect( rcImage, clr3dShadow, clr3dHilite );
                }
            }

            pDC->DrawText( strText, rcText, nFormat );

            VERIFY( pDC->RestoreDC( nSavedDC ) );
            return;
        }
    }

    CWnd::OnDrawItem( nIDCtl, lpDrawItemStruct );
}

void CCustomizeDialog::OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
    if ( lpMeasureItemStruct->CtlType == ODT_LISTBOX )
    {
        if ( ( nIDCtl == IDC_LB_AVAILABLE ) ||
             ( nIDCtl == IDC_LB_CURRENT ) )
        {
            lpMeasureItemStruct->itemHeight = GetButtonSize().cy;
            return;
        }
    }

    CWnd::OnMeasureItem( nIDCtl, lpMeasureItemStruct );
}

HBRUSH CCustomizeDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    HBRUSH hbr = CWnd::OnCtlColor( pDC, pWnd, nCtlColor );

    if ( nCtlColor == CTLCOLOR_LISTBOX )
    {
        UINT nIDCtl = ( UINT )pWnd->GetDlgCtrlID();
        if ( ( nIDCtl == IDC_LB_AVAILABLE ) ||
             ( nIDCtl == IDC_LB_CURRENT ) )
        {
            hbr = ::GetSysColorBrush( COLOR_BTNFACE );
        }
    }

    return hbr;
}
