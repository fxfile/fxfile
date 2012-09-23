//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_RENAME_TIP_DLG_H__
#define __FX_RENAME_TIP_DLG_H__
#pragma once

class RenameTipDlg : public CDialog
{
    typedef CDialog super;

public:
    RenameTipDlg(void);

public:
    virtual xpr_bool_t Create(CWnd* pParentWnd);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpcs);
    afx_msg xpr_bool_t OnInitDialog(void);
};

#endif // __FX_RENAME_TIP_DLG_H__
