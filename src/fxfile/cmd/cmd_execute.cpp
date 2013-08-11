//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_execute.h"

#include "router/cmd_context.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t ExecuteCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ExecuteCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->executeAllSelItems();
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sExplorerCtrl->executeAllSelItems();
    }
}

xpr_sint_t ParamExecuteCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ParamExecuteCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->execute(-1);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sExplorerCtrl->execute(-1, XPR_TRUE);
    }
}
} // namespace cmd
} // namespace fxfile
