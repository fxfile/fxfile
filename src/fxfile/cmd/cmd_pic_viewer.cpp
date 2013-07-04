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

#include "picture_viewer.h"
#include "main_frame.h"
#include "desktop_background.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t PictureViewerCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void PictureViewerCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->togglePictureViewer();
}

xpr_sint_t PictureViewerZoomInCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerZoomInCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->zoomIn();
}

xpr_sint_t PictureViewerZoomOutCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerZoomOutCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->zoomOut();
}

xpr_sint_t PictureViewerZoom100Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isDocking() == XPR_FALSE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerZoom100Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->zoom100();
}

xpr_sint_t PictureViewerRatioResizeCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        if (sMainFrame->mPictureViewer->isDocking() == XPR_FALSE)
            sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isRatioResize() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerRatioResizeCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->toggleRatioResize();
}

xpr_sint_t PictureViewerDockingCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isDocking() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerDockingCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->toggleDocking();
}

xpr_sint_t PictureViewerAutoHideCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isAutoHide() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerAutoHideCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->toggleAutoHide();
}

xpr_sint_t PictureViewerLockImageCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isLockSize() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerLockImageCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setLockSize();
}

xpr_sint_t PictureViewerLockWindowCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isLockWindow() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerLockWindowCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setLockWindow();
}

xpr_sint_t PictureViewerLock100Command::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isLock100() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerLock100Command::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setLock100();
}

xpr_sint_t PictureViewerLockThumbnailCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isLockThumbnail() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerLockThumbnailCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setLockThumbnail();
}

xpr_sint_t PictureViewerShowFileNameCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
    {
        sState |= StateEnable;

        if (sMainFrame->mPictureViewer->isShowFileName() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void PictureViewerShowFileNameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->toggleFileName();
}

xpr_sint_t PictureViewerBackgroundCenterCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PictureViewerBackgroundCenterCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setDesktop(DesktopBackground::StyleCenter);
}

xpr_sint_t PictureViewerBackgroundTitleCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PictureViewerBackgroundTitleCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setDesktop(DesktopBackground::StyleTile);
}

xpr_sint_t PictureViewerBackgroundStretchCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PictureViewerBackgroundStretchCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->setDesktop(DesktopBackground::StyleStretch);
}

xpr_sint_t PictureViewerToggleCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PictureViewerToggleCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->toggleShowHide();
}

xpr_sint_t PictureViewerHideCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (sMainFrame->isVisiblePictureViewer() == XPR_TRUE)
        sState |= StateEnable;

    return sState;
}

void PictureViewerHideCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    sMainFrame->mPictureViewer->hide();
}
} // namespace cmd
} // namespace fxfile
