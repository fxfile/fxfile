//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_FILE_OPER_H__
#define __FX_CMD_FILE_OPER_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(FileDeleteCommand);
XPR_COMMAND_DECLARE_CLASS(FileDuplicateCommand);
XPR_COMMAND_DECLARE_CLASS(FileLinkCommand);
XPR_COMMAND_DECLARE_CLASS(FolderCopyCommand);
XPR_COMMAND_DECLARE_CLASS(FolderMoveCommand);
} // namespace cmd

#endif // __FX_CMD_FILE_OPER_H__
