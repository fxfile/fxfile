//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_EXPLORER_PANE_H__
#define __FX_EXPLORER_PANE_H__
#pragma once

#include "rgc/DropTarget.h"
#include "rgc/StatusBarObserver.h"

#include "TabPane.h"
#include "ExplorerCtrlObserver.h"
#include "AddressBarObserver.h"
#include "PathBarObserver.h"

class AddressBar;
class ExplorerCtrl;
class PathBar;
class ActivateBar;
class DrivePathBar;
class ContentsWnd;
class ExplorerStatusBar;
class DropTarget;
class ListCtrlPrint;

class ExplorerPane
    : public TabPane
    , public ExplorerCtrlObserver
    , public AddressBarObserver
    , public PathBarObserver
    , public StatusBarObserver
{
    typedef TabPane super;

public:
    ExplorerPane(void);
    virtual ~ExplorerPane(void);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    // from TabPane
    virtual xpr_bool_t isSharedPane(void) const;
    virtual xpr_bool_t canDuplicatePane(void) const;

    virtual CWnd *     newSubPane(xpr_uint_t aId);
    virtual CWnd *     getSubPane(xpr_uint_t aId = InvalidId) const;
    virtual xpr_size_t getSubPaneCount(void) const;
    virtual xpr_uint_t getCurSubPaneId(void) const;
    virtual xpr_uint_t setCurSubPane(xpr_uint_t aId);
    virtual void       destroySubPane(xpr_uint_t aId);
    virtual void       destroySubPane(void);

    virtual StatusBar *getStatusBar(void) const;
    virtual const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const;

    virtual void setChangedOption(Option &aOption);

public:
    ExplorerCtrl *getExplorerCtrl(xpr_uint_t aId = InvalidId) const;
    AddressBar   *getAddressBar(void) const;
    PathBar      *getPathBar(void) const;
    DrivePathBar *getDrivePathBar(void) const;

    void updateBookmark(xpr_bool_t aUpdatePosition = XPR_FALSE);
    void enableBookmark(xpr_bool_t aEnable);
    xpr_bool_t isEnabledBookmark(void) const;

    void visibleAddressBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleAddressBar(void) const;

    void visiblePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisiblePathBar(void) const;

    void visibleDrivePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    void refreshDrivePathBar(void);

    void invalidateContentsWnd(void);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);
    void setContentsStyle(xpr_bool_t aShowInfoBar, xpr_sint_t aContentsStyle);

    void visibleStatusBar(xpr_bool_t aVisible, xpr_bool_t aLoading = XPR_FALSE);
    xpr_bool_t isVisibleStatusBar(void) const;

    void recalcLayout(void);

    void saveOption(void);

    void updateStatusBar(xpr_uint_t aId);

protected:
    class ExplorerCtrlData;

    xpr_bool_t createAddressBar(void);
    xpr_bool_t createPathBar(void);
    xpr_bool_t createDrivePathBar(void);
    xpr_bool_t createContentsWnd(void);
    xpr_bool_t createStatusBar(void);
    void       destroyAddressBar(void);
    void       destroyPathBar(void);
    void       destroyDrivePathBar(void);
    void       destroyContentsWnd(void);
    void       destroyStatusBar(void);

    void setExplorerOption(ExplorerCtrl *aExplorerCtrl, const Option &aOption);

    void setStatusBarText(void);
    void setStatusBarDrive(const xpr_tchar_t *aCurPath = XPR_NULL);

    void setStatusPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText);
    void setStatusDisk(const xpr_tchar_t *aPath);
    void setStatusPaneBookmarkText(xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect = 0);
    void setStatusPaneDriveText(xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect = 0);
    void updateStatusBar(void);

    xpr_bool_t isVisibleContentsWnd(void) const;

    void setContentsFolder(LPTVITEMDATA aTvItemData);
    void setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate = XPR_FALSE);
    void setContentsSingleItem(LPLVITEMDATA sLvItemData, const xpr_tchar_t *aName = XPR_NULL, const xpr_tchar_t *aType = XPR_NULL, const xpr_tchar_t *aDate = XPR_NULL, const xpr_tchar_t *aSize = XPR_NULL, const xpr_tchar_t *aAttr = XPR_NULL);
    void setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames);

    void OnExpSelNormal(void);
    void OnExpSelSingleItem(void);
    void OnExpSelMultiItem(void);

    ExplorerCtrlData *getExplorerCtrlData(xpr_uint_t aId = InvalidId) const;

protected:
    typedef std::map<xpr_uint_t, ExplorerCtrlData *> ExplorerCtrlMap;
    ExplorerCtrlMap mExplorerCtrlMap;
    xpr_uint_t      mCurExplorerCtrlId;

    ContentsWnd       *mContentsWnd;
    AddressBar        *mAddressBar;
    PathBar           *mPathBar;
    DrivePathBar      *mDrivePathBar;
    ExplorerStatusBar *mStatusBar;

    xpr_sint_t     mContentsStyle;
    CRect          mMarginRect;
    xpr_bool_t     mIsDrivePathBar;

    DropTarget     mDropTarget;

protected:
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);

protected:
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
};

#endif // __FX_EXPLORER_PANE_H__
