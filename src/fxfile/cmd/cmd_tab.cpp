//
// Copyright (c) 2012-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_tab.h"

#include "router/cmd_parameters.h"
#include "router/cmd_parameter_define.h"
#include "main_frame.h"
#include "explorer_view.h"
#include "option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t NewTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void NewTabCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    LPITEMIDLIST sPidl = XPR_NULL;

    if (XPR_IS_NOT_NULL(sParameters))
    {
        sPidl = (LPITEMIDLIST)sParameters->get(CommandParameterIdPidl);
    }

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab;

        if (XPR_IS_NOT_NULL(sPidl))
        {
            sTab = sExplorerView->newTab(sPidl);
        }
        else
        {
            sTab = sExplorerView->newTab();
        }

        if (sTab >= 0)
        {
            if (sExplorerView->getTabCount() == 1)
            {
                sExplorerView->setCurTab(sTab);
            }
            else
            {
                if (XPR_IS_TRUE(gOpt->mConfig.mTabSwitchToNewTab))
                {
                    sExplorerView->setCurTab(sTab);
                }
            }
        }
    }
}

xpr_sint_t DuplicateTabCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        if (sExplorerView->canDuplicateTab() == XPR_TRUE)
            sState = StateEnable;
    }

    return sState;
}

void DuplicateTabCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->duplicateTab();
    }
}

xpr_sint_t DuplicateTabOnCursorCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();
        if (sTab >= 0)
        {
            if (sExplorerView->canDuplicateTab(sTab) == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void DuplicateTabOnCursorCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();

        sExplorerView->duplicateTab(sTab);
    }
}

xpr_sint_t CloseTabCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    return StateEnable;
}

namespace
{
void closeTab(MainFrame *aMainFrame, ExplorerView *aExplorerView, xpr_sint_t aTab)
{
    XPR_ASSERT(aMainFrame != XPR_NULL);
    XPR_ASSERT(aExplorerView != XPR_NULL);

    xpr_sint_t sTabCount = aExplorerView->getTabCount();
    xpr_bool_t sExplorerTabMode = aExplorerView->isExplorerTabMode();

    if (sTabCount >= 1)
    {
        if ((sTabCount > 1) || (sTabCount == 1 && sExplorerTabMode == XPR_FALSE))
        {
            xpr_bool_t sConfirmToClose = XPR_FALSE;

            if (XPR_IS_TRUE(gOpt->mConfig.mTabConfirmToClose))
            {
                xpr::string sTabText;
                aExplorerView->getTabText(aTab, sTabText);

                xpr::string sMsg(gApp.loadString(XPR_STRING_LITERAL("tab.msg.confirm_to_close")));
                sMsg += XPR_STRING_LITERAL("\r\n\r\n");
                sMsg += sTabText;

                xpr_sint_t sMsgId = aMainFrame->MessageBox(sMsg.c_str(), XPR_NULL, MB_YESNO | MB_ICONQUESTION);
                if (sMsgId == IDYES)
                {
                    sConfirmToClose = XPR_TRUE;
                }
            }
            else
            {
                sConfirmToClose = XPR_TRUE;
            }

            if (XPR_IS_TRUE(sConfirmToClose))
            {
                aExplorerView->SetRedraw(XPR_FALSE);

                aExplorerView->closeTab(aTab);

                if (sTabCount == 1 && sExplorerTabMode == XPR_FALSE)
                {
                    aExplorerView->newTab();
                    aExplorerView->setCurTab(0);
                }

                aExplorerView->SetRedraw(XPR_TRUE);
                aExplorerView->RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
            }
        }
    }
}

void closeOthers(MainFrame *aMainFrame, ExplorerView *aExplorerView, xpr_sint_t aTab)
{
    XPR_ASSERT(aMainFrame != XPR_NULL);
    XPR_ASSERT(aExplorerView != XPR_NULL);

    xpr_bool_t sConfirmToClose = XPR_FALSE;

    if (XPR_IS_TRUE(gOpt->mConfig.mTabConfirmToClose))
    {
        xpr::string sTabText;
        aExplorerView->getTabText(aTab, sTabText);

        xpr::string sMsg(gApp.loadString(XPR_STRING_LITERAL("tab.msg.confirm_to_close_others")));
        sMsg += XPR_STRING_LITERAL("\r\n\r\n");
        sMsg += sTabText;

        xpr_sint_t sMsgId = aMainFrame->MessageBox(sMsg.c_str(), XPR_NULL, MB_YESNO | MB_ICONQUESTION);
        if (sMsgId == IDYES)
        {
            sConfirmToClose = XPR_TRUE;
        }
    }
    else
    {
        sConfirmToClose = XPR_TRUE;
    }

    if (XPR_IS_TRUE(sConfirmToClose))
    {
        aExplorerView->closeOthers();
    }
}
} // anonymous namespace

void CloseTabCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTabCount = sExplorerView->getTabCount();

        xpr_sint_t sTab = -1;
        if (sParameters != XPR_NULL)
        {
            sTab = (xpr_sint_t)(xpr_sintptr_t)sParameters->get(CommandParameterIdTab);
            if (XPR_IS_OUT_OF_RANGE(0, sTab, sTabCount - 1))
            {
                return;
            }
        }

        closeTab(sMainFrame, sExplorerView, sTab);
    }
}

xpr_sint_t CloseTabOnCursorCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();
        if (sTab >= 0)
            sState = StateEnable;
    }

    return sState;
}

void CloseTabOnCursorCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();

        closeTab(sMainFrame, sExplorerView, sTab);
    }
}

xpr_sint_t CloseOtherTabsCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

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

void CloseOtherTabsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        closeOthers(sMainFrame, sExplorerView, -1);
    }
}

xpr_sint_t CloseOtherTabsOnCursorCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();
        if (sTab >= 0)
            sState = StateEnable;
    }

    return sState;
}

void CloseOtherTabsOnCursorCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->getTabOnCursor();

        closeOthers(sMainFrame, sExplorerView, sTab);
    }
}

xpr_sint_t CloseAllTabsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void CloseAllTabsCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_bool_t sConfirmToCloseAll = XPR_FALSE;

        if (XPR_IS_TRUE(gOpt->mConfig.mTabConfirmToCloseAll))
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("tab.msg.confirm_to_close_all"));
            xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDYES)
            {
                sConfirmToCloseAll = XPR_TRUE;
            }
        }
        else
        {
            sConfirmToCloseAll = XPR_TRUE;
        }

        if (XPR_IS_TRUE(sConfirmToCloseAll))
        {
            sExplorerView->closeAllTabs();
        }
    }
}

xpr_sint_t FirstTabCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FirstTabCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

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
    FXFILE_COMMAND_DECLARE_CTRL;

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
    FXFILE_COMMAND_DECLARE_CTRL;

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
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->lastTab();
    }
}
} // namespace cmd
} // namespace fxfile
