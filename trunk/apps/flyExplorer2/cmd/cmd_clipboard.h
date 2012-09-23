//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CMD_CLIPBOARD_H__
#define __FX_CMD_CLIPBOARD_H__
#pragma once

#include "cmd_command.h"

namespace cmd
{
XPR_COMMAND_DECLARE_CLASS(ClipboardFileCutCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFileCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFilePasteCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFilePasteCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFilePasteLinkCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFileSpecialPasteCommand);

XPR_COMMAND_DECLARE_CLASS(ClipboardNameCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardFileNameCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardPathCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardDevPathCopyCommand);
XPR_COMMAND_DECLARE_CLASS(ClipboardUrlCopyCommand);

xpr_sint_t pasteNewTextFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aUnicode);
xpr_sint_t pasteNewBitmapFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aDib);
} // namespace cmd

#endif // __FX_CMD_CLIPBOARD_H__
