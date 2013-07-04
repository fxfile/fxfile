//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_PATH_BAR_H__
#define __FXFILE_DRIVE_PATH_BAR_H__ 1
#pragma once

#include "drive_toolbar.h"

namespace fxfile
{
class DrivePathBar : public DriveToolBar
{
public:
    DrivePathBar(void);
    virtual ~DrivePathBar(void);

public:
    void getToolBarSize(xpr_sint_t *aWidth, xpr_sint_t *aHeight);

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint(void);
    DECLARE_MESSAGE_MAP()
};
} // namespace fxfile

#endif // __FXFILE_DRIVE_PATH_BAR_H__
