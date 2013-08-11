//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_sort.h"

#include "resource.h"
#include "view_set.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t SortCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        ColumnSortInfo *sColumnSortInfo = sExplorerCtrl->getSortInfo();
        if (XPR_IS_NOT_NULL(sColumnSortInfo))
        {
            xpr_bool_t sChecked = XPR_FALSE;
            if (sColumnSortInfo->mFormatId == GUID_NULL)
            {
                switch (sCommandId)
                {
                case ID_VIEW_ARRAY_NAME: sChecked = (sColumnSortInfo->mPropertyId == 0); break;
                case ID_VIEW_ARRAY_SIZE: sChecked = (sColumnSortInfo->mPropertyId == 1); break;
                case ID_VIEW_ARRAY_TYPE: sChecked = (sColumnSortInfo->mPropertyId == 2); break;
                case ID_VIEW_ARRAY_DATE: sChecked = (sColumnSortInfo->mPropertyId == 3); break;
                case ID_VIEW_ARRAY_ATTR: sChecked = (sColumnSortInfo->mPropertyId == 4); break;
                case ID_VIEW_ARRAY_EXT:  sChecked = (sColumnSortInfo->mPropertyId == 5); break;
                default:                 sChecked = XPR_FALSE; break;
                }
            }

            if (XPR_IS_TRUE(sChecked))
                sState |= StateRadio;
        }
    }

    return sState;
}

void SortCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        ColumnId sColumnId = {0};
        switch (sCommandId)
        {
        case ID_VIEW_ARRAY_NAME: sColumnId.mPropertyId = 0; break;
        case ID_VIEW_ARRAY_SIZE: sColumnId.mPropertyId = 1; break;
        case ID_VIEW_ARRAY_TYPE: sColumnId.mPropertyId = 2; break;
        case ID_VIEW_ARRAY_DATE: sColumnId.mPropertyId = 3; break;
        case ID_VIEW_ARRAY_ATTR: sColumnId.mPropertyId = 4; break;
        case ID_VIEW_ARRAY_EXT:  sColumnId.mPropertyId = 5; break;
        }

        sExplorerCtrl->sortItems(&sColumnId);
    }
}

xpr_sint_t SortAscendingCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        if (sExplorerCtrl->getSortInfo()->mAscending == XPR_TRUE)
            sState |= StateRadio;
    }

    return sState;
}

void SortAscendingCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->sortItems(sExplorerCtrl->getSortInfo(), XPR_TRUE);
    }
}

xpr_sint_t SortDecendingCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        if (sExplorerCtrl->getSortInfo()->mAscending == XPR_FALSE)
            sState |= StateRadio;
    }

    return sState;
}

void SortDecendingCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->sortItems(sExplorerCtrl->getSortInfo(), XPR_FALSE);
    }
}
} // namespace cmd
} // namespace fxfile
