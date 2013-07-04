//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BATCH_RENAME_TAB_CASE_DLG_H__
#define __FXFILE_BATCH_RENAME_TAB_CASE_DLG_H__ 1
#pragma once

#include "batch_rename_tab_dlg.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class BatchRenameTabCaseDlg : public BatchRenameTabDlg
{
    typedef BatchRenameTabDlg super;

public:
    BatchRenameTabCaseDlg(void);

protected:
    DlgState *mDlgState;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);

protected:
    virtual void DoDataExchange(CDataExchange *aDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnApply(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_RENAME_TAB_CASE_DLG_H__
