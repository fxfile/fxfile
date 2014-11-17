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

#include "shell.h"

#include "about_dlg.h"
#include "tip_dlg.h"
#include "win_app.h"
#include "program_opts.h"
#include "info_copyable_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
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

xpr_sint_t CmdLineArgsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void CmdLineArgsCommand::execute(CommandContext &aContext)
{
    InfoCopyableDlg sDlg;
    sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.command_line_arguments.title")));
    sDlg.setInfo(gApp.m_lpCmdLine);
    sDlg.DoModal();
}

xpr_sint_t ProgramOptsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void ProgramOptsCommand::execute(CommandContext &aContext)
{
    xpr::string sUsage;
    ProgramOpts::getUsage(sUsage);

    InfoCopyableDlg sDlg;
    sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.program_options.title")));
    sDlg.setInfo(sUsage);
    sDlg.setInfoFont(XPR_STRING_LITERAL("Consolas"));
    sDlg.DoModal();
}

xpr_sint_t EnvVarsCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void EnvVarsCommand::execute(CommandContext &aContext)
{
    xpr::Env *sEnvList;
    xpr_size_t sCount;
    xpr_rcode_t sRcode;
    xpr::string sMsg;

    sRcode = xpr::getEnvList(&sEnvList, &sCount);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        if (sCount > 0)
        {
            xpr::Env *sEnvNode = sEnvList;

            do
            {
                sMsg += sEnvNode->mEnvVar;

                if (XPR_IS_NOT_NULL(sEnvNode->mNext))
                {
                    sMsg += XPR_STRING_LITERAL("\r\n");
                }

                sEnvNode = sEnvNode->mNext;

            } while (XPR_IS_NOT_NULL(sEnvNode));
        }

        InfoCopyableDlg sDlg;
        sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.environment_variables.title")));
        sDlg.setInfo(sMsg);
        sDlg.DoModal();

        xpr::freeEnvList(sEnvList);
    }
}

xpr_sint_t HompageCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void HompageCommand::execute(CommandContext &aContext)
{
    NavigateURL(XPR_STRING_LITERAL("http://flychk.com"));
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
} // namespace fxfile
