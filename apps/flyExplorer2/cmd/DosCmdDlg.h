//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DOS_CMD_DLG_H__
#define __FX_DOS_CMD_DLG_H__
#pragma once

#include "DlgState.h"

class DosCmdDlg : public CDialog
{
    typedef CDialog super;

public:
    DosCmdDlg(const xpr_tchar_t *aPath = XPR_NULL);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

protected:
    DlgState  mState;
    CComboBox mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
};

#endif // __FX_DOS_CMD_DLG_H__
