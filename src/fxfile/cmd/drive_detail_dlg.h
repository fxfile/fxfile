//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_DETAIL_DLG_H__
#define __FXFILE_DRIVE_DETAIL_DLG_H__ 1
#pragma once

#include "gui/DrivePieCtrl.h"
#include "gui/TextProgressCtrl.h"

namespace fxfile
{
namespace cmd
{
class DriveDetailDlg : public CDialog
{
    typedef CDialog super;

public:
    DriveDetailDlg(void);
    void setDrive(xpr_tchar_t aDrive);

protected:
    DrivePieCtrl     mDrivePieCtrl;
    TextProgressCtrl mProgressCtrl;
    HICON            mDriveIconHandle;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_DRIVE_DETAIL_DLG_H__
