//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SizeGripWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SizeGripWnd::SizeGripWnd(void)
{
}

SizeGripWnd::~SizeGripWnd(void)
{
}

BEGIN_MESSAGE_MAP(SizeGripWnd, CWnd)
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

xpr_bool_t SizeGripWnd::Create(CWnd *aParentWnd, CRect aRect, xpr_uint_t aId)
{
    DWORD sStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
    sStyle |= SBS_SIZEBOX;
    sStyle |= SBS_SIZEGRIP;
    sStyle |= SBS_SIZEBOXBOTTOMRIGHTALIGN;

    return CWnd::Create(XPR_STRING_LITERAL("SCROLLBAR"), XPR_NULL, sStyle, aRect, aParentWnd, aId, XPR_NULL);
}

xpr_bool_t SizeGripWnd::OnSetCursor(CWnd *aWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage)
{
    HCURSOR sCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_SIZENWSE));
    SetCursor(sCursor);
    return XPR_TRUE;
}
