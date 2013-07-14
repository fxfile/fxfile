/////////////////////////////////////////////////////////////////////////////
// CustomizeDialog.h: interface for the CCustomizeDialog class.
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

#ifndef __CUSTOMIZEDIALOG_H__
#define __CUSTOMIZEDIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// Options
enum
{
    TBT_NONE = 0,
    TBT_RIGHT,
    TBT_TEXT
};

enum
{
    TBI_SMALL = 0,
    TBI_LARGE
};

enum
{
    TBG_DISABLED = 0,
    TBG_COLD,
    TBG_HOT,
};

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

class CCustomizeDialog;

class COptionsDialog : public CDialog
{
// Construction
public:
    COptionsDialog( int nTextType, int nIconType );

// Dialog Data
    //{{AFX_DATA(COptionsDialog)
    CComboBox m_cbTextType;
    CComboBox m_cbIconType;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COptionsDialog)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Operations
public:
    bool SelectTextType( int nTextType );
    bool SelectIconType( int nIconType );

// Implementation
protected:
    CCustomizeDialog* GetCustomizeDialog() const;

// Implementation data
protected:
    int m_nTextType;
    int m_nIconType;

// Generated message map functions
protected:
    //{{AFX_MSG(COptionsDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnTextType();
    afx_msg void OnIconType();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDialog dialog

class CToolBarEx;

class CCustomizeDialog : public CWnd
{
    DECLARE_DYNAMIC( CCustomizeDialog );

// Construction
public:
    CCustomizeDialog( CToolBarEx* pToolBar );

// Operations
public:
    void SetTextType( int nTextType, bool bInDialog );
    void SetIconType( int nIconType, bool bInDialog );

    void AddTextType( CComboBox& cbTextType, int nTextType, LPCTSTR lpcszString );
    void AddIconType( CComboBox& cbIconType, int nIconType, LPCTSTR lpcszString );

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCustomizeDialog)
    protected:
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
protected:
    CSize GetButtonSize() const;

// Implementation data
protected:
    COptionsDialog  m_dlgOptions;
    CToolBarEx*     m_pToolBar;

// Generated message map functions
protected:
    //{{AFX_MSG(CCustomizeDialog)
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    //}}AFX_MSG
    LRESULT OnInitDialog( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  // !__CUSTOMIZEDIALOG_H__
