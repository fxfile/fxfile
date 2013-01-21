//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TreeCtrlEx::TreeCtrlEx(void)
    : mVistaEnhanced(XPR_FALSE)
{
}

TreeCtrlEx::~TreeCtrlEx(void)
{
}

IMPLEMENT_DYNAMIC(TreeCtrlEx, CTreeCtrl);

BEGIN_MESSAGE_MAP(TreeCtrlEx, super)
END_MESSAGE_MAP()

xpr_bool_t TreeCtrlEx::hasChildItem(HTREEITEM aTreeItem) const
{
    TVITEM sTvItem = {0};
    sTvItem.hItem = aTreeItem;
    sTvItem.mask  = TVIF_CHILDREN;
    if (GetItem(&sTvItem) == XPR_FALSE)
        return XPR_FALSE;

    return (sTvItem.cChildren != 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t TreeCtrlEx::isVistaEnhanced(void) const
{
    return mVistaEnhanced;
}

void TreeCtrlEx::enableVistaEnhanced(xpr_bool_t aEnable)
{
    if (XPR_IS_TRUE(aEnable))
    {
        // enable explorer theme
        SetWindowTheme(m_hWnd, XPR_WIDE_STRING_LITERAL("explorer"), XPR_NULL);

        ModifyStyle(TVS_HASLINES, TVS_FULLROWSELECT | TVS_TRACKSELECT);

        DWORD sExStyle = TVS_EX_DOUBLEBUFFER | TVS_EX_FADEINOUTEXPANDOS | TVS_EX_AUTOHSCROLL;
        SetExtendedStyle(sExStyle, sExStyle);
    }
    else
    {
        // disable explorer theme
        SetWindowTheme(m_hWnd, XPR_NULL, XPR_NULL);

        ModifyStyle(TVS_FULLROWSELECT | TVS_TRACKSELECT, TVS_HASLINES);

        DWORD sExStyle = TVS_EX_DOUBLEBUFFER | TVS_EX_FADEINOUTEXPANDOS | TVS_EX_AUTOHSCROLL;
        SetExtendedStyle(sExStyle, 0);
    }

    mVistaEnhanced = aEnable;
}

DWORD TreeCtrlEx::GetExtendedStyle() const
{
    ASSERT(::IsWindow(m_hWnd));
    return (DWORD)::SendMessage(m_hWnd, TVM_GETEXTENDEDSTYLE, (WPARAM)0, 0);
}

DWORD TreeCtrlEx::SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles)
{
    ASSERT(::IsWindow(m_hWnd));
    return (DWORD)::SendMessage(m_hWnd, TVM_SETEXTENDEDSTYLE, (WPARAM)dwExMask, (LPARAM)dwExStyles);
}
