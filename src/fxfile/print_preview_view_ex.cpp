//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "print_preview_view_ex.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
IMPLEMENT_DYNCREATE(PrintPreviewViewEx, CPreviewView)

PrintPreviewViewEx::PrintPreviewViewEx(void)
{
}

PrintPreviewViewEx::~PrintPreviewViewEx(void)
{
}

BEGIN_MESSAGE_MAP(PrintPreviewViewEx, super)
    ON_WM_CREATE()
END_MESSAGE_MAP()

xpr_sint_t PrintPreviewViewEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_pToolBar->EnableToolTips(XPR_TRUE);

    // Subclassing
    mPrintButton  .SubclassDlgItem(AFX_ID_PREVIEW_PRINT,   m_pToolBar);
    mPrevButton   .SubclassDlgItem(AFX_ID_PREVIEW_PREV,    m_pToolBar);
    mNextButton   .SubclassDlgItem(AFX_ID_PREVIEW_NEXT,    m_pToolBar);
    mZoomInButton .SubclassDlgItem(AFX_ID_PREVIEW_ZOOMIN,  m_pToolBar);
    mZoomOutButton.SubclassDlgItem(AFX_ID_PREVIEW_ZOOMOUT, m_pToolBar);
    mCloseButton  .SubclassDlgItem(AFX_ID_PREVIEW_CLOSE,   m_pToolBar);

    mPrintButton  .SetBitmaps(IDB_PREVIEW_PRINT_HOT,   RGB(255,0,255), IDB_PREVIEW_PRINT_COLD,   RGB(255,0,255));
    mPrevButton   .SetBitmaps(IDB_PREVIEW_PREV_HOT,    RGB(255,0,255), IDB_PREVIEW_PREV_COLD,    RGB(255,0,255));
    mNextButton   .SetBitmaps(IDB_PREVIEW_NEXT_HOT,    RGB(255,0,255), IDB_PREVIEW_NEXT_COLD,    RGB(255,0,255));
    mZoomInButton .SetBitmaps(IDB_PREVIEW_ZOOMIN_HOT,  RGB(255,0,255), IDB_PREVIEW_ZOOMIN_COLD,  RGB(255,0,255));
    mZoomOutButton.SetBitmaps(IDB_PREVIEW_ZOOMOUT_HOT, RGB(255,0,255), IDB_PREVIEW_ZOOMOUT_COLD, RGB(255,0,255));

    mPrevButton.SetAlign(mPrevButton.GetAlign() + 2);

    mPrintButton.SetWindowText(gApp.loadString(XPR_STRING_LITERAL("print_preview.button.print")));
    mPrevButton .SetWindowText(gApp.loadString(XPR_STRING_LITERAL("print_preview.button.previous_page")));
    mNextButton .SetWindowText(gApp.loadString(XPR_STRING_LITERAL("print_preview.button.next_page")));
    mCloseButton.SetWindowText(gApp.loadString(XPR_STRING_LITERAL("print_preview.button.close")));

    return 0;
}

xpr_bool_t PrintPreviewViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
    if (cs.lpszClass == XPR_NULL)
        cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNCLIENT);
    return CView::PreCreateWindow(cs);
}

#ifdef XPR_CFG_BUILD_DEBUG
void PrintPreviewViewEx::AssertValid() const
{
    CView::AssertValid();
}

void PrintPreviewViewEx::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
#endif // XPR_CFG_BUILD_DEBUG

void PrintPreviewViewEx::Close(void)
{
    OnPreviewClose();
}
} // namespace fxfile
