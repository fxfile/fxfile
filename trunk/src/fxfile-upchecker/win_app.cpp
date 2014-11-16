//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "win_app.h"
#include "main_window.h"
#include "single_process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WinApp gApp;

WinApp::WinApp(void)
{
}

BEGIN_MESSAGE_MAP(WinApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

xpr_bool_t WinApp::InitInstance(void)
{
    INITCOMMONCONTROLSEX sInitCtrls;
    sInitCtrls.dwSize = sizeof(sInitCtrls);
    sInitCtrls.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&sInitCtrls);

    super::InitInstance();

    if (AfxSocketInit() == XPR_FALSE)
    {
        AfxMessageBox(_T("Windows sockets initialization failed."));
        return XPR_FALSE;
    }

    AfxEnableControlContainer();

    //
    // initialize XPR
    //
    xpr_bool_t sResult = xpr::initialize();
    XPR_ASSERT(sResult == XPR_TRUE);

    // check single process
    if (fxfile::upchecker::SingleProcess::check() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    fxfile::upchecker::SingleProcess::lock();

    SetRegistryKey(_T("fxfile-upchecker"));

    if (mMainWindow.Create() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    m_pMainWnd = &mMainWindow;

    return XPR_TRUE;
}

xpr_sint_t WinApp::ExitInstance(void) 
{
    //
    // finalize XPR
    //
    xpr::finalize();

    return super::ExitInstance();
}
