//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_file_ass.h"

#include "fxb/fxb_file_ass.h"

#include "cmd_context.h"
#include "FileAssDlg.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
static void executeFileAss(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aExplorerCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aExplorerCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sLvItemData))
        return;

    if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) ||
        XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        return;

    std::tstring sPath;
    fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

    fxb::FileAssItem *sFileAssItem;
    sFileAssItem = fxb::FileAssMgr::instance().getItemFromPath(sPath.c_str(), aType);
    if (XPR_IS_NULL(sFileAssItem))
        return;

    std::tstring sFile;
    std::tstring sParameters;

    fxb::GetEnvRealPath(sFileAssItem->mPath, sFile);
    sParameters = sPath;

    ::ShellExecute(aExplorerCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
}

static void executeFileAss(SearchResultCtrl &aSearchResultCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aSearchResultCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aSearchResultCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    LPSRITEMDATA sSrItemData = (LPSRITEMDATA)aSearchResultCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sSrItemData))
        return;

    if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return;

    std::tstring sPath;
    sSrItemData->getPath(sPath);

    fxb::FileAssItem *sFileAssItem;
    sFileAssItem = fxb::FileAssMgr::instance().getItemFromPath(sPath.c_str(), aType);
    if (XPR_IS_NULL(sFileAssItem))
        return;

    std::tstring sFile;
    std::tstring sParameters;

    fxb::GetEnvRealPath(sFileAssItem->mPath, sFile);
    sParameters = sPath;

    ::ShellExecute(aSearchResultCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
}

static void executeFileMultiAss(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aExplorerCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aExplorerCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sLvItemData))
        return;

    if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) ||
         XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        return;

    std::tstring sPath;
    fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

    fxb::FileAssDeque sFileAssDeque;
    if (fxb::FileAssMgr::instance().getItemFromPath(sPath.c_str(), aType, sFileAssDeque) == XPR_FALSE)
        return;

    if (sFileAssDeque.empty() == true)
        return;

    fxb::FileAssItem *sFileAssItem = XPR_NULL;

    if (sFileAssDeque.size() == 1)
        sFileAssItem = *sFileAssDeque.begin();
    else
    {
        FileAssDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setList(sFileAssDeque);
        if (sDlg.DoModal() == IDOK)
        {
            sFileAssItem = sDlg.getSelFileAssItem();
        }
    }

    if (XPR_IS_NOT_NULL(sFileAssItem))
    {
        std::tstring sFile;
        std::tstring sParameters;

        fxb::GetEnvRealPath(sFileAssItem->mPath, sFile);
        sParameters = sPath;

        ::ShellExecute(aExplorerCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
    }

    sFileAssDeque.clear();
}

static void executeFileMultiAss(SearchResultCtrl &aSearchResultCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aSearchResultCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aSearchResultCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    LPSRITEMDATA sSrItemData = (LPSRITEMDATA)aSearchResultCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sSrItemData))
        return;

    if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return;

    std::tstring sPath;
    sSrItemData->getPath(sPath);

    fxb::FileAssDeque sFileAssDeque;
    if (fxb::FileAssMgr::instance().getItemFromPath(sPath.c_str(), aType, sFileAssDeque) == XPR_FALSE)
        return;

    if (sFileAssDeque.empty() == true)
        return;

    fxb::FileAssItem *sFileAssItem = XPR_NULL;

    if (sFileAssDeque.size() == 1)
        sFileAssItem = *sFileAssDeque.begin();
    else
    {
        FileAssDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setList(sFileAssDeque);
        if (sDlg.DoModal() == IDOK)
        {
            sFileAssItem = sDlg.getSelFileAssItem();
        }
    }

    if (XPR_IS_NOT_NULL(sFileAssItem))
    {
        std::tstring sFile;
        std::tstring sParameters;

        fxb::GetEnvRealPath(sFileAssItem->mPath, sFile);
        sParameters = sPath;

        ::ShellExecute(aSearchResultCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
    }

    sFileAssDeque.clear();
}

xpr_sint_t FileViewCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileViewCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        executeFileAss(*sSearchResultCtrl, fxb::FileAssTypeViewer);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileAss(*sExplorerCtrl, fxb::FileAssTypeViewer);
    }
}

xpr_sint_t FileEditCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileEditCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        executeFileAss(*sSearchResultCtrl, fxb::FileAssTypeEditor);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileAss(*sExplorerCtrl, fxb::FileAssTypeEditor);
    }
}

xpr_sint_t FileViewerAssCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileViewerAssCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        executeFileMultiAss(*sSearchResultCtrl, fxb::FileAssTypeViewer);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileMultiAss(*sExplorerCtrl, fxb::FileAssTypeViewer);
    }
}

xpr_sint_t FileEditorAssCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileEditorAssCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        executeFileMultiAss(*sSearchResultCtrl, fxb::FileAssTypeEditor);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileMultiAss(*sExplorerCtrl, fxb::FileAssTypeEditor);
    }
}

xpr_sint_t FileShellAssCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = (sExplorerCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void FileShellAssCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelCount <= 0)
            return;

        xpr_sint_t sIndex = sSearchResultCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        LPSRITEMDATA sSrItemData = (LPSRITEMDATA)sSearchResultCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sSrItemData))
            return;

        if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            return;

        std::tstring sPath;
        sSrItemData->getPath(sPath);

        std::tstring sStartup = sPath;
        xpr_size_t sFind = sStartup.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != std::tstring::npos)
            sStartup = sStartup.substr(0, sFind);

        fxb::OpenAsFile(sPath.c_str(), sStartup.c_str());
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sSelCount = sExplorerCtrl->GetSelectedCount();
        if (sSelCount <= 0)
            return;

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            return;

        if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) ||
             XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
            return;

        std::tstring sPath;
        fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        std::tstring sStartup = sPath;
        xpr_size_t sFind = sStartup.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != std::tstring::npos)
            sStartup = sStartup.substr(0, sFind);

        fxb::OpenAsFile(sPath.c_str(), sStartup.c_str());
    }
}
} // namespace cmd
