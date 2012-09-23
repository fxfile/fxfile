//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ShortcutDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ShortcutDlg::ShortcutDlg(const xpr_tchar_t *lpcszPath)
    : super(IDD_CREATE_SHORTCUT)
    , mFullPidl(XPR_NULL)
{
    _tcscpy(mPath, lpcszPath);
}

ShortcutDlg::~ShortcutDlg(void)
{
    COM_FREE(mFullPidl);
}

void ShortcutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ShortcutDlg, super)
    ON_BN_CLICKED(IDC_CREATE_SHORTCUT_TARGET_BROWSE, OnTargetBrowse)
END_MESSAGE_MAP()

xpr_bool_t ShortcutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CREATE_SHORTCUT_SHOWCMD);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.show.default")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.show.minimize")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.show.maximize")));

    ((CEdit *)GetDlgItem(IDC_CREATE_SHORTCUT_NAME   ))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CREATE_SHORTCUT_TARGET ))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CREATE_SHORTCUT_STARTUP))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CREATE_SHORTCUT_DESC   ))->LimitText(MAX_SHORTCUT_DESC);

    setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.top_desc")));
    setDescIcon(IDI_SHORTCUT, XPR_TRUE);
    WORD sInvalidComb = HKCOMB_A | HKCOMB_C | HKCOMB_NONE | HKCOMB_S | HKCOMB_SA | HKCOMB_SC | HKCOMB_SCA;
    ((CHotKeyCtrl *)GetDlgItem(IDC_CREATE_SHORTCUT_HOTKEY))->SetRules(sInvalidComb, HOTKEYF_CONTROL | HOTKEYF_ALT);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.title")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_NAME,     theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.name")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_TARGET,   theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.target")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_TARGET_BROWSE,  theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.button.target_browse")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_GROUP_ADVANCED, theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.group.advanced")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_STARTUP,  theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.startup")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_HOTKEY,   theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.hotkey")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_SHOWCMD,  theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.show")));
    SetDlgItemText(IDC_CREATE_SHORTCUT_LABEL_DESC,     theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.label.desc")));

    return XPR_TRUE;
}

void ShortcutDlg::OnOK(void) 
{
    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    xpr_tchar_t sTarget[XPR_MAX_PATH + 1];
    xpr_tchar_t sStartup[XPR_MAX_PATH + 1];
    xpr_tchar_t sDesc[MAX_SHORTCUT_DESC + 1];
    WORD sHotKey, sTemp;
    xpr_sint_t sShowCmd;

    GetDlgItemText(IDC_CREATE_SHORTCUT_NAME,    sName,    XPR_MAX_PATH);
    GetDlgItemText(IDC_CREATE_SHORTCUT_TARGET,  sTarget,  XPR_MAX_PATH);
    GetDlgItemText(IDC_CREATE_SHORTCUT_STARTUP, sStartup, XPR_MAX_PATH);
    GetDlgItemText(IDC_CREATE_SHORTCUT_DESC,    sDesc,    MAX_SHORTCUT_DESC);
    ((CHotKeyCtrl *)GetDlgItem(IDC_CREATE_SHORTCUT_HOTKEY))->GetHotKey(sHotKey, sTemp);
    sShowCmd = ((CComboBox *)GetDlgItem(IDC_CREATE_SHORTCUT_SHOWCMD))->GetCurSel();
    switch (sShowCmd)
    {
    case 1:  sShowCmd = SW_SHOWMINNOACTIVE; break;
    case 2:  sShowCmd = SW_SHOWMAXIMIZED;   break;
    default: sShowCmd = SW_SHOWNORMAL;      break;
    }

    if (_tcslen(sName) <= 0 || _tcslen(sTarget) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.msg.input"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    if (fxb::VerifyFileName(sName) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.msg.wrong_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    std::tstring sLinkPath(mPath);
    fxb::RemoveLastSplit(sLinkPath);
    sLinkPath += XPR_STRING_LITERAL("\\");
    sLinkPath += sName;
    sLinkPath += XPR_STRING_LITERAL(".lnk");

    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    ::SHGetPathFromIDList(mFullPidl, sPath);

    if (!_tcsicmp(sPath, sTarget))
        fxb::CreateShortcut(sLinkPath.c_str(), mFullPidl, sStartup, sHotKey, sShowCmd, sDesc);
    else
        fxb::CreateShortcut(sLinkPath.c_str(), sTarget, sStartup, sHotKey, sShowCmd, sDesc);

    super::OnOK();
}

void ShortcutDlg::OnTargetBrowse(void) 
{
    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.create_shortcut.folder_browse.title"));
    sBrowseInfo.ulFlags   = BIF_BROWSEINCLUDEFILES | BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl = XPR_NULL)
        return;

    std::tstring sName, sPath;
    fxb::GetName(sFullPidl, SHGDN_INFOLDER,   sName);
    fxb::GetName(sFullPidl, SHGDN_FORPARSING, sPath);

    fxb::RemoveInvalidChar(sName);

    COM_FREE(mFullPidl);
    mFullPidl = sFullPidl;

    SetDlgItemText(IDC_CREATE_SHORTCUT_NAME, sName.c_str());
    SetDlgItemText(IDC_CREATE_SHORTCUT_TARGET, sPath.c_str());
}
