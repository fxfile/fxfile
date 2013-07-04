//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_MAIN_GUI_H__
#define __FXFILE_CMD_MAIN_GUI_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(ShowToolBarCommand);
FXFILE_COMMAND_DECLARE_CLASS(ShowDriveBarCommand);
FXFILE_COMMAND_DECLARE_CLASS(ShowBookmarkBarCommand);

FXFILE_COMMAND_DECLARE_CLASS(AddressBarDropCommand);

FXFILE_COMMAND_DECLARE_CLASS(DriveBarShortTextCommand);
FXFILE_COMMAND_DECLARE_CLASS(DriveBarLongTextCommand);
FXFILE_COMMAND_DECLARE_CLASS(DriveBarWrapableCommand);
FXFILE_COMMAND_DECLARE_CLASS(DriveBarRefreshCommand);

FXFILE_COMMAND_DECLARE_CLASS(BookmarkBarNoNameCommand);
FXFILE_COMMAND_DECLARE_CLASS(BookmarkBarNameCommand);
FXFILE_COMMAND_DECLARE_CLASS(BookmarkBarWrapableCommand);
FXFILE_COMMAND_DECLARE_CLASS(BookmarkBarRefreshCommand);

FXFILE_COMMAND_DECLARE_CLASS(ToolLockCommand);
FXFILE_COMMAND_DECLARE_CLASS(ToolBarCustomizeCommand);

FXFILE_COMMAND_DECLARE_CLASS(NoneFolderPaneCommand);
FXFILE_COMMAND_DECLARE_CLASS(SingleFolderPaneCommand);
FXFILE_COMMAND_DECLARE_CLASS(EachFolderPaneCommand);
FXFILE_COMMAND_DECLARE_CLASS(ShowFolderPaneCommand);
FXFILE_COMMAND_DECLARE_CLASS(LeftFolderPaneCommand);
FXFILE_COMMAND_DECLARE_CLASS(RightFolderPaneCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_MAIN_GUI_H__
