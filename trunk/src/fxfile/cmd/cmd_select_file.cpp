//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_select_file.h"

#include "fnmatch.h"
#include "sel_name_dlg.h"
#include "sel_filter_dlg.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
static void selectName(ExplorerCtrl &aExplorerCtrl, xpr_bool_t aSelect)
{
    SelNameDlg sDlg(aSelect);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_bool_t sOnlySel = sDlg.isOnlySel();
    const xpr_tchar_t *sSelName = sDlg.getSelName();
    xpr_tchar_t (*sPattern)[XPR_MAX_PATH + 1] = XPR_NULL;

    xpr_sint_t i = 0, j;
    xpr_sint_t sSepCount = 0;
    xpr_sint_t sCount = (xpr_sint_t)_tcslen(sSelName);
    for ( i =0; i < sCount; ++i)
    {
        if (sSelName[i] == XPR_STRING_LITERAL(';'))
            sSepCount++;
    }

    xpr_tchar_t *sLastExt = (xpr_tchar_t *)_tcsrchr(sSelName, XPR_STRING_LITERAL(';'));
    if (XPR_IS_NULL(sLastExt) && sSepCount == 0)
    {
        sSepCount++;
        sPattern = new xpr_tchar_t[1][XPR_MAX_PATH + 1];
        _tcscpy(sPattern[0], sSelName);
    }
    else
    {
        if (_tcscmp(sLastExt+1, XPR_STRING_LITERAL("")) != 0)
            sSepCount++;

        CString sName;
        sPattern = new xpr_tchar_t[sSepCount][XPR_MAX_PATH + 1];
        for (i = 0; i < sSepCount; ++i)
        {
            AfxExtractSubString(sName, sSelName, i, XPR_STRING_LITERAL(';'));
            sName.TrimLeft();
            sName.TrimRight();
            _tcscpy(sPattern[i], sName);
        }
    }

    if (sSepCount > 0)
    {
        xpr_bool_t sResult = XPR_FALSE;
        xpr_sint_t sFirst = -1;

        LPLVITEMDATA sLvItemData;
        xpr_tchar_t sName[XPR_MAX_PATH + 1];
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sCount = aExplorerCtrl.GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            sResult = XPR_FALSE;

            sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(i);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                if (sLvItemData->mShellAttributes & SFGAO_FILESYSTEM)
                {
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                    xpr_tchar_t *sSplit = _tcsrchr(sPath, XPR_STRING_LITERAL('\\'));
                    if (XPR_IS_NOT_NULL(sSplit)) _tcscpy(sName, sSplit + 1);
                    else                         _tcscpy(sName, sPath);
                }
                else
                {
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_INFOLDER, sName);
                }

                for (j = 0; j < sSepCount; ++j)
                {
                    if (fnmatch(sPattern[j], sName, FNM_CASEFOLD) != FNM_NOMATCH)
                    {
                        sResult = XPR_TRUE;
                        break;
                    }
                }
            }
            else
            {
                sResult = XPR_FALSE;
            }

            if (XPR_IS_TRUE(sResult))
            {
                if (XPR_IS_TRUE(aSelect) && sFirst == -1)
                {
                    aExplorerCtrl.SetSelectionMark(i);
                    aExplorerCtrl.SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
                    sFirst = i;
                }

                aExplorerCtrl.SetItemState(i, XPR_IS_TRUE(aSelect) ? LVIS_SELECTED : 0, LVIS_SELECTED);
            }
            else
            {
                if (XPR_IS_TRUE(sOnlySel) || XPR_IS_NULL(sLvItemData))
                    aExplorerCtrl.SetItemState(i, 0, LVIS_SELECTED);
            }
        }

        if (XPR_IS_TRUE(aSelect) && sFirst != -1)
            aExplorerCtrl.EnsureVisible(sFirst, XPR_FALSE);
    }

    XPR_SAFE_DELETE_ARRAY(sPattern);
}

static void selectName(SearchResultCtrl &aSearchResultCtrl, xpr_bool_t aSelect)
{
    SelNameDlg sDlg(aSelect);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_bool_t sOnlySel = sDlg.isOnlySel();
    const xpr_tchar_t *sSelName = sDlg.getSelName();
    xpr_tchar_t (*sPattern)[XPR_MAX_PATH + 1] = XPR_NULL;

    xpr_sint_t i = 0, j;
    xpr_sint_t sSepCount = 0;
    xpr_sint_t sCount = (xpr_sint_t)_tcslen(sSelName);
    for (i = 0; i < sCount; ++i)
    {
        if (sSelName[i] == ';')
            sSepCount++;
    }

    xpr_tchar_t *sLastExt = (xpr_tchar_t *)_tcsrchr(sSelName, ';');
    if (XPR_IS_NULL(sLastExt) && sSepCount == 0)
    {
        sSepCount++;
        sPattern = new xpr_tchar_t[1][XPR_MAX_PATH + 1];
        _tcscpy(sPattern[0], sSelName);
    }
    else
    {
        if (_tcscmp(sLastExt+1, XPR_STRING_LITERAL("")))
            sSepCount++;

        CString sName;
        sPattern = new xpr_tchar_t[sSepCount][XPR_MAX_PATH + 1];
        for (i = 0; i < sSepCount; ++i)
        {
            AfxExtractSubString(sName, sSelName, i, ';');
            sName.TrimLeft();
            sName.TrimRight();
            _tcscpy(sPattern[i], sName);
        }
    }

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sFirst = -1;
    SrItemData *sSrItemData = XPR_NULL;
    xpr_tchar_t sName[XPR_MAX_PATH + 1];

    sCount = aSearchResultCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(i);
        if (XPR_IS_NULL(sSrItemData))
            continue;

        _tcscpy(sName, sSrItemData->mFileName);

        sResult = XPR_FALSE;
        for (j = 0; j < sSepCount; ++j)
        {
            if (fnmatch(sPattern[j], sName, FNM_CASEFOLD) != FNM_NOMATCH)
            {
                sResult = XPR_TRUE;
                break;
            }
        }

        if (XPR_IS_TRUE(sResult))
        {
            aSearchResultCtrl.SetItemState(i, XPR_IS_TRUE(aSelect) ? LVIS_SELECTED : 0, LVIS_SELECTED);
            if (XPR_IS_TRUE(aSelect) && sFirst == -1)
            {
                aSearchResultCtrl.SetSelectionMark(i);
                aSearchResultCtrl.SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
                sFirst = i;
            }
        }
        else
        {
            if (XPR_IS_TRUE(sOnlySel) || XPR_IS_NULL(sSrItemData))
                aSearchResultCtrl.SetItemState(i, 0, LVIS_SELECTED);
        }
    }

    if (XPR_IS_TRUE(aSelect) && sFirst != -1)
        aSearchResultCtrl.EnsureVisible(sFirst, XPR_FALSE);

    XPR_SAFE_DELETE_ARRAY(sPattern);

    aSearchResultCtrl.SetFocus();
}

xpr_sint_t SelectNameCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectNameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectName(*sSearchResultCtrl, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectName(*sExplorerCtrl, XPR_TRUE);
    }
}

xpr_sint_t UnselectNameCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void UnselectNameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectName(*sSearchResultCtrl, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectName(*sExplorerCtrl, XPR_FALSE);
    }
}

xpr_sint_t SelectSameFilterCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectSameFilterCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        if (sSearchResultCtrl->GetSelectedCount() <= 0)
            return;

        xpr_sint_t sIndex = sSearchResultCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        xpr_sint_t i;
        xpr_sint_t sCount;
        SrItemData *sSrItemData;

        sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sSrItemData))
            return;

        if (!XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            FilterMgr &sFilterMgr = FilterMgr::instance();

            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            sSrItemData->getPath(sPath);

            xpr_sint_t sFilter;
            xpr_sint_t sSelFilter;

            sSelFilter = sFilterMgr.getIndex(sPath, XPR_FALSE);

            sCount = sSearchResultCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(i);
                if (XPR_IS_NULL(sSrItemData))
                    continue;

                if (!XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    sSrItemData->getPath(sPath);

                    sFilter = sFilterMgr.getIndex(sPath, XPR_FALSE);
                    if (sFilter == sSelFilter)
                        sSearchResultCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                }
            }
        }
        else if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            sCount = sSearchResultCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(i);
                if (XPR_IS_NULL(sSrItemData))
                    continue;

                if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                    sSearchResultCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        xpr_sint_t i;
        xpr_sint_t sCount;
        LPLVITEMDATA sLvItemData;

        sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            return;

        if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
            !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            FilterMgr &sFilterMgr = FilterMgr::instance();

            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            xpr_sint_t sFilter;
            xpr_sint_t sSelFilter;

            sSelFilter = sFilterMgr.getIndex(sPath, XPR_FALSE);

            sCount = sExplorerCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(i);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
                    !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                {
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                    sFilter = sFilterMgr.getIndex(sPath, XPR_FALSE);
                    if (sFilter == sSelFilter)
                        sExplorerCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                }
            }
        }
        else if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            sCount = sExplorerCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(i);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                    sExplorerCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
}

xpr_sint_t SelectSameExtCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectSameExtCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        if (sSearchResultCtrl->GetSelectedCount() <= 0)
            return;

        xpr_sint_t sIndex = sSearchResultCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        xpr_sint_t i;
        xpr_sint_t sCount;
        SrItemData *sSrItemData;

        sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sSrItemData))
            return;

        if (!XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            sSrItemData->getPath(sPath);

            const xpr_tchar_t *sExt = GetFileExt(sPath);
            if (XPR_IS_NULL(sExt))
                return;

            xpr_tchar_t sExt2[XPR_MAX_PATH + 1] = {0};
            _tcscpy(sExt2, sExt);

            sCount = sSearchResultCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(i);
                if (XPR_IS_NULL(sSrItemData))
                    continue;

                if (!XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    sSrItemData->getPath(sPath);

                    sExt = GetFileExt(sPath);
                    if (XPR_IS_NULL(sExt))
                        continue;

                    if (!_tcsicmp(sExt, sExt2))
                        sSearchResultCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                }
            }
        }
        else if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            sCount = sSearchResultCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(i);
                if (XPR_IS_NULL(sSrItemData))
                    continue;

                if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                    sSearchResultCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (sExplorerCtrl->GetSelectedCount() <= 0)
            return;

        xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
        if (sIndex < 0)
            return;

        xpr_sint_t i;
        xpr_sint_t sCount;
        LPLVITEMDATA sLvItemData;

        sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            return;

        if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
            !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            const xpr_tchar_t *sExt = GetFileExt(sPath);
            if (XPR_IS_NULL(sExt))
                return;

            xpr_tchar_t sExt2[XPR_MAX_PATH + 1] = {0};
            _tcscpy(sExt2, sExt);

            sCount = sExplorerCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(i);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                if ( XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
                    !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                {
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                    sExt = GetFileExt(sPath);
                    if (XPR_IS_NULL(sExt))
                        continue;

                    if (_tcsicmp(sExt, sExt2) == 0)
                        sExplorerCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                }
            }
        }
        else if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            sCount = sExplorerCtrl->GetItemCount();
            for (i = 0; i < sCount; ++i)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(i);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                    sExplorerCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
}

static void selectFile(ExplorerCtrl &aExplorerCtrl, xpr_bool_t aFile)
{
    aExplorerCtrl.unselectAll();

    xpr_bool_t sSel;
    xpr_sint_t i, sCount;
    LPLVITEMDATA sLvItemData;

    sCount = aExplorerCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        sSel = XPR_FALSE;

        if (XPR_IS_TRUE(aFile))
            sSel = ((sLvItemData->mShellAttributes & SFGAO_FILESYSTEM) && ((sLvItemData->mShellAttributes & SFGAO_FOLDER) != SFGAO_FOLDER));
        else
            sSel = ((sLvItemData->mShellAttributes & SFGAO_FOLDER) == SFGAO_FOLDER);

        if (XPR_IS_TRUE(sSel))
        {
            aExplorerCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

static void selectFile(SearchResultCtrl &aSearchResultCtrl, xpr_bool_t aFile)
{
    aSearchResultCtrl.unselectAll();

    xpr_bool_t sSelect;
    xpr_sint_t i, sCount;
    SrItemData *sSrItemData;

    sCount = aSearchResultCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(i);
        if (sSrItemData == XPR_NULL)
            continue;

        sSelect = XPR_FALSE;

        if (aFile == XPR_TRUE)
            sSelect = XPR_TEST_NONE_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
        else
            sSelect = XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

        if (sSelect == XPR_TRUE)
        {
            aSearchResultCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}

static void unselectFile(ExplorerCtrl &aExplorerCtrl, xpr_bool_t aFile)
{
    xpr_sint_t i;
    xpr_bool_t sUnsel;
    LPLVITEMDATA sLvItemData;

    POSITION sPosition = aExplorerCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        i = aExplorerCtrl.GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        sUnsel = XPR_FALSE;

        if (XPR_IS_TRUE(aFile))
            sUnsel = ((sLvItemData->mShellAttributes & SFGAO_FILESYSTEM) && ((sLvItemData->mShellAttributes & SFGAO_FOLDER) != SFGAO_FOLDER));
        else
            sUnsel = ((sLvItemData->mShellAttributes & SFGAO_FOLDER) == SFGAO_FOLDER);

        if (XPR_IS_TRUE(sUnsel))
        {
            aExplorerCtrl.SetItemState(i, 0, LVIS_SELECTED);
        }
    }
}

static void unselectFile(SearchResultCtrl &aSearchResultCtrl, xpr_bool_t aFile)
{
    xpr_sint_t i;
    xpr_bool_t sUnselect;
    SrItemData *sSrItemData;

    POSITION sPosition = aSearchResultCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        i = aSearchResultCtrl.GetNextSelectedItem(sPosition);

        sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(i);
        if (sSrItemData == XPR_NULL)
            continue;

        sUnselect = XPR_FALSE;

        if (aFile == XPR_TRUE)
            sUnselect = XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);
        else
            sUnselect = XPR_TEST_NONE_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

        if (sUnselect == XPR_TRUE)
        {
            aSearchResultCtrl.SetItemState(i, 0, LVIS_SELECTED);
        }
    }
}

xpr_sint_t SelectFileCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectFile(*sSearchResultCtrl, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectFile(*sExplorerCtrl, XPR_TRUE);
    }
}

xpr_sint_t SelectFolderCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectFile(*sSearchResultCtrl, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectFile(*sExplorerCtrl, XPR_FALSE);
    }
}

xpr_sint_t UnselectFileCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void UnselectFileCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        unselectFile(*sSearchResultCtrl, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        unselectFile(*sExplorerCtrl, XPR_TRUE);
    }
}

xpr_sint_t UnselectFolderCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void UnselectFolderCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        unselectFile(*sSearchResultCtrl, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        unselectFile(*sExplorerCtrl, XPR_FALSE);
    }
}

static void selectFilter(ExplorerCtrl &aExplorerCtrl, xpr_bool_t aSelect)
{
    SelFilterDlg sDlg(aSelect);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_bool_t sOnlySel = sDlg.isOnlySel();
    FilterItem *sFilterItem = sDlg.getSelFilterItem();

    if (XPR_IS_NULL(sFilterItem))
        return;

    xpr_bool_t sResult = XPR_FALSE;
    xpr_sint_t sFirst = -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPLVITEMDATA sLvItemData;
    xpr::tstring sPath;
    const xpr_tchar_t *sExt;

    sCount = aExplorerCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sResult = XPR_FALSE;

        sLvItemData = (LPLVITEMDATA)aExplorerCtrl.GetItemData(i);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                {
                    sResult = !_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*"));
                }
                else
                {
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
                    sExt = GetFileExt(sPath);

                    if (XPR_IS_NOT_NULL(sExt))
                        sResult = IsEqualFilter(sFilterItem->mExts.c_str(), sExt);
                    else
                        sResult = !_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*.*"));
                }
            }
        }

        if (XPR_IS_TRUE(sResult))
        {
            if (XPR_IS_TRUE(aSelect) && sFirst == -1)
            {
                aExplorerCtrl.SetSelectionMark(i);
                aExplorerCtrl.SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
                sFirst = i;
            }

            aExplorerCtrl.SetItemState(i, XPR_IS_TRUE(aSelect) ? LVIS_SELECTED : 0, LVIS_SELECTED);
        }
        else
        {
            if (XPR_IS_TRUE(sOnlySel) || XPR_IS_NULL(sLvItemData))
                aExplorerCtrl.SetItemState(i, 0, LVIS_SELECTED);
        }
    }

    if (XPR_IS_TRUE(aSelect) && sFirst != -1)
        aExplorerCtrl.EnsureVisible(sFirst, XPR_FALSE);
}

static void selectFilter(SearchResultCtrl &aSearchResultCtrl, xpr_bool_t aSelect)
{
    SelFilterDlg sDlg(aSelect);
    if (sDlg.DoModal() != IDOK)
        return;

    xpr_bool_t sOnlySel = sDlg.isOnlySel();
    FilterItem *sFilterItem = sDlg.getSelFilterItem();

    if (XPR_IS_NULL(sFilterItem))
        return;

    xpr_bool_t sResult = XPR_FALSE;
    xpr_sint_t sFirst = -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    SrItemData *sSrItemData;
    xpr::tstring sPath;
    const xpr_tchar_t *sExt;

    sCount = aSearchResultCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sResult = XPR_FALSE;

        sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(i);
        if (XPR_IS_NOT_NULL(sSrItemData))
        {
            if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = !_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*"));
            }
            else
            {
                sSrItemData->getPath(sPath);
                sExt = GetFileExt(sPath);

                if (XPR_IS_NOT_NULL(sExt))
                    sResult = IsEqualFilter(sFilterItem->mExts.c_str(), sExt);
                else
                    sResult = !_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*.*"));
            }
        }

        if (XPR_IS_TRUE(sResult))
        {
            if (XPR_IS_TRUE(aSelect) && sFirst == -1)
            {
                aSearchResultCtrl.SetSelectionMark(i);
                aSearchResultCtrl.SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
                sFirst = i;
            }

            aSearchResultCtrl.SetItemState(i, XPR_IS_TRUE(aSelect) ? LVIS_SELECTED : 0, LVIS_SELECTED);
        }
        else
        {
            if (XPR_IS_TRUE(sOnlySel) || XPR_IS_NOT_NULL(sSrItemData))
                aSearchResultCtrl.SetItemState(i, 0, LVIS_SELECTED);
        }
    }

    if (XPR_IS_TRUE(aSelect) && sFirst != -1)
        aSearchResultCtrl.EnsureVisible(sFirst, XPR_FALSE);
}

xpr_sint_t SelectFilterCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectFilterCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectFilter(*sSearchResultCtrl, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectFilter(*sExplorerCtrl, XPR_TRUE);
    }
}

xpr_sint_t UnselectFilterCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void UnselectFilterCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        selectFilter(*sSearchResultCtrl, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        selectFilter(*sExplorerCtrl, XPR_FALSE);
    }
}

xpr_sint_t SelectAllCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectAllCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->SetFocus();
        sSearchResultCtrl->selectAll();
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sExplorerCtrl->SetFocus();
        sExplorerCtrl->selectAll();
    }
}

xpr_sint_t UnselectAllCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void UnselectAllCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        sSearchResultCtrl->SetFocus();
        sSearchResultCtrl->unselectAll();
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sExplorerCtrl->SetFocus();
        sExplorerCtrl->unselectAll();
    }
}

xpr_sint_t SelectReversalCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void SelectReversalCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetItemCount();

        xpr_sint_t i;
        xpr_uint_t sState, sMask = LVIS_SELECTED;
        for (i = 0; i < sCount; ++i)
        {
            if (sSearchResultCtrl->GetItemState(i, sMask) & LVIS_SELECTED)
                sState = 0;
            else
                sState = LVIS_SELECTED;

            sSearchResultCtrl->SetItemState(i, sState, sMask);
        }
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sExplorerCtrl->SetFocus();

        xpr_sint_t i;
        xpr_sint_t sCount = sExplorerCtrl->GetItemCount();
        xpr_uint_t sState, sMask = LVIS_SELECTED;
        for (i = 0; i < sCount; ++i)
        {
            if (sExplorerCtrl->GetItemState(i, sMask) & LVIS_SELECTED)
                sState = 0;
            else
                sState = LVIS_SELECTED;

            sExplorerCtrl->SetItemState(i, sState, sMask);
        }
    }
}
} // namespace cmd
} // namespace fxfile
