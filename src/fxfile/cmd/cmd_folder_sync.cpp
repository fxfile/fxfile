//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_folder_sync.h"

#include "folder_sync_dlg.h"
#include "main_frame.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t FolderSyncCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_EXPLORER_CTRL
    {
        sEnable = XPR_TRUE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FolderSyncCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr::string sDir[2];

    ExplorerCtrl *sExplorerCtrls[2];
    sExplorerCtrls[0] = sMainFrame->getExplorerCtrl();
    sExplorerCtrls[1] = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrls[0]) && sExplorerCtrls[0]->isFileSystemFolder() == XPR_TRUE) sDir[0] = sExplorerCtrls[0]->getCurPath();
    if (XPR_IS_NOT_NULL(sExplorerCtrls[1]) && sExplorerCtrls[1]->isFileSystemFolder() == XPR_TRUE) sDir[1] = sExplorerCtrls[1]->getCurPath();

    FolderSyncDlg sDlg;
    sDlg.setDir(sDir[0].c_str(), sDir[1].c_str());
    sDlg.DoModal();
}
} // namespace cmd
} // namespace fxfile
