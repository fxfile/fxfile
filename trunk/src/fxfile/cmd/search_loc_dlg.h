//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_LOC_DLG_H__
#define __FXFILE_SEARCH_LOC_DLG_H__ 1
#pragma once

#include "search.h"
#include "gui/ResizingDialog.h"
#include "gui/DropMenuButton.h"
#include "gui/BCMenu.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class SearchLocDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    SearchLocDlg(void);
    virtual ~SearchLocDlg(void);

public:
    SearchUserLocDeque *getSearchLoc(xpr_bool_t aNull = XPR_FALSE);
    void setSearchLoc(SearchUserLocDeque *aSearchLocDeque);

protected:
    void addLoc(SearchUserLoc *aSearchLoc, xpr_sint_t aIndex = -1);
    xpr_sint_t addLocPath(SearchUserLocItem *aItem, xpr_sint_t aIndex = -1);
    xpr_sint_t findLoc(LPITEMIDLIST sFullPidl);
    xpr_sint_t findLoc(const xpr_tchar_t *aPath);
    xpr_sint_t findLocName(const xpr_tchar_t *aName);
    void setItemFocus(xpr_sint_t aIndex);
    void updateStatus(void);
    void setLocPath(void);

protected:
    CImageList mLocImgList;
    CImageList mListImgList;
    CBrush mBrush;

    SearchUserLocDeque *mSearchUserLoc;

protected:
    HICON            mIcon;
    DlgState        *mDlgState;
    CListCtrl        mListCtrl;
    CComboBoxEx      mLocComboBox;
    CDropMenuButton  mNewDropButton;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDeleteitemItemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkItemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDestroy(void);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnItemAdd(void);
    afx_msg void OnItemModify(void);
    afx_msg void OnItemDelete(void);
    afx_msg void OnItemMoveUp(void);
    afx_msg void OnItemMoveDown(void);
    virtual void OnOK(void);
    afx_msg void OnPaint(void);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor);
    afx_msg void OnCbnSelchangeList(void);
    afx_msg void OnCbenDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnListNew(void);
    afx_msg void OnListModify(void);
    afx_msg void OnListDelete(void);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu);
    afx_msg LRESULT OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu);
    afx_msg void OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_SEARCH_LOC_DLG_H__
