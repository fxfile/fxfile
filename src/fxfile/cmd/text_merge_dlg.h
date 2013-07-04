//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TEXT_MERGE_DLG_H__
#define __FXFILE_TEXT_MERGE_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class TextMerge;

class TextMergeDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    TextMergeDlg(void);
    virtual ~TextMergeDlg(void);

public:
    void addPath(const xpr_tchar_t *aPath);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatusText);

protected:
    typedef std::deque<xpr::tstring> PathDeque;
    PathDeque  mPathDeque;
    TextMerge *mTextMerge;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CComboBox      mEncodingComboBox;
    CListCtrl      mListCtrl;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TEXT_MERGE_DLG_H__
