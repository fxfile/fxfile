//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_property.h"

#include "context_menu.h"

#include "main_frame.h"
#include "folder_ctrl.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t FilePropertyCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FilePropertyCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
            ContextMenu::invokeCommand(sTvItemData->mShellFolder, (LPCITEMIDLIST *)&(sTvItemData->mPidl), 1, CMID_VERB_PROPERTIES, sFolderCtrl->GetSafeHwnd());
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelectedCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelectedCount > 0)
        {
            xpr_sint_t sCount = 0;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelectedCount];

            if (sSearchResultCtrl->getSelPidls(&sShellFolder, sPidls, sCount))
                ContextMenu::invokeCommand(sShellFolder, (LPCITEMIDLIST *)sPidls, sCount, CMID_VERB_PROPERTIES, sSearchResultCtrl->m_hWnd);

            xpr_sint_t i;
            for (i = 0; i < sCount; ++i)
            {
                COM_FREE(sPidls[i]);
            }

            COM_RELEASE(sShellFolder);
            XPR_SAFE_DELETE_ARRAY(sPidls);
        }
    }
    FXFILE_COMMAND_ELSE
    {
        xpr_sint_t sSelectedCount = sExplorerCtrl->GetSelectedCount();
        if (sSelectedCount > 0 && sExplorerCtrl->getRealSelCount() > 0)
        {
            xpr_sint_t sCount = 0;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPCITEMIDLIST *sPidls = XPR_NULL;

            sExplorerCtrl->getSelItemData(&sShellFolder, &sPidls, sCount);

            if (sCount > 0 && XPR_IS_NOT_NULL(sShellFolder))
                ContextMenu::invokeCommand(sShellFolder, sPidls, sCount, CMID_VERB_PROPERTIES, sExplorerCtrl->GetSafeHwnd());

            XPR_SAFE_DELETE_ARRAY(sPidls);
        }
        else
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
                ContextMenu::invokeCommand(sTvItemData->mShellFolder, (LPCITEMIDLIST *)&sTvItemData->mPidl, 1, CMID_VERB_PROPERTIES, sExplorerCtrl->GetSafeHwnd());
        }
    }
}
} // namespace cmd
} // namespace fxfile
