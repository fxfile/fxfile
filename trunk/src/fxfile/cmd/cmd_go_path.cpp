//
// Copyright (c) 2012-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_go_path.h"

#include "winapi_ex.h"
#include "option.h"
#include "main_frame.h"
#include "folder_ctrl.h"
#include "explorer_view.h"
#include "explorer_ctrl.h"
#include "go_path_dlg.h"
#include "history_dlg.h"
#include "router/cmd_parameters.h"
#include "router/cmd_parameter_define.h"

#include <Knownfolders.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t GoPathCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoPathCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    LPITEMIDLIST sFullPidl1 = XPR_NULL;
    LPITEMIDLIST sFullPidl2 = XPR_NULL;

    GoPathDlg sDlg;
    sDlg.setExpPath(sFullPidl1, sFullPidl2);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    sDlg.getPath(sPath);

    if (_tcslen(sPath) <= 0)
        return;

    xpr_bool_t sResult = XPR_FALSE;

    LPITEMIDLIST sFullPidl = Path2Pidl(sPath);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        if (XPR_IS_TRUE(IsFolder(sFullPidl)))
        {
            if (XPR_IS_TRUE(sDlg.isNewTab()))
            {
                cmd::CommandParameters sParameters;
                sParameters.set(cmd::CommandParameterIdPidl, (void *)sFullPidl);

                sMainFrame->executeCommand(ID_WINDOW_TAB_NEW, XPR_NULL, &sParameters);

                COM_FREE(sFullPidl);
            }
            else
            {
                ExplorerCtrl *sExplorerCtrl = sMainFrame->getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
                {
                    sResult = sExplorerCtrl->explore(sFullPidl);
                }

                if (XPR_IS_FALSE(sResult))
                {
                    COM_FREE(sFullPidl);
                }
            }
        }
        else
        {
            if (sDlg.isEnableFile() == XPR_TRUE)
            {
                sResult = ExecFile(sFullPidl);
            }

            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        if (sDlg.isEnableUrl() == XPR_TRUE)
        {
            NavigateURL(sPath);
        }
    }
}

xpr_sint_t GoInitFolderAllCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolderAllCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder();
}

xpr_sint_t GoInitFolder1Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder1Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(0);
}

xpr_sint_t GoInitFolder2Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder2Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(1);
}

xpr_sint_t GoInitFolder3Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder3Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(2);
}

xpr_sint_t GoInitFolder4Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder4Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(3);
}

xpr_sint_t GoInitFolder5Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder5Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(4);
}

xpr_sint_t GoInitFolder6Command::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoInitFolder6Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->goInitFolder(5);
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

static xpr_bool_t goNewSystemFolder(MainFrame &aMainFrame, KNOWNFOLDERID aKnownFolderId)
{
    xpr_bool_t sResult = XPR_FALSE;

    ExplorerCtrl *sExplorerCtrl = aMainFrame.getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPITEMIDLIST sFullPidl = XPR_NULL;
        HRESULT sHResult = fxfile::base::Pidl::create(aKnownFolderId, sFullPidl);
        if (SUCCEEDED(sHResult))
        {
            if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
            {
                sResult = XPR_TRUE;
            }
        }

        if (XPR_IS_FALSE(sResult))
        {
            COM_FREE(sFullPidl);
        }
    }

    return sResult;
}

xpr_sint_t GoSystemDesktopCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemDesktopCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_DESKTOP);
}

xpr_sint_t GoSystemLibrariesCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemLibrariesCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goNewSystemFolder(*sMainFrame, FOLDERID_Libraries);
}

xpr_sint_t GoSystemDocumentsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemDocumentsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (goNewSystemFolder(*sMainFrame, FOLDERID_DocumentsLibrary) == XPR_FALSE)
    {
        goSystemFolder(*sMainFrame, CSIDL_MYDOCUMENTS);
    }
}

xpr_sint_t GoSystemPicturesCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemPicturesCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (goNewSystemFolder(*sMainFrame, FOLDERID_PicturesLibrary) == XPR_FALSE)
    {
        goSystemFolder(*sMainFrame, CSIDL_MYPICTURES);
    }
}

xpr_sint_t GoSystemMusicCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemMusicCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (goNewSystemFolder(*sMainFrame, FOLDERID_MusicLibrary) == XPR_FALSE)
    {
        goSystemFolder(*sMainFrame, CSIDL_MYMUSIC);
    }
}

xpr_sint_t GoSystemVideosCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemVideosCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (goNewSystemFolder(*sMainFrame, FOLDERID_VideosLibrary) == XPR_FALSE)
    {
        goSystemFolder(*sMainFrame, CSIDL_MYVIDEO);
    }
}

xpr_sint_t GoSystemMyDocumentCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemMyDocumentCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        HRESULT sHResult;
        LPITEMIDLIST sFullPidl = XPR_NULL;
        LPSHELLFOLDER sShellFolder = XPR_NULL;

        sHResult = ::SHGetDesktopFolder(&sShellFolder);
        if (FAILED(sHResult))
            return;

        sHResult = sShellFolder->ParseDisplayName(
            XPR_NULL,
            XPR_NULL,
            XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"),
            XPR_NULL,
            &sFullPidl,
            XPR_NULL);

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
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_DRIVES);
}

xpr_sint_t GoSystemNetworkCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemNetworkCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_NETWORK);
}

xpr_sint_t GoSystemControlsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemControlsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_CONTROLS);
}

xpr_sint_t GoSystemRecycleCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemRecycleCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_BITBUCKET);
}

xpr_sint_t GoSystemProgramFilesCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemProgramFilesCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_PROGRAM_FILES);
}

xpr_sint_t GoSystemCommonCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemCommonCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_PROGRAM_FILES_COMMON);
}

xpr_sint_t GoSystemWindowsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemWindowsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_WINDOWS);
}

xpr_sint_t GoSystemWindowsSystemCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemWindowsSystemCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_SYSTEM);
}

xpr_sint_t GoSystemFontsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemFontsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_FONTS);
}

xpr_sint_t GoSystemStartupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemStartupCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_STARTUP);
}

xpr_sint_t GoSystemTemporaryCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemTemporaryCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        DWORD sLen = ::GetTempPath(XPR_MAX_PATH + 1, sPath);
        if (sLen > 0)
        {
            sPath[_tcslen(sPath) - 1] = XPR_STRING_LITERAL('\0');

            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sPath);
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
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_FAVORITES);
}

xpr_sint_t GoSystemInternetCacheCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemInternetCacheCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_INTERNET_CACHE);
}

xpr_sint_t GoSystemInternetCookieCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemInternetCookieCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_COOKIES);
}

xpr_sint_t GoSystemSendToCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemSendToCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_SENDTO);
}

xpr_sint_t GoSystemRecentCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemRecentCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_RECENT);
}

xpr_sint_t GoSystemAppDataCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoSystemAppDataCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    goSystemFolder(*sMainFrame, CSIDL_APPDATA);
}

xpr_sint_t GoSiblingUpCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = (gOpt->mMain.mSingleFolderPaneMode == XPR_FALSE) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoSiblingUpCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sFolderCtrl != XPR_NULL)
    {
        sFolderCtrl->goSiblingUp();
    }
}

xpr_sint_t GoSiblingDownCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = (gOpt->mMain.mSingleFolderPaneMode == XPR_FALSE) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoSiblingDownCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

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
    FXFILE_COMMAND_DECLARE_CTRL;

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
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (IsFileSystemFolder(sTvItemData->mFullPidl) == XPR_FALSE)
            return;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
        sPath[3] = XPR_STRING_LITERAL('\0');

        LPITEMIDLIST sFullPidl = XPR_NULL;
        HRESULT sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
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
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (sTvItemData == XPR_NULL)
            return;

        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
        if (sFullPidl == XPR_NULL)
            return;

        xpr_bool_t sRemoved;
        HistoryDeque sUpDeque;

        while (true)
        {
            sRemoved = fxfile::base::Pidl::removeLastItem(sFullPidl);
            if (sRemoved == XPR_FALSE)
                break;

            sUpDeque.push_back(fxfile::base::Pidl::clone(sFullPidl));
        }

        COM_FREE(sFullPidl);

        HistoryDlg sDlg;
        sDlg.addHistory(gApp.loadString(XPR_STRING_LITERAL("popup.up.title")), gApp.loadString(XPR_STRING_LITERAL("popup.up.tab")), &sUpDeque);

        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            xpr_size_t sHistory;
            xpr_uint_t sIndex;
            sDlg.getSelHistory(sHistory, sIndex);

            sExplorerCtrl->goUp(sIndex);
        }

        HistoryDeque::iterator sIterator = sUpDeque.begin();
        for (; sIterator != sUpDeque.end(); ++sIterator)
        {
            sFullPidl = *sIterator;
            COM_FREE(sFullPidl);
        }
        sUpDeque.clear();
    }
}

xpr_sint_t GoFxFileCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoFxFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetModuleDir(sPath, XPR_MAX_PATH);

        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sPath);
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
    FXFILE_COMMAND_DECLARE_CTRL;

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
} // namespace fxfile
