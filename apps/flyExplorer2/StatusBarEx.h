//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_STATUS_BAR_EX_H__
#define __FX_STATUS_BAR_EX_H__
#pragma once

#include "rgc/TextProgressCtrl.h"

class StatusBarEx : public CStatusBar
{
public:
    StatusBarEx(void);
    virtual ~StatusBarEx(void);

public:
    xpr_bool_t init(void);

public:
    void setGroup(HICON aIcon, const xpr_tchar_t *aGroup);
    void setDiskFreeSpace(const xpr_tchar_t *aPath);
    void recalcLayout(void);
    void showDriveProperty(void);

    void setDynamicPaneText(xpr_sint_t aPane, const xpr_tchar_t *aText, xpr_sint_t aOffset = 0);

protected:
    HICON mDriveIcon;
    HICON mGroupIcon;
    TextProgressCtrl mDriveProgressCtrl;

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnDblclk(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg LRESULT OnProgressDblclk(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_STATUS_BAR_EX_H__