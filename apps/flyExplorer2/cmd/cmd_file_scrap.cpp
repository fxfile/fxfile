//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_scrap.h"

#include "fxb/fxb_file_scrap_mgr.h"

#include "FileScrapDlg.h"
#include "FileScrapGrpDlg.h"
#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t FileScrapAddCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        if (sFolderCtrl->hasSelShellAttributes(SFGAO_CANCOPY)   == XPR_TRUE ||
            sFolderCtrl->hasSelShellAttributes(SFGAO_CANMOVE)   == XPR_TRUE ||
            sFolderCtrl->hasSelShellAttributes(SFGAO_CANDELETE) == XPR_TRUE )
        {
            sEnable = XPR_TRUE;
        }
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (sExplorerCtrl->hasSelShellAttributes(SFGAO_CANCOPY)   == XPR_TRUE ||
            sExplorerCtrl->hasSelShellAttributes(SFGAO_CANMOVE)   == XPR_TRUE ||
            sExplorerCtrl->hasSelShellAttributes(SFGAO_CANDELETE) == XPR_TRUE)
        {
            sEnable = XPR_TRUE;
        }
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapAddCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            fxb::FileScrapMgr::instance().add(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1);
        }
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_tchar_t *sSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sCount + 1];
        xpr_sint_t sRCount = 0, sPos = 0, sIndex;
        xpr_tchar_t szPath[XPR_MAX_PATH + 1] = {0};
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(szPath);

                _tcscpy(sSource + sPos, szPath);
                sPos += (xpr_sint_t)_tcslen(szPath) + 1;
                sRCount++;
            }
        }

        fxb::FileScrapMgr::instance().addPath(sSource, sRCount);

        XPR_SAFE_DELETE_ARRAY(sSource);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPITEMIDLIST *sPidl = XPR_NULL;
        xpr_sint_t sCount = 0;

        sExplorerCtrl->getSelItemData(&sShellFolder, &sPidl, sCount);
        if (sShellFolder != XPR_NULL && sPidl != XPR_NULL && sCount > 0)
        {
            fxb::FileScrapMgr::instance().add(sShellFolder, sPidl, sCount);
        }

        XPR_SAFE_DELETE_ARRAY(sPidl);
    }
}

xpr_sint_t FileScrapToCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapToCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sFolderCtrl->getCurPath(sPath);

        fxb::FileScrapMgr::instance().doCopyOperation(sPath);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        _tcscpy(sPath, sExplorerCtrl->getCurPath());

        fxb::FileScrapMgr::instance().doCopyOperation(sPath);
    }
}

xpr_sint_t FileScrapToMoveCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapToMoveCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sFolderCtrl->getCurPath(sPath);

        fxb::FileScrapMgr::instance().doMoveOperation(sPath);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        _tcscpy(sPath, sExplorerCtrl->getCurPath());

        fxb::FileScrapMgr::instance().doMoveOperation(sPath);
    }
}

xpr_sint_t FileScrapDeleteCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapDeleteCommand::execute(CommandContext &aContext)
{
    fxb::FileScrapMgr::instance().doDeleteOperation();
}

xpr_sint_t FileScrapListRemoveCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapListRemoveCommand::execute(CommandContext &aContext)
{
    fxb::FileScrapMgr::instance().removeList();
}

xpr_sint_t FileScrapViewCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FileScrapViewCommand::execute(CommandContext &aContext)
{
    FileScrapDlg sDlg;
    sDlg.DoModal();
}

xpr_sint_t FileScrapGroupDefaultCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FileScrapGroupDefaultCommand::execute(CommandContext &aContext)
{
    FileScrapGrpDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap_default_group_set.title")));
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_uint_t sGroupId = sDlg.getSelGroupId();
    if (sGroupId == fxb::FileScrap::InvalidGroupId)
        return;

    fxb::FileScrap::instance().setCurGourpId(sGroupId);
}

xpr_sint_t FileScrapClipboardCutCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapClipboardCutCommand::execute(CommandContext &aContext)
{
    fxb::FileScrapMgr::instance().cutToClipboard();
}

xpr_sint_t FileScrapClipboardCopyCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sEnable = XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapClipboardCopyCommand::execute(CommandContext &aContext)
{
    fxb::FileScrapMgr::instance().copyToClipboard();
}

xpr_sint_t FileScrapDropCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t sState = StateEnable;

    if (sMainFrame->isShowFileScrapDrop())
        sState |= StateCheck;

    return sState;
}

void FileScrapDropCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->toggleFileScrapDrop();
}
} // namespace cmd
