//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_help.h"

#include "AboutDlg.h"
#include "TipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t HelpCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HelpCommand::execute(CommandContext &aContext)
{
    ::HtmlHelp(::GetDesktopWindow(), AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_TOC, XPR_NULL);
}

xpr_sint_t HelpFindCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HelpFindCommand::execute(CommandContext &aContext)
{
    ::HtmlHelp(::GetDesktopWindow(), AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_INDEX, XPR_NULL);
}

xpr_sint_t HelpAcceleratorCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HelpAcceleratorCommand::execute(CommandContext &aContext)
{
    xpr_tchar_t sAccelerator[XPR_MAX_PATH * 2 + 1] = {0};
    _stprintf(sAccelerator, XPR_STRING_LITERAL("%s::html/shortkey.htm"), AfxGetApp()->m_pszHelpFilePath);

    ::HtmlHelp(::GetDesktopWindow(), sAccelerator, HH_DISPLAY_TOC, XPR_NULL);
}

xpr_sint_t TipOfTodayCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void TipOfTodayCommand::execute(CommandContext &aContext)
{
    TipDlg sDlg;
    sDlg.DoModal();
}

xpr_sint_t HompageCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HompageCommand::execute(CommandContext &aContext)
{
    fxb::NavigateURL(XPR_STRING_LITERAL("http://flychk.com"));
}

xpr_sint_t EmailCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void EmailCommand::execute(CommandContext &aContext)
{
    ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), XPR_STRING_LITERAL("mailto:flychk@flychk.com"), XPR_NULL, XPR_NULL, 0);
}

xpr_sint_t AboutCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void AboutCommand::execute(CommandContext &aContext)
{
    AboutDlg sDlg;
    sDlg.DoModal();
}
} // namespace cmd
