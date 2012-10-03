//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CRC_CREATE_DLG_H__
#define __FX_CRC_CREATE_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"

namespace fxb
{
class CrcCreate;
} // namespace fxb

class DlgState;

class CrcCreateDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    CrcCreateDlg(void);
    virtual ~CrcCreateDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath);
    void saveDefaultPath(const xpr_tchar_t *aPath);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatus);

protected:
    xpr_bool_t mEnable;

    fxb::CrcCreate *mCrcCreate;

    xpr_tchar_t mSaveDefaultPath[XPR_MAX_PATH + 1];
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

    typedef std::deque<std::tstring> PathDeque;
    PathDeque mListDeque;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CListCtrl      mListCtrl;
    CComboBox      mMethodWnd;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg void OnOK(void);
    afx_msg void OnBrowse(void);
    afx_msg void OnSelchangeMethod(void);
    afx_msg void OnEach(void);
    virtual void OnCancel(void);
};

#endif // __FX_CRC_CREATE_DLG_H__
