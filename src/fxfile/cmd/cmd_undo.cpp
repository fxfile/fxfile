//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_undo.h"

#include "file_op_undo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t UndoCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = FileOpUndo::isUndo();

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void UndoCommand::execute(CommandContext &aContext)
{
    if (FileOpUndo::isUndo() == XPR_TRUE)
    {
        FileOpUndo sFileOpUndo;
        sFileOpUndo.start();
    }
}
} // namespace cmd
} // namespace fxfile
