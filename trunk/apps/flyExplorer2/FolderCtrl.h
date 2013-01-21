//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FOLDER_CTRL_H__
#define __FX_FOLDER_CTRL_H__
#pragma once

#include "fxb/fxb_shell_change_notify.h"
#include "rgc/TreeCtrlEx.h"
#include "rgc/DropTarget.h"

namespace fxb
{
class ContextMenu;
class ShellIcon;
} // namespace fxb

class FolderCtrlObserver;

class FolderCtrl : public TreeCtrlEx, public DropTargetObserver
{
    typedef TreeCtrlEx super;

public:
    FolderCtrl(void);
    virtual ~FolderCtrl(void);

public:
    void setObserver(FolderCtrlObserver *aObserver);

    void       setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    xpr_bool_t init(LPITEMIDLIST aRootFullPidl, xpr_bool_t aSelDefItem, const xpr_tchar_t *aSelFullPath = XPR_NULL, xpr_bool_t *aSelItem = XPR_NULL);
    xpr_bool_t explore(HTREEITEM aTreeItem);
    void       refresh(HTREEITEM aTreeItem);
    void       setUpdate(xpr_bool_t aUpdate);
    xpr_bool_t isInit(void) const { return mInit; }

    HTREEITEM searchTree(const CString &aSearchName, xpr_bool_t aWithinSelItem, xpr_bool_t aExpand);
    HTREEITEM searchSel(LPITEMIDLIST aFullPidl, xpr_bool_t aUpdate = XPR_FALSE);

    LPARAM GetItemData(HTREEITEM aTreeItem) const;

    void getFullPath(HTREEITEM aTreeItem, std::tstring &aFullPath) const;
    void getFullPath(HTREEITEM aTreeItem, xpr_tchar_t *aFullPath, xpr_size_t aMaxLen) const;
    void getCurPath(xpr_tchar_t *aPath) const;
    void getCurPath(std::tstring &aPath) const;

    void setHiddenSystem(xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem);

    void renameDirectly(HTREEITEM aTreeItem);

    xpr_bool_t isFileSystem(void) const;
    xpr_bool_t hasSelShellAttributes(xpr_ulong_t aShellAttributes) const;

    void setImageList(CImageList *aSmallImgList);

    // go sibling up and down
    void goSiblingUp(void);
    void goSiblingDown(void);

    xpr_bool_t getSelItemPath(xpr_tchar_t **aPaths, xpr_sint_t &aCount, xpr_ulong_t aShelllAttributes/* = 0*/) const;

    // Expand & Collapse
    void expandAll(HTREEITEM aTreeItem);
    void collapseAll(HTREEITEM aTreeItem);

    void setCustomFont(xpr_tchar_t *aFontText);
    void setCustomFont(CFont *aFont);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);
    void setChangeNotify(std::tstring aPath, xpr_bool_t aAllSubTree, xpr_bool_t aEnable = XPR_TRUE);
    void changeRecursiveNotify(HTREEITEM aTreeItem, xpr_bool_t aAllSubTree, xpr_bool_t aEnable);

    xpr_sint_t getClipFormatCount(void) const;
    void       getClipFormat(CLIPFORMAT *aClipFormat) const;
    xpr_bool_t getDataObject(LPDATAOBJECT *aDataObject) const;

protected:
    xpr_bool_t exploreItem(LPTVITEMDATA aTvItemData);

    struct EnumData;

    typedef void (FolderCtrl::*FailEnumFunc)(EnumData *aEnumData);
    typedef void (FolderCtrl::*PreEnumFunc)(EnumData *aEnumData);
    typedef xpr_bool_t (FolderCtrl::*ProcItemFunc)(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, HTREEITEM aParentTreeItem, EnumData *aEnumData);
    typedef void (FolderCtrl::*PostEnumFunc)(EnumData *aEnumData);

    // Default
    xpr_bool_t enumItem(
        LPSHELLFOLDER  aShellFolder,
        LPITEMIDLIST   aFullPidl,
        HTREEITEM      aParentTreeItem,
        FailEnumFunc   aFailEnumFunc,
        PreEnumFunc    aPreEnumFunc,
        ProcItemFunc   aProcItemFunc,
        PostEnumFunc   aPostEnumFunc,
        EnumData      *aEnumData);

    void       FailFillItem(EnumData *aEnumData);
    void       PreFillItem(EnumData *aEnumData);
    xpr_bool_t FillItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, HTREEITEM aParentTreeItem, EnumData *aEnumData);
    void       PostFillItem(EnumData *aEnumData);

    HTREEITEM insertPartialNetComRootItem(LPITEMIDLIST aFullPidl);
    void      getTreeIcon(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aFullPidl, LPITEMIDLIST aPidl, LPTVITEM aTvItem) const;

    void sortItem(HTREEITEM aParentTreeItem);
    static xpr_sint_t CALLBACK TreeViewCompareProc(LPARAM, LPARAM, LPARAM);

    // Shell Context Menu
    void       trackContextMenu(xpr_bool_t aRightClick);
    xpr_bool_t invokeCommandSelf(fxb::ContextMenu *aContextMenu, xpr_uint_t aId, HTREEITEM aTreeItem);

    typedef std::deque<HTREEITEM> ScanDeque;
    void expandRecursiveAll(HTREEITEM aTreeItem);
    void scanRecursiveExist(ScanDeque &aScanDeque, HTREEITEM aTreeItem);

    void showHiddenSystem(HTREEITEM aTreeItem, xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem);
    void hideHiddenSystem(HTREEITEM aTreeItem, xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem);

    // Shell Change Notify
    void       enumShChangeNotify(HTREEITEM aParentTreeItem, fxb::ShNotifyInfo *aShNotifyInfo); // Shell Change Notify
    void       OnShcnPreEnum(EnumData *aEnumData);
    xpr_bool_t OnShcnEnum(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, HTREEITEM aParentTreeItem, EnumData *aEnumData);
    void       OnShcnPostEnum(EnumData *aEnumData);

    xpr_bool_t OnShcnCreateItem(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnRenameItem(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnDeleteItem(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnUpdateDir(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnNetShare(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnDriveAdd(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnDriveRemove(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnEnumUpdateDir(LPTVITEMDATA aTvItemData, HTREEITEM aParentTreeItem, fxb::ShNotifyInfo *aShNotifyInfo);
    xpr_bool_t OnShcnUpdateItem(fxb::Shcn *aShcn);
    xpr_bool_t OnShcnEnumNetShare(LPTVITEMDATA aTvItemData, HTREEITEM aTreeItem, fxb::ShNotifyInfo *aShNotifyInfo);
    void OnShcnUpdateImage(HTREEITEM aTreeItem, xpr_sint_t aImageIndex);
    xpr_bool_t OnShcnUpdateDrive(const xpr_tchar_t *aDrive);
    xpr_bool_t OnShcnMediaInsRem(fxb::Shcn *aShcn, xpr_bool_t aInserted);

    xpr_bool_t beginShcn(xpr_slong_t aEventId, xpr_tchar_t *aFullPath1, xpr_tchar_t *aFullPath2, fxb::ShNotifyInfo *aShNotifyInfo);
    xpr_bool_t endShcn(HTREEITEM aTreeItem);

    // Drag & Drop, and Clipboard
    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

    void beginDragDrop(NM_TREEVIEW *aNmTreeView);

    xpr_bool_t isChildItem(HTREEITEM aTreeItem, HTREEITEM aChildTreeItem);

protected:
    xpr_sint_t mViewIndex;

    FolderCtrlObserver *mObserver;

    xpr_bool_t  mInit;
    xpr_bool_t  mUpdate;

    CImageList     *mSmallImgList;
    fxb::ShellIcon *mShellIcon;

    // Drag & Drop, and Clipboard
    DropTarget  mDropTarget;

    CFont      *mCustomFont;

    xpr_bool_t  mMouseEdit;
    xpr_bool_t  mSkipExpand;

    xpr_bool_t  mSelTimer;
    HTREEITEM   mTimerSelTreeItem;
    HTREEITEM   mOldSelTreeItem;

    xpr_uint_t  mShcnId;

private:
    xpr_bool_t  mScrollUpTimer;
    xpr_bool_t  mScrollDownTimer;
    xpr_bool_t  mScrollLeftTimer;
    xpr_bool_t  mScrollRightTimer;

    HTREEITEM   mDropTreeItem;                  // Drag Hit Item
    HTREEITEM   mDragExpandTreeItem;            // Drag Exapnd Item

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);

protected:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnRclick(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnSelchanged(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnItemexpanding(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnItemExpanded(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKeyDown(xpr_uint_t nChar, xpr_uint_t nRepCnt, xpr_uint_t nFlags);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSelchanging(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnBeginlabeledit(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnEndlabeledit(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnSysColorChange(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnTimer(xpr_uint_t nIDEvent);
    afx_msg void OnPaint(void);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnKeydown(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg xpr_sint_t OnMouseActivate(CWnd* pDesktopWnd, xpr_uint_t nHitTest, xpr_uint_t message);
    afx_msg void OnMButtonDown(xpr_uint_t nFlags, CPoint point);
    afx_msg void OnCustomdraw(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnClick(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg LRESULT OnShellChangeNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileChangeNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnShellAsyncIcon(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_FOLDER_CTRL_H__
