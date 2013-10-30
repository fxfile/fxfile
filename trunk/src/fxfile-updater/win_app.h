//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPDATER_WINAPP_H__
#define __FXFILE_UPDATER_WINAPP_H__ 1
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "main_window.h"

class WinApp : public CWinApp
{
    typedef CWinApp super;

public:
    WinApp(void);

public:
    virtual xpr_bool_t InitInstance(void);
    virtual xpr_sint_t ExitInstance(void);

protected:
    fxfile::updater::MainWindow mMainWindow;

protected:
    DECLARE_MESSAGE_MAP()
};

extern WinApp theApp;

#endif // __FXFILE_UPDATER_WINAPP_H__
