//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_HISTORY_DLG_H__
#define __FXFILE_HISTORY_DLG_H__ 1
#pragma once

#include "history.h"
#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class HistoryDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    HistoryDlg(xpr_size_t aHistory = 0);
    virtual ~HistoryDlg(void);

public:
    void getSelHistory(xpr_size_t &aHistory, xpr_uint_t &aIndex);
    xpr_sint_t addHistory(const xpr_tchar_t *aTitleText, const xpr_tchar_t *aTabText, const HistoryDeque *aHistoryDeque);

protected:
    struct HistoryDisp
    {
        const xpr_tchar_t  *mTitleText;
        const xpr_tchar_t  *mTabText;
        const HistoryDeque *mHistoryDeque;
    };

protected:
    xpr_uint_t getItemIndex(HTREEITEM aTreeItem);
    void updateStatus(void);
    void getContextMenu(xpr_bool_t aRClick);
    void showTab(xpr_size_t aIndex);
    HistoryDisp *getCurHistoryDisp(void);

protected:
    CImageList mTreeImgList;

    CToolBar   mToolBar;
    CImageList mToolBarImgList;

    typedef std::deque<HistoryDisp *> HistoryDispDeque;
    HistoryDispDeque mHistoryDispDeque;
    xpr_size_t mCurHistoryDisp;
    xpr_uint_t mIndex;

protected:
    HICON      mIcon;
    DlgState  *mDlgState;
    CTreeCtrl  mTreeCtrl;
    CTabCtrl   mTabCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    virtual void OnOK(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnGetInfoTipTree(NMHDR *pNMHDR, LRESULT *pResult);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
    afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_HISTORY_DLG_H__
