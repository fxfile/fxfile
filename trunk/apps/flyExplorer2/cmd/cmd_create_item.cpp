//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_create_item.h"

#include "fxb/fxb_shell_new.h"

#include "CreateItemDlg.h"
#include "ShortcutDlg.h"
#include "BatchCreateDlg.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t CreateFolderCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateFolderCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        std::tstring sPath;
        std::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        fxb::RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mNewItemDlg == XPR_TRUE)
        {
            std::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.create_folder.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, theApp.loadString(XPR_STRING_LITERAL("popup.create_folder.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, theApp.loadString(XPR_STRING_LITERAL("popup.create_folder.msg.empty")));
            sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.create_folder.label.top_desc")));
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
            sResult = fxb::SetNewPath(sPath, sDir, theApp.loadString(XPR_STRING_LITERAL("popup.create_folder.new_name")), XPR_STRING_LITERAL(""));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createFolder(sPath, !gOpt->mNewItemDlg);
        }
    }
}

xpr_sint_t CreateGeneralFileCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateGeneralFileCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        std::tstring sPath;
        std::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        fxb::RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mNewItemDlg == XPR_TRUE)
        {
            HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".flyExplorer"), XPR_TRUE);

            std::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setExt(XPR_STRING_LITERAL(""));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, theApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, theApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.msg.empty")));
            sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.label.top_desc")));
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
            sResult = fxb::SetNewPath(sPath, sDir, theApp.loadString(XPR_STRING_LITERAL("popup.create_general_file.new_name")), XPR_STRING_LITERAL(""));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createGeneralFile(sPath, !gOpt->mNewItemDlg);
        }
    }
}

xpr_sint_t CreateTextFileCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateTextFileCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        std::tstring sPath;
        std::tstring sDir;

        sExplorerCtrl->getCurPath(sDir);
        fxb::RemoveLastSplit(sDir);

        xpr_bool_t sResult = XPR_FALSE;
        if (gOpt->mNewItemDlg == XPR_TRUE)
        {
            HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".txt"), XPR_TRUE);

            std::tstring sCurDir;
            sExplorerCtrl->getCurPath(sCurDir);

            CreateItemDlg sDlg;
            sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.title")));
            sDlg.setDir(sCurDir.c_str());
            sDlg.setExt(XPR_STRING_LITERAL("txt"));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, theApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.msg.exist")));
            sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, theApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.msg.empty")));
            sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.label.top_desc")));
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
            sResult = fxb::SetNewPath(sPath, sDir, theApp.loadString(XPR_STRING_LITERAL("popup.create_text_file.new_name")), XPR_STRING_LITERAL(".txt"));
        }

        if (sResult == XPR_TRUE)
        {
            sExplorerCtrl->createTextFile(sPath, !gOpt->mNewItemDlg);
        }
    }
}

xpr_sint_t CreateShortcutCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateShortcutCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        const xpr_tchar_t *sCurDir = sExplorerCtrl->getCurPath();

        ShortcutDlg sDlg(sCurDir);
        sDlg.DoModal();
    }
}

xpr_sint_t CreateBatchCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void CreateBatchCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

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
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    if (sExplorerCtrl != XPR_NULL)
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileNewCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sExplorerCtrl != XPR_NULL)
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_FALSE)
            return;

        const xpr_tchar_t *sDir = sExplorerCtrl->getCurPath();
        if (sDir == XPR_NULL)
            return;

        fxb::ShellNew &sShellNew = fxb::ShellNew::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        if (sShellNew.doCommand(sCommandId, sDir, sPath) == XPR_TRUE)
        {
            sExplorerCtrl->setInsSelPath(sPath);
            sExplorerCtrl->doShChangeNotifyUpdateDir();
        }
    }
}
} // namespace cmd
