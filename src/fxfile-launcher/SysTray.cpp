//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SysTray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SysTray::SysTray(void)
    : mHwnd(NULL)
    , mNotifyIconData(NULL)
{
}

SysTray::~SysTray(void)
{
    destroyTray();
}

BOOL SysTray::createTray(HWND aHwnd, UINT aMsg, UINT aId, LPCTSTR aToolTip, HICON aIcon)
{
    if (aHwnd == NULL || aMsg == 0 || aId == 0 || aToolTip == NULL || aIcon == NULL)
        return FALSE;

    if (mNotifyIconData != NULL)
        return FALSE;

    mNotifyIconData = new NOTIFYICONDATA;
    if (mNotifyIconData == NULL)
        return FALSE;

    memset(mNotifyIconData, 0, sizeof(NOTIFYICONDATA));
    mNotifyIconData->cbSize           = sizeof(NOTIFYICONDATA);
    mNotifyIconData->uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    mNotifyIconData->hWnd             = aHwnd;
    mNotifyIconData->hIcon            = aIcon;
    mNotifyIconData->uCallbackMessage = aMsg;
    mNotifyIconData->uID              = aId;
    _tcscpy(mNotifyIconData->szTip, aToolTip);

    BOOL sResult = ::Shell_NotifyIcon(NIM_ADD, mNotifyIconData);
    if (sResult == TRUE)
    {
        mHwnd = aHwnd;
    }
    else
    {
        destroyTray();
    }

    return sResult;
}

void SysTray::destroyTray(void)
{
    if (mNotifyIconData == NULL)
        return;

    ::Shell_NotifyIcon(NIM_DELETE, mNotifyIconData);
    ::DestroyIcon(mNotifyIconData->hIcon);
    mNotifyIconData->hIcon = NULL;
    delete mNotifyIconData;
    mNotifyIconData = NULL;
    mHwnd = NULL;
}

BOOL SysTray::recreateTray(void)
{
    NOTIFYICONDATA sNotifyIconData;
    memcpy(&sNotifyIconData, mNotifyIconData, sizeof(NOTIFYICONDATA));
    sNotifyIconData.hIcon = ::DuplicateIcon(AfxGetApp()->m_hInstance, mNotifyIconData->hIcon);

    destroyTray();

    return createTray(sNotifyIconData.hWnd, sNotifyIconData.uCallbackMessage, sNotifyIconData.uID, sNotifyIconData.szTip, sNotifyIconData.hIcon);
}

void SysTray::showHideWindow(BOOL aShow, BOOL aAnimate)
{
    if (aShow == TRUE)
    {
        showWndFromTray(aAnimate);
    }
    else
    {
        hideWndToTray(aAnimate);
    }
}

void SysTray::toggleWindow(BOOL aAnimate)
{
    if (::IsWindowVisible(mHwnd) == FALSE)
    {
        showWndFromTray(aAnimate);
    }
    else
    {
        hideWndToTray(aAnimate);
    }
}

void SysTray::showWndFromTray(BOOL aAnimate)
{
    ::SetForegroundWindow(mHwnd);

    UINT sStyle = CWnd::FromHandle(mHwnd)->GetStyle();
    if ((sStyle & WS_MINIMIZE) == WS_MINIMIZE)
    {
        ::ShowWindow(mHwnd, SW_RESTORE);
    }
    else
    {
        animateMaximizeFromTray();
        ::ShowWindow(mHwnd, SW_SHOW);
    }
}

void SysTray::hideWndToTray(BOOL aAnimate)
{
    animateMinimizeToTray();
    ::ShowWindow(mHwnd, SW_HIDE);
}

void SysTray::animateMinimizeToTray(void)
{
    CRect sFromRect;
    ::GetWindowRect(mHwnd, &sFromRect);

    CRect sToRect;
    getTrayWndRect(&sToRect);

    ::DrawAnimatedRects(mHwnd, IDANI_CAPTION, &sFromRect, &sToRect);
}

void SysTray::animateMaximizeFromTray(void)
{
    CRect sFromRect;
    getTrayWndRect(&sFromRect);

    WINDOWPLACEMENT sWindowPlacement;
    ::GetWindowPlacement(mHwnd, &sWindowPlacement); // It cannot get window rect by GetWindowRect API function on minimized window state.

    ::DrawAnimatedRects(mHwnd, IDANI_CAPTION, &sFromRect, &sWindowPlacement.rcNormalPosition);
}

void SysTray::getTrayWndRect(LPRECT aRect)
{
    if (aRect == NULL)
        return;

    HWND sTaskBarHwnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
    if (sTaskBarHwnd != NULL)
    {
        HWND sTrayHwnd = ::FindWindowEx(sTaskBarHwnd, NULL, _T("TrayNotifyWnd"), NULL);
        if (sTrayHwnd != NULL)
            ::GetWindowRect(sTrayHwnd, aRect);
        else
        {
            ::GetWindowRect(sTaskBarHwnd, aRect);
            aRect->left = aRect->right  - 20;
            aRect->top  = aRect->bottom - 20;
        }
    }
    else
    {
        int sWidth  = GetSystemMetrics(SM_CXSCREEN);
        int sHeight = GetSystemMetrics(SM_CYSCREEN);
        SetRect(aRect, sWidth-40, sHeight-20, sWidth, sHeight);
    }
}
