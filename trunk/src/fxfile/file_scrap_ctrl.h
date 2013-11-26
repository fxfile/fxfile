//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_CTRL_H__
#define __FXFILE_FILE_SCRAP_CTRL_H__ 1
#pragma once

#include "shell_change_notify.h"
#include "file_scrap.h"

#include "gui/BCMenu.h"
#include "gui/ListCtrlEx.h"

namespace fxfile
{
class ShellIcon;
class ContextMenu;

class FileScrapCtrlObserver;
class FlatHeaderCtrl;

class FileScrapCtrl : public ListCtrlEx
{
    typedef ListCtrlEx super;

public:
    FileScrapCtrl(void);
    virtual ~FileScrapCtrl(void);

public:
    void setObserver(FileScrapCtrlObserver *aObserver);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void initList(FileScrap::Group *aGroup, xpr_bool_t aRedraw = XPR_TRUE);
    void clearList(void);

    void removeItems(const FileScrap::IdSet &aIdSet);

public:
    void sortItems(xpr_sint_t aColumn);
    void sortItems(xpr_sint_t aColumn, xpr_bool_t aAscending);

    void refresh(void);

    void getStatus(xpr_sint_t &aFileCount, xpr_sint_t &aDirCount) const;

    xpr_bool_t getSelFullPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount);
    xpr_bool_t getSelPidls(LPSHELLFOLDER *aShellFolder, LPCITEMIDLIST *aPidls, xpr_sint_t &aCount);
    void getDispFileName(const xpr_tchar_t *aFileName, xpr_tchar_t *aDispFileName);
    void getSelFileScrapItemIdSet(FileScrap::IdSet &sIdSet);

    void selectAll(void);
    void unselectAll(void);

    // item execution
    void execute(xpr_sint_t aIndex);
    void executeAllSelItems(void);

protected:
    class ItemData;

    void updateStatus(void);

protected:
    xpr_bool_t setDataObject(LPDATAOBJECT *aDataObject, xpr_uint_t aShellAttributes);
    void beginDragDrop(NM_LISTVIEW *aNmListView);

    void executeSelFolder(const xpr_tchar_t *aPath);
    void executeLink(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void executeUrl(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void executeLinkFolder(LPCITEMIDLIST aFullPidl);
    void doExecuteError(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void doExecuteError(const xpr_tchar_t *aPath);

    xpr_bool_t canInvokeCommandSelf(xpr_uint_t aId);
    void       invokeCommandSelf(ContextMenu &aContextMenu, xpr_uint_t aId);
    static xpr_sint_t CALLBACK DefaultItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    xpr_sint_t findItemSignature(xpr_uint_t aSignature);

protected:
    FileScrapCtrlObserver *mObserver;

    BCMenu mMenu;

    xpr_sint_t mSortColumn;
    xpr_bool_t mSortAscending;

    ShellIcon *mShellIcon;

    FileScrap::Group *mGroup;
    xpr_sint_t   mFileCount;
    xpr_sint_t   mDirCount;

    static xpr_uint_t mCodeMgr;
    xpr_uint_t   mCode;
    xpr_uint_t   mSignature;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnCustomdraw(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    afx_msg void OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnItemActivate(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg xpr_sint_t OnMouseActivate(CWnd *aDesktopWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg void OnHScroll(xpr_uint_t aSBCode, xpr_uint_t aPos, CScrollBar *aScrollBar);
    afx_msg void OnDestroy(void);
    afx_msg void OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct);
    afx_msg LRESULT OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu);
    afx_msg void OnColumnclick(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnKeyDown(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags);
    afx_msg void OnLvnKeyDown(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg LRESULT OnShellAsyncIcon(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_CTRL_H__
