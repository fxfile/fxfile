//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_COMMAND_MAP_H__
#define __FX_CMD_COMMAND_MAP_H__
#pragma once

namespace cmd
{
class CommandExecutor;

class CommandMap
{
public:
    static void map(CommandExecutor &aExecutor);
};
} // namespace cmd

#endif // __FX_CMD_COMMAND_MAP_H__
