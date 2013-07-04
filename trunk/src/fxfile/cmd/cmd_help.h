//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_HELP_H__
#define __FXFILE_CMD_HELP_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(HelpCommand);
FXFILE_COMMAND_DECLARE_CLASS(HelpFindCommand);
FXFILE_COMMAND_DECLARE_CLASS(HelpAcceleratorCommand);
FXFILE_COMMAND_DECLARE_CLASS(TipOfTodayCommand);
FXFILE_COMMAND_DECLARE_CLASS(HompageCommand);
FXFILE_COMMAND_DECLARE_CLASS(EmailCommand);
FXFILE_COMMAND_DECLARE_CLASS(AboutCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_HELP_H__
