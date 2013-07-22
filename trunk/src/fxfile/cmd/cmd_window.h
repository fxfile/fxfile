//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_WINDOW_H__
#define __FXFILE_CMD_WINDOW_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(WindowNextCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowPrevCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplitToolBarCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplitSingleCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplit1x2Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplit2x1Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplit2x2Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplit1x3Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplit2x3Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplitEquallyCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowSplitRatioCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowSwapFolderCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowEqualFolderCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowOtherEqualFolderCommand);

FXFILE_COMMAND_DECLARE_CLASS(WindowCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowMoveCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowCopy1Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowMove1Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowCopy2Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowMove2Command);
FXFILE_COMMAND_DECLARE_CLASS(WindowDualCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowDualMoveCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowDualDeleteCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowCompareCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_WINDOW_H__
