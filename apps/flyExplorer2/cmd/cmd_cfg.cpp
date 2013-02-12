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

#include "MainFrame.h"
#include "AccelTableDlg.h"
#include "cfg/CfgMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t SaveOptionCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SaveOptionCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    theApp.saveAllOptions();
}

xpr_sint_t AcceleratorCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void AcceleratorCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

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
    CfgMgrDlg sDlg;
    sDlg.DoModal();
}
} // namespace cmd
