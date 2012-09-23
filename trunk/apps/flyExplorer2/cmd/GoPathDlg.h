//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_GO_PATH_DLG_H__
#define __FX_GO_PATH_DLG_H__
#pragma once

#include "DlgState.h"

class GoPathDlg : public CDialog
{
    typedef CDialog super;

public:
    GoPathDlg(void);
    virtual ~GoPathDlg(void);

public:
    void setExpPath(LPITEMIDLIST aPidl1, LPITEMIDLIST aPidl2);
    void getPath(xpr_tchar_t *aPath);
    xpr_bool_t isEnableUrl(void);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];
    xpr_bool_t mUrl;

    LPITEMIDLIST mPidl1;
    LPITEMIDLIST mPidl2;

protected:
    DlgState  mState;
    CComboBox mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnGoPathBrowse(void);
    afx_msg void OnBnClickedExp1Path(void);
    afx_msg void OnBnClickedExp2Path(void);
};

#endif // __FX_GO_PATH_DLG_H__
