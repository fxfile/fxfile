//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_go_path.h"

#include "GoPathDlg.h"
#include "HistoryDlg.h"

#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t GoPathCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoPathCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    LPITEMIDLIST sFullPidl1 = XPR_NULL;
    LPITEMIDLIST sFullPidl2 = XPR_NULL;

    ExplorerCtrl *sExplorerCtrl1 = sMainFrame->getExplorerCtrl();
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1)) sFullPidl1 = sExplorerCtrl1->getFolderData()->mFullPidl;
    if (XPR_IS_NOT_NULL(sExplorerCtrl2)) sFullPidl2 = sExplorerCtrl2->getFolderData()->mFullPidl;

    GoPathDlg sDlg;
    sDlg.setExpPath(sFullPidl1, sFullPidl2);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    sDlg.getPath(sPath);

    if (_tcslen(sPath) <= 0)
        return;

    xpr_bool_t sResult = XPR_FALSE;

    LPITEMIDLIST sFullPidl = fxb::Path2Pidl(sPath);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        ExplorerCtrl *sExplorerCtrl = sMainFrame->getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
        {
            sExplorerCtrl->explore(sFullPidl);
            sResult = XPR_TRUE;
        }

        if (XPR_IS_FALSE(sResult))
        {
            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        if (sDlg.isEnableUrl() == XPR_TRUE)
            fxb::NavigateURL(sPath);
    }
}

xpr_sint_t GoInitFolderCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolderCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder();
}

xpr_sint_t GoInitFolder1Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder1Command::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(0);
}

xpr_sint_t GoInitFolder2Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder2Command::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(1);
}

static void goSystemFolder(MainFrame &aMainFrame, xpr_sint_t aCSIDL)
{
    ExplorerCtrl *sExplorerCtrl = aMainFrame.getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        HRESULT sResult;
        LPITEMIDLIST sFullPidl = XPR_NULL;
        HWND sHwnd = aMainFrame.GetSafeHwnd();

        sResult = ::SHGetSpecialFolderLocation(sHwnd, aCSIDL, &sFullPidl);
        if (FAILED(sResult))
            return;

        sExplorerCtrl->explore(sFullPidl);
    }
}

xpr_sint_t GoSystemDesktopCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemDesktopCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_DESKTOP);
}

xpr_sint_t GoSystemMyDocumentCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemMyDocumentCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        HRESULT sHResult;
        LPITEMIDLIST sFullPidl = XPR_NULL;
        LPSHELLFOLDER sShellFolder = XPR_NULL;

        sHResult = ::SHGetDesktopFolder(&sShellFolder);
        if (FAILED(sHResult))
            return;

        sHResult = sShellFolder->ParseDisplayName(XPR_NULL, XPR_NULL, XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"), XPR_NULL, &sFullPidl, XPR_NULL);
        if (SUCCEEDED(sHResult))
            sExplorerCtrl->explore(sFullPidl);

        COM_RELEASE(sShellFolder);
    }
}

xpr_sint_t GoSystemMyComputerCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemMyComputerCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_DRIVES);
}

xpr_sint_t GoSystemNetworkCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemNetworkCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_NETWORK);
}

xpr_sint_t GoSystemControlsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemControlsCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_CONTROLS);
}

xpr_sint_t GoSystemRecycleCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemRecycleCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_BITBUCKET);
}

xpr_sint_t GoSystemProgramFilesCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemProgramFilesCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_PROGRAM_FILES);
}

xpr_sint_t GoSystemCommonCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemCommonCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_PROGRAM_FILES_COMMON);
}

xpr_sint_t GoSystemWindowsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemWindowsCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_WINDOWS);
}

xpr_sint_t GoSystemSystemCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemSystemCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_SYSTEM);
}

xpr_sint_t GoSystemFontsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemFontsCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_FONTS);
}

xpr_sint_t GoSystemStartupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemStartupCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_STARTUP);
}

xpr_sint_t GoSystemTemporaryCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemTemporaryCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        DWORD sLen = ::GetTempPath(XPR_MAX_PATH + 1, sPath);
        if (sLen > 0)
        {
            sPath[_tcslen(sPath) - 1] = XPR_STRING_LITERAL('\0');

            LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sPath);
            if (sFullPidl != XPR_NULL)
                sExplorerCtrl->explore(sFullPidl);
        }
    }
}

xpr_sint_t GoSystemFavoritesCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemFavoritesCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_FAVORITES);
}

xpr_sint_t GoSystemInternetCacheCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemInternetCacheCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_INTERNET_CACHE);
}

xpr_sint_t GoSystemInternetCookieCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemInternetCookieCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_COOKIES);
}

xpr_sint_t GoSystemSendToCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemSendToCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_SENDTO);
}

xpr_sint_t GoSystemRecentCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemRecentCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_RECENT);
}

xpr_sint_t GoSiblingUpCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = (gOpt->mSingleFolderPaneMode == XPR_FALSE) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoSiblingUpCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sFolderCtrl != XPR_NULL)
    {
        sFolderCtrl->goSiblingUp();
    }
}

xpr_sint_t GoSiblingDownCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = (gOpt->mSingleFolderPaneMode == XPR_FALSE) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoSiblingDownCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sFolderCtrl != XPR_NULL)
    {
        sFolderCtrl->goSiblingDown();
    }
}

xpr_sint_t GoDirectUpCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoDirectUpCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        sExplorerCtrl->goUp();
    }
}

xpr_sint_t GoRootCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoRootCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (fxb::IsFileSystemFolder(sTvItemData->mFullPidl) == XPR_FALSE)
            return;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
        sPath[3] = XPR_STRING_LITERAL('\0');

        LPITEMIDLIST sFullPidl = XPR_NULL;
        HRESULT sHResult = fxb::SHGetPidlFromPath(sPath, &sFullPidl);
        if (FAILED(sHResult) || XPR_IS_NULL(sFullPidl))
            return;

        sExplorerCtrl->explore(sFullPidl);
    }
}

xpr_sint_t GoUpBoxCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoUpBoxCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (sTvItemData == XPR_NULL)
            return;

        LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(sTvItemData->mFullPidl);
        if (sFullPidl == XPR_NULL)
            return;

        xpr_bool_t sRemoved;
        fxb::HistoryDeque sUpDeque;

        while (true)
        {
            sRemoved = fxb::IL_RemoveLastID(sFullPidl);
            if (sRemoved == XPR_FALSE)
                break;

            sUpDeque.push_back(fxb::CopyItemIDList(sFullPidl));
        }

        COM_FREE(sFullPidl);

        HistoryDlg sDlg;
        sDlg.addHistory(theApp.loadString(XPR_STRING_LITERAL("popup.up.title")), theApp.loadString(XPR_STRING_LITERAL("popup.up.tab")), &sUpDeque);

        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            xpr_size_t sHistory;
            xpr_uint_t sIndex;
            sDlg.getSelHistory(sHistory, sIndex);

            sExplorerCtrl->goUp(sIndex);
        }

        fxb::HistoryDeque::iterator sIterator = sUpDeque.begin();
        for (; sIterator != sUpDeque.end(); ++sIterator)
        {
            sFullPidl = *sIterator;
            COM_FREE(sFullPidl);
        }
        sUpDeque.clear();
    }
}

xpr_sint_t GoFlyExplorerCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoFlyExplorerCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetModuleDir(sPath, XPR_MAX_PATH);

        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sPath);
        if (XPR_IS_NOT_NULL(sFullPidl))
            sExplorerCtrl->explore(sFullPidl);
    }
}

xpr_sint_t GoUpCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoUpCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_sint_t sUp = sCommandId - ID_GO_UP_FIRST;

        xpr_sint_t sUpCount = sExplorerCtrl->getUpCount();
        if (sUpCount > 10)
            sUp += sUpCount - 10;

        sExplorerCtrl->goUp(sUp);
    }
}
} // namespace cmd
