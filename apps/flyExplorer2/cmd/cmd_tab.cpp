//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_tab.h"

#include "MainFrame.h"
#include "ExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t NewTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void NewTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->newTab();
        if (sTab >= 0)
            sExplorerView->setCurTab(sTab);
    }
}

xpr_sint_t DuplicateTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void DuplicateTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->duplicateTab();
    }
}

xpr_sint_t DuplicateTabFromCursorCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabFromCursor();
        if (sTab >= 0)
            sState = StateEnable;
    }

    return sState;
}

void DuplicateTabFromCursorCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->duplicateTab(XPR_TRUE);
    }
}

xpr_sint_t CloseTabCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTabCount = sExplorerView->getTabCount();
        if (sTabCount > 1)
            sState = StateEnable;
    }

    return sState;
}

void CloseTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->closeTab();
    }
}

xpr_sint_t CloseTabFromCursorCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabFromCursor();
        if (sTab >= 0)
            sState = StateEnable;
    }

    return sState;
}

void CloseTabFromCursorCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->closeTab(XPR_TRUE);
    }
}

xpr_sint_t CloseAllTabsButThisCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTabCount = sExplorerView->getTabCount();
        if (sTabCount > 1)
            sState = StateEnable;
    }

    return sState;
}

void CloseAllTabsButThisCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->closeAllTabsButThis();
    }
}

xpr_sint_t CloseAllTabsButThisFromCursorCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabFromCursor();
        if (sTab >= 0)
            sState = StateEnable;
    }

    return sState;
}

void CloseAllTabsButThisFromCursorCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->closeAllTabsButThis(XPR_TRUE);
    }
}

xpr_sint_t CloseAllTabsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void CloseAllTabsCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->closeAllTabs();
    }
}

xpr_sint_t FirstTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FirstTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->firstTab();
    }
}

xpr_sint_t PreviousTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void PreviousTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->previousTab();
    }
}

xpr_sint_t NextTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void NextTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->nextTab();
    }
}

xpr_sint_t LastTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void LastTabCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->lastTab();
    }
}
} // namespace cmd
