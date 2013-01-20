//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_EXPLORER_VIEW_H__
#define __FX_EXPLORER_VIEW_H__
#pragma once

#include "rgc/DropTarget.h"
#include "rgc/TabCtrlObserver.h"

#include "FolderPaneObserver.h"
#include "TabPaneObserver.h"
#include "Splitter.h"
#include "TabType.h"

class ExplorerViewObserver;
class FolderPane;
class FolderCtrl;
class AddressBar;
class ExplorerPane;
class ExplorerCtrl;
class ActivateBar;
class SearchResultCtrl;
class FileScrapPane;
class DropTarget;
class ListCtrlPrint;
class StatusBar;

class ExplorerView
    : public CView
    , public TabCtrlObserver
    , public SplitterObserver
    , public FolderPaneObserver
    , public TabPaneObserver
{
    typedef CView super;

public:
    ExplorerView(void);
    virtual ~ExplorerView(void);

public:
    void setObserver(ExplorerViewObserver *aObserver);

public:
    void setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    xpr_sint_t    newTab(TabType aTabType = TabTypeExplorer);
    xpr_sint_t    newTab(LPITEMIDLIST aInitFolder);
    xpr_sint_t    newTab(const std::tstring &aInitFolder);
    xpr_sint_t    newSearchResultTab(void);
    xpr_sint_t    newFileScrapTab(void);
    xpr_sint_t    duplicateTab(xpr_bool_t aOnCursor = XPR_FALSE);
    xpr_bool_t    canDuplicateTab(xpr_bool_t aOnCursor = XPR_FALSE) const;
    xpr_bool_t    isExplorerTabMode(void) const;
    xpr_sint_t    getTabOnCursor(void) const;
    xpr_sint_t    getTabCount(void) const;
    TabPane      *getTabPane(xpr_sint_t aTab = -1) const;
    xpr_uint_t    getTabPaneId(xpr_sint_t aTab = -1) const;
    xpr_sint_t    findTab(xpr_uint_t aTabPaneId) const;
    xpr_sint_t    findTabFirstExplorerCtrl(void) const;
    void          setCurTab(xpr_sint_t aTab);
    xpr_bool_t    setCurTabLastActivedExplorerCtrl(void);
    void          firstTab(void);
    void          previousTab(void);
    void          nextTab(void);
    void          lastTab(void);
    void          closeTab(xpr_sint_t aTab = -1, xpr_bool_t aOnCursor = XPR_FALSE);
    void          closeAllTabsButThis(xpr_bool_t aOnCursor = XPR_FALSE);
    void          closeAllTabs(void);

    FolderPane       *getFolderPane(void) const;
    FolderCtrl       *getFolderCtrl(void) const;
    AddressBar       *getAddressBar(void) const;
    ExplorerPane     *getExplorerPane(void) const;
    ExplorerCtrl     *getExplorerCtrl(xpr_sint_t aTab = -1) const;
    SearchResultCtrl *getSearchResultCtrl(xpr_sint_t aTab = -1) const;
    FileScrapPane    *getFileScrapPane(xpr_sint_t aTab = -1) const;
    ActivateBar      *getActivateBar(void) const;
    StatusBar        *getStatusBar(void) const;

    xpr_bool_t visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE, xpr_bool_t aSaveFolderPaneVisible = XPR_TRUE);
    xpr_bool_t isVisibleFolderPane(void) const;
    xpr_bool_t isLeftFolderPane(void) const;
    void       setLeftFolderPane(xpr_bool_t aLeft);

    void       visibleActivateBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleActivateBar(void) const;
    void       setActivateBarStatus(xpr_bool_t aActivate, xpr_bool_t aLastActivated);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);

    const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const;

    void print(void);
    void printPreview(void);

    void recalcLayout(void);

    void saveOption(void);

protected:
    xpr_uint_t generateTabPaneId(void);

    enum
    {
        InitFolderNone      = 0x00000000,
        InitFolderSplit     = 0x00000001,
        InitFolderCmdParam  = 0x00000002,
        InitFolderCfgInit   = 0x00000004,
        InitFolderLastSaved = 0x00000008,
        InitFolderDefault   = 0x00000010,
        InitFolderAll       = 0xffffffff,
    };

    LPITEMIDLIST getInitFolder(xpr_sint_t aIndex, std::tstring &aSelFile, xpr_uint_t aFlags = InitFolderAll, xpr_uint_t *aInitFolderType = XPR_NULL) const;

    xpr_bool_t openFolder(const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    xpr_bool_t openFolder(LPITEMIDLIST aFullPidl);

    xpr_bool_t createTabCtrl(void);
    xpr_bool_t createSplitter(void);
    xpr_bool_t createFolderPane(void);
    xpr_bool_t createActivateBar(void);
    void       destroyFolderPane(void);
    void       destroyActivateBar(void);

protected:
    ExplorerViewObserver *mObserver;

    xpr_sint_t     mViewIndex;
    xpr_bool_t     mInit;

    TabCtrl       *mTabCtrl;
    xpr_uint_t     mNextTabPaneId;
    xpr_uint_t     mLastActivedExplorerPaneId;
    CPoint         mContextMenuCursor;

    Splitter       mSplitter;
    FolderPane    *mFolderPane;
    ExplorerPane  *mExplorerPane;
    ActivateBar   *mActivateBar;

    ListCtrlPrint *mListCtrlPrint;

    DropTarget     mDropTarget;

    class TabData;

protected:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);
    virtual void OnInitialUpdate(void);
    virtual void OnDraw(CDC *aDC);
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);

    // print
    virtual xpr_bool_t OnPreparePrinting(CPrintInfo *aPrintInfo);
    virtual void OnBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnUpdate(CView *aSender, LPARAM aHint, CObject *aHintObject);
    virtual void OnPrint(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void OnEndPrintPreview(CDC *aDC, CPrintInfo *aPrintInfo, POINT aPoint, CPreviewView *aView);
    virtual void OnActivateView(xpr_bool_t aActivate, CView *aActivateView, CView *aDeactiveView);

protected:
    // from TabCtrlObserver
    virtual void       onTabInserted(TabCtrl &aTabCtrl, xpr_size_t aTab);
    virtual xpr_bool_t onTabChangingCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab);
    virtual void       onTabChangedCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab);
    virtual void       onTabRemove(TabCtrl &aTabCtrl, xpr_size_t aTab);
    virtual void       onTabRemoved(TabCtrl &aTabCtrl);
    virtual void       onTabRemoveAll(TabCtrl &aTabCtrl);
    virtual void       onTabEmptied(TabCtrl &aTabCtrl);
    virtual void       onTabDoubleClicked(TabCtrl &aTabCtrl, xpr_size_t aTab);
    virtual void       onTabContextMenu(TabCtrl &aTabCtrl, xpr_size_t aTab, const POINT &aPoint);

    // from SplitterObserver
    virtual CWnd *onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn);
    virtual void onSplitterPaneDestroy(Splitter &aSplitter, CWnd *aPaneWnd);
    virtual void onSplitterActivedPane(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn);

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

    // from TabPaneObserver
    virtual void onCreated(TabPane &aTabPane);
    virtual void onDestroyed(TabPane &aTabPane);
    virtual void onExplored(TabPane &aTabPane, xpr_uint_t aId, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy);
    virtual xpr_bool_t onOpenFolder(TabPane &aTabPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onOpenFolder(TabPane &aTabPane, LPITEMIDLIST aFullPidl);
    virtual void onSetFocus(TabPane &aTabPane);
    virtual void onMoveFocus(TabPane &aTabPane, xpr_sint_t aCurWnd);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    afx_msg void OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnCaptureChanged(CWnd *aWnd);
};

#endif // __FX_EXPLORER_VIEW_H__
