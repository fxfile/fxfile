//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_LIST_DLG_H__
#define __FXFILE_FILE_LIST_DLG_H__ 1
#pragma once

#include "title_dlg.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class FileList;

class FileListDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    FileListDlg(void);
    virtual ~FileListDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath);
    void setTextFile(const xpr_tchar_t *aTextFile);

protected:
    void enableWindow(xpr_bool_t aEnable = XPR_TRUE);

protected:
    typedef std::deque<xpr::string> PathDeque;
    PathDeque mPathDeque;

    xpr::string mTextFile;

    FileList *mFileList;

protected:
    DlgState *mDlgState;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnPathBrowse(void);
    afx_msg void OnOK(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);

public:
    virtual xpr_bool_t DestroyWindow();
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_LIST_DLG_H__
