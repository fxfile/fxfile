//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_FILE_SCRAP_H__
#define __FXFILE_CMD_FILE_SCRAP_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(FileScrapAddCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapCurCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapCurMoveCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapCopyToCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapMoveToCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapDeleteCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapValidateCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapRemoveListCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapRemoveAllListCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapRefreshCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapViewCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapGroupAddCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapGroupRenameCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapGroupDeleteCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapGroupDefaultCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapClipboardCutCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapClipboardCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileScrapDropCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_FILE_SCRAP_H__
