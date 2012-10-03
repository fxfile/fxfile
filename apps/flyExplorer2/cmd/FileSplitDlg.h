//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SPLIT_DLG_H__
#define __FX_FILE_SPLIT_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"

namespace fxb
{
class FileSplit;
} // namespace fxb

class DlgState;

class FileSplitDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    FileSplitDlg(void);
    virtual ~FileSplitDlg(void);

public:
    void setPath(const xpr_tchar_t *lpcszPath);
    void setDestDir(const xpr_tchar_t *lpcszDestDir);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatus);

    void setFileSizeText(void);
    xpr_uint64_t getFileSize(void);
    xpr_uint64_t getSplitSize(void);
    xpr_size_t getSplitCount(void);

protected:
    std::tstring mPath;
    std::tstring mDestDir;

    fxb::FileSplit *mFileSplit;

protected:
    xpr_bool_t mSetSplitSizeMode;
    xpr_bool_t mSetSplitCoutMode;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CComboBox      mSplitSizePreset;
    CComboBox      mSplitSizeUnit;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPathBrowse(void);
    afx_msg void OnOptionSplit(void);
    afx_msg void OnDestDirBrowse(void);
    afx_msg void OnSelchangeSplitSizePreset(void);
    afx_msg void OnUpdateSplitSize(void);
    afx_msg void OnUpdateSplitCount(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_FILE_SPLIT_DLG_H__
