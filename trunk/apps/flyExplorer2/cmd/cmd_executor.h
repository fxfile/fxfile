//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_DISPATCHER_H__
#define __FX_CMD_DISPATCHER_H__
#pragma once

namespace cmd
{
class CommandContext;
class Command;

class CommandExecutor
{
public:
    CommandExecutor(void);
    virtual ~CommandExecutor(void);

public:
    virtual xpr_bool_t  isBindedCommand(xpr_uint_t aCommandId);
    virtual xpr_rcode_t bindCommand(xpr_uint_t aCommandId, Command *aCommand);
    virtual xpr_rcode_t bindCommand(xpr_uint_t aCommandBeginId, xpr_uint_t aCommandEndId, Command *aCommand);
    virtual xpr_sint_t  canExecute(CommandContext &aContext);
    virtual xpr_bool_t  execute(CommandContext &aContext);

protected:
    class CommandItem
    {
    public:
        CommandItem(void);
        ~CommandItem(void);

    public:
        xpr_uint_t  mCommandId;
        xpr_uint_t  mEndCommandId;
        Command    *mCommand;
    };

    typedef std::map<xpr_uint_t, CommandItem *> CommandTable;
    typedef std::list<CommandItem *> CommandRangeList;

    CommandTable     mCommandTable;
    CommandRangeList mCommandRangeList;
};
} // namespace cmd

#endif // __FX_CMD_DISPATCHER_H__
