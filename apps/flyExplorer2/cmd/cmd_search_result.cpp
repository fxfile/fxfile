//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_search_result.h"

#include "fxb/fxb_file_scrap.h"

#include "MainFrame.h"
#include "SearchResultCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t SearchResultViewIconCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sState |= StateEnable;

        DWORD sViewStyle = sSearchResultCtrl->GetStyle() & LVS_TYPEMASK;
        if (sViewStyle == LVS_ICON)
            sState |= StateRadio;
    }

    return sState;
}

void SearchResultViewIconCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->ModifyStyle(LVS_TYPEMASK, LVS_ICON);
    }
}

xpr_sint_t SearchResultViewSmallIconCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sState |= StateEnable;

        DWORD sViewStyle = sSearchResultCtrl->GetStyle() & LVS_TYPEMASK;
        if (sViewStyle == LVS_SMALLICON)
            sState |= StateRadio;
    }

    return sState;
}

void SearchResultViewSmallIconCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
    }
}

xpr_sint_t SearchResultViewListCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sState |= StateEnable;

        DWORD sViewStyle = sSearchResultCtrl->GetStyle() & LVS_TYPEMASK;
        if (sViewStyle == LVS_LIST)
            sState |= StateRadio;
    }

    return sState;
}

void SearchResultViewListCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->ModifyStyle(LVS_TYPEMASK, LVS_LIST);
    }
}

xpr_sint_t SearchResultViewDetailCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sState |= StateEnable;

        DWORD sViewStyle = sSearchResultCtrl->GetStyle() & LVS_TYPEMASK;
        if (sViewStyle == LVS_REPORT)
            sState |= StateRadio;
    }

    return sState;
}

void SearchResultViewDetailCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
    }
}

xpr_sint_t SearchResultDeleteAllCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
        sState |= StateEnable;

    return sState;
}

void SearchResultDeleteAllCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        CWaitCursor sWaitCursor;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        LPSRITEMDATA sSrItemData = XPR_NULL;

        xpr_sint_t sIndex;
        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (LPSRITEMDATA)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                sFileScrap.addItem(sGroupId, sPath);
            }
        }
    }
}

xpr_sint_t SearchResultAddFileScrapCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
        sState |= StateEnable;

    return sState;
}

void SearchResultAddFileScrapCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->clearList();
    }
}

xpr_sint_t SearchResultCloseCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
        sState |= StateEnable;

    return sState;
}

void SearchResultCloseCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->clearList();

        gFrame->ShowControlBar(&gFrame->mWorkingBar, XPR_FALSE, XPR_FALSE);
    }
}
} // namespace cmd
