//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncBookmarkDlg.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

    ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_SHOW_TOOLTIP ))->SetCheck(gOpt->mContentsTooltip);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_EXPAND_FOLDER))->SetCheck(gOpt->mContentsBookmarkExpandFolder);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_REAL_PATH    ))->SetCheck(gOpt->mContentsBookmarkRealPath);
    ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_FAST_NET_ICON))->SetCheck(gOpt->mContentsBookmarkFastNetIcon);

    SetDlgItemText(IDC_CFG_FUNC_BOOKMARK_EXPAND_FOLDER, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.go_and_expand_folder")));
    SetDlgItemText(IDC_CFG_FUNC_BOOKMARK_REAL_PATH,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.set_real_path")));
    SetDlgItemText(IDC_CFG_FUNC_BOOKMARK_SHOW_TOOLTIP,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.show_tooltip")));
    SetDlgItemText(IDC_CFG_FUNC_BOOKMARK_FAST_NET_ICON, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.bookmark.check.display_fast_network_icon")));

    return XPR_TRUE;
}

void CfgFuncBookmarkDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mContentsBookmarkExpandFolder = ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_EXPAND_FOLDER))->GetCheck();
    gOpt->mContentsBookmarkRealPath     = ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_REAL_PATH    ))->GetCheck();
    gOpt->mContentsTooltip              = ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_SHOW_TOOLTIP ))->GetCheck();
    gOpt->mContentsBookmarkFastNetIcon  = ((CButton *)GetDlgItem(IDC_CFG_FUNC_BOOKMARK_FAST_NET_ICON))->GetCheck();

    sOptionMgr.applyFolderCtrl(1, XPR_FALSE);
    sOptionMgr.applyExplorerView(1, XPR_FALSE);
    sOptionMgr.applyEtc(1);
}

xpr_bool_t CfgFuncBookmarkDlg::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}

xpr_bool_t CfgFuncBookmarkDlg::DestroyWindow(void) 
{
    return super::DestroyWindow();
}

xpr_bool_t CfgFuncBookmarkDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_FUNC_BOOKMARK_EXPAND_FOLDER:
        case IDC_CFG_FUNC_BOOKMARK_REAL_PATH:
        case IDC_CFG_FUNC_BOOKMARK_SHOW_TOOLTIP:
        case IDC_CFG_FUNC_BOOKMARK_FAST_NET_ICON:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
