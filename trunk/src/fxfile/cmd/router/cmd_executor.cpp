//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_executor.h"
#include "cmd_context.h"
#include "cmd_command.h"

#include "../../functors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
CommandExecutor::CommandItem::CommandItem(void)
    : mCommandId(0), mEndCommandId(0), mCommand(XPR_NULL)
{
}

CommandExecutor::CommandItem::~CommandItem(void)
{
    XPR_SAFE_DELETE(mCommand);
}

CommandExecutor::CommandExecutor(void)
{
}

CommandExecutor::~CommandExecutor(void)
{
    clearMap(mCommandTable);
    clear(mCommandRangeList);
}

xpr_bool_t CommandExecutor::isBindedCommand(xpr_uint_t aCommandId)
{
    return (mCommandTable.find(aCommandId) == mCommandTable.end()) ? XPR_FALSE : XPR_TRUE;
}

xpr_rcode_t CommandExecutor::bindCommand(xpr_uint_t aCommandId, Command *aCommand)
{
    if (aCommand == XPR_NULL)
        return 0;

    CommandTable::iterator sIterator = mCommandTable.find(aCommandId);
    if (sIterator != mCommandTable.end())
    {
        XPR_TRACE(XPR_STRING_LITERAL("[CommandExecutor] %d ID command is already exist!!!\n"), aCommandId);

        CommandItem *sCommandItem = sIterator->second;
        XPR_SAFE_DELETE(sCommandItem);

        mCommandTable.erase(sIterator);
    }

    CommandItem *sCommandItem = new CommandItem;
    sCommandItem->mCommandId    = aCommandId;
    sCommandItem->mEndCommandId = aCommandId;
    sCommandItem->mCommand      = aCommand;

    mCommandTable[sCommandItem->mCommandId] = sCommandItem;

    return 1;
}

xpr_rcode_t CommandExecutor::bindCommand(xpr_uint_t aCommandId, xpr_uint_t aEndCommandId, Command *aCommand)
{
    if (aCommand == XPR_NULL)
        return 0;

    CommandItem *sCommandItem = new CommandItem;
    sCommandItem->mCommandId    = aCommandId;
    sCommandItem->mEndCommandId = aEndCommandId;
    sCommandItem->mCommand      = aCommand;

    mCommandRangeList.push_back(sCommandItem);

    return 1;
}

xpr_sint_t CommandExecutor::canExecute(CommandContext &aContext)
{
    xpr_uint_t sCommandId = aContext.getCommandId();

    CommandItem *sCommandItem = XPR_NULL;

    CommandTable::iterator sIter = mCommandTable.find(sCommandId);
    if (sIter != mCommandTable.end())
    {
        sCommandItem = sIter->second;
    }
    else
    {
        CommandRangeList::iterator sRangeIter = mCommandRangeList.begin();
        for (; sRangeIter != mCommandRangeList.end(); ++sRangeIter)
        {
            CommandItem *sRangeCommand = *sRangeIter;

            if (XPR_IS_RANGE(sRangeCommand->mCommandId, sCommandId, sRangeCommand->mEndCommandId))
            {
                sCommandItem = sRangeCommand;
                break;
            }
        }
    }

    if (sCommandItem == XPR_NULL)
        return Command::StateUnbinded;

    return sCommandItem->mCommand->canExecute(aContext);
}

xpr_bool_t CommandExecutor::execute(CommandContext &aContext)
{
    xpr_uint_t sCommandId = aContext.getCommandId();

    CommandItem *sCommandItem = XPR_NULL;

    CommandTable::iterator sIter = mCommandTable.find(sCommandId);
    if (sIter != mCommandTable.end())
    {
        sCommandItem = sIter->second;
    }
    else
    {
        CommandRangeList::iterator sRangeIter = mCommandRangeList.begin();
        for (; sRangeIter != mCommandRangeList.end(); ++sRangeIter)
        {
            CommandItem *sRangeCommand = *sRangeIter;

            if (XPR_IS_RANGE(sRangeCommand->mCommandId, sCommandId, sRangeCommand->mEndCommandId))
            {
                sCommandItem = sRangeCommand;
                break;
            }
        }
    }

    if (sCommandItem == XPR_NULL)
        return XPR_FALSE;

    sCommandItem->mCommand->execute(aContext);

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
