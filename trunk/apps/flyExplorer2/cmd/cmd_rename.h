//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_RENAME_H__
#define __FX_CMD_RENAME_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(RenameCommand);
XPR_COMMAND_DECLARE_CLASS(BatchRenameCommand);
XPR_COMMAND_DECLARE_CLASS(BatchOnlyFileRenameCommand);
XPR_COMMAND_DECLARE_CLASS(BatchOnlyFolderRenameCommand);
} // namespace cmd

#endif // __FX_CMD_RENAME_H__
