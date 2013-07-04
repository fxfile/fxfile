//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DOS_CMD_DLG_H__
#define __FXFILE_DOS_CMD_DLG_H__ 1
#pragma once

namespace fxfile
{
class DlgState;

namespace cmd
{
class DosCmdDlg : public CDialog
{
    typedef CDialog super;

public:
    DosCmdDlg(const xpr_tchar_t *aPath = XPR_NULL);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

protected:
    DlgState  *mDlgState;
    CComboBox  mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_DOS_CMD_DLG_H__
