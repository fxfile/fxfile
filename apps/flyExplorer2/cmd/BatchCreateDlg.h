//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BATCH_CREATE_DLG_H__
#define __FX_BATCH_CREATE_DLG_H__
#pragma once

#include "fxb/fxb_batch_create.h"
#include "rgc/ResizingDialog.h"

namespace fxb
{
class BatchCreate;
} // namespace fxb

class DlgState;
class BatchCreateTabDlg;

class BatchCreateDlg : public CResizingDialog
{
    friend class BatchCreateTabDlg;

    typedef CResizingDialog super;

public:
    BatchCreateDlg(const xpr_tchar_t *aPath);
    virtual ~BatchCreateDlg(void);

protected:
    void addDialog(BatchCreateTabDlg *aDlg, const xpr_tchar_t *aTitle);
    xpr_sint_t getCurTab(void);
    BatchCreateTabDlg *getDialog(xpr_sint_t aIndex);
    void showDialog(xpr_sint_t aIndex);

    xpr_bool_t translateHotKey(MSG *aMsg);

    void setEnableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatus);

    fxb::BatchCreate::CreateType getCreateType(void);
    void setCreateType(fxb::BatchCreate::CreateType aType);

protected:
    std::tstring mPath;
    fxb::BatchCreate *mBatchCreate;

    typedef std::deque<BatchCreateTabDlg *> TabDeque;
    TabDeque mTabDeque;
    xpr_sint_t mOldShowDlg;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CComboBox      mTypeWnd;
    CTabCtrl       mTabCtrl;
    CProgressCtrl  mProgressCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSelChangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_BATCH_CREATE_DLG_H__
