//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_RESULT_CTRL_H__
#define __FXFILE_SEARCH_RESULT_CTRL_H__ 1
#pragma once

#include "shell_change_notify.h"

#include "gui/BCMenu.h"
#include "gui/ListCtrlEx.h"

namespace fxfile
{
class ShellIcon;
class ContextMenu;

class SearchResultCtrlObserver;
class FlatHeaderCtrl;

class SrItemData
{
public:
    xpr_bool_t getPath(xpr::tstring &aPath)
    {
        aPath = mDir;
        if (aPath[aPath.length() - 1] != XPR_STRING_LITERAL('\\'))
            aPath += XPR_STRING_LITERAL('\\');
        aPath += mFileName;

        return XPR_TRUE;
    }

    xpr_bool_t getPath(xpr_tchar_t *aPath)
    {
        if (XPR_IS_NULL(aPath))
            return XPR_FALSE;

        _tcscpy(aPath, mDir);
        if (aPath[_tcslen(aPath) - 1] != XPR_STRING_LITERAL('\\'))
            _tcscat(aPath, XPR_STRING_LITERAL("\\"));
        _tcscat(aPath, mFileName);

        return XPR_TRUE;
    }

public:
    xpr_uint_t   mSignature;
    xpr_tchar_t *mDir;
    xpr_tchar_t *mFileName;
    xpr_uint64_t mFileSize;
    FILETIME     mModifiedFileTime;
    DWORD        mFileAttributes;
};

class SearchResultCtrl : public ListCtrlEx
{
    typedef ListCtrlEx super;

public:
    SearchResultCtrl(void);
    virtual ~SearchResultCtrl(void);

public:
    void setObserver(SearchResultCtrlObserver *aObserver);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void onStartSearch(void);
    void onSearch(const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData);
    void onEndSearch(void);

    void addList(const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData);
    void addList(SrItemData *aSrItemData);
    void insertList(xpr_sint_t aIndex, const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData);
    void insertList(xpr_sint_t aIndex, SrItemData *aSrItemData);
    void clearList(void);

    void sortItems(xpr_sint_t aColumn);
    void sortItems(xpr_sint_t aColumn, xpr_bool_t aAscending);

    void refresh(void);

    void getResultInfo(xpr_sint_t &aResultFileCount, xpr_sint_t &aResultDirCount, xpr_uint64_t &aResultTotalSize) const;

    xpr_bool_t getSelFullPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount);
    xpr_bool_t getSelPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount);
    void getDispFileName(const xpr_tchar_t *aFileName, xpr_tchar_t *aDispFileName);

    void renameDirectly(xpr_sint_t aIndex);

    void selectAll(void);
    void unselectAll(void);

    // item execution
    void execute(xpr_sint_t aIndex);
    void executeAllSelItems(void);

protected:
    xpr_bool_t setDataObject(LPDATAOBJECT *aDataObject, xpr_uint_t aShellAttributes);
    void beginDragDrop(NM_LISTVIEW *aNmListView);

    void executeSelFolder(const xpr_tchar_t *aPath);
    void executeLink(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void executeUrl(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void executeLinkFolder(LPCITEMIDLIST aFullPidl);
    void doExecuteError(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    void doExecuteError(const xpr_tchar_t *aPath);

    void OnShcnRename(Shcn *aShcn);
    void OnShcnUpdateDir(Shcn *aShcn);

    void setChangeNotify(xpr_bool_t aNotify = XPR_TRUE);

    xpr_bool_t canInvokeCommandSelf(xpr_uint_t aId);
    xpr_bool_t canInvokeCommandSelf(const xpr_tchar_t *aVerb);
    void       invokeCommandSelf(ContextMenu &aContextMenu, xpr_uint_t aId, const xpr_tchar_t *aVerb);
    static xpr_sint_t CALLBACK DefaultItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    xpr_sint_t findItemSignature(xpr_uint_t aSignature);

protected:
    SearchResultCtrlObserver *mObserver;

    BCMenu mMenu;

    xpr_sint_t mSortColumn;
    xpr_bool_t mSortAscending;

    ShellIcon *mShellIcon;

    typedef std::deque<SrItemData *> ResultDeque;
    ResultDeque  mResultDeque;
    xpr_sint_t   mResultFileCount;
    xpr_sint_t   mResultDirCount;
    xpr_uint64_t mResultTotalSize;
    xpr::Mutex   mMutex;
    xpr_bool_t   mNotify;

    static xpr_uint_t mCodeMgr;
    xpr_uint_t   mCode;
    xpr_uint_t   mSignature;

    xpr_tchar_t *mEditExt;
    xpr_sint_t   mEditSel;
    xpr_bool_t   mMouseEdit;

    xpr_uint_t   mShcnId;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);
    virtual xpr_bool_t OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo);

protected:
    virtual xpr_bool_t OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *aResult);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR *aWindowPos);
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult);
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
    afx_msg void OnBeginlabeledit(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnEndlabeledit(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnColumnclick(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnKeyDown(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags);
    afx_msg void OnLvnKeyDown(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnTimer(UINT_PTR aIdEvent);
    afx_msg LRESULT OnShellChangeNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnShellAsyncIcon(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_SEARCH_RESULT_CTRL_H__
