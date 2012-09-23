//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_MAIN_WND_H__
#define __FX_MAIN_WND_H__
#pragma once

class SysTray;

class MainWnd : public CWnd
{
public:
    MainWnd(void);
    virtual ~MainWnd(void);

public:
    BOOL Create(void);

public:
    void install(void);
    void uninstall(void);

protected:
    void createTray(void);
    void destroyTray(void);

    void getSettingPath(LPTSTR lpszPath);
    void loadSetting(void);
    void saveSetting(void);
    BOOL loadCmdLine(void);

protected:
    BOOL     mInstalled;
    SysTray *mTray;

    struct Setting
    {
        BOOL mLoad;
        BOOL mTray;
        WORD mVirtualKeyCode;
    };

    Setting mSetting;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    afx_msg LRESULT OnKeyHooked(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTrayMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTaskRestarted(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandLoad(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandTray(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandHotKey(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandExit(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_MAIN_WND_H__
