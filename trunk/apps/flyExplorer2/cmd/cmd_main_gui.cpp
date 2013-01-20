//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_main_gui.h"

#include "MainFrame.h"
#include "ExplorerView.h"
#include "ExplorerPane.h"
#include "ExplorerCtrl.h"
#include "AddressBar.h"
#include "SearchDlg.h"
#include "CtrlId.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
xpr_sint_t ShowToolBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.isBandVislble(AFX_IDW_TOOLBAR) == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void ShowToolBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sVisible = sMainFrame->m_wndReBar.isBandVislble(AFX_IDW_TOOLBAR);
    sMainFrame->m_wndReBar.setBandVisible(AFX_IDW_TOOLBAR, !sVisible);
}

xpr_sint_t ShowDriveBarCommand::canExecute(CommandContext &aContext)
{
    xpr_bool_t sState = StateEnable;

    if (gOpt->mDriveBar == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void ShowDriveBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sVisible = !gOpt->mDriveBar;

    if (gOpt->mDriveBarViewSplit == XPR_TRUE)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = sMainFrame->getViewCount();
        ExplorerPane *sExplorerPane;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerPane = sMainFrame->getExplorerPane(i);
            if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
                sExplorerPane->visibleDrivePathBar(sVisible);
        }
    }
    else
    {
        sMainFrame->m_wndReBar.setBandVisible(AFX_IDW_DRIVE_BAR, sVisible);
    }

    gOpt->mDriveBar = sVisible;
}

xpr_sint_t ShowBookmarkBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.isBandVislble(AFX_IDW_BOOKMARK_BAR) == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void ShowBookmarkBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sVisible = sMainFrame->m_wndReBar.isBandVislble(AFX_IDW_BOOKMARK_BAR);
    sMainFrame->m_wndReBar.setBandVisible(AFX_IDW_BOOKMARK_BAR, !sVisible);
}

xpr_sint_t ShowAddressBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        if (sExplorerPane->isVisibleAddressBar() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void ShowAddressBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
        sExplorerPane->visibleAddressBar(!sExplorerPane->isVisibleAddressBar());
}

xpr_sint_t ShowPathBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        if (sExplorerPane->isVisiblePathBar() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void ShowPathBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
        sExplorerPane->visiblePathBar(!sExplorerPane->isVisiblePathBar());
}

xpr_sint_t ShowActivateWndCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        if (sExplorerView->isVisibleActivateBar() == XPR_TRUE)
            sState |= StateCheck;
    }

    return sState;
}

void ShowActivateWndCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerView *sExplorerView = sMainFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
        sExplorerView->visibleActivateBar(!sExplorerView->isVisibleActivateBar());
}

xpr_sint_t AddressBarDropCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        if (sExplorerPane->isVisibleAddressBar())
            sState |= StateEnable;
    }

    return sState;
}

void AddressBarDropCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NULL(sExplorerPane))
        return;

    AddressBar *sAddressBar = sExplorerPane->getAddressBar();
    if (XPR_IS_NULL(sAddressBar) || XPR_IS_NULL(sAddressBar->m_hWnd))
        return;

    HWND sHwnd = ::GetFocus();
    CWnd *sWnd = sAddressBar->GetWindow(GW_CHILD);
    while (XPR_IS_NOT_NULL(sWnd))
    {
        // ComboBox's ListBox
        if (sWnd->m_hWnd == sHwnd)
            break;

        sWnd = sWnd->GetWindow(GW_HWNDNEXT);
    }

    if (XPR_IS_NULL(sWnd))
    {
        sAddressBar->SetFocus();
        sAddressBar->ShowDropDown();
    }
}

xpr_sint_t DriveBarShortTextCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isDriveShortText())
        sState |= StateRadio;

    return sState;
}

void DriveBarShortTextCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    gOpt->mDriveBarShortText = XPR_TRUE;

    if (gOpt->mDriveBarViewSplit == XPR_TRUE)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = sMainFrame->getViewCount();
        ExplorerPane *sExplorerPane;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerPane = sMainFrame->getExplorerPane(i);
            if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
                sExplorerPane->setDrivePathBarShortText(gOpt->mDriveBarShortText);
        }
    }
    else
    {
        DriveToolBar *sDriveToolBar = (DriveToolBar *)&sMainFrame->m_wndReBar.mDriveToolBar;
        sDriveToolBar->setShortText(gOpt->mDriveBarShortText);
    }
}

xpr_sint_t DriveBarLongTextCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isDriveShortText() == XPR_FALSE)
        sState |= StateRadio;

    return sState;
}

void DriveBarLongTextCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    gOpt->mDriveBarShortText = XPR_FALSE;

    if (gOpt->mDriveBarViewSplit == XPR_TRUE)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = sMainFrame->getViewCount();
        ExplorerPane *sExplorerPane;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerPane = sMainFrame->getExplorerPane(i);
            if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
                sExplorerPane->setDrivePathBarShortText(gOpt->mDriveBarShortText);
        }
    }
    else
    {
        DriveToolBar *sDriveToolBar = (DriveToolBar *)&sMainFrame->m_wndReBar.mDriveToolBar;
        sDriveToolBar->setShortText(gOpt->mDriveBarShortText);
    }
}

xpr_sint_t DriveBarWrapableCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = 0;

    if (gOpt->mDriveBarViewSplit == XPR_FALSE)
        sState |= StateEnable;

    if (sMainFrame->m_wndReBar.isWrapable(AFX_IDW_DRIVE_BAR) == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void DriveBarWrapableCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_uint_t sId = AFX_IDW_DRIVE_BAR;
    sMainFrame->m_wndReBar.setWrapable(sId, !sMainFrame->m_wndReBar.isWrapable(sId));
}

xpr_sint_t DriveBarEachAssignCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (gOpt->mDriveBarViewSplit == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void DriveBarEachAssignCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setDriveViewSplit(!gOpt->mDriveBarViewSplit);
}

xpr_sint_t DriveBarLeftPlaceCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isDriveViewSplitLeft() == XPR_TRUE)
        sState |= StateRadio;

    return sState;
}

void DriveBarLeftPlaceCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t i;
    xpr_sint_t sViewCount = sMainFrame->getViewCount();
    ExplorerPane *sExplorerPane;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerPane = sMainFrame->getExplorerPane(i);
        if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
        {
            gOpt->mLeftDriveBarViewSplit = XPR_TRUE;
            sExplorerPane->setDrivePathBarLeft(gOpt->mLeftDriveBarViewSplit);
        }
    }
}

xpr_sint_t DriveBarRightPlaceCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isDriveViewSplitLeft() == XPR_FALSE)
        sState |= StateRadio;

    return sState;
}

void DriveBarRightPlaceCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_sint_t i;
    xpr_sint_t sViewCount = sMainFrame->getViewCount();
    ExplorerPane *sExplorerPane;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerPane = sMainFrame->getExplorerPane(i);
        if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
        {
            gOpt->mLeftDriveBarViewSplit = XPR_FALSE;
            sExplorerPane->setDrivePathBarLeft(gOpt->mLeftDriveBarViewSplit);
        }
    }
}

xpr_sint_t DriveBarRefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void DriveBarRefreshCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->m_wndReBar.mDriveToolBar.refresh();
}

xpr_sint_t BookmarkBarNoNameCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.mBookmarkToolBar.isText() == XPR_FALSE)
        sState |= StateRadio;

    return sState;
}

void BookmarkBarNoNameCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->m_wndReBar.mBookmarkToolBar.setText(XPR_FALSE);
}

xpr_sint_t BookmarkBarNameCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.mBookmarkToolBar.isText() == XPR_TRUE)
        sState |= StateRadio;

    return sState;
}

void BookmarkBarNameCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->m_wndReBar.mBookmarkToolBar.setText(XPR_TRUE);
}

xpr_sint_t BookmarkBarWrapableCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.isWrapable(AFX_IDW_BOOKMARK_BAR) == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void BookmarkBarWrapableCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_uint_t sId = AFX_IDW_BOOKMARK_BAR;
    sMainFrame->m_wndReBar.setWrapable(sId, !sMainFrame->m_wndReBar.isWrapable(sId));
}

xpr_sint_t BookmarkBarRefreshCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void BookmarkBarRefreshCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->updateBookmark();
}

xpr_sint_t ToolLockCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->m_wndReBar.IsLocked())
        sState |= StateCheck;

    return sState;
}

void ToolLockCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->m_wndReBar.Lock(!sMainFrame->m_wndReBar.IsLocked());
}

xpr_sint_t ToolBarCustomizeCommand::canExecute(CommandContext &aContext)
{
    return StateEnable;
}

void ToolBarCustomizeCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->m_wndReBar.mMainToolBar.Customize();
}

xpr_sint_t ShowStatusBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    // TODO
    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        if (sExplorerPane->isVisibleStatusBar() == XPR_TRUE)
        {
            sState |= StateCheck;
        }
    }

    return sState;
}

void ShowStatusBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    // TODO
    ExplorerPane *sExplorerPane = sMainFrame->getExplorerPane();
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        xpr_bool_t sVisible = !sExplorerPane->isVisibleStatusBar();
        sExplorerPane->visibleStatusBar(sVisible);

        if (XPR_IS_TRUE(sVisible))
        {
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
                sExplorerCtrl->updateStatus();
        }
    }
}

xpr_sint_t NoneFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    return StateEnable;
}

void NoneFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setNoneFolderPane();
}

xpr_sint_t ShowFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isVisibleFolderPane() == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void ShowFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->showFolderPane(!sMainFrame->isVisibleFolderPane());
}

xpr_sint_t SingleFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isSingleFolderPaneMode() == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void SingleFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setSingleFolderPaneMode(XPR_TRUE);
}

xpr_sint_t EachFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isSingleFolderPaneMode() == XPR_FALSE)
        sState |= StateCheck;

    return sState;
}

void EachFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setSingleFolderPaneMode(XPR_FALSE);
}

xpr_sint_t LeftFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    return StateEnable;
}

void LeftFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setLeftFolderPane(XPR_TRUE);
}

xpr_sint_t RightFolderPaneCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    return StateEnable;
}

void RightFolderPaneCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    sMainFrame->setLeftFolderPane(XPR_FALSE);
}

xpr_sint_t ShowSearchBarCommand::canExecute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    xpr_bool_t sState = StateEnable;

    if (sMainFrame->isVisibleSearchDlg() == XPR_TRUE)
        sState |= StateCheck;

    return sState;
}

void ShowSearchBarCommand::execute(CommandContext &aContext)
{
    XPR_COMMAND_DECLARE_CTRL;

    if (sMainFrame->isVisibleSearchDlg() == XPR_TRUE)
    {
        sMainFrame->destroySearchDlg();
    }
    else
    {
        LPITEMIDLIST sFullPidl = XPR_NULL;

        if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                sFullPidl = fxb::CopyItemIDList(sTvItemData->mFullPidl);
            }
        }

        sMainFrame->showSearchDlg(sFullPidl);
    }
}
} // namespace cmd
