//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_style.h"

#include "resource.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t ViewStyleCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sExplorerCtrl != XPR_NULL)
    {
        DWORD sViewStyle = sExplorerCtrl->getViewStyle();

        if (sCommandId == ID_VIEW_LINEUP)
        {
            if (sViewStyle == LVS_ICON || sViewStyle == LVS_SMALLICON)
                sState |= StateEnable;
        }
        else
        {
            sState |= StateEnable;

            xpr_bool_t sChecked = XPR_FALSE;
            switch (sCommandId)
            {
            case ID_VIEW_STYLE_DETAILS:    sChecked = (sViewStyle == LVS_REPORT);    break;
            case ID_VIEW_STYLE_SMALLICONS: sChecked = (sViewStyle == LVS_SMALLICON); break;
            case ID_VIEW_STYLE_LARGEICONS: sChecked = (sViewStyle == LVS_ICON);      break;
            case ID_VIEW_STYLE_LIST:       sChecked = (sViewStyle == LVS_LIST);      break;
            case ID_VIEW_STYLE_THUMBNAIL:  sChecked = (sViewStyle == LVS_THUMBNAIL); break;
            default:                       sChecked = XPR_FALSE;                     break;
            }

            if (XPR_IS_TRUE(sChecked))
                sState |= StateRadio;
        }
    }

    return sState;
}

void ViewStyleCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        DWORD sViewStyle = -1;

        switch (sCommandId)
        {
        case ID_VIEW_LINEUP:           sExplorerCtrl->Arrange(LVA_SNAPTOGRID); break;
        case ID_VIEW_STYLE_DETAILS:    sViewStyle = LVS_REPORT;                break;
        case ID_VIEW_STYLE_SMALLICONS: sViewStyle = LVS_SMALLICON;             break;
        case ID_VIEW_STYLE_LARGEICONS: sViewStyle = LVS_ICON;                  break;
        case ID_VIEW_STYLE_LIST:       sViewStyle = LVS_LIST;                  break;
        case ID_VIEW_STYLE_THUMBNAIL:  sViewStyle = LVS_THUMBNAIL;             break;
        }

        if (sViewStyle != -1)
        {
            xpr_bool_t sRefesh = (sViewStyle == LVS_THUMBNAIL) ? XPR_TRUE : XPR_FALSE;
            sExplorerCtrl->setViewStyle(sViewStyle, sRefesh);
        }
    }
}

xpr_sint_t ViewStyleToolBarCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void ViewStyleToolBarCommand::execute(CommandContext &aContext)
{
}
} // namespace cmd
} // namespace fxfile
