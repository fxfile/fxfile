//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DRIVE_DETAIL_DLG_H__
#define __FX_DRIVE_DETAIL_DLG_H__
#pragma once

#include "rgc/DrivePieCtrl.h"
#include "rgc/TextProgressCtrl.h"

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

#endif // __FX_DRIVE_DETAIL_DLG_H__
