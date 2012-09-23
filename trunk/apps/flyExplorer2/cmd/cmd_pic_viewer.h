//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_PIC_VIEWER_H__
#define __FX_CMD_PIC_VIEWER_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(PicViewerCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerZoomInCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerZoomOutCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerZoom100Command);
XPR_COMMAND_DECLARE_CLASS(PicViewerRatioResizeCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerDockingCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerAutoHideCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerLockImageCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerLockWindowCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerLock100Command);
XPR_COMMAND_DECLARE_CLASS(PicViewerLockThumbnailCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerShowFileNameCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerBackgroundCenterCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerBackgroundTitleCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerBackgroundStretchCommand);

XPR_COMMAND_DECLARE_CLASS(PicViewerToggleCommand);
XPR_COMMAND_DECLARE_CLASS(PicViewerHideCommand);
} // namespace cmd

#endif // __FX_CMD_PIC_VIEWER_H__
