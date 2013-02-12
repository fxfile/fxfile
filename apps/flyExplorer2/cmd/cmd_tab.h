//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_TAB_H__
#define __FX_CMD_TAB_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(NewTabCommand);
XPR_COMMAND_DECLARE_CLASS(DuplicateTabCommand);
XPR_COMMAND_DECLARE_CLASS(DuplicateTabOnCursorCommand);
XPR_COMMAND_DECLARE_CLASS(CloseTabCommand);
XPR_COMMAND_DECLARE_CLASS(CloseTabOnCursorCommand);
XPR_COMMAND_DECLARE_CLASS(CloseOtherTabsCommand);
XPR_COMMAND_DECLARE_CLASS(CloseOtherTabsOnCursorCommand);
XPR_COMMAND_DECLARE_CLASS(CloseAllTabsCommand);
XPR_COMMAND_DECLARE_CLASS(FirstTabCommand);
XPR_COMMAND_DECLARE_CLASS(PreviousTabCommand);
XPR_COMMAND_DECLARE_CLASS(NextTabCommand);
XPR_COMMAND_DECLARE_CLASS(LastTabCommand);
} // namespace cmd

#endif // __FX_CMD_TAB_H__
