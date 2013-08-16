//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "dos_cmd_dlg.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
#define MAX_CMD 1024

DosCmdDlg::DosCmdDlg(const xpr_tchar_t *aPath)
    : super(IDD_DOS_CMD, XPR_NULL)
    , mDlgState(XPR_NULL)
{
    if (aPath != XPR_NULL) _tcscpy(mPath, aPath);
    else                   _tcscpy(mPath, XPR_STRING_LITERAL("C:\\"));

    if (_tcslen(mPath) == 2)
        _tcscat(mPath, XPR_STRING_LITERAL("\\"));
}

void DosCmdDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_DOS_COMMAND, mComboBox);
}

BEGIN_MESSAGE_MAP(DosCmdDlg, super)
END_MESSAGE_MAP()

xpr_bool_t DosCmdDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    xpr_bool_t sNoClose = XPR_TRUE;

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("DosCmd"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Command"), mComboBox.GetDlgCtrlID());
        mDlgState->load();

        sNoClose = mDlgState->getStateB(XPR_STRING_LITERAL("No Close"), XPR_TRUE);
    }

    if (mComboBox.GetCount() > 0)
        mComboBox.SetCurSel(0);

    GetDlgItem(IDC_DOS_PATH)->SetWindowText(mPath);
    ((CButton *)GetDlgItem(IDC_DOS_NO_CLOSE))->SetCheck(sNoClose);

    mComboBox.LimitText(MAX_CMD);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.title")));
    SetDlgItemText(IDC_DOS_LABEL_PATH,    gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.label.path")));
    SetDlgItemText(IDC_DOS_LABEL_INPUT,   gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.label.input")));
    SetDlgItemText(IDC_DOS_LABEL_COMMAND, gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.label.command")));
    SetDlgItemText(IDC_DOS_NO_CLOSE,      gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.check.no_close")));
    SetDlgItemText(IDOK,                  gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,              gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

xpr_bool_t runElevated(HWND aHwnd,
                       const xpr_tchar_t *aPath,
                       const xpr_tchar_t *aParameters = XPR_NULL,
                       const xpr_tchar_t *aDirectory = XPR_NULL)
{
    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize       = sizeof(sShellExecuteInfo);
    sShellExecuteInfo.fMask        = 0;
    sShellExecuteInfo.hwnd         = aHwnd;
    sShellExecuteInfo.lpVerb       = XPR_STRING_LITERAL("runas");
    sShellExecuteInfo.lpFile       = aPath;
    sShellExecuteInfo.lpParameters = aParameters;
    sShellExecuteInfo.lpDirectory  = aDirectory;
    sShellExecuteInfo.nShow        = SW_NORMAL;

    return ::ShellExecuteEx(&sShellExecuteInfo);
}

void DosCmdDlg::OnOK(void) 
{
    // Get Command
    xpr_tchar_t sCommand[MAX_CMD + 1] = {0};
    mComboBox.GetWindowText(sCommand, MAX_CMD);
    if (_tcslen(sCommand) <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.dos_cmd.msg.input"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    // Check Flags
    xpr_bool_t sNoClose  = ((CButton *)GetDlgItem(IDC_DOS_NO_CLOSE))->GetCheck();
    //xpr_bool_t sUACAdmin = ((CButton *)GetDlgItem(IDC_DOS_UAC_ADMIN))->GetCheck();

    // Set Parameters
    xpr_tchar_t sParameters[MAX_CMD * 2] = {0};
    sParameters[0] = '\0';
    if (sNoClose) _tcscat(sParameters, XPR_STRING_LITERAL("/k "));
    else          _tcscat(sParameters, XPR_STRING_LITERAL("/c "));
    _tcscat(sParameters, sCommand);

    // Dos Command Execution
    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
#ifdef XPR_CFG_UNICODE
    sShellExecuteInfo.fMask        = SEE_MASK_UNICODE;
#else
    sShellExecuteInfo.fMask        = 0;
#endif
    sShellExecuteInfo.hwnd         = *AfxGetMainWnd();
    sShellExecuteInfo.nShow        = SW_SHOWDEFAULT;
    sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();
    sShellExecuteInfo.lpDirectory  = mPath;
    sShellExecuteInfo.lpFile       = (xpr::getOsVer() >= xpr::kOsVerWinNT) ? XPR_STRING_LITERAL("cmd.exe") : XPR_STRING_LITERAL("command.com");
    sShellExecuteInfo.lpParameters = sParameters;
    //sShellExecuteInfo.lpVerb       = sUACAdmin ? XPR_STRING_LITERAL("runas") : XPR_NULL;

    ::ShellExecuteEx(&sShellExecuteInfo);

    // Save Command
    DlgState::insertComboEditString(&mComboBox);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->setStateB(XPR_STRING_LITERAL("No Close"), sNoClose);
        mDlgState->save();
    }

    super::OnOK();
}
} // namespace cmd
} // namespace fxfile
