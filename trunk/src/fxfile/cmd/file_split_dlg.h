//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SPLIT_DLG_H__
#define __FXFILE_FILE_SPLIT_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class FileSplit;

class FileSplitDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    FileSplitDlg(void);
    virtual ~FileSplitDlg(void);

public:
    void setPath(const xpr_tchar_t *aPath);
    void setDestDir(const xpr_tchar_t *aDestDir);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatus);

    void setFileSizeText(void);
    xpr_uint64_t getFileSize(void);
    xpr_uint64_t getSplitSize(void);
    xpr_size_t getSplitCount(void);

protected:
    xpr::string mPath;
    xpr::string mDestDir;

    FileSplit *mFileSplit;

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
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_SPLIT_DLG_H__
