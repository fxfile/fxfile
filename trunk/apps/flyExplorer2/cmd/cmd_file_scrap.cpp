//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_scrap.h"

#include "ExplorerView.h"
#include "FileScrapGrpDlg.h"
#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"
#include "FileScrapPane.h"

#include "cmd/InputDlg.h"

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
            fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
            xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

            sFileScrap.addItem(sGroupId, sTvItemData->mShellFolder, &sTvItemData->mPidl, 1);
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

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

        sFileScrap.addItem(sGroupId, sSource, sRCount);

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
            fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
            xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

            sFileScrap.addItem(sGroupId, sShellFolder, sPidl, sCount);
        }

        XPR_SAFE_DELETE_ARRAY(sPidl);
    }
}

xpr_sint_t FileScrapCurCopyCommand::canExecute(CommandContext &aContext)
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

void FileScrapCurCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sFolderCtrl->getCurPath(sPath);

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();
        sFileScrap.copyOperation(sGroupId, sPath);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        _tcscpy(sPath, sExplorerCtrl->getCurPath());

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();
        sFileScrap.copyOperation(sGroupId, sPath);
    }
}

xpr_sint_t FileScrapCurMoveCommand::canExecute(CommandContext &aContext)
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

void FileScrapCurMoveCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sFolderCtrl->getCurPath(sPath);

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();
        sFileScrap.moveOperation(sGroupId, sPath);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        _tcscpy(sPath, sExplorerCtrl->getCurPath());

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();
        sFileScrap.moveOperation(sGroupId, sPath);
    }
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND aHwnd, xpr_uint_t aMsg, LPARAM lParam, LPARAM aData)
{
    if (aMsg == BFFM_INITIALIZED)
        ::SendMessage(aHwnd, BFFM_SETSELECTION, XPR_FALSE, aData);

    return 0;
}

XPR_INLINE xpr_bool_t browse(FileScrapPane *aFileScrapPane, xpr_tchar_t *aTarget)
{
    xpr_bool_t sResult = XPR_FALSE;

    static LPITEMIDLIST sFullPidl = XPR_NULL;
    COM_FREE(sFullPidl);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = aFileScrapPane->GetSafeHwnd();
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("file_scrap.folder_browse.title"));
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sFullPidl;

    sFullPidl = SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        SHGetPathFromIDList(sFullPidl, aTarget);
        sResult = XPR_TRUE;
    }

    return sResult;
}

XPR_INLINE xpr_bool_t operate(MainFrame *aMainFrame, FileScrapPane *aFileScrapPane, xpr_sint_t aFunc)
{
    fxb::FileScrap::Group *sGroup = aFileScrapPane->getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    if (sFileScrap.isItemEmpty(sGroup->mGroupId) == XPR_TRUE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.no_file"));
        aMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    CWaitCursor sWaitCursor;
    if (aFunc == FO_DELETE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_delete_or_trash"));
        xpr_sint_t sMsgId = aMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (sMsgId != IDCANCEL)
        {
            if (sMsgId == IDYES) sFileScrap.deleteOperation(sGroup->mGroupId);
            else                 sFileScrap.trashOperation(sGroup->mGroupId);

            sResult = XPR_TRUE;
        }
    }
    else
    {
        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        if (browse(aFileScrapPane, sTarget) == XPR_TRUE)
        {
            if (aFunc == FO_COPY) sFileScrap.copyOperation(sGroup->mGroupId, sTarget);
            else                  sFileScrap.moveOperation(sGroup->mGroupId, sTarget);

            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

xpr_sint_t FileScrapCopyToCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapCopyToCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        operate(sMainFrame, sFileScrapPane, FO_COPY);
    }
}

xpr_sint_t FileScrapMoveToCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapMoveToCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        operate(sMainFrame, sFileScrapPane, FO_MOVE);
    }
}

xpr_sint_t FileScrapValidateCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapValidateCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NULL(sGroup))
            return;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_size_t sNotExistItemCount = sFileScrap.getItemCountNotExist(sGroup->mGroupId);
        if (sNotExistItemCount == 0)
            return;

        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("file_scrap.msg.some_invalidated"), XPR_STRING_LITERAL("%d")), sNotExistItemCount);
        xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
        if (sMsgId == IDYES)
        {
            xpr_size_t sRemoveItemCount = sFileScrap.removeItemNotExist(sGroup->mGroupId);
            if (sRemoveItemCount == 0)
            {
                const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.validated"));
                sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
                return;
            }

            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.removed_invalidated_items"));
            sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
        }
    }
}

xpr_sint_t FileScrapDeleteCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapDeleteCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        operate(sMainFrame, sFileScrapPane, FO_DELETE);
    }
    XPR_COMMAND_ELSE
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_delete_or_trash"));
        xpr_sint_t sMsgId = AfxGetMainWnd()->MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);

        if (sMsgId == IDYES)
        {
            sFileScrap.deleteOperation(sGroupId);
        }
        else if (sMsgId == IDNO)
        {
            sFileScrap.trashOperation(sGroupId);
        }
    }
}

xpr_sint_t FileScrapRemoveListCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getSelectedItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapRemoveListCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NULL(sGroup))
            return;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        fxb::FileScrap::IdSet sIdSet;
        sFileScrapPane->getSelFileScrapItemIdSet(sIdSet);

        if (sIdSet.empty() == false)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_clear"));
            xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDYES)
            {
                sFileScrap.removeItems(sGroup->mGroupId, sIdSet);
            }
        }
    }
}

xpr_sint_t FileScrapRemoveAllListCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sEnable = (sFileScrapPane->getItemCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
        sEnable = (sFileScrap.getItemCount(sFileScrap.getCurGroupId()) > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileScrapRemoveAllListCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NULL(sGroup))
            return;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        if (sFileScrap.isItemEmpty(sGroup->mGroupId) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_clear_all"));
            xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDYES)
            {
                sFileScrap.removeAllItems(sGroup->mGroupId);
            }
        }
    }
    XPR_COMMAND_ELSE
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

        if (sFileScrap.isItemEmpty(sGroupId) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_clear_all"));
            xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
            if (sMsgId == IDYES)
            {
                sFileScrap.removeAllItems(sGroupId);
            }
        }
    }
}

xpr_sint_t FileScrapRefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FileScrapRefreshCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sFileScrapPane->refresh();
    }
}

xpr_sint_t FileScrapViewCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FileScrapViewCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        sFileScrapPane->SetFocus();
    }
    else
    {
        ExplorerView *sExplorerView = sMainFrame->getExplorerView();
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            xpr_sint_t sTab = sExplorerView->newTab(TabTypeFileScrap);
            if (sTab >= 0)
                sExplorerView->setCurTab(sTab);
        }
    }
}

xpr_sint_t FileScrapGroupAddCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void FileScrapGroupAddCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        if (sFileScrap.getGroupCount() >= fxb::MAX_FILE_SCRAP_GROUP)
        {
            xpr_tchar_t sMsg[0xff] = {0};
            _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("file_scrap.msg.limit_group_count"), XPR_STRING_LITERAL("%d")), fxb::MAX_FILE_SCRAP_GROUP);
            sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            return;
        }

        InputDlg sDlg;
        sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("file_scrap.new_group.title")));
        sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("file_scrap.new_group.desc")));
        sDlg.setCheckEmpty();
        sDlg.setLimitText(fxb::MAX_FILE_SCRAP_GROUP_NAME);

        xpr_uint_t sGroupId = fxb::FileScrap::InvalidGroupId;
        const xpr_tchar_t *sText;

        while (true)
        {
            if (sDlg.DoModal() != IDOK)
                return;

            sText = sDlg.getText();

            if (fxb::VerifyFileName(sText) == XPR_FALSE)
            {
                const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.wrong_group_name"));
                sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                continue;
            }

            sGroupId = sFileScrap.addGroup(sText);
            if (sGroupId != fxb::FileScrap::InvalidGroupId)
                break;

            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.duplicated_group_name"));
            sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        }
    }
}

xpr_sint_t FileScrapGroupRenameCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NOT_NULL(sGroup))
        {
            fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
            sEnable = (sGroup->mGroupId != sFileScrap.getDefGroupId());
        }
    }

    return XPR_IS_TRUE(sEnable) ? StateEnable : StateDisable;
}

void FileScrapGroupRenameCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NULL(sGroup))
            return;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        InputDlg sDlg;
        sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("file_scrap.group_rename.title")));
        sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("file_scrap.group_rename.desc")));
        sDlg.setText(sGroup->mGroupName.c_str());
        sDlg.setCheckEmpty();
        sDlg.setLimitText(fxb::MAX_FILE_SCRAP_GROUP_NAME);

        xpr_uint_t sGroupId = fxb::FileScrap::InvalidGroupId;
        const xpr_tchar_t *sText;

        while (true)
        {
            if (sDlg.DoModal() != IDOK)
                return;

            sText = sDlg.getText();

            if (fxb::VerifyFileName(sText) == XPR_FALSE)
            {
                const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.wrong_group_name"));
                sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                continue;
            }

            if (_tcsicmp(sText, sGroup->mGroupName.c_str()) == 0)
                return;

            if (sFileScrap.findGroup(sText) == XPR_NULL)
                break;

            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.duplicated_group_name"));
            sMainFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        }

        sFileScrap.renameGroup(sGroup->mGroupName.c_str(), sText);
    }
}

xpr_sint_t FileScrapGroupDeleteCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_TRUE;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NOT_NULL(sGroup))
        {
            fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
            sEnable = (sGroup->mGroupId != sFileScrap.getDefGroupId());
        }
    }

    return XPR_IS_TRUE(sEnable) ? StateEnable : StateDisable;
}

void FileScrapGroupDeleteCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FILE_SCRAP_PANE
    {
        fxb::FileScrap::Group *sGroup = sFileScrapPane->getCurGroup();
        if (XPR_IS_NULL(sGroup))
            return;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("file_scrap.msg.confirm_delete"));
        xpr_sint_t sMsgId = sMainFrame->MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
        if (sMsgId != IDYES)
            return;

        fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

        sFileScrap.removeGroup(sGroup->mGroupId);
    }
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

    fxb::FileScrap::instance().setCurGroupId(sGroupId);
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
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.setClipboardCut(sGroupId);
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
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    xpr_uint_t sGroupId = sFileScrap.getCurGroupId();

    sFileScrap.setClipboardCopy(sGroupId);
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
