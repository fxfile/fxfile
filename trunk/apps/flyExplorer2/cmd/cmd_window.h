//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_WINDOW_H__
#define __FX_CMD_WINDOW_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(WindowNextCommand);
XPR_COMMAND_DECLARE_CLASS(WindowPrevCommand);
XPR_COMMAND_DECLARE_CLASS(WindowSplitToolBarCommand);
XPR_COMMAND_DECLARE_CLASS(WindowSplitSingleCommand);
XPR_COMMAND_DECLARE_CLASS(WindowSplit1x2Command);
XPR_COMMAND_DECLARE_CLASS(WindowSplit2x1Command);
XPR_COMMAND_DECLARE_CLASS(WindowSplit2x2Command);
XPR_COMMAND_DECLARE_CLASS(WindowSplit1x3Command);
XPR_COMMAND_DECLARE_CLASS(WindowSplit2x3Command);
XPR_COMMAND_DECLARE_CLASS(WindowSplitEquallyCommand);
XPR_COMMAND_DECLARE_CLASS(WindowSplitRatioCommand);
XPR_COMMAND_DECLARE_CLASS(WindowSwapFolderCommand);
XPR_COMMAND_DECLARE_CLASS(WindowEqualFolderCommand);
XPR_COMMAND_DECLARE_CLASS(WindowOtherEqualFolderCommand);

XPR_COMMAND_DECLARE_CLASS(WindowCopyCommand);
XPR_COMMAND_DECLARE_CLASS(WindowMoveCommand);
XPR_COMMAND_DECLARE_CLASS(WindowCopy1Command);
XPR_COMMAND_DECLARE_CLASS(WindowMove1Command);
XPR_COMMAND_DECLARE_CLASS(WindowCopy2Command);
XPR_COMMAND_DECLARE_CLASS(WindowMove2Command);
XPR_COMMAND_DECLARE_CLASS(WindowDualCopyCommand);
XPR_COMMAND_DECLARE_CLASS(WindowDualMoveCommand);
XPR_COMMAND_DECLARE_CLASS(WindowDualDeleteCommand);
XPR_COMMAND_DECLARE_CLASS(WindowCompareCommand);

} // namespace cmd

#endif // __FX_CMD_WINDOW_H__
