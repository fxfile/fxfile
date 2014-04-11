//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CRC_VERIFY_DLG_H__
#define __FXFILE_CRC_VERIFY_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class CrcVerify;

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

    typedef std::deque<xpr::string> FileDeque;
    FileDeque mFileDeque;
    xpr::string mDir;

    CrcVerify *mCrcVerify;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CListCtrl      mListCtrl;

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
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CRC_VERIFY_DLG_H__
