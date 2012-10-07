//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_SEARCH_RESULT_H__
#define __FX_CMD_SEARCH_RESULT_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(SearchResultViewIconCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultViewSmallIconCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultViewListCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultViewDetailCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultRemoveListCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultAddFileScrapCommand);
XPR_COMMAND_DECLARE_CLASS(SearchResultCloseCommand);
} // namespace cmd

#endif // __FX_CMD_SEARCH_RESULT_H__
