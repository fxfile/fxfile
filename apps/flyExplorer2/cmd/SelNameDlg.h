//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEL_NAME_DLG_H__
#define __FX_SEL_NAME_DLG_H__
#pragma once

#include "DlgState.h"

class SelNameDlg : public CDialog
{
    typedef CDialog super;

public:
    SelNameDlg(xpr_bool_t aSelect);

public:
    const xpr_tchar_t *getSelName(void) { return mName; }
    xpr_bool_t isOnlySel(void) { return mOnlySel; }

protected:
    xpr_bool_t  mSelect;
    xpr_bool_t  mOnlySel;
    xpr_tchar_t mName[XPR_MAX_PATH + 1];

protected:
    DlgState  mState;
    CComboBox mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
};

#endif // __FX_SEL_NAME_DLG_H__
