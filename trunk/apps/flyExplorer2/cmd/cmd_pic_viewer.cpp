//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_pic_viewer.h"

#include "PicViewer.h"
#include "MainFrame.h"
#include "desktop_background.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t PicViewerCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void PicViewerCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->togglePicViewer();
}

xpr_sint_t PicViewerZoomInCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerZoomInCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->zoomIn();
}

xpr_sint_t PicViewerZoomOutCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerZoomOutCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->zoomOut();
}

xpr_sint_t PicViewerZoom100Command::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerZoom100Command::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->zoom100();
}

xpr_sint_t PicViewerRatioResizeCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        if (sMainFrame->mPicViewer->isDocking() == XPR_FALSE)
            sState |= StateEnable;

        if (sMainFrame->mPicViewer->isRatioResize() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerRatioResizeCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->toggleRatioResize();
}

xpr_sint_t PicViewerDockingCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isDocking() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerDockingCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->toggleDocking();
}

xpr_sint_t PicViewerAutoHideCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isAutoHide() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerAutoHideCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->toggleAutoHide();
}

xpr_sint_t PicViewerLockImageCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isLockSize() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerLockImageCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setLockSize();
}

xpr_sint_t PicViewerLockWindowCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isLockWindow() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerLockWindowCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setLockWindow();
}

xpr_sint_t PicViewerLock100Command::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isLock100() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerLock100Command::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setLock100();
}

xpr_sint_t PicViewerLockThumbnailCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isLockThumbnail() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerLockThumbnailCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setLockThumbnail();
}

xpr_sint_t PicViewerShowFileNameCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPicViewer->isShowFileName() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PicViewerShowFileNameCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->toggleFileName();
}

xpr_sint_t PicViewerBackgroundCenterCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PicViewerBackgroundCenterCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setDesktop(DesktopBackground::StyleCenter);
}

xpr_sint_t PicViewerBackgroundTitleCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PicViewerBackgroundTitleCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setDesktop(DesktopBackground::StyleTile);
}

xpr_sint_t PicViewerBackgroundStretchCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PicViewerBackgroundStretchCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->setDesktop(DesktopBackground::StyleStretch);
}

xpr_sint_t PicViewerToggleCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PicViewerToggleCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->toggleShowHide();
}

xpr_sint_t PicViewerHideCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePicViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PicViewerHideCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->mPicViewer->hide();
}
} // namespace cmd
