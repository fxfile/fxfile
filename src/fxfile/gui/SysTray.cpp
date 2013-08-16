//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SysTray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SysTray::SysTray(void)
    : mHwnd(XPR_NULL)
    , mNotifyIconData(XPR_NULL)
{
}

SysTray::~SysTray(void)
{
    destroyTray();
}

xpr_bool_t SysTray::createTray(HWND aWnd, xpr_uint_t aMsg, xpr_uint_t aId, const xpr_tchar_t *aToolTip, HICON aIcon)
{
    if (XPR_IS_NULL(aWnd) || aMsg == 0 || aId == 0 || XPR_IS_NULL(aToolTip) || XPR_IS_NULL(aIcon))
        return XPR_FALSE;

    if (XPR_IS_NOT_NULL(mNotifyIconData))
        return XPR_FALSE;

    mNotifyIconData = new NOTIFYICONDATA;
    if (XPR_IS_NULL(mNotifyIconData))
        return XPR_FALSE;

    memset(mNotifyIconData, 0, sizeof(NOTIFYICONDATA));
    mNotifyIconData->cbSize           = sizeof(NOTIFYICONDATA);
    mNotifyIconData->uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    mNotifyIconData->hWnd             = aWnd;
    mNotifyIconData->hIcon            = aIcon;
    mNotifyIconData->uCallbackMessage = aMsg;
    mNotifyIconData->uID              = aId;
    _tcscpy(mNotifyIconData->szTip, aToolTip);

    xpr_bool_t sResult = ::Shell_NotifyIcon(NIM_ADD, mNotifyIconData);
    if (XPR_IS_TRUE(sResult))
    {
        mHwnd = aWnd;
    }
    else
    {
        destroyTray();
    }

    return sResult;
}

void SysTray::destroyTray(void)
{
    if (XPR_IS_NULL(mNotifyIconData))
        return;

    ::Shell_NotifyIcon(NIM_DELETE, mNotifyIconData);
    DESTROY_ICON(mNotifyIconData->hIcon);
    XPR_SAFE_DELETE(mNotifyIconData);
    mHwnd = XPR_NULL;
}

xpr_bool_t SysTray::recreateTray(void)
{
    NOTIFYICONDATA sNotifyIconData;
    memcpy(&sNotifyIconData, mNotifyIconData, sizeof(NOTIFYICONDATA));
    sNotifyIconData.hIcon = ::DuplicateIcon(fxfile::gApp.m_hInstance, mNotifyIconData->hIcon);

    destroyTray();

    return createTray(sNotifyIconData.hWnd, sNotifyIconData.uCallbackMessage, sNotifyIconData.uID, sNotifyIconData.szTip, sNotifyIconData.hIcon);
}

void SysTray::showWindow(xpr_bool_t aShow, xpr_bool_t aAnimate)
{
    if (XPR_IS_TRUE(aShow))
    {
        showFromTray(aAnimate);
    }
    else
    {
        hideToTray(aAnimate);
    }
}

void SysTray::toggleWindow(xpr_bool_t aAnimate)
{
    if (::IsWindowVisible(mHwnd) == XPR_FALSE)
    {
        showFromTray(aAnimate);
    }
    else
    {
        hideToTray(aAnimate);
    }
}

void SysTray::showFromTray(xpr_bool_t aAnimate)
{
    ::SetForegroundWindow(mHwnd);

    xpr_uint_t uStyle = CWnd::FromHandle(mHwnd)->GetStyle();
    if (XPR_TEST_BITS(uStyle, WS_MINIMIZE))
    {
        ::ShowWindow(mHwnd, SW_RESTORE);
    }
    else
    {
        animateMaximizeFromTray();
        ::ShowWindow(mHwnd, SW_SHOW);
    }
}

void SysTray::hideToTray(xpr_bool_t aAnimate)
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

    // I cannot get window rectagle by GetWindowRect API function on minimized window state.
    WINDOWPLACEMENT sWindowPlacement;
    ::GetWindowPlacement(mHwnd, &sWindowPlacement);

    ::DrawAnimatedRects(mHwnd, IDANI_CAPTION, &sFromRect, &sWindowPlacement.rcNormalPosition);
}

void SysTray::getTrayWndRect(LPRECT aRect)
{
    if (XPR_IS_NULL(aRect))
        return;

    HWND aTaskBarHwnd = ::FindWindow(XPR_STRING_LITERAL("Shell_TrayWnd"), XPR_NULL);
    if (XPR_IS_NOT_NULL(aTaskBarHwnd))
    {
        HWND sTrayHwnd = ::FindWindowEx(aTaskBarHwnd, XPR_NULL, XPR_STRING_LITERAL("TrayNotifyWnd"), XPR_NULL);
        if (XPR_IS_NOT_NULL(sTrayHwnd))
            ::GetWindowRect(sTrayHwnd, aRect);
        else
        {
            ::GetWindowRect(aTaskBarHwnd, aRect);
            aRect->left = aRect->right  - 20;
            aRect->top  = aRect->bottom - 20;
        }
    }
    else
    {
        xpr_sint_t sWidth  = GetSystemMetrics(SM_CXSCREEN);
        xpr_sint_t sHeight = GetSystemMetrics(SM_CYSCREEN);
        SetRect(aRect, sWidth-40, sHeight-20, sWidth, sHeight);
    }
}
