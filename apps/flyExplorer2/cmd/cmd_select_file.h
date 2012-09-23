//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_SELECT_FILE_H__
#define __FX_CMD_SELECT_FILE_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(SelectNameCommand);
XPR_COMMAND_DECLARE_CLASS(UnselectNameCommand);
XPR_COMMAND_DECLARE_CLASS(SelectSameFilterCommand);
XPR_COMMAND_DECLARE_CLASS(SelectSameExtCommand);

XPR_COMMAND_DECLARE_CLASS(SelectFileCommand);
XPR_COMMAND_DECLARE_CLASS(SelectFolderCommand);
XPR_COMMAND_DECLARE_CLASS(UnselectFileCommand);
XPR_COMMAND_DECLARE_CLASS(UnselectFolderCommand);
XPR_COMMAND_DECLARE_CLASS(SelectFilterCommand);
XPR_COMMAND_DECLARE_CLASS(UnselectFilterCommand);

XPR_COMMAND_DECLARE_CLASS(SelectAllCommand);
XPR_COMMAND_DECLARE_CLASS(UnselectAllCommand);
XPR_COMMAND_DECLARE_CLASS(SelectReversalCommand);
} // namespace cmd

#endif // __FX_CMD_SELECT_FILE_H__
