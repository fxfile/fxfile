//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ExplorerView.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_bookmark.h"
#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_cmd_line_parser.h"
#include "fxb/fxb_file_ass.h"
#include "fxb/fxb_clip_format.h"
#include "fxb/fxb_size_format.h"

#include "rgc/DropTarget.h"
#include "rgc/MemDC.h"
#include "rgc/TabCtrl.h"

#include "ExplorerViewObserver.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"
#include "ExplorerCtrlPrint.h"
#include "FolderPane.h"
#include "FolderCtrl.h"
#include "PreviewViewEx.h"
#include "PathBar.h"
#include "ActivateBar.h"
#include "DrivePathBar.h"
#include "AddressBar.h"
#include "ContentsWnd.h"
#include "OptionMgr.h"
#include "CtrlId.h"
#include "SearchResultPane.h"
#include "FileScrapPane.h"
#include "StatusBarEx.h"

#include "cmd/cmd_parameters.h"
#include "cmd/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#include "cmd/ParamExecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_sint_t kTabHeight = 27;

#define MAX_CONTENS_SHOW_SEL_ITEMS       17
#define CONTENTS_EXPLORER_STYLE_WIDTH   200
#define CONTENTS_BASIC_STYLE_HEIGHT      36

//
// control id
//
enum
{
    CTRL_ID_TAB_CTRL = 50,
    CTRL_ID_ADDRESS_BAR,
    CTRL_ID_FOLDER_PANE,
    CTRL_ID_PATH_BAR,
    CTRL_ID_ACTIVATE_BAR,
    CTRL_ID_CONTENTS_WND,
    CTRL_ID_STATUS_BAR,
    CTRL_ID_EXPLORER_CTRL = 200,
};

class ExplorerView::TabData
{
    friend class ExplorerView;

public:
    TabData(TabType aTabType)
        : mTabWnd(XPR_NULL)
        , mTabType(aTabType)
    {
    }

    virtual ~TabData(void)
    {
        DESTROY_DELETE(mTabWnd);
    }

protected:
    TabType  mTabType;
    CWnd    *mTabWnd;
};

class ExplorerView::ExplorerTabData : public ExplorerView::TabData
{
    friend class ExplorerView;

public:
    ExplorerTabData(void)
        : TabData(TabTypeExplorer)
    {
        mStatusPane0[0] = 0;
        mStatusPane1[0] = 0;
    }

    virtual ~ExplorerTabData(void)
    {
    }

public:
    ExplorerCtrl *getExplorerCtrl(void) const
    {
        return dynamic_cast<ExplorerCtrl *>(mTabWnd);
    }

protected:
    xpr_tchar_t mStatusPane0[XPR_MAX_PATH + 1];
    xpr_tchar_t mStatusPane1[XPR_MAX_PATH + 1];
};

class ExplorerView::SearchResultTabData : public ExplorerView::TabData
{
    friend class ExplorerView;

public:
    SearchResultTabData(void)
        : TabData(TabTypeSearchResult)
    {
    }

    virtual ~SearchResultTabData(void)
    {
    }

public:
    SearchResultPane *getSearchResultPane(void) const
    {
        return dynamic_cast<SearchResultPane *>(mTabWnd);
    }
};

class ExplorerView::FileScrapTabData : public ExplorerView::TabData
{
    friend class ExplorerView;

public:
    FileScrapTabData(void)
        : TabData(TabTypeFileScrap)
    {
    }

    virtual ~FileScrapTabData(void)
    {
    }

public:
    FileScrapPane *getFileScrapPane(void) const
    {
        return dynamic_cast<FileScrapPane *>(mTabWnd);
    }
};

IMPLEMENT_DYNCREATE(ExplorerView, CView)

ExplorerView::ExplorerView(void)
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mInit(XPR_TRUE)
    , mTabCtrl(XPR_NULL)
    , mFolderPane(XPR_NULL)
    , mAddressBar(XPR_NULL), mPathBar(XPR_NULL), mActivateBar(XPR_NULL), mDrivePathBar(XPR_NULL), mContentsWnd(XPR_NULL), mStatusBar(XPR_NULL)
    , mIsDrivePathBar(XPR_FALSE)
    , mMarginRect(CRect(CONTENTS_EXPLORER_STYLE_WIDTH, 0, 0, 0))
    , mListCtrlPrint(XPR_NULL)
    , mNextExplorerCtrlId(CTRL_ID_EXPLORER_CTRL), mLastActivedExplorerCtrlId(0)
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

    // load tab from option
    TabPathDeque::iterator sIterator = gOpt->mViewSplitTab[mViewIndex].mTabPathDeque.begin();
    for (; sIterator != gOpt->mViewSplitTab[mViewIndex].mTabPathDeque.end(); ++sIterator)
    {
        const std::tstring &sFullPath = *sIterator;

        newTab(sFullPath);
    }

    if (mTabCtrl->getTabCount() == 0)
    {
        newTab();
    }

    if (mTabCtrl->setCurTab(gOpt->mViewSplitTab[mViewIndex].mCurTab) == XPR_FALSE)
        mTabCtrl->setCurTab(0);

    // etc control
    visiblePathBar(gOpt->mPathBar[mViewIndex], XPR_TRUE);
    visibleActivateBar(gOpt->mActivateBar[mViewIndex], XPR_TRUE);
    visibleDrivePathBar(gFrame->isDriveBar() && gFrame->isDriveViewSplit(), XPR_TRUE);
    visibleAddressBar(gOpt->mAddressBar[mViewIndex], XPR_TRUE);
    visibleStatusBar(gOpt->mStatusBar[mViewIndex], XPR_TRUE);

    // folder pane
    xpr_bool_t sVisibleFolderPane = XPR_FALSE;
    if (XPR_IS_FALSE(gOpt->mSingleFolderPaneMode) && XPR_IS_TRUE(gOpt->mShowEachFolderPane[mViewIndex]))
        sVisibleFolderPane = visibleFolderPane(XPR_TRUE, XPR_TRUE);

    FolderCtrl *sFolderCtrl = getFolderCtrl();
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode) && mViewIndex == 0)
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
        mSplitter.moveColumn(0, gOpt->mEachFolderPaneSize[mViewIndex]);
        mSplitter.resize();
    }

    // register for drag and drop
    mDropTarget.Register(this);

    // apply configuration
    OptionMgr &sOptionMgr = OptionMgr::instance();

    sOptionMgr.setOnlyExplorerViewApply(XPR_TRUE);
    sOptionMgr.applyExplorerView(this, XPR_TRUE);
    sOptionMgr.setOnlyExplorerViewApply(XPR_FALSE);

    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        if (XPR_IS_NOT_NULL(mContentsWnd) && XPR_IS_NOT_NULL(mContentsWnd->m_hWnd))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {

                setContentsFolder(sTvItemData);
                setContentsNormal(sTvItemData, XPR_TRUE);
            }
        }
    }

    mInit = XPR_FALSE;

    return 0;
}

LPITEMIDLIST ExplorerView::getInitFolder(xpr_sint_t    aIndex,
                                         std::tstring &aSelFile,
                                         xpr_uint_t    aFlags,
                                         xpr_uint_t   *aInitFolderType) const
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
                sFullPidl = fxb::CopyItemIDList(sSplitFullPidl);
        }
    }

    // [2] command parameter interpretation
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderCmdParam;
        if (sInitFolderType != 0)
        {
            xpr_tchar_t sCmdOption[0xff] = {0};
            _stprintf(sCmdOption, XPR_STRING_LITERAL("%d"), aIndex + 1);

            std::tstring sPathArg;
            if (fxb::CmdLineParser::instance().getArg(sCmdOption, sPathArg) == XPR_TRUE)
            {
                if (sPathArg.length() == 2)
                    sPathArg += XPR_STRING_LITERAL("\\");

                if (fxb::IsExistFile(sPathArg) == XPR_TRUE)
                {
                    std::tstring sDirArg;

                    if (fxb::IsFileSystemFolder(sPathArg) == XPR_FALSE)
                    {
                        xpr_size_t sFind = sPathArg.rfind(XPR_STRING_LITERAL('\\'));
                        if (sFind != std::tstring::npos)
                        {
                            aSelFile = sPathArg.substr(sFind + 1);
                            sDirArg = sPathArg.substr(0, sFind);
                        }
                    }
                    else
                    {
                        if (fxb::CmdLineParser::instance().isExistArg(XPR_STRING_LITERAL("U")) == XPR_TRUE)
                        {
                            xpr_size_t sFind = sPathArg.rfind(XPR_STRING_LITERAL('\\'));
                            if (sFind != std::tstring::npos)
                            {
                                aSelFile = sPathArg.substr(sFind + 1);
                                sDirArg = sPathArg.substr(0, sFind);
                            }
                        }
                    }

                    if (sDirArg.empty() == true)
                        sDirArg = sPathArg;

                    if (sDirArg.length() == 2)
                        sDirArg += XPR_STRING_LITERAL('\\');

                    if (fxb::IsFileSystemFolder(sDirArg) == XPR_TRUE)
                    {
                        sFullPidl = fxb::Path2Pidl(sDirArg.c_str());
                    }
                }
                else
                {
                    sFullPidl = fxb::Path2Pidl(sPathArg.c_str());
                }
            }
        }
    }

    // [3] init folder
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderCfgInit;
        if (sInitFolderType != 0)
        {
            if (gOpt->mExplorerInitFolderType[mViewIndex] == INIT_TYPE_INIT_FOLDER)
            {
                const xpr_tchar_t *sInitFolder = gOpt->mExplorerInitFolder[mViewIndex];
                if (fxb::IsEmptyString(sInitFolder) == XPR_FALSE)
                {
                    sFullPidl = fxb::Path2Pidl(sInitFolder);
                }
            }
        }
    }

    // [4] last save folder
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderLastSaved;
        if (sInitFolderType != 0)
        {
            if (gOpt->mExplorerInitFolderType[mViewIndex] == INIT_TYPE_LAST_FOLDER)
            {
                const xpr_tchar_t *sLastFolder = gOpt->mLastFolder[mViewIndex];
                if (fxb::IsEmptyString(sLastFolder) == XPR_FALSE)
                {
                    if ((XPR_IS_FALSE(gOpt->mExplorerNoNetLastFolder[mViewIndex])) ||
                        (XPR_IS_TRUE (gOpt->mExplorerNoNetLastFolder[mViewIndex]) && fxb::IsNetItem(sLastFolder) == XPR_FALSE))
                    {
                        sFullPidl = fxb::Path2Pidl(sLastFolder);
                    }
                }
            }
        }
    }

    // [5] default folder : My Documents
    if (XPR_IS_NULL(sFullPidl))
    {
        sInitFolderType = aFlags & InitFolderDefault;
        if (sInitFolderType != 0)
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);

            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder))
            {
                sShellFolder->ParseDisplayName(
                    XPR_NULL,
                    XPR_NULL,
                    XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"),
                    XPR_NULL,
                    &sFullPidl,
                    XPR_NULL);
            }

            COM_RELEASE(sShellFolder);
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
    saveOption();

    DESTROY_DELETE(mTabCtrl);
    DESTROY_DELETE(mFolderPane);
    DESTROY_DELETE(mAddressBar);
    DESTROY_DELETE(mPathBar);
    DESTROY_DELETE(mStatusBar);

    destroyPathBar();
    destroyActivateBar();
    destroyDrivePathBar();
    destroyContentsWnd();
    destroyStatusBar();

    super::OnDestroy();

    XPR_SAFE_DELETE(mListCtrlPrint);

    mDropTarget.Revoke();
}

void ExplorerView::saveOption(void)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->saveOption();
    }

    if (XPR_IS_NOT_NULL(mTabCtrl))
    {
        gOpt->mViewSplitTab[mViewIndex].mTabPathDeque.clear();
        gOpt->mViewSplitTab[mViewIndex].mCurTab = 0;

        if (XPR_IS_TRUE(gOpt->mTabSave))
        {
            gOpt->mViewSplitTab[mViewIndex].mCurTab = mTabCtrl->getCurTab();

            xpr_size_t i, sTabCount;
            ExplorerCtrl *sExplorerCtrl;
            ExplorerTabData *sExplorerTabData;

            sTabCount = mTabCtrl->getTabCount();
            for (i = 0; i < sTabCount; ++i)
            {
                sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(i));
                if (XPR_IS_NULL(sExplorerTabData))
                    continue;

                sExplorerCtrl = sExplorerTabData->getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
                    if (XPR_IS_NOT_NULL(sTvItemData))
                    {
                        std::tstring sTabPath;
                        fxb::Pidl2Path(sTvItemData->mFullPidl, sTabPath);

                        gOpt->mViewSplitTab[mViewIndex].mTabPathDeque.push_back(sTabPath);
                    }
                }
            }
        }
    }

    if (XPR_IS_FALSE(gOpt->mSingleFolderPaneMode))
    {
        gOpt->mShowEachFolderPane[mViewIndex] = XPR_IS_NOT_NULL(mFolderPane) ? XPR_TRUE : XPR_FALSE;
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

    HDWP sHdwp = ::BeginDeferWindowPos((xpr_sint_t)mTabCtrl->getTabCount() + 6);

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

    // splitter
    mSplitter.setWindowRect(sRect);

    xpr_sint_t sPaneSize = gOpt->mEachFolderPaneSize[mViewIndex];
    if (XPR_IS_FALSE(gOpt->mLeftEachFolderPane[mViewIndex]))
    {
        xpr_sint_t sSplitSize = mSplitter.getSplitSize();
        sPaneSize = sWidth- sPaneSize - sSplitSize;
    }

    mSplitter.moveColumn(0, sPaneSize);

    mSplitter.resize(sHdwp);

    xpr_sint_t sColumn = XPR_IS_TRUE(gOpt->mLeftEachFolderPane[mViewIndex]) ? 1 : 0;
    mSplitter.getPaneRect(0, sColumn, sRect);

    if (XPR_IS_TRUE(sExplorerTabMode))
    {
        // address bar
        if (isVisibleAddressBar() == XPR_TRUE && XPR_IS_NOT_NULL(mAddressBar) && XPR_IS_NOT_NULL(mAddressBar->m_hWnd))
        {
            xpr_sint_t sCxEdge = ::GetSystemMetrics(SM_CXEDGE);
            xpr_sint_t sCyEdge = ::GetSystemMetrics(SM_CYEDGE);

            ::DeferWindowPos(sHdwp, *mAddressBar, XPR_NULL, sRect.left - sCxEdge, sRect.top - sCyEdge, sRect.Width() + (sCxEdge * 2), 1000, SWP_NOZORDER);

            CRect sAddressWindowRect;
            mAddressBar->GetWindowRect(&sAddressWindowRect);

            sRect.top += sAddressWindowRect.Height();
            sRect.top -= sCyEdge;
        }

        // path bar and drive bar
        xpr_sint_t sDriveWidth = 0;
        xpr_sint_t sDriveHeight = 0;
        CRect sDriveBarRect(0,0,0,0);
        CRect sPathRect(0,0,0,0);

        xpr_sint_t sTop = sRect.top;
        if (XPR_IS_NOT_NULL(mPathBar) && XPR_IS_NOT_NULL(mPathBar->m_hWnd))
        {
            sPathRect = sRect;
            sPathRect.bottom = sPathRect.top + mPathBar->getHeight();
        }

        if (XPR_IS_NOT_NULL(mDrivePathBar) && XPR_IS_NOT_NULL(mDrivePathBar->m_hWnd))
        {
            sDriveBarRect = sRect;

            mDrivePathBar->getToolBarSize(&sDriveWidth, &sDriveHeight);
            if (XPR_IS_NOT_NULL(mPathBar) && XPR_IS_NOT_NULL(mPathBar->m_hWnd))
            {
                if (gFrame->isDriveViewSplitLeft() == XPR_TRUE)
                {
                    sDriveBarRect.left  = sRect.left;
                    sDriveBarRect.right = sDriveBarRect.left + sDriveWidth;
                    sPathRect.left = sDriveBarRect.right;
                }
                else
                {
                    sDriveBarRect.left  = sRect.right - sDriveWidth;
                    sDriveBarRect.right = sRect.right;
                    sPathRect.right -= sDriveWidth;
                }
            }

            sDriveBarRect.bottom = sRect.top + sDriveHeight;
        }

        sTop = max(sPathRect.bottom, sDriveBarRect.bottom);
        sPathRect.bottom  = sTop;
        sDriveBarRect.bottom = sTop;
        if (XPR_IS_NOT_NULL(mPathBar) && XPR_IS_NOT_NULL(mPathBar->m_hWnd))
        {
            ::DeferWindowPos(sHdwp, *mPathBar, XPR_NULL, sPathRect.left, sPathRect.top, sPathRect.Width(), sPathRect.Height(), SWP_NOZORDER);
        }

        if (XPR_IS_NOT_NULL(mDrivePathBar) && XPR_IS_NOT_NULL(mDrivePathBar->m_hWnd))
        {
            ::DeferWindowPos(sHdwp, *mDrivePathBar, XPR_NULL, sDriveBarRect.left, sDriveBarRect.top, sDriveBarRect.Width(), sDriveBarRect.Height(), SWP_NOZORDER);
        }

        sRect.top = max(sRect.top, sTop);
    }

    if (XPR_IS_NOT_NULL(mActivateBar) && XPR_IS_NOT_NULL(mActivateBar->m_hWnd))
    {
        CRect sActivateBarRect(sRect);
        sActivateBarRect.bottom = sActivateBarRect.top + 5;

        ::DeferWindowPos(sHdwp, *mActivateBar, XPR_NULL, sActivateBarRect.left, sActivateBarRect.top, sActivateBarRect.Width(), sActivateBarRect.Height(), SWP_NOZORDER);

        sRect.top += sActivateBarRect.Height();
    }

    if (XPR_IS_TRUE(sExplorerTabMode))
    {
        // status bar
        if (XPR_IS_NOT_NULL(mStatusBar) && XPR_IS_NOT_NULL(mStatusBar->m_hWnd))
        {
            CRect sStatusBarRect(sRect);
            sStatusBarRect.top = sStatusBarRect.bottom - mStatusBar->getDefaultHeight();

            ::DeferWindowPos(sHdwp, *mStatusBar, XPR_NULL, sStatusBarRect.left, sStatusBarRect.top, sStatusBarRect.Width(), sStatusBarRect.Height(), SWP_NOZORDER);

            sRect.bottom -= sStatusBarRect.Height();
        }

        // contents window
        if (XPR_IS_NOT_NULL(mContentsWnd) && XPR_IS_NOT_NULL(mContentsWnd->m_hWnd))
        {
            CRect sContentsWndRect(sRect);
            if (mMarginRect.left > 0) sContentsWndRect.right  = sContentsWndRect.left + mMarginRect.left;
            if (mMarginRect.top  > 0) sContentsWndRect.bottom = sContentsWndRect.top  + mMarginRect.top;

            ::DeferWindowPos(sHdwp, *mContentsWnd, XPR_NULL, sContentsWndRect.left, sContentsWndRect.top, sContentsWndRect.Width(), sContentsWndRect.Height(), SWP_NOZORDER);

            sRect.left   += mMarginRect.left;
            sRect.top    += mMarginRect.top;
            sRect.right  -= mMarginRect.right;
            sRect.bottom -= mMarginRect.bottom;
        }
    }

    // tab based window (such as ExplorerCtrl, SearchResultCtrl and so on...)
    if (XPR_IS_NOT_NULL(mTabCtrl) && XPR_IS_NOT_NULL(mTabCtrl->m_hWnd))
    {
        TabData *sTabData = (TabData *)mTabCtrl->getTabData(mTabCtrl->getCurTab());
        if (XPR_IS_NOT_NULL(sTabData))
        {
            if (XPR_IS_NOT_NULL(sTabData->mTabWnd))
            {
                ::DeferWindowPos(sHdwp, *sTabData->mTabWnd, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
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
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sDocName);

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
    if (DoPrintPreview(IDD_PRINT_PREVIEW, this, RUNTIME_CLASS(PreviewViewEx), sPrintPreviewState) == XPR_FALSE)
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

    if (XPR_IS_NOT_NULL(mDrivePathBar) && XPR_IS_NOT_NULL(mDrivePathBar->m_hWnd) && aWnd->m_hWnd == mDrivePathBar->m_hWnd)
    {
        BCMenu sMenu;
        if (sMenu.LoadMenu(IDR_COOLBAR_DRIVEBAR) == XPR_TRUE)
        {
            BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
            if (XPR_IS_NOT_NULL(sPopupMenu))
            {
                sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
                return;
            }
        }
    }

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

            fxb::ContextMenu::trackBackMenu(sShellFolder, &aPoint, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, GetSafeHwnd());
        }

        COM_RELEASE(sShellFolder);
        return;
    }

    // flyExplorer's own context menu
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
                    if (XPR_IS_TRUE(gOpt->mFileScrapContextMenu))
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

xpr_uint_t ExplorerView::generateTabWndId(void)
{
    xpr_uint_t sWndId = mNextExplorerCtrlId;
    CWnd *sChildWnd;

    do
    {
        sChildWnd = GetDlgItem(sWndId);
        if (sChildWnd == XPR_NULL)
        {
            mNextExplorerCtrlId = sWndId + 1;

            return sWndId;
        }

        if (sWndId == kuint16max)
            sWndId = CTRL_ID_EXPLORER_CTRL;
        else
            ++sWndId;

    } while (true);

    return CTRL_ID_EXPLORER_CTRL;
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

xpr_sint_t ExplorerView::newTab(const std::tstring &aInitFolder)
{
    LPITEMIDLIST sFullPidl = fxb::Path2Pidl(aInitFolder);

    xpr_sint_t sTab = newTab(sFullPidl);

    COM_FREE(sFullPidl);

    return sTab;
}

xpr_sint_t ExplorerView::newTab(LPITEMIDLIST aInitFolder)
{
    if (XPR_IS_NULL(mTabCtrl))
        return -1;

    ExplorerTabData *sExplorerTabData;
    TabPathDeque::iterator sIterator;

    sExplorerTabData = new ExplorerTabData;
    if (XPR_IS_NULL(sExplorerTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;

    ExplorerCtrl *sExplorerCtrl = new ExplorerCtrl;
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerTabData->mTabWnd = sExplorerCtrl;
        sExplorerTabData->mStatusPane0[0] = 0;
        sExplorerTabData->mStatusPane1[0] = 0;

        sExplorerCtrl->setObserver(dynamic_cast<ExplorerCtrlObserver *>(this));
        sExplorerCtrl->setViewIndex(mViewIndex);

        xpr_uint_t sExplorerCtrlId = generateTabWndId();
        if (sExplorerCtrl->Create(this, sExplorerCtrlId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            sInsertedTab = mTabCtrl->addTab(XPR_NULL, -1, sExplorerTabData);
        }
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sExplorerTabData);

        return -1;
    }

    // set image list
    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();
    sExplorerCtrl->setImageList(&sSysImgListMgr.mSysImgList32, &sSysImgListMgr.mSysImgList16);

    // apply configuration
    OptionMgr &sOptionMgr = OptionMgr::instance();
    sOptionMgr.applyExplorerCtrl(sExplorerCtrl, XPR_TRUE);

    // initailize folder location
    xpr_bool_t sExplored = XPR_FALSE;

    if (XPR_IS_NOT_NULL(aInitFolder))
    {
        LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(aInitFolder);
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

    std::tstring sSelFile;

    if (XPR_IS_FALSE(sExplored))
    {
        LPITEMIDLIST sFullPidl;
        xpr_uint_t sFlags, sInitFolderType;

        sFlags = InitFolderAll;
        sFlags &= ~InitFolderLastSaved;
        sInitFolderType = InitFolderNone;

        if (XPR_IS_FALSE(gOpt->mSplitLastFolder))
            sFlags &= ~InitFolderSplit;

        sFullPidl = getInitFolder(mViewIndex, sSelFile, sFlags, &sInitFolderType);

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

            sFullPidl = getInitFolder(mViewIndex, sSelFile, sFlags);
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
    if (sSelFile.empty() == false)
    {
        xpr_sint_t sFind = sExplorerCtrl->findItemFileName(sSelFile.c_str());
        if (sFind >= 0)
        {
            sExplorerCtrl->unselectAll();
            sExplorerCtrl->selectItem(sFind);
        }
    }

    // set view style
    if (gOpt->mExplorerSaveViewSet == SAVE_VIEW_SET_NONE && XPR_IS_TRUE(gOpt->mExplorerSaveViewStyle))
    {
        if (gOpt->mViewStyle[mViewIndex] != LVS_REPORT)
            sExplorerCtrl->setViewStyle(gOpt->mViewStyle[mViewIndex], XPR_TRUE);
    }

    // load history
    if (XPR_IS_TRUE(gOpt->mSaveHistory))
    {
        // TODO each tab data
        sExplorerCtrl->loadHistory();
    }

    return (xpr_sint_t)sInsertedTab;
}

xpr_sint_t ExplorerView::newSearchResultTab(void)
{
    SearchResultTabData *sSearchResultTabData = new SearchResultTabData;
    if (XPR_IS_NULL(sSearchResultTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;

    SearchResultPane *sSearchResultPane = new SearchResultPane;
    if (XPR_IS_NOT_NULL(sSearchResultPane))
    {
        sSearchResultTabData->mTabWnd = sSearchResultPane;

        sSearchResultPane->setObserver(dynamic_cast<SearchResultPaneObserver *>(this));

        xpr_uint_t sSearchResultPaneId = generateTabWndId();
        if (sSearchResultPane->Create(this, sSearchResultPaneId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            const xpr_tchar_t *sTitle = theApp.loadString(XPR_STRING_LITERAL("search_result.title"));

            sInsertedTab = mTabCtrl->addTab(sTitle, -1, sSearchResultTabData);
        }
        else
        {
            XPR_SAFE_DELETE(sSearchResultPane);
        }
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sSearchResultTabData);

        return -1;
    }

    return (xpr_sint_t)sInsertedTab;
}

xpr_sint_t ExplorerView::newFileScrapTab(void)
{
    FileScrapTabData *sFileScrapTabData = new FileScrapTabData;
    if (XPR_IS_NULL(sFileScrapTabData))
        return -1;

    xpr_size_t sInsertedTab = TabCtrl::InvalidTab;

    FileScrapPane *sFileScrapPane = new FileScrapPane;
    if (XPR_IS_NOT_NULL(sFileScrapPane))
    {
        sFileScrapTabData->mTabWnd = sFileScrapPane;

        sFileScrapPane->setObserver(dynamic_cast<FileScrapPaneObserver *>(this));

        xpr_uint_t sFileScrapPaneId = generateTabWndId();
        if (sFileScrapPane->Create(this, sFileScrapPaneId, CRect(0,0,0,0)) == XPR_TRUE)
        {
            const xpr_tchar_t *sTitle = theApp.loadString(XPR_STRING_LITERAL("file_scrap.title"));

            sInsertedTab = mTabCtrl->addTab(sTitle, -1, sFileScrapTabData);
        }
        else
        {
            XPR_SAFE_DELETE(sFileScrapPane);
        }
    }

    if (sInsertedTab == TabCtrl::InvalidTab)
    {
        XPR_SAFE_DELETE(sFileScrapTabData);

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

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return -1;

    ExplorerCtrl *sExplorerCtrl = sExplorerTabData->getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            std::tstring sTabPath;
            fxb::Pidl2Path(sTvItemData->mFullPidl, sTabPath);

            xpr_sint_t sNewTab = newTab(sTabPath);
            if (sNewTab >= 0)
            {
                mTabCtrl->setCurTab(sNewTab);

                return sNewTab;
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

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sExplorerTabData))
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

    xpr_sint_t sTab = findTabWndId(mLastActivedExplorerCtrlId);
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

xpr_sint_t ExplorerView::findTabWndId(xpr_uint_t aTabWndId) const
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
            if (XPR_IS_NOT_NULL(sTabData->mTabWnd))
            {
                if (sTabData->mTabWnd->GetDlgCtrlID() == aTabWndId)
                {
                    return (xpr_sint_t)i;
                }
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
            if (sTabData->mTabType == TabTypeExplorer)
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

CWnd *ExplorerView::getTabWnd(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab < 0)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return XPR_NULL;

    return sTabData->mTabWnd;
}

xpr_uint_t ExplorerView::getTabWndId(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return 0;

    xpr_sint_t sTab = aTab;
    if (sTab < 0)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NULL(sTabData))
        return 0;

    if (XPR_IS_NULL(sTabData->mTabWnd))
        return 0;

    return sTabData->mTabWnd->GetDlgCtrlID();
}

xpr_bool_t ExplorerView::isExplorerTabMode(void) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    TabData *sTabData = (TabData *)mTabCtrl->getTabData(sTab);
    if (XPR_IS_NOT_NULL(sTabData) && sTabData->mTabType == TabTypeExplorer)
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
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        if (XPR_IS_TRUE(gOpt->mSingleFolderTreeLinkage))
            return gFrame->getFolderCtrl();

        return XPR_NULL;
    }

    FolderPane *sFolderPane = getFolderPane();
    if (XPR_IS_NULL(sFolderPane))
        return XPR_NULL;

    return sFolderPane->getFolderCtrl();
}

ExplorerCtrl *ExplorerView::getExplorerCtrl(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return XPR_NULL;

    return sExplorerTabData->getExplorerCtrl();
}

SearchResultCtrl *ExplorerView::getSearchResultCtrl(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    SearchResultTabData *sSearchResultTabData = dynamic_cast<SearchResultTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sSearchResultTabData))
        return XPR_NULL;

    return sSearchResultTabData->getSearchResultPane()->getSearchResultCtrl();
}

FileScrapPane *ExplorerView::getFileScrapPane(xpr_sint_t aTab) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_sint_t sTab = aTab;
    if (sTab == -1)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    FileScrapTabData *sFileScrapTabData = dynamic_cast<FileScrapTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sFileScrapTabData))
        return XPR_NULL;

    return sFileScrapTabData->getFileScrapPane();
}

AddressBar *ExplorerView::getAddressBar(void) const
{
    return mAddressBar;
}

PathBar *ExplorerView::getPathBar(void) const
{
    return mPathBar;
}

xpr_bool_t ExplorerView::visibleFolderPane(xpr_bool_t aVisible, xpr_bool_t aLoading, xpr_bool_t aSaveFolderPaneVisible)
{
    if (XPR_IS_TRUE(aSaveFolderPaneVisible))
        gOpt->mShowEachFolderPane[mViewIndex] = aVisible;

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
                            if (gOpt->mFolderTreeInitNoExpand == XPR_FALSE)
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

        xpr_sint_t sPaneSize = gOpt->mEachFolderPaneSize[mViewIndex];
        if (XPR_IS_FALSE(gOpt->mLeftEachFolderPane[mViewIndex]))
        {
            xpr_sint_t sSplitSize = mSplitter.getSplitSize();
            sPaneSize = sWindowRect.Width() - sPaneSize - sSplitSize;
        }

        if (XPR_IS_TRUE(gOpt->mLeftEachFolderPane[mViewIndex]))
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

    if (XPR_IS_TRUE(gOpt->mLeftEachFolderPane[mViewIndex]))
        mSplitter.switchPane(0, 1, 0, 0);

    mSplitter.split(1, 1);
    mSplitter.resize();

    mFolderPane->DestroyWindow();
    XPR_SAFE_DELETE(mFolderPane);
}

xpr_bool_t ExplorerView::isLeftFolderPane(void) const
{
    return gOpt->mLeftEachFolderPane[mViewIndex];
}

void ExplorerView::setLeftFolderPane(xpr_bool_t aLeft)
{
    if (isVisibleFolderPane() == XPR_FALSE)
        return;

    if (gOpt->mLeftEachFolderPane[mViewIndex] != aLeft)
    {
        gOpt->mLeftEachFolderPane[mViewIndex] = aLeft;

        mSplitter.switchPane(0, 0, 0, 1);

        recalcLayout();
    }
}

void ExplorerView::visibleAddressBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createAddressBar();

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
                mAddressBar->explore(sTvItemData->mFullPidl);
        }
    }
    else
    {
        destroyAddressBar();
    }

    gOpt->mAddressBar[mViewIndex] = aVisible;

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerView::isVisibleAddressBar(void) const
{
    return gOpt->mAddressBar[mViewIndex];
}

void ExplorerView::createAddressBar(void)
{
    if (XPR_IS_NOT_NULL(mAddressBar))
        return;

    mAddressBar = new AddressBar;
    if (XPR_IS_NOT_NULL(mAddressBar))
    {
        mAddressBar->setObserver(dynamic_cast<AddressBarObserver *>(this));
        mAddressBar->Create(this, CTRL_ID_ADDRESS_BAR, CRect(0,0,0,300));
        mAddressBar->setSystemImageList(&fxb::SysImgListMgr::instance().mSysImgList16);
        mAddressBar->setCustomFont(gOpt->mCustomFontText);
    }
}

void ExplorerView::destroyAddressBar(void)
{
    if (XPR_IS_NULL(mAddressBar))
        return;

    DESTROY_DELETE(mAddressBar);
}

void ExplorerView::createPathBar(void)
{
    if (XPR_IS_NOT_NULL(mPathBar))
        return;

    mPathBar = new PathBar;
    if (XPR_IS_NOT_NULL(mPathBar))
    {
        mPathBar->setObserver(dynamic_cast<PathBarObserver *>(this));

        if (mPathBar->Create(this, CTRL_ID_PATH_BAR, CRect(0,0,0,0)) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(mPathBar);
        }

        mPathBar->setMode(gOpt->mPathBarRealPath);
    }
}

void ExplorerView::destroyPathBar(void)
{
    if (XPR_IS_NULL(mPathBar))
        return;

    DESTROY_DELETE(mPathBar);
}

void ExplorerView::visiblePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createPathBar();

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
                mPathBar->setPath(sTvItemData->mFullPidl);
        }
    }
    else
    {
        destroyPathBar();
    }

    gOpt->mPathBar[mViewIndex] = aVisible;

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerView::isVisiblePathBar(void) const
{
    return gOpt->mPathBar[mViewIndex];
}

void ExplorerView::setPathBarMode(xpr_bool_t aRealPath)
{
    if (XPR_IS_NOT_NULL(mPathBar) && XPR_IS_NOT_NULL(mPathBar->m_hWnd))
        mPathBar->setMode(aRealPath);
}

ActivateBar *ExplorerView::getActivateBar(void) const
{
    return mActivateBar;
}

void ExplorerView::createActivateBar(void)
{
    if (XPR_IS_NOT_NULL(mActivateBar))
        return;

    mActivateBar = new ActivateBar;
    if (XPR_IS_NOT_NULL(mActivateBar))
    {
        if (mActivateBar->Create(this, CTRL_ID_ACTIVATE_BAR, CRect(0,0,0,0)) == XPR_FALSE)
            return;
    }
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

    gOpt->mActivateBar[mViewIndex] = aVisible;

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerView::isVisibleActivateBar(void) const
{
    return gOpt->mActivateBar[mViewIndex];
}

void ExplorerView::createDrivePathBar(void)
{
    if (XPR_IS_NOT_NULL(mDrivePathBar))
        return;

    mDrivePathBar = new DrivePathBar;
    if (XPR_IS_NOT_NULL(mDrivePathBar))
    {
        if (mDrivePathBar->CreateEx(this) == XPR_FALSE)
            return;

        mDrivePathBar->createDriveBar();
    }
}

void ExplorerView::destroyDrivePathBar(void)
{
    if (XPR_IS_NULL(mDrivePathBar))
        return;

    mDrivePathBar->destroyDriveBar();
    DESTROY_DELETE(mDrivePathBar);
}

DrivePathBar *ExplorerView::getDrivePathBar(void) const
{
    return mDrivePathBar;
}

void ExplorerView::setDrivePathBarLeft(xpr_bool_t aLeft)
{
    recalcLayout();
}

void ExplorerView::setDrivePathBarShortText(xpr_bool_t aShortText)
{
    if (XPR_IS_NOT_NULL(mDrivePathBar) && XPR_IS_NOT_NULL(mDrivePathBar->m_hWnd))
    {
        mDrivePathBar->setShortText(aShortText);
        recalcLayout();
    }
}

void ExplorerView::visibleDrivePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createDrivePathBar();
    }
    else
    {
        destroyDrivePathBar();
    }

    mIsDrivePathBar = aVisible;

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

void ExplorerView::createContentsWnd(void)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        return;

    mContentsWnd = new ContentsWnd;
    if (XPR_IS_NOT_NULL(mContentsWnd))
    {
        mContentsWnd->Create(this, CTRL_ID_CONTENTS_WND, CRect(0,0,0,0));
    }
}

void ExplorerView::destroyContentsWnd(void)
{
    DESTROY_DELETE(mContentsWnd);
}

xpr_bool_t ExplorerView::isVisibleContentsWnd(void) const
{
    return (mContentsWnd != XPR_NULL);
}

void ExplorerView::setContentsStyle(xpr_sint_t aContentsStyle)
{
    switch (aContentsStyle)
    {
    case CONTENTS_EXPLORER:
        {
            createContentsWnd();

            // default contents
            mMarginRect = CRect(CONTENTS_EXPLORER_STYLE_WIDTH, 0, 0, 0);

            ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
                sExplorerCtrl->resetStatus();

            setBookmarkColor(gOpt->mContentsBookmarkColor);
            updateBookmark();
            setBookmarkPopup(gOpt->mContentsTooltip);
            enableBookmark(gOpt->mContentsBookmark);

            break;
        }

    case CONTENTS_BASIC:
        {
            createContentsWnd();

            // basic contents
            mMarginRect = CRect(0, CONTENTS_BASIC_STYLE_HEIGHT, 0, 0);

            updateBookmark();
            setBookmarkPopup(XPR_FALSE);
            enableBookmark(XPR_FALSE);

            break;
        }

    case CONTENTS_NONE:
    default:
        {
            destroyContentsWnd();

            mMarginRect = CRect(0, 0, 0, 0);

            break;
        }
    }
}

void ExplorerView::setContentsFolder(LPTVITEMDATA aTvItemData)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsFolder(aTvItemData);
}

void ExplorerView::setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsNormal(aTvItemData, aUpdate);
}

void ExplorerView::setContentsSingleItem(LPLVITEMDATA       aLvItemData,
                                         const xpr_tchar_t *aName,
                                         const xpr_tchar_t *aType,
                                         const xpr_tchar_t *aDate,
                                         const xpr_tchar_t *aSize,
                                         const xpr_tchar_t *aAttr)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsSingleItem(aLvItemData, aName, aType, aDate, aSize, aAttr);
}

void ExplorerView::setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsMultiItem(aCount, aSize, aNames);
}

void ExplorerView::setBookmarkPopup(xpr_bool_t aPopup)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setBookmarkPopup(aPopup);
}

void ExplorerView::setBookmarkColor(COLORREF aBookmarkColor)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setBookmarkColor(aBookmarkColor);
}

void ExplorerView::updateBookmark(xpr_bool_t aUpdatePosition)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
    {
        mContentsWnd->updateBookmark();

        if (XPR_IS_TRUE(aUpdatePosition))
        {
            mContentsWnd->updateBookmarkPosition();
            mContentsWnd->enableBookmark(mContentsWnd->isEnableBookmark());
        }
    }
}

void ExplorerView::enableBookmark(xpr_bool_t aEnable)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->enableBookmark(aEnable);
}

xpr_bool_t ExplorerView::isEnableBookmark(void) const
{
    if (XPR_IS_NULL(mContentsWnd))
        return XPR_FALSE;

    return mContentsWnd->isEnableBookmark();
}

xpr_bool_t ExplorerView::isVisibleBookmark(void) const
{
    if (XPR_IS_NULL(mContentsWnd))
        return XPR_FALSE;

    return mContentsWnd->isVisibleBookmark();
}

void ExplorerView::invalidateContentsWnd(void)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->Invalidate(XPR_TRUE);
}

void ExplorerView::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);
}

void ExplorerView::createStatusBar(void)
{
    if (XPR_IS_NOT_NULL(mStatusBar))
        return;

    mStatusBar = new StatusBarEx;
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        mStatusBar->setObserver(dynamic_cast<StatusBarObserver *>(this));

        if (mStatusBar->Create(this, CTRL_ID_STATUS_BAR, CRect(0,0,0,0)) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(mStatusBar);
        }
    }
}

void ExplorerView::destroyStatusBar(void)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    DESTROY_DELETE(mStatusBar);
}

void ExplorerView::visibleStatusBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createStatusBar();
    }
    else
    {
        destroyStatusBar();
    }

    gOpt->mStatusBar[mViewIndex] = aVisible;

    if (XPR_IS_TRUE(aVisible))
    {
        updateStatusBar();
    }

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerView::isVisibleStatusBar(void) const
{
    return gOpt->mStatusBar[mViewIndex];
}

StatusBar *ExplorerView::getStatusBar(void) const
{
    return mStatusBar;
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
        CWnd *sTabWnd = getTabWnd(sCurTab);
        if (XPR_IS_NOT_NULL(sTabWnd))
        {
            sTabWnd->SetFocus();
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
                    if (XPR_IS_TRUE(gOpt->mSingleFolderTreeLinkage))
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
        if (XPR_IS_NOT_NULL(sOldTabData->mTabWnd))
        {
            sOldTabData->mTabWnd->ShowWindow(SW_HIDE);
        }
    }

    TabData *sNewTabData = (TabData *)aTabCtrl.getTabData(aNewTab);
    if (XPR_IS_NOT_NULL(sNewTabData))
    {
        ExplorerTabData *sNewExplorerTabData = dynamic_cast<ExplorerTabData *>(sNewTabData);
        if (XPR_IS_NOT_NULL(sNewExplorerTabData))
        {
            ExplorerCtrl *sNewExplorerCtrl = sNewExplorerTabData->getExplorerCtrl();
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

                    // update address bar
                    AddressBar *sAddressBar = getAddressBar();
                    if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
                    {
                        sAddressBar->explore(sTvItemData->mFullPidl);
                    }

                    // update path bar
                    PathBar *sPathBar = getPathBar();
                    if (XPR_IS_NOT_NULL(sPathBar) && XPR_IS_NOT_NULL(sPathBar->m_hWnd))
                    {
                        sPathBar->setPath(sTvItemData->mFullPidl);
                    }

                    // update contents
                    if (XPR_IS_NOT_NULL(mContentsWnd) && XPR_IS_NOT_NULL(mContentsWnd->m_hWnd))
                    {
                        setContentsFolder(sTvItemData);
                        setContentsNormal(sTvItemData, XPR_TRUE);
                    }

                    // update status bar
                    updateStatusBar();

                    // update tab text
                    std::tstring sName;
                    fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);

                    aTabCtrl.setTabText(aNewTab, sName.c_str());

                    if (gFrame->getActiveView() == mViewIndex)
                    {
                        // update main title
                        gFrame->setMainTitle(sTvItemData->mShellFolder, sTvItemData->mPidl);
                    }

                    mLastActivedExplorerCtrlId = sNewExplorerCtrl->GetDlgCtrlID();
                }
            }
        }

        if (XPR_IS_NOT_NULL(sNewTabData->mTabWnd))
        {
            sNewTabData->mTabWnd->ShowWindow(SW_SHOW);

            if (XPR_IS_FALSE(mInit))
                sNewTabData->mTabWnd->SetFocus();
        }
    }

    // tab mode change (explorer tab mode <-> non-explorer tab mode)
    xpr_bool_t sExplorerTabMode = isExplorerTabMode();

    CWnd *sTabModeChangeWnds[] = {
        mContentsWnd,
        mAddressBar,
        mPathBar,
        mDrivePathBar,
        mStatusBar
    };

    xpr_size_t i;
    xpr_size_t sCount;
    CWnd *sWnd;

    sCount = XPR_COUNT_OF(sTabModeChangeWnds);
    for (i = 0; i < sCount; ++i)
    {
        sWnd = sTabModeChangeWnds[i];

        if (XPR_IS_TRUE(sExplorerTabMode))
        {
            if (XPR_IS_NOT_NULL(sWnd) && XPR_IS_NOT_NULL(sWnd->m_hWnd) && sWnd->IsWindowVisible() == XPR_FALSE)
            {
                sWnd->ShowWindow(SW_SHOW);
            }
        }
        else
        {
            if (XPR_IS_NOT_NULL(sWnd) && XPR_IS_NOT_NULL(sWnd->m_hWnd) && sWnd->IsWindowVisible() == XPR_TRUE)
            {
                sWnd->ShowWindow(SW_HIDE);
            }
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

        gOpt->mEachFolderPaneSize[mViewIndex] = sPaneSize.cx;

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

        gOpt->mEachFolderPaneSize[mViewIndex] = sPaneSize.cx;

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

    if (XPR_IS_TRUE(gOpt->mShowEachFolderPane[mViewIndex]) && XPR_IS_FALSE(gOpt->mLeftEachFolderPane[mViewIndex]))
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

    LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(aFullPidl);
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
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    xpr_uint_t sExplorerCtrlId = sExplorerCtrl->GetDlgCtrlID();
    xpr_sint_t sTab = findTabWndId(sExplorerCtrlId);

    updateStatusBar(sTab);
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

    fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();
    LPDATAOBJECT sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    if (fxb::IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = fxb::DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }
    else if (fxb::IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = fxb::DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }
    else if (fxb::IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = fxb::DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }
    else if (fxb::IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = fxb::DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteBitmap(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteDIB(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteUnicodeText(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewTextFile(*sExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteText(aOleDataObject) == XPR_TRUE)
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
// from ExplorerCtrlObserver
//
xpr_bool_t ExplorerView::onExplore(AddressBar &aAddressBar, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    if (XPR_IS_FALSE(aUpdateBuddy))
        return XPR_TRUE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_TRUE;

    xpr_bool_t sResult = XPR_FALSE;

    LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(aFullPidl);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        sResult = sExplorerCtrl->explore(sFullPidl);
        if (XPR_IS_FALSE(sResult))
        {
            COM_FREE(sFullPidl);
        }
    }

    return sResult;
}

LPITEMIDLIST ExplorerView::onFailExec(AddressBar &aAddressBar, const xpr_tchar_t *aExecPath)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_NULL;

    xpr_sint_t sIndex = sExplorerCtrl->findItemName(aExecPath);
    if (sIndex < 0)
        sIndex = sExplorerCtrl->findItemFileName(aExecPath);

    if (sIndex > 0)
    {
        LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);

        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            return fxb::ConcatPidls(sTvItemData->mFullPidl, sLvItemData->mPidl);
        }
    }

    return XPR_NULL;
}

void ExplorerView::onMoveFocus(AddressBar &aAddressBar)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onMoveFocus(*this, 1);
}

xpr_bool_t ExplorerView::onExplore(PathBar &aPathBar, LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(aFullPidl);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sResult = sExplorerCtrl->explore(sFullPidl);
            if (XPR_IS_FALSE(sResult))
            {
                COM_FREE(sFullPidl);
            }
        }
    }

    return sResult;
}

void ExplorerView::onExplore(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy, const xpr_tchar_t *aCurPath)
{
    if (XPR_IS_NULL(aFullPidl))
        return;

    if (XPR_IS_NULL(mTabCtrl))
        return;

    LPTVITEMDATA sTvItemData = aExplorerCtrl.getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return;

    xpr_uint_t sExplorerCtrlId = aExplorerCtrl.GetDlgCtrlID();

    xpr_size_t sCurTab = mTabCtrl->getCurTab();
    xpr_sint_t sTab = findTabWndId(sExplorerCtrlId);

    if (sTab != -1 && sTab == sCurTab)
    {
        // update folder control
        FolderCtrl *sFolderCtrl = getFolderCtrl();
        if (aUpdateBuddy == XPR_TRUE && XPR_IS_NOT_NULL(sFolderCtrl))
        {
            sFolderCtrl->searchSel(aFullPidl);
        }

        // update address bar
        AddressBar *sAddressBar = getAddressBar();
        if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
            sAddressBar->explore(sTvItemData->mFullPidl);

        // update path bar
        PathBar *sPathBar = getPathBar();
        if (XPR_IS_NOT_NULL(sPathBar) && XPR_IS_NOT_NULL(sPathBar->m_hWnd))
            sPathBar->setPath(sTvItemData->mFullPidl);

        // update contents
        if (isVisibleContentsWnd() == XPR_TRUE)
        {
            setContentsFolder(sTvItemData);
            setContentsNormal(sTvItemData, XPR_TRUE);
        }
    }

    if (sTab >= 0)
    {
        std::tstring sName;
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);

        mTabCtrl->setTabText(sTab, sName.c_str());
    }

    if (gFrame->getActiveView() == mViewIndex)
    {
        // update main title
        gFrame->setMainTitle(sTvItemData->mShellFolder, sTvItemData->mPidl);
    }

    // update status bar
    updateStatusBar(sTab);
    //SetStatusBarDrive();
}

void ExplorerView::onUpdateStatus(ExplorerCtrl &aExplorerCtrl, xpr_size_t aRealSelCount)
{
    if (aRealSelCount == 0)
    {
        OnExpSelNormal();
    }
    else if (aRealSelCount == 1)
    {
        OnExpSelSingleItem();
    }
    else if (aRealSelCount > 1)
    {
        OnExpSelMultiItem();
    }
}

void ExplorerView::onUpdateFreeSize(ExplorerCtrl &aExplorerCtrl)
{
    xpr_uint_t sExplorerCtrlId = aExplorerCtrl.GetDlgCtrlID();
    xpr_sint_t sTab = findTabWndId(sExplorerCtrlId);

    updateStatusBar(sTab);
}

void ExplorerView::onSetFocus(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->setActiveView(mViewIndex);
}

void ExplorerView::onKillFocus(ExplorerCtrl &aExplorerCtrl)
{
}

void ExplorerView::onRename(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_RENAME);
}

void ExplorerView::onContextMenuDelete(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_DELETE);
}

void ExplorerView::onContextMenuRename(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_RENAME);
}

void ExplorerView::onContextMenuPaste(ExplorerCtrl &aExplorerCtrl)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdIsSelectedItem, (void *)XPR_TRUE);

    gFrame->executeCommand(ID_EDIT_PASTE, &aExplorerCtrl, &sParameters);
}

void ExplorerView::onGoDrive(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_GO_DRIVE);
}

xpr_sint_t ExplorerView::onDrop(ExplorerCtrl &aExplorerCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
{
    xpr_sint_t sResult = 0;

    fxb::ClipFormat &sClipFormat = fxb::ClipFormat::instance();
    LPDATAOBJECT sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);

    if (fxb::IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = fxb::DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteInetUrl(aOleDataObject, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = fxb::DoPasteInetUrl(sDataObject, aTargetDir, sClipFormat.mInetUrl, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA) == XPR_TRUE)
    {
        sResult = fxb::DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorA, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteFileContents(aOleDataObject, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW) == XPR_TRUE)
    {
        sResult = fxb::DoPasteFileContents(sDataObject, aTargetDir, sClipFormat.mFileContents, sClipFormat.mFileDescriptorW, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteBitmap(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(aExplorerCtrl, aTargetDir, sDataObject, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteDIB(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewBitmapFile(aExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteUnicodeText(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewTextFile(aExplorerCtrl, aTargetDir, sDataObject, XPR_TRUE);
        if (sResult != 0)
            return sResult;
    }

    if (fxb::IsPasteText(aOleDataObject) == XPR_TRUE)
    {
        sResult = cmd::pasteNewTextFile(aExplorerCtrl, aTargetDir, sDataObject, XPR_FALSE);
        if (sResult != 0)
            return sResult;
    }

    return sResult;
}

xpr_bool_t ExplorerView::onGetExecParam(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath, xpr_tchar_t *aParam, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aParam) || aMaxLen <= 0)
        return XPR_TRUE;

    ParamExecDlg sDlg;
    sDlg.setPath(aPath);
    if (sDlg.DoModal() == IDCANCEL)
        return XPR_FALSE;

    const xpr_tchar_t *sParameter = sDlg.getParameter();
    if (_tcslen(sParameter) <= aMaxLen)
    {
        _tcscpy(aParam, sParameter);
    }

    return XPR_TRUE;
}

void ExplorerView::onExecError(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH * 2 + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.msg.wrong_path"), XPR_STRING_LITERAL("%s")), aPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
}

void ExplorerView::onSetViewStyle(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aStyle, xpr_bool_t aRefresh)
{
    if (gOpt->mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerCtrl = gFrame->getExplorerCtrl(i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                if (&aExplorerCtrl != sExplorerCtrl)
                {
                    sExplorerCtrl->setViewStyle(aStyle, aRefresh, XPR_FALSE);
                }
            }
        }
    }
}

void ExplorerView::onUseColumn(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId)
{
    if (gOpt->mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerCtrl = gFrame->getExplorerCtrl(i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                if (&aExplorerCtrl != sExplorerCtrl)
                {
                    sExplorerCtrl->useColumn(aColumnId, XPR_FALSE);
                }
            }
        }
    }
}

void ExplorerView::onSortItems(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId, xpr_bool_t aAscending)
{
    if (gOpt->mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerCtrl = gFrame->getExplorerCtrl(i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                if (&aExplorerCtrl != sExplorerCtrl)
                {
                    sExplorerCtrl->sortItems(aColumnId, aAscending, XPR_FALSE);
                }
            }
        }
    }
}

void ExplorerView::onSetColumnWidth(ExplorerCtrl &aExplorerCtrl, xpr_sint_t sColumnIndex, xpr_sint_t sWidth)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = gFrame->getViewCount();
    ExplorerCtrl *sExplorerCtrl;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerCtrl = gFrame->getExplorerCtrl(i);
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            if (&aExplorerCtrl != sExplorerCtrl)
            {
                sExplorerCtrl->SetColumnWidth(sColumnIndex, sWidth);
            }
        }
    }
}

void ExplorerView::onMoveFocus(ExplorerCtrl &aExplorerCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onMoveFocus(*this, 0);
}

void ExplorerView::OnExpSelNormal(void)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return;

    xpr_bool_t sVisibleContentsWnd = isVisibleContentsWnd();
    xpr_bool_t sVisibleStatusBar   = isVisibleStatusBar();

    if (XPR_IS_FALSE(sVisibleContentsWnd) && XPR_IS_FALSE(sVisibleStatusBar))
        return;

    if (XPR_IS_TRUE(sVisibleContentsWnd))
        setContentsNormal(sTvItemData);

    if (XPR_IS_TRUE(sVisibleStatusBar))
    {
        xpr_uint_t sExplorerCtrlId = sExplorerCtrl->GetDlgCtrlID();
        xpr_sint_t sTab = findTabWndId(sExplorerCtrlId);

        updateStatusBar(sTab);
    }
}

void ExplorerView::OnExpSelSingleItem(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sCurTab = mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sCurTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return;

    xpr_bool_t sVisibleContentsWnd = isVisibleContentsWnd();
    xpr_bool_t sVisibleStatusBar   = isVisibleStatusBar();

    if (XPR_IS_FALSE(sVisibleContentsWnd) && XPR_IS_FALSE(sVisibleStatusBar))
        return;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    xpr_sint_t sIndex = -1;

    POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);
        if (sExplorerCtrl->GetItemData(sIndex))
            break;
    }

    if (sIndex == -1)
    {
        OnExpSelNormal();
        return;
    }

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
    if (XPR_IS_NULL(sLvItemData))
    {
        OnExpSelNormal();
        return;
    }

    static xpr_uint64_t sFileSize;
    static xpr_tchar_t sName[XPR_MAX_PATH + 1];
    static xpr_tchar_t sType[XPR_MAX_PATH + 1];
    static xpr_tchar_t sDate[XPR_MAX_PATH + 1];
    static xpr_tchar_t sSize[XPR_MAX_PATH + 1];
    static xpr_tchar_t sAttr[XPR_MAX_PATH + 1];
    sFileSize = 0;
    sName[0] = sType[0] = sDate[0] = sSize[0] = sAttr[0] = XPR_STRING_LITERAL('\0');

    sExplorerCtrl->getItemName(sIndex, sName, XPR_MAX_PATH, EXT_TYPE_ALWAYS);
    sExplorerCtrl->getFileType(sIndex, sType, XPR_MAX_PATH);
    sExplorerCtrl->getFileTime(sIndex, sDate, XPR_MAX_PATH);
    sExplorerCtrl->getFileSize(sIndex, sFileSize);
    sExplorerCtrl->getFileAttr(sIndex, sAttr, XPR_MAX_PATH);

    if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
    {
        xpr_sint_t sUnit = gOpt->mSingleSelFileSizeUnit;
        switch (sUnit)
        {
        case SIZE_UNIT_DEFAULT:
            fxb::SizeFormat::getDefSizeFormat(sFileSize, sSize, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_CUSTOM:
            fxb::SizeFormat::instance().getSizeFormat(sFileSize, sSize, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_NONE:
            fxb::GetFormatedNumber(sFileSize, sSize, XPR_MAX_PATH);
            break;

        default:
            fxb::SizeFormat::getSizeUnitFormat(sFileSize, sUnit, sSize, XPR_MAX_PATH);
            break;
        }
    }

    if (XPR_IS_TRUE(sVisibleContentsWnd))
        setContentsSingleItem(sLvItemData, sName, sType, sDate, sSize, sAttr);

    if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
    {
        _tcscpy(sExplorerTabData->mStatusPane0, theApp.loadString(XPR_STRING_LITERAL("explorer_window.status.pane1.single_selected")));
        _tcscpy(sExplorerTabData->mStatusPane1, XPR_STRING_LITERAL(""));

        if (XPR_IS_TRUE(sVisibleStatusBar))
            setStatusBarText();
    }
    else
    {
        _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.single_selected_file"), XPR_STRING_LITERAL("%s,%s,%s")), sType, sDate, sAttr);
        _tcscpy(sExplorerTabData->mStatusPane1, sSize);

        if (XPR_IS_TRUE(sVisibleStatusBar))
            setStatusBarText();
    }
}

void ExplorerView::OnExpSelMultiItem(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sCurTab = mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sCurTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return;

    xpr_bool_t sVisibleContentsWnd = isVisibleContentsWnd();
    xpr_bool_t sVisibleStatusBar   = isVisibleStatusBar();

    if (XPR_IS_FALSE(sVisibleContentsWnd) && XPR_IS_FALSE(sVisibleStatusBar))
        return;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return;

    static xpr_tchar_t sSize[XPR_MAX_PATH + 1];
    static xpr_tchar_t sText[XPR_MAX_PATH * MAX_CONTENS_SHOW_SEL_ITEMS + 1];
    sSize[0] = sText[0] = XPR_STRING_LITERAL('\0');
    xpr_uint64_t sSelFileSize = 0ui64;

    sSelFileSize = sExplorerCtrl->getSelFileSize();

    xpr_sint_t sUnit = gOpt->mMultiSelFileSizeUnit;
    switch (sUnit)
    {
    case SIZE_UNIT_DEFAULT:
        fxb::SizeFormat::getDefSizeFormat(sSelFileSize, sSize, XPR_MAX_PATH);
        break;

    case SIZE_UNIT_CUSTOM:
        fxb::SizeFormat::instance().getSizeFormat(sSelFileSize, sSize, XPR_MAX_PATH);
        break;

    case SIZE_UNIT_NONE:
        fxb::GetFormatedNumber(sSelFileSize, sSize, XPR_MAX_PATH);
        break;

    default:
        fxb::SizeFormat::getSizeUnitFormat(sSelFileSize, sUnit, sSize, XPR_MAX_PATH);
        break;
    }

    xpr_size_t sRealSelCount = 0;
    xpr_size_t sRealSelFolderCount = 0;
    xpr_size_t sRealSelFileCount = 0;

    sRealSelCount = sExplorerCtrl->getRealSelCount(XPR_FALSE, &sRealSelFolderCount, &sRealSelFileCount);

    if (XPR_IS_TRUE(sVisibleContentsWnd))
    {
        if (sRealSelCount < MAX_CONTENS_SHOW_SEL_ITEMS)
        {
            xpr_sint_t sIndex;

            POSITION sPosition = sExplorerCtrl->GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = sExplorerCtrl->GetNextSelectedItem(sPosition);

                if (sExplorerCtrl->GetItemData(sIndex) != XPR_NULL)
                    _stprintf(sText+_tcslen(sText), XPR_STRING_LITERAL("\n%s"), sExplorerCtrl->GetItemText(sIndex, 0));
            }
        }

        setContentsMultiItem(sRealSelCount, sSize, sText);
    }

    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        xpr_tchar_t sRealSelCountText[0xff] = {0};
        xpr_tchar_t sRealSelFileCountText[0xff] = {0};
        xpr_tchar_t sRealSelFolderCountText[0xff] = {0};

        fxb::GetFormatedNumber(sRealSelCount,       sRealSelCountText,       XPR_COUNT_OF(sRealSelCountText      ) - 1);
        fxb::GetFormatedNumber(sRealSelFileCount,   sRealSelFileCountText,   XPR_COUNT_OF(sRealSelFileCountText  ) - 1);
        fxb::GetFormatedNumber(sRealSelFolderCount, sRealSelFolderCountText, XPR_COUNT_OF(sRealSelFolderCountText) - 1);

        switch (gOpt->mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder"), XPR_STRING_LITERAL("%s,%s")), sRealSelCountText, sRealSelFolderCountText);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file"), XPR_STRING_LITERAL("%s,%s")), sRealSelCountText, sRealSelFileCountText);
            break;

        default:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file_folder"), XPR_STRING_LITERAL("%s,%s,%s")), sRealSelCountText, sRealSelFolderCountText, sRealSelFileCountText);
            break;
        }
    }
    else
    {
        switch (gOpt->mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder"), XPR_STRING_LITERAL("%d,%d")), sRealSelCount, sRealSelFolderCount);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file"), XPR_STRING_LITERAL("%d,%d")), sRealSelCount, sRealSelFileCount);
            break;

        default:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder_etc"), XPR_STRING_LITERAL("%d,%d,%d")), sRealSelCount, sRealSelFolderCount, sRealSelFileCount);
            break;
        }
    }

    _tcscpy(sExplorerTabData->mStatusPane1, sSize);

    if (XPR_IS_TRUE(sVisibleStatusBar))
        setStatusBarText();
}

void ExplorerView::updateStatusBar(xpr_sint_t aTab)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_sint_t sTab = aTab;
    if (sTab < 0)
        sTab = (xpr_sint_t)mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl(sTab);
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return;

    // get file, folder item count
    xpr_sint_t sCount = 0;
    xpr_sint_t sFileCount = 0, sFolderCount = 0;
    xpr_uint64_t sTotalSize = 0ui64;

    sTotalSize = sExplorerCtrl->getTotalFileSize(&sCount, &sFileCount, &sFolderCount);

    // StatusBar Pane 1 - item count
    // StatusBar Pane 2 - all file item size
    // StatusBar Pane 3 - current disk free size
    // StatusBar Pane 4 - current disk free size progress

    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        xpr_tchar_t sCountText[0xff] = {0};
        xpr_tchar_t sFileCountText[0xff] = {0};
        xpr_tchar_t sFolderCountText[0xff] = {0};

        fxb::GetFormatedNumber(sCount,       sCountText,       XPR_COUNT_OF(sCountText      ) - 1);
        fxb::GetFormatedNumber(sFileCount,   sFileCountText,   XPR_COUNT_OF(sFileCountText  ) - 1);
        fxb::GetFormatedNumber(sFolderCount, sFolderCountText, XPR_COUNT_OF(sFolderCountText) - 1);

        switch (gOpt->mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder"), XPR_STRING_LITERAL("%s,%s")), sCountText, sFolderCountText);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.file"), XPR_STRING_LITERAL("%s,%s")), sCountText, sFileCountText);
            break;

        default:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.file_folder"), XPR_STRING_LITERAL("%s,%s,%s")), sCountText, sFolderCountText, sFileCountText);
            break;
        }

        xpr_sint_t sUnit = gOpt->mMultiSelFileSizeUnit;
        switch (sUnit)
        {
        case SIZE_UNIT_DEFAULT:
            fxb::SizeFormat::getDefSizeFormat(sTotalSize, sExplorerTabData->mStatusPane1, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_CUSTOM:
            fxb::SizeFormat::instance().getSizeFormat(sTotalSize, sExplorerTabData->mStatusPane1, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_NONE:
            fxb::GetFormatedNumber(sTotalSize, sExplorerTabData->mStatusPane1, XPR_MAX_PATH);
            break;

        default:
            fxb::SizeFormat::getSizeUnitFormat(sTotalSize, sUnit, sExplorerTabData->mStatusPane1, XPR_MAX_PATH);
            break;
        }
    }
    else
    {
        switch (gOpt->mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder"), XPR_STRING_LITERAL("%d,%d")), sCount, sFolderCount);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.etc"), XPR_STRING_LITERAL("%d,%d")), sCount, sFileCount);
            break;

        default:
            _stprintf(sExplorerTabData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder_etc"), XPR_STRING_LITERAL("%d,%d,%d")), sCount, sFolderCount, sFileCount);
            break;
        }

        sExplorerTabData->mStatusPane1[0] = XPR_STRING_LITERAL('\0');
    }

    updateStatusBar();
}

void ExplorerView::updateStatusBar(void)
{
    setStatusBarDrive();
    setStatusBarText();
}

void ExplorerView::setStatusPaneBookmarkText(xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    xpr_tchar_t sText[XPR_MAX_PATH * 2 + 1] = {0};

    fxb::BookmarkItem *sBookmarkItem = fxb::BookmarkMgr::instance().getBookmark(aBookmarkIndex);
    if (XPR_IS_NOT_NULL(sBookmarkItem))
    {
        if (aDropEffect == DROPEFFECT_MOVE || aDropEffect == DROPEFFECT_COPY)
        {
            _stprintf(
                sText,
                XPR_STRING_LITERAL("%s: \'%s\'"),
                (aDropEffect == DROPEFFECT_MOVE) ? theApp.loadString(XPR_STRING_LITERAL("bookmark.status.drag_move")) : theApp.loadString(XPR_STRING_LITERAL("bookmark.status.drag_copy")),
                sBookmarkItem->mPath.c_str());
        }
        else
        {
            if (aDropEffect == DROPEFFECT_LINK)
            {
                if (fxb::IsFileSystemFolder(sBookmarkItem->mPath.c_str()) == XPR_TRUE)
                    _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("bookmark.status.drag_shortcut")), sBookmarkItem->mPath.c_str());
            }

            if (sText[0] == XPR_STRING_LITERAL('\0'))
                _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("bookmark.status.file_association")), sBookmarkItem->mPath.c_str());
        }
    }
    else
    {
        if (aInsert == -1 || aInsert >= fxb::BookmarkMgr::instance().getCount())
            _stprintf(sText, theApp.loadString(XPR_STRING_LITERAL("bookmark.status.drag_last_insert")));
        else
            _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("bookmark.status.drag_specific_insert"), XPR_STRING_LITERAL("%d")), aInsert);
    }

    mStatusBar->setPaneText(0, sText);
}

void ExplorerView::setStatusPaneDriveText(xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    xpr_tchar_t sText[XPR_MAX_PATH * 2 + 1] = {0};
    if (aDriveChar != 0)
    {
        if (aDropEffect == DROPEFFECT_MOVE || aDropEffect == DROPEFFECT_COPY || aDropEffect == DROPEFFECT_LINK)
        {
            xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
            _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), aDriveChar);

            LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sDrive);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

                switch (aDropEffect)
                {
                case DROPEFFECT_MOVE: _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("drive.status.drag_move")),     sName); break;
                case DROPEFFECT_COPY: _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("drive.status.drag_copy")),     sName); break;
                case DROPEFFECT_LINK: _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("drive.status.drag_shortcut")), sName); break;
                }
            }

            COM_FREE(sFullPidl);
        }
    }

    mStatusBar->setPaneText(0, sText);
}

void ExplorerView::setStatusPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    if (aIndex < 0 && 1 < aIndex)
        return;

    if (aIndex == 1)
    {
        mStatusBar->setDynamicPaneText(aIndex, aText, 0);
        return;
    }

    mStatusBar->setPaneText(aIndex, aText);
}

void ExplorerView::setStatusDisk(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    mStatusBar->setDiskFreeSpace(aPath);
}

void ExplorerView::setStatusBarText(void)
{
    if (XPR_IS_NULL(mTabCtrl))
        return;

    xpr_size_t sCurTab = mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sCurTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return;

    if (isVisibleStatusBar() == XPR_FALSE)
        return;

    setStatusPaneText(0, sExplorerTabData->mStatusPane0);
    setStatusPaneText(1, sExplorerTabData->mStatusPane1);
}

void ExplorerView::setStatusBarDrive(const xpr_tchar_t *aCurPath)
{
    if (isVisibleStatusBar() == XPR_FALSE)
        return;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    if (XPR_IS_NULL(aCurPath))
        aCurPath = sExplorerCtrl->getCurPath();

    setStatusDisk(aCurPath);
}

const xpr_tchar_t *ExplorerView::getStatusPaneText(xpr_sint_t aIndex) const
{
    if (XPR_IS_NULL(mTabCtrl))
        return XPR_NULL;

    xpr_size_t sCurTab = mTabCtrl->getCurTab();

    ExplorerTabData *sExplorerTabData = dynamic_cast<ExplorerTabData *>((TabData *)mTabCtrl->getTabData(sCurTab));
    if (XPR_IS_NULL(sExplorerTabData))
        return XPR_NULL;

    switch (aIndex)
    {
    case 0: return sExplorerTabData->mStatusPane0;
    case 1: return sExplorerTabData->mStatusPane1;
    }

    return XPR_NULL;
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
        LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(aDir);

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

xpr_bool_t ExplorerView::openFolder(LPITEMIDLIST aFullPidl)
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

    LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(aFullPidl);

    xpr_bool_t sResult = sExplorerCtrl->explore(sFullPidl);
    if (XPR_IS_FALSE(sResult))
    {
        COM_FREE(sFullPidl);
    }

    return sResult;
}

xpr_bool_t ExplorerView::onExplore(SearchResultPane &aSearchResultPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    return openFolder(aDir, aSelPath);
}

xpr_bool_t ExplorerView::onExplore(SearchResultPane &aSearchResultPane, LPITEMIDLIST aFullPidl)
{
    return openFolder(aFullPidl);
}

void ExplorerView::onSetFocus(SearchResultPane &aSearchResultPane)
{
    gFrame->setActiveView(mViewIndex);
}

void ExplorerView::onCreated(FileScrapPane &aFileScrapPane)
{
    gFrame->setFileScrapPane(&aFileScrapPane);
}

void ExplorerView::onDestroyed(FileScrapPane &aFileScrapPane)
{
    gFrame->setFileScrapPane(XPR_NULL);
}

void ExplorerView::onSetFocus(FileScrapPane &aFileScrapPane)
{
    gFrame->setActiveView(mViewIndex);
}

xpr_bool_t ExplorerView::onExplore(FileScrapPane &aFileScrapPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    return openFolder(aDir, aSelPath);
}

xpr_bool_t ExplorerView::onExplore(FileScrapPane &aFileScrapPane, LPITEMIDLIST aFullPidl)
{
    return openFolder(aFullPidl);
}

void ExplorerView::onStatusBarRemove(StatusBar &aStatusBar, xpr_size_t aIndex)
{
}

void ExplorerView::onStatusBarRemoved(StatusBar &aStatusBar)
{
}

void ExplorerView::onStatusBarRemoveAll(StatusBar &aStatusBar)
{
}

void ExplorerView::onStatuBarContextMenu(StatusBar &aStatusBar, xpr_size_t aIndex, const POINT &aPoint)
{
}

void ExplorerView::onStatusBarDoubleClicked(StatusBar &aStatusBar, xpr_size_t sIndex)
{
    if (sIndex == 2 || sIndex == 3)
    {
        mStatusBar->showDriveProperties();
    }
}
