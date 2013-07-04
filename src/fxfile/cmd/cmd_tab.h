//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_TAB_H__
#define __FXFILE_CMD_TAB_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(NewTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(DuplicateTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(DuplicateTabOnCursorCommand);
FXFILE_COMMAND_DECLARE_CLASS(CloseTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(CloseTabOnCursorCommand);
FXFILE_COMMAND_DECLARE_CLASS(CloseOtherTabsCommand);
FXFILE_COMMAND_DECLARE_CLASS(CloseOtherTabsOnCursorCommand);
FXFILE_COMMAND_DECLARE_CLASS(CloseAllTabsCommand);
FXFILE_COMMAND_DECLARE_CLASS(FirstTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(PreviousTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(NextTabCommand);
FXFILE_COMMAND_DECLARE_CLASS(LastTabCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_TAB_H__
