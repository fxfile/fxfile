//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_network.h"

#include "wnet_mgr.h"

#include "main_frame.h"
#include "folder_ctrl.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t NetworkDriveConnectCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void NetworkDriveConnectCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    DWORD sResult;
    sResult = WnetMgr::instance().connectNetDrive(sMainFrame->GetSafeHwnd());
    if (sResult == ERROR_INVALID_PASSWORD)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.network_drive.msg.wrong_password"));
        sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
    else if (sResult == ERROR_NO_NETWORK)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.network_drive.msg.cannot_connect"));
        sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
    else if (sResult == ERROR_NOT_ENOUGH_MEMORY)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.network_drive.msg.insufficient_memory"));
        sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
}

xpr_sint_t NetworkDriveDisconnectCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void NetworkDriveDisconnectCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    WnetMgr::instance().disconnectNetDrive(sMainFrame->GetSafeHwnd());
}
} // namespace cmd
} // namespace fxfile
