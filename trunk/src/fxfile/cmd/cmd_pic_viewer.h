//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_PIC_VIEWER_H__
#define __FXFILE_CMD_PIC_VIEWER_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerZoomInCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerZoomOutCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerZoom100Command);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerRatioResizeCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerDockingCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerAutoHideCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerLockImageCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerLockWindowCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerLock100Command);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerLockThumbnailCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerShowFileNameCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerBackgroundCenterCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerBackgroundTitleCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerBackgroundStretchCommand);

FXFILE_COMMAND_DECLARE_CLASS(PictureViewerToggleCommand);
FXFILE_COMMAND_DECLARE_CLASS(PictureViewerHideCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_PIC_VIEWER_H__
