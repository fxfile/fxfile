//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_MAIN_GUI_H__
#define __FX_CMD_MAIN_GUI_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(ShowToolBarCommand);
XPR_COMMAND_DECLARE_CLASS(ShowDriveBarCommand);
XPR_COMMAND_DECLARE_CLASS(ShowBookmarkBarCommand);
XPR_COMMAND_DECLARE_CLASS(ShowAddressBarCommand);
XPR_COMMAND_DECLARE_CLASS(ShowPathBarCommand);
XPR_COMMAND_DECLARE_CLASS(ShowActivateWndCommand);

XPR_COMMAND_DECLARE_CLASS(AddressBarDropCommand);

XPR_COMMAND_DECLARE_CLASS(DriveBarShortTextCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarLongTextCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarWrapableCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarEachAssignCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarLeftPlaceCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarRightPlaceCommand);
XPR_COMMAND_DECLARE_CLASS(DriveBarRefreshCommand);

XPR_COMMAND_DECLARE_CLASS(BookmarkBarNoNameCommand);
XPR_COMMAND_DECLARE_CLASS(BookmarkBarNameCommand);
XPR_COMMAND_DECLARE_CLASS(BookmarkBarWrapableCommand);
XPR_COMMAND_DECLARE_CLASS(BookmarkBarRefreshCommand);

XPR_COMMAND_DECLARE_CLASS(ToolLockCommand);
XPR_COMMAND_DECLARE_CLASS(ToolBarCustomizeCommand);

XPR_COMMAND_DECLARE_CLASS(ShowStatusBarCommand);

XPR_COMMAND_DECLARE_CLASS(NoneFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(SingleFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(EachFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(ShowFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(LeftFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(RightFolderPaneCommand);
XPR_COMMAND_DECLARE_CLASS(ShowSearchBarCommand);
} // namespace cmd

#endif // __FX_CMD_MAIN_GUI_H__
