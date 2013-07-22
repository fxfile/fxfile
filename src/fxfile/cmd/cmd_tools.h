//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_TOOLS_H__
#define __FXFILE_CMD_TOOLS_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(FileSearchCommand);
FXFILE_COMMAND_DECLARE_CLASS(EmptyRecycleBinCommand);
FXFILE_COMMAND_DECLARE_CLASS(WindowsExplorerCommand);
FXFILE_COMMAND_DECLARE_CLASS(CmdCommand);
FXFILE_COMMAND_DECLARE_CLASS(DosCmdCommand);
FXFILE_COMMAND_DECLARE_CLASS(SharedProcCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_TOOLS_H__
