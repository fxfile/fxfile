//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_app.h"

#include "main_frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t MinimizeTrayCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void MinimizeTrayCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->minimizeToTray();
}

xpr_sint_t AppExitCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void AppExitCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->SendMessage(WM_CLOSE);
}
} // namespace cmd
} // namespace fxfile
