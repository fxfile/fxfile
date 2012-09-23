//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_search.h"

#include "MainFrame.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t SearchLocationCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->mSearchBar.IsWindowVisible() == XPR_TRUE)
        sState = StateEnable;

    return sState;
}

void SearchLocationCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sMainFrame->mSearchBar.IsWindowVisible() == XPR_FALSE)
        return;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(sTvItemData->mFullPidl);

                if (sMainFrame->mSearchBar.getSearchDlg()->insertLocation(sFullPidl) == XPR_FALSE)
                {
                    COM_FREE(sFullPidl);
                }
            }
        }
    }
}
} // namespace cmd
