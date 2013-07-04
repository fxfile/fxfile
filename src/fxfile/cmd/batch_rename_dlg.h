//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BATCH_RENAME_DLG_H__
#define __FXFILE_BATCH_RENAME_DLG_H__ 1
#pragma once

#include "batch_rename.h"

#include "gui/ResizingDialog.h"
#include "gui/DlgToolBar.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class BatchRenameTabDlg;

class BatchRenameDlg : public CResizingDialog
{
    typedef CResizingDialog super;

    friend class BatchRenameTabDlg;

public:
    BatchRenameDlg(void);
    virtual ~BatchRenameDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath, xpr_bool_t aFolder);
    void addPath(BatRenItem *aBatRenItem);
    xpr_size_t getFileCount(void);

protected:
    void setStatus(const xpr_tchar_t *aStatusText);
    void setBatchFormatText(const xpr::tstring &aBatchFormat, xpr_sint_t aSel = -1);
    void setBatchFormatText(xpr_sint_t aSel = -1);
    void enableWindow(xpr_bool_t aEnable);

    void editItem(xpr_sint_t aIndex = -1);
    void moveItem(xpr_sint_t aIndex, xpr_sint_t aNewIndex);

    void addTabDialog(BatchRenameTabDlg *aTabDlg, const xpr_tchar_t *aTitle);
    BatchRenameTabDlg *getTabDialog(xpr_sint_t aIndex);
    void showTabDialog(xpr_sint_t aIndex);

    void OnFormatApply(void);
    void OnReplaceApply(void);
    void OnInsertApply(void);
    void OnDeleteApply(void);
    void OnCaseApply(void);

    xpr_bool_t translateHotKey(MSG *aMsg);

protected:
    BatchRename *mBatchRename;

    xpr_bool_t mRenaming;
    xpr_sint_t mOldShowDlg;

protected:
    DlgToolBar mToolBar;

    typedef std::deque<BatchRenameTabDlg *> TabDeque;
    TabDeque mTabDeque;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CTabCtrl       mTabCtrl;
    CProgressCtrl  mProgressCtrl;
    CListCtrl      mListCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnInit(void);
    virtual void OnOK(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg HCURSOR OnQueryDragIcon(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    virtual void OnCancel(void);
    afx_msg void OnItemActivate(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBatchFormatApply(void);
    afx_msg void OnUpdateInit(CCmdUI *pCmdUI);
    afx_msg void OnUpdateBatchFormatArchive(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNoChangeExt(CCmdUI *pCmdUI);
    afx_msg void OnUpdateResult(CCmdUI *pCmdUI);
    afx_msg void OnUpdateUndo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRedo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateItemUp(CCmdUI *pCmdUI);
    afx_msg void OnUpdateItemDown(CCmdUI *pCmdUI);
    afx_msg void OnUpdateItemMove(CCmdUI *pCmdUI);
    afx_msg void OnBatchFormatArchive(void);
    afx_msg void OnNoChangeExt(void);
    afx_msg void OnResult(void);
    afx_msg void OnEdit(void);
    afx_msg void OnUndo(void);
    afx_msg void OnRedo(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnItemMove(void);
    afx_msg void OnSelchangeSrnTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemEdit(void);
    afx_msg LRESULT OnFinalize(WPARAM, LPARAM);
    afx_msg LRESULT OnPostStatus(WPARAM, LPARAM);
    afx_msg LRESULT OnApply(WPARAM, LPARAM);
    afx_msg xpr_bool_t OnToolTipNotify(xpr_uint_t id, NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_RENAME_DLG_H__
