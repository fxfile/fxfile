//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SINGLE_RENAME_DLG_H__
#define __FX_SINGLE_RENAME_DLG_H__
#pragma once

#include "TitleDlg.h"

namespace fxb
{
class RenameHelper;
} // namespace fxb

class SingleRenameDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    SingleRenameDlg(void);
    virtual ~SingleRenameDlg(void);

public:
    void setItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aAttributes);
    const xpr_tchar_t *getNewName(void);

protected:
    fxb::RenameHelper *mRenameHelper;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnTimer(xpr_uint_t nIDEvent);
};

#endif // __FX_SINGLE_RENAME_DLG_H__
