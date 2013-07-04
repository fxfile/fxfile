//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_print.h"

#include "main_frame.h"
#include "explorer_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t PrintCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void PrintCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->print();
    }
}

xpr_sint_t PrintPreviewCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void PrintPreviewCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->printPreview();
    }
}

xpr_sint_t PrintSetupCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void PrintSetupCommand::execute(CommandContext &aContext)
{
    CPrintDialog sDlg(XPR_TRUE);
    AfxGetApp()->DoPrintDialog(&sDlg);
}
} // namespace cmd
} // namespace fxfile
