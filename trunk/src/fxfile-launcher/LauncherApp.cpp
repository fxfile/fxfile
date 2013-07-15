//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "LauncherApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LauncherApp theApp;
static HANDLE gMutex;

LauncherApp::LauncherApp(void)
{
}

BEGIN_MESSAGE_MAP(LauncherApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

BOOL LauncherApp::InitInstance(void)
{
    gMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("fxfile-launcher_Mutex"));
    if (gMutex != NULL)
    {
        gMutex = NULL;
        return FALSE;
    }

    gMutex = ::CreateMutex(NULL, FALSE, _T("fxfile-launcher_Mutex"));

    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    if (mMainWnd.Create() == FALSE)
        return FALSE;

    m_pMainWnd = &mMainWnd;

    return TRUE;
}

int LauncherApp::ExitInstance(void) 
{
    if (gMutex != NULL)
    {
        mMainWnd.DestroyWindow();

        ::CloseHandle(gMutex);
        gMutex = NULL;
    }

    return CWinApp::ExitInstance();
}
