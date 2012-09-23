//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_history.h"

#include "HistoryDlg.h"
#include "ExplorerCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t GoDirectBackwardCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getBackwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoDirectBackwardCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sExplorerCtrl->goBackward();
}

xpr_sint_t GoDirectForwardCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getForwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoDirectForwardCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sExplorerCtrl->goForward();
}

xpr_sint_t GoDirectHistoryCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getHistoryCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoDirectHistoryCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sExplorerCtrl->goHistory();
}

static void goHistoryBox(ExplorerCtrl *sExplorerCtrl, xpr_sint_t aType)
{
    if (sExplorerCtrl == XPR_NULL)
        return;

    HistoryDlg sDlg(aType);
    sDlg.addHistory(theApp.loadString(XPR_STRING_LITERAL("popup.history.title.backward")), theApp.loadString(XPR_STRING_LITERAL("popup.history.tab.backward")), sExplorerCtrl->getBackwardList());
    sDlg.addHistory(theApp.loadString(XPR_STRING_LITERAL("popup.history.title.forward")),  theApp.loadString(XPR_STRING_LITERAL("popup.history.tab.forward")),  sExplorerCtrl->getForwardList());
    sDlg.addHistory(theApp.loadString(XPR_STRING_LITERAL("popup.history.title.history")),  theApp.loadString(XPR_STRING_LITERAL("popup.history.tab.history")),  sExplorerCtrl->getHistoryList());
    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId != IDOK)
        return;

    xpr_size_t sHistory;
    xpr_uint_t sIndex;
    sDlg.getSelHistory(sHistory, sIndex);

    switch (sHistory)
    {
    case 0: sExplorerCtrl->goBackward(sIndex); break;
    case 1: sExplorerCtrl->goForward(sIndex);  break;
    case 2: sExplorerCtrl->goHistory(sIndex);  break;
    }
}

xpr_sint_t GoBackwardBoxCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getBackwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoBackwardBoxCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goHistoryBox(sExplorerCtrl, 0);
}

xpr_sint_t GoForwardBoxCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getForwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoForwardBoxCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goHistoryBox(sExplorerCtrl, 1);
}

xpr_sint_t GoHistoryBoxCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getHistoryCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoHistoryBoxCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    goHistoryBox(sExplorerCtrl, 2);
}

xpr_sint_t BackwardClearCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BackwardClearCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->clearBackward();
    }
}

xpr_sint_t ForwardClearCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void ForwardClearCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->clearForward();
    }
}

xpr_sint_t HistoryClearCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HistoryClearCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->clearHistory();
    }
}

xpr_sint_t HistoryAllClearCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HistoryAllClearCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->clearAllHistory();
    }
}

xpr_sint_t GoBackwardCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getBackwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoBackwardCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        xpr_sint_t sBackward = sCommandId - ID_GO_BACKWARD_FIRST;
        sExplorerCtrl->goBackward(sBackward);
    }
}

xpr_sint_t GoForwardCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getForwardCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoForwardCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        xpr_sint_t sForward = sCommandId - ID_GO_FORWARD_FIRST;
        sExplorerCtrl->goForward(sForward);
    }
}

xpr_sint_t GoHistoryCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
        sEnable = (sExplorerCtrl->getHistoryCount() > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void GoHistoryCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        xpr_sint_t sHistory = sCommandId - ID_GO_HISTORY_FIRST;
        sExplorerCtrl->goHistory(sHistory);
    }
}
} // namespace cmd
