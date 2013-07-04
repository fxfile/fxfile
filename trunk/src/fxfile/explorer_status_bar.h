//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_EXPLORER_STATUS_BAR_H__
#define __FXFILE_EXPLORER_STATUS_BAR_H__ 1
#pragma once

#include "gui/StatusBar.h"
#include "gui/TextProgressCtrl.h"

namespace fxfile
{
class ExplorerStatusBar : public StatusBar
{
    typedef StatusBar super;

public:
    enum
    {
        kPaneIdNormal = 10,
        kPaneIdSize,
        kPaneIdDrive,
        kPaneIdDriveProgress,
    };

public:
    ExplorerStatusBar(void);
    virtual ~ExplorerStatusBar(void);

public:
    void setDiskFreeSpace(const xpr_tchar_t *aPath);
    void showDriveProperties(void);

protected:
    void recalcLayout(void);

protected:
    HICON mDriveIcon;
    TextProgressCtrl mDriveProgressCtrl;

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg LRESULT OnProgressDblclk(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};
} // namespace fxfile

#endif // __FXFILE_EXPLORER_STATUS_BAR_H__
