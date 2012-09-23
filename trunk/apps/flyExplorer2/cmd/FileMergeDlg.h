//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_MERGE_DLG_H__
#define __FX_FILE_MERGE_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"
#include "DlgState.h"

namespace fxb
{
class FileMerge;
} // namespace fxb

class FileMergeDlg : public CResizingDialog
{
public:
    FileMergeDlg(void);
    virtual ~FileMergeDlg(void);

public:
    void setDir(const xpr_tchar_t *aDir);
    void addPath(const xpr_tchar_t *aPath);
    void setDestPath(const xpr_tchar_t *aDestPath);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *sStatus);

protected:
    typedef std::deque<std::tstring> PathDeque;
    PathDeque    mPathDeque;
    std::tstring mDir;
    std::tstring mDestPath;

    fxb::FileMerge *mFileMerge;

protected:
    HICON         mIcon;
    DlgState      mState;
    CProgressCtrl mProgressCtrl;
    CListCtrl     mListCtrl;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnDestFileBrowse(void);
};

#endif // __FX_FILE_MERGE_DLG_H__
