//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_dlg.h"

#include "batch_rename_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchRenameTabDlg::BatchRenameTabDlg(xpr_uint_t aResourceId)
    : super(aResourceId, XPR_NULL)
    , mResourceId(aResourceId)
{
}

void BatchRenameTabDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabDlg, super)
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabDlg::Create(CWnd* pParentWnd) 
{
    return super::Create(mResourceId, pParentWnd);
}

xpr_bool_t BatchRenameTabDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (((BatchRenameDlg *)GetParent())->translateHotKey(pMsg))
        return XPR_TRUE;

    return super::PreTranslateMessage(pMsg);
}

void BatchRenameTabDlg::OnTabInit(void)
{
}
} // namespace cmd
} // namespace fxfile
