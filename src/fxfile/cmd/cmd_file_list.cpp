//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_list.h"

#include "option.h"
#include "file_list_dlg.h"
#include "explorer_ctrl.h"

#include "gui/FileDialogST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t FileListCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = XPR_TRUE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileListCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr_tchar_t sFilter[0xff] = {0};
        _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.txt)\0*.txt\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.text_files")));

        CFileDialogST sFileDialog(XPR_FALSE, XPR_STRING_LITERAL("*.txt"), XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, sExplorerCtrl);
        if (gOpt->mConfig.mFileListCurDir == XPR_TRUE && sExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
            sFileDialog.SetInitDir(sExplorerCtrl->getCurPath());

        if (sFileDialog.DoModal() != IDOK)
            return;

        FileListDlg sDlg;
        sDlg.setTextFile(sFileDialog.GetPathName());

        xpr_sint_t sIndex;
        LPLVITEMDATA sLvItemData;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        if (sExplorerCtrl->getRealSelCount() > 0)
        {
            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                sDlg.addPath(sPath);
            }
        }
        else
        {
            xpr_sint_t i;
            xpr_sint_t sCount = sExplorerCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(i);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                sDlg.addPath(sPath);
            }
        }

        sDlg.DoModal();
    }
}
} // namespace cmd
} // namespace fxfile
