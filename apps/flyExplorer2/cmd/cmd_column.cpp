//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_column.h"

#include "fxb/fxb_shell_column.h"
#include "ColumnSetDlg.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t ColumnCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
        {
            if (sCommandId != ID_VIEW_COLUMN_NAME)
                sState |= StateEnable;

            ColumnId sColumnId = {0};
            switch (sCommandId)
            {
            case ID_VIEW_COLUMN_NAME: sColumnId.mPropertyId = 0; break;
            case ID_VIEW_COLUMN_SIZE: sColumnId.mPropertyId = 1; break;
            case ID_VIEW_COLUMN_TYPE: sColumnId.mPropertyId = 2; break;
            case ID_VIEW_COLUMN_DATE: sColumnId.mPropertyId = 3; break;
            case ID_VIEW_COLUMN_ATTR: sColumnId.mPropertyId = 4; break;
            case ID_VIEW_COLUMN_EXT:  sColumnId.mPropertyId = 5; break;
            }

            if (sExplorerCtrl->isUseColumn(&sColumnId))
                sState |= StateCheck;
        }
    }

    return sState;
}

void ColumnCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        ColumnId sColumnId = {0};
        switch (sCommandId)
        {
        case ID_VIEW_COLUMN_NAME: sColumnId.mPropertyId = 0; break;
        case ID_VIEW_COLUMN_SIZE: sColumnId.mPropertyId = 1; break;
        case ID_VIEW_COLUMN_TYPE: sColumnId.mPropertyId = 2; break;
        case ID_VIEW_COLUMN_DATE: sColumnId.mPropertyId = 3; break;
        case ID_VIEW_COLUMN_ATTR: sColumnId.mPropertyId = 4; break;
        case ID_VIEW_COLUMN_EXT:  sColumnId.mPropertyId = 5; break;
        }

        sExplorerCtrl->useColumn(&sColumnId);
    }
}

xpr_sint_t ColumnSetCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void ColumnSetCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        LPSHELLFOLDER2 sShellFolder2 = XPR_NULL;
        ColumnDataList sColumnList;
        sExplorerCtrl->getColumnDataList(sShellFolder2, sColumnList);

        xpr_sint_t cxAvgChar = fxb::ShellColumn::instance().getAvgCharWidth(sExplorerCtrl);

        ColumnSetDlg sDlg;
        sDlg.setShellFolder2(sShellFolder2);
        sDlg.setAvgCharWidth(cxAvgChar);
        sDlg.setColumnList(sColumnList);
        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            ColumnDataList sUseColumnList;
            sDlg.getUseColumnList(sUseColumnList);

            xpr_sint_t i;
            xpr_sint_t sViewCount;
            ExplorerCtrl *sAllExplorerCtrl;

            sViewCount = sMainFrame->getViewCount();

            for (i = 0; i < sViewCount; ++i)
            {
                sAllExplorerCtrl = sMainFrame->getExplorerCtrl(i);
                if (XPR_IS_NOT_NULL(sAllExplorerCtrl))
                {
                    sAllExplorerCtrl->setColumnDataList(sUseColumnList);
                }
            }

            fxb::clear(sUseColumnList);
        }

        COM_RELEASE(sShellFolder2);
    }
}
} // namespace cmd
