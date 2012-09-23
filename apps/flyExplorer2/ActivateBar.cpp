//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ActivateBar.h"

#include "rgc/MemDC.h"

#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("ActivateBar");

ActivateBar::ActivateBar(void)
    : mActivate(XPR_FALSE), mLastActivated(XPR_FALSE)
    , mBackColor(GetSysColor(COLOR_ACTIVECAPTION))
{
}

ActivateBar::~ActivateBar(void)
{
}

BEGIN_MESSAGE_MAP(ActivateBar, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

xpr_bool_t ActivateBar::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;

    WNDCLASS sWndClass;
    HINSTANCE sInstance = AfxGetInstanceHandle();

    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = kClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

xpr_sint_t ActivateBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CWnd::OnCreate(aCreateStruct) == -1)
        return -1;

    return 0;
}

void ActivateBar::OnDestroy(void)
{
    CWnd::OnDestroy();

}

void ActivateBar::activate(xpr_bool_t aActivate, xpr_bool_t aLastActivated)
{
    mActivate = aActivate;
    mLastActivated = aLastActivated;

    if (XPR_IS_TRUE(mActivate))
        mLastActivated = XPR_FALSE;

    if (XPR_IS_FALSE(mActivate))
    {
        if (XPR_IS_TRUE(aLastActivated))
            mBackColor = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), gOpt->mActiveViewColor, 150);
        else
            mBackColor = fxb::CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 50);
    }
    else
    {
        mBackColor = gOpt->mActiveViewColor;
    }

    Invalidate(XPR_FALSE);
    UpdateWindow();
}

xpr_bool_t ActivateBar::isActivated(void) const
{
    return mActivate;
}

void ActivateBar::setActivateBackColor(COLORREF aBackColor)
{
    if (XPR_IS_FALSE(mActivate))
        return;

    mBackColor = aBackColor;

    Invalidate(XPR_FALSE);
    UpdateWindow();
}

void ActivateBar::setBackColor(COLORREF aBackColor)
{
    mBackColor = aBackColor;
}

COLORREF ActivateBar::getBackColor(void) const
{
    return mBackColor;
}

void ActivateBar::OnPaint(void)
{
    CPaintDC sDC(this);
    CMemDC sMemDC(&sDC);

    CRect sClientRect;
    GetClientRect(&sClientRect);

    sMemDC.FillSolidRect(sClientRect, mBackColor);
}

xpr_bool_t ActivateBar::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE;
    //return CWnd::OnEraseBkgnd(pDC);
}
