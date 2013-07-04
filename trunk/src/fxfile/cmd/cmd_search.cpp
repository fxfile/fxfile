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

#include "main_frame.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t SearchLocationCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isVisibleSearchDlg() == XPR_TRUE)
        sState = StateEnable;

    return sState;
}

void SearchLocationCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isVisibleSearchDlg() == XPR_FALSE)
        return;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);

                if (sMainFrame->insertSearchLocation(sFullPidl) == XPR_FALSE)
                {
                    COM_FREE(sFullPidl);
                }
            }
        }
    }
}
} // namespace cmd
} // namespace fxfile
