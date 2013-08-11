//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_window.h"

#include "file_op_thread.h"

#include "main_frame.h"
#include "explorer_view.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t WindowNextCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowNextCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    Splitter *sSplitter = sMainFrame->getSplitter();
    if (XPR_IS_NULL(sSplitter))
        return;

    xpr_sint_t sRow = 0, sColumn = 0;
    xpr_sint_t sRowCount = 1, sColumnCount = 1;
    sSplitter->getActivePane(&sRow, &sColumn);
    sSplitter->getPaneCount(&sRowCount, &sColumnCount);

    if (++sColumn >= sColumnCount)
    {
        sColumn = 0;
        if (++sRow >= sRowCount)
            sRow = 0;
    }

    sSplitter->setActivePane(sRow, sColumn);
}

xpr_sint_t WindowPrevCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowPrevCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    Splitter *sSplitter = sMainFrame->getSplitter();
    if (XPR_IS_NULL(sSplitter))
        return;

    xpr_sint_t sRow = 0, sColumn = 0;
    xpr_sint_t sRowCount = 1, sColumnCount = 1;
    sSplitter->getActivePane(&sRow, &sColumn);
    sSplitter->getPaneCount(&sRowCount, &sColumnCount);

    if (--sColumn < 0)
    {
        sColumn = sColumnCount - 1;
        if (--sRow < 0)
            sRow = sRowCount - 1;
    }

    sSplitter->setActivePane(sRow, sColumn);
}

xpr_sint_t WindowSplitToolBarCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void WindowSplitToolBarCommand::execute(CommandContext &aContext)
{
}

xpr_sint_t WindowSplitSingleCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        sState |= StateRadio;

    return sState;
}

void WindowSplitSingleCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(1, 1);
}

xpr_sint_t WindowSplit1x2Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    sMainFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount == 1 && sViewSplitColumnCount == 2)
        sState |= StateRadio;

    return sState;
}

void WindowSplit1x2Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(1, 2);
}

xpr_sint_t WindowSplit2x1Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    sMainFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount == 2 && sViewSplitColumnCount == 1)
        sState |= StateRadio;

    return sState;
}

void WindowSplit2x1Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(2, 1);
}

xpr_sint_t WindowSplit2x2Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    sMainFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount == 2 && sViewSplitColumnCount == 2)
        sState |= StateRadio;

    return sState;
}

void WindowSplit2x2Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(2, 2);
}

xpr_sint_t WindowSplit1x3Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    sMainFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount == 1 && sViewSplitColumnCount == 3)
        sState |= StateRadio;

    return sState;
}

void WindowSplit1x3Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(1, 3);
}

xpr_sint_t WindowSplit2x3Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    sMainFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount == 2 && sViewSplitColumnCount == 3)
        sState |= StateRadio;

    return sState;
}

void WindowSplit2x3Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->splitView(2, 3);
}

xpr_sint_t WindowSplitEquallyCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowSplitEquallyCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->setSplitEqually();
}

xpr_sint_t WindowSplitRatioCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowSplitRatioCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_double_t sRatio = (xpr_float_t)((sCommandId - ID_WINDOW_SPLIT_10) + 1) / 10;

    sMainFrame->setSplitRatio(sRatio);
}

xpr_sint_t WindowSwapFolderCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowSwapFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        LPITEMIDLIST sFullPidl1 = fxfile::base::Pidl::clone(sExplorerCtrl1->getFolderData()->mFullPidl);
        LPITEMIDLIST sFullPidl2 = fxfile::base::Pidl::clone(sExplorerCtrl2->getFolderData()->mFullPidl);

        sExplorerCtrl1->explore(sFullPidl2);
        sExplorerCtrl2->explore(sFullPidl1);
    }
}

xpr_sint_t WindowEqualFolderCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowEqualFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl2->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
            if (sExplorerCtrl1->explore(sFullPidl) == XPR_FALSE)
                COM_FREE(sFullPidl);
        }
    }
}

xpr_sint_t WindowOtherEqualFolderCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowOtherEqualFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl1->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
            if (sExplorerCtrl2->explore(sFullPidl) == XPR_FALSE)
                COM_FREE(sFullPidl);
        }
    }
}

xpr_sint_t WindowCopyCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANCOPY) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

static void copyTo(ExplorerCtrl *sExplorerCtrl, const xpr_tchar_t *aTarget)
{
    xpr_sint_t sCount = 0;
    xpr_tchar_t *sSource = XPR_NULL;
    if (sExplorerCtrl->getSelItemPath(&sSource, sCount, SFGAO_CANCOPY) == XPR_TRUE)
    {
        xpr_tchar_t *sTarget = new xpr_tchar_t[XPR_MAX_PATH + 1];
        _tcscpy(sTarget, aTarget);
        sTarget[_tcslen(sTarget) + 1] = XPR_STRING_LITERAL('\0');

        FileOpThread *sFileOpThread = new FileOpThread;
        sFileOpThread->start(XPR_TRUE, sSource, sCount, sTarget);
    }
}

static void moveTo(ExplorerCtrl *sExplorerCtrl, const xpr_tchar_t *aTarget)
{
    xpr_sint_t sCount = 0;
    xpr_tchar_t *sSource = XPR_NULL;
    if (sExplorerCtrl->getSelItemPath(&sSource, sCount, SFGAO_CANMOVE) == XPR_TRUE)
    {
        xpr_tchar_t *sTarget = new xpr_tchar_t[XPR_MAX_PATH + 1];
        _tcscpy(sTarget, aTarget);
        sTarget[_tcslen(sTarget) + 1] = XPR_STRING_LITERAL('\0');

        FileOpThread *sFileOpThread = new FileOpThread;
        sFileOpThread->start(XPR_FALSE, sSource, sCount, sTarget);
    }
}

void WindowCopyCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        copyTo(sExplorerCtrl1, sExplorerCtrl2->getCurPath());
}

xpr_sint_t WindowMoveCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANMOVE) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowMoveCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        moveTo(sExplorerCtrl1, sExplorerCtrl2->getCurPath());
}

xpr_sint_t WindowCopy1Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
                std::swap(sExplorerCtrl1, sExplorerCtrl2);

            if (sExplorerCtrl2->hasSelShellAttributes(SFGAO_CANCOPY) == XPR_TRUE && sExplorerCtrl1->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowCopy1Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
            std::swap(sExplorerCtrl1, sExplorerCtrl2);

        const xpr_tchar_t *sCurPath = sExplorerCtrl1->getCurPath();

        copyTo(sExplorerCtrl2, sCurPath);
    }
}

xpr_sint_t WindowMove1Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
                std::swap(sExplorerCtrl1, sExplorerCtrl2);

            if (sExplorerCtrl2->hasSelShellAttributes(SFGAO_CANMOVE) == XPR_TRUE && sExplorerCtrl1->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowMove1Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
            std::swap(sExplorerCtrl1, sExplorerCtrl2);

        const xpr_tchar_t *sCurPath = sExplorerCtrl1->getCurPath();

        moveTo(sExplorerCtrl2, sCurPath);
    }
}

xpr_sint_t WindowCopy2Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
                std::swap(sExplorerCtrl1, sExplorerCtrl2);

            if (sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANCOPY) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowCopy2Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
            std::swap(sExplorerCtrl1, sExplorerCtrl2);

        const xpr_tchar_t *sCurPath = sExplorerCtrl2->getCurPath();

        copyTo(sExplorerCtrl1, sCurPath);
    }
}

xpr_sint_t WindowMove2Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
                std::swap(sExplorerCtrl1, sExplorerCtrl2);

            if (sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANMOVE) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowMove2Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        if (sExplorerCtrl1->getViewIndex() > sExplorerCtrl2->getViewIndex())
            std::swap(sExplorerCtrl1, sExplorerCtrl2);

        const xpr_tchar_t *sCurPath = sExplorerCtrl2->getCurPath();

        moveTo(sExplorerCtrl1, sCurPath);
    }
}

xpr_sint_t WindowDualCopyCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if ((sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANCOPY) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE) ||
                (sExplorerCtrl2->hasSelShellAttributes(SFGAO_CANCOPY) == XPR_TRUE && sExplorerCtrl1->isFileSystemFolder() == XPR_TRUE))
            {
                sState = StateEnable;
            }
        }
    }

    return sState;
}

void WindowDualCopyCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        copyTo(sExplorerCtrl1, sExplorerCtrl2->getCurPath());
        copyTo(sExplorerCtrl2, sExplorerCtrl1->getCurPath());
    }
}

xpr_sint_t WindowDualMoveCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if ((sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANMOVE) == XPR_TRUE && sExplorerCtrl2->isFileSystemFolder() == XPR_TRUE) ||
                (sExplorerCtrl2->hasSelShellAttributes(SFGAO_CANMOVE) == XPR_TRUE && sExplorerCtrl1->isFileSystemFolder() == XPR_TRUE))
            {
                sState = StateEnable;
            }
        }
    }

    return sState;
}

void WindowDualMoveCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        moveTo(sExplorerCtrl1, sExplorerCtrl2->getCurPath());
        moveTo(sExplorerCtrl2, sExplorerCtrl1->getCurPath());
    }
}

xpr_sint_t WindowDualDeleteCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
    {
        ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
        ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);
 
        if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
        {
            if (sExplorerCtrl1->hasSelShellAttributes(SFGAO_CANDELETE) == XPR_TRUE ||
                sExplorerCtrl2->hasSelShellAttributes(SFGAO_CANDELETE) == XPR_TRUE)
                sState = StateEnable;
        }
    }

    return sState;
}

void WindowDualDeleteCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isSingleView() == XPR_TRUE)
        return;

    ExplorerCtrl *sExplorerCtrl1 = sExplorerCtrl;
    ExplorerCtrl *sExplorerCtrl2 = sMainFrame->getExplorerCtrl(-2);

    if (XPR_IS_NOT_NULL(sExplorerCtrl1) && XPR_IS_NOT_NULL(sExplorerCtrl2))
    {
        sMainFrame->executeCommand(ID_FILE_DELETE, sExplorerCtrl1);
        sMainFrame->executeCommand(ID_FILE_DELETE, sExplorerCtrl2);
    }
}

xpr_sint_t WindowCompareCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = 0;

    if (sMainFrame->isSingleView() == XPR_FALSE)
        sState |= StateEnable;

    return sState;
}

void WindowCompareCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->compareWindow();
}
} // namespace cmd
} // namespace fxfile
