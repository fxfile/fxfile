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

#include "FolderCtrl.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t RefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void RefreshCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->refresh();
    }
    XPR_COMMAND_ELSE
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
