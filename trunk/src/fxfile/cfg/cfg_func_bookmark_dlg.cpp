//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_func_bookmark_dlg.h"

#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgFuncBookmarkDlg::CfgFuncBookmarkDlg(void)
    : super(IDD_CFG_FUNC_BOOKMARK, XPR_NULL)
{
}

void CfgFuncBookmarkDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgFuncBookmarkDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncBookmarkDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_CFG_BOOKMARK_EXPAND_FOLDER, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.go_and_expand_folder")));
    SetDlgItemText(IDC_CFG_BOOKMARK_REAL_PATH,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.set_real_path")));
    SetDlgItemText(IDC_CFG_BOOKMARK_SHOW_TOOLTIP,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.show_tooltip")));
    SetDlgItemText(IDC_CFG_BOOKMARK_FAST_NET_ICON, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.display_fast_network_icon")));

    return XPR_TRUE;
}

void CfgFuncBookmarkDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_SHOW_TOOLTIP ))->SetCheck(aConfig.mBookmarkTooltip);
    ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_EXPAND_FOLDER))->SetCheck(aConfig.mBookmarkExpandFolder);
    ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_REAL_PATH    ))->SetCheck(aConfig.mBookmarkRealPath);
    ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_FAST_NET_ICON))->SetCheck(aConfig.mBookmarkFastNetIcon);
}

void CfgFuncBookmarkDlg::onApply(Option::Config &aConfig)
{
    aConfig.mBookmarkTooltip      = ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_SHOW_TOOLTIP ))->GetCheck();
    aConfig.mBookmarkExpandFolder = ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_EXPAND_FOLDER))->GetCheck();
    aConfig.mBookmarkRealPath     = ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_REAL_PATH    ))->GetCheck();
    aConfig.mBookmarkFastNetIcon  = ((CButton *)GetDlgItem(IDC_CFG_BOOKMARK_FAST_NET_ICON))->GetCheck();
}

xpr_bool_t CfgFuncBookmarkDlg::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}
} // namespace cfg
} // namespace fxfile
