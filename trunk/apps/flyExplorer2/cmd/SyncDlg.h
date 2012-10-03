//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SYNC_DLG_H__
#define __FX_SYNC_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"
#include "rgc/BCMenu.h"

class DlgState;

namespace fxb
{
class SyncDirs;
} // namespace fxb

class SyncDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    SyncDlg(void);
    virtual ~SyncDlg(void);

public:
    void setDir(const xpr_tchar_t *aDir1, const xpr_tchar_t *aDir2);

protected:
    xpr_bool_t getPath(xpr_sint_t aIndex, xpr_tchar_t *aPath, xpr_sint_t aMaxPathLen);

    void view(void);
    void scan(void);
    void compare(void);
    void synchronize(void);

    enum State
    {
        StateNone,
        StateCompare,
        StateSynchronize,
    };

    void setControlState(State aState = StateNone);
    void selectListItem(xpr_sint_t aIndex, xpr_bool_t aUnselAll = XPR_FALSE, xpr_bool_t aFocus = XPR_FALSE);
    void selectListAllItem(xpr_bool_t aFocus = XPR_FALSE);
    void setItemSync(xpr_uint_t aSync);
    void setItemOriginalSync(void);
    void setStatus(const xpr_tchar_t *aStatusText);

protected:
    BCMenu         mMenu;
    CImageList     mSyncImgList;

    std::tstring   mDir[2];
    fxb::SyncDirs *mSyncDirs;
    xpr_bool_t     mStop;

    xpr_sint_t     mPathOffset[3];
    xpr_sint_t     mPathBrowseWidth;

protected:
    HICON      mIcon;
    DlgState  *mDlgState;
    CComboBox  mPathComboBox[2];
    CListCtrl  mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnSizeItem(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy, HDWP hdwp);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    afx_msg void OnPath1Browse(void);
    afx_msg void OnPath2Browse(void);
    afx_msg void OnCompare(void);
    afx_msg void OnSynchronize(void);
    afx_msg LRESULT OnSyncStatus(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint pt);
    afx_msg LRESULT OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu);
    afx_msg void OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSyncLeft(void);
    afx_msg void OnSyncRight(void);
    afx_msg void OnSyncNone(void);
    afx_msg void OnSyncOriginal(void);
    afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPrevDiff(void);
    afx_msg void OnNextDiff(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnDestroy(void);
};

#endif // __FX_SYNC_DLG_H__
