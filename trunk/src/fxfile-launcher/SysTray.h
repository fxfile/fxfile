//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SYS_TRAY_H__
#define __FX_SYS_TRAY_H__
#pragma once

class SysTray
{
public:
    SysTray(void);
    virtual ~SysTray(void);

public:
    BOOL createTray(HWND aHwnd, UINT aMsg, UINT aId, LPCTSTR aToolTip, HICON aIcon);
    void destroyTray(void);
    BOOL recreateTray(void);

    void showHideWindow(BOOL aShow, BOOL aAnimate = TRUE);
    void toggleWindow(BOOL aAnimate = TRUE);

    void showWndFromTray(BOOL aAnimate = TRUE);
    void hideWndToTray(BOOL aAnimate = TRUE);

public:
    static void getTrayWndRect(LPRECT aRect);

protected:
    void animateMinimizeToTray(void);
    void animateMaximizeFromTray(void);

protected:
    HWND mHwnd;
    NOTIFYICONDATA *mNotifyIconData;
};

#endif // __FX_SYS_TRAY_H__
