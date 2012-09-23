//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_CREATE_ITEM_H__
#define __FX_CMD_CREATE_ITEM_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(CreateFolderCommand);
XPR_COMMAND_DECLARE_CLASS(CreateGeneralFileCommand);
XPR_COMMAND_DECLARE_CLASS(CreateTextFileCommand);
XPR_COMMAND_DECLARE_CLASS(CreateShortcutCommand);
XPR_COMMAND_DECLARE_CLASS(CreateBatchCommand);
XPR_COMMAND_DECLARE_CLASS(FileNewCommand);
} // namespace cmd

#endif // __FX_CMD_CREATE_ITEM_H__
