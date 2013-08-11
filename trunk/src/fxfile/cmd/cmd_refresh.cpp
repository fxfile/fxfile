//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_refresh.h"

#include "folder_ctrl.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t RefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void RefreshCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->refresh();
    }
    FXFILE_COMMAND_ELSE
    {
        if (XPR_IS_NOT_NULL(sFolderCtrl) && XPR_IS_NOT_NULL(sFolderCtrl->m_hWnd))
        {
            HTREEITEM sRootTreeItem = sFolderCtrl->GetRootItem();
            HTREEITEM sTreeItem = sFolderCtrl->GetChildItem(sRootTreeItem);
            sFolderCtrl->refresh(sTreeItem);
        }

        if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
            sExplorerCtrl->refresh();
    }
}
} // namespace cmd
} // namespace fxfile
