//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SINGLE_RENAME_DLG_H__
#define __FXFILE_SINGLE_RENAME_DLG_H__ 1
#pragma once

#include "title_dlg.h"

namespace fxfile
{
class RenameHelper;
} // namespace fxfile

namespace fxfile
{
namespace cmd
{
class SingleRenameDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    SingleRenameDlg(void);
    virtual ~SingleRenameDlg(void);

public:
    void setItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aAttributes);
    const xpr_tchar_t *getNewName(void);

protected:
    RenameHelper *mRenameHelper;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_SINGLE_RENAME_DLG_H__
