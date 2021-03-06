//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ListCtrlEx.h"

#include "FlatHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ListCtrlEx::ListCtrlEx(void)
    : mVistaEnhanced(XPR_FALSE)
    , mHeaderCtrl(XPR_NULL)
{
}

BEGIN_MESSAGE_MAP(ListCtrlEx, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_sint_t ListCtrlEx::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    // header control subclassing
    if (subclassHeader(XPR_FALSE) == XPR_FALSE)
        return -1;

    return 0;
}

void ListCtrlEx::OnDestroy(void)
{
    super::OnDestroy();

    XPR_SAFE_DELETE(mHeaderCtrl);
}

xpr_bool_t ListCtrlEx::isVistaEnhanced(void) const
{
    return mVistaEnhanced;
}

void ListCtrlEx::enableVistaEnhanced(xpr_bool_t aEnable)
{
    if (XPR_IS_TRUE(aEnable))
    {
        // enable explorer theme
        SetWindowTheme(m_hWnd, XPR_WIDE_STRING_LITERAL("explorer"), XPR_NULL);
    }
    else
    {
        // diable explorer theme
        SetWindowTheme(m_hWnd, XPR_NULL, XPR_NULL);
    }

    DWORD sExStyle = GetExtendedStyle();
    sExStyle |= LVS_EX_DOUBLEBUFFER; // support from WinXP
    SetExtendedStyle(sExStyle);

    mVistaEnhanced = aEnable;
}

xpr_bool_t ListCtrlEx::subclassHeader(xpr_bool_t aBoldFont)
{
    if (XPR_IS_NOT_NULL(mHeaderCtrl))
        return XPR_TRUE;

    mHeaderCtrl = new FlatHeaderCtrl;
    ASSERT(mHeaderCtrl);

    HWND sHwnd = GetDlgItem(0)->GetSafeHwnd();
    if (XPR_IS_NULL(sHwnd))
        return XPR_FALSE;

    if (mHeaderCtrl->SubclassWindow(sHwnd) == XPR_FALSE)
        return XPR_FALSE;

    mHeaderCtrl->initHeader(aBoldFont);

    return XPR_TRUE;
}

xpr_sint_t ListCtrlEx::setSortImage(xpr_sint_t aColumn, xpr_bool_t aAscending)
{
    if (XPR_IS_NULL(mHeaderCtrl))
        return -1;

    return mHeaderCtrl->setSortImage(aColumn, aAscending);
}

LRESULT ListCtrlEx::WindowProc(xpr_uint_t msg, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lResult = super::WindowProc(msg, wParam, lParam);

    if (msg == WM_SETREDRAW)
    {
        xpr_bool_t sRedraw = (xpr_bool_t)wParam;
        if (XPR_IS_TRUE(sRedraw))
        {
            if (XPR_IS_NOT_NULL(mHeaderCtrl))
                mHeaderCtrl->Invalidate(XPR_FALSE);
        }
    }

    return lResult;
}
