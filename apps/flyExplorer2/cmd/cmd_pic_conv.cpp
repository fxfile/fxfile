//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_pic_conv.h"

#include "PicCnvDlg.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t PicConvCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_EXPLORER_CTRL
    {
        sEnable = sExplorerCtrl->isSelected();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void PicConvCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_EXPLORER_CTRL
    {
        if (sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        PicCnvDlg sDlg;

        xpr_sint_t sIndex;
        std::tstring sPath;
        LPLVITEMDATA sLvItemData;

        POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

            sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if (XPR_IS_NULL(sLvItemData))
                continue;

            if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                continue;

            if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
            {
                fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                sDlg.addPath(sPath);
            }
        }

        sDlg.DoModal();
    }
}
} // namespace cmd
