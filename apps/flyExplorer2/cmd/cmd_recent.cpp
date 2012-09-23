//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_recent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t RecentFileCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void RecentFileCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    theApp.getRecentFile(sCommandId, sPath);

    fxb::ExecFile(sPath);
}
} // namespace cmd
