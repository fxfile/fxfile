//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "RenTabDlg.h"

#include "BatchRenameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RenTabDlg::RenTabDlg(xpr_uint_t aResourceId)
    : super(aResourceId, XPR_NULL)
    , mResourceId(aResourceId)
{
}

void RenTabDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(RenTabDlg, super)
END_MESSAGE_MAP()

xpr_bool_t RenTabDlg::Create(CWnd* pParentWnd) 
{
    return super::Create(mResourceId, pParentWnd);
}

xpr_bool_t RenTabDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (((BatchRenameDlg *)GetParent())->translateHotKey(pMsg))
        return XPR_TRUE;

    return super::PreTranslateMessage(pMsg);
}

void RenTabDlg::OnTabInit(void)
{
}
