//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_DROP_DLG_H__
#define __FX_FILE_SCRAP_DROP_DLG_H__
#pragma once

#include "rgc/BCMenu.h"
#include "rgc/DropTarget.h"

class DlgState;

class FileScrapDropDlg : public CDialog, public DropTargetObserver
{
    typedef CDialog super;

public:
    FileScrapDropDlg(void);

protected:
    void setAlwaysTopMost(xpr_bool_t aAlwaysTopMost);
    void setTransparent(xpr_bool_t aTransparent, xpr_uint_t aPercent);
    xpr_bool_t browse(xpr_tchar_t *aTarget);

protected:
    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *pOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *pOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

protected:
    DropTarget mDropTarget;
    xpr_uint_t mShellIDListClipFormat;

    xpr_sint_t mTransparentPercentage;
    xpr_bool_t mTransparent;
    xpr_bool_t mAlwaysTopMost;

protected:
    BCMenu    mMenu;
    DlgState *mDlgState;

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd);
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(xpr_uint_t nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu);
    afx_msg LRESULT OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu);
    afx_msg void OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnRButtonDblClk(xpr_uint_t nFlags, CPoint point);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnFileScrapCopy(void);
    afx_msg void OnFileScrapMove(void);
    afx_msg void OnFileScrapDelete(void);
    afx_msg void OnFileScrapView(void);
    afx_msg void OnFileScrapRemove(void);
    afx_msg void OnFileScrapClose(void);
    afx_msg void OnClose(void);
    afx_msg void OnFileScrapAlwaysTopMost(void);
    afx_msg void OnFileScrapTransparent0(void);
    afx_msg void OnFileScrapTransparent20(void);
    afx_msg void OnFileScrapTransparent40(void);
    afx_msg void OnFileScrapTransparent60(void);
    afx_msg void OnFileScrapTransparent80(void);
    afx_msg void OnFileScrapClipboardCut(void);
    afx_msg void OnFileScrapClipboardCopy(void);
    afx_msg void OnFileScrapGroupDefault(void);
    afx_msg void OnUpdateFcdCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdMove(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdDelete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdView(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdRemove(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdAlwaysTopMost(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdTransparent(CCmdUI* pCmdUI);
    afx_msg void OnLButtonDblClk(xpr_uint_t nFlags, CPoint point);
    afx_msg void OnUpdateFcdClipboardCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFcdClipboardCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTransparent(CCmdUI *pCmdUI);
};

#endif // __FX_FILE_SCRAP_DROP_DLG_H__
