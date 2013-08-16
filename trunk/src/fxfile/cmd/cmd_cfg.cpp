//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_cfg.h"

#include "main_frame.h"
#include "accel_table_dlg.h"
#include "cfg/cfg_main_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t SaveOptionCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SaveOptionCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    gApp.saveAllOptions();
}

xpr_sint_t AcceleratorCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void AcceleratorCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    HACCEL sAccelTable = sMainFrame->getAccelTable();

    AccelTableDlg sDlg(sAccelTable, IDR_MAINFRAME);
    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId == IDOK)
    {
        sMainFrame->setAccelerator(sDlg.mAccel, sDlg.mCount);
    }
}

xpr_sint_t OptionCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void OptionCommand::execute(CommandContext &aContext)
{
    cfg::CfgMainDlg sDlg;
    sDlg.DoModal();
}
} // namespace cmd
} // namespace fxfile
