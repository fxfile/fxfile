//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_drive.h"

#include "drive_dlg.h"
#include "drive_info_dlg.h"
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
xpr_sint_t DrivePopupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void DrivePopupCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    DriveDlg sDlg;
    if (sDlg.DoModal() == IDOK)
        sMainFrame->gotoDrive(sDlg.getDrive());
}

xpr_sint_t GoDriveCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void GoDriveCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_tchar_t sDriveChar = (sCommandId - ID_DRIVE_FIRST) + XPR_STRING_LITERAL('A');
    sMainFrame->gotoDrive(sDriveChar);
}

xpr_sint_t DriveInfoCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void DriveInfoCommand::execute(CommandContext &aContext)
{
    DriveInfoDlg sDlg;
    sDlg.DoModal();
}
} // namespace cmd
} // namespace fxfile
