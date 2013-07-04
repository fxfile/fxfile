//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_INFO_DLG_H__
#define __FXFILE_DRIVE_INFO_DLG_H__ 1
#pragma once

#include "drive_detail_dlg.h"

namespace fxfile
{
namespace cmd
{
class DriveInfoDlg : public CDialog
{
    typedef CDialog super;

public:
    DriveInfoDlg(xpr_tchar_t aDrive = 0);

public:
    void setDriveCount(xpr_sint_t aCount);
    void showDialog(xpr_sint_t aIndex);

protected:
    DriveDetailDlg *mDetailDlgs;
    xpr_sint_t      mCount;
    HTREEITEM      *mTreeItems;
    xpr_tchar_t     mDrives[26];
    xpr_tchar_t     mInitDrive;

protected:
    CTreeCtrl mTreeCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_DRIVE_INFO_DLG_H__
