//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PROC_SHARED_FILE_H__
#define __FXFILE_PROC_SHARED_FILE_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class ProcSharedFileDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    ProcSharedFileDlg(const xpr_tchar_t *aPath);
    virtual ~ProcSharedFileDlg(void);

protected:
    xpr_sint_t setPath(const xpr_tchar_t *aPath);
    void setStatus(const xpr_tchar_t *aStatusText);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

protected:
    HICON      mIcon;
    DlgState  *mDlgState;
    CListCtrl  mListCtrl;

public:
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnProperties(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PROC_SHARED_FILE_H__
