//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_COMBINE_DLG_H__
#define __FXFILE_FILE_COMBINE_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class FileCombine;

class FileCombineDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    FileCombineDlg(void);
    virtual ~FileCombineDlg(void);

    void setPath(const xpr_tchar_t *aPath);
    void setDestDir(const xpr_tchar_t *aDestDir);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatus);
    void setCrcFile(void);

protected:
    xpr::tstring mPath;
    xpr::tstring mDestDir;

    FileCombine *mFileCombine;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;

public:
    virtual xpr_bool_t DestroyWindow();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual void OnOK(void);
    afx_msg void OnPathBrowse(void);
    afx_msg void OnDestDirBrowse(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_COMBINE_DLG_H__
