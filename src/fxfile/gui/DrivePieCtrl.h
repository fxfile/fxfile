//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_PIE_CTRL_H__
#define __FXFILE_DRIVE_PIE_CTRL_H__
#pragma once

class DrivePieCtrl : public CStatic
{
public:
    DrivePieCtrl(void);
    virtual ~DrivePieCtrl(void);

public:
    void setString(const xpr_tchar_t *aTotalSizeText, const xpr_tchar_t *aUsedSizeText, const xpr_tchar_t *aFreeSizeText);
    void setDrive(const xpr_tchar_t *aDrive);

protected:
    void drawLegends(CDC &aDC);
    void getDriveSpace(void);
    void drawDrivePie(CDC &aDC);

protected:
    xpr::tstring mTotalSizeText;
    xpr::tstring mUsedSizeText;
    xpr::tstring mFreeSizeText;

    xpr_tchar_t  mDrive[XPR_MAX_PATH + 1];
    xpr_uint64_t mUsedSize;
    xpr_uint64_t mTotalSize;
    xpr_uint64_t mFreeSize;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnPaint(void);
};

#endif // __FXFILE_DRIVE_PIE_CTRL_H__
