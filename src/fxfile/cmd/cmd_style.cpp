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
#include "option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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
        xpr_bool_t sChecked = XPR_FALSE;
        xpr_sint_t sViewStyle = sExplorerCtrl->getViewStyle();

        sState |= StateEnable;

        switch (sCommandId)
        {
        case ID_VIEW_STYLE_EXTRA_LARGE_ICONS: sChecked = (sViewStyle == VIEW_STYLE_EXTRA_LARGE_ICONS); break;
        case ID_VIEW_STYLE_LARGE_ICONS:       sChecked = (sViewStyle == VIEW_STYLE_LARGE_ICONS);       break;
        case ID_VIEW_STYLE_MEDIUM_ICONS:      sChecked = (sViewStyle == VIEW_STYLE_MEDIUM_ICONS);      break;
        case ID_VIEW_STYLE_SMALL_ICONS:       sChecked = (sViewStyle == VIEW_STYLE_SMALL_ICONS);       break;
        case ID_VIEW_STYLE_LIST:              sChecked = (sViewStyle == VIEW_STYLE_LIST);              break;
        case ID_VIEW_STYLE_DETAILS:           sChecked = (sViewStyle == VIEW_STYLE_DETAILS);           break;
        case ID_VIEW_STYLE_THUMBNAIL:         sChecked = (sViewStyle == VIEW_STYLE_THUMBNAIL);         break;
        default:                              sChecked = XPR_FALSE;                                    break;
        }

        if (XPR_IS_TRUE(sChecked))
        {
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
        xpr_sint_t sViewStyle = -1;

        switch (sCommandId)
        {
        case ID_VIEW_STYLE_EXTRA_LARGE_ICONS: sViewStyle = VIEW_STYLE_EXTRA_LARGE_ICONS; break;
        case ID_VIEW_STYLE_LARGE_ICONS:       sViewStyle = VIEW_STYLE_LARGE_ICONS;       break;
        case ID_VIEW_STYLE_MEDIUM_ICONS:      sViewStyle = VIEW_STYLE_MEDIUM_ICONS;      break;
        case ID_VIEW_STYLE_SMALL_ICONS:       sViewStyle = VIEW_STYLE_SMALL_ICONS;       break;
        case ID_VIEW_STYLE_LIST:              sViewStyle = VIEW_STYLE_LIST;              break;
        case ID_VIEW_STYLE_DETAILS:           sViewStyle = VIEW_STYLE_DETAILS;           break;
        case ID_VIEW_STYLE_THUMBNAIL:         sViewStyle = VIEW_STYLE_THUMBNAIL;         break;
        }

        if (sViewStyle != -1)
        {
            xpr_bool_t sRefesh = (sViewStyle == VIEW_STYLE_THUMBNAIL) ? XPR_TRUE : XPR_FALSE;
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
