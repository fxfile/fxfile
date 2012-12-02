//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "OptionMgr.h"

#include "fxb/fxb_ini_file.h"
#include "fxb/fxb_filter.h"
#include "fxb/fxb_file_ass.h"
#include "fxb/fxb_thumbnail.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_sys_img_list.h"

#include "Option.h"
#include "MainFrame.h"
#include "ExplorerView.h"
#include "ExplorerCtrl.h"
#include "FolderCtrl.h"
#include "PathBar.h"
#include "ActivateBar.h"
#include "AddressBar.h"
#include "PicViewer.h"
#include "CfgPath.h"

#include <atlbase.h> // for CRegKey

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const xpr_sint_t OPT_COUNT = 6;

static const xpr_tchar_t LAUNCHER_DEBUG_EXE[] = XPR_STRING_LITERAL("fxLauncher_dbg.exe");
static const xpr_tchar_t LAUNCHER_EXE[]       = XPR_STRING_LITERAL("fxLauncher.exe");
static const xpr_tchar_t LAUNCHER_WND_CLASS[] = XPR_STRING_LITERAL("fxLauncher");

static const xpr_uint_t WM_LAUNCHER_COMMAND_LOAD   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Load"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_TRAY   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Tray"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_HOTKEY = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: HotKey"));
static const xpr_uint_t WM_LAUNCHER_COMMAND_EXIT   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxLauncher: Exit"));

OptionMgr::OptionMgr(void)
    : mOption(new Option)
    , mModifiedHidden(XPR_TRUE)
    , mModifiedSystem(XPR_TRUE)
    , mOnlyExplorerView(XPR_FALSE)
{
}

OptionMgr::~OptionMgr(void)
{
    XPR_SAFE_DELETE(mOption);
}

void OptionMgr::initDefault(void)
{
    mOption->initDefault();
    fxb::FilterMgr::instance().initDefault();
    fxb::FileAssMgr::instance().initDefault();
    fxb::SizeFormat::instance().initDefault();

    saveOption();
    saveFilter();
    saveFileAss();
    saveSizeFormat();

    mModifiedHidden = XPR_TRUE;
}

void OptionMgr::load(xpr_bool_t &aInitCfg)
{
    xpr_time_t sTime1 = xpr::timer_ms();

    aInitCfg = XPR_FALSE;

    // Load Option
    if (loadOption() == XPR_FALSE)
    {
        saveOption();
        aInitCfg = XPR_TRUE;
    }

    // Load Filter
    if (loadFilter() == XPR_FALSE)
        saveFilter();

    // Load File Association
    if (loadFileAss() == XPR_FALSE)
        saveFileAss();

    // Load Size Format
    if (loadSizeFormat() == XPR_FALSE)
        saveSizeFormat();

    xpr_time_t sTime2 = xpr::timer_ms();

    XPR_TRACE(XPR_STRING_LITERAL("Loading time of configuration file = %I64dms\n"), sTime2 - sTime1);
}

xpr_bool_t OptionMgr::save(void)
{
    // Save Option
    saveOption();

    // Save Filter
    saveFilter();

    // Save File Association
    saveFileAss();

    // Save Size Format
    saveSizeFormat();

    return XPR_TRUE;
}

xpr_bool_t OptionMgr::loadOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    return mOption->loadFromFile(sPath);
}

xpr_bool_t OptionMgr::saveOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeConfig, sPath, XPR_MAX_PATH);

    return mOption->saveToFile(sPath);
}

xpr_bool_t OptionMgr::saveMainOption(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeMain, sPath, XPR_MAX_PATH);

    return mOption->saveToFile(sPath, XPR_TRUE);
}

xpr_bool_t OptionMgr::loadFilter(void)
{
    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeFilter, sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = sFilterMgr.loadFromFile(sPath);
    if (XPR_IS_FALSE(sResult))
        sFilterMgr.initDefault();

    return sResult;
}

xpr_bool_t OptionMgr::saveFilter(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeFilter, sPath, XPR_MAX_PATH);

    return fxb::FilterMgr::instance().saveToFile(sPath);
}

xpr_bool_t OptionMgr::loadFileAss(void)
{
    fxb::FileAssMgr &sFileAssMgr = fxb::FileAssMgr::instance();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeFileAss, sPath, XPR_MAX_PATH);

    xpr_bool_t sResult = sFileAssMgr.loadFromFile(sPath);
    if (XPR_IS_FALSE(sResult))
        sFileAssMgr.initDefault();

    return sResult;
}

xpr_bool_t OptionMgr::saveFileAss(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeFileAss, sPath, XPR_MAX_PATH);

    return fxb::FileAssMgr::instance().saveToFile(sPath);
}

xpr_bool_t OptionMgr::loadSizeFormat(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeSizeFmt, sPath, XPR_MAX_PATH);

    return fxb::SizeFormat::instance().loadFromFile(sPath);
}

xpr_bool_t OptionMgr::saveSizeFormat(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeSizeFmt, sPath, XPR_MAX_PATH);

    return fxb::SizeFormat::instance().saveToFile(sPath);
}

void OptionMgr::applyFolderCtrl(xpr_bool_t aLoading)
{
    xpr_sint_t i, j;
    xpr_sint_t sViewCount = gFrame->getViewCount();
    FolderCtrl *sFolderCtrl;

    for (i = 0; i < sViewCount; ++i)
    {
        sFolderCtrl = gFrame->getFolderCtrl(i);
        if (XPR_IS_NOT_NULL(sFolderCtrl))
        {
            for (j = 0; j < OPT_COUNT; ++j)
                applyFolderCtrl(sFolderCtrl, j, aLoading);
        }
    }
}

void OptionMgr::applyFolderCtrl(FolderCtrl *aFolderCtrl, xpr_bool_t aLoading)
{
    xpr_sint_t i;
    for (i = 0; i < OPT_COUNT; ++i)
        applyFolderCtrl(aFolderCtrl, i, aLoading);
}

void OptionMgr::applyFolderCtrl(xpr_sint_t aIndex, xpr_bool_t aLoading)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = gFrame->getViewCount();
    FolderCtrl *sFolderCtrl;

    for (i = 0; i < sViewCount; ++i)
    {
        sFolderCtrl = gFrame->getFolderCtrl(i);
        if (XPR_IS_NOT_NULL(sFolderCtrl))
            applyFolderCtrl(sFolderCtrl, aIndex, aLoading);
    }
}

void OptionMgr::applyFolderCtrl(FolderCtrl *aFolderCtrl, xpr_sint_t aIndex, xpr_bool_t aLoading)
{
    if (XPR_IS_NULL(aFolderCtrl))
        return;

    if (aIndex == 0) // general
    {
        DWORD sStyle = TVS_TRACKSELECT | TVS_SINGLEEXPAND;
        if (mOption->mMouseClick == MOUSE_ONE_CLICK) aFolderCtrl->ModifyStyle(0, sStyle);
        else                                         aFolderCtrl->ModifyStyle(sStyle, 0);

        if ((XPR_IS_FALSE(aLoading)) && (XPR_IS_TRUE(mModifiedHidden) || XPR_IS_TRUE(mModifiedSystem)))
            aFolderCtrl->setHiddenSystem(mModifiedHidden, mModifiedSystem);

        if (XPR_IS_TRUE(mOption->mFolderTreeIsItemHeight))
        {
            aFolderCtrl->SetItemHeight(mOption->mFolderTreeItemHeight);
        }
        else
        {
            aFolderCtrl->SetItemHeight(-1);
        }
    }
    else if (aIndex == 2)
    {
        xpr_sint_t sViewIndex = aFolderCtrl->getViewIndex();
        if (mOption->mFolderTreeCustomFolder[sViewIndex] == FOLDER_TREE_CUSTOM_USER_DEFINED)
        {
            aFolderCtrl->SetBkColor(mOption->mFolderTreeBkgndColor[sViewIndex]);
            aFolderCtrl->SetTextColor(mOption->mFolderTreeTextColor[sViewIndex]);
        }
        else if (mOption->mFolderTreeCustomFolder[sViewIndex] == FOLDER_TREE_CUSTOM_EXPLORER)
        {
            if (mOption->mExplorerCustomFolder[sViewIndex])
            {
                aFolderCtrl->SetBkColor(mOption->mExplorerBkgndColor[sViewIndex]);
                aFolderCtrl->SetTextColor(mOption->mExplorerTextColor[sViewIndex]);
            }
        }
        else
        {
            aFolderCtrl->SetBkColor(::GetSysColor(COLOR_WINDOW));
            aFolderCtrl->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
        }

        xpr_tchar_t *sCustomFontText = mOption->mCustomFontText;
        if (XPR_IS_TRUE(mOption->mFolderTreeCustomFont))
            sCustomFontText = mOption->mFolderTreeCustomFontText;

        aFolderCtrl->setCustomFont(sCustomFontText);
    }
    else if (aIndex == 3)
    {
        HTREEITEM sTreeItem = aFolderCtrl->GetSelectedItem();
        if (XPR_IS_NOT_NULL(sTreeItem))
        {
            CRect sRect;
            aFolderCtrl->GetItemRect(sTreeItem, &sRect, XPR_FALSE);
            aFolderCtrl->InvalidateRect(sRect, XPR_FALSE);
        }

        aFolderCtrl->setDragContents(!mOption->mDragNoContents);
    }
}

void OptionMgr::applyExplorerView(xpr_bool_t aLoading)
{
    Splitter *sSplitter = gFrame->getSplitter();
    if (XPR_IS_NULL(sSplitter))
        return;

    xpr_sint_t i, j, k;
    xpr_sint_t sRow = sSplitter->getRowCount();
    xpr_sint_t sColumn = sSplitter->getColumnCount();
    ExplorerView *sExplorerView;
    CWnd *sWnd;

    for (i = 0; i < sRow; ++i)
    {
        for (j = 0; j < sColumn; ++j)
        {
            sWnd = sSplitter->getPaneWnd(i, j);
            if (XPR_IS_NULL(sWnd))
                continue;

            sExplorerView = dynamic_cast<ExplorerView *>(sWnd);
            if (XPR_IS_NOT_NULL(sExplorerView))
            {
                for (k = 0; k < OPT_COUNT; ++k)
                    applyExplorerView(sExplorerView, k, aLoading);
            }
        }
    }
}

void OptionMgr::applyExplorerView(ExplorerView *aExplorerView, xpr_bool_t aLoading)
{
    if (XPR_IS_NULL(aExplorerView))
        return;

    xpr_sint_t i;
    for (i = 0; i < OPT_COUNT; ++i)
    {
        applyExplorerView(aExplorerView, i, aLoading);
    }
}

void OptionMgr::applyExplorerView(xpr_sint_t aIndex, xpr_bool_t aLoading)
{
    Splitter *sSplitter = gFrame->getSplitter();
    if (XPR_IS_NULL(sSplitter))
        return;

    xpr_sint_t i, j;
    xpr_sint_t sRow = sSplitter->getRowCount();
    xpr_sint_t sColumn = sSplitter->getColumnCount();
    ExplorerView *sExplorerView;
    CWnd *sWnd;

    for (i = 0; i < sRow; ++i)
    {
        for (j = 0; j < sColumn; ++j)
        {
            sWnd = sSplitter->getPaneWnd(i, j);
            if (XPR_IS_NOT_NULL(sWnd))
            {
                sExplorerView = dynamic_cast<ExplorerView *>(sWnd);
                if (XPR_IS_NOT_NULL(sExplorerView))
                {
                    applyExplorerView(sExplorerView, aIndex, aLoading);
                }
            }
        }
    }
}

void OptionMgr::applyExplorerView(ExplorerView *aExplorerView, xpr_sint_t aIndex, xpr_bool_t aLoading)
{
    if (XPR_IS_NULL(aExplorerView))
        return;

    xpr_sint_t i, sTabCount;
    ExplorerCtrl *sExplorerCtrl = XPR_NULL;
    AddressBar   *sAddressBar   = aExplorerView->getAddressBar();
    PathBar      *sPathBar      = aExplorerView->getPathBar();
    ActivateBar  *sActivateBar  = aExplorerView->getActivateBar();

    if (aIndex == 0) // general
    {
        if (XPR_IS_FALSE(mOnlyExplorerView))
        {
            sTabCount = aExplorerView->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerCtrl = aExplorerView->getExplorerCtrl(i);
                if (XPR_IS_NULL(sExplorerCtrl))
                    continue;

                applyExplorerCtrl(sExplorerCtrl, aIndex, aLoading);
            }
        }
    }
    else if (aIndex == 1) // contents
    {
        aExplorerView->setContentsStyle(mOption->mContentsStyle);

        if (XPR_IS_FALSE(aLoading))
            gFrame->updateBookmark();

        if (mOption->mContentsStyle != CONTENTS_EXPLORER)
        {
            if (XPR_IS_NOT_NULL(gFrame->mPicViewer))
                gFrame->mPicViewer->setDocking(XPR_FALSE);
        }

        aExplorerView->recalcLayout();
        aExplorerView->invalidateContentsWnd();

        if (XPR_IS_FALSE(mOnlyExplorerView))
        {
            sTabCount = aExplorerView->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerCtrl = aExplorerView->getExplorerCtrl(i);
                if (XPR_IS_NULL(sExplorerCtrl))
                    continue;

                applyExplorerCtrl(sExplorerCtrl, aIndex, aLoading);
            }
        }
    }
    else if (aIndex == 2) // explorer
    {
        if (XPR_IS_FALSE(mOnlyExplorerView))
        {
            sTabCount = aExplorerView->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerCtrl = aExplorerView->getExplorerCtrl(i);
                if (XPR_IS_NULL(sExplorerCtrl))
                    continue;

                applyExplorerCtrl(sExplorerCtrl, aIndex, aLoading);
            }
        }

        aExplorerView->setPathBarMode(mOption->mPathBarRealPath);

        if (XPR_IS_NOT_NULL(sAddressBar))
        {
            sAddressBar->setCustomFont(mOption->mCustomFontText);
        }

        if (XPR_IS_NOT_NULL(sPathBar))
        {
            sPathBar->setIconDisp(mOption->mPathBarIcon);
            sPathBar->setCustomFont(mOption->mCustomFontText);
            sPathBar->setHighlightColor(mOption->mPathBarHighlight, mOption->mPathBarHighlightColor);
        }

        if (XPR_IS_NOT_NULL(sActivateBar) && XPR_IS_NOT_NULL(sActivateBar->m_hWnd))
            sActivateBar->setActivateBackColor(mOption->mActiveViewColor);

        if (XPR_IS_FALSE(aLoading))
        {
            if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
            {
                sAddressBar->Invalidate();
                sAddressBar->UpdateWindow();
            }
        }

        aExplorerView->recalcLayout();
    }
    else if (aIndex == 3)
    {
        if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
            sAddressBar->setAutoComplete();

        if (XPR_IS_FALSE(mOnlyExplorerView))
        {
            sTabCount = aExplorerView->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerCtrl = aExplorerView->getExplorerCtrl(i);
                if (XPR_IS_NULL(sExplorerCtrl))
                    continue;

                applyExplorerCtrl(sExplorerCtrl, aIndex, aLoading);
            }

            aExplorerView->setDragContents(!mOption->mDragNoContents);
        }
    }
    else if (aIndex == 5)
    {
        fxb::Thumbnail &sThumbnail = fxb::Thumbnail::instance();

        sThumbnail.setThumbSize(CSize(mOption->mThumbnailWidth, mOption->mThumbnailHeight));
        sThumbnail.setThumbPriority(mOption->mThumbnailPriority);

        if (XPR_IS_FALSE(mOnlyExplorerView))
        {
            sTabCount = aExplorerView->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerCtrl = aExplorerView->getExplorerCtrl(i);
                if (XPR_IS_NULL(sExplorerCtrl))
                    continue;

                applyExplorerCtrl(sExplorerCtrl, aIndex, aLoading);
            }
        }
    }
}

void OptionMgr::applyExplorerCtrl(ExplorerCtrl *aExplorerCtrl, xpr_bool_t aLoading)
{
    xpr_sint_t i;
    for (i = 0; i < OPT_COUNT; ++i)
        applyExplorerCtrl(aExplorerCtrl, i, aLoading);
}

void OptionMgr::applyExplorerCtrl(ExplorerCtrl *aExplorerCtrl, xpr_sint_t aIndex, xpr_bool_t aLoading)
{
    if (XPR_IS_NULL(aExplorerCtrl))
        return;

    if (aIndex == 0)
    {
        DWORD sExStyle = aExplorerCtrl->GetExtendedStyle();
        if (mOption->mMouseClick == MOUSE_ONE_CLICK)
        {
            sExStyle |= LVS_EX_UNDERLINEHOT;
            sExStyle |= LVS_EX_ONECLICKACTIVATE;
            sExStyle |= LVS_EX_TRACKSELECT;

        }
        else
        {
            sExStyle &= ~LVS_EX_UNDERLINEHOT;
            sExStyle &= ~LVS_EX_ONECLICKACTIVATE;
            sExStyle &= ~LVS_EX_TRACKSELECT;
        }

        if (XPR_IS_TRUE(mOption->mTooltip)) sExStyle |= LVS_EX_INFOTIP;
        else                                sExStyle &= ~LVS_EX_INFOTIP;

        aExplorerCtrl->SetExtendedStyle(sExStyle);

        if ((XPR_IS_FALSE(aLoading)) && (XPR_IS_TRUE(mModifiedHidden) || XPR_IS_TRUE(mModifiedSystem)))
            aExplorerCtrl->setHiddenSystem(mModifiedHidden, mModifiedSystem);

        if (XPR_IS_FALSE(aLoading))
            aExplorerCtrl->refresh();
    }
    else if (aIndex == 1)
    {
        aExplorerCtrl->GetDlgItem(0)->Invalidate();

        if (XPR_IS_FALSE(aLoading))
            aExplorerCtrl->refresh();
    }
    else if (aIndex == 2)
    {
        DWORD sExStyle = aExplorerCtrl->GetExtendedStyle();

        if (XPR_IS_TRUE(mOption->mGridLines))     sExStyle |= LVS_EX_GRIDLINES;
        else                                      sExStyle &= ~LVS_EX_GRIDLINES;
        if (XPR_IS_TRUE(mOption->mFullRowSelect)) sExStyle |= LVS_EX_FULLROWSELECT;
        else                                      sExStyle &= ~LVS_EX_FULLROWSELECT;

        aExplorerCtrl->SetExtendedStyle(sExStyle);

        xpr_sint_t sViewIndex = aExplorerCtrl->getViewIndex();
        if (XPR_IS_TRUE(mOption->mExplorerCustomFolder[sViewIndex]))
        {
            aExplorerCtrl->SetBkImage((xpr_tchar_t *)(const xpr_tchar_t *)mOption->mExplorerBkgndFile[sViewIndex]);
            aExplorerCtrl->SetBkColor(mOption->mExplorerBkgndColor[sViewIndex]);
            aExplorerCtrl->SetTextBkColor(mOption->mExplorerBkgndColor[sViewIndex]);
            aExplorerCtrl->SetTextColor(mOption->mExplorerTextColor[sViewIndex]);
        }
        else
        {
            aExplorerCtrl->SetBkImage(XPR_STRING_LITERAL(""));
            aExplorerCtrl->SetBkColor(::GetSysColor(COLOR_WINDOW));
            aExplorerCtrl->SetTextBkColor(::GetSysColor(COLOR_WINDOW));
            aExplorerCtrl->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
        }

        fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

        sSysImgListMgr.getCustomImgList();
        if (XPR_IS_TRUE(mOption->mExplorerCustomIcon))
            aExplorerCtrl->setImageList(&sSysImgListMgr.mCusImgList32, &sSysImgListMgr.mCusImgList16);
        else
            aExplorerCtrl->setImageList(&sSysImgListMgr.mSysImgList32, &sSysImgListMgr.mSysImgList16);

        aExplorerCtrl->setCustomFont(mOption->mCustomFontText);

        if (XPR_IS_FALSE(aLoading))
        {
            aExplorerCtrl->refresh();
        }
    }
    else if (aIndex == 3)
    {
        aExplorerCtrl->setMaxHistory(mOption->mHistoryCount);
        aExplorerCtrl->setMaxBackward(mOption->mBackwardCount);

        aExplorerCtrl->setDragContents(!mOption->mDragNoContents);
    }
    else if (aIndex == 5)
    {
        if (aExplorerCtrl->getViewStyle() == LVS_THUMBNAIL)
            aExplorerCtrl->setViewStyle(LVS_THUMBNAIL, XPR_TRUE);
    }
}

void OptionMgr::applyEtc(void)
{
    xpr_sint_t i;
    for (i = 0; i < OPT_COUNT; ++i)
        applyEtc(i);
}

void OptionMgr::applyEtc(xpr_sint_t aIndex)
{
    if (aIndex == 3)
    {
        if (XPR_IS_FALSE(mOption->mTray)) gFrame->destroyTray();
        else                              gFrame->createTray();

        theApp.setSingleInstance(mOption->mSingleInstance);

        CRegKey sRegKey;
        if (sRegKey.Open(HKEY_CLASSES_ROOT, XPR_STRING_LITERAL("Folder\\Shell")) == ERROR_SUCCESS)
        {
            if (XPR_IS_TRUE(mOption->mRegShellContextMenu))
            {
                // HKEY_CLASSES_ROOT\Folder\shell\open with flyExplorer\command
                // (default) : "C:\Program Files\flyExplorer\flyExplorer.exe" "%1"

                xpr_tchar_t sCmd[XPR_MAX_PATH + 2 + 1] = {0};
                fxb::GetModuleDir(sCmd+1, XPR_MAX_PATH + 1);
                sCmd[0] = XPR_STRING_LITERAL('\"');
                _tcscat(sCmd, XPR_STRING_LITERAL("\\flyExplorer.exe\" \"%1\""));

                xpr_tchar_t sKey[0xff] = {0};
                _stprintf(sKey, XPR_STRING_LITERAL("%sflyExplorer%s\\command"), theApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_flyExplorer.prefix")), theApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_flyExplorer.suffix")));

                sRegKey.SetKeyValue(sKey, sCmd);
            }
            else
            {
                xpr_tchar_t sKey[0xff] = {0};
                _stprintf(sKey, XPR_STRING_LITERAL("%sflyExplorer%s"), theApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_flyExplorer.prefix")), theApp.loadString(XPR_STRING_LITERAL("shell_menu.open_with_flyExplorer.suffix")));

                sRegKey.RecurseDeleteKey(sKey);
            }
        }

        sRegKey.Close();

        gFrame->setDragContents(!mOption->mDragNoContents);

        if (XPR_IS_TRUE(mOption->mLauncher))
        {
            HWND sHwnd = ::FindWindow(LAUNCHER_WND_CLASS, XPR_NULL);
            if (XPR_IS_NULL(sHwnd))
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetModuleDir(sPath, XPR_MAX_PATH);

                _tcscat(sPath, XPR_STRING_LITERAL("\\"));

#ifdef XPR_CFG_BUILD_DEBUG
                _tcscat(sPath, LAUNCHER_DEBUG_EXE);
#else
                _tcscat(sPath, LAUNCHER_EXE);
#endif

                xpr_tchar_t szParameters[0xff] = {0};
                DWORD dwGlobalHotKey = mOption->mLauncherGlobalHotKey;
                _stprintf(szParameters, XPR_STRING_LITERAL("/load=%d, /tray=%d /hotkey=%d"), mOption->mLauncher, mOption->mLauncherTray, dwGlobalHotKey);

                ::ShellExecute(XPR_NULL, XPR_STRING_LITERAL("open"), sPath, szParameters, XPR_NULL, 0);
            }
            else
            {
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_LOAD,   (WPARAM)mOption->mLauncher,             0);
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_HOTKEY, (WPARAM)mOption->mLauncherGlobalHotKey, 0);
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_TRAY,   (WPARAM)mOption->mLauncherTray,         0);
            }
        }
        else
        {
            HWND sHwnd = ::FindWindow(LAUNCHER_WND_CLASS, XPR_NULL);
            if (XPR_IS_NOT_NULL(sHwnd))
            {
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_LOAD,   (WPARAM)mOption->mLauncher,             0);
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_HOTKEY, (WPARAM)mOption->mLauncherGlobalHotKey, 0);
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_TRAY,   (WPARAM)mOption->mLauncherTray,         0);
                ::PostMessage(sHwnd, WM_LAUNCHER_COMMAND_EXIT,   0,                                      0);
            }
        }

        if (sRegKey.Open(HKEY_CURRENT_USER, XPR_STRING_LITERAL("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) == ERROR_SUCCESS)
        {
            if (XPR_IS_TRUE(mOption->mLauncherWinStartup))
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetModuleDir(sPath, XPR_MAX_PATH);

                _tcscat(sPath, XPR_STRING_LITERAL("\\"));

#ifdef XPR_CFG_BUILD_DEBUG
                _tcscat(sPath, LAUNCHER_DEBUG_EXE);
#else
                _tcscat(sPath, LAUNCHER_EXE);
#endif

                sRegKey.SetStringValue(XPR_STRING_LITERAL("fxLauncher"), sPath);
            }
            else
            {
                sRegKey.DeleteValue(XPR_STRING_LITERAL("fxLauncher"));
            }
        }

        sRegKey.Close();
    }
}

xpr_bool_t OptionMgr::FontDlgToString(CFontDialog &aDlg, xpr_tchar_t *aFontText)
{
    if (XPR_IS_NULL(aFontText))
        return XPR_FALSE;

    xpr_sint_t sWeight = aDlg.GetWeight();        // return the chosen font weight
    if (sWeight == FW_NORMAL || sWeight == FW_BOLD)
        sWeight = aDlg.IsBold();

    _stprintf(aFontText, XPR_STRING_LITERAL("%s,%d,%d,%d,%d,%d"), aDlg.GetFaceName(), aDlg.GetSize() / 10,
        sWeight, aDlg.IsItalic(), aDlg.IsStrikeOut(), aDlg.IsUnderline());

    return XPR_TRUE;
}

xpr_bool_t OptionMgr::StringToLogFont(const xpr_tchar_t *aFontText, LOGFONT &aLogFont)
{
    if (XPR_IS_NULL(aFontText))
        return XPR_FALSE;

    xpr_tchar_t sFontText[MAX_FONT_TEXT + 1] = {0};
    _tcscpy(sFontText, aFontText);

    memset(&aLogFont, 0, sizeof(LOGFONT));

    xpr_sint_t i;
    xpr_tchar_t *sSplit = sFontText;
    xpr_tchar_t *sSplit2;
    for (i = 0; i <= 5; ++i)
    {
        if (*sSplit == XPR_STRING_LITERAL('\0'))
            break;

        sSplit2 = _tcschr(sSplit, XPR_STRING_LITERAL(','));
        if (XPR_IS_NOT_NULL(sSplit2))
            *sSplit2 = XPR_STRING_LITERAL('\0');

        switch (i)
        {
        case 0: _tcscpy(aLogFont.lfFaceName, sSplit);                               break;
        case 1: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfHeight);     break;
        case 2: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfWeight);     break;
        case 3: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfItalic);     break;
        case 4: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfStrikeOut);  break;
        case 5: _stscanf(sSplit, XPR_STRING_LITERAL("%d"), &aLogFont.lfUnderline);  break;
        }

        sSplit += _tcslen(sSplit) + 1;
    }

    CClientDC sClientDC(CWnd::GetDesktopWindow());
    aLogFont.lfCharSet = DEFAULT_CHARSET;
    aLogFont.lfHeight  = -MulDiv(aLogFont.lfHeight, sClientDC.GetDeviceCaps(LOGPIXELSY), 72);
    if (aLogFont.lfWeight    == 1) aLogFont.lfWeight    = FW_BOLD;
    if (aLogFont.lfStrikeOut != 0) aLogFont.lfStrikeOut = XPR_TRUE;
    if (aLogFont.lfUnderline != 0) aLogFont.lfUnderline = XPR_TRUE;

    return XPR_TRUE;
}
