//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_RENAME_TIP_DLG_H__
#define __FXFILE_RENAME_TIP_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
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
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_RENAME_TIP_DLG_H__
