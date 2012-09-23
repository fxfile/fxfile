//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgDlg::CfgDlg(xpr_uint_t aResourceId, CWnd *aParentWnd)
    : super(aResourceId, aParentWnd)
    , mResourceId(aResourceId)
{
}

void CfgDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgDlg, super)
    ON_MESSAGE(WM_APPLY, OnMsgApply)
END_MESSAGE_MAP()

xpr_bool_t CfgDlg::Create(xpr_uint_t aIndex, CWnd *aParentWnd) 
{
    mIndex = aIndex;
    return super::Create(mResourceId, aParentWnd);
}

LRESULT CfgDlg::OnMsgApply(WPARAM wParam, LPARAM lParam)
{
    OnApply();
    return XPR_TRUE;
}

void CfgDlg::OnApply(void)
{
}

xpr_bool_t CfgDlg::getModified(void)
{
    return (xpr_bool_t)::SendMessage(GetParent()->m_hWnd, WM_GETMODIFIED, (WPARAM)mIndex, 0);
}

void CfgDlg::setModified(xpr_bool_t aModified)
{
    ::SendMessage(GetParent()->m_hWnd, WM_SETMODIFIED, (WPARAM)mIndex, (LPARAM)aModified);
}

xpr_bool_t CfgDlg::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}
