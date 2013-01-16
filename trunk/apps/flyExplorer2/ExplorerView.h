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
#include "rgc/StatusBarObserver.h"

#include "rgc/DropTarget.h"

#include "FolderPaneObserver.h"
#include "ExplorerCtrlObserver.h"
#include "SearchResultPaneObserver.h"
#include "FileScrapPaneObserver.h"
#include "AddressBarObserver.h"
#include "PathBarObserver.h"
#include "Splitter.h"

class ExplorerViewObserver;
class FolderPane;
class FolderCtrl;
class AddressBar;
class ExplorerCtrl;
class PathBar;
class ActivateBar;
class DrivePathBar;
class ContentsWnd;
class StatusBarEx;
class SearchResultCtrl;
class FileScrapPane;
class DropTarget;
class ListCtrlPrint;

class ExplorerView
    : public CView
    , public TabCtrlObserver
    , public SplitterObserver
    , public FolderPaneObserver
    , public ExplorerCtrlObserver
    , public SearchResultPaneObserver
    , public FileScrapPaneObserver
    , public AddressBarObserver
    , public PathBarObserver
    , public StatusBarObserver
{
    typedef CView super;

    DECLARE_DYNCREATE(ExplorerView)

public:
    ExplorerView(void);
    virtual ~ExplorerView(void);

public:
    void setObserver(ExplorerViewObserver *aObserver);

public:
    void setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    enum TabType
    {
        TabTypeNone,
        TabTypeExplorer,
        TabTypeSearchResult,
        TabTypeFileScrap,
    };

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
    CWnd         *getTabWnd(xpr_sint_t aTab) const;
    xpr_uint_t    getTabWndId(xpr_sint_t aTab) const;
    xpr_sint_t    findTabWndId(xpr_uint_t aTabWndId) const;
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
    ExplorerCtrl     *getExplorerCtrl(xpr_sint_t aTab = -1) const;
    SearchResultCtrl *getSearchResultCtrl(xpr_sint_t aTab = -1) const;
    FileScrapPane    *getFileScrapPane(xpr_sint_t aTab = -1) const;
    AddressBar       *getAddressBar(void) const;
    PathBar          *getPathBar(void) const;
    ActivateBar      *getActivateBar(void) const;
    DrivePathBar     *getDrivePathBar(void) const;
    StatusBar        *getStatusBar(void) const;

    xpr_bool_t visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE, xpr_bool_t aSaveFolderPaneVisible = XPR_TRUE);
    xpr_bool_t isVisibleFolderPane(void) const;
    xpr_bool_t isLeftFolderPane(void) const;
    void       setLeftFolderPane(xpr_bool_t aLeft);

    void updateBookmark(xpr_bool_t aUpdatePosition = XPR_FALSE);
    void enableBookmark(xpr_bool_t aEnable);
    xpr_bool_t isEnableBookmark(void) const;
    xpr_bool_t isVisibleBookmark(void) const;
    void setBookmarkPopup(xpr_bool_t aPopup);
    void setBookmarkColor(COLORREF aBookmarkColor);

    void visibleAddressBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleAddressBar(void) const;

    void setPathBarMode(xpr_bool_t aRealPath);
    void visiblePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisiblePathBar(void) const;

    void visibleActivateBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleActivateBar(void) const;

    void setDrivePathBarLeft(xpr_bool_t aLeft);
    void setDrivePathBarShortText(xpr_bool_t aShortText);
    void visibleDrivePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);

    void setActivateBarStatus(xpr_bool_t aActivate, xpr_bool_t aLastActivated);

    void invalidateContentsWnd(void);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);
    void setContentsStyle(xpr_sint_t aContentsStyle);

    void visibleStatusBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleStatusBar(void) const;
    const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const;

    void print(void);
    void printPreview(void);

    void recalcLayout(void);

    void saveOption(void);

protected:
    xpr_uint_t generateTabWndId(void);

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
    void destroyFolderPane(void);
    void createAddressBar(void);
    void destroyAddressBar(void);
    void createPathBar(void);
    void destroyPathBar(void);
    void createActivateBar(void);
    void destroyActivateBar(void);
    void createDrivePathBar(void);
    void destroyDrivePathBar(void);
    void createContentsWnd(void);
    void destroyContentsWnd(void);
    void createStatusBar(void);
    void destroyStatusBar(void);

    void setStatusBarText(void);
    void setStatusBarDrive(const xpr_tchar_t *aCurPath = XPR_NULL);

    void setStatusPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText);
    void setStatusDisk(const xpr_tchar_t *aPath);
    void setStatusPaneBookmarkText(xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect = 0);
    void setStatusPaneDriveText(xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect = 0);
    void updateStatusBar(xpr_sint_t aTab);
    void updateStatusBar(void);

    xpr_bool_t isVisibleContentsWnd(void) const;

    void setContentsFolder(LPTVITEMDATA aTvItemData);
    void setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate = XPR_FALSE);
    void setContentsSingleItem(LPLVITEMDATA sLvItemData, const xpr_tchar_t *aName = XPR_NULL, const xpr_tchar_t *aType = XPR_NULL, const xpr_tchar_t *aDate = XPR_NULL, const xpr_tchar_t *aSize = XPR_NULL, const xpr_tchar_t *aAttr = XPR_NULL);
    void setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames);

    void OnExpSelNormal(void);
    void OnExpSelSingleItem(void);
    void OnExpSelMultiItem(void);

protected:
    ExplorerViewObserver *mObserver;

    xpr_sint_t     mViewIndex;
    xpr_bool_t     mInit;

    TabCtrl       *mTabCtrl;
    xpr_uint_t     mNextExplorerCtrlId;
    xpr_uint_t     mLastActivedExplorerCtrlId;
    std::tstring   mInitTabFullPath;
    CPoint         mContextMenuCursor;

    Splitter       mSplitter;
    FolderPane    *mFolderPane;
    ContentsWnd   *mContentsWnd;
    AddressBar    *mAddressBar;
    PathBar       *mPathBar;
    ActivateBar   *mActivateBar;
    DrivePathBar  *mDrivePathBar;
    StatusBarEx   *mStatusBar;

    CRect          mMarginRect;
    xpr_bool_t     mIsDrivePathBar;

    ListCtrlPrint *mListCtrlPrint;

    DropTarget     mDropTarget;

    class TabData;
    class ExplorerTabData;
    class SearchResultTabData;
    class FileScrapTabData;

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

    // from ExplorerCtrlObserver
    virtual void onExplore(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy, const xpr_tchar_t *aCurPath);
    virtual void onUpdateStatus(ExplorerCtrl &aExplorerCtrl, xpr_size_t aRealSelCount);
    virtual void onUpdateFreeSize(ExplorerCtrl &aExplorerCtrl);
    virtual void onSetFocus(ExplorerCtrl &aExplorerCtrl);
    virtual void onKillFocus(ExplorerCtrl &aExplorerCtrl);
    virtual void onRename(ExplorerCtrl &aExplorerCtrl);
    virtual void onContextMenuDelete(ExplorerCtrl &aExplorerCtrl);
    virtual void onContextMenuRename(ExplorerCtrl &aExplorerCtrl);
    virtual void onContextMenuPaste(ExplorerCtrl &aExplorerCtrl);
    virtual void onGoDrive(ExplorerCtrl &aExplorerCtrl);
    virtual xpr_sint_t onDrop(ExplorerCtrl &aExplorerCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir);
    virtual xpr_bool_t onGetExecParam(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath, xpr_tchar_t *aParam, xpr_size_t aMaxLen);
    virtual void onExecError(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath);
    virtual void onSetViewStyle(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aStyle, xpr_bool_t aRefresh);
    virtual void onUseColumn(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId);
    virtual void onSortItems(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId, xpr_bool_t aAscending);
    virtual void onSetColumnWidth(ExplorerCtrl &aExplorerCtrl, xpr_sint_t sColumnIndex, xpr_sint_t sWidth);
    virtual void onMoveFocus(ExplorerCtrl &aExplorerCtrl);

    // from SearchResultPaneObserver
    virtual xpr_bool_t onExplore(SearchResultPane &aSearchResultPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onExplore(SearchResultPane &aSearchResultPane, LPITEMIDLIST aFullPidl);
    virtual void onSetFocus(SearchResultPane &aSearchResultPane);

    // from FileScrapPaneObserver
    virtual void onCreated(FileScrapPane &aFileScrapPane);
    virtual void onDestroyed(FileScrapPane &aFileScrapPane);
    virtual void onSetFocus(FileScrapPane &aFileScrapPane);
    virtual xpr_bool_t onExplore(FileScrapPane &aFileScrapPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onExplore(FileScrapPane &aFileScrapPane, LPITEMIDLIST aFullPidl);

    // from AddressBarObserver
    virtual xpr_bool_t onExplore(AddressBar &aAddressBar, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy);
    virtual LPITEMIDLIST onFailExec(AddressBar &aAddressBar, const xpr_tchar_t *aExecPath);
    virtual void onMoveFocus(AddressBar &aAddressBar);

    // from PathBarObserver
    virtual xpr_bool_t onExplore(PathBar &aPathBar, LPITEMIDLIST aFullPidl);

    // from StatusBarObserver
    virtual void onStatusBarRemove(StatusBar &aStatusBar, xpr_size_t aIndex);
    virtual void onStatusBarRemoved(StatusBar &aStatusBar);
    virtual void onStatusBarRemoveAll(StatusBar &aStatusBar);
    virtual void onStatuBarContextMenu(StatusBar &aStatusBar, xpr_size_t aIndex, const POINT &aPoint);
    virtual void onStatusBarDoubleClicked(StatusBar &aStatusBar, xpr_size_t sIndex);

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
