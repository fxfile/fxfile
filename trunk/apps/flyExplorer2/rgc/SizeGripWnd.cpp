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

SizeGripWnd::SizeGripWnd()
{
}

SizeGripWnd::~SizeGripWnd()
{
}

BEGIN_MESSAGE_MAP(SizeGripWnd, CWnd)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL SizeGripWnd::Create(CWnd* pParentWnd, CRect rc, UINT nID) 
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	dwStyle |= SBS_SIZEBOX;
	dwStyle |= SBS_SIZEGRIP;
	dwStyle |= SBS_SIZEBOXBOTTOMRIGHTALIGN;

	return CWnd::Create(_T("SCROLLBAR"), NULL, dwStyle, rc, pParentWnd, nID, NULL);
}

BOOL SizeGripWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_SIZENWSE));
	SetCursor(hCursor);
	return TRUE;
}
