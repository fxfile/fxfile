//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "EditScroll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

EditScroll::EditScroll(void)
{
}

EditScroll::~EditScroll(void)
{
}

BEGIN_MESSAGE_MAP(EditScroll, CEdit)
    ON_WM_VSCROLL()
END_MESSAGE_MAP()

void EditScroll::OnVScroll(xpr_uint_t aSBCode, xpr_uint_t aPos, CScrollBar* aScrollBar)
{
    CEdit::OnVScroll(aSBCode, aPos, aScrollBar);

    if (aSBCode == SB_THUMBPOSITION)
        GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_VSCROLL), (LPARAM)m_hWnd);
    else if (aSBCode == SB_THUMBTRACK)
        GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_VSCROLL), (LPARAM)m_hWnd);
}
