//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_REN_TAB_DLG_1_H__
#define __FX_REN_TAB_DLG_1_H__
#pragma once

#include "RenTabDlg.h"
#include "DlgState.h"

class RenTabDlg1 : public RenTabDlg
{
    typedef RenTabDlg super;

public:
    RenTabDlg1(void);

protected:
    DlgState mState;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnApply(void);
};

#endif // __FX_REN_TAB_DLG_1_H__
