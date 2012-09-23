//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_PARAM_EXEC_DLG_H__
#define __FX_PARAM_EXEC_DLG_H__
#pragma once

#include "DlgState.h"

class ParamExecDlg : public CDialog
{
    typedef CDialog super;

public:
    ParamExecDlg(void);

    void setPath(const xpr_tchar_t *aPath);
    const xpr_tchar_t *getParameter(void);

protected:
    xpr_tchar_t  mPath[XPR_MAX_PARAM + 1];
    std::tstring mParameter;

protected:
    DlgState  mState;
    CComboBox mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
};

#endif // __FX_PARAM_EXEC_DLG_H__
