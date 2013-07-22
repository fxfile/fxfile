//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_CREATE_ITEM_H__
#define __FXFILE_CMD_CREATE_ITEM_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(CreateFolderCommand);
FXFILE_COMMAND_DECLARE_CLASS(CreateGeneralFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(CreateTextFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(CreateShortcutCommand);
FXFILE_COMMAND_DECLARE_CLASS(CreateBatchCommand);
FXFILE_COMMAND_DECLARE_CLASS(FileNewCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_CREATE_ITEM_H__
