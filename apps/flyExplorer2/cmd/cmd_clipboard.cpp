//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_clipboard.h"

#include <afxodlgs.h> // for COlePasteSpecialDialog

#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_clip_format.h"

#include "CreateItemDlg.h"
#include "MainFrame.h"
#include "FolderCtrl.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"

#include "cmd_parameters.h"
#include "cmd_parameter_define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
static xpr_bool_t isFocusEditCtrl(void)
{
    HWND sHwnd = ::GetFocus();

    static xpr_tchar_t sClassName[MAX_CLASS_NAME + 1];
    sClassName[0] = XPR_STRING_LITERAL('\0');
    ::GetClassName(sHwnd, sClassName, MAX_CLASS_NAME);

    return (_tcsicmp(sClassName, XPR_STRING_LITERAL("Edit")) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_sint_t ClipboardFileCutCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = isFocusEditCtrl() ? XPR_TRUE : sFolderCtrl->hasSelShellAttributes(SFGAO_CANMOVE);;
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = isFocusEditCtrl() ? XPR_TRUE : sExplorerCtrl->hasSelShellAttributes(SFGAO_CANMOVE);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFileCutCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        if (isFocusEditCtrl() == XPR_TRUE)
            ::SendMessage(::GetFocus(), WM_CUT, 0, 0);
        else
        {
            xpr_sint_t sClipFormatCount = sFolderCtrl->getClipFormatCount();
            if (sClipFormatCount <= 0)
                return;

            CWaitCursor sWaitCursor;

            CLIPFORMAT *sClipFormat = new CLIPFORMAT[sClipFormatCount];
            sFolderCtrl->getClipFormat(sClipFormat);

            LPDATAOBJECT sDataObject = XPR_NULL;
            if (sFolderCtrl->getDataObject(&sDataObject) == XPR_TRUE)
            {
                fxb::SetClipboard(sDataObject, DROPEFFECT_MOVE, sClipFormat);
                COM_RELEASE(sDataObject);
            }

            XPR_SAFE_DELETE_ARRAY(sClipFormat);
        }
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelCount > 0)
        {
            xpr_sint_t sCount = 0;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];

            if (sSearchResultCtrl->getSelFullPidls(&sShellFolder, sPidls, sCount) == XPR_TRUE)
                fxb::ContextMenu::invokeCommand(sShellFolder, sPidls, sCount, CMID_VERB_CUT, sSearchResultCtrl->GetSafeHwnd());

            xpr_sint_t i;
            for (i = 0; i < sCount; ++i)
            {
                COM_FREE(sPidls[i]);
            }
            COM_RELEASE(sShellFolder);
            XPR_SAFE_DELETE_ARRAY(sPidls);
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (isFocusEditCtrl() == XPR_TRUE)
            ::SendMessage(::GetFocus(), WM_CUT, 0, 0);
        else
        {
            if (sExplorerCtrl->GetSelectedCount() <= 0)
                return;

            xpr_sint_t sClipFormatCount = sExplorerCtrl->getClipFormatCount();
            if (sClipFormatCount <= 0)
                return;

            CWaitCursor sWaitCursor;

            CLIPFORMAT *sClipFormat = new CLIPFORMAT[sClipFormatCount];
            sExplorerCtrl->getClipFormat(sClipFormat);

            LPDATAOBJECT sDataObject = XPR_NULL;
            if (sExplorerCtrl->getSelDataObject(&sDataObject, SFGAO_CANMOVE) == XPR_TRUE)
            {
                fxb::SetClipboard(sDataObject, DROPEFFECT_MOVE, sClipFormat);
                COM_RELEASE(sDataObject);
            }

            XPR_SAFE_DELETE_ARRAY(sClipFormat);
        }
    }
}

xpr_sint_t ClipboardFileCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = isFocusEditCtrl() ? XPR_TRUE : sFolderCtrl->hasSelShellAttributes(SFGAO_CANCOPY);
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = isFocusEditCtrl() ? XPR_TRUE : sExplorerCtrl->hasSelShellAttributes(SFGAO_CANCOPY);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFileCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        if (isFocusEditCtrl() == XPR_TRUE)
            ::SendMessage(::GetFocus(), WM_COPY, 0, 0);
        else
        {
            xpr_sint_t sClipFormatCount = sFolderCtrl->getClipFormatCount();
            if (sClipFormatCount <= 0)
                return;

            CWaitCursor sWaitCursor;

            CLIPFORMAT *sClipFormat = new CLIPFORMAT[sClipFormatCount];
            sFolderCtrl->getClipFormat(sClipFormat);

            LPDATAOBJECT sDataObject = XPR_NULL;
            if (sFolderCtrl->getDataObject(&sDataObject))
            {
                fxb::SetClipboard(sDataObject, DROPEFFECT_COPY | DROPEFFECT_LINK, sClipFormat);
                COM_RELEASE(sDataObject);
            }

            XPR_SAFE_DELETE_ARRAY(sClipFormat);
        }
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sSelCount = sSearchResultCtrl->GetSelectedCount();
        if (sSelCount > 0)
        {
            xpr_sint_t sCount = 0;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];

            if (sSearchResultCtrl->getSelFullPidls(&sShellFolder, sPidls, sCount) == XPR_TRUE)
                fxb::ContextMenu::invokeCommand(sShellFolder, sPidls, sCount, CMID_VERB_COPY, sSearchResultCtrl->GetSafeHwnd());

            xpr_sint_t i;
            for (i = 0; i < sCount; ++i)
            {
                COM_FREE(sPidls[i]);
            }
            COM_RELEASE(sShellFolder);
            XPR_SAFE_DELETE_ARRAY(sPidls);
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (isFocusEditCtrl() == XPR_TRUE)
            ::SendMessage(::GetFocus(), WM_COPY, 0, 0);
        else
        {
            if (sExplorerCtrl->GetSelectedCount() <= 0)
                return;

            xpr_sint_t sClipFormatCount = sExplorerCtrl->getClipFormatCount();
            if (sClipFormatCount <= 0)
                return;

            CWaitCursor sWaitCursor;

            CLIPFORMAT *sClipFormat = new CLIPFORMAT[sClipFormatCount];
            sExplorerCtrl->getClipFormat(sClipFormat);

            LPDATAOBJECT sDataObject = XPR_NULL;
            if (sExplorerCtrl->getSelDataObject(&sDataObject, SFGAO_CANCOPY) == XPR_TRUE)
            {
                fxb::SetClipboard(sDataObject, DROPEFFECT_COPY, sClipFormat);
                COM_RELEASE(sDataObject);
            }

            XPR_SAFE_DELETE_ARRAY(sClipFormat);
        }
    }
}

static xpr_bool_t IsFilePaste(FolderCtrl &aFolderCtrl, MainFrame &aMainFrame)
{
    fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

    xpr_bool_t sEnable = isFocusEditCtrl();
    if (sEnable == XPR_FALSE)
    {
        HTREEITEM sTreeItem = aFolderCtrl.GetSelectedItem();
        if (sTreeItem != XPR_NULL)
        {
            LPTVITEMDATA sTvItemData = (LPTVITEMDATA)aFolderCtrl.GetItemData(sTreeItem);
            if (sTvItemData != XPR_NULL)
            {
                xpr_tchar_t szCurPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, szCurPath);
                if (_tcslen(szCurPath) > 0)
                {
                    COleDataObject sOleDataObject;
                    sOleDataObject.AttachClipboard();
                    if (sOleDataObject.IsDataAvailable(sClipFormat.mShellIDList) == XPR_TRUE)
                        sEnable = XPR_TRUE;

                    xpr_bool_t sFileSystem = aFolderCtrl.hasSelShellAttributes(SFGAO_FILESYSTEM);

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteUnicodeText(&sOleDataObject) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteText(&sOleDataObject) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteBitmap(&sOleDataObject) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }

                    if (sEnable == XPR_FALSE)
                    {
                        if (sFileSystem == XPR_TRUE && fxb::IsPasteDIB(&sOleDataObject) == XPR_TRUE)
                            sEnable = XPR_TRUE;
                    }
                }
            }
        }
    }

    return sEnable;
}

static xpr_bool_t IsFilePaste(ExplorerCtrl &aExplorerCtrl, MainFrame &aMainFrame)
{
    fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

    xpr_bool_t sEnable = isFocusEditCtrl();
    if (sEnable == XPR_FALSE)
    {
        const xpr_tchar_t *sCurPath = aExplorerCtrl.getCurPath();
        if (_tcslen(sCurPath) > 0)
        {
            COleDataObject sOleDataObject;
            sOleDataObject.AttachClipboard();
            if (sOleDataObject.IsDataAvailable(sClipFormat.mShellIDList) == XPR_TRUE)
                sEnable = XPR_TRUE;

            xpr_bool_t sFolderFileSystem = aExplorerCtrl.isFileSystemFolder();

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteUnicodeText(&sOleDataObject) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteText(&sOleDataObject) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteBitmap(&sOleDataObject) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }

            if (sEnable == XPR_FALSE)
            {
                if (sFolderFileSystem == XPR_TRUE && fxb::IsPasteDIB(&sOleDataObject) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }
        }
    }

    return sEnable;
}

xpr_sint_t ClipboardFilePasteCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePaste(*sFolderCtrl, *sMainFrame);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePaste(*sExplorerCtrl, *sMainFrame);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFilePasteCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = XPR_NULL;
        if (sParameters != XPR_NULL)
            sTreeItem = (HTREEITEM)sParameters->get(CommandParameterIdTreeTargetItem);

        if (sTreeItem == XPR_NULL)
            sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sTargetDir);

        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        COleDataObject sOleDataObject;
        sOleDataObject.AttachClipboard();

        xpr_sint_t sResult;
        xpr_bool_t sFileSystem = sFolderCtrl->hasSelShellAttributes(SFGAO_FILESYSTEM);
        LPDATAOBJECT sDataObject = sOleDataObject.GetIDataObject(XPR_FALSE);

        if (XPR_IS_TRUE(sFileSystem))
        {
            if (fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
            {
                sResult = fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                if (sResult)
                    return;
            }

            if (fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
            {
                sResult = fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                if (sResult)
                    return;
            }

            if (fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
            {
                sResult = fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                if (sResult)
                    return;
            }

            if (fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
            {
                sResult = fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                if (sResult)
                    return;
            }

            xpr_sint_t sResult = 0;
            sExplorerCtrl = sMainFrame->getExplorerCtrl(sFolderCtrl);

            if (fxb::IsPasteBitmap(&sOleDataObject) == XPR_TRUE)
            {
                sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                if (sResult != 0)
                    return;
            }

            if (fxb::IsPasteDIB(&sOleDataObject) == XPR_TRUE)
            {
                sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                if (sResult != 0)
                    return;
            }

            if (fxb::IsPasteUnicodeText(&sOleDataObject) == XPR_TRUE)
            {
                sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                if (sResult != 0)
                    return;
            }

            if (fxb::IsPasteText(&sOleDataObject) == XPR_TRUE)
            {
                sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                if (sResult != 0)
                    return;
            }
        }

        DROPEFFECT sDropEffect = DROPEFFECT_NONE;
        fxb::GetPreferredDropEffect(&sOleDataObject, sClipFormat.mDropEffect, sDropEffect);

        xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
        xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

        if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
            fxb::ContextMenu::invokeCommand(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1, CMID_VERB_PASTE, sFolderCtrl->GetSafeHwnd());
        else
        {
            const xpr_tchar_t *sLinkSuffix = theApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

            fxb::DoPaste(sFolderCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir, sLinkSuffix);
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        if (isFocusEditCtrl() == XPR_TRUE)
            ::SendMessage(::GetFocus(), WM_PASTE, 0, 0);
        else
        {
            //
            // clipboard priority
            //
            // 1. Selected Folder
            // 2. Internet URL
            // 3. File Contents (ex. Outlook)
            // 4. Bitmap Image
            // 5. DIB Image
            // 6. Text
            // 7. General Folder/File
            //
            fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

            xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1];
            _tcscpy(sTargetDir, sExplorerCtrl->getCurPath());

            xpr_bool_t sSelItem = XPR_FALSE;
            if (sParameters != XPR_NULL)
                sSelItem = (xpr_bool_t)(xpr_sintptr_t)sParameters->get(CommandParameterIdIsSelectedItem);

            if (sSelItem == XPR_TRUE)
            {
                xpr_sint_t sSelCount = sExplorerCtrl->GetSelectedCount();
                if (sSelCount >= 0)
                {
                    xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();

                    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                    if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    {
                        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                            fxb::GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sTargetDir, sLvItemData->mShellAttributes);
                        else
                            fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sTargetDir);

                        DROPEFFECT sDropEffect = DROPEFFECT_NONE;
                        fxb::GetPreferredDropEffect(sClipFormat.mDropEffect, sDropEffect);

                        xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
                        xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

                        if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
                            fxb::ContextMenu::invokeCommand(sLvItemData->mShellFolder, &sLvItemData->mPidl, 1, CMID_VERB_PASTE, sExplorerCtrl->GetSafeHwnd());
                        else
                            sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir);

                        return;
                    }
                }
            }

            COleDataObject sOleDataObject;
            sOleDataObject.AttachClipboard();

            xpr_sint_t sResult;
            xpr_bool_t sFolderFileSystem = sExplorerCtrl->isFileSystemFolder();
            LPDATAOBJECT sDataObject = sOleDataObject.GetIDataObject(XPR_FALSE);

            if (XPR_IS_TRUE(sFolderFileSystem))
            {
                if (fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                {
                    sResult = fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    if (sResult)
                        return;
                }

                if (fxb::IsPasteInetUrl(&sOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
                {
                    sResult = fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_TRUE);
                    if (sResult)
                        return;
                }

                if (fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
                {
                    sResult = fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    if (sResult)
                        return;
                }

                if (fxb::IsPasteFileContents(&sOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
                {
                    sResult = fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_TRUE);
                    if (sResult)
                        return;
                }

                xpr_sint_t sResult = 0;

                if (fxb::IsPasteBitmap(&sOleDataObject) == XPR_TRUE)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                    if (sResult != 0)
                        return;
                }

                if (fxb::IsPasteDIB(&sOleDataObject) == XPR_TRUE)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                    if (sResult != 0)
                        return;
                }

                if (fxb::IsPasteUnicodeText(&sOleDataObject) == XPR_TRUE)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                    if (sResult != 0)
                        return;
                }

                if (fxb::IsPasteText(&sOleDataObject) == XPR_TRUE)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                    if (sResult != 0)
                        return;
                }
            }

            DROPEFFECT sDropEffect = DROPEFFECT_NONE;
            fxb::GetPreferredDropEffect(&sOleDataObject, sClipFormat.mDropEffect, sDropEffect);

            xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
            xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

            if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
            {
                LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                fxb::ContextMenu::invokeCommand(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1, CMID_VERB_PASTE, sExplorerCtrl->GetSafeHwnd());
            }
            else
            {
                sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir);
            }
        }
    }
}

xpr_sint_t ClipboardFilePasteCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePaste(*sFolderCtrl, *sMainFrame);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePaste(*sExplorerCtrl, *sMainFrame);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFilePasteCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sTarget);

        fxb::DoPaste(sFolderCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTarget, 0, XPR_TRUE);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sTarget, sExplorerCtrl->getCurPath());

        sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTarget, XPR_TRUE);
    }
}

xpr_sint_t ClipboardFilePasteLinkCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePaste(*sFolderCtrl, *sMainFrame);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePaste(*sExplorerCtrl, *sMainFrame);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFilePasteLinkCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sTarget);

        const xpr_tchar_t *sLinkSuffix = theApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

        fxb::DoPaste(sFolderCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, DROPEFFECT_LINK, sTarget, sLinkSuffix);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sTarget, sExplorerCtrl->getCurPath());

        sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, DROPEFFECT_LINK, sTarget);
    }
}

xpr_sint_t ClipboardFileSpecialPasteCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePaste(*sFolderCtrl, *sMainFrame);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePaste(*sExplorerCtrl, *sMainFrame);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFileSpecialPasteCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        HTREEITEM sTreeItem = XPR_NULL;
        if (XPR_IS_NULL(sTreeItem))
            sTreeItem = sFolderCtrl->GetSelectedItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sTargetDir);

        COlePasteSpecialDialog sDlg;
        //sDlg.AddFormat(CF_METAFILEPICT,  TYMED_MFPICT,                   AFX_IDS_METAFILE_FORMAT,                XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_DIB,                    TYMED_HGLOBAL,                  AFX_IDS_DIB_FORMAT,                     XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_BITMAP,                 TYMED_GDI,                      AFX_IDS_BITMAP_FORMAT,                  XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_TEXT,                   TYMED_HGLOBAL,                  AFX_IDS_TEXT_FORMAT,                    XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_UNICODETEXT,            TYMED_HGLOBAL,                  IDS_CLIPBOARD_UNICODE_TEXT_FORMAT,      XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mInetUrl,      TYMED_HGLOBAL,                  IDS_CLIPBOARD_URL_FORMAT,               XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mFileContents, TYMED_ISTREAM | TYMED_ISTORAGE, IDS_CLIPBOARD_FILE_CONTENTS_FORMAT,     XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mShellIDList,  TYMED_HGLOBAL,                  IDS_CLIPBOARD_SHELL_IDLIST_FORMAT,      XPR_FALSE, XPR_FALSE);
        //sDlg.AddFormat(sClipFormat.mFileName,     TYMED_FILE | TYMED_HGLOBAL,     IDS_CLIPBOARD_FILE_NAME_FORMAT,         XPR_FALSE, XPR_FALSE);
        //sDlg.AddFormat(sClipFormat.mFileNameW,    TYMED_FILE | TYMED_HGLOBAL,     IDS_CLIPBOARD_UNICODE_FILE_NAME_FORMAT, XPR_FALSE, XPR_FALSE);

        if (sDlg.DoModal() != IDOK)
            return;

        CLIPFORMAT sSpecialClipFormat = sDlg.m_ps.arrPasteEntries[sDlg.m_ps.nSelectedIndex].fmtetc.cfFormat;
        if (sSpecialClipFormat == sClipFormat.mShellIDList)
        {
            DROPEFFECT sDropEffect = DROPEFFECT_NONE;
            fxb::GetPreferredDropEffect(sClipFormat.mDropEffect, sDropEffect);

            xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
            xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

            if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
                fxb::ContextMenu::invokeCommand(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1, CMID_VERB_PASTE, sFolderCtrl->GetSafeHwnd());
            else
            {
                const xpr_tchar_t *sLinkSuffix = theApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

                fxb::DoPaste(sFolderCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir, sLinkSuffix);
            }
        }
        else
        {
            COleDataObject sOleDataObject;
            sOleDataObject.AttachClipboard();

            xpr_bool_t sFileSystem = sFolderCtrl->hasSelShellAttributes(SFGAO_FILESYSTEM);
            LPDATAOBJECT sDataObject = sOleDataObject.GetIDataObject(XPR_FALSE);

            if (XPR_IS_TRUE(sFileSystem))
            {
                xpr_sint_t sResult = 0;

                if (sSpecialClipFormat == sClipFormat.mInetUrl)
                {
                    fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                }
                else if (sSpecialClipFormat == sClipFormat.mFileContents)
                {
                    fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_BITMAP)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                }
                else if (sSpecialClipFormat == CF_DIB)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_UNICODETEXT)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_TEXT)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                }
            }
        }
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();

        COlePasteSpecialDialog sDlg;
        //sDlg.AddFormat(CF_METAFILEPICT,  TYMED_MFPICT,                   IDS_CLIPBOARD_METAFILE_FORMAT,          XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_DIB,                    TYMED_HGLOBAL,                  IDS_CLIPBOARD_DIB_FORMAT,               XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_BITMAP,                 TYMED_GDI,                      IDS_CLIPBOARD_BITMAP_FORMAT,            XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_TEXT,                   TYMED_HGLOBAL,                  IDS_CLIPBOARD_TEXT_FORMAT,              XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(CF_UNICODETEXT,            TYMED_HGLOBAL,                  IDS_CLIPBOARD_UNICODE_TEXT_FORMAT,      XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mInetUrl,      TYMED_HGLOBAL,                  IDS_CLIPBOARD_URL_FORMAT,               XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mFileContents, TYMED_ISTREAM | TYMED_ISTORAGE, IDS_CLIPBOARD_FILE_CONTENTS_FORMAT,     XPR_FALSE, XPR_FALSE);
        sDlg.AddFormat(sClipFormat.mShellIDList,  TYMED_HGLOBAL,                  IDS_CLIPBOARD_SHELL_IDLIST_FORMAT,      XPR_FALSE, XPR_FALSE);
        //sDlg.AddFormat(mFileName,     TYMED_FILE | TYMED_HGLOBAL,     IDS_CLIPBOARD_FILE_NAME_FORMAT,         XPR_FALSE, XPR_FALSE);
        //sDlg.AddFormat(mFileNameW,    TYMED_FILE | TYMED_HGLOBAL,     IDS_CLIPBOARD_UNICODE_FILE_NAME_FORMAT, XPR_FALSE, XPR_FALSE);

        if (sDlg.DoModal() != IDOK)
            return;

        xpr_bool_t sSelItem = XPR_FALSE;
        xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sTargetDir, sExplorerCtrl->getCurPath());

        CLIPFORMAT sSpecialClipFormat = sDlg.m_ps.arrPasteEntries[sDlg.m_ps.nSelectedIndex].fmtetc.cfFormat;
        if (sSpecialClipFormat == sClipFormat.mShellIDList)
        {
            if (XPR_IS_TRUE(sSelItem))
            {
                xpr_sint_t sSelCount = sExplorerCtrl->GetSelectedCount();
                if (sSelCount >= 0)
                {
                    xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
                    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                    if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    {
                        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                            fxb::GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sTargetDir, sLvItemData->mShellAttributes);
                        else
                            fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sTargetDir);

                        DROPEFFECT sDropEffect = DROPEFFECT_NONE;
                        fxb::GetPreferredDropEffect(sClipFormat.mDropEffect, sDropEffect);

                        xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
                        xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

                        if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
                            fxb::ContextMenu::invokeCommand(sLvItemData->mShellFolder, &sLvItemData->mPidl, 1, CMID_VERB_PASTE, sExplorerCtrl->GetSafeHwnd());
                        else
                            sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir);

                        return;
                    }
                }
            }

            DROPEFFECT sDropEffect = DROPEFFECT_NONE;
            fxb::GetPreferredDropEffect(sClipFormat.mDropEffect, sDropEffect);

            xpr_bool_t sExternalCopyFileOp = (sDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(gOpt->mExternalCopyFileOp);
            xpr_bool_t sExternalMoveFileOp = (sDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(gOpt->mExternalMoveFileOp);

            if (XPR_IS_TRUE(sExternalCopyFileOp) || XPR_IS_TRUE(sExternalMoveFileOp))
            {
                LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                fxb::ContextMenu::invokeCommand(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1, CMID_VERB_PASTE, sExplorerCtrl->GetSafeHwnd());
            }
            else
                sExplorerCtrl->doPasteSelect(sExplorerCtrl->GetSafeHwnd(), sClipFormat.mShellIDList, sClipFormat.mDropEffect, sTargetDir);
        }
        else
        {
            COleDataObject sOleDataObject;
            sOleDataObject.AttachClipboard();

            xpr_sint_t sResult = 0;
            xpr_bool_t sFolderFileSystem = sExplorerCtrl->isFileSystemFolder();
            LPDATAOBJECT sDataObject = sOleDataObject.GetIDataObject(XPR_FALSE);

            if (XPR_IS_TRUE(sFolderFileSystem))
            {
                if (sSpecialClipFormat == sClipFormat.mInetUrl)
                {
                    fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    fxb::DoPasteInetUrl(sDataObject, sTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                }
                else if (sSpecialClipFormat == sClipFormat.mFileContents)
                {
                    fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
                    fxb::DoPasteFileContents(sDataObject, sTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_BITMAP)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                }
                else if (sSpecialClipFormat == CF_DIB)
                {
                    sResult = pasteNewBitmapFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_UNICODETEXT)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_TRUE);
                }
                else if (sSpecialClipFormat == CF_TEXT)
                {
                    sResult = pasteNewTextFile(*sExplorerCtrl, sTargetDir, sDataObject, XPR_FALSE);
                }
            }
        }
    }
}

xpr_sint_t ClipboardNameCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = XPR_TRUE;
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sSelectedCount = sExplorerCtrl->GetSelectedCount();
        if (sSelectedCount > 0)
        {
            if (sSelectedCount == 1)
            {
                xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();

                LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (sLvItemData != XPR_NULL)
                    sEnable = XPR_TRUE;
            }
            else
            {
                sEnable = XPR_TRUE;
            }
        }

        // copy folder name
        if (sEnable == XPR_FALSE && sSelectedCount == 0)
            sEnable = XPR_TRUE;
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardNameCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        std::tstring sName;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);

        fxb::SetTextClipboard(sFolderCtrl->GetSafeHwnd(), sName.c_str());
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_tchar_t *sText = new xpr_tchar_t[XPR_MAX_PATH * sCount + 1];
        sText[0] = '\0';

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sIndex;
        xpr_tchar_t *sSplit;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                sSplit = _tcsrchr(sPath, '\\') + 1;
                if (XPR_IS_NULL(sSplit))
                    continue;

                if (sText[0] != '\0')
                    _tcscat(sText, sClipSeperatorText);

                sSearchResultCtrl->getDispFileName(sSplit, sName);

                _tcscat(sText, sName);
            }
        }

        fxb::SetTextClipboard(sSearchResultCtrl->GetSafeHwnd(), sText);

        XPR_SAFE_DELETE_ARRAY(sText);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sClipText;
        std::tstring sName;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);
                sClipText = sName;
            }
        }
        else
        {
            xpr_sint_t sIndex;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                if (sClipText.empty() == false)
                    sClipText += sClipSeperatorText;

                sClipText += sExplorerCtrl->GetItemText(sIndex, 0);
            }
        }

        fxb::SetTextClipboard(sExplorerCtrl->GetSafeHwnd(), sClipText.c_str());
    }
}

xpr_sint_t ClipboardFileNameCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = XPR_TRUE;
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        xpr_sint_t sSelectedCount = sExplorerCtrl->GetSelectedCount();
        if (sSelectedCount > 0)
        {
            if (sSelectedCount == 1)
            {
                xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();

                if (sExplorerCtrl->isFileSystem(sIndex) == XPR_TRUE)
                    sEnable = XPR_TRUE;
            }
            else
            {
                sEnable = XPR_TRUE;
            }
        }

        if (sEnable == XPR_FALSE && sSelectedCount == 0)
        {
            if (sExplorerCtrl->isFileSystemFolder() == XPR_TRUE)
                sEnable = XPR_TRUE;
        }
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardFileNameCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        std::tstring sName;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);

        fxb::SetTextClipboard(sFolderCtrl->GetSafeHwnd(), sName.c_str());
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_tchar_t *sText = new xpr_tchar_t[XPR_MAX_PATH * sCount + 1];
        sText[0] = '\0';

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sIndex;
        xpr_tchar_t *sSplit;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                sSplit = _tcsrchr(sPath, '\\') + 1;
                if (XPR_IS_NULL(sSplit))
                    continue;

                if (sText[0] != '\0')
                    _tcscat(sText, sClipSeperatorText);

                _tcscat(sText, sSplit);
            }
        }

        fxb::SetTextClipboard(sSearchResultCtrl->GetSafeHwnd(), sText);

        XPR_SAFE_DELETE_ARRAY(sText);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sClipText;
        std::tstring sFileName;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName);
                sClipText = sFileName;
            }
        }
        else
        {
            xpr_sint_t sIndex;
            LPLVITEMDATA sLvItemData = XPR_NULL;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData))
                {
                    fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName);

                    if (sClipText.empty() == false)
                        sClipText += sClipSeperatorText;

                    sClipText += sFileName;
                }
            }
        }

        fxb::SetTextClipboard(sExplorerCtrl->GetSafeHwnd(), sClipText.c_str());
    }
}

static xpr_bool_t IsFilePathCopy(FolderCtrl &aFolderCtrl)
{
    xpr_bool_t sResult = XPR_FALSE;

    HTREEITEM sTreeItem = aFolderCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)aFolderCtrl.GetItemData(sTreeItem);
        if (sTvItemData == XPR_NULL)
            return XPR_FALSE;

        sResult = fxb::IsFileSystemFolder(sTvItemData->mShellFolder, sTvItemData->mPidl);
    }

    return sResult;
}

static xpr_bool_t IsFilePathCopy(ExplorerCtrl &aExplorerCtrl)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sSelectedCount = aExplorerCtrl.GetSelectedCount();
    if (sSelectedCount > 0)
    {
        if (sSelectedCount == 1)
        {
            xpr_sint_t sIndex = aExplorerCtrl.GetSelectionMark();

            if (aExplorerCtrl.isFileSystem(sIndex) == XPR_TRUE)
                sResult = XPR_TRUE;
        }
        else
            sResult = XPR_TRUE;
    }

    if (sResult == XPR_FALSE && sSelectedCount == 0)
    {
        if (aExplorerCtrl.isFileSystemFolder() == XPR_TRUE)
            sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_sint_t ClipboardPathCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePathCopy(*sFolderCtrl);
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePathCopy(*sExplorerCtrl);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardPathCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        std::tstring sPath;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);

        fxb::SetTextClipboard(sFolderCtrl->GetSafeHwnd(), sPath.c_str());
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_tchar_t *sText = new xpr_tchar_t[XPR_MAX_PATH * sCount + 1];
        sText[0] = '\0';

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sIndex;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                if (sText[0] != '\0')
                    _tcscat(sText, sClipSeperatorText);

                _tcscat(sText, sPath);
            }
        }

        fxb::SetTextClipboard(sSearchResultCtrl->GetSafeHwnd(), sText);

        XPR_SAFE_DELETE_ARRAY(sText);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sClipText;
        std::tstring sPath;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
                sClipText = sPath;
            }
        }
        else
        {
            xpr_sint_t sIndex;
            LPLVITEMDATA sLvItemData = XPR_NULL;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                {
                    if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                        fxb::GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sPath, sLvItemData->mShellAttributes);
                    else
                        fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                    if (sClipText.empty() == false)
                        sClipText += sClipSeperatorText;

                    sClipText += sPath;
                }
            }
        }

        fxb::SetTextClipboard(sExplorerCtrl->GetSafeHwnd(), sClipText.c_str());
    }
}

xpr_sint_t ClipboardDevPathCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePathCopy(*sFolderCtrl);
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePathCopy(*sExplorerCtrl);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardDevPathCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        std::tstring sDevPath;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sDevPath);

        fxb::ConvDevPath(sDevPath);

        fxb::SetTextClipboard(sFolderCtrl->GetSafeHwnd(), sDevPath.c_str());
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_tchar_t *sText = new xpr_tchar_t[XPR_MAX_PATH * sCount + 1];
        memset(sText, 0, (XPR_MAX_PATH * sCount + 1) * sizeof(xpr_tchar_t));

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sIndex;
        xpr_tchar_t sName[XPR_MAX_PATH * 2 + 1];
        xpr_tchar_t sTemp[XPR_MAX_PATH + 1];
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                _tcscpy(sName, sPath);

                xpr_tchar_t *sSplit = sName;
                while (true)
                {
                    sSplit = _tcschr(sSplit, '\\');
                    if (XPR_IS_NULL(sSplit))
                        break;
                    _tcscpy(sTemp, sSplit);
                    _tcscpy(sSplit+1, sTemp);
                    sSplit[0] = '\\';
                    sSplit += 2;
                }

                if (sText[0] != '\0')
                    _tcscat(sText, sClipSeperatorText);

                _tcscat(sText, sName);
            }
        }

        fxb::SetTextClipboard(sSearchResultCtrl->GetSafeHwnd(), sText);

        XPR_SAFE_DELETE_ARRAY(sText);
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sClipText;
        std::tstring sPath;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
                fxb::ConvDevPath(sPath);
                sClipText = sPath;
            }
        }
        else
        {
            xpr_sint_t sIndex;
            LPLVITEMDATA sLvItemData = XPR_NULL;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                {
                    if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                        fxb::GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sPath, sLvItemData->mShellAttributes);
                    else
                        fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                    fxb::ConvDevPath(sPath);

                    if (sClipText.empty() == false)
                        sClipText += sClipSeperatorText;

                    sClipText += sPath;
                }
            }
        }

        fxb::SetTextClipboard(sExplorerCtrl->GetSafeHwnd(), sClipText.c_str());
    }
}

xpr_sint_t ClipboardUrlCopyCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sEnable = XPR_FALSE;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        sEnable = IsFilePathCopy(*sFolderCtrl);
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        sEnable = (sSearchResultCtrl->GetSelectedCount() > 0) ? XPR_TRUE : XPR_FALSE;
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        sEnable = IsFilePathCopy(*sExplorerCtrl);
    }

    return (sEnable == XPR_TRUE) ? StateEnable : StateDisable;
}

void ClipboardUrlCopyCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    XPR_COMMAND_IF_FOLDER_CTRL
    {
        HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
        if (XPR_IS_NULL(sTreeItem))
            return;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sTreeItem);
        if (XPR_IS_NULL(sTvItemData))
            return;

        std::tstring sUrlPath;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sUrlPath);

        fxb::ConvUrlPath(sUrlPath);

        fxb::SetTextClipboard(sFolderCtrl->GetSafeHwnd(), sUrlPath.c_str());
    }
    XPR_COMMAND_ELSE_IF_SEARCH_RESULT_CTRL
    {
        xpr_sint_t sCount = sSearchResultCtrl->GetSelectedCount();
        if (sCount <= 0)
            return;

        xpr_size_t sFind;
        std::tstring sClipText;
        std::tstring sPath;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sIndex;
        SrItemData *sSrItemData = XPR_NULL;

        POSITION sPosition = sSearchResultCtrl->GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = sSearchResultCtrl->GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)sSearchResultCtrl->GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                if (sClipText.empty() == false)
                    sClipText += sClipSeperatorText;

                sFind = 0;
                while ((sFind = sPath.find(XPR_STRING_LITERAL('\\'), sFind)) != std::tstring::npos)
                {
                    sPath.replace(sFind, 1, 1, XPR_STRING_LITERAL('/'));
                    sFind++;
                }

                sClipText += XPR_STRING_LITERAL("file:///");
                sClipText += sPath;
            }
        }

        fxb::SetTextClipboard(sSearchResultCtrl->GetSafeHwnd(), sClipText.c_str());
    }
    XPR_COMMAND_ELSE_IF_EXPLORER_CTRL
    {
        std::tstring sClipText;
        std::tstring sPath;

        xpr_tchar_t sClipSeperatorText[31] = {0};
        fxb::ConvertStringToFormat(gOpt->mClipboardSeparator, sClipSeperatorText);

        xpr_sint_t sCount = sExplorerCtrl->GetSelectedCount();
        if (sCount <= 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
                fxb::ConvUrlPath(sPath);
                sClipText = sPath;
            }
        }
        else
        {
            xpr_sint_t sIndex;
            LPLVITEMDATA sLvItemData = XPR_NULL;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
                if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                {
                    if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                        fxb::GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sPath, sLvItemData->mShellAttributes);
                    else
                        fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                    fxb::ConvUrlPath(sPath);

                    if (sClipText.empty() == false)
                        sClipText += sClipSeperatorText;

                    sClipText += sPath;
                }
            }
        }

        fxb::SetTextClipboard(sExplorerCtrl->GetSafeHwnd(), sClipText.c_str());
    }
}

xpr_sint_t pasteNewTextFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aUnicode)
{
    xpr_sint_t sResult = 0;

    HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".txt"), XPR_TRUE);

    CreateItemDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_text_file.title")));
    sDlg.setDir(aDir);
    sDlg.setExt(XPR_STRING_LITERAL("txt"));
    sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_text_file.msg.exist")));
    sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_text_file.msg.empty")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_text_file.label.top_desc")));
    sDlg.setDescIcon(sIcon, XPR_TRUE);

    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId == IDOK)
    {
        const xpr_tchar_t *sPath = sDlg.getNewItem();

        xpr_bool_t sSucceeded = XPR_FALSE;
        if (XPR_IS_TRUE(aUnicode))
            sSucceeded = fxb::DoPasteUnicodeText(aDataObject, sPath);
        else
            sSucceeded = fxb::DoPasteText(aDataObject, sPath);

        if (XPR_IS_TRUE(sSucceeded))
        {
            const xpr_tchar_t *sCurDir = aExplorerCtrl.getCurPath();

            xpr_bool_t sIsEqualedCurDir = fxb::IsEqualPath(aDir, sCurDir);
            if (sIsEqualedCurDir == XPR_TRUE)
                aExplorerCtrl.setInsSelPath(sPath);

            sResult = 1;
        }
    }
    else
    {
        sResult = -1;
    }

    return sResult;
}

xpr_sint_t pasteNewBitmapFile(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aDir, LPDATAOBJECT aDataObject, xpr_bool_t aDib)
{
    xpr_sint_t sResult = 0;

    HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".bmp"), XPR_TRUE);

    std::tstring sCurDir;
    aExplorerCtrl.getCurPath(sCurDir);

    CreateItemDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_bitmap_file.title")));
    sDlg.setDir(aDir);
    sDlg.setExt(XPR_STRING_LITERAL("bmp"));
    sDlg.setMsg(CreateItemDlg::MSG_ID_EXIST, theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_bitmap_file.msg.exist")));
    sDlg.setMsg(CreateItemDlg::MSG_ID_EMPTY, theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_bitmap_file.msg.empty")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.paste_into_new_bitmap_file.label.top_desc")));
    sDlg.setDescIcon(sIcon, XPR_TRUE);

    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId == IDOK)
    {
        const xpr_tchar_t *sPath = sDlg.getNewItem();

        xpr_bool_t sSucceeded = XPR_FALSE;
        if (XPR_IS_TRUE(aDib))
            sSucceeded = fxb::DoPasteDIB(aDataObject, sPath);
        else
            sSucceeded = fxb::DoPasteBitmap(aDataObject, sPath);

        if (XPR_IS_TRUE(sSucceeded))
        {
            const xpr_tchar_t *sCurDir = aExplorerCtrl.getCurPath();

            xpr_bool_t sIsEqualedCurDir = fxb::IsEqualPath(aDir, sCurDir);
            if (sIsEqualedCurDir == XPR_TRUE)
                aExplorerCtrl.setInsSelPath(sPath);

            sResult = 1;
        }
    }
    else
    {
        sResult = -1;
    }

    return sResult;
}
} // namespace cmd
