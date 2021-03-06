//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_MAIN_FRAME_H__
#define __FXFILE_MAIN_FRAME_H__ 1
#pragma once

#include "gui/rebar/FrameWndEx.h"
#include "gui/BCMenu.h"
#include "gui/DropTarget.h"

#include "fxfile_def.h"
#include "removable_device.h"
#include "folder_view_observer.h"
#include "explorer_view_observer.h"
#include "search_dlg_observer.h"
#include "splitter.h"

class SysTray;

namespace fxfile
{
class FolderView;
class FolderPane;
class FolderCtrl;
class ExplorerView;
class ExplorerPane;
class ExplorerCtrl;
class SearchResultCtrl;
class FileScrapPane;
class AddressBar;
class PictureViewer;
class FileScrapDropDlg;
class SearchDlg;
class SyncDirs;

namespace cmd
{
class CommandExecutor;
class CommandParameters;
} // namespace cmd

class MainFrame
    : public FrameWndEx
    , public SplitterObserver
    , public FolderViewObserver
    , public ExplorerViewObserver
    , public RemovableDeviceObserver
    , public SearchDlgObserver
{
    typedef FrameWndEx super;

    DECLARE_DYNCREATE(MainFrame)

public:
    MainFrame(void);
    virtual ~MainFrame(void);

public:
    xpr_bool_t confirmToClose(xpr_bool_t aForce = XPR_FALSE);
    void destroy(void);

public:
    Splitter         *getSplitter(void) const;
    FolderView       *getFolderView(void) const;
    FolderPane       *getFolderPane(xpr_sint_t aIndex = -1) const;
    ExplorerView     *getExplorerView(xpr_sint_t aIndex = -1) const;
    ExplorerPane     *getExplorerPane(xpr_sint_t aIndex = -1) const;

    AddressBar       *getAddressBar(xpr_sint_t aIndex = -1) const;
    FolderCtrl       *getFolderCtrl(xpr_sint_t aIndex = -1) const;
    ExplorerCtrl     *getExplorerCtrl(xpr_sint_t aIndex = -1) const;
    ExplorerCtrl     *getExplorerCtrl(FolderCtrl *aFolderCtrl) const;
    SearchResultCtrl *getSearchResultCtrl(xpr_sint_t aIndex = -1) const;
    FileScrapPane    *getFileScrapPane(void) const;
    void              setFileScrapPane(FileScrapPane *aFileScrapPane);
    DriveToolBar     *getDriveBar(void) const;

    void ShowControlBar(CControlBar *aControlBar, xpr_bool_t aShow, xpr_bool_t aDelay);

    xpr_bool_t isNoneFolderPane(void) const;
    xpr_bool_t isVisibleFolderPane(void) const;
    xpr_bool_t isSingleFolderPaneMode(void) const;
    xpr_bool_t isLeftFolderPane(void) const;
    void showFolderPane(xpr_bool_t aShow);
    void setSingleFolderPaneMode(xpr_bool_t aSingleFolderPaneMode);
    void setNoneFolderPane(void);
    void setLeftFolderPane(xpr_bool_t aLeft);

    SysTray *createTray(void);
    void destroyTray(void);
    void minimizeToTray(void);
    void exitTrayApp(void);
    void showTrayMainFrame(void);

    void splitView(xpr_sint_t aRowCount, xpr_sint_t aColumnCount, xpr_bool_t aLoading = XPR_FALSE);
    void setSplitEqually(void);
    void setSplitRatio(xpr_double_t aRatio);
    xpr_bool_t isSingleView(void) const;
    void getViewSplit(xpr_sint_t &aRowCount, xpr_sint_t &aColumnCount) const;
    xpr_sint_t getViewIndex(void) const;
    xpr_sint_t getViewCount(void) const;
    xpr_bool_t getViewIndexFromViewSplit(xpr_sint_t aRowCount, xpr_sint_t aColumnCount, xpr_sint_t aRow, xpr_sint_t aColumn, xpr_sint_t &aViewIndex) const;
    xpr_bool_t getViewSplitFromViewIndex(xpr_sint_t aViewIndex, xpr_sint_t aRowCount, xpr_sint_t aColumnCount, xpr_sint_t &aRow, xpr_sint_t &aColumn) const;
    LPITEMIDLIST getSplitFolder(xpr_sint_t aIndex) const;

    xpr_sint_t getActiveView(void) const;
    xpr_sint_t getLastActivedView(void) const;
    void setActiveView(xpr_sint_t aViewIndex);
    void setActiveView(xpr_sint_t aRow, xpr_sint_t aColumn);

    xpr_sint_t addBookmark(LPITEMIDLIST aFullPidl, xpr_sint_t aInsert = -1);
    void updateBookmark(xpr_bool_t aAllBookmarks = XPR_TRUE);

    void gotoBookmark(xpr_sint_t aBookmarkIndex);
    void gotoDrive(xpr_tchar_t aDriveChar);
    void goInitFolder(xpr_sint_t aIndex = -1);
    xpr_bool_t goWorkingFolder(xpr_sint_t aIndex);
    xpr_bool_t setWorkingFolder(xpr_size_t aIndex, LPITEMIDLIST aFullPidl);
    void resetWorkingFolder(xpr_size_t aIndex);
    void resetWorkingFolder(void);

    xpr_bool_t isDriveBar(void) const;
    xpr_bool_t isDriveShortText(void) const;
    void setDriveBar(xpr_bool_t aVisible);

    void setDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath);
    const xpr_tchar_t *getDrivePath(xpr_sint_t aIndex, xpr_tchar_t aDriveChar) const;
    const xpr_tchar_t *getDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath) const;
    const xpr_tchar_t *getDrivePath(xpr_sint_t aIndex, xpr_uint_t aId) const;

    HACCEL getAccelTable(void) const { return m_hAccelTable; }
    void setAccelerator(ACCEL *aAccel, xpr_sint_t aCount);

    void recalcLayout(xpr_uint_t aType = 0);

    void setMainTitle(xpr_tchar_t *aTitle);
    void setMainTitle(LPITEMIDLIST aFullPidl);
    void setMainTitle(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);

    void compareWindow(void);

    void moveFocus(xpr_sint_t aCurWnd);
    void moveFocus(xpr_sint_t aCurWnd, xpr_bool_t aShift, xpr_bool_t aCtrl);

    xpr_bool_t canExecute(xpr_uint_t aCommandId, CCmdUI *aCmdUI, CWnd *aTargetWnd = XPR_NULL, cmd::CommandParameters *aParameters = XPR_NULL);
    xpr_bool_t executeCommand(xpr_uint_t aCommandId, CWnd *aTargetWnd = XPR_NULL, cmd::CommandParameters *aParameters = XPR_NULL);

    xpr_bool_t isShowFileScrapDrop(void) const;
    void toggleFileScrapDrop(void);

    void popupBookmarkMenu(void);
    void popupWorkingFolderMenu(void);

    xpr_bool_t isVisiblePictureViewer(void) const;
    void togglePictureViewer(void);

    xpr_bool_t isVisibleSearchDlg(void) const;
    void       showSearchDlg(LPITEMIDLIST aFullPidl);
    xpr_bool_t insertSearchLocation(LPITEMIDLIST aFullPidl);
    void       destroySearchDlg(void);

    void setOption(Option &aOption);
    void setChangedOption(Option &aOption);
    void saveAllOptions(void);

    xpr_bool_t isPreviewMode(void) const;

public:
    Splitter          mSingleFolderSplitter;
    Splitter          mSplitter;

    PictureViewer    *mPictureViewer;
    SearchDlg        *mSearchDlg;
    FileScrapDropDlg *mFileScrapDropDlg;

protected:
    void loadAccelTable(void);
    void saveAccelTable(void);

    virtual void init(void);
    void saveOption(void);

    void trackBookmarkPopupMenu(xpr_sint_t x, xpr_sint_t y, LPCRECT aRect = XPR_NULL);
    void trackWorkingFolderPopupMenu(xpr_sint_t x, xpr_sint_t y, LPCRECT aRect = XPR_NULL);
    xpr_sint_t insertBookmarkPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertDrivePopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoUpPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoBackwardPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoForwardPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoHistoryPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoWorkingFolderPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoWorkingFolderSetPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertGoWorkingFolderResetPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertRecentFileListPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);
    xpr_sint_t insertShellNewPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert);

    void getClientRectExecptForControlBars(CRect &aRect);

    void initFolderCtrl(void);

protected:
    // from SplitterObserver
    virtual CWnd *onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn);
    virtual void onSplitterPaneDestroy(Splitter &aSplitter, CWnd *aPaneWnd);
    virtual void onSplitterActivedPane(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn);

    // from RemovableDeviceObserver
    virtual xpr_bool_t OnQueryRemove(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet);
    virtual xpr_bool_t OnQueryRemoveFailed(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet);
    virtual void OnRemoveComplete(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet);

    // from FolderViewObserver
    virtual xpr_bool_t onExplore(FolderView &aFolderView, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy);
    virtual void onRename(FolderView &aFolderView, HTREEITEM aTreeItem);
    virtual void onContextMenuDelete(FolderView &aFolderView, HTREEITEM aTreeItem);
    virtual void onContextMenuRename(FolderView &aFolderView, HTREEITEM aTreeItem);
    virtual void onContextMenuPaste(FolderView &aFolderView, HTREEITEM aTreeItem);
    virtual xpr_sint_t onDrop(FolderView &aFolderView, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir);
    virtual void onMoveFocus(FolderView &aFolderView, xpr_sint_t aCurWnd);

    // from ExplorerViewObserver
    virtual void onMoveFocus(ExplorerView &aExplorerView, xpr_sint_t aCurWnd);

    // from SearchDlgObserver
    virtual void onPostNcDestroy(SearchDlg &aSearchDlg);

protected:
    cmd::CommandExecutor *mCommandExecutor;

    xpr_sint_t  mAccelCount;
    ACCEL       mAccel[MAX_ACCEL];

    DropTarget  mDropTarget;
    BCMenu      mMenu;
    SysTray    *mSysTray;
    xpr_bool_t  mExit;
    xpr_bool_t  mPreviewMode;

    FileScrapPane *mFileScrapPane;

    // Compare Directories
    SyncDirs *mCompareDirs;

    typedef std::map<xpr_tchar_t, xpr::string> DrivePathMap;
    DrivePathMap mDrivePathMap[MAX_VIEW_SPLIT];

    LPITEMIDLIST mSplitFullPidl[2];

public:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);
    virtual xpr_bool_t DestroyWindow(void);
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);
    virtual void GetMessageString(xpr_uint_t aId, CString &aMessage) const;
    virtual void ActivateFrame(xpr_sint_t aCmdShow = -1);
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);
    virtual void WinHelp(DWORD aData, xpr_uint_t aCmd = HELP_CONTEXT);
    virtual xpr_bool_t LoadFrame(xpr_uint_t aIdResource, DWORD aDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd *aParentWnd = XPR_NULL, CCreateContext *aCreateContext = XPR_NULL);
    virtual void RecalcLayout(xpr_bool_t aNotify = XPR_TRUE);
    virtual void OnSetPreviewMode(xpr_bool_t aPreview, CPrintPreviewState *aState);
    virtual CWnd* GetMessageBar(void);

protected:
    virtual LRESULT WindowProc(xpr_uint_t aMessage, WPARAM wParam, LPARAM lParam);
    virtual xpr_bool_t OnCreateClient(LPCREATESTRUCT aCreateStruct, CCreateContext *aCreateContext);
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

#ifdef XPR_CFG_BUILD_DEBUG
    virtual void AssertValid(void) const;
    virtual void Dump(CDumpContext &aDumpContext) const;
#endif

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnToolbarDropDown(NMHDR *aNmHdr, LRESULT *aLResult);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnClose(void);
    afx_msg void OnSysCommand(xpr_uint_t aId, LPARAM lParam);
    afx_msg void OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct);
    afx_msg LRESULT OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg void OnParentNotify(xpr_uint_t aMessage, LPARAM lParam);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO *aMinMaxInfo);
    afx_msg void OnTimer(UINT_PTR aIdEvent);
    afx_msg void OnActivateApp(xpr_bool_t aActive, DWORD aThreadId);
    afx_msg void OnEndSession(xpr_bool_t aEnding);
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSingleProcess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskRestarted(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCompareDirsStatus(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnCaptureChanged(CWnd *aWnd);
};
} // namespace fxfile

#endif // __FXFILE_MAIN_FRAME_H__
