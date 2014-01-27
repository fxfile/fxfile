//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "explorer_view.h"

#include "sys_img_list.h"
#include "bookmark.h"
#include "context_menu.h"
#include "file_scrap.h"
#include "program_opts.h"
#include "program_ass.h"
#include "clip_format.h"
#include "size_format.h"
#include "clipboard.h"

#include "gui/MemDC.h"
#include "gui/TabCtrl.h"

#include "explorer_view_observer.h"
#include "main_frame.h"
#include "explorer_pane.h"
#include "explorer_ctrl.h"
#include "explorer_ctrl_print.h"
#include "folder_pane.h"
#include "folder_ctrl.h"
#include "print_preview_view_ex.h"
#include "activate_bar.h"
#include "option.h"
#include "search_result_pane.h"
#include "search_result_ctrl.h"
#include "file_scrap_pane.h"
#include "tab_pane.h"

#include "cmd/router/cmd_parameters.h"
#include "cmd/router/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#include "cmd/param_exec_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
static const xpr_sint_t kTabHeight = 27;

//
// control id
//
enum
{
    CTRL_ID_TAB_CTRL = 50,
    CTRL_ID_FOLDER_PANE,
    CTRL_ID_ACTIVATE_BAR,
    CTRL_ID_TAB_PANE = 200,
};

class ExplorerView::TabData
{
    friend class ExplorerView;

public:
    TabData(TabType aTabType)
        : mTabPane(XPR_NULL), mTabPaneId(0)
    {
    }

    virtual ~TabData(void)
    {
        if (XPR_IS_NOT_NULL(mTabPane))
        {
            if (mTabPane->isSharedPane() == XPR_FALSE)
            {
                DESTROY_DELETE(mTabPane);
            }
            else
            {
                mTabPane->destroySubPane(mTabPaneId);
                mTabPane->ShowWindow(SW_HIDE);
                mTabPane = XPR_NULL;
            }
        }
    }

protected:
    xpr_bool_t canDuplicatePane(void) const
    {
        if (XPR_IS_NULL(mTabPane))
            return XPR_FALSE;

        return mTabPane->canDuplicatePane();
    }

    TabType getTabType(void) const
    {
        if (XPR_IS_NULL(mTabPane))
            return TabTypeNone;

        return mTabPane->getTabType();
    }

    xpr_bool_t isTabType(TabType aTabType) const
    {
        return (getTabType() == aTabType) ? XPR_TRUE : XPR_FALSE;
    }

    CWnd *getSubPaneWnd(void) const
    {
        if (XPR_IS_NULL(mTabPane))
            return XPR_NULL;

        return mTabPane->getSubPane(mTabPaneId);
    }

protected:
    TabPane    *mTabPane;
    xpr_uint_t  mTabPaneId;
};

ExplorerView::ExplorerView(void)
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mInit(XPR_TRUE)
    , mTabCtrl(XPR_NULL)
    , mFolderPane(XPR_NULL)
    , mActivateBar(XPR_NULL)
    , mExplorerPane(XPR_NULL)
    , mListCtrlPrint(XPR_NULL)
    , mNextTabPaneId(CTRL_ID_TAB_PANE), mLastActivedExplorerPaneId(0)
    , mContextMenuCursor(CPoint(0, 0))
{
}

ExplorerView::~ExplorerView(void)
{
}

xpr_bool_t ExplorerView::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    //const xpr_char_t *sClassName = "MyView";

    //WNDCLASS sWndClass = {0};
    //sWndClass.lpfnWndProc   = ::DefWindowProc;
    //sWndClass.hInstance     = AfxGetInstanceHandle();
    //sWndClass.hCursor       = LoadCursor(XPR_NULL, IDC_ARROW);
    //sWndClass.style         = CS_DBLCLKS;
    //sWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    //sWndClass.lpszClassName = sClassName;
    //if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
    //    return XPR_FALSE;

    //aCreateStruct.lpszClass = sClassName;

    //aCreateStruct.dwExStyle |= WS_EX_CLIENTEDGE;
    //aCreateStruct.style &= ~WS_BORDER;

    CBrush sBrush(::GetSysColor(COLOR_WINDOW));

    HCURSOR sCursor = ::LoadCursor(XPR_NULL, MAKEINTRESOURCE(IDC_ARROW));
    aCreateStruct.lpszClass = AfxRegisterWndClass(0, sCursor, sBrush);
    if (XPR_IS_NOT_NULL(sCursor)) ::DestroyCursor(sCursor);

    aCreateStruct.style |= WS_CLIPCHILDREN;

    return super::PreCreateWindow(aCreateStruct);
}

BEGIN_MESSAGE_MAP(ExplorerView, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CAPTURECHANGED()
    ON_COMMAND(ID_FILE_PRINT_DIRECT, super::OnFilePrint)
END_MESSAGE_MAP()

void ExplorerView::setObserver(ExplorerViewObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_sint_t ExplorerView::getViewIndex(void) const
{
    return mViewIndex;
}

void ExplorerView::setViewIndex(xpr_sint_t aViewIndex)
{
    if (aViewIndex == mViewIndex)
        return;

    mViewIndex = aViewIndex;

    FolderPane *sFolderPane = getFolderPane();
    if (XPR_IS_NOT_NULL(sFolderPane))
        sFolderPane->setViewIndex(mViewIndex);

    xpr_sint_t i, sTabCount;
    ExplorerCtrl *sExplorerCtrl;

    sTabCount = getTabCount();
    for (i = 0; i < sTabCount; ++i)
    {
        sExplorerCtrl = getExplorerCtrl(i);
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
            sExplorerCtrl->setViewIndex(aViewIndex);
    }
}

XPR_INLINE void loadHistory(Option::Main::Tab &aTabOption, ExplorerCtrl &aExplorerCtrl)
{
    LPITEMIDLIST sFullPidl;
    Option::Main::HistoryList::iterator sHistoryIterator;

    HistoryDeque *sBackwardDeque = new HistoryDeque;
    HistoryDeque *sForwardDeque  = new HistoryDeque;
    HistoryDeque *sHistoryDeque  = new HistoryDeque;

    if (XPR_IS_NULL(sBackwardDeque) ||
        XPR_IS_NULL(sForwardDeque)  ||
        XPR_IS_NULL(sHistoryDeque))
    {
        XPR_SAFE_DELETE(sBackwardDeque);
        XPR_SAFE_DELETE(sForwardDeque);
        XPR_SAFE_DELETE(sHistoryDeque);
        return;
    }

    FXFILE_STL_FOR_EACH(sHistoryIterator, aTabOption.mBackwardList)
    {
        const xpr::tstring &sHistoryPath = *sHistoryIterator;

        sFullPidl = Path2Pidl(sHistoryPath);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sBackwardDeque->push_back(sFullPidl);
        }
    }

    FXFILE_STL_FOR_EACH(sHistoryIterator, aTabOption.mForwardList)
    {
        const xpr::tstring &sHistoryPath = *sHistoryIterator;

        sFullPidl = Path2Pidl(sHistoryPath);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sForwardDeque->push_back(sFullPidl);
        }
    }

    FXFILE_STL_FOR_EACH(sHistoryIterator, aTabOption.mHistoryList)
    {
        const xpr::tstring &sHistoryPath = *sHistoryIterator;

        sFullPidl = Path2Pidl(sHistoryPath);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sHistoryDeque->push_back(sFullPidl);
        }
    }

    aExplorerCtrl.loadHistory(sBackwardDeque, sForwardDeque, sHistoryDeque);

    XPR_SAFE_DELETE(sBackwardDeque);
    XPR_SAFE_DELETE(sForwardDeque);
    XPR_SAFE_DELETE(sHistoryDeque);
}

xpr_sint_t ExplorerView::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    // tab
    if (createTabCtrl() == XPR_FALSE)
        return -1;

    // splitter
    if (createSplitter() == XPR_FALSE)
        return -1;

    xpr_bool_t sLastNewTab = XPR_FALSE;

    // load last tabs
    switch (gOpt->mConfig.mExplorerInitFolderType[mViewIndex])
    {
    case INIT_TYPE_LAST_FOLDER:
    case INIT_TYPE_LAST_TAB:
        {
            xpr_sint_t         sTab;
            Option::Main::Tab *sTabOption;
            ExplorerCtrl      *sExplorerCtrl;
            Option::Main::TabDeque::iterator sIterator;

            FXFILE_STL_FOR_EACH(sIterator, gOpt->mMain.mView[mViewIndex].mTabDeque)
            {
                sTabOption = *sIterator;
                XPR_ASSERT(sTabOption != XPR_NULL);

                // new tab
                sTab = newTab(sTabOption->mPath);

                sExplorerCtrl = getExplorerCtrl(sTab);
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    // load history
                    loadHistory(*sTabOption, *sExplorerCtrl);
                }
            }

            sLastNewTab = XPR_TRUE;

            break;
        }
    }

    if (mTabCtrl->getTabCount() == 0)
    {
        newTab();

        sLastNewTab = XPR_FALSE;
    }

    if (mTabCtrl->setCurTab(gOpt->mMain.mView[mViewIndex].mCurTab) == XPR_FALSE)
    {
        mTabCtrl->setCurTab(0);
    }

    if (XPR_IS_FALSE(sLastNewTab))
    {
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            Option::Main::TabDeque::iterator sIterator = gOpt->mMain.mView[mViewIndex].mTabDeque.begin();
            if (sIterator != gOpt->mMain.mView[mViewIndex].mTabDeque.end())
            {
                Option::Main::Tab *sTabOption = *sIterator;
                XPR_ASSERT(sTabOption != XPR_NULL);

                loadHistory(*sTabOption, *sExplorerCtrl);
            }
        }
    }

    // activate bar
    visibleActivateBar(gOpt->mConfig.mShowActivateBar, XPR_TRUE);

    // folder pane
    xpr_bool_t sVisibleFolderPane = XPR_FALSE;
    if (XPR_IS_FALSE(gOpt->mMain.mSingleFolderPaneMode) && XPR_IS_TRUE(gOpt->mMain.mShowEachFolderPane[mViewIndex]))
        sVisibleFolderPane = visibleFolderPane(XPR_TRUE, XPR_TRUE);

    FolderCtrl *sFolderCtrl = getFolderCtrl();
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode) && mViewIndex == 0)
        sFolderCtrl = gFrame->getFolderCtrl();

    if (XPR_IS_NOT_NULL(sFolderCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            sFolderCtrl->searchSel(sTvItemData->mFullPidl, XPR_FALSE);
        }
    }

    if (XPR_IS_FALSE(sVisibleFolderPane))
    {
        mSplitter.split(1, 1);

        CRect sRect;
        GetClientRect(sRect);

        mSplitter.setWindowRect(sRect);
        mSplitter.moveColumn(0, gOpt->mMain.mEachFolderPaneSize[mViewIndex]);
        mSplitter.resize();
    }

    // register for drag and drop
    mDropTarget.Register(this);

    setDragContents(!gOpt->mConfig.mDragNoContents);

    mInit = XPR_FALSE;

    return 0;
}

LPITEMIDLIST ExplorerView::getInitFolderByProgramOption(xpr_sint_t aIndex, xpr::tstring &aSelFile) const
{
    LPITEMIDLIST sFullPidl = XPR_NULL;

    ProgramOpts &sProgramOpts = SingletonManager::get<ProgramOpts>();

    xpr::tstring sPathArg = sProgramOpts.getDir(aIndex);
    if (sPathArg.empty() == XPR_FALSE)
    {
        if (sPathArg.length() == 2)
            sPathArg += XPR_STRING_LITERAL("\\");

        if (xpr::FileSys::exist(sPathArg.c_str()) == XPR_TRUE)
        {
            xpr::tstring sDirArg;

            if (IsFileSystemFolder(sPathArg.c_str()) == XPR_FALSE)
            {
                xpr_size_t sFind = sPathArg.rfind(XPR_STRING_LITERAL('\\'));
                if (sFind != xpr::tstring::npos)
                {
                    aSelFile = sPathArg.substr(sFind + 1);
                    sDirArg = sPathArg.substr(0, sFind);
                }
            }
            else
            {
                if (sProgramOpts.isSelect() == XPR_TRUE)
                {
                    xpr_size_t sFind = sPathArg.rfind(XPR_STRING_LITERAL('\\'));
                    if (sFind != xpr::tstring::npos)
                    {
                        aSelFile = sPathArg.substr(sFind + 1);
                        sDirArg = sPathArg.substr(0, sFind);
                    }
                }
            }

            if (sDirArg.empty() == XPR_TRUE)
                sDirArg = sPathArg;

            if (sDirArg.length() == 2)
                sDirArg += XPR_STRING_LITERAL('\\');

            if (IsFileSystemFolder(sDirArg.c_str()) == XPR_TRUE)
            {
                sFullPidl = Path2Pidl(sDirArg.c_str());
            }
        }
        else
        {
            sFullPidl = Path2Pidl(sPathArg.c_str());
        }
    }

    return sFullPidl;
}

LPITEMIDLIST ExplorerView::getInitFolder(xpr_sint_t  aIndex,
                                         xpr_uint_t  aFlags,
                                         xpr_uint_t *aInitFolderType) const
{
    xpr_uint_t sInitFolderType;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    // [1] previous split folder
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderSplit;
        if (sInitFolderType != 0)
        {
            LPITEMIDLIST sSplitFullPidl = gFrame->getSplitFolder(aIndex);
            if (XPR_IS_NOT_NULL(sSplitFullPidl))
                sFullPidl = fxfile::base::Pidl::clone(sSplitFullPidl);
        }
    }

    // [2] init folder of configuration option
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderCfgInit;
        if (sInitFolderType != 0)
        {
            if (gOpt->mConfig.mExplorerInitFolderType[mViewIndex] == INIT_TYPE_INIT_FOLDER)
            {
                const xpr_tchar_t *sInitFolder = gOpt->mConfig.mExplorerInitFolder[mViewIndex];
                if (sInitFolder[0] != 0)
                {
                    sFullPidl = Path2Pidl(sInitFolder);
                }
            }
        }
    }

    // [3] default folder of explorer control
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderDefault;
        if (sInitFolderType != 0)
        {
            sFullPidl = ExplorerCtrl::getDefInitFolder();
        }
    }

    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        if (XPR_IS_NOT_NULL(aInitFolderType))
            *aInitFolderType = sInitFolderType;
    }

    return sFullPidl;
}

void ExplorerView::OnInitialUpdate(void)
{
    super::OnInitialUpdate();

}

void ExplorerView::OnDestroy(void)
{
    DESTROY_DELETE(mTabCtrl);
    DESTROY_DELETE(mFolderPane);
    DESTROY_DELETE(mExplorerPane);

    destroyActivateBar();

    super::OnDestroy();

    XPR_SAFE_DELETE(mListCtrlPrint);

    mDropTarget.Revoke();
}

void ExplorerView::setChangedOption(Option &aOption)
{
    xpr_sint_t           i;
    xpr_sint_t           sTabCount;
    TabPane             *sTabPane;
    std::set<TabPane *>  mTabPaneSet;

    // set activate bar options
    visibleActivateBar(gOpt->mConfig.mShowActivateBar, XPR_FALSE);

    if (XPR_IS_NOT_NULL(mActivateBar))
    {
        mActivateBar->setActivateBackColor(aOption.mConfig.mActiveViewColor);
    }

    // set folder pane options
    if (XPR_IS_NOT_NULL(mFolderPane))
    {
        mFolderPane->setChangedOption(aOption);
    }

    // set tab pane options like explorer pane, search result pane, file scrap pane and so on...
    sTabCount = getTabCount();
    for (i = 0; i < sTabCount; ++i)
    {
        sTabPane = getTabPane(i);

        XPR_ASSERT(sTabPane != XPR_NULL);

        if (mTabPaneSet.find(sTabPane) == mTabPaneSet.end())
        {
            sTabPane->setChangedOption(aOption);

            mTabPaneSet.insert(sTabPane);
        }
    }

    // set contents display while dragging
    setDragContents(!aOption.mConfig.mDragNoContents);

    // update layout
    recalcLayout();
}

XPR_INLINE void saveTabOption(Option::Main::Tab &aTab, const ExplorerCtrl &aExplorerCtrl)
{
    xpr_sint_t sViewIndex = aExplorerCtrl.getViewIndex();

    // save tab path
    LPTVITEMDATA sTvItemData = aExplorerCtrl.getFolderData();
    if (XPR_IS_NOT_NULL(sTvItemData))
    {
        xpr_bool_t sSaveFolder = XPR_TRUE;

        if (gOpt->mConfig.mExplorerNoNetLastFolder[sViewIndex] == XPR_TRUE)
        {
            if (IsNetItem(sTvItemData->mFullPidl) == XPR_TRUE)
                sSaveFolder = XPR_FALSE;
        }

        if (XPR_IS_TRUE(sSaveFolder))
        {
            Pidl2Path(sTvItemData->mFullPidl, aTab.mPath);
        }
    }

    // save history
    if (XPR_IS_TRUE(gOpt->mConfig.mSaveHistory))
    {
        const HistoryDeque *sBackwardDeque = aExplorerCtrl.getBackwardDeque();
        const HistoryDeque *sForwardDeque  = aExplorerCtrl.getForwardDeque();
        const HistoryDeque *sHistoryDeque  = aExplorerCtrl.getHistoryDeque();

        LPITEMIDLIST sFullPidl;
        xpr::tstring sHistoryPath;
        HistoryDeque::const_iterator sIterator;

        if (XPR_IS_NOT_NULL(sBackwardDeque))
        {
            FXFILE_STL_FOR_EACH(sIterator, *sBackwardDeque)
            {
                sFullPidl = *sIterator;
                XPR_ASSERT(sFullPidl != XPR_NULL);

                if (Pidl2Path(sFullPidl, sHistoryPath) == XPR_TRUE)
                {
                    if (sHistoryPath.empty() == XPR_FALSE)
                    {
                        aTab.mBackwardList.push_back(sHistoryPath);
                    }
                }
            }
        }

        if (XPR_IS_NOT_NULL(sForwardDeque))
        {
            FXFILE_STL_FOR_EACH(sIterator, *sForwardDeque)
            {
                sFullPidl = *sIterator;
                XPR_ASSERT(sFullPidl != XPR_NULL);

                if (Pidl2Path(sFullPidl, sHistoryPath) == XPR_TRUE)
                {
                    if (sHistoryPath.empty() == XPR_FALSE)
                    {
                        aTab.mForwardList.push_back(sHistoryPath);
                    }
                }
            }
        }

        if (XPR_IS_NOT_NULL(sHistoryDeque))
        {
            FXFILE_STL_FOR_EACH(sIterator, *sHistoryDeque)
            {
                sFullPidl = *sIterator;
                XPR_ASSERT(sFullPidl != XPR_NULL);

                if (Pidl2Path(sFullPidl, sHistoryPath) == XPR_TRUE)
                {
                    if (sHistoryPath.empty() == XPR_FALSE)
                    {
                        aTab.mHistoryList.push_back(sHistoryPath);
                    }
                }
            }
        }
    }
}

void ExplorerView::saveOption(void)
{
    xpr_size_t         i;
    xpr_size_t         sTabCount;
    Option::Main::Tab *sTabOption;
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    ExplorerPane *sExplorerPane = getExplorerPane();

    // save option in explorer pane
    if (XPR_IS_NOT_NULL(sExplorerPane))
    {
        sExplorerPane->saveOption();
    }

    // save last folder or last tab, and history
    gOpt->mMain.mView[mViewIndex].clear();

    gOpt->mMain.mView[mViewIndex].mCurTab = mTabCtrl->getCurTab();

    if (gOpt->mConfig.mExplorerInitFolderType[mViewIndex] == INIT_TYPE_LAST_TAB)
    {
        sTabCount = getTabCount();
        for (i = 0; i < sTabCount; ++i)
        {
            sExplorerCtrl = getExplorerCtrl((xpr_sint_t)i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                sTabOption = new Option::Main::Tab;
                if (XPR_IS_NULL(sTabOption))
                    break;

                gOpt->mMain.mView[mViewIndex].mTabDeque.push_back(sTabOption);

                saveTabOption(*sTabOption, *sExplorerCtrl);
            }
        }
    }
    else if (gOpt->mConfig.mExplorerInitFolderType[mViewIndex] == INIT_TYPE_LAST_FOLDER ||
             XPR_IS_TRUE(gOpt->mConfig.mSaveHistory))
    {
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            sTabOption = new Option::Main::Tab;
            if (XPR_IS_NOT_NULL(sTabOption))
            {
                gOpt->mMain.mView[mViewIndex].mTabDeque.push_back(sTabOption);

                saveTabOption(*sTabOption, *sExplorerCtrl);
            }
        }
    }

    // save view style
    if (gOpt->mConfig.mExplorerSaveViewStyle == XPR_TRUE)
    {
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            gOpt->mMain.mViewStyle[mViewIndex] = sExplorerCtrl->getViewStyle();
        }
    }

    // save visible state of folder pane
    if (XPR_IS_FALSE(gOpt->mMain.mSingleFolderPaneMode))
    {
        gOpt->mMain.mShowEachFolderPane[mViewIndex] = XPR_IS_NOT_NULL(mFolderPane) ? XPR_TRUE : XPR_FALSE;
    }
}

void ExplorerView::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    if (cx > 0 && cy > 0)
        recalcLayout();
}

void ExplorerView::recalcLayout(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    CRect sClientRect;
    GetClientRect(&sClientRect);

    const xpr_sint_t sWidth  = sClientRect.Width();
    const xpr_sint_t sHeight = sClientRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    CRect sRect(sClientRect);
    xpr_bool_t sExplorerTabMode = isExplorerTabMode();

    HDWP sHdwp = ::BeginDeferWindowPos((xpr_sint_t)mTabCtrl->getTabCount() + 3);

    // tab
    if (XPR_IS_NOT_NULL(mTabCtrl) && XPR_IS_NOT_NULL(mTabCtrl->m_hWnd))
    {
        xpr_size_t sTabCount = mTabCtrl->getTabCount();
        if (sTabCount > 1)
        {
            CRect sTabRect(sRect);
            sTabRect.bottom = sTabRect.top + kTabHeight;

            ::DeferWindowPos(sHdwp, *mTabCtrl, XPR_NULL, sTabRect.left, sTabRect.top, sTabRect.Width(), sTabRect.Height(), SWP_NOZORDER);

            sRect.top += kTabHeight;
        }
    }

    // activate bar
    if (XPR_IS_NOT_NULL(mActivateBar) && XPR_IS_NOT_NULL(mActivateBar->m_hWnd))
    {
        CRect sActivateBarRect(sRect);
        sActivateBarRect.bottom = sActivateBarRect.top + 5;

        ::DeferWindowPos(sHdwp, *mActivateBar, XPR_NULL, sActivateBarRect.left, sActivateBarRect.top, sActivateBarRect.Width(), sActivateBarRect.Height(), SWP_NOZORDER);

        sRect.top += sActivateBarRect.Height();
    }

    // splitter
    mSplitter.setWindowRect(sRect);

    xpr_sint_t sPaneSize = gOpt->mMain.mEachFolderPaneSize[mViewIndex];
    if (XPR_IS_FALSE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]))
    {
        xpr_sint_t sSplitSize = mSplitter.getSplitSize();
        sPaneSize = sWidth- sPaneSize - sSplitSize;
    }

    mSplitter.moveColumn(0, sPaneSize);

    mSplitter.resize(sHdwp);

    xpr_sint_t sColumn = XPR_IS_TRUE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]) ? 1 : 0;
    mSplitter.getPaneRect(0, sColumn, sRect);

    // tab based window (such as ExplorerPane, SearchResultPane, FileScrapPane and so on...)
    if (XPR_IS_NOT_NULL(mTabCtrl) && XPR_IS_NOT_NULL(mTabCtrl->m_hWnd))
    {
        xpr_sint_t sCurTab = (xpr_sint_t)mTabCtrl->getCurTab();

        TabData *sTabData = (TabData *)mTabCtrl->getTabData(sCurTab);
        if (XPR_IS_NOT_NULL(sTabData))
        {
            if (XPR_IS_NOT_NULL(sTabData->mTabPane))
            {
                ::DeferWindowPos(sHdwp, *sTabData->mTabPane, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
            }
        }
    }

    ::EndDeferWindowPos(sHdwp);
}

void ExplorerView::OnPaint(void)
{
    CPaintDC sDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    CMemDC sMemDC(&sDC);
    sMemDC.FillSolidRect(&sClientRect, ::GetSysColor(COLOR_WINDOW));

    xpr_sint_t sSplitSize = mSplitter.getSplitSize();

    CSize sPaneSize(0, 0);
    mSplitter.getPaneSize(0, 0, sPaneSize);

    CRect sSplitterRect(sClientRect);
    sSplitterRect.left  = sPaneSize.cx;
    sSplitterRect.right = sSplitterRect.left + sSplitSize;

    sMemDC.FillSolidRect(&sClientRect, ::GetSysColor(COLOR_BTNFACE));
}

xpr_bool_t ExplorerView::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE; // because of thumbnail
    return super::OnEraseBkgnd(aDC);
}

void ExplorerView::OnDraw(CDC *aDC)
{
}

//--------------------------------
// Printer Processing
//--------------------------------
// ID_FILE_PRINT
//       бщ
// super::OnFilePrint
//       бщ
// super::OnPreparePrinting
//       бщ
// super::OnBeginPrinting
//       бщ
// super::OnPrint    <-----+
//       бщ                 | Yes
// more print page?  ------+
//       бщ No
// super::OnEndPrinting
//--------------------------------

xpr_bool_t ExplorerView::OnPreparePrinting(CPrintInfo *aPrintInfo)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_FALSE;

    xpr_tchar_t sDocName[XPR_MAX_PATH + 1] = {0};
    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        _tcscpy(sDocName, sExplorerCtrl->getCurPath());
    else
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sDocName);

    mListCtrlPrint = new ExplorerCtrlPrint;
    if (XPR_IS_NULL(mListCtrlPrint))
        return XPR_FALSE;

    mListCtrlPrint->setListCtrl(sExplorerCtrl);
    mListCtrlPrint->setAppName(AfxGetAppName());
    mListCtrlPrint->setDocTitle(sDocName);
    mListCtrlPrint->onPreparePrinting(aPrintInfo);

    return DoPreparePrinting(aPrintInfo);
}

void ExplorerView::OnBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onBeginPrinting(aDC, aPrintInfo);
}

void ExplorerView::OnPrint(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onPrint(aDC, aPrintInfo);

    super::OnPrint(aDC, aPrintInfo);
}

void ExplorerView::OnEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    if (XPR_IS_NOT_NULL(mListCtrlPrint))
        mListCtrlPrint->onEndPrinting(aDC, aPrintInfo);

    XPR_SAFE_DELETE(mListCtrlPrint);
}

void ExplorerView::print(void)
{
    super::OnFilePrint();
}

void ExplorerView::printPreview(void)
{
    CPrintPreviewState *sPrintPreviewState = new CPrintPreviewState;
    //sPrintPreviewState->nIDMainPane = GetDlgCtrlID();

    //if (DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CPreviewView), sPrintPreviewState) == XPR_FALSE)
    if (DoPrintPreview(IDD_PRINT_PREVIEW, this, RUNTIME_CLASS(PrintPreviewViewEx), sPrintPreviewState) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Error: DoPrintPreview failed.\n"));
        AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
        XPR_SAFE_DELETE(sPrintPreviewState);
    }
}

void ExplorerView::OnEndPrintPreview(CDC *aDC, CPrintInfo *aPrintInfo, POINT point, CPreviewView* pView) 
{
    super::OnEndPrintPreview(aDC, aPrintInfo, point, pView);
}

void ExplorerView::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    // Windows Explorer's background context menu
    if (GetKeyState(VK_CONTROL) < 0)
    {
        LPTVITEMDATA sTvItemData = getExplorerCtrl()->getFolderData();

        LPSHELLFOLDER sShellFolder = XPR_NULL;
        HRESULT sHResult = sTvItemData->mShellFolder->BindToObject(
            (LPCITEMIDLIST)sTvItemData->mPidl,
            0,
            IID_IShellFolder,
            reinterpret_cast<LPVOID *>(&sShellFolder));

        if (SUCCEEDED(sHResult))
        {
            if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
            {
                aPoint.x = sWindowRect.left;
                aPoint.y = sWindowRect.top;
            }

            ContextMenu::trackBackMenu(sShellFolder, &aPoint, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, GetSafeHwnd());
        }

        COM_RELEASE(sShellFolder);
        return;
    }

    // fxfile's own context menu
    BCMenu sMenu;
    BCMenu *sPopupMenu = XPR_NULL;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData) && sTvItemData->mShellAttributes & SFGAO_FILESYSTEM)
        {
            if (sMenu.LoadMenu(IDR_EXP_FILESYSTEM_POPUP) == XPR_TRUE)
            {
                sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
                if (XPR_IS_NOT_NULL(sPopupMenu))
                {
                    BCMenu *sSubMenu = XPR_NULL;
                    if (XPR_IS_TRUE(gOpt->mConfig.mFileScrapContextMenu))
                        sSubMenu = (BCMenu *)sPopupMenu->GetSubMenu(14);
                    else
                    {
                        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_CUR_COPY,        MF_BYCOMMAND);
                        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_CUR_MOVE,        MF_BYCOMMAND);
                        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_DELETE,          MF_BYCOMMAND);
                        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_VIEW,            MF_BYCOMMAND);
                        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_REMOVE_ALL_LIST, MF_BYCOMMAND);
                        sPopupMenu->DeleteMenu(8, MF_BYPOSITION);

                        sSubMenu = (BCMenu *)sPopupMenu->GetSubMenu(8);
                    }
                }
            }
        }
        else
        {
            if (sMenu.LoadMenu(IDR_EXP_NONFILESYSTEM_POPUP) == XPR_TRUE)
                sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
        }
    }

    if (XPR_IS_NOT_NULL(sPopupMenu))
    {
        if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
        {
            aPoint.x = sWindowRect.left;
            aPoint.y = sWindowRect.top;
        }

        sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, gFrame);
    }
}

xpr_bool_t ExplorerView::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo)
{
    if (aHandlerInfo == XPR_NULL)
    {
        xpr_sint_t sMsg = 0;
        xpr_sint_t sCode = aCode;

        if (sCode != CN_UPDATE_COMMAND_UI)
        {
            sMsg  = HIWORD(aCode);
            sCode = LOWORD(aCode);
        }

        if (sMsg == 0)
            sMsg = WM_COMMAND;

        if (sCode == CN_COMMAND)
        {
            ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
                gFrame->executeCommand(aId, sExplorerCtrl);
        }
    }

    return super::OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
}

xpr_bool_t ExplorerView::createTabCtrl(void)
{
    mTabCtrl = new TabCtrl;
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_FALSE;

    mTabCtrl->setObserver(dynamic_cast<TabCtrlObserver *>(this));

    if (mTabCtrl->Create(this, CTRL_ID_TAB_CTRL, CRect(0, 0, 0, 0)) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(mTabCtrl);
        return XPR_FALSE;
    }

    mTabCtrl->ShowWindow(SW_HIDE);

    return XPR_TRUE;
}

xpr_uint_t ExplorerView::generateTabPaneId(void)
{
    xpr_uint_t sPaneId = mNextTabPaneId;
    CWnd *sChildWnd;

    do
    {
        sChildWnd = GetDlgItem(sPaneId);
        if (sChildWnd == XPR_NULL)
        {
            mNextTabPaneId = sPaneId + 1;

            return sPaneId;
        }

        if (sPaneId == kuint16max)
            sPaneId = CTRL_ID_TAB_PANE;
        else
            ++sPaneId;

    } while (true);

    return CTRL_ID_TAB_PANE;
}

xpr_sint_t ExplorerView::newTab(TabType aTabType)
{
    switch (aTabType)
    {
    case TabTypeExplorer:
        {
            return newTab(XPR_NULL);
        }

    case TabTypeSearchResult:
        {
            return newSearchResultTab();
        }

    case TabTypeFileScrap:
        {
            return newFileScrapTab();
        }
    }

    return -1;
}

xpr_sint_t ExplorerView::newTab(const xpr::tstring &aInitFolder)
{
    LPITEMIDLIST sFullPidl = Path2Pidl(aInitFolder);

    xpr_sint_t sTab = newTab(sFullPidl);

    COM_FREE(sFullPidl);

    return sTab;
}

xpr_sint_t ExplorerView::newTab(LPCITEMIDLIST aInitFolder)
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    if (XPR_IS_NULL(mExplorerPane))
    {
        mExplorerPane = new ExplorerPane;
        if (XPR_IS_NULL(mExplorerPane))
            return -1;

        mExplorerPane->setViewIndex(mViewIndex);
        mExplorerPane->setObserver(dynamic_cast<TabPaneObserver *>(this));

        xpr_uint_t sExplorerPaneId = generateTabPaneId();
        if (mExplorerPane->Create(this, sExplorerPaneId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            // nothing to do
        }
        else
        {
            XPR_SAFE_DELETE(mExplorerPane);
        }

        if (XPR_IS_NULL(mExplorerPane))
            return -1;
    }

    TabData *sTabData = new TabData(TabTypeExplorer);
    if (XPR_IS_NULL(sTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;
    xpr_uint_t sExplorerCtrlId = generateTabPaneId();

    ExplorerCtrl *sExplorerCtrl = dynamic_cast<ExplorerCtrl *>(mExplorerPane->newSubPane(sExplorerCtrlId));
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sTabData->mTabPane   = mExplorerPane;
        sTabData->mTabPaneId = sExplorerCtrlId;

        sInsertedTab = mTabCtrl->addTab(XPR_NULL, -1, sTabData);
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sTabData);

        return -1;
    }

    // initailize folder location
    xpr_bool_t sExplored = XPR_FALSE;

    xpr::tstring sSelFile;

    if (XPR_IS_TRUE(mInit))
    {
        LPITEMIDLIST sFullPidl = getInitFolderByProgramOption(mViewIndex, sSelFile);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
            {
                sFullPidl = XPR_NULL;
                sExplored = XPR_TRUE;
            }

            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_FALSE(sExplored))
    {
        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(aInitFolder);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
            {
                sFullPidl = XPR_NULL;
                sExplored = XPR_TRUE;
            }

            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_FALSE(sExplored))
    {
        LPITEMIDLIST sFullPidl;
        xpr_uint_t sFlags, sInitFolderType;

        sFlags = InitFolderAll;
        sInitFolderType = InitFolderNone;

        if (XPR_IS_FALSE(gOpt->mConfig.mViewSplitReopenLastFolder))
            sFlags &= ~InitFolderSplit;

        sFullPidl = getInitFolder(mViewIndex, sFlags, &sInitFolderType);

        if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
        {
            sFullPidl = XPR_NULL;
            sExplored = XPR_TRUE;
        }
        else
        {
            COM_FREE(sFullPidl);

            sFlags = InitFolderDefault;
            if (sInitFolderType != InitFolderCfgInit)
                sFlags |= InitFolderCfgInit;

            sFullPidl = getInitFolder(mViewIndex, sFlags);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
                {
                    sFullPidl = XPR_NULL;
                    sExplored = XPR_TRUE;
                }
            }
        }

        COM_FREE(sFullPidl);
    }

    // select item by command line argument
    if (sSelFile.empty() == XPR_FALSE)
    {
        xpr_sint_t sFind = sExplorerCtrl->findItemFileName(sSelFile.c_str());
        if (sFind >= 0)
        {
            sExplorerCtrl->unselectAll();
            sExplorerCtrl->selectItem(sFind);
        }
    }

    return (xpr_sint_t)sInsertedTab;
}

xpr_sint_t ExplorerView::newSearchResultTab(void)
{
    TabData *sTabData = new TabData(TabTypeSearchResult);
    if (XPR_IS_NULL(sTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;

    SearchResultPane *sTabPane = new SearchResultPane;
    if (XPR_IS_NOT_NULL(sTabPane))
    {
        sTabPane->setViewIndex(mViewIndex);
        sTabPane->setObserver(dynamic_cast<TabPaneObserver *>(this));

        xpr_uint_t sTabPaneId = generateTabPaneId();
        if (sTabPane->Create(this, sTabPaneId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            sTabData->mTabPane   = sTabPane;
            sTabData->mTabPaneId = sTabPaneId;

            const xpr_tchar_t *sTitle = gApp.loadString(XPR_STRING_LITERAL("search_result.title"));

            sInsertedTab = mTabCtrl->addTab(sTitle, -1, sTabData);
        }
        else
        {
            XPR_SAFE_DELETE(sTabPane);
        }
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sTabData);

        return -1;
    }

    return (xpr_sint_t)sInsertedTab;
}

xpr_sint_t ExplorerView::newFileScrapTab(void)
{
    TabData *sTabData = new TabData(TabTypeFileScrap);
    if (XPR_IS_NULL(sTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;

    FileScrapPane *sTabPane = new FileScrapPane;
    if (XPR_IS_NOT_NULL(sTabPane))
    {
        sTabPane->setViewIndex(mViewIndex);
        sTabPane->setObserver(dynamic_cast<TabPaneObserver *>(this));

        xpr_uint_t sTabPaneId = generateTabPaneId();
        if (sTabPane->Create(this, sTabPaneId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            sTabData->mTabPane   = sTabPane;
            sTabData->mTabPaneId = sTabPaneId;

            const xpr_tchar_t *sTitle = gApp.loadString(XPR_STRING_LITERAL("file_scrap.title"));

            sInsertedTab = mTabCtrl->addTab(sTitle, -1, sTabData);
        }
        else
        {
            XPR_SAFE_DELETE(sTabPane);
        }
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sTabData);

        return -1;
    }

    return (xpr_sint_t)sInsertedTab;
}

xpr_sint_t ExplorerView::duplicateTab(xpr_bool_t aOnCursor)
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    xpr_size_t sTab;
    if (XPR_IS_TRUE(aOnCursor))
    {
        CPoint sPoint(mContextMenuCursor);
        mTabCtrl->ScreenToClient(&sPoint);

        sTab = mTabCtrl->hitTest(sPoint);
    }
    else
    {
        sTab = mTabCtrl->getCurTab();
    }

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return -1;

    if (sTabData->canDuplicatePane() == XPR_FALSE)
        return -1;

    if (sTabData->isTabType(TabTypeExplorer) == XPR_TRUE)
    {
        ExplorerCtrl *sExplorerCtrl = dynamic_cast<ExplorerCtrl *>(sTabData->getSubPaneWnd());
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                xpr::tstring sTabPath;
                Pidl2Path(sTvItemData->mFullPidl, sTabPath);

                xpr_sint_t sNewTab = newTab(sTabPath);
                if (sNewTab >= 0)
                {
                    mTabCtrl->setCurTab(sNewTab);

                    return sNewTab;
                }
            }
        }
    }

    return -1;
}

xpr_bool_t ExplorerView::canDuplicateTab(xpr_bool_t aOnCursor) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_FALSE;

    xpr_size_t sTab;
    if (XPR_IS_TRUE(aOnCursor))
    {
        CPoint sPoint(mContextMenuCursor);
        mTabCtrl->ScreenToClient(&sPoint);

        sTab = mTabCtrl->hitTest(sPoint);
    }
    else
    {
        sTab = mTabCtrl->getCurTab();
    }

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_FALSE;

    if (sTabData->canDuplicatePane() == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

void ExplorerView::closeTab(xpr_sint_t aTab, xpr_bool_t aOnCursor)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sTab = aTab;
    if (sTab == -1)
    {
        if (XPR_IS_TRUE(aOnCursor))
        {
            CPoint sPoint(mContextMenuCursor);
            mTabCtrl->ScreenToClient(&sPoint);

            sTab = mTabCtrl->hitTest(sPoint);
        }
        else
        {
            sTab = mTabCtrl->getCurTab();
        }
    }

    xpr_size_t sTabCount = mTabCtrl->getTabCount();
    if (!XPR_IS_RANGE(0, sTab, sTabCount - 1))
        return;

    mTabCtrl->removeTab(sTab);

    recalcLayout();

    RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void ExplorerView::closeAllTabsButThis(xpr_bool_t aOnCursor)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sTab;
    if (XPR_IS_TRUE(aOnCursor))
    {
        CPoint sPoint(mContextMenuCursor);
        mTabCtrl->ScreenToClient(&sPoint);

        sTab = mTabCtrl->hitTest(sPoint);
    }
    else
    {
        sTab = mTabCtrl->getCurTab();
    }

    mTabCtrl->setCurTab(sTab);

    xpr_size_t i;
    xpr_size_t sTabCount = mTabCtrl->getTabCount();

    for (i = sTab + 1; i < sTabCount; ++i)
    {
        mTabCtrl->removeTab(sTab + 1);
    }

    for (i = 0; i < sTab; ++i)
    {
        mTabCtrl->removeTab(0);
    }

    recalcLayout();
}

void ExplorerView::closeAllTabs(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    mTabCtrl->removeAllTabs();

    newTab();

    mTabCtrl->setCurTab(0);

    recalcLayout();
}

void ExplorerView::setCurTab(xpr_sint_t aCurTab)
{
    if (aCurTab < 0)
        return;

    if (XPR_IS_NULL(mTabCtrl))
        return;

    mTabCtrl->setCurTab((xpr_size_t)aCurTab);
}

xpr_bool_t ExplorerView::setCurTabLastActivedExplorerCtrl(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_FALSE;

    xpr_sint_t sTab = findTab(mLastActivedExplorerPaneId);
    if (sTab < 0)
        return XPR_FALSE;

    return mTabCtrl->setCurTab(sTab);
}

void ExplorerView::firstTab(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    mTabCtrl->setCurTab(0);
}

void ExplorerView::previousTab(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sTab = 0;
    xpr_size_t sTabCount = mTabCtrl->getTabCount();

    if (sTabCount > 1)
    {
        sTab = mTabCtrl->getCurTab();
        if (sTab == 0)
            sTab = sTabCount - 1;
        else
            --sTab;
    }

    mTabCtrl->setCurTab(sTab);
}

void ExplorerView::nextTab(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sTab = 0;
    xpr_size_t sTabCount = mTabCtrl->getTabCount();

    if (sTabCount > 1)
    {
        sTab = mTabCtrl->getCurTab();
        if (sTab == (sTabCount - 1))
            sTab = 0;
        else
            ++sTab;
    }

    mTabCtrl->setCurTab(sTab);
}

void ExplorerView::lastTab(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sTab = 0;
    xpr_size_t sTabCount = mTabCtrl->getTabCount();

    if (sTabCount > 1)
        sTab = sTabCount - 1;

    mTabCtrl->setCurTab(sTab);
}

xpr_sint_t ExplorerView::findTab(xpr_uint_t aTabPaneId) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    xpr_size_t i, sTabCount;
    TabData *sTabData;

    sTabCount = mTabCtrl->getTabCount();
    for (i = 0; i < sTabCount; ++i)
    {
        sTabData = (TabData *)mTabCtrl->getTabData(i);
        if (XPR_IS_NOT_NULL(sTabData))
        {
            if (sTabData->mTabPaneId == aTabPaneId)
            {
                return (xpr_sint_t)i;
            }
        }
    }

    return -1;
}

xpr_sint_t ExplorerView::findTabFirstExplorerCtrl(void) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    xpr_size_t i, sTabCount;
    TabData *sTabData;

    sTabCount = mTabCtrl->getTabCount();
    for (i = 0; i < sTabCount; ++i)
    {
        sTabData = (TabData *)mTabCtrl->getTabData(i);
        if (XPR_IS_NOT_NULL(sTabData))
        {
            if (sTabData->isTabType(TabTypeExplorer) == XPR_TRUE)
            {
                return (xpr_sint_t)i;
            }
        }
    }

    return -1;
}

xpr_sint_t ExplorerView::getTabCount(void) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return 0;

    return (xpr_sint_t)mTabCtrl->getTabCount();
}

xpr_sint_t ExplorerView::getTabOnCursor(void) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    CPoint sPoint(mContextMenuCursor);
    mTabCtrl->ScreenToClient(&sPoint);

    xpr_size_t sTab = mTabCtrl->hitTest(sPoint);
    if (sTab == TabCtrl::InvalidTab)
        return -1;

    return (xpr_sint_t)sTab;
}

TabPane *ExplorerView::getTabPane(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab < 0)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_NULL;

    return sTabData->mTabPane;
}

xpr_uint_t ExplorerView::getTabPaneId(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return 0;

    xpr_sint_t sTab = aTab;
    if (sTab < 0)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return 0;

    if (XPR_IS_NULL(sTabData->mTabPane))
        return 0;

    return sTabData->mTabPaneId;
}

xpr_bool_t ExplorerView::isExplorerTabMode(void) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NOT_NULL(sTabData) && sTabData->isTabType(TabTypeExplorer) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t ExplorerView::createSplitter(void)
{
    mSplitter.setObserver(dynamic_cast<SplitterObserver *>(this));
    mSplitter.setMaxSplitCount(1, 2);
    mSplitter.setPaneSizeLimit(50, ksintmax);
    mSplitter.setSplitSize(2);

    return XPR_TRUE;
}

FolderPane *ExplorerView::getFolderPane(void) const
{
    return mFolderPane;
}

FolderCtrl *ExplorerView::getFolderCtrl(void) const
{
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        if (XPR_IS_TRUE(gOpt->mConfig.mFolderTreeLinkageOnSingle))
            return gFrame->getFolderCtrl();

        return XPR_NULL;
    }

    FolderPane *sFolderPane = getFolderPane();
    if (XPR_IS_NULL(sFolderPane))
        return XPR_NULL;

    return sFolderPane->getFolderCtrl();
}

AddressBar *ExplorerView::getAddressBar(void) const
{
    ExplorerPane *sExplorerPane = getExplorerPane();
    if (XPR_IS_NULL(sExplorerPane))
        return XPR_NULL;

    return sExplorerPane->getAddressBar();
}

ExplorerPane *ExplorerView::getExplorerPane(void) const
{
    return mExplorerPane;
}

ExplorerCtrl *ExplorerView::getExplorerCtrl(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl) || XPR_IS_NULL(mExplorerPane))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_NULL;

    return dynamic_cast<ExplorerCtrl *>(sTabData->getSubPaneWnd());
}

SearchResultCtrl *ExplorerView::getSearchResultCtrl(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_NULL;

    return dynamic_cast<SearchResultCtrl *>(sTabData->getSubPaneWnd());
}

FileScrapPane *ExplorerView::getFileScrapPane(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_NULL;

    return dynamic_cast<FileScrapPane *>(sTabData->mTabPane);
}

xpr_bool_t ExplorerView::visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading, xpr_bool_t aSaveFolderPaneVisible)
{
    if (XPR_IS_TRUE(aSaveFolderPaneVisible))
        gOpt->mMain.mShowEachFolderPane[mViewIndex] = aVisible;

    if (XPR_IS_TRUE(aVisible))
    {
        if (createFolderPane() == XPR_FALSE)
            return XPR_FALSE;

        if (XPR_IS_FALSE(aLoading))
        {
            FolderCtrl *sFolderCtrl = getFolderCtrl();
            if (XPR_IS_NOT_NULL(sFolderCtrl))
            {
                ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                    if (XPR_IS_NOT_NULL(sTvItemData))
                    {
                        HTREEITEM sTreeItem = sFolderCtrl->searchSel(sTvItemData->mFullPidl, XPR_FALSE);
                        if (XPR_IS_NOT_NULL(sTreeItem))
                        {
                            if (gOpt->mConfig.mFolderTreeInitNoExpand == XPR_FALSE)
                                sFolderCtrl->Expand(sTreeItem, TVE_EXPAND);

                            sFolderCtrl->EnsureVisible(sTreeItem);
                        }
                    }
                }
            }
        }
    }
    else
    {
        destroyFolderPane();
    }

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerView::isVisibleFolderPane(void) const
{
    if (XPR_IS_NULL(mFolderPane))
        return XPR_FALSE;

    return mFolderPane->IsWindowVisible();
}

xpr_bool_t ExplorerView::createFolderPane(void)
{
    if (XPR_IS_NOT_NULL(mFolderPane))
        return XPR_TRUE;

    mFolderPane = new FolderPane;
    if (XPR_IS_NOT_NULL(mFolderPane))
    {
        mFolderPane->setObserver(dynamic_cast<FolderPaneObserver *>(this));
        mFolderPane->setViewIndex(mViewIndex);
        if (mFolderPane->Create(this, CTRL_ID_FOLDER_PANE, CRect(0, 0, 0, 0)) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(mFolderPane);
            return XPR_FALSE;
        }

        CRect sWindowRect;
        mSplitter.getWindowRect(sWindowRect);

        xpr_sint_t sPaneSize = gOpt->mMain.mEachFolderPaneSize[mViewIndex];
        if (XPR_IS_FALSE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]))
        {
            xpr_sint_t sSplitSize = mSplitter.getSplitSize();
            sPaneSize = sWindowRect.Width() - sPaneSize - sSplitSize;
        }

        if (XPR_IS_TRUE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]))
            mSplitter.switchPane(0, 0, 0, 1);

        mSplitter.split(1, 2);
        mSplitter.moveColumn(0, sPaneSize);
        mSplitter.resize();
    }

    return XPR_TRUE;
}

void ExplorerView::destroyFolderPane(void)
{
    if (XPR_IS_NULL(mFolderPane))
        return;

    if (XPR_IS_TRUE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]))
        mSplitter.switchPane(0, 1, 0, 0);

    mSplitter.split(1, 1);
    mSplitter.resize();

    mFolderPane->DestroyWindow();
    XPR_SAFE_DELETE(mFolderPane);
}

xpr_bool_t ExplorerView::isLeftFolderPane(void) const
{
    return gOpt->mMain.mLeftEachFolderPane[mViewIndex];
}

void ExplorerView::setLeftFolderPane(xpr_bool_t aLeft)
{
    if (isVisibleFolderPane() == XPR_FALSE)
        return;

    if (gOpt->mMain.mLeftEachFolderPane[mViewIndex] != aLeft)
    {
        gOpt->mMain.mLeftEachFolderPane[mViewIndex] = aLeft;

        mSplitter.switchPane(0, 0, 0, 1);

        recalcLayout();
    }
}

ActivateBar *ExplorerView::getActivateBar(void) const
{
    return mActivateBar;
}

xpr_bool_t ExplorerView::createActivateBar(void)
{
    if (XPR_IS_NOT_NULL(mActivateBar))
        return XPR_TRUE;

    mActivateBar = new ActivateBar;
    if (XPR_IS_NOT_NULL(mActivateBar))
    {
        if (mActivateBar->Create(this, CTRL_ID_ACTIVATE_BAR, CRect(0,0,0,0)) == XPR_TRUE)
        {
            mActivateBar->setActivateBackColor(gOpt->mConfig.mActiveViewColor);
        }
        else
        {
            XPR_SAFE_DELETE(mActivateBar);
        }
    }

    return XPR_IS_NOT_NULL(mActivateBar) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerView::destroyActivateBar(void)
{
    if (XPR_IS_NULL(mActivateBar))
        return;

    DESTROY_DELETE(mActivateBar);
}

void ExplorerView::visibleActivateBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createActivateBar();

        xpr_bool_t sActivate      = (gFrame->getActiveView()      == mViewIndex) ? XPR_TRUE : XPR_FALSE;
        xpr_bool_t sLastActivated = (gFrame->getLastActivedView() == mViewIndex) ? XPR_TRUE : XPR_FALSE;

        mActivateBar->activate(sActivate, sLastActivated);
    }
    else
    {
        destroyActivateBar();
    }

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerView::isVisibleActivateBar(void) const
{
    return gOpt->mConfig.mShowActivateBar;
}

// TODO
StatusBar *ExplorerView::getStatusBar(void) const
{
    TabPane *sTabPane = getTabPane();
    if (XPR_IS_NULL(sTabPane))
        return XPR_NULL;

    return sTabPane->getStatusBar();
}

// TODO
const xpr_tchar_t *ExplorerView::getStatusPaneText(xpr_sint_t aIndex) const
{
    TabPane *sTabPane = getTabPane();
    if (XPR_IS_NULL(sTabPane))
        return XPR_NULL;

    return sTabPane->getStatusPaneText(aIndex);
}

void ExplorerView::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);

    if (XPR_IS_NOT_NULL(mExplorerPane))
        mExplorerPane->setDragContents(aDragContents);
}

void ExplorerView::OnUpdate(CView *aSender, LPARAM aHint, CObject *aHintObject)
{
}

void ExplorerView::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    xpr_sint_t sCurTab = (xpr_sint_t)mTabCtrl->getCurTab();
    if (sCurTab >= 0)
    {
        CWnd *sTabPane = getTabPane(sCurTab);
        if (XPR_IS_NOT_NULL(sTabPane))
        {
            sTabPane->SetFocus();
        }
    }
}

void ExplorerView::setActivateBarStatus(xpr_bool_t aActivate, xpr_bool_t aLastActivated)
{
    if (XPR_IS_NOT_NULL(mActivateBar) && IsWindow(mActivateBar->m_hWnd))
    {
        mActivateBar->activate(aActivate, aLastActivated);
    }
}

void ExplorerView::OnActivateView(xpr_bool_t aActivate, CView *aActivateView, CView *aDeactiveView)
{
    if (XPR_IS_TRUE(aActivate) && aDeactiveView != this && aActivateView == this)
    {
        if (GetParentFrame()->GetActiveWindow() != this)
        {
            ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sExplorerCtrl) && ::IsWindow(sExplorerCtrl->GetSafeHwnd()) == XPR_TRUE)
            {
                LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                if (XPR_IS_NOT_NULL(sTvItemData))
                {
                    // update single folder pane linkage
                    if (XPR_IS_TRUE(gOpt->mConfig.mFolderTreeLinkageOnSingle))
                    {
                        FolderCtrl *sFolderCtrl = getFolderCtrl();
                        if (XPR_IS_NOT_NULL(sFolderCtrl) && XPR_IS_NOT_NULL(sFolderCtrl->m_hWnd))
                        {
                            sFolderCtrl->searchSel(sTvItemData->mFullPidl, XPR_FALSE);
                        }
                    }

                    // update title bar
                    gFrame->setMainTitle(sTvItemData->mFullPidl);
                }
            }
        }
    }

    super::OnActivateView(aActivate, aActivateView, aDeactiveView);
}

void ExplorerView::onTabInserted(TabCtrl &aTabCtrl, xpr_size_t aTab)
{
    xpr_size_t sTabCount = aTabCtrl.getTabCount();
    if (sTabCount > 1)
        aTabCtrl.ShowWindow(SW_SHOW);

    recalcLayout();
}

xpr_bool_t ExplorerView::onTabChangingCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab)
{
    return XPR_TRUE;
}

void ExplorerView::onTabChangedCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab)
{
    TabData *sOldTabData = (TabData *)aTabCtrl.getTabData(aOldTab);
    if (XPR_IS_NOT_NULL(sOldTabData))
    {
        if (XPR_IS_NOT_NULL(sOldTabData->mTabPane))
        {
            sOldTabData->mTabPane->ShowWindow(SW_HIDE);
        }
    }

    TabData *sNewTabData = (TabData *)aTabCtrl.getTabData(aNewTab);
    if (XPR_IS_NOT_NULL(sNewTabData))
    {
        if (XPR_IS_NOT_NULL(sNewTabData->mTabPane))
        {
            if (sNewTabData->mTabPane->isSharedPane() == XPR_TRUE)
            {
                sNewTabData->mTabPane->setCurSubPane(sNewTabData->mTabPaneId);
            }
        }

        if (sNewTabData->isTabType(TabTypeExplorer) == XPR_TRUE)
        {
            ExplorerCtrl *sNewExplorerCtrl = dynamic_cast<ExplorerCtrl *>(sNewTabData->getSubPaneWnd());
            if (XPR_IS_NOT_NULL(sNewExplorerCtrl))
            {
                LPTVITEMDATA sTvItemData = sNewExplorerCtrl->getFolderData();
                if (XPR_IS_NOT_NULL(sTvItemData))
                {
                    // update folder control
                    FolderCtrl *sFolderCtrl = getFolderCtrl();
                    if (XPR_IS_NOT_NULL(sFolderCtrl) && XPR_IS_NOT_NULL(sFolderCtrl->m_hWnd))
                    {
                        sFolderCtrl->searchSel(sTvItemData->mFullPidl, XPR_FALSE);
                    }

                    // update tab text
                    xpr::tstring sName;
                    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);

                    aTabCtrl.setTabText(aNewTab, sName.c_str());

                    if (gFrame->getActiveView() == mViewIndex)
                    {
                        // update main title
                        gFrame->setMainTitle(sTvItemData->mShellFolder, sTvItemData->mPidl);
                    }

                    mLastActivedExplorerPaneId = sNewTabData->mTabPaneId;
                }
            }
        }

        if (XPR_IS_NOT_NULL(sNewTabData->mTabPane))
        {
            sNewTabData->mTabPane->ShowWindow(SW_SHOW);

            if (XPR_IS_FALSE(mInit))
                sNewTabData->mTabPane->SetFocus();
        }
    }

    recalcLayout();
}

void ExplorerView::onTabRemove(TabCtrl &aTabCtrl, xpr_size_t aTab)
{
    TabData *sTabData = (TabData *)aTabCtrl.getTabData(aTab);
    XPR_SAFE_DELETE(sTabData);
}

void ExplorerView::onTabRemoved(TabCtrl &aTabCtrl)
{
    xpr_size_t sTabCount = aTabCtrl.getTabCount();
    if (sTabCount <= 1)
        aTabCtrl.ShowWindow(SW_HIDE);
}

void ExplorerView::onTabRemoveAll(TabCtrl &aTabCtrl)
{
    aTabCtrl.ShowWindow(SW_HIDE);
}

void ExplorerView::onTabEmptied(TabCtrl &aTabCtrl)
{
}

void ExplorerView::onTabDoubleClicked(TabCtrl &aTabCtrl, xpr_size_t aTab)
{
    closeTab((xpr_sint_t)aTab);
}

void ExplorerView::onTabContextMenu(TabCtrl &aTabCtrl, xpr_size_t aTab, const POINT &aPoint)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_TAB) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            mContextMenuCursor = aPoint;

            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
        }
    }
}

void ExplorerView::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint)
{

    super::OnLButtonDblClk(aFlags, aPoint);
}

void ExplorerView::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.beginTracking(m_hWnd, aPoint) == XPR_FALSE)
    {
        mSplitter.beginTracking(m_hWnd, aPoint);
    }

    super::OnLButtonDown(aFlags, aPoint);
}

void ExplorerView::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.moveTracking(aPoint);

        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mSplitter.getPaneSize(0, sColumn, sPaneSize);

        gOpt->mMain.mEachFolderPaneSize[mViewIndex] = sPaneSize.cx;

        recalcLayout();
    }
    else
    {
        mSplitter.moveTracking(aPoint);
    }

    super::OnMouseMove(aFlags, aPoint);
}

void ExplorerView::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.endTracking(aPoint);

        // update new split size
        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mSplitter.getPaneSize(sRow, sColumn, sPaneSize);

        gOpt->mMain.mEachFolderPaneSize[mViewIndex] = sPaneSize.cx;

        recalcLayout();
    }

    super::OnLButtonUp(aFlags, aPoint);
}

void ExplorerView::OnCaptureChanged(CWnd *aWnd)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.cancelTracking();
    }

    super::OnCaptureChanged(aWnd);
}

//
// from SplitterObserver
//
CWnd *ExplorerView::onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn)
{
    xpr_sint_t sPaneCount = aSplitter.getPaneCount();

    if (XPR_IS_TRUE(gOpt->mMain.mShowEachFolderPane[mViewIndex]) && XPR_IS_FALSE(gOpt->mMain.mLeftEachFolderPane[mViewIndex]))
    {
        if ((sPaneCount == 1) || (aRow == 0 && aColumn == 0))
            return XPR_NULL;
        else if (aRow == 0 && aColumn == 1)
            return mFolderPane;
    }
    else
    {
        if ((sPaneCount == 1) || (aRow == 0 && aColumn == 1))
            return XPR_NULL;
        else if (aRow == 0 && aColumn == 0)
            return mFolderPane;
    }

    return XPR_NULL;
}

void ExplorerView::onSplitterPaneDestroy(Splitter &aSplitter, CWnd *aPaneWnd)
{
}

void ExplorerView::onSplitterActivedPane(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn)
{
}

//
// from FolderPaneObserver
//
xpr_bool_t ExplorerView::onExplore(FolderPane &aFolderPane, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (sExplorerCtrl == XPR_NULL)
        return XPR_TRUE;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(aFullPidl);
    if (sFullPidl == XPR_NULL)
        return XPR_TRUE;

    if (sExplorerCtrl->explore(sFullPidl, XPR_FALSE) == XPR_FALSE)
    {
        COM_FREE(sFullPidl);
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

void ExplorerView::onUpdateStatus(FolderPane &aFolderPane)
{
    ExplorerPane *sExplorerPane = getExplorerPane();
    if (XPR_IS_NULL(sExplorerPane))
        return;

    xpr_uint_t sTabPaneId = getTabPaneId(-1);

    sExplorerPane->updateStatusBar(sTabPaneId);
}

void ExplorerView::onRename(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    gFrame->executeCommand(ID_FILE_RENAME, XPR_NULL, &sParameters);
}

void ExplorerView::onContextMenuDelete(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    gFrame->executeCommand(ID_FILE_DELETE, XPR_NULL, &sParameters);
}

void ExplorerView::onContextMenuRename(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    gFrame->executeCommand(ID_FILE_RENAME, XPR_NULL, &sParameters);
}

void ExplorerView::onContextMenuPaste(FolderPane &aFolderPane, HTREEITEM aTreeItem)
{
    FolderCtrl *sFolderCtrl = getFolderCtrl();
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeTargetItem, aTreeItem);

    gFrame->executeCommand(ID_EDIT_PASTE, sFolderCtrl, &sParameters);
}

xpr_sint_t ExplorerView::onDrop(FolderPane &aFolderPane, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
{
    xpr_sint_t sResult = 0;

    ClipFormat &sClipFormat = ClipFormat::instance();
    LPDATAOBJECT sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    if (IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }
    else if (IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }
    else if (IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }
    else if (IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (IsPasteBitmap(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    if (IsPasteDIB(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (IsPasteUnicodeText(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewTextFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (IsPasteText(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewTextFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    return sResult;
}

void ExplorerView::onSetFocus(FolderPane &aFolderPane)
{
    gFrame->setActiveView(mViewIndex);
}

void ExplorerView::onMoveFocus(FolderPane &aFolderPane, xpr_sint_t aCurWnd)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onMoveFocus(*this, aCurWnd);
}

//
// from TabPaneObserver
//
void ExplorerView::onCreated(TabPane &aTabPane)
{
    if (aTabPane.isTabType(TabTypeFileScrap) == XPR_TRUE)
    {
        FileScrapPane *sFileScrapPane = dynamic_cast<FileScrapPane *>(&aTabPane);

        gFrame->setFileScrapPane(sFileScrapPane);
    }
}

void ExplorerView::onDestroyed(TabPane &aTabPane)
{
    if (aTabPane.isTabType(TabTypeFileScrap) == XPR_TRUE)
    {
        gFrame->setFileScrapPane(XPR_NULL);
    }
}

void ExplorerView::onExplored(TabPane &aTabPane, xpr_uint_t aId, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    if (XPR_IS_NULL(aFullPidl))
        return;

    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sCurTab = mTabCtrl->getCurTab();
    xpr_sint_t sTab = findTab(aId);

    if (sTab != -1 && sTab == sCurTab)
    {
        // update folder control
        FolderCtrl *sFolderCtrl = getFolderCtrl();
        if (aUpdateBuddy == XPR_TRUE && XPR_IS_NOT_NULL(sFolderCtrl))
        {
            sFolderCtrl->searchSel(aFullPidl);
        }
    }

    if (sTab >= 0)
    {
        xpr::tstring sName;
        GetName(aFullPidl, SHGDN_INFOLDER, sName);

        mTabCtrl->setTabText(sTab, sName.c_str());
    }

    if (gFrame->getActiveView() == mViewIndex)
    {
        // update main title
        gFrame->setMainTitle(aFullPidl);
    }
}

void ExplorerView::onSetFocus(TabPane &aTabPane)
{
    gFrame->setActiveView(mViewIndex);
}

void ExplorerView::onMoveFocus(TabPane &aTabPane, xpr_sint_t aCurWnd)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onMoveFocus(*this, aCurWnd);
}

xpr_bool_t ExplorerView::onOpenFolder(TabPane &aTabPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    return openFolder(aDir, aSelPath);
}

xpr_bool_t ExplorerView::onOpenFolder(TabPane &aTabPane, LPCITEMIDLIST aFullPidl)
{
    return openFolder(aFullPidl);
}

xpr_bool_t ExplorerView::openFolder(const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    if (XPR_IS_NULL(aDir))
        return XPR_FALSE;

    xpr_bool_t sNewTab = XPR_FALSE;

    if (setCurTabLastActivedExplorerCtrl() == XPR_FALSE)
    {
        xpr_sint_t sTab = findTabFirstExplorerCtrl();
        if (sTab < 0)
        {
            sTab = newTab(aDir);

            sNewTab = XPR_TRUE;
        }

        setCurTab(sTab);
    }

    if (XPR_IS_TRUE(sNewTab) && XPR_IS_NULL(aSelPath))
        return XPR_TRUE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (XPR_IS_TRUE(sNewTab))
    {
        sResult = XPR_TRUE;
    }
    else
    {
        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aDir);

        sResult = sExplorerCtrl->explore(sFullPidl);
        if (XPR_IS_FALSE(sResult))
        {
            COM_FREE(sFullPidl);
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        if (XPR_IS_NOT_NULL(aSelPath))
        {
            xpr_sint_t sIndex = sExplorerCtrl->findItemPath(aSelPath);
            if (sIndex >= 0)
            {
                sExplorerCtrl->unselectAll();
                sExplorerCtrl->selectItem(sIndex);
            }
        }
    }

    return sResult;
}

xpr_bool_t ExplorerView::openFolder(LPCITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    xpr_bool_t sNewTab = XPR_FALSE;

    if (setCurTabLastActivedExplorerCtrl() == XPR_FALSE)
    {
        xpr_sint_t sTab = findTabFirstExplorerCtrl();
        if (sTab < 0)
        {
            sTab = newTab(aFullPidl);

            sNewTab = XPR_TRUE;
        }

        setCurTab(sTab);
    }

    if (XPR_IS_TRUE(sNewTab))
        return XPR_TRUE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_FALSE;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(aFullPidl);

    xpr_bool_t sResult = sExplorerCtrl->explore(sFullPidl);
    if (XPR_IS_FALSE(sResult))
    {
        COM_FREE(sFullPidl);
    }

    return sResult;
}
} // namespace fxfile
