//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_recent.h"

#include "recent_file_list.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t RecentFileCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void RecentFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_sint_t sIndex = sCommandId - ID_FILE_RECENT_FILE_LIST_FIRST;

    RecentFileList &sRecentFileList = RecentFileList::instance();

    const xpr_tchar_t *sPath = sRecentFileList.getFile(sIndex);
    XPR_ASSERT(sPath != XPR_NULL);

    ExecFile(sPath);
}
} // namespace cmd
} // namespace fxfile
