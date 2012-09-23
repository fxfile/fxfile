//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_COMMAND_H__
#define __FX_CMD_COMMAND_H__
#pragma once

#include "cmd_context.h"

namespace cmd
{
class CommandContext;
class CommandRouter;

class Command
{
    friend class CommandRouter;

public:
    enum
    {
        StateUnbinded = 0x80000000,
        StateDisable  = 0x00000000,
        StateEnable   = 0x00000001,
        StateCheck    = 0x00000002,
        StateRadio    = 0x00000004,
    };

public:
    Command(void) {}
    virtual ~Command(void) {}

public:
    virtual xpr_sint_t canExecute(CommandContext &aContext) { return StateEnable; }
    virtual void execute(CommandContext &aContext) {}
};

#define XPR_COMMAND_DECLARE_CLASS(CommandClass) \
class CommandClass : public Command \
{ \
public: \
    CommandClass(void) {} \
    virtual ~CommandClass(void) {} \
 \
public: \
    virtual xpr_sint_t canExecute(CommandContext &aContext); \
    virtual void execute(CommandContext &aContext); \
}

#define XPR_COMMAND_DECLARE_CTRL \
    HWND sFocusWnd = GetFocus(); \
    xpr_uint_t         sCommandId        = aContext.getCommandId(); \
    MainFrame         *sMainFrame        = aContext.getMainFrame(); \
    CWnd              *sTargetWnd        = aContext.getTargetWnd(); \
    CommandParameters *sParameters       = aContext.getParameters(); \
    FolderCtrl        *sFolderCtrl       = XPR_NULL; \
    ExplorerCtrl      *sExplorerCtrl     = XPR_NULL; \
    SearchResultCtrl  *sSearchResultCtrl = XPR_NULL; \
     \
    do \
    { \
        if (sTargetWnd != XPR_NULL) \
        { \
            aContext.castKnownCtrl(sTargetWnd, sFolderCtrl, sExplorerCtrl, sSearchResultCtrl); \
        } \
         \
        if (sFolderCtrl == XPR_NULL && sExplorerCtrl == XPR_NULL && sSearchResultCtrl == XPR_NULL) \
        { \
            aContext.getKnownCtrl(aContext, sFolderCtrl, sExplorerCtrl, sSearchResultCtrl); \
            sTargetWnd = XPR_NULL; \
        } \
    } \
    while (0)

#define XPR_COMMAND_IF_FOLDER_CTRL \
    if ((sFolderCtrl != XPR_NULL) && ((sFocusWnd == sFolderCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sFolderCtrl->m_hWnd)))

#define XPR_COMMAND_ELSE_IF_FOLDER_CTRL \
    else if ((sFolderCtrl != XPR_NULL) && ((sFocusWnd == sFolderCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sFolderCtrl->m_hWnd)))

#define XPR_COMMAND_IF_SEARCH_RESULT_CTRL \
    if ((sSearchResultCtrl != XPR_NULL) && ((sFocusWnd == sSearchResultCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sSearchResultCtrl->m_hWnd)))

#define XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL \
    else if ((sSearchResultCtrl != XPR_NULL) && ((sFocusWnd == sSearchResultCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sSearchResultCtrl->m_hWnd)))

#define XPR_COMMAND_IF_EXPLORER_CTRL \
    if ((sExplorerCtrl != XPR_NULL) && ((sFocusWnd == sExplorerCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sExplorerCtrl->m_hWnd)))

#define XPR_COMMAND_ELSE_IF_EXPLORER_CTRL  \
    else if ((sExplorerCtrl != XPR_NULL) && ((sFocusWnd == sExplorerCtrl->m_hWnd) || (sTargetWnd != XPR_NULL && sTargetWnd->m_hWnd == sExplorerCtrl->m_hWnd)))

#define XPR_COMMAND_ELSE \
    else
} // namespace cmd

#endif // __FX_CMD_COMMAND_H__
