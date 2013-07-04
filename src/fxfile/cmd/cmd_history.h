//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_HISTORY_H__
#define __FXFILE_CMD_HISTORY_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(GoDirectBackwardCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoDirectForwardCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoDirectHistoryCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoBackwardBoxCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoForwardBoxCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoHistoryBoxCommand);
FXFILE_COMMAND_DECLARE_CLASS(BackwardClearCommand);
FXFILE_COMMAND_DECLARE_CLASS(ForwardClearCommand);
FXFILE_COMMAND_DECLARE_CLASS(HistoryClearCommand);
FXFILE_COMMAND_DECLARE_CLASS(HistoryAllClearCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoBackwardCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoForwardCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoHistoryCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_HISTORY_H__
