//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FOLDER_VIEW_H__
#define __FX_FOLDER_VIEW_H__
#pragma once

#include "FolderPaneObserver.h"

class FolderViewObserver;
class FolderPane;
class ExplorerCtrlPrint;

class FolderView
    : public CView
    , public FolderPaneObserver
{
    typedef CView super;

public:
    FolderView(void);
    virtual ~FolderView(void);

public:
    void setObserver(FolderViewObserver *aObserver);

public:
    FolderPane *getFolderPane(void) const;

    void visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleFolderPane(void) const;

    void print(void);
    void printPreview(void);

    void setChangedOption(Option &aOption);

protected:
    void recalcLayout(void);

protected:
    void createFolderPane(void);
    void destroyFolderPane(void);

protected:
    FolderViewObserver *mObserver;

    FolderPane *mFolderPane;

    ExplorerCtrlPrint *mListCtrlPrint;

public:
    virtual void OnDraw(CDC *aDC);  // overridden to draw this view
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);
    virtual void OnInitialUpdate(void);
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);

protected:
    virtual xpr_bool_t OnPreparePrinting(CPrintInfo *aPrintInfo);
    virtual void OnBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnUpdate(CView *aSender, LPARAM aHint, CObject *aHintObject);
    virtual void OnPrint(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnEndPrintPreview(CDC *aDC, CPrintInfo *aPrintInfo, POINT aPoint, CPreviewView *aView);
    virtual void OnActivateView(xpr_bool_t aActivate, CView *aActivateView, CView *aDeactiveView);

protected:
    // from FolderPaneObserver
    virtual xpr_bool_t onExplore(FolderPane &aFolderPane, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy);
    virtual void onUpdateStatus(FolderPane &aFolderPane);
    virtual void onRename(FolderPane &aFolderPane, HTREEITEM aTreeItem);
    virtual void onContextMenuDelete(FolderPane &aFolderPane, HTREEITEM aTreeItem);
    virtual void onContextMenuRename(FolderPane &aFolderPane, HTREEITEM aTreeItem);
    virtual void onContextMenuPaste(FolderPane &aFolderPane, HTREEITEM aTreeItem);
    virtual xpr_sint_t onDrop(FolderPane &aFolderPane, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir);
    virtual void onSetFocus(FolderPane &aFolderPane);
    virtual void onMoveFocus(FolderPane &aFolderPane, xpr_sint_t aCurWnd);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnDestroy(void);
};

#endif // __FX_FOLDER_VIEW_H__
