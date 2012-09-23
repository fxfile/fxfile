//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CRC_VERIFY_DLG_H__
#define __FX_CRC_VERIFY_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"
#include "DlgState.h"

namespace fxb
{
class CrcVerify;
} // namespace fxb

class CrcVerifyDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    CrcVerifyDlg(const xpr_tchar_t *aDir);
    virtual ~CrcVerifyDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath);

protected:
    void setStatus(const xpr_tchar_t *aStatus);
    void enableWindow(xpr_bool_t aEnable);

protected:
    xpr_bool_t mEnable;

    typedef std::deque<std::tstring> FileDeque;
    FileDeque mFileDeque;
    std::tstring mDir;

    fxb::CrcVerify *mCrcVerify;

protected:
    HICON         mIcon;
    DlgState      mState;
    CProgressCtrl mProgressCtrl;
    CListCtrl     mListCtrl;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    virtual void OnCancel(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#endif // __FX_CRC_VERIFY_DLG_H__