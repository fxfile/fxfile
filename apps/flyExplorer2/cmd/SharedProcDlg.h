//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SHARED_PROC_H__
#define __FX_SHARED_PROC_H__
#pragma once

#include "rgc/ResizingDialog.h"

class DlgState;

class SharedProcDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    SharedProcDlg(const xpr_tchar_t *aPath);
    virtual ~SharedProcDlg(void);

protected:
    xpr_sint_t setPath(const xpr_tchar_t *aPath);
    void setStatus(const xpr_tchar_t *aStatusText);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

protected:
    HICON      mIcon;
    DlgState  *mDlgState;
    CListCtrl  mListCtrl;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnProperties(void);
};

#endif // __FX_SHARED_PROC_H__
