//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_rename.h"

#include "functors.h"
#include "option.h"
#include "single_rename_dlg.h"
#include "batch_rename_dlg.h"
#include "folder_ctrl.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"

#include "router/cmd_parameters.h"
#include "router/cmd_parameter_define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
xpr_sint_t RenameCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (sTreeItem != XPR_NULL)
        {
            LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_CANRENAME))
                    sEnable = XPR_TRUE;
            }
        }
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sSelectedCount = sExplorerCtrl->GetSelectedCount();
        if (sSelectedCount == 1)
        {
            xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();

            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if ((XPR_IS_NULL(sLvItemData)) || (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_CANRENAME)))
                sEnable = XPR_TRUE;
        }
        else if (sSelectedCount > 1)
        {
            sEnable = XPR_TRUE;
        }
        else
        {
            sEnable = XPR_TRUE;
        }
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

static void doBatchRename(FolderCtrl &aFolderCtrl, xpr_bool_t aWithFile, xpr_bool_t aWithFolder, FileSysItem *aFileSysItem = XPR_NULL, LPITEMIDLIST aParentFullPidl = XPR_NULL)
{
    FileSysItem sFileSysItem;

    if (aFileSysItem == XPR_NULL)
    {
        if (aWithFolder == XPR_FALSE)
            return;

        aFileSysItem = &sFileSysItem;

        HTREEITEM sTreeItem;
        sTreeItem = aFolderCtrl.GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)aFolderCtrl.GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM) &&
                XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_CANRENAME))
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);

                sFileSysItem.mPath           = sPath;
                sFileSysItem.mFileAttributes = sTvItemData->mFileAttributes;
            }
        }
    }

    if (aFileSysItem != XPR_NULL)
    {
        xpr::string sPath = aFileSysItem->mPath;
        xpr_bool_t sFolder = XPR_TEST_BITS(aFileSysItem->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

        xpr_size_t sOffset = sPath.rfind(XPR_STRING_LITERAL('\\'));
        if (sOffset != xpr::string::npos)
        {
            BatchRename::Item *sBatchRenameItem = new BatchRename::Item;
            sBatchRenameItem->mDir    = sPath.substr(0, sOffset);
            sBatchRenameItem->mOld    = sPath.substr(sOffset + 1);
            sBatchRenameItem->mNew    = sBatchRenameItem->mOld;
            sBatchRenameItem->mFolder = sFolder;

            BatchRenameDlg sDlg;
            sDlg.addPath(sBatchRenameItem);
            xpr_sintptr_t sId = sDlg.DoModal();
            if (sId == IDOK)
            {
                ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST | SHCNF_FLUSH, (LPVOID)aParentFullPidl, XPR_NULL);
            }
        }
    }
}

static void doBatchRename(ExplorerCtrl &aExplorerCtrl, xpr_bool_t aWithFile, xpr_bool_t aWithFolder, FileSysItemDeque *aFileSysItemList = XPR_NULL)
{
    if (aWithFile == XPR_FALSE && aWithFolder == XPR_FALSE)
        return;

    FileSysItemDeque sItemList;

    if (aFileSysItemList == XPR_NULL)
    {
        aFileSysItemList = &sItemList;

        xpr_ulong_t sMask = SFGAO_FILESYSTEM;
        xpr_ulong_t sUnmask = 0;

        if (aWithFile == XPR_TRUE && aWithFolder == XPR_FALSE)
            sUnmask |= SFGAO_FOLDER;
        else if (aWithFile == XPR_FALSE && aWithFolder == XPR_TRUE)
            sMask |= SFGAO_FOLDER;

        xpr_sint_t sSelCount = aExplorerCtrl.GetSelectedCount();
        if (sSelCount >= 1)
        {
            aExplorerCtrl.getSelFileSysItems(sItemList, XPR_FALSE, sMask, sUnmask);
        }

        if (sItemList.empty() == XPR_TRUE)
        {
            // If you select [..] or nothing, then all of folders and files will be renamed.
            aExplorerCtrl.getAllFileSysItems(sItemList, XPR_FALSE, sMask, sUnmask);
        }
    }

    if (aFileSysItemList != XPR_NULL && aFileSysItemList->empty() == XPR_FALSE)
    {
        xpr::string sPath;
        xpr_bool_t sFolder;
        BatchRename::Item *sBatchRenameItem;
        BatchRenameDlg sDlg;

        FileSysItemDeque::const_iterator sIterator = aFileSysItemList->begin();
        for (; sIterator != aFileSysItemList->end(); ++sIterator)
        {
            const FileSysItem *sFileSysItem = *sIterator;

            sPath = sFileSysItem->mPath;
            sFolder = XPR_TEST_BITS(sFileSysItem->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

            xpr_size_t sOffset = sPath.rfind(XPR_STRING_LITERAL('\\'));
            if (sOffset == xpr::string::npos)
                return;

            sBatchRenameItem = new BatchRename::Item;
            sBatchRenameItem->mDir    = sPath.substr(0, sOffset);
            sBatchRenameItem->mOld    = sPath.substr(sOffset + 1);
            sBatchRenameItem->mNew    = sBatchRenameItem->mOld;
            sBatchRenameItem->mFolder = sFolder;

            sDlg.addPath(sBatchRenameItem);
        }

        clear(*aFileSysItemList);

        aExplorerCtrl.enableChangeNotify(XPR_FALSE);

        xpr_sintptr_t sId = sDlg.DoModal();

        aExplorerCtrl.enableChangeNotify(XPR_TRUE);

        if (sId == IDOK)
        {
            aExplorerCtrl.refresh();
        }
    }
}

static void doRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem = XPR_NULL)
{
    if (aTreeItem == XPR_NULL)
        aTreeItem = aFolderCtrl.GetSelectedItem();

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)aFolderCtrl.GetItemData(aTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return;

    if (XPR_TEST_NONE_BITS(sTvItemData->mShellAttributes, SFGAO_CANRENAME))
        return;

    xpr_bool_t sRenamed = XPR_FALSE;

    if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BY_POPUP)
    {
        SingleRenameDlg sDlg;
        sDlg.setItem(sTvItemData->mShellFolder, sTvItemData->mPidl, sTvItemData->mFullPidl, sTvItemData->mShellAttributes);
        sDlg.DoModal();

        sRenamed = XPR_TRUE;
    }
    else if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BATCH_RENAME)
    {
        if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);

            FileSysItem sFileSysItem;
            sFileSysItem.mPath           = sPath;
            sFileSysItem.mFileAttributes = sTvItemData->mFileAttributes;

            doBatchRename(aFolderCtrl, XPR_TRUE, XPR_TRUE, &sFileSysItem, sTvItemData->mFullPidl);

            sRenamed = XPR_TRUE;
        }
    }

    if (sRenamed == XPR_FALSE)
    {
        aFolderCtrl.renameDirectly(aTreeItem);
    }
}

static void doBatchRename(SearchResultCtrl &mSearchResultCtrl, FileSysItemDeque &aFileSysItemList)
{
    xpr::string sPath;
    xpr_bool_t sFolder;
    BatchRename::Item *sBatchRenameItem;
    BatchRenameDlg sDlg;

    FileSysItemDeque::const_iterator sIterator = aFileSysItemList.begin();
    for (; sIterator != aFileSysItemList.end(); ++sIterator)
    {
        const FileSysItem *sFileSysItem = *sIterator;

        sPath = sFileSysItem->mPath;
        sFolder = XPR_TEST_BITS(sFileSysItem->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

        xpr_size_t sOffset = sPath.rfind(XPR_STRING_LITERAL('\\'));
        if (sOffset == xpr::string::npos)
            return;

        sBatchRenameItem = new BatchRename::Item;
        sBatchRenameItem->mDir    = sPath.substr(0, sOffset);
        sBatchRenameItem->mOld    = sPath.substr(sOffset + 1);
        sBatchRenameItem->mNew    = sBatchRenameItem->mOld;
        sBatchRenameItem->mFolder = sFolder;

        sDlg.addPath(sBatchRenameItem);
    }

    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId == IDOK)
    {
        mSearchResultCtrl.refresh();

        mSearchResultCtrl.SetFocus();
    }
}

static void doBatchRename(SearchResultCtrl &aSearchResultCtrl, xpr_bool_t aWithFile = XPR_TRUE, xpr_bool_t aWithFolder = XPR_TRUE)
{
    FileSysItemDeque sFileSysItemDeque;

    xpr_sint_t sIndex;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    FileSysItem *sFileSysItem;
    SrItemData *sSrItemData = XPR_NULL;
    DWORD sFileAttributes;
    xpr_bool_t sFolder;

    POSITION sPosition = aSearchResultCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = aSearchResultCtrl.GetNextSelectedItem(sPosition);

        sSrItemData = (SrItemData *)aSearchResultCtrl.GetItemData(sIndex);
        if (XPR_IS_NULL(sSrItemData))
            continue;

        sSrItemData->getPath(sPath);
        if (IsEqualFileExt(sPath, XPR_STRING_LITERAL(".lnk")) == XPR_TRUE)
            continue;

        sFileAttributes = ::GetFileAttributes(sPath);
        sFolder = XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY);

        if (aWithFolder == XPR_FALSE && sFolder == XPR_TRUE)
            continue;

        if (aWithFile == XPR_FALSE   && sFolder == XPR_FALSE)
            continue;

        sFileSysItem = new FileSysItem;
        sFileSysItem->mPath           = sPath;
        sFileSysItem->mFileAttributes = sFileAttributes;
        sFileSysItemDeque.push_back(sFileSysItem);
    }

    if (sFileSysItemDeque.empty() == XPR_TRUE)
        return;

    doBatchRename(aSearchResultCtrl, sFileSysItemDeque);

    clear(sFileSysItemDeque);
}

void RenameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = XPR_NULL;
        if (sParameters != XPR_NULL)
            sTreeItem = (HTREEITEM)sParameters->get(CommandParameterIdTreeSelectedItem);

        doRename(*sFolderCtrl, sTreeItem);
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelCount <= 0)
            return;

        if (sSelCount > 1 && XPR_IS_TRUE(gOpt->mConfig.mBatchRenameMultiSel))
        {
            doBatchRename(*sSearchResultCtrl, XPR_TRUE, gOpt->mConfig.mBatchRenameWithFolder);
        }
        else
        {
            xpr_sint_t sIndex = sSearchResultCtrl->GetSelectionMark();

            xpr_bool_t sResult = XPR_FALSE;

            if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BY_POPUP)
            {
                SrItemData *sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sSrItemData))
                {
                    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                    sSrItemData->getPath(sPath);

                    LPITEMIDLIST sFullPidl;
                    sFullPidl = fxfile::base::Pidl::create(sPath);
                    if (XPR_IS_NOT_NULL(sFullPidl))
                    {
                        xpr_bool_t    sSuccess;
                        LPSHELLFOLDER sShellFolder = XPR_NULL;
                        LPCITEMIDLIST sPidl        = XPR_NULL;

                        sSuccess = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
                        if (XPR_IS_TRUE(sSuccess) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
                        {
                            xpr_ulong_t sShellAttributes = 0;
                            sShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&sPidl, &sShellAttributes);

                            SingleRenameDlg sDlg;
                            sDlg.setItem(sShellFolder, sPidl, sFullPidl, sShellAttributes);
                            sDlg.DoModal();
                        }

                        COM_RELEASE(sShellFolder);
                        COM_FREE(sFullPidl);
                    }

                    sResult = XPR_TRUE;
                }
            }
            else if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BATCH_RENAME)
            {
                doBatchRename(*sSearchResultCtrl, XPR_TRUE, gOpt->mConfig.mBatchRenameMultiSel);
                sResult = XPR_TRUE;
            }

            if (XPR_IS_FALSE(sResult))
            {
                sSearchResultCtrl->renameDirectly(sIndex);
            }
        }
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (sExplorerCtrl->isFileSystemFolder() == XPR_TRUE && gOpt->mConfig.mBatchRenameMultiSel == XPR_TRUE)
        {
            LPLVITEMDATA sLvItemData = XPR_NULL;
            xpr_sint_t sSelIndex = sExplorerCtrl->GetSelectionMark();
            xpr_sint_t sSelCount = sExplorerCtrl->GetSelectedCount();
            if (sSelCount == 1)
            {
                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sSelIndex);
                if ((XPR_IS_NULL(sLvItemData)) || (XPR_IS_NOT_NULL(sLvItemData) && !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_CANRENAME)))
                    sSelCount = 0;
            }

            FileSysItemDeque sFileSysItemDeque;
            xpr_ulong_t sMask = SFGAO_FILESYSTEM;
            xpr_ulong_t sUnmask = (gOpt->mConfig.mBatchRenameWithFolder == XPR_TRUE) ? 0 : SFGAO_FOLDER;

            if (sSelCount > 1)
            {
                sExplorerCtrl->getSelFileSysItems(sFileSysItemDeque, XPR_FALSE, sMask, sUnmask);
            }
            else if (sSelCount <= 0)
            {
                // If you select [..] or nothing, then all of folders and files will be renamed.
                sExplorerCtrl->getAllFileSysItems(sFileSysItemDeque, XPR_FALSE, sMask, sUnmask);
            }

            if (sFileSysItemDeque.empty() == XPR_FALSE)
            {
                doBatchRename(*sExplorerCtrl, XPR_TRUE, XPR_TRUE, &sFileSysItemDeque);

                clear(sFileSysItemDeque);
                return;
            }
        }

        xpr_bool_t sRenamed = XPR_FALSE;
        xpr_sint_t sSelIndex = sExplorerCtrl->GetSelectionMark();

        if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BY_POPUP)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sSelIndex);
            if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_CANRENAME))
            {
                LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(sExplorerCtrl->getFolderData()->mFullPidl, sLvItemData->mPidl);

                SingleRenameDlg sDlg;
                sDlg.setItem(sLvItemData->mShellFolder, sLvItemData->mPidl, sFullPidl, sLvItemData->mShellAttributes);
                sDlg.DoModal();

                COM_FREE(sFullPidl);

                sRenamed = XPR_TRUE;
            }
        }
        else if (gOpt->mConfig.mSingleRenameType == SINGLE_RENAME_TYPE_BATCH_RENAME)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sSelIndex);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                {
                    doBatchRename(*sExplorerCtrl, XPR_TRUE, gOpt->mConfig.mBatchRenameWithFolder);

                    sRenamed = XPR_TRUE;
                }
            }
        }

        if (sRenamed == XPR_FALSE)
        {
            sExplorerCtrl->renameDirectly(sSelIndex);
        }
    }
}

xpr_sint_t BatchRenameCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void BatchRenameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        doBatchRename(*sFolderCtrl, XPR_TRUE, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        doBatchRename(*sSearchResultCtrl, XPR_TRUE, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        doBatchRename(*sExplorerCtrl, XPR_TRUE, XPR_TRUE);
    }
}

xpr_sint_t BatchOnlyFileRenameCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void BatchOnlyFileRenameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        doBatchRename(*sFolderCtrl, XPR_TRUE, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        doBatchRename(*sSearchResultCtrl, XPR_TRUE, XPR_FALSE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        doBatchRename(*sExplorerCtrl, XPR_TRUE, XPR_FALSE);
    }
}

xpr_sint_t BatchOnlyFolderRenameCommand::canExecute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = XPR_TRUE;
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = sExplorerCtrl->isFileSystemFolder();
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void BatchOnlyFolderRenameCommand::execute(CommandContext &aContext)
{
    FXFILE_COMMAND_DECLARE_CTRL;

    FXFILE_COMMAND_IF_FOLDER_CTRL
    {
        doBatchRename(*sFolderCtrl, XPR_FALSE, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        doBatchRename(*sSearchResultCtrl, XPR_FALSE, XPR_TRUE);
    }
    FXFILE_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        doBatchRename(*sExplorerCtrl, XPR_FALSE, XPR_TRUE);
    }
}
} // namespace cmd
} // namespace fxfile
