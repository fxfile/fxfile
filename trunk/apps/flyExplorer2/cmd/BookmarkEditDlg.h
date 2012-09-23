//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BOOKMARK_EDIT_DLG_H__
#define __FX_BOOKMARK_EDIT_DLG_H__
#pragma once

#include "fxb/fxb_bookmark.h"
#include "rgc/ResizingDialog.h"
#include "DlgState.h"

namespace fxb
{
class ShellIcon;
} // namespace fxb

class BookmarkEditDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    BookmarkEditDlg(void);
    virtual ~BookmarkEditDlg(void);

protected:
    enum IconReq
    {
        IconReqNone = 0,
        IconReqBookmarkMgr,
        IconReqShellIcon,
    };

    void addBookmark(fxb::BookmarkItem *aBookmarkItem, IconReq aIconReq, xpr_sint_t aIndex = -1, xpr_sint_t aBookmarkIndex = -1);
    xpr_sint_t findSignature(xpr_uint_t aSignature);
    void setItemFocus(xpr_sint_t aIndex);
    void updateStatus(void);
    void apply(void);

protected:
    CImageList      mImgList;
    fxb::ShellIcon *mShellIcon;

protected:
    HICON     mIcon;
    DlgState  mState;
    CListCtrl mListCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDestroy(void);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnItemAdd(void);
    afx_msg void OnItemModify(void);
    afx_msg void OnItemDelete(void);
    afx_msg void OnItemMoveUp(void);
    afx_msg void OnItemMoveDown(void);
    virtual void OnOK(void);
    afx_msg void OnQuickLaunch(void);
    afx_msg void OnDefault(void);
    afx_msg LRESULT OnBookmarkAsyncIcon(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnShellAsyncIcon(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_BOOKMARK_EDIT_DLG_H__
