//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "about_tab_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace cmd
{
AboutTabDlg::AboutTabDlg(xpr_uint_t aResourceId)
    : super(aResourceId, XPR_NULL)
    , mResourceId(aResourceId)
{
}

void AboutTabDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(AboutTabDlg, super)
END_MESSAGE_MAP()

xpr_bool_t AboutTabDlg::Create(CWnd *sParentWnd) 
{
    return super::Create(mResourceId, sParentWnd);
}

xpr_bool_t AboutTabDlg::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}

void AboutTabDlg::OnTabInit(void)
{
}
} // namespace cmd
} // namespace fxfile
