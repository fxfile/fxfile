//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_GO_PATH_H__
#define __FXFILE_CMD_GO_PATH_H__ 1
#pragma once

#include "router/cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(GoPathCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolderAllCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder1Command);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder2Command);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder3Command);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder4Command);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder5Command);
FXFILE_COMMAND_DECLARE_CLASS(GoInitFolder6Command);

FXFILE_COMMAND_DECLARE_CLASS(GoSystemDesktopCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemLibrariesCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemDocumentsCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemPicturesCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemMusicCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemVideosCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemMyDocumentCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemMyComputerCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemNetworkCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemControlsCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemRecycleCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemProgramFilesCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemCommonCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemWindowsCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemWindowsSystemCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemFontsCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemStartupCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemTemporaryCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemFavoritesCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemInternetCacheCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemInternetCookieCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemSendToCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemRecentCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSystemAppDataCommand);

FXFILE_COMMAND_DECLARE_CLASS(GoSiblingUpCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoSiblingDownCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoDirectUpCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoRootCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoUpBoxCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoFxFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoFxFileConfFileCommand);
FXFILE_COMMAND_DECLARE_CLASS(GoUpCommand);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_GO_PATH_H__
