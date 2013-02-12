//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FolderView.h"

#include "FolderPane.h"
#include "ExplorerCtrl.h"
#include "ExplorerCtrlPrint.h"
#include "PreviewViewEx.h"
#include "MainFrame.h"

#include "rgc/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// control id
//
enum
{
    CTRL_ID_FOLDER_PANE = 50,
};

FolderView::FolderView(void)
    : mObserver(XPR_NULL)
    , mFolderPane(XPR_NULL)
    , mListCtrlPrint(XPR_NULL)
{
}

FolderView::~FolderView(void)
{
}

xpr_bool_t FolderView::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    //const xpr_char_t *sClassName = "MyView";

    //WNDCLASS sWndClass = {0};
    //sWndClass.lpfnWndProc   = ::DefWindowProc;
    //sWndClass.hInstance     = AfxGetInstanceHandle();
    //sWndClass.hCursor       = LoadCursor(XPR_NULL, IDC_ARROW);
    //sWndClass.style         = CS_DBLCLKS;
    //sWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    //sWndClass.lpszClassName = sClassName;
    //if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
    //    return XPR_FALSE;

    //aCreateStruct.lpszClass = sClassName;

    //aCreateStruct.dwExStyle |= WS_EX_CLIENTEDGE;
    //aCreateStruct.style &= ~WS_BORDER;

    CBrush sBrush(::GetSysColor(COLOR_WINDOW));

    HCURSOR sCursor = ::LoadCursor(XPR_NULL, MAKEINTRESOURCE(IDC_ARROW));
    aCreateStruct.lpszClass = AfxRegisterWndClass(0, sCursor, sBrush);
    if (XPR_IS_NOT_NULL(sCursor)) ::DestroyCursor(sCursor);

    aCreateStruct.style |= WS_CLIPCHILDREN;

    return super::PreCreateWindow(aCreateStruct);
}

BEGIN_MESSAGE_MAP(FolderView, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_PRINT_DIRECT, super::OnFilePrint)
END_MESSAGE_MAP()

void FolderView::setObserver(FolderViewObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_sint_t FolderView::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    createFolderPane();

    return 0;
}

void FolderView::OnInitialUpdate(void)
{
    super::OnInitialUpdate();

}

void FolderView::OnDestroy(void)
{
    destroyFolderPane();

    super::OnDestroy();
}

void FolderView::setChangedOption(Option &aOption)
{
    XPR_ASSERT(mFolderPane != XPR_NULL);

    mFolderPane->setChangedOption(aOption);
}

void FolderView::OnDraw(CDC *aDC)
{
}

//--------------------------------
// Printer Processing
//--------------------------------
// ID_FILE_PRINT
//       бщ
// super::OnFilePrint
//       бщ
// super::OnPreparePrinting
//       бщ
// super::OnBeginPrinting
//       бщ
// super::OnPrint    <-----+
//       бщ                 | Yes
// more print page?  ------+
//       бщ No
// super::OnEndPrinting
//--------------------------------

xpr_bool_t FolderView::OnPreparePrinting(CPrintInfo *aPrintInfo)
{
    ExplorerCtrl *sExplorerCtrl = gFrame->getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_FALSE;

    xpr_tchar_t sDocName[XPR_MAX_PATH + 1];
    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        _tcscpy(sDocName, sExplorerCtrl->getCurPath());
    else
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sDocName);

    mListCtrlPrint = new ExplorerCtrlPrint;
    if (XPR_IS_NULL(mListCtrlPrint))
        return XPR_FALSE;

    mListCtrlPrint->setListCtrl(sExplorerCtrl);
    mListCtrlPrint->setAppName(AfxGetAppName());
    mListCtrlPrint->setDocTitle(sDocName);
    mListCtrlPrint->onPreparePrinting(aPrintInfo);

    return DoPreparePrinting(aPrintInfo);
}

void FolderView::OnBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onBeginPrinting(aDC, aPrintInfo);
}

void FolderView::OnPrint(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onPrint(aDC, aPrintInfo);

    super::OnPrint(aDC, aPrintInfo);
}

void FolderView::OnEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onEndPrinting(aDC, aPrintInfo);

    XPR_SAFE_DELETE(mListCtrlPrint);
}

void FolderView::print(void)
{
    super::OnFilePrint();
}

void FolderView::printPreview(void)
{
    CPrintPreviewState *sPrintPreviewState = new CPrintPreviewState;

    //if (DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CPreviewView), sPrintPreviewState) == XPR_FALSE)
    if (DoPrintPreview(IDD_PRINT_PREVIEW, this, RUNTIME_CLASS(PreviewViewEx), sPrintPreviewState) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Error: DoPrintPreview failed.\n"));
        AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
        XPR_SAFE_DELETE(sPrintPreviewState);
    }
}

void FolderView::OnEndPrintPreview(CDC *aDC, CPrintInfo *aPrintInfo, POINT point, CPreviewView* pView) 
{
    super::OnEndPrintPreview(aDC, aPrintInfo, point, pView);
}

FolderPane *FolderView::getFolderPane(void) const
{
    return mFolderPane;
}

void FolderView::visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createFolderPane();
    }
    else
    {
        destroyFolderPane();
    }

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t FolderView::isVisibleFolderPane(void) const
{
    return XPR_IS_NOT_NULL(mFolderPane) ? XPR_TRUE : XPR_FALSE;
}

void FolderView::createFolderPane(void)
{
    if (XPR_IS_NOT_NULL(mFolderPane))
        return;

    mFolderPane = new FolderPane;
    if (XPR_IS_NOT_NULL(mFolderPane))
    {
        mFolderPane->setObserver(dynamic_cast<FolderPaneObserver *>(this));
        mFolderPane->setViewIndex(0);

        mFolderPane->Create(this, CTRL_ID_FOLDER_PANE, CRect(0,0,0,0));
    }
}

void FolderView::destroyFolderPane(void)
{
    if (XPR_IS_NULL(mFolderPane))
        return;

    mFolderPane->DestroyWindow();
    XPR_SAFE_DELETE(mFolderPane);
}

void FolderView::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void FolderView::recalcLayout(void)
{
    CRect sRect;
    GetClientRect(&sRect);

    if (XPR_IS_NOT_NULL(mFolderPane))
    {
        mFolderPane->MoveWindow(sRect);
    }
}

void FolderView::OnPaint(void)
{
    CPaintDC sDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    CMemDC sMemDC(&sDC);
    sMemDC.FillSolidRect(&sClientRect, ::GetSysColor(COLOR_WINDOW));
}

xpr_bool_t FolderView::OnEraseBkgnd(CDC *aDC)
{
    //return super::OnEraseBkgnd(aDC);

    // Because of Thumbnail
    return XPR_TRUE;
}

void FolderView::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

}

void FolderView::OnUpdate(CView *aSender, LPARAM aHint, CObject *aHintObject)
{
}

xpr_bool_t FolderView::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo)
{
    if (getFolderPane() == XPR_NULL)
        return XPR_FALSE;

    return super::OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
}

void FolderView::OnActivateView(xpr_bool_t aActivate, CView *aActivateView, CView *aDeactiveView)
{
    if (XPR_IS_TRUE(aActivate) && aDeactiveView != this && aActivateView == this)
    {
    }

    super::OnActivateView(aActivate, aActivateView, aDeactiveView);
}

//
// from FolderPaneObserver
//
xpr_bool_t FolderView::onExplore(FolderPane &aFolderPane, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onExplore(*this, aFullPidl, aUpdateBuddy);
    }

    return XPR_FALSE;
}

void FolderView::onUpdateStatus(FolderPane &aFolderPane)
{
}

void FolderView::onRename(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onRename(*this, aTreeItem);
    }
}

void FolderView::onContextMenuDelete(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuDelete(*this, aTreeItem);
    }
}

void FolderView::onContextMenuRename(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuRename(*this, aTreeItem);
    }
}

void FolderView::onContextMenuPaste(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onContextMenuPaste(*this, aTreeItem);
    }
}

xpr_sint_t FolderView::onDrop(FolderPane &aFolderPane, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onDrop(*this, aOleDataObject, aTargetDir);
    }

    return 0;
}

void FolderView::onSetFocus(FolderPane &aFolderPane)
{
}

void FolderView::onMoveFocus(FolderPane &aFolderPane, xpr_sint_t aCurWnd)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onMoveFocus(*this, aCurWnd);
    }
}
