//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_SELECT_FILE_H__
#define __FXFILE_CMD_SELECT_FILE_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(SelectNameCommand);
FXFILE_COMMAND_DECLARE_CLASS(UnselectNameCommand);
FXFILE_COMMAND_DECLARE_CLASS(SelectSameFilterCommand);
FXFILE_COMMAND_DECLARE_CLASS(SelectSameExtCommand);

FXFILE_COMMAND_DECLARE_CLASS(SelectFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(SelectFolderCommand);
FXFILE_COMMAND_DECLARE_CLASS(UnselectFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(UnselectFolderCommand);
FXFILE_COMMAND_DECLARE_CLASS(SelectFilterCommand);
FXFILE_COMMAND_DECLARE_CLASS(UnselectFilterCommand);

FXFILE_COMMAND_DECLARE_CLASS(SelectAllCommand);
FXFILE_COMMAND_DECLARE_CLASS(UnselectAllCommand);
FXFILE_COMMAND_DECLARE_CLASS(SelectReversalCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_SELECT_FILE_H__
