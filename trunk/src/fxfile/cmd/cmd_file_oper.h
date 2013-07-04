//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_FILE_OPER_H__
#define __FXFILE_CMD_FILE_OPER_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(FileDeleteCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileDuplicateCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileLinkCommand);
FXFILE_COMMAND_DECLARE_CLASS(FolderCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(FolderMoveCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_FILE_OPER_H__
