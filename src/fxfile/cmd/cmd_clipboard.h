//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CMD_CLIPBOARD_H__
#define __FXFILE_CMD_CLIPBOARD_H__ 1
#pragma once

#include "cmd_command.h"

namespace fxfile
{
namespace cmd
{
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFileCutCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFileCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFilePasteCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFilePasteCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFilePasteLinkCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFileSpecialPasteCommand);

FXFILE_COMMAND_DECLARE_CLASS(ClipboardNameCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardFileNameCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardPathCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardDevPathCopyCommand);
FXFILE_COMMAND_DECLARE_CLASS(ClipboardUrlCopyCommand);

xpr_sint_t pasteNewTextFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aUnicode);
xpr_sint_t pasteNewBitmapFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aDib);
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_CMD_CLIPBOARD_H__
