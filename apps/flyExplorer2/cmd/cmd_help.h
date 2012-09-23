//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_HELP_H__
#define __FX_CMD_HELP_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(HelpCommand);
XPR_COMMAND_DECLARE_CLASS(HelpFindCommand);
XPR_COMMAND_DECLARE_CLASS(HelpAcceleratorCommand);
XPR_COMMAND_DECLARE_CLASS(TipOfTodayCommand);
XPR_COMMAND_DECLARE_CLASS(HompageCommand);
XPR_COMMAND_DECLARE_CLASS(EmailCommand);
XPR_COMMAND_DECLARE_CLASS(AboutCommand);
} // namespace cmd

#endif // __FX_CMD_HELP_H__
