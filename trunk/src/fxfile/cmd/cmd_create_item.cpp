//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_create_item.h"

#include "shell_new.h"
#include "option.h"
#include "create_item_dlg.h"
#include "shortcut_dlg.h"
#include "batch_create_dlg.h"
#include "main_frame.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t CreateFolderCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr::tstring sPath;
        xpr::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mConfig.mNewItemDlg == XPR_TRUE)
        {
            xpr::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.create_folder.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, gApp.loadString(XPR_STRING_LITERAL("popup.create_folder.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, gApp.loadString(XPR_STRING_LITERAL("popup.create_folder.msg.empty")));
            sDlg.setDesc(gApp.loadString(XPR_STRING_LITERAL("popup.create_folder.label.top_desc")));
            sDlg.setDescIcon(IDR_MAINFRAME, XPR_TRUE);

            xpr_sintptr_t sId = sDlg.DoModal();
            if (sId == IDOK)
            {
                sPath = sDlg.getNewItem();
                sResult = XPR_TRUE;
            }
        }
        else
        {
            sResult = SetNewPath(sPath, sDir, gApp.loadString(XPR_STRING_LITERAL("popup.create_folder.new_name")), XPR_STRING_LITERAL(""));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createFolder(sPath, !gOpt->mConfig.mNewItemDlg);
        }
    }
}

xpr_sint_t CreateGeneralFileCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateGeneralFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr::tstring sPath;
        xpr::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mConfig.mNewItemDlg == XPR_TRUE)
        {
            HICON sIcon = GetFileExtIcon(XPR_STRING_LITERAL(".fxfile"), XPR_TRUE);

            xpr::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setExt(XPR_STRING_LITERAL(""));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, gApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, gApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.msg.empty")));
            sDlg.setDesc(gApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.label.top_desc")));
            sDlg.setDescIcon(sIcon, XPR_TRUE);

            xpr_sintptr_t sId = sDlg.DoModal();
            if (sId == IDOK)
            {
                sPath = sDlg.getNewItem();
                sResult = XPR_TRUE;
            }

            DESTROY_ICON(sIcon);
        }
        else
        {
            sResult = SetNewPath(sPath, sDir, gApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.new_name")), XPR_STRING_LITERAL(""));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createGeneralFile(sPath, !gOpt->mConfig.mNewItemDlg);
        }
    }
}

xpr_sint_t CreateTextFileCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateTextFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        xpr::tstring sPath;
        xpr::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mConfig.mNewItemDlg == XPR_TRUE)
        {
            HICON sIcon = GetFileExtIcon(XPR_STRING_LITERAL(".txt"), XPR_TRUE);

            xpr::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setExt(XPR_STRING_LITERAL("txt"));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, gApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, gApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.msg.empty")));
            sDlg.setDesc(gApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.label.top_desc")));
            sDlg.setDescIcon(sIcon, XPR_TRUE);

            xpr_sintptr_t sId = sDlg.DoModal();
            if (sId == IDOK)
            {
                sPath = sDlg.getNewItem();
                sResult = XPR_TRUE;
            }

            DESTROY_ICON(sIcon);
        }
        else
        {
            sResult = SetNewPath(sPath, sDir, gApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.new_name")), XPR_STRING_LITERAL(".txt"));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createTextFile(sPath, !gOpt->mConfig.mNewItemDlg);
        }
    }
}

xpr_sint_t CreateShortcutCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateShortcutCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        const xpr_tchar_t *sCurDir = sExplorerCtrl->getCurPath();

        ShortcutDlg sDlg(sCurDir);
        sDlg.DoModal();
    }
}

xpr_sint_t CreateBatchCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateBatchCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        const xpr_tchar_t *sCurDir = sExplorerCtrl->getCurPath();
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();

        BatchCreateDlg sDlg(sCurDir);
        xpr_sintptr_t sId = sDlg.DoModal();
        if (sId == IDOK)
        {
            ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, (LPCVOID)sTvItemData->mFullPidl, XPR_NULL);
        }
    }
}

xpr_sint_t FileNewCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileNewCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_FALSE)
            return;

        const xpr_tchar_t *sDir = sExplorerCtrl->getCurPath();
        if (sDir == XPR_NULL)
            return;

        ShellNew &sShellNew = ShellNew::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        if (sShellNew.doCommand(sCommandId, sDir, sPath) == XPR_TRUE)
        {
            sExplorerCtrl->setInsSelPath(sPath);
            sExplorerCtrl->doShChangeNotifyUpdateDir();
        }
    }
}
} // namespace cmd
} // namespace fxfile
