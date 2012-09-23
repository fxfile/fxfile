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

#include "fxb/fxb_file_op_undo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t UndoCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = fxb::FileOpUndo::isUndo();

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void UndoCommand::execute(CommandContext &aContext)
{
    if (fxb::FileOpUndo::isUndo() == XPR_TRUE)
    {
        fxb::FileOpUndo sFileOpUndo;
        sFileOpUndo.start();
    }
}
} // namespace cmd
