//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_WORKING_FOLDER_H__
#define __FX_CMD_WORKING_FOLDER_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(WorkingFolderPopupCommand);
XPR_COMMAND_DECLARE_CLASS(GoWorkingFolderCommand);
XPR_COMMAND_DECLARE_CLASS(WorkingFolderSetCommand);
XPR_COMMAND_DECLARE_CLASS(WorkingFolderResetCommand);
XPR_COMMAND_DECLARE_CLASS(WorkingFolderResetAllCommand);
} // namespace cmd

#endif // __FX_CMD_WORKING_FOLDER_H__