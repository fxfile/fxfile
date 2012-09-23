//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_tools.h"

#include "fxb/fxb_trash.h"
#include "fxb/fxb_size_format.h"

#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerCtrl.h"
#include "DosCmdDlg.h"
#include "SharedProcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t EmptyRecycleBinCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void EmptyRecycleBinCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    HRESULT sHResult;

    xpr_sint64_t sCount = 0i64;
    xpr_sint64_t sSize = 0i64;

    fxb::Trash::getQueryInfo(&sCount, &sSize);

    if (sCount <= 0i64)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.trash_empty.msg.already_empty"));
        sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
        return;
    }

    xpr_tchar_t sSizeText[0xff] = {0};
    xpr_tchar_t sSizeUnitText[0xff] = {0};
    xpr_tchar_t sCountText[0xff] = {0};
    fxb::GetFormatedNumber(sSize,  sSizeText,  0xfe);
    fxb::GetFormatedNumber(sCount, sCountText, 0xfe);
    fxb::SizeFormat::getSizeUnitFormat(sSize, SIZE_UNIT_AUTO, sSizeUnitText, 0xfe);

    xpr_tchar_t sMsg[0xff] = {0};
    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.trash_empty.msg.confirm_empty"), XPR_STRING_LITERAL("%s,%s,%s")), sSizeUnitText, sSizeText, sCountText);
    xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    CWaitCursor sWaitCursor;
    sHResult = fxb::Trash::empty(sMainFrame->GetSafeHwnd(), XPR_NULL, SHERB_NOCONFIRMATION);
    if (SUCCEEDED(sHResult))
    {
        xpr_tchar_t sFullPath1[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sFullPath2[XPR_MAX_PATH + 1] = {0};

        LPITEMIDLIST sPidl = XPR_NULL;
        ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_BITBUCKET, &sPidl);
        fxb::GetFullPath(sPidl, sFullPath1);
        COM_FREE(sPidl);

        xpr_sint_t j;
        xpr_sint_t sViewCount = sMainFrame->getViewCount();

        for (j = 0; j < sViewCount; ++j)
        {
            sExplorerCtrl = sMainFrame->getExplorerCtrl(j);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                if (XPR_IS_NULL(sTvItemData))
                    continue;

                fxb::GetFullPath(sTvItemData->mFullPidl, sFullPath2);
                if (_tcscmp(sFullPath1, sFullPath2) == 0)
                    sExplorerCtrl->refresh();
            }
        }

        sWaitCursor.Restore();

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.trash_empty.msg.complete"));
        sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
    }
}

xpr_sint_t WindowsExplorerCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WindowsExplorerCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        // /n - new window
        // /e - with folder pane
        // ex) explorer.exe /n,/e,c:\\WinNT

        xpr_tchar_t sPath[XPR_MAX_PATH + 1*2] = {0};
        _tcscpy(sPath, XPR_STRING_LITERAL("/n,/e,"));

        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();

        HRESULT sHResult = fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath+_tcslen(sPath));
        if (SUCCEEDED(sHResult))
            fxb::ExecFile(XPR_STRING_LITERAL("explorer.exe"), XPR_NULL, sPath);
    }
}

xpr_sint_t CmdCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void CmdCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_tchar_t sStartup[XPR_MAX_PATH + 1] = {0};

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                _tcscpy(sStartup, sExplorerCtrl->getCurPath());
        }
    }

    if (sStartup[0] == 0)
        _tcscpy(sStartup, XPR_STRING_LITERAL("C:\\"));

    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize      = sizeof(SHELLEXECUTEINFO);
#ifdef XPR_CFG_UNICODE
    sShellExecuteInfo.fMask       = SEE_MASK_UNICODE;
#else
    sShellExecuteInfo.fMask       = 0;
#endif
    sShellExecuteInfo.hwnd        = sMainFrame->GetSafeHwnd();
    sShellExecuteInfo.nShow       = SW_SHOWDEFAULT;
    sShellExecuteInfo.hInstApp    = ::AfxGetInstanceHandle();
    sShellExecuteInfo.lpDirectory = sStartup;
    sShellExecuteInfo.lpFile      = XPR_IS_TRUE(fxb::UserEnv::instance().mPlatformNT) ? XPR_STRING_LITERAL("cmd.exe") : XPR_STRING_LITERAL("command.com");

    ::ShellExecuteEx(&sShellExecuteInfo);
}

xpr_sint_t DosCmdCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void DosCmdCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                _tcscpy(sPath, sExplorerCtrl->getCurPath());
        }
    }

    if (sPath[0] == 0)
        _tcscpy(sPath, XPR_STRING_LITERAL("C:\\"));

    DosCmdDlg sDlg(sPath);
    sDlg.DoModal();
}

xpr_sint_t SharedProcCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SharedProcCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);

            SharedProcDlg sDlg(sPath);
            sDlg.DoModal();
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();

        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            SharedProcDlg sDlg(sPath);
            sDlg.DoModal();
        }
    }
}
} // namespace cmd
