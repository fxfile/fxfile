//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_HISTORY_H__
#define __FX_CMD_HISTORY_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(GoDirectBackwardCommand);
XPR_COMMAND_DECLARE_CLASS(GoDirectForwardCommand);
XPR_COMMAND_DECLARE_CLASS(GoDirectHistoryCommand);
XPR_COMMAND_DECLARE_CLASS(GoBackwardBoxCommand);
XPR_COMMAND_DECLARE_CLASS(GoForwardBoxCommand);
XPR_COMMAND_DECLARE_CLASS(GoHistoryBoxCommand);
XPR_COMMAND_DECLARE_CLASS(BackwardClearCommand);
XPR_COMMAND_DECLARE_CLASS(ForwardClearCommand);
XPR_COMMAND_DECLARE_CLASS(HistoryClearCommand);
XPR_COMMAND_DECLARE_CLASS(HistoryAllClearCommand);
XPR_COMMAND_DECLARE_CLASS(GoBackwardCommand);
XPR_COMMAND_DECLARE_CLASS(GoForwardCommand);
XPR_COMMAND_DECLARE_CLASS(GoHistoryCommand);
} // namespace cmd

#endif // __FX_CMD_HISTORY_H__
