//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "MainWnd.h"

#include "resource.h"
#include "../fxfile-keyhook/fxfile-keyhook.h"
#include "SysTray.h"
#include "IniFile.h"
#include "CmdLineParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const LPCTSTR LAUNCHER_WND_CLASS = _T("fxfile-launcher");

static const UINT WM_TASKRESTARTED           = ::RegisterWindowMessage(_T("TaskbarCreated")); 
static const UINT WM_LAUNCHER_COMMAND_LOAD   = ::RegisterWindowMessage(_T("fxfile-launcher: Load"));
static const UINT WM_LAUNCHER_COMMAND_TRAY   = ::RegisterWindowMessage(_T("fxfile-launcher: Tray"));
static const UINT WM_LAUNCHER_COMMAND_HOTKEY = ::RegisterWindowMessage(_T("fxfile-launcher: HotKey"));
static const UINT WM_LAUNCHER_COMMAND_EXIT   = ::RegisterWindowMessage(_T("fxfile-launcher: Exit"));

enum
{
    WM_KEY_HOOKED = WM_USER+100,
    WM_TRAY_MESSAGE,
};

static BOOL RegWndClass(LPCTSTR lpcszClassName, HBRUSH hBrush, HCURSOR hCursor, LONG lStyle = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS)
{
    BOOL sResult = FALSE;

    WNDCLASS wndClass = {0};
    HINSTANCE hInst = AfxGetInstanceHandle();
    if (!::GetClassInfo(hInst, lpcszClassName, &wndClass))
    {
        wndClass.style			= lStyle;
        wndClass.lpfnWndProc	= ::DefWindowProc;
        wndClass.cbClsExtra		= 0;
        wndClass.cbWndExtra		= 0;
        wndClass.hInstance		= hInst;
        wndClass.hIcon			= NULL;
        wndClass.hCursor		= hCursor;
        wndClass.hbrBackground	= hBrush;
        wndClass.lpszMenuName	= NULL;
        wndClass.lpszClassName	= lpcszClassName;

        sResult = AfxRegisterClass(&wndClass);
        if (sResult == FALSE)
        {
            AfxThrowResourceException();
        }
    }

    return sResult;
}

static void GetModulePath(LPTSTR aPath, size_t aMaxLen)
{
    if (aPath == NULL || aMaxLen <= 0)
        return;

    HMODULE hModule = ::GetModuleHandle(NULL);
    ::GetModuleFileName(hModule, aPath, (DWORD)aMaxLen);
}

static void GetModuleDir(LPTSTR aDir, size_t aMaxLen)
{
    if (aDir == NULL || aMaxLen <= 0)
        return;

    ::GetModulePath(aDir, aMaxLen);

    LPTSTR sSplit = _tcsrchr(aDir, '\\');
    if (sSplit != NULL)
        *sSplit = '\0';
}

#define __LowerCase_A__     97
#define __LowerCase_Z__     122
#define __UpperCase_A__     65
#define __UpperCase_Z__     90
#define __STRING_DISTANCE__ __LowerCase_A__ - __UpperCase_A__

static inline TCHAR TOUPPER(TCHAR ConvertChar)
{
    if (__LowerCase_A__ <= ConvertChar && ConvertChar <= __LowerCase_Z__)
        return TCHAR(ConvertChar - 32);
    else
        return ConvertChar;
}

MainWnd::MainWnd(void)
    : mInstalled(FALSE)
    , mTray(NULL)
{
    mSetting.mLoad           = FALSE;
    mSetting.mTray           = FALSE;
    mSetting.mVirtualKeyCode = 0;
}

MainWnd::~MainWnd(void)
{
    uninstall();
}

BOOL MainWnd::Create(void)
{
    ::RegWndClass(LAUNCHER_WND_CLASS, reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), ::LoadCursor(NULL, IDC_ARROW));

    return CreateEx(0, LAUNCHER_WND_CLASS, LAUNCHER_WND_CLASS, 0,0,0,0,0,0,0);
}

BEGIN_MESSAGE_MAP(MainWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()

    ON_COMMAND(ID_CLOSE, OnClose)

    ON_MESSAGE(WM_KEY_HOOKED, OnKeyHooked)
    ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayMessage)

    ON_REGISTERED_MESSAGE(WM_TASKRESTARTED,           OnTaskRestarted)
    ON_REGISTERED_MESSAGE(WM_LAUNCHER_COMMAND_LOAD,   OnCommandLoad)
    ON_REGISTERED_MESSAGE(WM_LAUNCHER_COMMAND_TRAY,   OnCommandTray)
    ON_REGISTERED_MESSAGE(WM_LAUNCHER_COMMAND_HOTKEY, OnCommandHotKey)
    ON_REGISTERED_MESSAGE(WM_LAUNCHER_COMMAND_EXIT,   OnCommandExit)
END_MESSAGE_MAP()

int MainWnd::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CWnd::OnCreate(aCreateStruct) == -1)
        return -1;

    loadSetting();

    if (loadCmdLine())
        saveSetting();

    if (mSetting.mLoad == FALSE)
        return -1;

    install();

    if (mSetting.mTray == TRUE)
        createTray();

    ::SetHookKey(mSetting.mVirtualKeyCode);

    return 0;
}

void MainWnd::OnDestroy(void)
{
    uninstall();

    destroyTray();

    saveSetting();

    CWnd::OnDestroy();
}

void MainWnd::install(void)
{
    if (mInstalled == TRUE)
        uninstall();

    mInstalled = ::InstallHook(m_hWnd, WM_KEY_HOOKED);
    if (mInstalled == FALSE)
    {
        const TCHAR *sMsg = _T("Error!\r\n\r\nUnable to install hook");
        ::MessageBox(NULL, sMsg, _T("fxfile-launcher"), MB_OK | MB_ICONSTOP);
    }
}

void MainWnd::uninstall(void)
{
    if (mInstalled == TRUE)
        ::RemoveHook();

    mInstalled = FALSE;
}

void MainWnd::createTray(void)
{
    if (mTray != NULL)
        return;

    UINT sIconId = IDI_TRAY;
    HICON sIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(sIconId), IMAGE_ICON, 16, 16, 0);

    mTray = new SysTray;
    mTray->createTray(GetSafeHwnd(), WM_TRAY_MESSAGE, IDS_TRAY_NOTIFY, _T("fxfile-launcher"), sIcon);
}

void MainWnd::destroyTray(void)
{
    if (mTray == NULL)
        return;

    mTray->destroyTray();

    delete mTray;
    mTray = NULL;
}

void MainWnd::getSettingPath(LPTSTR aPath)
{
    if (aPath == NULL)
        return;

    if (::SHGetSpecialFolderPath(NULL, aPath, CSIDL_APPDATA, FALSE))
    {
        _tcscat(aPath, _T("\\fxfile-launcher"));
        ::CreateDirectory(aPath, NULL);
    }
    else
    {
        ::GetModuleDir(aPath, MAX_PATH);
    }

    _tcscat(aPath, _T("\\fxfile-launcher.ini"));
}

void MainWnd::loadSetting(void)
{
    TCHAR sPath[MAX_PATH + 1] = {0};
    getSettingPath(sPath);

    IniFile sIniFile;
    sIniFile.SetPath(sPath);
    if (sIniFile.ReadFileW() == FALSE)
        return;

    mSetting.mLoad           = sIniFile.GetValueB(_T("Main"), _T("Load"));
    mSetting.mTray           = sIniFile.GetValueB(_T("Main"), _T("Tray"));
    mSetting.mVirtualKeyCode = sIniFile.GetValueU(_T("Main"), _T("Global Hot Key"));

    mSetting.mVirtualKeyCode = (WORD)TOUPPER((TCHAR)mSetting.mVirtualKeyCode);
}

void MainWnd::saveSetting(void)
{
    TCHAR sPath[MAX_PATH + 1] = {0};
    getSettingPath(sPath);

    IniFile sIniFile;
    sIniFile.SetPath(sPath);

    sIniFile.ReadFileW();

    sIniFile.SetValueB(_T("Main"), _T("Load"),           mSetting.mLoad);
    sIniFile.SetValueB(_T("Main"), _T("Tray"),           mSetting.mTray);
    sIniFile.SetValueU(_T("Main"), _T("Global Hot Key"), mSetting.mVirtualKeyCode);

    sIniFile.WriteFileW();
}

BOOL MainWnd::loadCmdLine(void)
{
    BOOL sResult = FALSE;

    std::tstring sValue;
    CmdLineParser sCmdLineParser;
    sCmdLineParser.parse();

    if (sCmdLineParser.getArg(_T("LOAD"), sValue))
    {
        _stscanf(sValue.c_str(), _T("%d"), &mSetting.mLoad);
        sResult = TRUE;
    }

    if (sCmdLineParser.getArg(_T("TRAY"), sValue))
    {
        _stscanf(sValue.c_str(), _T("%d"), &mSetting.mTray);
        sResult = TRUE;
    }

    if (sCmdLineParser.getArg(_T("HOTKEY"), sValue))
    {
        DWORD dwVirtualKeyCode = mSetting.mVirtualKeyCode;
        _stscanf(sValue.c_str(), _T("%d"), &dwVirtualKeyCode);
        mSetting.mVirtualKeyCode = (WORD)dwVirtualKeyCode;
        sResult = TRUE;
    }

    return sResult;
}

void MainWnd::OnClose(void)
{
    PostMessage(WM_CLOSE);
}

LRESULT MainWnd::OnKeyHooked(WPARAM wParam, LPARAM lParam)
{
    TCHAR sDir[MAX_PATH + 1] = {0};
    GetModuleDir(sDir, MAX_PATH);
    //_tcscat(sDir, _T("\\x86-unicode"));

    TCHAR sPath[MAX_PATH + 1] = {0};
    _tcscpy(sPath, sDir);

#ifdef _DEBUG
    _tcscat(sPath, _T("\\fxfile_dbg.exe"));
#else
    _tcscat(sPath, _T("\\fxfile.exe"));
#endif

    ::ShellExecute(NULL, _T("open"), sPath, NULL, sDir, 0);

    return 0;
}

LRESULT MainWnd::OnTrayMessage(WPARAM wParam, LPARAM lParam)
{
    if (wParam == IDS_TRAY_NOTIFY)
    {
        switch (lParam)
        {
        case WM_RBUTTONUP:
            {
                SetForegroundWindow();

                CPoint sPoint;
                ::GetCursorPos(&sPoint);

                CMenu sMenu;
                if (sMenu.LoadMenu(IDR_TRAY) == TRUE)
                {
                    CMenu *sPopupMenu = sMenu.GetSubMenu(0);
                    if (sPopupMenu != NULL)
                    {
                        sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sPoint.x, sPoint.y, this);
                    }
                }
                break;
            }
        }
    }

    return 0;
}

LRESULT MainWnd::OnTaskRestarted(WPARAM wParam, LPARAM lParam)
{
    if (mTray != NULL)
        mTray->recreateTray();

    return 0;
}

LRESULT MainWnd::OnCommandLoad(WPARAM wParam, LPARAM lParam)
{
    mSetting.mLoad = (BOOL)wParam;

    saveSetting();

    return 0;
}

LRESULT MainWnd::OnCommandTray(WPARAM wParam, LPARAM lParam)
{
    mSetting.mTray = (BOOL)wParam;

    if (mSetting.mTray != NULL)
        createTray();
    else
        destroyTray();

    saveSetting();

    return 0;
}

LRESULT MainWnd::OnCommandHotKey(WPARAM wParam, LPARAM lParam)
{
    mSetting.mVirtualKeyCode = (WORD)wParam;

    ::SetHookKey(mSetting.mVirtualKeyCode);

    saveSetting();

    return 0;
}

LRESULT MainWnd::OnCommandExit(WPARAM wParam, LPARAM lParam)
{
    OnClose();
    return 0;
}
