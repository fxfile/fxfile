//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvHotKeyDlg.h"

#include "../resource.h"
#include "../Option.h"
#include "../DlgStateMgr.h"
#include "../CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAdvHotKeyDlg::CfgAdvHotKeyDlg(void)
    : super(IDD_CFG_ADV_HOTKEY, XPR_NULL)
{
}

void CfgAdvHotKeyDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_GLOBAL_HOTKEY_HOTKEY, mHotKeyCtrl);
}

BEGIN_MESSAGE_MAP(CfgAdvHotKeyDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvHotKeyDlg::OnInitDialog(void)
{
    super::OnInitDialog();

    WORD sInvalidComb = HKCOMB_A | HKCOMB_CA | HKCOMB_NONE | HKCOMB_S | HKCOMB_SA | HKCOMB_SC | HKCOMB_SCA | HKCOMB_C;
    mHotKeyCtrl.SetRules(sInvalidComb, 0);

    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.check.enable")));
    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LABEL_HOTKEY,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.label.hotkey")));
    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LABEL_WINDOW_KEY,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.label.window_key")));
    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LABEL_CF,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.label.cf")));
    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_WIN_STARTUP, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.check.windows_boot")));
    SetDlgItemText(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_TRAY,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.global_hotkey.check.show_tray")));

    return XPR_TRUE;
}

void CfgAdvHotKeyDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER            ))->SetCheck(aConfig.mLauncher);
    ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_TRAY       ))->SetCheck(aConfig.mLauncherTray);
    ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_WIN_STARTUP))->SetCheck(aConfig.mLauncherWinStartup);
    mHotKeyCtrl.SetHotKey(aConfig.mLauncherGlobalHotKey, 0);
}

void CfgAdvHotKeyDlg::onApply(Option::Config &aConfig)
{
    WORD sVirtualKeyCode, sModifier;
    mHotKeyCtrl.GetHotKey(sVirtualKeyCode, sModifier);

    aConfig.mLauncher             = ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER))->GetCheck();
    aConfig.mLauncherTray         = ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_TRAY))->GetCheck();
    aConfig.mLauncherWinStartup   = ((CButton *)GetDlgItem(IDC_CFG_GLOBAL_HOTKEY_LAUNCHER_WIN_STARTUP))->GetCheck();
    aConfig.mLauncherGlobalHotKey = sVirtualKeyCode;
}

xpr_bool_t CfgAdvHotKeyDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == 0x300)
    {
        switch (sId)
        {
        case IDC_CFG_GLOBAL_HOTKEY_HOTKEY:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
