//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_LAUNCHER_APP_H__
#define __FX_LAUNCHER_APP_H__
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "MainWnd.h"

class LauncherApp : public CWinApp
{
public:
    LauncherApp(void);

public:
    virtual BOOL InitInstance(void);
    virtual int  ExitInstance(void);

protected:
    MainWnd mMainWnd;

public:
    DECLARE_MESSAGE_MAP()
};

extern LauncherApp theApp;

#endif // __FX_LAUNCHER_APP_H__
