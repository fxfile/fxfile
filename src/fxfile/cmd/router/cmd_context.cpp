//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_context.h"

#include "main_frame.h"
#include "folder_ctrl.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"
#include "file_scrap_pane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
CommandContext::CommandContext(void)
    : mCommandId(0), mMainFrame(XPR_NULL), mTargetWnd(XPR_NULL), mParameters(XPR_NULL)
{
}

CommandContext::~CommandContext(void)
{
}

xpr_uint_t CommandContext::getCommandId(void) const
{
    return mCommandId;
}

MainFrame *CommandContext::getMainFrame(void) const
{
    return mMainFrame;
}

CWnd *CommandContext::getTargetWnd(void) const
{
    return mTargetWnd;
}

CommandParameters *CommandContext::getParameters(void) const
{
    return mParameters;
}

void CommandContext::setCommandId(xpr_uint_t aCommandId)
{
    mCommandId = aCommandId;
}

void CommandContext::setMainFrame(MainFrame *aMainFrame)
{
    mMainFrame = aMainFrame;
}

void CommandContext::setTargetWnd(CWnd *aTargetWnd)
{
    mTargetWnd = aTargetWnd;
}

void CommandContext::setParameters(CommandParameters *aParameters)
{
    mParameters = aParameters;
}

void CommandContext::getKnownCtrl(CommandContext &aContext, FolderCtrl *&aFolderCtrl, ExplorerCtrl *&aExplorerCtrl, SearchResultCtrl *&aSearchResultCtrl, FileScrapPane *&aFileScrapPane)
{
    if (aContext.mMainFrame == XPR_NULL)
        return;

    aFolderCtrl       = aContext.mMainFrame->getFolderCtrl();
    aExplorerCtrl     = aContext.mMainFrame->getExplorerCtrl();
    aSearchResultCtrl = aContext.mMainFrame->getSearchResultCtrl();
    aFileScrapPane    = aContext.mMainFrame->getFileScrapPane();
}

void CommandContext::castKnownCtrl(CWnd *aWnd, FolderCtrl *&aFolderCtrl, ExplorerCtrl *&aExplorerCtrl, SearchResultCtrl *&aSearchResultCtrl, FileScrapPane *&aFileScrapPane)
{
    aFolderCtrl       = dynamic_cast<FolderCtrl       *>(aWnd);
    aExplorerCtrl     = dynamic_cast<ExplorerCtrl     *>(aWnd);
    aSearchResultCtrl = dynamic_cast<SearchResultCtrl *>(aWnd);
    aFileScrapPane    = dynamic_cast<FileScrapPane    *>(aWnd);
}
} // namespace cmd
} // namespace fxfile
