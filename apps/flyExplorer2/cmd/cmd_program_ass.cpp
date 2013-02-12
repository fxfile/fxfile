//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_program_ass.h"

#include "fxb/fxb_program_ass.h"

#include "cmd_context.h"
#include "ProgramAssDlg.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
static void executeProgramAss(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aType)
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

    fxb::ProgramAssItem *sProgramAssItem;
    sProgramAssItem = fxb::ProgramAssMgr::instance().getItemFromPath(sPath.c_str(), aType);
    if (XPR_IS_NULL(sProgramAssItem))
        return;

    std::tstring sFile;
    std::tstring sParameters;

    fxb::GetEnvRealPath(sProgramAssItem->mPath, sFile);
    sParameters = sPath;

    ::ShellExecute(aExplorerCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
}

static void executeProgramAss(SearchResultCtrl &aSearchResultCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aSearchResultCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aSearchResultCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    SrItemData *sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sSrItemData))
        return;

    if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return;

    std::tstring sPath;
    sSrItemData->getPath(sPath);

    fxb::ProgramAssItem *sProgramAssItem;
    sProgramAssItem = fxb::ProgramAssMgr::instance().getItemFromPath(sPath.c_str(), aType);
    if (XPR_IS_NULL(sProgramAssItem))
        return;

    std::tstring sFile;
    std::tstring sParameters;

    fxb::GetEnvRealPath(sProgramAssItem->mPath, sFile);
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

    fxb::ProgramAssDeque sProgramAssDeque;
    if (fxb::ProgramAssMgr::instance().getItemFromPath(sPath.c_str(), aType, sProgramAssDeque) == XPR_FALSE)
        return;

    if (sProgramAssDeque.empty() == true)
        return;

    fxb::ProgramAssItem *sProgramAssItem = XPR_NULL;

    if (sProgramAssDeque.size() == 1)
        sProgramAssItem = *sProgramAssDeque.begin();
    else
    {
        ProgramAssDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setList(sProgramAssDeque);
        if (sDlg.DoModal() == IDOK)
        {
            sProgramAssItem = sDlg.getSelProgramAssItem();
        }
    }

    if (XPR_IS_NOT_NULL(sProgramAssItem))
    {
        std::tstring sFile;
        std::tstring sParameters;

        fxb::GetEnvRealPath(sProgramAssItem->mPath, sFile);
        sParameters = sPath;

        ::ShellExecute(aExplorerCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
    }

    sProgramAssDeque.clear();
}

static void executeFileMultiAss(SearchResultCtrl &aSearchResultCtrl, xpr_sint_t aType)
{
    xpr_sint_t sSelCount = aSearchResultCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = aSearchResultCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    SrItemData *sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sSrItemData))
        return;

    if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return;

    std::tstring sPath;
    sSrItemData->getPath(sPath);

    fxb::ProgramAssDeque sProgramAssDeque;
    if (fxb::ProgramAssMgr::instance().getItemFromPath(sPath.c_str(), aType, sProgramAssDeque) == XPR_FALSE)
        return;

    if (sProgramAssDeque.empty() == true)
        return;

    fxb::ProgramAssItem *sProgramAssItem = XPR_NULL;

    if (sProgramAssDeque.size() == 1)
        sProgramAssItem = *sProgramAssDeque.begin();
    else
    {
        ProgramAssDlg sDlg;
        sDlg.setPath(sPath);
        sDlg.setList(sProgramAssDeque);
        if (sDlg.DoModal() == IDOK)
        {
            sProgramAssItem = sDlg.getSelProgramAssItem();
        }
    }

    if (XPR_IS_NOT_NULL(sProgramAssItem))
    {
        std::tstring sFile;
        std::tstring sParameters;

        fxb::GetEnvRealPath(sProgramAssItem->mPath, sFile);
        sParameters = sPath;

        ::ShellExecute(aSearchResultCtrl.m_hWnd, XPR_STRING_LITERAL("open"), sFile.c_str(), sParameters.c_str(), XPR_NULL, SW_SHOW);
    }

    sProgramAssDeque.clear();
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
        executeProgramAss(*sSearchResultCtrl, fxb::ProgramAssTypeViewer);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeProgramAss(*sExplorerCtrl, fxb::ProgramAssTypeViewer);
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
        executeProgramAss(*sSearchResultCtrl, fxb::ProgramAssTypeEditor);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeProgramAss(*sExplorerCtrl, fxb::ProgramAssTypeEditor);
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
        executeFileMultiAss(*sSearchResultCtrl, fxb::ProgramAssTypeViewer);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileMultiAss(*sExplorerCtrl, fxb::ProgramAssTypeViewer);
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
        executeFileMultiAss(*sSearchResultCtrl, fxb::ProgramAssTypeEditor);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        executeFileMultiAss(*sExplorerCtrl, fxb::ProgramAssTypeEditor);
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

        SrItemData *sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
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
