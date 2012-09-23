//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_tray.h"

#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t TrayShowMainFrameCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void TrayShowMainFrameCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->showTrayMainFrame();
}

xpr_sint_t TrayAppExitCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void TrayAppExitCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->exitTrayApp();
}
} // namespace cmd
