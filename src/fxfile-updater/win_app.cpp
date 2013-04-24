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

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WinApp theApp;

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

    CWinApp::InitInstance();

    if (AfxSocketInit() == XPR_FALSE)
    {
        AfxMessageBox("Windows sockets initialization failed.");
        return XPR_FALSE;
    }

    AfxEnableControlContainer();

    SetRegistryKey(_T("fxUpdate"));

    if (mMainWindow.Create() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    m_pMainWnd = &mMainWindow;

    return XPR_TRUE;
}
