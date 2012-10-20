//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_working_folder.h"

#include "MainFrame.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t WorkingFolderPopupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WorkingFolderPopupCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->popupWorkingFolderMenu();
}

xpr_sint_t GoWorkingFolderCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoWorkingFolderCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sIndex = sCommandId - ID_GO_WORKING_FOLDER_FIRST;
    if (!XPR_IS_RANGE(0, sIndex, MAX_WORKING_FOLDER-1))
        return;

    if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
    {
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                sMainFrame->setWorkingFolder(sIndex, sTvItemData->mFullPidl);
            }
        }
    }
    else
    {
        if (sMainFrame->goWorkingFolder(sIndex) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("working_folder.msg.not_set"));
            sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
        }
    }
}

xpr_sint_t WorkingFolderSetCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WorkingFolderSetCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sIndex = sCommandId - ID_GO_WORKING_FOLDER_SET_FIRST;
    if (!XPR_IS_RANGE(0, sIndex, MAX_WORKING_FOLDER-1))
        return;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            sMainFrame->setWorkingFolder(sIndex, sTvItemData->mFullPidl);
        }
    }
}

xpr_sint_t WorkingFolderResetCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WorkingFolderResetCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sIndex = sCommandId - ID_GO_WORKING_FOLDER_RESET_FIRST;
    if (!XPR_IS_RANGE(0, sIndex, MAX_WORKING_FOLDER-1))
        return;

    sMainFrame->resetWorkingFolder(sIndex);
}

xpr_sint_t WorkingFolderResetAllCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WorkingFolderResetAllCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->resetWorkingFolder();
}
} // namespace cmd
