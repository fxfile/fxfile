//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_TOOLS_H__
#define __FX_CMD_TOOLS_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(FileSearchCommand);
XPR_COMMAND_DECLARE_CLASS(EmptyRecycleBinCommand);
XPR_COMMAND_DECLARE_CLASS(WindowsExplorerCommand);
XPR_COMMAND_DECLARE_CLASS(CmdCommand);
XPR_COMMAND_DECLARE_CLASS(DosCmdCommand);
XPR_COMMAND_DECLARE_CLASS(SharedProcCommand);
} // namespace cmd

#endif // __FX_CMD_TOOLS_H__
