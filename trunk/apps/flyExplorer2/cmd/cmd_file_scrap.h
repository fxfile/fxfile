//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_FILE_SCRAP_H__
#define __FX_CMD_FILE_SCRAP_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(FileScrapAddCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapCurCopyCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapCurMoveCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapCopyToCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapMoveToCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapDeleteCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapValidateCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapRemoveListCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapRemoveAllListCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapRefreshCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapViewCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapGroupAddCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapGroupRenameCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapGroupDeleteCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapGroupDefaultCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapClipboardCutCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapClipboardCopyCommand);
XPR_COMMAND_DECLARE_CLASS(FileScrapDropCommand);
} // namespace cmd

#endif // __FX_CMD_FILE_SCRAP_H__
