//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_split.h"

#include "file_combine_dlg.h"
#include "file_split_dlg.h"
#include "file_merge_dlg.h"
#include "text_merge_dlg.h"
#include "main_frame.h"
#include "explorer_view.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t FileCombineCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() == 1) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileCombineCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->GetSelectedCount() != 1)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.select"));
            sExplorerCtrl->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
            return;
        }

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            return;

        if (IsFileSystem(sLvItemData->mShellFolder, sLvItemData->mPidl) == XPR_FALSE)
            return;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        xpr_sint_t nStartNumber = -1;

        const xpr_tchar_t *sExt = GetFileExt(sPath);
        if (_tcslen(sExt) > 0)
            _stscanf(sExt+1, XPR_STRING_LITERAL("%d"), &nStartNumber);

        if (nStartNumber == -1)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.unsuitable"));
            sExplorerCtrl->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            return;
        }

        xpr_tchar_t sDestDir[XPR_MAX_PATH + 1] = {0};
        ExplorerView *sExplorerView = sMainFrame->getExplorerView(-2);
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            ExplorerCtrl *sOtherExplorerCtrl = sExplorerView->getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sOtherExplorerCtrl))
            {
                if (sOtherExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
                    sOtherExplorerCtrl->getCurPath(sDestDir);
            }
        }

        if (_tcslen(sDestDir) <= 0)
            sExplorerCtrl->getCurPath(sDestDir);

        FileCombineDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setDestDir(sDestDir);
        sDlg.DoModal();
    }
}

xpr_sint_t FileSplitCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() == 1) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileSplitCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->GetSelectedCount() != 1)
            return;

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            return;

        if (IsFileSystem(sLvItemData->mShellFolder, sLvItemData->mPidl) == XPR_FALSE)
            return;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        xpr_tchar_t sDestDir[XPR_MAX_PATH + 1] = {0};
        ExplorerView *sExplorerView = sMainFrame->getExplorerView(-2);
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            ExplorerCtrl *sOtherExplorerCtrl = sExplorerView->getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sOtherExplorerCtrl))
            {
                if (sOtherExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
                    sOtherExplorerCtrl->getCurPath(sDestDir);
            }
        }

        if (_tcslen(sDestDir) <= 0)
            sExplorerCtrl->getCurPath(sDestDir);

        FileSplitDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setDestDir(sDestDir);
        sDlg.DoModal();
    }
}

xpr_sint_t FileMergeCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 1) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileMergeCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_FALSE)
            return;

        if (sExplorerCtrl->GetSelectedCount() <= 1)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_merge.msg.select_multiple"));
            sExplorerCtrl->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            return;
        }

        xpr_tchar_t sDestDir[XPR_MAX_PATH + 1] = {0};
        ExplorerView *sExplorerView = sMainFrame->getExplorerView(-2);
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            ExplorerCtrl *sOtherExplorerCtrl = sExplorerView->getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sOtherExplorerCtrl))
            {
                if (sOtherExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
                    sOtherExplorerCtrl->getCurPath(sDestDir);
            }
        }

        if (_tcslen(sDestDir) <= 0)
            sExplorerCtrl->getCurPath(sDestDir);

        xpr_tchar_t sCurPath[XPR_MAX_PATH + 1] = {0};
        sExplorerCtrl->getCurPath(sCurPath);

        FileMergeDlg sDlg;
        sDlg.setDir(sCurPath);
        sDlg.setDestPath(sDestDir);

        xpr_sint_t sIndex;
        LPLVITEMDATA sLvItemData;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];

        POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

            sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if (XPR_IS_NULL(sLvItemData))
                continue;

            if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                continue;

            if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                continue;

            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
            sDlg.addPath(sPath);
        }

        sDlg.DoModal();
    }
}

xpr_sint_t TextFileMergeCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void TextFileMergeCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_FALSE || sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        TextMergeDlg sDlg;

        xpr_sint_t sIndex;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        LPLVITEMDATA sLvItemData = XPR_NULL;

        POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

            sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if (XPR_IS_NULL(sLvItemData))
                continue;

            if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
                !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
            {
                GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                sDlg.addPath(sPath);
            }
        }

        sDlg.DoModal();
    }
}
} // namespace cmd
} // namespace fxfile
