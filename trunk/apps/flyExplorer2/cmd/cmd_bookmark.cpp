//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_bookmark.h"

#include "BookmarkEditDlg.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t BookmarkPopupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BookmarkPopupCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->popupBookmarkMenu();
}

xpr_sint_t GoBookmarkCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoBookmarkCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sIndex = sCommandId - ID_BOOKMARK_FIRST;
    if (sIndex >= 0)
        sMainFrame->gotoBookmark(sIndex);
}

xpr_sint_t BookmarkAddCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BookmarkAddCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        LPITEMIDLIST sFullPidl = XPR_NULL;

        xpr_bool_t sUpdatedBookmark = XPR_FALSE;
        if (sExplorerCtrl->GetSelectedCount() > 0)
        {
            xpr_sint_t sIndex;
            xpr_sint_t sResult;
            LPLVITEMDATA sLvItemData = XPR_NULL;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData))
                {
                    sFullPidl = fxb::ConcatPidls(sTvItemData->mFullPidl, sLvItemData->mPidl);

                    sResult = sMainFrame->addBookmark(sFullPidl);
                    if (sResult == 1 || sResult == 0)
                        sUpdatedBookmark = XPR_TRUE;
                    else
                        break;

                    COM_FREE(sFullPidl);
                }
            }

            COM_FREE(sFullPidl);
        }
        else
        {
            sUpdatedBookmark = sMainFrame->addBookmark(sTvItemData->mFullPidl);
        }

        // Update Bookmark
        if (sUpdatedBookmark != 0)
            sMainFrame->updateBookmark();
    }
}

xpr_sint_t BookmarkModifyCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BookmarkModifyCommand::execute(CommandContext &aContext)
{
    BookmarkEditDlg sDlg;
    sDlg.DoModal();
}

xpr_sint_t BookmarkRefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BookmarkRefreshCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->updateBookmark();
}
} // namespace cmd
