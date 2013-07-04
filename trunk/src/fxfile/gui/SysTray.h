//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SYS_TRAY_H__
#define __FXFILE_SYS_TRAY_H__
#pragma once

class SysTray
{
public:
    SysTray(void);
    virtual ~SysTray(void);

public:
    xpr_bool_t createTray(HWND aHwnd, xpr_uint_t aMsg, xpr_uint_t aId, const xpr_tchar_t *aToolTip, HICON aIcon);
    void destroyTray(void);
    xpr_bool_t recreateTray(void);

    void showWindow(xpr_bool_t aShow, xpr_bool_t aAnimate = XPR_TRUE);
    void toggleWindow(xpr_bool_t aAnimate = XPR_TRUE);

    void showFromTray(xpr_bool_t aAnimate = XPR_TRUE);
    void hideToTray(xpr_bool_t aAnimate = XPR_TRUE);

public:
    static void getTrayWndRect(LPRECT aRect);

protected:
    void animateMinimizeToTray(void);
    void animateMaximizeFromTray(void);

protected:
    HWND            mHwnd;
    NOTIFYICONDATA *mNotifyIconData;
};

#endif // __FXFILE_SYS_TRAY_H__
