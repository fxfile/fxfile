//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "MainFrame.h"

#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_bookmark.h"
#include "fxb/fxb_file_op_thread.h"
#include "fxb/fxb_file_op_undo.h"
#include "fxb/fxb_sync_dirs.h"
#include "fxb/fxb_trash.h"
#include "fxb/fxb_cmd_line_parser.h"
#include "fxb/fxb_shell_new.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_wnet_mgr.h"
#include "fxb/fxb_clip_format.h"
#include "fxb/fxb_sys_img_list.h"

#include "rgc/FileDialogST.h"
#include "rgc/SysTray.h"

#include "Option.h"
#include "FolderView.h"
#include "FolderPane.h"
#include "FolderCtrl.h"
#include "ExplorerView.h"
#include "ExplorerCtrl.h"
#include "SearchResultCtrl.h"
#include "BookmarkItemEditDlg.h"
#include "accel_table.h"
#include "FileScrapDropDlg.h"
#include "PathBar.h"
#include "AddressBar.h"
#include "BarState.h"
#include "ViewSet.h"
#include "PicViewer.h"
#include "WaitDlg.h"
#include "OptionMgr.h"
#include "CfgPath.h"
#include "PreviewViewEx.h"
#include "CtrlId.h"
#include "DlgStateMgr.h"

#include "cfg/CfgMgrDlg.h"

#include "cmd/cmd_command.h"
#include "cmd/cmd_context.h"
#include "cmd/cmd_executor.h"
#include "cmd/cmd_command_map.h"
#include "cmd/cmd_parameters.h"
#include "cmd/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#include "command_string_table.h"

#include <htmlhelp.h>           // for Html Help
#include <wininet.h>            // for URL
#include <ExDisp.h>             // for ShellUIHelper
#include <comdef.h>
#include <Imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MainFrame *gFrame;

#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

//
// user defined message
//
enum
{
    WM_TRAY_MESSAGE         = WM_USER + 1,
    WM_COMPARE_DIRS_STATUS  = WM_USER + 2,
};

xpr_uint_t WM_SINGLEINST    = ::RegisterWindowMessage(XPR_STRING_LITERAL("flyExplorer_Single_Instance"));
xpr_uint_t WM_TASKRESTARTED = ::RegisterWindowMessage(XPR_STRING_LITERAL("TaskbarCreated")); 

MainFrame::MainFrame(void)
    : mExit(XPR_FALSE)
    , mSysTray(XPR_NULL)
    , mCompareDirs(XPR_NULL)
    , mPicViewer(XPR_NULL)
    , mFileScrapDropDlg(XPR_NULL)
    , mAccelCount(0)
    , mCommandExecutor(XPR_NULL)
    , mPreviewMode(XPR_FALSE)
{
    mSplitFullPidl[0] = XPR_NULL;
    mSplitFullPidl[1] = XPR_NULL;

    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mFolderPane[i] = XPR_NULL;
    }

    mCommandExecutor = new cmd::CommandExecutor;
    cmd::CommandMap::map(*mCommandExecutor);
}

MainFrame::~MainFrame(void)
{
    COM_FREE(mSplitFullPidl[0]);
    COM_FREE(mSplitFullPidl[1]);

    XPR_SAFE_DELETE(mCommandExecutor);
}

xpr_bool_t MainFrame::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    if (super::PreCreateWindow(aCreateStruct) == XPR_FALSE)
        return XPR_FALSE;

    aCreateStruct.style &= ~FWS_ADDTOTITLE;
    aCreateStruct.dwExStyle &= ~WS_EX_CLIENTEDGE;

    CBrush sBrush(::GetSysColor(COLOR_WINDOWFRAME));
    HCURSOR sCursor = ::LoadCursor(XPR_NULL, IDC_ARROW);
    HICON sIcon = ::LoadIcon(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
    aCreateStruct.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, sCursor, sBrush, sIcon);
    DESTROY_CURSOR(sCursor);
    DESTROY_ICON(sIcon);

    xpr_sint_t sCmdShow = gOpt->mCmdShow;
    CRect sWindowRect = gOpt->mWindow;

    xpr_bool_t sDefWindow = XPR_TRUE;

    if (sWindowRect.IsRectEmpty() == XPR_FALSE)
    {
        if ((sWindowRect.right  - sWindowRect.left) > 0 && (sWindowRect.bottom - sWindowRect.top ) > 0)
            sDefWindow = XPR_FALSE;
    }

    if (XPR_IS_FALSE(sDefWindow))
    {
        CRect sWorkAreaRect;
        fxb::GetDesktopRect(sWindowRect, sWorkAreaRect, XPR_TRUE);

        if (sWorkAreaRect.PtInRect(CPoint(sWindowRect.left,  sWindowRect.top))    == XPR_FALSE &&
            sWorkAreaRect.PtInRect(CPoint(sWindowRect.right, sWindowRect.top))    == XPR_FALSE &&
            sWorkAreaRect.PtInRect(CPoint(sWindowRect.right, sWindowRect.bottom)) == XPR_FALSE &&
            sWorkAreaRect.PtInRect(CPoint(sWindowRect.left,  sWindowRect.bottom)) == XPR_FALSE)
        {
            xpr_sint_t cx = sWindowRect.Width();
            xpr_sint_t cy = sWindowRect.Height();

            if (cx > sWorkAreaRect.Width())  cx = sWorkAreaRect.Width();
            if (cy > sWorkAreaRect.Height()) cy = sWorkAreaRect.Height();

            sWindowRect.left   = (sWorkAreaRect.Width()  - cx) / 2;
            sWindowRect.top    = (sWorkAreaRect.Height() - cy) / 2;
            sWindowRect.right  = sWindowRect.left + cx;
            sWindowRect.bottom = sWindowRect.top  + cy;
        }

        // if the task bar is placed to left side or up side
        CRect sDesktopRect;
        fxb::GetDesktopRect(sWindowRect, sDesktopRect, XPR_FALSE);

        if (sWorkAreaRect.left != sDesktopRect.left ||
            sWorkAreaRect.top  != sDesktopRect.top)
        {
            xpr_sint_t xOffset = sWorkAreaRect.left - sDesktopRect.left;
            xpr_sint_t yOffset = sWorkAreaRect.top  - sDesktopRect.top;

            sWindowRect.left   += xOffset;
            sWindowRect.right  += xOffset;
            sWindowRect.top    += yOffset;
            sWindowRect.bottom += yOffset;
        }
    }

    if (XPR_IS_TRUE(sDefWindow))
    {
        CRect sWorkAreaRect;
        fxb::GetPrimaryDesktopRect(sWorkAreaRect, XPR_TRUE);

        sWindowRect.left   = (sWorkAreaRect.Width()  - DEFAULT_WINDOW_WIDTH)  / 2;
        sWindowRect.top    = (sWorkAreaRect.Height() - DEFAULT_WINDOW_HEIGHT) / 2;
        sWindowRect.right  = sWindowRect.left + DEFAULT_WINDOW_WIDTH;
        sWindowRect.bottom = sWindowRect.top  + DEFAULT_WINDOW_HEIGHT;

        sCmdShow = SW_SHOWNORMAL;
    }

    if (sCmdShow != SW_SHOWNORMAL && sCmdShow != SW_MAXIMIZE)
        sCmdShow = SW_SHOWNORMAL;

    aCreateStruct.x  = sWindowRect.left;
    aCreateStruct.y  = sWindowRect.top;
    aCreateStruct.cx = sWindowRect.Width();
    aCreateStruct.cy = sWindowRect.Height();

    theApp.m_nCmdShow = sCmdShow;

    return XPR_TRUE;
}

IMPLEMENT_DYNCREATE(MainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(MainFrame, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_SETFOCUS()
    ON_WM_SYSCOMMAND()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_INITMENUPOPUP()
    ON_WM_PARENTNOTIFY()
    ON_WM_GETMINMAXINFO()
    ON_WM_TIMER()
    ON_WM_ACTIVATEAPP()
    ON_WM_ENDSESSION()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CAPTURECHANGED()
    ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
    ON_REGISTERED_MESSAGE(WM_SINGLEINST, OnSingleInstance)
    ON_REGISTERED_MESSAGE(WM_TASKRESTARTED, OnTaskRestarted)
    ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayNotify)
    ON_MESSAGE(WM_COMPARE_DIRS_STATUS, OnCompareDirsStatus)
END_MESSAGE_MAP()

void MainFrame::init(void)
{
    gFrame = this;

    fxb::SysImgListMgr::instance().getSystemImgList();

    // Load Tray State
    if (XPR_IS_TRUE(gOpt->mTray))
        createTray();

    // Load File Scrap
    if (XPR_IS_TRUE(gOpt->mFileScrapSave))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        CfgPath::instance().getLoadPath(CfgPath::TypeFileScrap, sPath, XPR_MAX_PATH);

        fxb::FileScrap::instance().loadFromFile(sPath);
    }

    fxb::ClipFormat::instance().registerClipFormat();

    setDragContents(!gOpt->mDragNoContents);
}

xpr_sint_t MainFrame::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    init();

    // create re-bar
    CreateRebar();

    if (m_wndReBar.isWrapable(AFX_IDW_BOOKMARK_BAR) == XPR_TRUE)
    {
        RecalcLayout();
        m_wndReBar.mBookmarkToolBar.UpdateToolbarSize();
    }

    // create status bar
    if (mStatusBar.Create(this) == XPR_FALSE || mStatusBar.init() == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create status bar\n"));
        return -1;      // fail to create
    }

    // enable docking control bar
    EnableDocking(CBRS_ALIGN_ANY);

    xpr_tchar_t sTitle[0xff] = {0};

    // create search bar
    _stprintf(sTitle, XPR_STRING_LITERAL("%s"), theApp.loadString(XPR_STRING_LITERAL("bar.search.title")));
    mSearchBar.Create(sTitle, this, CSize(210,200), XPR_TRUE, AFX_IDW_SEARCH_BAR, WS_VISIBLE | WS_CHILD | CBRS_TOP);
    mSearchBar.SetBarStyle(mSearchBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    mSearchBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&mSearchBar, AFX_IDW_DOCKBAR_RIGHT);

    // load control bar state
    loadControlBarState();

    ShowControlBar(&mSearchBar, XPR_FALSE, XPR_FALSE);

    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    if (mDropTarget.Register(this) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("main_frame.msg.warning_not_supported_drag_drop_clipboard"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING);
    }

    if (XPR_IS_TRUE(gOpt->mPicViewer))
    {
        mPicViewer = new PicViewer;
        mPicViewer->Create(this);
    }

    if (XPR_IS_TRUE(gOpt->mFileScrapDrop))
    {
        mFileScrapDropDlg = new FileScrapDropDlg;
        mFileScrapDropDlg->Create(this);
        mFileScrapDropDlg->ShowWindow(SW_SHOW);
    }

    fxb::RemovableDevice &sRemovableDevice = fxb::RemovableDevice::instance();
    sRemovableDevice.create();
    sRemovableDevice.registerObserver(this);

    return 0;
}

xpr_bool_t MainFrame::LoadFrame(xpr_uint_t aIdResource, DWORD aDefaultStyle, CWnd *aParentWnd, CCreateContext *aCreateContext) 
{
    xpr_bool_t sResult = super::LoadFrame(aIdResource, aDefaultStyle, aParentWnd, aCreateContext);

    // User Defined Accelrator Table
    loadAccelTable();

    return sResult;
}

xpr_bool_t MainFrame::OnCreateClient(LPCREATESTRUCT aCreateStruct, CCreateContext *aCreateContext)
{
    xpr_sint_t sRowCount    = gOpt->mViewSplitRowCount;
    xpr_sint_t sColumnCount = gOpt->mViewSplitColumnCount;

    std::tstring sSplitArg;
    if (fxb::CmdLineParser::instance().getArg(XPR_STRING_LITERAL("W"), sSplitArg) == XPR_TRUE)
    {
        xpr_sint_t sCmdRowCount = -1, sCmdColumnCount = -1;
        _stscanf(sSplitArg.c_str(), XPR_STRING_LITERAL("%d,%d"), &sCmdRowCount, sCmdColumnCount);

        if (XPR_IS_RANGE(1, sCmdRowCount, MAX_VIEW_SPLIT_ROW))
            sRowCount = sCmdRowCount;

        if (XPR_IS_RANGE(1, sCmdColumnCount, MAX_VIEW_SPLIT_COLUMN))
            sRowCount = sCmdColumnCount;
    }

    if (!XPR_IS_RANGE(1, sRowCount, MAX_VIEW_SPLIT_ROW))
        sRowCount = 1;

    if (!XPR_IS_RANGE(1, sColumnCount, MAX_VIEW_SPLIT_COLUMN))
        sColumnCount = 1;

    // splitter
    mSplitter.setObserver(dynamic_cast<SplitterObserver *>(this));
    mSplitter.setMaxSplitCount(MAX_VIEW_SPLIT_ROW, MAX_VIEW_SPLIT_COLUMN);
    mSplitter.setPaneSizeLimit(MIN_PANE_SIZE, ksintmax);
    mSplitter.setSplitSize(SPLITTER_SIZE);

    mOneFolderSplitter.setObserver(dynamic_cast<SplitterObserver *>(this));
    mOneFolderSplitter.setMaxSplitCount(1, 2);
    mOneFolderSplitter.setPaneSizeLimit(MIN_PANE_SIZE, ksintmax);
    mOneFolderSplitter.setSplitSize(SPLITTER_SIZE);

    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode) && XPR_IS_TRUE(gOpt->mShowSingleFolderPane))
        mOneFolderSplitter.split(1, 2);
    else
        mOneFolderSplitter.split(0, 0);

    splitView(sRowCount, sColumnCount, XPR_TRUE);

    mSplitter.setActivePane(0, 0);
    mSplitter.setFocus();

    return XPR_TRUE;
}

// from SplitterObserver
CWnd *MainFrame::onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn)
{
    if (aSplitter == mOneFolderSplitter)
    {
        if ((XPR_IS_TRUE (gOpt->mLeftSingleFolderPane) && aRow == 0 && aColumn == 0) ||
            (XPR_IS_FALSE(gOpt->mLeftSingleFolderPane) && aRow == 0 && aColumn == 1))
        {
            FolderView *sFolderView = new FolderView;
            if (XPR_IS_NULL(sFolderView))
                return XPR_NULL;

            sFolderView->setObserver(dynamic_cast<FolderViewObserver *>(this));

            DWORD sStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;
            xpr_uint_t sId = AFX_IDW_FOLDER_VIEW;

            if (sFolderView->Create(XPR_NULL, XPR_NULL, sStyle, CRect(0, 0, 0, 0), this, sId, XPR_NULL) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(sFolderView);
                return XPR_NULL;
            }

            return sFolderView;
        }

        return XPR_NULL;
    }

    ExplorerView *sExplorerView = new ExplorerView;
    if (XPR_IS_NULL(sExplorerView))
        return XPR_NULL;

    xpr_sint_t sMaxRowCount = 0, sMaxColumnCount = 0;
    aSplitter.getMaxSplitCount(sMaxRowCount, sMaxColumnCount);

    xpr_sint_t sViewIndex = 0;
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    aSplitter.getPaneCount(&sRowCount, &sColumnCount);
    getViewSplitToViewIndex(sRowCount, sColumnCount, aRow, aColumn, sViewIndex);

    sExplorerView->setObserver(dynamic_cast<ExplorerViewObserver *>(this));
    sExplorerView->setViewIndex(sViewIndex);

    DWORD sStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;
    xpr_uint_t sId = AFX_IDW_EXPLORER_VIEW + aRow * sMaxColumnCount + aColumn;

    if (sExplorerView->Create(XPR_NULL, XPR_NULL, sStyle, CRect(0, 0, 0, 0), this, sId, XPR_NULL) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(sExplorerView);
        return XPR_NULL;
    }

    return sExplorerView;
}

void MainFrame::onSplitterPaneDestroy(Splitter &aSplitter, CWnd *aPaneWnd)
{
    if (XPR_IS_NOT_NULL(aPaneWnd))
    {
        CView *sView = dynamic_cast<CView *>(aPaneWnd);
        aPaneWnd->DestroyWindow(); // if pane window is CView, it will self delete.

        if (XPR_IS_NULL(sView))
        {
            XPR_SAFE_DELETE(aPaneWnd);
        }
    }
}

void MainFrame::onSplitterActivedPane(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn)
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    aSplitter.getPaneCount(&sRowCount, &sColumnCount);

    // last actived view
    xpr_sint_t sLastActivedRow = -1, sLastActivedColumn = -1;
    aSplitter.getLastActivedPane(&sLastActivedRow, &sLastActivedColumn);

    xpr_sint_t sLastActivedViewIndex = -1;
    getViewSplitToViewIndex(sRowCount, sColumnCount, sLastActivedRow, sLastActivedColumn, sLastActivedViewIndex);

    // active new view
    xpr_sint_t sViewIndex = -1;
    getViewSplitToViewIndex(sRowCount, sColumnCount, aRow, aColumn, sViewIndex);

    xpr_sint_t i;
    xpr_bool_t sActive, sLastActived;
    xpr_sint_t sViewCount;
    ExplorerView *sExplorerView;

    sViewCount = aSplitter.getPaneCount();

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NULL(sExplorerView))
            continue;

        sActive = (i == sViewIndex) ? XPR_TRUE : XPR_FALSE;
        sLastActived = (i == sLastActivedViewIndex) ? XPR_TRUE : XPR_FALSE;

        sExplorerView->setActivateBarStatus(sActive, sLastActived);
    }
}

void MainFrame::OnDestroy(void)
{
    destroy();

    super::OnDestroy();
}

// when exited windows by called ExitWindow or ExitWindowEx function
void MainFrame::OnEndSession(xpr_bool_t aEnding)
{
    saveExplicitOption();

    destroy();
}

xpr_bool_t MainFrame::confirmToClose(xpr_bool_t aForce)
{
    if (XPR_IS_FALSE(aForce))
    {
        // Animation Minimization Tray
        if (XPR_IS_TRUE(gOpt->mTray) && XPR_IS_TRUE(gOpt->mTrayOnClose) && XPR_IS_FALSE(mExit))
        {
            if (XPR_IS_NOT_NULL(mSysTray))
                mSysTray->hideToTray();

            SetForegroundWindow();

            return XPR_FALSE;
        }

        // File Operation Thread Referece Counting
        xpr_sint_t sRefCount = fxb::FileOpThread::getRefCount();
        if (sRefCount > 0)
        {
            xpr_tchar_t sMsg[0xff] = {0};
            _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("main_frame.msg.exit_file_operation_in_progress"), XPR_STRING_LITERAL("%d")), sRefCount);
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_ICONWARNING | MB_YESNO);
            if (sMsgId != IDYES)
                return XPR_FALSE;
        }

        // Confirm Exit
        if (XPR_IS_TRUE(gOpt->mConfirmExit))
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("main_frame.msg.confirm_exit"));
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO);
            if (sMsgId != IDYES)
                return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

void MainFrame::saveOption(void)
{
    // save visible state for one folder pane
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        gOpt->mShowSingleFolderPane = isVisibleFolderPane();
    }

    // save file scrap
    if (XPR_IS_TRUE(gOpt->mFileScrapSave))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        CfgPath::instance().getSavePath(CfgPath::TypeFileScrap, sPath, XPR_MAX_PATH);

        fxb::FileScrap::instance().saveToFile(sPath);
    }

    // verify viewSet
    if (XPR_IS_TRUE(gOpt->mExplorerExitVerifyViewSet))
    {
        ViewSet sViewSet;
        sViewSet.verify();
    }

    DlgStateMgr::instance().save();

    saveControlBarState();

    gOpt->mPicViewer = XPR_IS_NOT_NULL(mPicViewer) && XPR_IS_NOT_NULL(mPicViewer->m_hWnd);
    gOpt->mFileScrapDrop = XPR_IS_NOT_NULL(mFileScrapDropDlg) && XPR_IS_NOT_NULL(mFileScrapDropDlg->m_hWnd);

    // save view split state
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    gOpt->mViewSplitRowCount    = sRowCount;
    gOpt->mViewSplitColumnCount = sColumnCount;

    // Save MainFrame Position
    WINDOWPLACEMENT sWindowPlacement = {0};
    GetWindowPlacement(&sWindowPlacement);

    gOpt->mCmdShow = sWindowPlacement.showCmd;
    gOpt->mWindow = sWindowPlacement.rcNormalPosition;
}

void MainFrame::saveExplicitOption(void)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    ExplorerView *sExplorerView;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NOT_NULL(sExplorerView))
            sExplorerView->saveOption();
    }

    saveOption();

    OptionMgr &sOptionMgr = OptionMgr::instance();

    sOptionMgr.saveOption();
    sOptionMgr.saveMainOption();

    if (CfgPath::isInstance() == XPR_TRUE)
        CfgPath::instance().save();
}

void MainFrame::destroy(void)
{
    destroyTray();

    //m_wndCoolBar.mMenuBar.DestroyMenuData();

    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();

    for (i = 0; i < sViewCount; ++i)
    {
        if (XPR_IS_NOT_NULL(mFolderPane[i]))
            mFolderPane[i]->DestroyWindow(); // self delete
    }

    DESTROY_WINDOW(mSearchBar);

    DESTROY_DELETE(mFileScrapDropDlg);
    DESTROY_DELETE(mPicViewer);

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mDrivePathMap[i].clear();
    }

    m_wndReBar.DestroyWindow();

    fxb::RemovableDevice::instance().destroy();

    if (XPR_IS_NOT_NULL(mCompareDirs))
    {
        mCompareDirs->stop();
        XPR_SAFE_DELETE_ARRAY(mCompareDirs);
    }

    WaitDlg::instance().end();
}

xpr_bool_t MainFrame::DestroyWindow(void)
{

    return super::DestroyWindow();
}

void MainFrame::OnPaint(void)
{
    CPaintDC sDC(this);

    if (isPreviewMode() == XPR_FALSE)
    {
        xpr_sint_t i;
        xpr_sint_t sSplitterCount;
        CRect sSplitterRect;

        sSplitterCount = mOneFolderSplitter.getSplitterCount();
        if (sSplitterCount > 0)
        {
            for (i = 0; i < sSplitterCount; ++i)
            {
                mOneFolderSplitter.getSplitterRect(i, sSplitterRect);

                sDC.FillSolidRect(&sSplitterRect, ::GetSysColor(COLOR_BTNFACE));
            }
        }

        sSplitterCount = mSplitter.getSplitterCount();
        if (sSplitterCount > 0)
        {
            xpr_sint_t i;
            CRect sSplitterRect;

            for (i = 0; i < sSplitterCount; ++i)
            {
                mSplitter.getSplitterRect(i, sSplitterRect);

                sDC.FillSolidRect(&sSplitterRect, ::GetSysColor(COLOR_BTNFACE));
            }
        }
    }
}

xpr_bool_t MainFrame::OnEraseBkgnd(CDC *aDC)
{
    return super::OnEraseBkgnd(aDC);
}

void MainFrame::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout(aType);
}

// reference: CWnd::RepositionBars(...)
void MainFrame::getClientRectExecptForControlBars(CRect &aRect)
{
    AFX_SIZEPARENTPARAMS sLayout;
    sLayout.hDWP      = XPR_NULL;
    sLayout.rect      = aRect;
    sLayout.sizeTotal = CSize(0, 0);
    sLayout.bStretch  = XPR_TRUE;

    xpr_uint_t sIdFirst = 0;
    xpr_uint_t sIdLast = 0xffff;

    HWND sChildHwnd;

    for (sChildHwnd = ::GetTopWindow(m_hWnd); sChildHwnd != XPR_NULL; sChildHwnd = ::GetNextWindow(sChildHwnd, GW_HWNDNEXT))
    {
        xpr_uint_t sId = ::GetDlgCtrlID(sChildHwnd);
        CWnd *sWnd = CWnd::FromHandlePermanent(sChildHwnd);

        if (XPR_IS_OUT_OF_RANGE(AFX_IDW_PANE_FIRST, sId, AFX_IDW_PANE_LAST) && XPR_IS_RANGE(sIdFirst, sId, sIdLast) && sWnd != XPR_NULL)
        {
            ::SendMessage(sChildHwnd, WM_SIZEPARENT, 0, (LPARAM)&sLayout);
        }
    }

    *aRect = sLayout.rect;
}

void MainFrame::recalcLayout(xpr_uint_t aType)
{
    if (XPR_IS_NULL(gOpt) || mSplitter.getPaneWnd(0, 0) == XPR_NULL)
        return;

    if (isPreviewMode() == XPR_TRUE)
        return;

    CRect sClientRect;
    GetClientRect(&sClientRect);

    if (sClientRect.Width() <= 0 || sClientRect.Height() <= 0)
        return;

    getClientRectExecptForControlBars(sClientRect);

    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    HDWP sHdwp = ::BeginDeferWindowPos(sRowCount * sColumnCount + 1);

    // adjust single folder splitter size
    CRect sOneFolderSplitterRect(sClientRect);
    mOneFolderSplitter.setWindowRect(sOneFolderSplitterRect);

    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        xpr_sint_t sPaneSize = gOpt->mSingleFolderPaneSize;
        if (XPR_IS_FALSE(gOpt->mLeftSingleFolderPane))
        {
            xpr_sint_t sSplitSize = mOneFolderSplitter.getSplitSize();
            sPaneSize = sOneFolderSplitterRect.Width() - sPaneSize - sSplitSize;
        }

        mOneFolderSplitter.moveColumn(0, sPaneSize);
    }

    mOneFolderSplitter.resize(sHdwp);

    // adjust view splitter size
    CRect sSplitterRect(sClientRect);

    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        xpr_sint_t sColumn = 1;
        if (XPR_IS_FALSE(gOpt->mLeftSingleFolderPane))
            sColumn = 0;

        mOneFolderSplitter.getPaneRect(0, sColumn, sSplitterRect);
    }

    mSplitter.setWindowRect(sSplitterRect);

    CRect sRect(sSplitterRect);
    xpr_sint_t sWidth  = sRect.Width();
    xpr_sint_t sHeight = sRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    if ((sRowCount <= 0 || sColumnCount <= 0) || (sRowCount == 1 && sColumnCount == 1))
    {
        mSplitter.moveColumn(0, sWidth);
        mSplitter.moveRow   (0, sHeight);
    }
    else if (XPR_IS_TRUE(gOpt->mViewSplitByRatio) && (sRowCount >= 1 || sColumnCount >= 1))
    {
        if (sRowCount == 1 && sColumnCount == 2)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mViewSplitRatio[0] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 1)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[0] + 0.5);

            mSplitter.moveRow(0, gOpt->mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 2)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[1]);
        }
        else if (sRowCount == 1 && sColumnCount == 3)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
        }
        else if (sRowCount == 2 && sColumnCount == 3)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);
            gOpt->mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[2] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
        }
    }
    else if (sRowCount >= 1 || sColumnCount >= 1)
    {
        if (sRowCount == 1 && sColumnCount == 2)
        {
            gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 1)
        {
            gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sHeight;

            mSplitter.moveRow(0, gOpt->mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 2)
        {
            gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mViewSplitRatio[1] = (xpr_double_t)gOpt->mViewSplitSize[1] / (xpr_double_t)sHeight;

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[1]);
        }
        else if (sRowCount == 1 && sColumnCount == 3)
        {
            gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mViewSplitRatio[1] = (xpr_double_t)gOpt->mViewSplitSize[1] / (xpr_double_t)sWidth;

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
        }
        else if (sRowCount == 2 && sColumnCount == 3)
        {
            gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mViewSplitRatio[1] = (xpr_double_t)gOpt->mViewSplitSize[1] / (xpr_double_t)sWidth;
            gOpt->mViewSplitRatio[2] = (xpr_double_t)gOpt->mViewSplitSize[2] / (xpr_double_t)sHeight;

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
        }
    }

    mSplitter.resize(sHdwp);

    ::EndDeferWindowPos(sHdwp);
}

void MainFrame::OnClose(void)
{
    // [bug patched] 2010/06/14
    // When program exit on print preview mode, barstate.ini file is broken.
    // It restore previous mode for control bar state.
    CView *sView = GetActiveView();
    PreviewViewEx *sPreviewViewEx = dynamic_cast<PreviewViewEx *>(sView);
    if (XPR_IS_NOT_NULL(sPreviewViewEx))
    {
        sPreviewViewEx->Close();
    }

    if (confirmToClose() == XPR_TRUE)
    {
        saveOption();

        super::OnClose();
    }
}

void MainFrame::exitTrayApp(void)
{
    mExit = XPR_TRUE;
    OnClose();
}

void MainFrame::OnSetFocus(CWnd *aOldWnd)
{
    mSplitter.setFocus();

    super::OnSetFocus(aOldWnd);
}

void MainFrame::OnSysCommand(xpr_uint_t aId, LPARAM lParam)
{
    if (XPR_IS_TRUE(gOpt->mTray) && XPR_IS_TRUE(gOpt->mTrayOnClose) && aId == SC_CLOSE)
    {
        if (XPR_IS_NOT_NULL(mSysTray))
            mSysTray->hideToTray();

        SetForegroundWindow();
    }
    else if (XPR_IS_TRUE(gOpt->mTrayOnMinmize) && aId == SC_MINIMIZE)
    {
        minimizeToTray();
    }
    else
    {
        super::OnSysCommand(aId, lParam);
    }
}

void MainFrame::OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct)
{
    xpr_bool_t sSetFlag = XPR_FALSE;

    if (aMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(xpr_uintptr_t)aMeasureItemStruct->itemID) == XPR_TRUE)
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(xpr_uintptr_t)aMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu))
            {
                mMenu.MeasureItem(aMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sSetFlag))
        super::OnMeasureItem(aIdCtl, aMeasureItemStruct);
}

LRESULT MainFrame::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu) 
{
    LRESULT sLResult;

    if (BCMenu::IsMenu(aMenu) == XPR_TRUE)
        sLResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sLResult = super::OnMenuChar(aChar, aFlags, aMenu);

    return sLResult;
}

void MainFrame::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    super::OnInitMenuPopup(aPopupMenu, aIndex, aSysMenu);

    if (aSysMenu == XPR_TRUE) return;
    if (aPopupMenu->m_hMenu == XPR_NULL) return;

    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (XPR_IS_NOT_NULL(sBCPopupMenu))
    {
        xpr_sint_t sLen;
        static xpr_tchar_t sHotKey[0xff];

        xpr_uint_t sId;
        xpr_sint_t sCount = aPopupMenu->GetMenuItemCount();

        xpr_sint_t sBookmarkInsert = -1;
        xpr_sint_t sDriveInsert = -1;
        xpr_sint_t sGoUpInsert = -1;
        xpr_sint_t sGoBackwardInsert = -1;
        xpr_sint_t sGoForwardInsert = -1;
        xpr_sint_t sGoHistoryInsert = -1;
        xpr_sint_t sGoWorkingFolderInsert = -1;
        xpr_sint_t sShellNewInsert = -1;
        xpr_sint_t sRecentInsert = -1;

        typedef std::deque<xpr_uint_t> CmdIdDeque;
        CmdIdDeque sDelCmdIdDeque;

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();
        CString sMenuText;

        xpr_sint_t i, j;
        for (i = 0; i < sCount; ++i)
        {
            sHotKey[0] = 0;

            sId = sBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if sId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if sId(xpr_uint_t) is -1, it's sub-menu.
                    sBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

                        sBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }

            // set keyboard shortcut
            for (j = 0; j < mAccelCount; ++j)
            {
                if (mAccel[j].cmd == sId)
                {
                    if (mAccel[j].fVirt & FCONTROL) _tcscat(sHotKey, XPR_STRING_LITERAL("Ctrl+"));
                    if (mAccel[j].fVirt & FSHIFT)   _tcscat(sHotKey, XPR_STRING_LITERAL("Shift+"));
                    if (mAccel[j].fVirt & FALT)     _tcscat(sHotKey, XPR_STRING_LITERAL("Alt+"));

                    if (mAccel[j].fVirt & FVIRTKEY)
                        _tcscat(sHotKey, fxb::GetKeyName(mAccel[j].key));
                    else
                    {
                        sLen = (xpr_sint_t)_tcslen(sHotKey);
                        sHotKey[sLen + 0] = (xpr_tchar_t)mAccel[j].key;
                        sHotKey[sLen + 1] = (xpr_tchar_t)XPR_STRING_LITERAL('\0');
                    }

                    break;
                }
            }

            sBCPopupMenu->SetMenuAccelKey(i, sHotKey);

            if (sId == ID_EDIT_UNDO)
            {
                fxb::FileOpUndo sFileOpUndo;
                xpr_sint_t sUndoMode = sFileOpUndo.getMode();

                const xpr_tchar_t *sText = XPR_NULL;
                switch (sUndoMode)
                {
                case fxb::MODE_COPY:   sText = theApp.loadString(XPR_STRING_LITERAL("cmd.undo.copy"));   break;
                case fxb::MODE_MOVE:   sText = theApp.loadString(XPR_STRING_LITERAL("cmd.undo.move"));   break;
                case fxb::MODE_RENAME: sText = theApp.loadString(XPR_STRING_LITERAL("cmd.undo.rename")); break;
                case fxb::MODE_TRASH:  sText = theApp.loadString(XPR_STRING_LITERAL("cmd.undo.trash"));  break;
                default:               sText = theApp.loadString(XPR_STRING_LITERAL("cmd.undo.none"));   break;
                }

                if (sText != XPR_NULL)
                    sBCPopupMenu->SetMenuText(ID_EDIT_UNDO, (xpr_tchar_t *)sText, MF_BYCOMMAND);
            }

            if (XPR_IS_RANGE(ID_DRIVE_FIRST, sId, ID_DRIVE_LAST))
            {
                if (sDriveInsert == -1)
                    sDriveInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_BOOKMARK_FIRST, sId, ID_BOOKMARK_LAST))
            {
                if (sBookmarkInsert == -1)
                    sBookmarkInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_GO_UP_FIRST, sId, ID_GO_UP_LAST))
            {
                if (sGoUpInsert == -1)
                    sGoUpInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_GO_BACKWARD_FIRST, sId, ID_GO_BACKWARD_LAST))
            {
                if (sGoBackwardInsert == -1)
                    sGoBackwardInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_GO_FORWARD_FIRST, sId, ID_GO_FORWARD_LAST))
            {
                if (sGoForwardInsert == -1)
                    sGoForwardInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_GO_HISTORY_FIRST, sId, ID_GO_HISTORY_LAST))
            {
                if (sGoHistoryInsert == -1)
                    sGoHistoryInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, sId, ID_GO_WORKING_FOLDER_LAST))
            {
                if (sGoWorkingFolderInsert == -1)
                    sGoWorkingFolderInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_FILE_NEW_FIRST, sId, ID_FILE_NEW_LAST))
            {
                if (sShellNewInsert == -1)
                    sShellNewInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (XPR_IS_RANGE(ID_FILE_RECENT_FIRST, sId, ID_FILE_RECENT_LAST))
            {
                if (sRecentInsert == -1)
                    sRecentInsert = i;

                sDelCmdIdDeque.push_back(sId);
            }

            if (sDriveInsert == -1)
            {
                if (sId == ID_DRIVE_DYNAMIC_MENU)
                {
                    sDriveInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sBookmarkInsert == -1)
            {
                if (sId == ID_BOOKMARK_DYNAMIC_MENU)
                {
                    sBookmarkInsert = i;
                    if (fxb::BookmarkMgr::instance().getCount() > 0)
                        sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sGoUpInsert == -1)
            {
                if (sId == ID_GO_UP_DYNAMIC_MENU)
                {
                    sGoUpInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sGoBackwardInsert == -1)
            {
                if (sId == ID_GO_BACKWARD_DYNAMIC_MENU)
                {
                    sGoBackwardInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sGoForwardInsert == -1)
            {
                if (sId == ID_GO_FORWARD_DYNAMIC_MENU)
                {
                    sGoForwardInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sGoHistoryInsert == -1)
            {
                if (sId == ID_GO_HISTORY_DYNAMIC_MENU)
                {
                    sGoHistoryInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sGoWorkingFolderInsert == -1)
            {
                if (sId == ID_GO_WORKING_FOLDER_DYNAMIC_MENU)
                {
                    sGoWorkingFolderInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sShellNewInsert == -1)
            {
                if (sId == ID_FILE_NEW_DYNAMIC_MENU)
                {
                    sShellNewInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }

            if (sRecentInsert == -1)
            {
                if (sId == ID_FILE_RECENT_DYNAMIC_MENU)
                {
                    sRecentInsert = i;
                    sDelCmdIdDeque.push_back(sId);
                }
            }
        }

        CmdIdDeque::iterator sIterator = sDelCmdIdDeque.begin();
        for (; sIterator != sDelCmdIdDeque.end(); ++sIterator)
        {
            sId = *sIterator;
            aPopupMenu->DeleteMenu(sId, MF_BYCOMMAND);
        }

        sDelCmdIdDeque.clear();

        //xpr_sint_t sToolButtonCount = 0;
        //TBBUTTONEX *ptbe = m_wndCoolBar.mMainToolBar.GetButton(&sToolButtonCount);
        //COLORREF clrMask = m_wndCoolBar.mMainToolBar.GetColorMask();

        //HIMAGELIST sImageList;
        //sImageList = ImageList_LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_TB_MAIN_HOT_SMALL),
        //    16, 0, clrMask, IMAGE_BITMAP, LR_CREATEDIBSECTION);

        //CImageList sImgList;
        //CImageList *sImgListRef;
        //sImgList.Attach(sImageList);

        //for (i = 0; i < sCount; ++i)
        //{
        //    sId = sBCPopupMenu->GetMenuItemID(i);
        //    if (sId == 0) // seperator
        //        continue;

        //    xpr_sint_t nOffset = -1;
        //    for (j = 0; j < sToolButtonCount; ++j)
        //    {
        //        if (sId == ptbe[j].tbinfo.idCommand)
        //        {
        //            nOffset = ptbe[j].tbinfo.iBitmap;
        //            break;
        //        }
        //    }

        //    sImgListRef = &sImgList;
        //    if (nOffset < 0)
        //        sImgListRef = XPR_NULL;

        //    sBCPopupMenu->ModifyODMenu(XPR_NULL, sId, sImgListRef, nOffset);
        //}

        if (sDriveInsert           >= 0) { insertDrivePopupMenu          (sBCPopupMenu, sDriveInsert          ); }
        if (sBookmarkInsert        >= 0) { insertBookmarkPopupMenu       (sBCPopupMenu, sBookmarkInsert       ); }
        if (sGoUpInsert            >= 0) { insertGoUpPopupMenu           (sBCPopupMenu, sGoUpInsert           ); }
        if (sGoBackwardInsert      >= 0) { insertGoBackwardPopupMenu     (sBCPopupMenu, sGoBackwardInsert     ); }
        if (sGoForwardInsert       >= 0) { insertGoForwardPopupMenu      (sBCPopupMenu, sGoForwardInsert      ); }
        if (sGoHistoryInsert       >= 0) { insertGoHistoryPopupMenu      (sBCPopupMenu, sGoHistoryInsert      ); }
        if (sGoWorkingFolderInsert >= 0) { insertGoWorkingFolderPopupMenu(sBCPopupMenu, sGoWorkingFolderInsert); }

        if (XPR_IS_TRUE(gOpt->mShellNewMenu) && sShellNewInsert >= 0)
        {
            insertShellNewPopupMenu(sBCPopupMenu, sShellNewInsert);
        }

        if (sRecentInsert >= 0)
        {
            theApp.updateRecentMenu(sBCPopupMenu, sRecentInsert);
        }

        // remove last seperator
        sCount = aPopupMenu->GetMenuItemCount();
        if (sCount > 0)
        {
            sId = aPopupMenu->GetMenuItemID(sCount-1);
            if (sId == 0) // seperator
                aPopupMenu->DeleteMenu(sCount-1, MF_BYPOSITION);
        }
    }

    //xpr_sint_t sMainIndex = m_wndCoolBar.m_wndMenuBar.GetCurIndex();
    //if (sMainIndex == 0 && aIndex == 8)
    //{
    //    while (aPopupMenu->DeleteMenu(0, MF_BYPOSITION)) ;

    //    FolderCtrl *sFolderCtrl = getFolderCtrl();
    //    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sFolderCtrl->GetItemData(sFolderCtrl->GetSelectedItem());

    //    if (XPR_IS_NULL(mShContextMenu))
    //        mShContextMenu = new ContextMenu(sFolderCtrl->m_hWnd);
    //    mShContextMenu->Init(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1);
    //    mShContextMenu->GetMenu(aPopupMenu, ID_CTTM);

    //    mShContextMenuHandle = aPopupMenu->m_hMenu;
    //}

    //if (BCMenu::IsMenu(aPopupMenu))
    //    BCMenu::UpdateMenu(aPopupMenu);
}

xpr_sint_t MainFrame::insertDrivePopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return -1;

    xpr_uint_t sId;
    xpr_tchar_t *sDrive;
    xpr_tchar_t sDriveStrings[XPR_MAX_PATH + 1] = {0};
    fxb::GetDriveStrings(sDriveStrings);

    LPITEMIDLIST sFullPidl;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    xpr_tchar_t sTemp[XPR_MAX_PATH + 1];
    xpr_tchar_t *sColon = XPR_NULL;
    SHFILEINFO sShFileInfo = {0};
    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    xpr_sint_t i, sCount = 0;
    xpr_sint_t nPos = 0;
    for (i = 0; ; ++i)
    {
        if (sDriveStrings[nPos] == 0)
            break;

        sDrive = sDriveStrings + nPos;
        sId = ID_DRIVE_FIRST + (*sDrive - XPR_STRING_LITERAL('A'));

        sFullPidl = fxb::IL_CreateFromPath(sDrive);
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        ::SHGetFileInfo((const xpr_tchar_t *)sFullPidl, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX);
        COM_FREE(sFullPidl);

        sColon = _tcsrchr(sText, XPR_STRING_LITERAL(':'));
        if (XPR_IS_NOT_NULL(sColon))
        {
            --sColon;
            _tcscpy(sTemp, sColon);
            *sColon++ = XPR_STRING_LITERAL('&');
            _tcscpy(sColon, sTemp);
        }

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, sId, sText, &sSysImgListMgr.mSysImgList16, sShFileInfo.iIcon);

        nPos += (xpr_sint_t)_tcslen(sDriveStrings)+1;
        sCount++;
    }

    return sCount;
}

xpr_sint_t MainFrame::insertBookmarkPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    WORD sVKey;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    xpr_tchar_t sShortKey[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;

    xpr_sint_t sIconIndex;
    HICON sIcon;
    CImageList sImgList;
    CImageList *sImgListRef;
    sImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);

    fxb::BookmarkMgr &sBookmarkMgr = fxb::BookmarkMgr::instance();

    xpr_sint_t i, sCount;
    fxb::BookmarkItem *sBookmarkItem;
    fxb::BookmarkMgr::Result sResult;

    sCount = sBookmarkMgr.getCount();
    if (sCount <= 0)
        return 0;

    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = sBookmarkMgr.getBookmark(i);

        if (sBookmarkItem->isSeparator())
        {
            aPopupMenu->InsertMenu(aInsert + i, MF_SEPARATOR | MF_BYPOSITION, 0, XPR_STRING_LITERAL(""));
        }
        else
        {
            sImgListRef = XPR_NULL;
            sIconIndex = -1;

            sIcon = XPR_NULL;
            sResult = sBookmarkMgr.getIcon(i, sIcon, XPR_TRUE);
            if (XPR_IS_NOT_NULL(sIcon))
                sIconIndex = sImgList.Add(sIcon);

            if (sIconIndex >= 0)
                sImgListRef = &sImgList;

            sShortKey[0] = XPR_STRING_LITERAL('\0');
            sVKey = LOWORD(sBookmarkItem->mHotKey);
            if (XPR_STRING_LITERAL('0') <= sVKey && sVKey <= XPR_STRING_LITERAL('9'))
                _stprintf(sShortKey, XPR_STRING_LITERAL("\tCtrl+&%c"), sVKey);
            _stprintf(sText, XPR_STRING_LITERAL("%s%s"), sBookmarkItem->mName.c_str(), sShortKey);

            aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_BOOKMARK_FIRST+i, sText, sImgListRef, sIconIndex);
        }
    }

    sImgList.DeleteImageList();

    return sCount;
}

xpr_sint_t MainFrame::insertGoUpPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return 0;

    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return 0;

    xpr_sint_t i, sCount, sIconIndex;
    xpr_bool_t sRemoved;
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(sTvItemData->mFullPidl);

    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    LPITEMIDLIST sFullPidl2 = sFullPidl;
    for (i = 0; ; ++i)
    {
        if (sFullPidl2->mkid.cb == 0)
            break;

        sFullPidl2 = (LPITEMIDLIST)((LPBYTE)sFullPidl2 + sFullPidl2->mkid.cb);
    }

    sCount = i;
    for (i = sCount - 1; i >= 0; --i)
    {
        sRemoved = fxb::IL_RemoveLastID(sFullPidl);
        if (XPR_IS_FALSE(sRemoved))
            break;

        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);

        sIconIndex = fxb::GetItemIconIndex(sFullPidl, XPR_FALSE);
        aPopupMenu->InsertMenu(aInsert, MF_STRING | MF_BYPOSITION, ID_GO_UP_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);
    }

    COM_FREE(sFullPidl);

    return sCount;
}

xpr_sint_t MainFrame::insertGoBackwardPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return 0;

    fxb::HistoryDeque *sBackwardDeque = sExplorerCtrl->getBackwardList();
    if (XPR_IS_NULL(sBackwardDeque))
        return 0;

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    fxb::HistoryDeque::reverse_iterator sReverseIterator;

    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    sCount = (xpr_sint_t)sBackwardDeque->size();
    if (sCount > gOpt->mBackwardMenuCount)
        sCount = gOpt->mBackwardMenuCount;

    sReverseIterator = sBackwardDeque->rbegin();
    for (i = 0; i < sCount; ++i)
    {
        if (sReverseIterator == sBackwardDeque->rend())
            break;

        sFullPidl = *sReverseIterator;

        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = fxb::GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_BACKWARD_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);

        sReverseIterator++;
    }

    return sCount;
}

xpr_sint_t MainFrame::insertGoForwardPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return 0;

    fxb::HistoryDeque *sForwardDeque = sExplorerCtrl->getForwardList();
    if (XPR_IS_NULL(sForwardDeque))
        return 0;

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    fxb::HistoryDeque::reverse_iterator sReverseIterator;

    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    sCount = (xpr_sint_t)sForwardDeque->size();
    if (sCount > gOpt->mBackwardMenuCount)
        sCount = gOpt->mBackwardMenuCount;

    sReverseIterator = sForwardDeque->rbegin();
    for (i = 0; i < sCount; ++i)
    {
        if (sReverseIterator == sForwardDeque->rend())
            break;

        sFullPidl = *sReverseIterator;

        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = fxb::GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_FORWARD_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);

        sReverseIterator++;
    }

    return sCount;
}

xpr_sint_t MainFrame::insertGoHistoryPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return 0;

    fxb::HistoryDeque *sHistoryDeque = sExplorerCtrl->getHistoryList();
    if (XPR_IS_NULL(sHistoryDeque))
        return 0;

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    fxb::HistoryDeque::reverse_iterator sReverseIterator;

    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    sCount = (xpr_sint_t)sHistoryDeque->size();
    if (sCount > gOpt->mHistoryMenuCount)
        sCount = gOpt->mHistoryMenuCount;

    sReverseIterator = sHistoryDeque->rbegin();
    for (i = 0; i < sCount; ++i)
    {
        if (sReverseIterator == sHistoryDeque->rend())
            break;

        sFullPidl = *sReverseIterator;

        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = fxb::GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_HISTORY_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);

        sReverseIterator++;
    }

    return sCount;
}

xpr_sint_t MainFrame::insertGoWorkingFolderPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[0xff] = {0};

    HIMAGELIST sImageList = ImageList_LoadImage(
        AfxGetResourceHandle(),
        MAKEINTRESOURCE(IDB_TB_MAIN_HOT_SMALL),
        16,
        0,
        RGB(255,0,255),
        IMAGE_BITMAP,
        LR_CREATEDIBSECTION);

    CImageList sToolBarImgList;
    sToolBarImgList.Attach(sImageList);

    sCount = MAX_WORKING_FOLDER;
    for (i = 0; i < sCount; ++i)
    {
        _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.go"), XPR_STRING_LITERAL("%d,%d")), i+1, i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_FIRST+i, sText, &sToolBarImgList, 44+i);
    }

    aPopupMenu->InsertMenu(aInsert++, MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);

    for (i = 0; i < sCount; ++i)
    {
        _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.set"), XPR_STRING_LITERAL("%d")), i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_SET_FIRST+i, sText);
    }

    aPopupMenu->InsertMenu(aInsert++, MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);

    for (i = 0; i < sCount; ++i)
    {
        _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.reset"), XPR_STRING_LITERAL("%d")), i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_RESET_FIRST+i, sText);
    }

    return sCount;
}

LRESULT MainFrame::WindowProc(xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam) 
{
    //if (aMsg == WM_UNINITMENUPOPUP)
    //{
    //    CMenu *sPopupMenu = CMenu::FromHandle((HMENU)wParam);
    //    XPR_TRACE(XPR_STRING_LITERAL("\nWM_UNINITMENUPOPUP"));

    //    if (mShContextMenuHandle == sPopupMenu->m_hMenu)
    //    {
    //        mShContextMenu->DestroySubclass();
    //        mShContextMenuHandle = XPR_NULL;
    //    }
    //}

    return super::WindowProc(aMsg, wParam, lParam);
}

xpr_bool_t MainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    //if (ID_CTTM <= wParam && wParam <= ID_CTTM+0x7FFF)
    //{
    //    xpr_uint_t uID = (xpr_uint_t)wParam - ID_CTTM;

    //    SHCM_InvokeCommand(uID);
    //    SHCM_Release();

    //    mShContextMenu->InvokeCommand(uID);
    //    delete mShContextMenu;
    //    mShContextMenu = XPR_NULL;
    //}

    return super::OnCommand(wParam, lParam);
}

AddressBar* MainFrame::getAddressBar(xpr_sint_t aIndex) const
{
    ExplorerView *sExplorerView = getExplorerView(aIndex);
    return XPR_IS_NOT_NULL(sExplorerView) ? sExplorerView->getAddressBar() : XPR_NULL;
}

FolderCtrl* MainFrame::getFolderCtrl(xpr_sint_t aIndex) const
{
    FolderPane *sFolderPane = getFolderPane(aIndex);
    return XPR_IS_NOT_NULL(sFolderPane) ? sFolderPane->getFolderCtrl() : XPR_NULL;
}

ExplorerCtrl* MainFrame::getExplorerCtrl(xpr_sint_t aIndex) const
{
    ExplorerView *sExplorerView = getExplorerView(aIndex);
    return XPR_IS_NOT_NULL(sExplorerView) ? sExplorerView->getExplorerCtrl() : XPR_NULL;
}

ExplorerCtrl *MainFrame::getExplorerCtrl(FolderCtrl *aFolderCtrl) const
{
    if (aFolderCtrl == XPR_NULL)
        return XPR_NULL;

    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    FolderCtrl *sFolderCtrl;

    for (i = 0; i < sViewCount; ++i)
    {
        sFolderCtrl = getFolderCtrl(i);
        if (sFolderCtrl == aFolderCtrl)
        {
            return getExplorerCtrl(i);
        }
    }

    return XPR_NULL;
}

SearchResultCtrl *MainFrame::getSearchResultCtrl(xpr_sint_t aIndex) const
{
    ExplorerView *sExplorerView = getExplorerView(aIndex);
    return XPR_IS_NOT_NULL(sExplorerView) ? sExplorerView->getSearchResultCtrl() : XPR_NULL;
}

FolderView *MainFrame::getFolderView(void) const
{
    xpr_sint_t sRow = 0, sColumn = 0;
    if (XPR_IS_FALSE(gOpt->mLeftSingleFolderPane))
        sColumn = 1;

    return dynamic_cast<FolderView *>(mOneFolderSplitter.getPaneWnd(sRow, sColumn));
}

Splitter *MainFrame::getSplitter(void) const
{
    return const_cast<Splitter *>(&mSplitter);
}

FolderPane *MainFrame::getFolderPane(xpr_sint_t aIndex) const
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        FolderView *sFolderView = getFolderView();
        if (XPR_IS_NOT_NULL(sFolderView))
            return sFolderView->getFolderPane();

        return XPR_NULL;
    }

    ExplorerView *sExplorerView = getExplorerView(aIndex);
    if (XPR_IS_NULL(sExplorerView))
        return XPR_NULL;

    return sExplorerView->getFolderPane();
}

ExplorerView *MainFrame::getExplorerView(xpr_sint_t aIndex) const
{
    ExplorerView *sExplorerView = XPR_NULL;

    if (aIndex == -1) // current focused explorer view
    {
        xpr_sint_t sRow = -1, sColumn = -1;
        mSplitter.getActivePane(&sRow, &sColumn);

        if (sRow != -1 && sColumn != -1)
        {
            CWnd *sWnd = mSplitter.getPaneWnd(sRow, sColumn);
            sExplorerView = DYNAMIC_DOWNCAST(ExplorerView, sWnd);
        }
    }
    else if (aIndex == -2) // other focused explorer view
    {
        xpr_sint_t sRow, sColumn;
        mSplitter.getLastActivedPane(&sRow, &sColumn);

        if (sRow != -1 && sColumn != -1)
        {
            CWnd *sWnd = mSplitter.getPaneWnd(sRow, sColumn);
            sExplorerView = DYNAMIC_DOWNCAST(ExplorerView, sWnd);
        }
    }
    else // explicitly explorer view
    {
        xpr_sint_t sRowCount = 0, sColumnCount = 0;
        mSplitter.getPaneCount(&sRowCount, &sColumnCount);

        xpr_sint_t sRow = 0, sColumn = 0;
        getViewIndexToViewSplit(aIndex, sRowCount, sColumnCount, sRow, sColumn);

        CWnd *sWnd = mSplitter.getPaneWnd(sRow, sColumn);
        sExplorerView = DYNAMIC_DOWNCAST(ExplorerView, sWnd);
    }

    return sExplorerView;
}

SysTray *MainFrame::createTray(void)
{
    if (XPR_IS_NOT_NULL(mSysTray))
        return mSysTray;

    xpr_uint_t sIconId = (fxb::UserEnv::instance().mWinVer <= fxb::UserEnv::Win2000) ? IDI_TRAY : IDI_TRAY_XP;
    HICON sIcon = (HICON)::LoadImage(theApp.m_hInstance, MAKEINTRESOURCE(sIconId), IMAGE_ICON, 0, 0, 0);

    mSysTray = new SysTray;
    if (mSysTray->createTray(GetSafeHwnd(), WM_TRAY_MESSAGE, IDS_TRAY_NOTIFY, XPR_STRING_LITERAL("flyExplorer"), sIcon) == XPR_FALSE)
    {
        destroyTray();
        DESTROY_ICON(sIcon);
    }

    return mSysTray;
}

void MainFrame::destroyTray(void)
{
    XPR_SAFE_DELETE(mSysTray);
}

LRESULT MainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
    if (wParam == IDS_TRAY_NOTIFY)
    {
        if (XPR_IS_NULL(mSysTray))
            return 0;

        switch (lParam)
        {
        case WM_LBUTTONDOWN:
            {
                if (XPR_IS_TRUE(gOpt->mTrayOneClick))
                {
                    if (IsWindowVisible() == XPR_TRUE)
                    {
                        mSysTray->hideToTray();
                    }
                    else
                    {
                        showTrayMainFrame();
                    }
                }
                else
                {
                    SetForegroundWindow();
                }
                break;
            }

        case WM_LBUTTONDBLCLK:
            {
                if (XPR_IS_FALSE(gOpt->mTrayOneClick))
                {
                    if (IsWindowVisible() == XPR_TRUE)
                    {
                        mSysTray->hideToTray();
                    }
                    else
                    {
                        showTrayMainFrame();
                    }
                }
                break;
            }

        case WM_RBUTTONUP:
            {
                SetForegroundWindow();

                CPoint sPoint;
                ::GetCursorPos(&sPoint);

                BCMenu sMenu;
                if (sMenu.LoadMenu(IDR_TRAY) == XPR_TRUE)
                {
                    BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
                    if (XPR_IS_NOT_NULL(sPopupMenu))
                    {
                        sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sPoint, this);
                    }
                }

                break;
            }
        }
    }

    return 0;
}

LRESULT MainFrame::OnTaskRestarted(WPARAM wParam, LPARAM lParam)
{
    if (XPR_IS_TRUE(gOpt->mTray))
    {
        if (XPR_IS_NOT_NULL(mSysTray))
            mSysTray->recreateTray();
    }

    return 0;
}

void MainFrame::minimizeToTray(void)
{
    createTray();

    if (XPR_IS_NOT_NULL(mSysTray))
        mSysTray->hideToTray();
}

void MainFrame::setStatusPaneBookmarkText(xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect)
{
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

    mStatusBar.SetPaneText(0, sText);
}

void MainFrame::setStatusPaneDriveText(xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect)
{
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

    mStatusBar.SetPaneText(0, sText);
}

void MainFrame::setStatusPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText)
{
    if (aIndex < 0 && 1 < aIndex)
        return;

    if (aIndex == 1)
    {
        mStatusBar.setDynamicPaneText(aIndex, aText);
        return;
    }

    mStatusBar.SetPaneText(aIndex, aText);
}

void MainFrame::setStatusDisk(const xpr_tchar_t *aPath)
{
    mStatusBar.setDiskFreeSpace(aPath);
}

void MainFrame::setStatusGroup(HICON aIcon, const xpr_tchar_t *aGroup)
{
    mStatusBar.setGroup(aIcon, aGroup);
}

xpr_bool_t MainFrame::isVisibleStatusBar(void) const
{
    return mStatusBar.IsWindowVisible();
}

void MainFrame::GetMessageString(xpr_uint_t aId, CString &aMessage) const
{
    aMessage = XPR_STRING_LITERAL("");

    static xpr_tchar_t sMessage[XPR_MAX_PATH * 2 + 1];
    static xpr_tchar_t sReady[XPR_MAX_PATH * 2 + 1];
    sMessage[0] = sReady[0] = XPR_STRING_LITERAL('\0');

    if (XPR_IS_RANGE(ID_BOOKMARK_FIRST, aId, ID_BOOKMARK_LAST))
    {
        xpr_sint_t sBookmarkIndex = aId - ID_BOOKMARK_FIRST;
        aMessage = fxb::BookmarkMgr::instance().getBookmark(sBookmarkIndex)->mPath.c_str();
    }
    else if (XPR_IS_RANGE(ID_DRIVE_FIRST, aId, ID_DRIVE_LAST))
    {
        MainFrame &sMainFrame = (MainFrame &)*this;

        xpr_sint_t sViewIndex = 0;
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
            sViewIndex = sExplorerCtrl->getViewIndex();

        xpr_tchar_t sDriveChar = (aId - ID_DRIVE_FIRST) + XPR_STRING_LITERAL('A');
        const xpr_tchar_t *sDriveLastPath = sMainFrame.getDrivePath(sViewIndex, aId);

        _stprintf(sMessage, theApp.loadFormatString(XPR_STRING_LITERAL("drive.status.desc"), XPR_STRING_LITERAL("%c")), sDriveChar);

        if (XPR_IS_NOT_NULL(sDriveLastPath) && sDriveLastPath[0] != 0)
        {
            _stprintf(sMessage + _tcslen(sMessage), theApp.loadFormatString(XPR_STRING_LITERAL("drive.status.memorized_last_folder"), XPR_STRING_LITERAL("%s")), sDriveLastPath );
        }

        aMessage = sMessage;
    }
    else if (XPR_IS_RANGE(ID_GO_UP_FIRST, aId, ID_GO_UP_LAST))
    {
        xpr_sint_t sUpIndex = aId - ID_GO_UP_FIRST;

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_IS_NOT_NULL(sTvItemData->mFullPidl))
            {
                LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(sTvItemData->mFullPidl);
                if (XPR_IS_NOT_NULL(sFullPidl))
                {
                    xpr_sint_t sItemIdCount = fxb::GetItemIDCount(sFullPidl);

                    xpr_sint_t i;
                    xpr_bool_t sRemoved;
                    for (i = sItemIdCount - 1; i >= sUpIndex; --i)
                    {
                        sRemoved = fxb::IL_RemoveLastID(sFullPidl);
                        if (XPR_IS_FALSE(sRemoved))
                            break;
                    }

                    fxb::GetFullPath(sFullPidl, sMessage);
                    aMessage = sMessage;

                    COM_FREE(sFullPidl);
                }
            }
        }
    }
    else if (XPR_IS_RANGE(ID_GO_BACKWARD_FIRST, aId, ID_GO_BACKWARD_LAST) ||
        XPR_IS_RANGE(ID_GO_FORWARD_FIRST,  aId, ID_GO_FORWARD_LAST ) ||
        XPR_IS_RANGE(ID_GO_HISTORY_FIRST,  aId, ID_GO_HISTORY_LAST ))
    {
        // 0 - backward
        // 1 - forward
        // 2 - history

        xpr_sint_t sType = 0;
        if (XPR_IS_RANGE(ID_GO_FORWARD_FIRST, aId, ID_GO_FORWARD_LAST))
            sType = 1;
        else if (XPR_IS_RANGE(ID_GO_HISTORY_FIRST, aId, ID_GO_HISTORY_LAST))
            sType = 2;

        xpr_sint_t sBackwardIndex = aId - ID_GO_BACKWARD_FIRST;
        switch (sType)
        {
        case 1: sBackwardIndex = aId - ID_GO_FORWARD_FIRST; break;
        case 2: sBackwardIndex = aId - ID_GO_HISTORY_FIRST; break;
        }

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            fxb::HistoryDeque *sHistoryDeque = sExplorerCtrl->getBackwardList();
            switch (sType)
            {
            case 1: sHistoryDeque = sExplorerCtrl->getForwardList(); break;
            case 2: sHistoryDeque = sExplorerCtrl->getHistoryList(); break;
            }

            if (XPR_IS_NOT_NULL(sHistoryDeque))
            {
                fxb::HistoryDeque::reverse_iterator sReverseIterator = sHistoryDeque->rbegin() + sBackwardIndex;
                if (sReverseIterator != sHistoryDeque->rend())
                {
                    LPITEMIDLIST sFullPidl = *sReverseIterator;
                    if (XPR_IS_NOT_NULL(sFullPidl))
                    {
                        if (fxb::IsFileSystem(sFullPidl) == XPR_TRUE)
                            fxb::GetName(sFullPidl, SHGDN_FORPARSING, sMessage);
                        else
                            fxb::GetFullPath(sFullPidl, sMessage);

                        aMessage = sMessage;
                    }
                }
            }
        }
    }
    else if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, aId, ID_GO_WORKING_FOLDER_LAST))
    {
        xpr_sint_t sWorkingFolderIndex = aId - ID_GO_WORKING_FOLDER_FIRST;
        aMessage = gOpt->mWorkingFolder[sWorkingFolderIndex];
    }
    else if (XPR_IS_RANGE(ID_FILE_RECENT_FIRST, aId, ID_FILE_RECENT_LAST))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        theApp.getRecentFile(aId, sPath);

        aMessage = sPath;
    }
    else
    {
        ::LoadString(AfxGetApp()->m_hInstance, aId, sMessage, XPR_MAX_PATH);
        ::LoadString(AfxGetApp()->m_hInstance, AFX_IDS_IDLEMESSAGE, sReady, XPR_MAX_PATH);

        if (_tcscmp(aMessage, sReady) == 0 ||
            _tcscmp(sMessage, sReady) == 0 ||
            aId == AFX_IDW_FOLDER_CTRL)
        {
            if (getExplorerCtrl() != XPR_NULL)
            {
                ExplorerView *sExplorerView = getExplorerView();
                if (XPR_IS_NOT_NULL(sExplorerView))
                    aMessage = sExplorerView->getStatusPaneText(0);
            }
        }
    }

    if (aMessage.GetLength() == 0)
        super::GetMessageString(aId, aMessage);
}

#ifdef XPR_CFG_BUILD_DEBUG
void MainFrame::AssertValid(void) const
{
    super::AssertValid();
}

void MainFrame::Dump(CDumpContext &aDumpContext) const
{
    super::Dump(aDumpContext);
}

#endif // XPR_CFG_BUILD_DEBUG

void MainFrame::OnToolbarDropDown(NMHDR *aNmHdr, LRESULT *aLResult)
{
    NMTOOLBAR *sNmToolBar = (NMTOOLBAR *)aNmHdr;
    CWnd *sWnd = CWnd::FromHandle(sNmToolBar->hdr.hwndFrom);

    xpr_uint_t sMenuId = 0;
    switch (sNmToolBar->iItem)
    {
    case ID_FILE_VIEW:         sMenuId = IDR_FILE_VIEW;         break;
    case ID_FILE_EDIT:         sMenuId = IDR_FILE_EDIT;         break;
    case ID_GO_BACK:           sMenuId = IDR_GO_BACK;           break;
    case ID_GO_FORWARD:        sMenuId = IDR_GO_FORWARD;        break;
    case ID_GO_HISTORY:        sMenuId = IDR_GO_HISTORY;        break;
    case ID_GO_UP:             sMenuId = IDR_GO_UP;             break;
    case ID_GO_BOOKMARK:       sMenuId = IDR_GO_BOOKMARK;       break;
    case ID_GO_DRIVE:          sMenuId = IDR_GO_DRIVE;          break;
    case ID_GO_WORKING_FOLDER: sMenuId = IDR_GO_WORKING_FOLDER; break;
    case ID_VIEW_STYLE:        sMenuId = IDR_EXP_VIEW_STYLE;    break;
    case ID_WINDOW_SPLIT:      sMenuId = IDR_SPLIT_VIEW;        break;

    default:
        return;
    }

    CRect sRect;
    sWnd->SendMessage(TB_GETRECT, sNmToolBar->iItem, (LPARAM)&sRect);
    sWnd->ClientToScreen(&sRect);

    if (sMenuId == IDR_GO_BOOKMARK)
    {
        trackBookmarkPopupMenu(sRect.left, sRect.bottom, &sRect);
        return;
    }
    else if (sMenuId == IDR_GO_WORKING_FOLDER)
    {
        trackWorkingFolderPopupMenu(sRect.left, sRect.bottom, &sRect);
        return;
    }

    BCMenu sMenu;
    if (sMenu.LoadMenu(sMenuId) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        ASSERT(sPopupMenu);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sRect.left, sRect.bottom, this, &sRect);
        }
    }
}

xpr_bool_t MainFrame::isNoneFolderPane(void) const
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        return isVisibleFolderPane() ? XPR_FALSE : XPR_TRUE;
    }

    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    ExplorerView *sExplorerView;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NULL(sExplorerView))
            continue;

        if (sExplorerView->isVisibleFolderPane() == XPR_TRUE)
            return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t MainFrame::isVisibleFolderPane(void) const
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        FolderView *sFolderView = getFolderView();
        if (XPR_IS_NOT_NULL(sFolderView))
        {
            FolderPane *sFolderPane = sFolderView->getFolderPane();
            if (XPR_IS_NOT_NULL(sFolderPane))
                return sFolderPane->IsWindowVisible();
        }

        return XPR_FALSE;
    }

    ExplorerView *sExplorerView = getExplorerView();
    if (XPR_IS_NULL(sExplorerView))
        return XPR_FALSE;

    return sExplorerView->isVisibleFolderPane();
}

xpr_bool_t MainFrame::isLeftFolderPane(void) const
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        return gOpt->mLeftSingleFolderPane;
    }

    ExplorerView *sExplorerView = getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        return sExplorerView->isLeftFolderPane();
    }

    return XPR_TRUE;
}

void MainFrame::initFolderCtrl(void)
{
    FolderCtrl *sFolderCtrl = getFolderCtrl();
    if (XPR_IS_NOT_NULL(sFolderCtrl))
    {
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();

            HTREEITEM sTreeItem = sFolderCtrl->searchSel(sTvItemData->mFullPidl, XPR_FALSE);
            if (XPR_IS_NOT_NULL(sTreeItem))
            {
                if (XPR_IS_FALSE(gOpt->mFolderTreeInitNoExpand))
                    sFolderCtrl->Expand(sTreeItem, TVE_EXPAND);

                sFolderCtrl->EnsureVisible(sTreeItem);
            }
        }
    }
}

void MainFrame::showFolderPane(xpr_bool_t aShow)
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        if (XPR_IS_TRUE(aShow))
        {
            mOneFolderSplitter.split(1, 2);

            initFolderCtrl();
        }
        else
        {
            mOneFolderSplitter.split(0, 0);
        }

        gOpt->mShowSingleFolderPane = aShow;

        recalcLayout();
    }
    else
    {
        ExplorerView *sExplorerView = getExplorerView();
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            sExplorerView->visibleFolderPane(aShow);
        }
    }
}

xpr_bool_t MainFrame::isSingleFolderPaneMode(void) const
{
    return XPR_IS_TRUE(gOpt->mSingleFolderPaneMode) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::setSingleFolderPaneMode(xpr_bool_t aSingleFolderPaneMode)
{
    if (gOpt->mSingleFolderPaneMode == aSingleFolderPaneMode)
        return;

    gOpt->mSingleFolderPaneMode = aSingleFolderPaneMode;

    if (XPR_IS_TRUE(aSingleFolderPaneMode))
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = getViewCount();
        ExplorerView *sExplorerView;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerView = getExplorerView(i);
            if (XPR_IS_NOT_NULL(sExplorerView))
            {
                sExplorerView->visibleFolderPane(XPR_FALSE, XPR_FALSE, XPR_FALSE);
            }
        }

        mOneFolderSplitter.split(1, 2);

        // initialize folder control for one folder pane
        initFolderCtrl();
    }
    else
    {
        mOneFolderSplitter.split(0, 0);

        xpr_sint_t i;
        xpr_sint_t sViewCount = getViewCount();
        ExplorerView *sExplorerView;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerView = getExplorerView(i);
            if (XPR_IS_NOT_NULL(sExplorerView))
            {
                if (XPR_IS_TRUE(gOpt->mShowEachFolderPane[i]))
                    sExplorerView->visibleFolderPane(XPR_TRUE);
            }
        }
    }

    recalcLayout();
}

void MainFrame::setNoneFolderPane(void)
{
    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        showFolderPane(XPR_FALSE);
        return;
    }

    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    ExplorerView *sExplorerView;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NULL(sExplorerView))
            continue;

        sExplorerView->visibleFolderPane(XPR_FALSE);
    }
}

void MainFrame::setLeftFolderPane(xpr_bool_t aLeft)
{
    if (isVisibleFolderPane() == XPR_FALSE)
        return;

    if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
    {
        if (gOpt->mLeftSingleFolderPane != aLeft)
        {
            gOpt->mLeftSingleFolderPane = aLeft;

            mOneFolderSplitter.switchPane(0, 0, 0, 1);

            recalcLayout();
        }

        return;
    }

    ExplorerView *sExplorerView = getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        sExplorerView->setLeftFolderPane(aLeft);
    }
}

xpr_bool_t MainFrame::executeCommand(xpr_uint_t aCommandId, CWnd *aTargetWnd, cmd::CommandParameters *aParameters)
{
    if (mCommandExecutor == XPR_NULL)
        return XPR_FALSE;

    cmd::CommandContext sCommandContext;
    sCommandContext.setCommandId(aCommandId);
    sCommandContext.setMainFrame(this);
    sCommandContext.setTargetWnd(aTargetWnd);
    sCommandContext.setParameters(aParameters);

    if (mCommandExecutor->execute(sCommandContext) == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t MainFrame::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo)
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

        if (sCode == CN_UPDATE_COMMAND_UI)
        {
            if (mCommandExecutor != XPR_NULL)
            {
                CCmdUI *sCmdUI = (CCmdUI *)aExtra;

                cmd::CommandContext sCommandContext;
                sCommandContext.setCommandId(aId);
                sCommandContext.setMainFrame(this);

                xpr_sint_t sCommandState = mCommandExecutor->canExecute(sCommandContext);
                if (sCommandState != cmd::Command::StateUnbinded)
                {
                    if (sCommandState == cmd::Command::StateDisable)
                    {
                        sCmdUI->Enable(XPR_FALSE);
                    }
                    else
                    {
                        if (XPR_TEST_BITS(sCommandState, cmd::Command::StateEnable))
                        {
                            sCmdUI->Enable(XPR_TRUE);
                        }
                        else if (XPR_TEST_BITS(sCommandState, cmd::Command::StateDisable))
                        {
                            sCmdUI->Enable(XPR_FALSE);
                        }

                        if (XPR_TEST_BITS(sCommandState, cmd::Command::StateCheck) || XPR_TEST_BITS(sCommandState, cmd::Command::StateRadio))
                        {
                            if (XPR_TEST_BITS(sCommandState, cmd::Command::StateCheck)) sCmdUI->SetCheck(XPR_TRUE);
                            if (XPR_TEST_BITS(sCommandState, cmd::Command::StateRadio)) sCmdUI->SetRadio(XPR_TRUE);
                        }
                        else
                        {
                            sCmdUI->SetCheck(XPR_FALSE);
                        }
                    }

                    return XPR_TRUE;
                }
            }
        }
        else if (sCode == CN_COMMAND)
        {
            if (executeCommand(aId) == XPR_TRUE)
                return XPR_TRUE;
        }
    }

    if (ID_VIEW_PIC_BEGIN <= aId && aId <= ID_VIEW_PIC_END)
    {
        if (XPR_IS_NOT_NULL(mPicViewer) && XPR_IS_NOT_NULL(mPicViewer->m_hWnd))
        {
            static xpr_bool_t sNoRecursive = XPR_FALSE;
            if (XPR_IS_FALSE(sNoRecursive))
            {
                sNoRecursive = XPR_TRUE;
                xpr_bool_t sResult = mPicViewer->OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
                sNoRecursive = XPR_FALSE;

                if (XPR_IS_TRUE(sResult))
                    return XPR_TRUE;
            }
        }
    }

    return super::OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
}

xpr_bool_t MainFrame::PreTranslateMessage(MSG *aMsg) 
{
    static xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
    sClassName[0] = XPR_STRING_LITERAL('\0');

    if (aMsg->message == WM_SYSKEYDOWN || aMsg->message == WM_KEYDOWN)
    {
        DWORD sVKey = (DWORD)aMsg->wParam;
        switch (sVKey)
        {
        case XPR_STRING_LITERAL('d'):
        case XPR_STRING_LITERAL('D'):
            if (GetAsyncKeyState(VK_MENU) < 0)
            {
                ExplorerView *sExplorerView = getExplorerView();
                if (XPR_IS_NOT_NULL(sExplorerView))
                {
                    AddressBar *sAddressBar = sExplorerView->getAddressBar();
                    if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
                        sAddressBar->SetFocus();
                }
                return XPR_TRUE;
            }
            break;

        case XPR_STRING_LITERAL('0'):
        case XPR_STRING_LITERAL('1'):
        case XPR_STRING_LITERAL('2'):
        case XPR_STRING_LITERAL('3'):
        case XPR_STRING_LITERAL('4'):
        case XPR_STRING_LITERAL('5'):
        case XPR_STRING_LITERAL('6'):
        case XPR_STRING_LITERAL('7'):
        case XPR_STRING_LITERAL('8'):
        case XPR_STRING_LITERAL('9'):
            if (GetKeyState(VK_CONTROL) < 0)
            {
                xpr_sint_t i, sCount;
                fxb::BookmarkItem *sBookmarkItem;

                fxb::BookmarkMgr &sBookmarkMgr = fxb::BookmarkMgr::instance();

                sCount = sBookmarkMgr.getCount();
                for (i = 0; i < sCount; ++i)
                {
                    sBookmarkItem = sBookmarkMgr.getBookmark(i);
                    if (XPR_IS_NOT_NULL(sBookmarkItem))
                    {
                        if (sVKey == LOWORD(sBookmarkItem->mHotKey))
                        {
                            gotoBookmark(i);
                            return XPR_TRUE;
                        }
                    }
                }
            }
            break;

        case VK_ADD:
        case VK_SUBTRACT:
            ::GetClassName(aMsg->hwnd, sClassName, MAX_CLASS_NAME);
            if (_tcsicmp(sClassName, XPR_STRING_LITERAL("SysTreeView32")) == 0)
            {
                if (CWnd::PreTranslateMessage(aMsg))
                    return XPR_TRUE;
                return XPR_FALSE;
            }
            break;
        }
    }

    // Skip Edit Control
    ::GetClassName(aMsg->hwnd, sClassName, MAX_CLASS_NAME);
    if (_tcsicmp(sClassName, XPR_STRING_LITERAL("Edit")) == 0)
        return XPR_FALSE;

    if (aMsg->message == WM_SYSKEYDOWN || aMsg->message == WM_KEYDOWN)
    {
        if (XPR_IS_TRUE(gOpt->mDriveShiftKey))
        {
            if (GetKeyState(VK_SHIFT) < 0)
            {
                xpr_tchar_t sDriveChar = (xpr_tchar_t)aMsg->wParam;
                if (aMsg->wParam == VK_PROCESSKEY)
                    sDriveChar = ::ImmGetVirtualKey(m_hWnd);

                xpr_tchar_t sDrive[2];
                sDrive[0] = sDriveChar;
                sDrive[1] = '\0';
                _tcsupr(sDrive);

                if (XPR_IS_RANGE('A', sDrive[0], 'Z'))
                {
                    gotoDrive(sDrive[0]);
                    return XPR_TRUE;
                }
            }
        }
    }

    return super::PreTranslateMessage(aMsg);
}

void MainFrame::popupWorkingFolderMenu(void)
{
    CToolBar *sToolBar = &m_wndReBar.mMainToolBar;
    xpr_sint_t sIndex = sToolBar->CommandToIndex(ID_GO_WORKING_FOLDER);
    if (sIndex < 0)
    {
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            CRect sRect;
            sExplorerCtrl->GetWindowRect(&sRect);
            trackWorkingFolderPopupMenu(sRect.left, sRect.top);
        }
        return;
    }

    CRect sRect;
    CPoint sPoint;
    sToolBar->GetItemRect(sIndex, &sRect);
    sPoint.x = sRect.left + 5;
    sPoint.y = sRect.top  + 5;

    WPARAM sWParam = MK_LBUTTON;
    LPARAM sLParam = MAKELPARAM(sPoint.x, sPoint.y);
    sToolBar->PostMessage(WM_LBUTTONDOWN, sWParam, sLParam);
}

xpr_bool_t MainFrame::goWorkingFolder(xpr_sint_t aIndex)
{
    if (!XPR_IS_RANGE(0, aIndex, MAX_WORKING_FOLDER-1))
        return XPR_FALSE;

    if (fxb::IsEmptyString(gOpt->mWorkingFolder[aIndex]) == XPR_TRUE)
        return XPR_FALSE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPITEMIDLIST sFullPidl = fxb::Path2Pidl(gOpt->mWorkingFolder[aIndex]);
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            if (sExplorerCtrl->explore(sFullPidl) == XPR_FALSE)
            {
                COM_FREE(sFullPidl);
            }
        }
    }

    return XPR_TRUE;
}

xpr_bool_t MainFrame::setWorkingFolder(xpr_size_t aIndex, LPITEMIDLIST aFullPidl)
{
    if (!XPR_IS_RANGE(0, aIndex, MAX_WORKING_FOLDER-1))
        return XPR_FALSE;

    xpr_bool_t sOnlyFileSystemPath = XPR_FALSE;

    if (XPR_IS_TRUE(gOpt->mWorkingFolderRealPath))
        sOnlyFileSystemPath = XPR_TRUE;
    else
    {
        sOnlyFileSystemPath = XPR_TRUE;

        xpr_bool_t sParentVirtualItem = fxb::IsParentVirtualItem(aFullPidl);
        if (XPR_IS_TRUE(sParentVirtualItem) && fxb::IsFileSystem(aFullPidl) == XPR_TRUE)
        {
            std::tstring sPath;
            std::tstring sFullPath;

            fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);
            fxb::GetFullPath(aFullPidl, sFullPath);

            xpr_tchar_t sMsg[XPR_MAX_PATH * 3 + 1] = {0};
            _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("working_folder.msg.question_real_path"), XPR_STRING_LITERAL("%s,%s")), sPath.c_str(), sFullPath.c_str());
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNOCANCEL | MB_ICONQUESTION);
            if (sMsgId == IDCANCEL)
                return XPR_FALSE;

            sOnlyFileSystemPath = (sMsgId == IDYES) ? XPR_TRUE : XPR_FALSE;
        }
    }

    return fxb::Pidl2Path(aFullPidl, gOpt->mWorkingFolder[aIndex], sOnlyFileSystemPath);
}

void MainFrame::resetWorkingFolder(xpr_size_t aIndex)
{
    if (!XPR_IS_RANGE(0, aIndex, MAX_WORKING_FOLDER - 1))
        return;

    xpr_tchar_t sMsg[0xff] = {0};
    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("working_folder.msg.question_initialize"), XPR_STRING_LITERAL("%d")), aIndex + 1);
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    gOpt->mWorkingFolder[aIndex][0] = XPR_STRING_LITERAL('\0');
}

void MainFrame::resetWorkingFolder(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("working_folder.msg.question_initialize_all"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    xpr_size_t i;
    for (i = 0; i < MAX_WORKING_FOLDER; ++i)
    {
        gOpt->mWorkingFolder[i][0] = XPR_STRING_LITERAL('\0');
    }
}

xpr_sint_t MainFrame::insertShellNewPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    fxb::ShellNew &sShellNew = fxb::ShellNew::instance();

    sShellNew.clear();
    sShellNew.setBaseCommandId(ID_FILE_NEW_FIRST);
    sShellNew.setText(theApp.loadString(XPR_STRING_LITERAL("cmd.new.shell_item_prefix")), theApp.loadString(XPR_STRING_LITERAL("cmd.new.shell_item_suffix")));
    sShellNew.getRegShellNew();

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    fxb::ShellNew::Item *sItem;

    sCount = (xpr_sint_t)sShellNew.getCount();
    if (sCount <= 0)
        return 0;

    fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();

    for (i = 0; i < sCount; ++i)
    {
        sItem = sShellNew.getItem(i);
        if (XPR_IS_NULL(sItem))
            continue;

        _tcscpy(sText, sItem->mFileType.c_str());

        aPopupMenu->InsertMenu(aInsert+i, MF_STRING | MF_BYPOSITION, ID_FILE_NEW_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sItem->mIconIndex);
    }

    return sCount;
}

void MainFrame::showTrayMainFrame(void)
{
    if (XPR_IS_TRUE(gOpt->mTrayRestoreInitFolder))
    {
        goInitFolder();
    }

    SetForegroundWindow();

    mSysTray->showFromTray();

    // minimize to tray on file menu
    if (XPR_IS_FALSE(gOpt->mTray))
        destroyTray();
}

void MainFrame::WinHelp(DWORD aData, xpr_uint_t aCmd)
{
    super::WinHelp(aData, aCmd);
}

void MainFrame::setSplitEqually(void)
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    if (sRowCount > 1 || sColumnCount > 1)
    {
        CRect sRect;
        mSplitter.getWindowRect(sRect);

        xpr_sint_t sWidth  = sRect.Width();
        xpr_sint_t sHeight = sRect.Height();

        if (sWidth > 0 && sHeight > 0)
        {
            if (sRowCount == 1 && sColumnCount == 2)
            {
                gOpt->mViewSplitRatio[0] = 0.5;
                gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mViewSplitRatio[0] + 0.5);

                mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 1)
            {
                gOpt->mViewSplitRatio[0] = 0.5;
                gOpt->mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[0] + 0.5);

                mSplitter.moveRow(0, gOpt->mViewSplitSize[0]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 2)
            {
                gOpt->mViewSplitRatio[0] = 0.5;
                gOpt->mViewSplitRatio[1] = 0.5;
                gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
                gOpt->mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[1] + 0.5);

                mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
                mSplitter.moveRow   (0, gOpt->mViewSplitSize[1]);
                mSplitter.resize();
            }
            else if (sRowCount == 1 && sColumnCount == 3)
            {
                gOpt->mViewSplitRatio[0] = 0.333;
                gOpt->mViewSplitRatio[1] = 0.333;
                gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
                gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);

                mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
                mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 3)
            {
                gOpt->mViewSplitRatio[0] = 0.333;
                gOpt->mViewSplitRatio[1] = 0.333;
                gOpt->mViewSplitRatio[2] = 0.5;
                gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
                gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);
                gOpt->mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[2] + 0.5);

                mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
                mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
                mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
                mSplitter.resize();
            }
        }
    }
}

LPITEMIDLIST MainFrame::getSplitFolder(xpr_sint_t aIndex) const
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return XPR_NULL;

    return mSplitFullPidl[aIndex];
}

xpr_sint_t MainFrame::getActiveView(void) const
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    xpr_sint_t sRow = 0, sColumn = 0;
    mSplitter.getActivePane(&sRow, &sColumn);

    xpr_sint_t sViewIndex = 0;
    getViewSplitToViewIndex(sRowCount, sColumnCount, sRow, sColumn, sViewIndex);

    return sViewIndex;
}

xpr_sint_t MainFrame::getLastActivedView(void) const
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    xpr_sint_t sRow = 0, sColumn = 0;
    mSplitter.getLastActivedPane(&sRow, &sColumn);

    xpr_sint_t sViewIndex = 0;
    getViewSplitToViewIndex(sRowCount, sColumnCount, sRow, sColumn, sViewIndex);

    return sViewIndex;
}

void MainFrame::setActiveView(xpr_sint_t aViewIndex)
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    xpr_sint_t sRow = 0, sColumn = 0;
    getViewIndexToViewSplit(aViewIndex, sRowCount, sColumnCount, sRow, sColumn);

    setActiveView(sRow, sColumn);
}

void MainFrame::setActiveView(xpr_sint_t aRow, xpr_sint_t aColumn)
{
    mSplitter.setActivePane(aRow, aColumn);
}

void MainFrame::splitView(xpr_sint_t aRowCount, xpr_sint_t aColumnCount, xpr_bool_t aLoading)
{
    if (!XPR_IS_RANGE(1, aRowCount, MAX_VIEW_SPLIT_ROW) || !XPR_IS_RANGE(1, aColumnCount, MAX_VIEW_SPLIT_COLUMN))
        aRowCount = aColumnCount = 1;

    xpr_sint_t sOldRowCount = 0, sOldColumnCount = 0;
    mSplitter.getPaneCount(&sOldRowCount, &sOldColumnCount);

    if (aRowCount == sOldRowCount && aColumnCount == sOldColumnCount)
        return;

    if (XPR_IS_FALSE(aLoading))
        SetRedraw(XPR_FALSE);

    xpr_bool_t sActivedView00 = XPR_FALSE;

    // old view
    if (XPR_IS_FALSE(aLoading))
    {
        if (XPR_IS_TRUE(gOpt->mSingleFolderPaneMode))
        {
            // TODO
        }
        else
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 2) ||
                (sOldRowCount == 2 && sOldColumnCount == 1))
            {
                ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    COM_FREE(mSplitFullPidl[0]);
                    COM_FREE(mSplitFullPidl[1]);

                    ExplorerCtrl *sExplorerCtrl0 = getExplorerCtrl(0);
                    ExplorerCtrl *sExplorerCtrl1 = getExplorerCtrl(1);

                    if (XPR_IS_NOT_NULL(sExplorerCtrl0))
                    {
                        LPTVITEMDATA sTvItemData = sExplorerCtrl0->getFolderData();
                        if (XPR_IS_NOT_NULL(sTvItemData))
                            mSplitFullPidl[0] = fxb::CopyItemIDList(sTvItemData->mFullPidl);
                    }

                    if (XPR_IS_NOT_NULL(sExplorerCtrl1))
                    {
                        LPTVITEMDATA sTvItemData = sExplorerCtrl1->getFolderData();
                        if (XPR_IS_NOT_NULL(sTvItemData))
                            mSplitFullPidl[1] = fxb::CopyItemIDList(sTvItemData->mFullPidl);
                    }

                    sActivedView00 = (sExplorerCtrl == sExplorerCtrl0) ? XPR_TRUE : XPR_FALSE;
                }
            }
        }
    }

    // split view
    mSplitter.split(aRowCount, aColumnCount);

    // evaluate ratio for new view split
    if (aRowCount == 1 && aColumnCount == 2)
    {
        if (gOpt->mViewSplitRatio[0] <= 0.0 || gOpt->mViewSplitRatio[0] >= 1.0)
            gOpt->mViewSplitRatio[0] = 0.5;
    }
    else if (aRowCount == 2 && aColumnCount == 1)
    {
        if (gOpt->mViewSplitRatio[0] <= 0.0 || gOpt->mViewSplitRatio[0] >= 1.0)
            gOpt->mViewSplitRatio[0] = 0.5;
    }
    else if (aRowCount == 2 && aColumnCount == 2)
    {
        if (gOpt->mViewSplitRatio[0] <= 0.0 || gOpt->mViewSplitRatio[0] >= 1.0)
            gOpt->mViewSplitRatio[0] = 0.5;

        if (gOpt->mViewSplitRatio[1] <= 0.0 || gOpt->mViewSplitRatio[1] >= 1.0)
            gOpt->mViewSplitRatio[1] = 0.5;
    }
    else if (aRowCount == 1 && aColumnCount == 3)
    {
        if (((gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]) <= 0.0) ||
            ((gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]) >= 1.0))
        {
            gOpt->mViewSplitRatio[0] = 0.333;
            gOpt->mViewSplitRatio[1] = 0.333;
        }
    }
    else if (aRowCount == 2 && aColumnCount == 3)
    {
        if (((gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]) <= 0.0) ||
            ((gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]) >= 1.0))
        {
            gOpt->mViewSplitRatio[0] = 0.333;
            gOpt->mViewSplitRatio[1] = 0.333;
        }

        if (gOpt->mViewSplitRatio[2] <= 0.0 || gOpt->mViewSplitRatio[2] >= 1.0)
            gOpt->mViewSplitRatio[2] = 0.5;
    }

    // change view split
    if (XPR_IS_FALSE(aLoading))
    {
        CRect sRect;
        mSplitter.getWindowRect(sRect);

        xpr_sint_t sWidth  = sRect.Width();
        xpr_sint_t sHeight = sRect.Height();

        // reset ratio for new view split
        if ((aRowCount == 1 && aColumnCount == 3) ||
            (aRowCount == 2 && aColumnCount == 3))
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 2) ||
                (sOldRowCount == 2 && sOldColumnCount == 1) ||
                (sOldRowCount == 2 && sOldColumnCount == 2))
            {
                gOpt->mViewSplitRatio[0] = 0.333;
                gOpt->mViewSplitRatio[1] = 0.333;
                gOpt->mViewSplitRatio[2] = 0.5;
            }
        }
        else if (aRowCount == 2 && aColumnCount == 2)
        {
            if (sOldRowCount == 2 && sOldColumnCount == 1)
            {
                gOpt->mViewSplitRatio[1] = gOpt->mViewSplitRatio[0];
            }
            else if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                     (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mViewSplitRatio[0] = 0.5;
                gOpt->mViewSplitRatio[1] = 0.5;
                gOpt->mViewSplitRatio[2] = 0;
            }
        }
        else if ((aRowCount == 1 && aColumnCount == 2) ||
                 (aRowCount == 2 && aColumnCount == 1))
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mViewSplitRatio[0] = 0.5;
                gOpt->mViewSplitRatio[1] = 0;
                gOpt->mViewSplitRatio[2] = 0;
            }
        }
        else if (aRowCount == 1 && aColumnCount == 1)
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mViewSplitRatio[0] = 0;
                gOpt->mViewSplitRatio[1] = 0;
                gOpt->mViewSplitRatio[2] = 0;
            }
        }

        // calulate size by ratio and resize pane window
        if (aRowCount == 1 && aColumnCount == 1)
        {
            mSplitter.moveRow   (0, sWidth);
            mSplitter.moveColumn(0, sHeight);
            mSplitter.resize();

            mSplitter.setActivePane(0, 0);

            // 1x2 or 2x1 view -> single(1x1) view: focused folder
            if (XPR_IS_TRUE(gOpt->mSingleViewActivatePath) && XPR_IS_FALSE(sActivedView00))
            {
                std::swap(mSplitFullPidl[0], mSplitFullPidl[1]);

                ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    LPITEMIDLIST sFullPidl = fxb::CopyItemIDList(mSplitFullPidl[0]);
                    if (XPR_IS_NOT_NULL(sFullPidl))
                    {
                        if (sExplorerCtrl->explore(sFullPidl) == XPR_FALSE)
                        {
                            COM_FREE(sFullPidl);
                        }
                    }
                }
            }
        }
        else if (aRowCount == 1 && aColumnCount == 2)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mViewSplitRatio[0] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 1)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[0] + 0.5);

            mSplitter.moveRow(0, gOpt->mViewSplitSize[0]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 2)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[1]);
            mSplitter.resize();
        }
        else if (aRowCount == 1 && aColumnCount == 3)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 3)
        {
            gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
            gOpt->mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[1] + 0.5);
            gOpt->mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[2] + 0.5);

            mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
            mSplitter.resize();
        }
    }

    if (aRowCount > 1 || aColumnCount > 1)
    {
        if (XPR_IS_NOT_NULL(mPicViewer))
            mPicViewer->setDocking(XPR_FALSE);
    }

    if (XPR_IS_FALSE(aLoading))
    {
        SetRedraw();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t MainFrame::isSingleView(void) const
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    getViewSplit(sRowCount, sColumnCount);

    if (sRowCount <= 1 && sColumnCount <= 1)
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_sint_t MainFrame::getViewIndex(void) const
{
    xpr_sint_t sRow = 0, sColumn = 0;
    mSplitter.getActivePane(&sRow, &sColumn);

    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    xpr_sint_t sViewIndex = 0;
    getViewSplitToViewIndex(sRowCount, sColumnCount, sRow, sColumn, sViewIndex);

    return sViewIndex;
}

void MainFrame::getViewSplit(xpr_sint_t &aRowCount, xpr_sint_t &aColumnCount) const
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    aRowCount    = sRowCount;
    aColumnCount = sColumnCount;
}

xpr_sint_t MainFrame::getViewCount(void) const
{
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    getViewSplit(sRowCount, sColumnCount);

    xpr_sint_t sViewCount = sRowCount * sColumnCount;

    return sViewCount;
}

//
// +---+   +---+   +---+---+   +---+---+   +---+---+---+   +---+---+---+
// | 0 |   | 0 |   | 0 | 1 |   | 0 | 1 |   | 0 | 1 | 2 |   | 0 | 1 | 2 |
// +---+   +---+   +---+---+   +---+---+   +---+---+---+   +---+---+---+
//         | 1 |               | 2 | 3 |                   | 3 | 4 | 5 |
//         +---+               +---+---+                   +---+---+---+
//
void MainFrame::getViewSplitToViewIndex(xpr_sint_t  aRowCount,
                                        xpr_sint_t  aColumnCount,
                                        xpr_sint_t  aRow,
                                        xpr_sint_t  aColumn,
                                        xpr_sint_t &aViewIndex) const
{
    aViewIndex = 0;

    if (aRowCount == 1 && aColumnCount == 2)
    {
        if (aRow == 0 && aColumn == 1)
            aViewIndex = 1;
    }
    else if (aRowCount == 1 && aColumnCount == 3)
    {
        if (aRow == 0 && aColumn == 1)
            aViewIndex = 1;
        else if (aRow == 0 && aColumn == 2)
            aViewIndex = 2;
    }
    else if (aRowCount == 2 && aColumnCount == 1)
    {
        if (aRow == 1 && aColumn == 0)
            aViewIndex = 1;
    }
    else if (aRowCount == 2 && aColumnCount == 2)
    {
        if (aRow == 0 && aColumn == 1)
            aViewIndex = 1;
        else if (aRow == 1 && aColumn == 0)
            aViewIndex = 2;
        else if (aRow == 1 && aColumn == 1)
            aViewIndex = 3;
    }
    else if (aRowCount == 2 && aColumnCount == 3)
    {
        if (aRow == 0 && aColumn == 1)
            aViewIndex = 1;
        else if (aRow == 0 && aColumn == 2)
            aViewIndex = 2;
        else if (aRow == 1 && aColumn == 0)
            aViewIndex = 3;
        else if (aRow == 1 && aColumn == 1)
            aViewIndex = 4;
        else if (aRow == 1 && aColumn == 2)
            aViewIndex = 5;
    }
}

void MainFrame::getViewIndexToViewSplit(xpr_sint_t  aViewIndex,
                                        xpr_sint_t  aRowCount,
                                        xpr_sint_t  aColumnCount,
                                        xpr_sint_t &aRow,
                                        xpr_sint_t &aColumn) const
{
    aRow = aColumn = 0;

    if (aRowCount == 1 && aColumnCount == 2)
    {
        if (aViewIndex == 1)
        {
            aRow = 0;
            aColumn = 1;
        }
    }
    else if (aRowCount == 1 && aColumnCount == 3)
    {
        if (aViewIndex == 1)
        {
            aRow = 0;
            aColumn = 1;
        }
        else if (aViewIndex == 2)
        {
            aRow = 0;
            aColumn = 2;
        }
    }
    else if (aRowCount == 2 && aColumnCount == 1)
    {
        if (aViewIndex == 1)
        {
            aRow = 1;
            aColumn = 0;
        }
    }
    else if (aRowCount == 2 && aColumnCount == 2)
    {
        if (aViewIndex == 1)
        {
            aRow = 0;
            aColumn = 1;
        }
        else if (aViewIndex == 2)
        {
            aRow = 1;
            aColumn = 0;
        }
        else if (aViewIndex == 3)
        {
            aRow = 1;
            aColumn = 1;
        }
    }
    else if (aRowCount == 2 && aColumnCount == 3)
    {
        if (aViewIndex == 1)
        {
            aRow = 0;
            aColumn = 1;
        }
        else if (aViewIndex == 2)
        {
            aRow = 0;
            aColumn = 2;
        }
        else if (aViewIndex == 3)
        {
            aRow = 1;
            aColumn = 0;
        }
        else if (aViewIndex == 4)
        {
            aRow = 1;
            aColumn = 1;
        }
        else if (aViewIndex == 5)
        {
            aRow = 1;
            aColumn = 2;
        }
    }
}

void MainFrame::gotoDrive(xpr_tchar_t aDriveChar)
{
    if (!XPR_IS_RANGE(XPR_STRING_LITERAL('a'), aDriveChar, XPR_STRING_LITERAL('z')) &&
        !XPR_IS_RANGE(XPR_STRING_LITERAL('A'), aDriveChar, XPR_STRING_LITERAL('Z')))
        return;

    //xpr_bool_t sShiftKey = (::GetAsyncKeyState(VK_SHIFT)   & 0x8000) ? XPR_TRUE : XPR_FALSE;
    xpr_bool_t sCtrlKey  = (::GetAsyncKeyState(VK_CONTROL) & 0x8000) ? XPR_TRUE : XPR_FALSE;

    xpr_sint_t sIndex = -1;
    //if (XPR_IS_TRUE(sShiftKey))
    //    sIndex = -2;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl(sIndex);
    if (XPR_IS_NULL(sExplorerCtrl))
        return;

    xpr_sint_t sViewIndex = sExplorerCtrl->getViewIndex();

    xpr_bool_t sMemorize = XPR_FALSE;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];

    // Save
    LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
    if (XPR_IS_NOT_NULL(sTvItemData) && sTvItemData->mShellAttributes & SFGAO_FILESYSTEM)
    {
        fxb::GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
        setDrivePath(sViewIndex, sPath);
    }

    // Load
    if (XPR_IS_TRUE(sCtrlKey) || XPR_IS_TRUE(gOpt->mDriveLastFolder))
    {
        const xpr_tchar_t *sDrivePath = getDrivePath(sViewIndex, aDriveChar);
        if (XPR_IS_NOT_NULL(sDrivePath) && fxb::IsExistFile(sDrivePath) == XPR_TRUE)
        {
            _tcscpy(sPath, sDrivePath);
            sMemorize = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sMemorize))
        _stprintf(sPath, XPR_STRING_LITERAL("%c:\\"), aDriveChar);

    LPITEMIDLIST sFullPidl = XPR_NULL;
    fxb::SHGetPidlFromPath(sPath, &sFullPidl);

    if (sExplorerCtrl->explore(sFullPidl) == XPR_TRUE)
    {
        if (sIndex == -2)
            sExplorerCtrl->SetFocus();
    }
    else
    {
        COM_FREE(sFullPidl);
    }
}

void MainFrame::setDriveBar(xpr_bool_t aVisible)
{
    gOpt->mDriveBar = aVisible;
}

xpr_bool_t MainFrame::isDriveBar(void) const
{
    return gOpt->mDriveBar;
}

xpr_bool_t MainFrame::isDriveShortText(void) const
{
    return gOpt->mDriveBarShortText;
}

xpr_bool_t MainFrame::isDriveViewSplit(void) const
{
    return gOpt->mDriveBarViewSplit;
}

xpr_bool_t MainFrame::isDriveViewSplitLeft(void) const
{
    return gOpt->mLeftDriveBarViewSplit;
}

void MainFrame::setDriveViewSplit(xpr_bool_t aDriveViewSplit)
{
    if (gOpt->mDriveBarViewSplit == aDriveViewSplit)
        return;

    m_wndReBar.setBandVisible(AFX_IDW_DRIVE_BAR, !aDriveViewSplit);

    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    ExplorerView *sExplorerView;

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NOT_NULL(sExplorerView) && XPR_IS_NOT_NULL(sExplorerView->m_hWnd))
            sExplorerView->visibleDrivePathBar(aDriveViewSplit);
    }

    gOpt->mDriveBar = XPR_TRUE;
    gOpt->mDriveBarViewSplit = aDriveViewSplit;
}

DriveToolBar *MainFrame::getDriveBar(void) const
{
    DriveToolBar *sDriveBar = XPR_NULL;
    if (isDriveViewSplit() == XPR_TRUE)
    {
        ExplorerView *sExplorerView = getExplorerView();
        if (XPR_IS_NOT_NULL(sExplorerView) && XPR_IS_NOT_NULL(sExplorerView->m_hWnd))
            sDriveBar = (DriveToolBar *)sExplorerView->getDrivePathBar();
    }
    else
    {
        sDriveBar = (DriveToolBar *)&m_wndReBar.mDriveToolBar;
    }

    return sDriveBar;
}

void MainFrame::setDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (!XPR_IS_RANGE(0, aIndex, 1) || XPR_IS_NULL(aPath))
        return;

    xpr_tchar_t sDriveChar = aPath[0];
    mDrivePathMap[aIndex][sDriveChar] = aPath;
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, xpr_tchar_t aDriveChar)
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return XPR_NULL;

    DrivePathMap::iterator sIterator = mDrivePathMap[aIndex].find(aDriveChar);
    if (sIterator == mDrivePathMap[aIndex].end())
        return XPR_NULL;

    return sIterator->second.c_str();
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_NULL;

    return getDrivePath(aIndex, aPath[0]);
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, xpr_uint_t aId)
{
    DriveToolBar *sDriveBar = getDriveBar();
    if (XPR_IS_NULL(sDriveBar) || XPR_IS_NULL(sDriveBar->m_hWnd))
        return XPR_NULL;

    xpr_sint_t sDriveIndex = sDriveBar->CommandToIndex(aId);
    if (sDriveIndex < 0)
    {
        //ASSERT(aIndex >= 0);
        return XPR_NULL;
    }

    xpr_tchar_t sDriveChar = sDriveBar->getButtonDrive(sDriveIndex);
    return getDrivePath(aIndex, sDriveChar);
}

void MainFrame::gotoBookmark(xpr_sint_t aBookmarkIndex)
{
    if (!XPR_IS_RANGE(0, aBookmarkIndex, MAX_BOOKMARK - 1))
        return;

    //xpr_bool_t sShiftKey = (::GetAsyncKeyState(VK_SHIFT) & 0x8000) ? XPR_TRUE : XPR_FALSE;

    fxb::BookmarkItem *sBookmarkItem = fxb::BookmarkMgr::instance().getBookmark(aBookmarkIndex);
    const xpr_tchar_t *sBookmarkPath = sBookmarkItem->mPath.c_str();
    const xpr_tchar_t *sBookmakrParam = sBookmarkItem->mParam.c_str();
    const xpr_tchar_t *sBookmarkStartup = sBookmarkItem->mStartup.c_str();
    xpr_sint_t sBookmarkShowCmd = sBookmarkItem->mShowCmd;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_bool_t sFile = XPR_FALSE;
    LPITEMIDLIST sFullPidl = sBookmarkItem->getPidl();
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        if (fxb::IsFileSystem(sFullPidl) == XPR_TRUE)
        {
            DWORD dwAttribs = GetFileAttributes(sBookmarkPath);
            if (!XPR_TEST_BITS(dwAttribs, FILE_ATTRIBUTE_DIRECTORY))
                sFile = XPR_TRUE;
        }

        if (XPR_IS_TRUE(sFile))
        {
            // The bookmark item is file.

            fxb::ExecFile(sBookmarkPath, sBookmarkStartup, sBookmakrParam, sBookmarkShowCmd);
            sResult = XPR_TRUE;
        }
        else
        {
            xpr_ulong_t sShellAttributes = SFGAO_FOLDER;
            fxb::GetItemAttributes(sFullPidl, sShellAttributes);

            if (!XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER))
            {
                // The bookmark item is virtual item such as date/time item of control panels.
                fxb::ExecFile(sFullPidl, sBookmarkStartup, sBookmakrParam, sBookmarkShowCmd);
                sResult = XPR_TRUE;
            }
            else
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sFullPidl, SHGDN_FORPARSING, sPath);

                if ((_tcslen(sPath) > 3) || (_tcslen(sPath) <= 3 && fxb::IsExistFile(sPath) == XPR_TRUE))
                {
                    // The bookmark item is folder

                    xpr_sint_t sIndex = -1;
                    //if (XPR_IS_TRUE(sShiftKey))
                    //    sIndex = -2;

                    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl(sIndex);
                    if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
                    {
                        LPITEMIDLIST sUpdateFullPidl = fxb::CopyItemIDList(sFullPidl);

                        sResult = sExplorerCtrl->explore(sUpdateFullPidl);
                        if (XPR_IS_TRUE(sResult))
                        {
                            if (XPR_IS_TRUE(gOpt->mContentsBookmarkExpandFolder) && XPR_IS_FALSE(gOpt->mSingleFolderPaneMode))
                            {
                                FolderCtrl *sFolderCtrl = getFolderCtrl(sIndex);
                                if (XPR_IS_NOT_NULL(sFolderCtrl) && XPR_IS_NOT_NULL(sFolderCtrl->m_hWnd))
                                {
                                    HTREEITEM sTreeItem = sFolderCtrl->GetSelectedItem();
                                    sFolderCtrl->Expand(sTreeItem, TVE_EXPAND);
                                }
                            }

                            if (sIndex == -2)
                            {
                                sExplorerCtrl->SetFocus();
                            }
                        }
                        else
                        {
                            COM_FREE(sUpdateFullPidl);
                        }
                    }
                }
            }
        }
    }

    COM_FREE(sFullPidl);

    if (XPR_IS_FALSE(sResult))
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH * 2 + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("bookmark.status.msg.wrong_path"), XPR_STRING_LITERAL("%s")), sBookmarkPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
}

xpr_bool_t MainFrame::isVisiblePicViewer(void) const
{
    return (mPicViewer != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::togglePicViewer(void)
{
    if (XPR_IS_NOT_NULL(mPicViewer))
    {
        DESTROY_DELETE(mPicViewer)
    }
    else
    {
        HWND sFocusHwnd = ::GetFocus();

        mPicViewer = new PicViewer;
        mPicViewer->Create(this);

        if (XPR_IS_NOT_NULL(sFocusHwnd))
        {
            ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
            {
                if (sExplorerCtrl->m_hWnd == sFocusHwnd)
                    ::SetFocus(sFocusHwnd);
            }
        }
    }
}

void MainFrame::popupBookmarkMenu(void)
{
    CToolBar *sToolBar = &m_wndReBar.mMainToolBar;
    xpr_sint_t sIndex = sToolBar->CommandToIndex(ID_GO_BOOKMARK);
    if (sIndex < 0)
    {
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            CRect sRect;
            sExplorerCtrl->GetWindowRect(&sRect);
            trackBookmarkPopupMenu(sRect.left, sRect.top);
        }
        return;
    }

    CRect sRect;
    CPoint sPoint;
    sToolBar->GetItemRect(sIndex, &sRect);
    sPoint.x = sRect.left + 5;
    sPoint.y = sRect.top  + 5;

    WPARAM sWParam = MK_LBUTTON;
    LPARAM sLParam = MAKELPARAM(sPoint.x, sPoint.y);
    sToolBar->PostMessage(WM_LBUTTONDOWN, sWParam, sLParam);
}

void MainFrame::trackBookmarkPopupMenu(xpr_sint_t x, xpr_sint_t y, LPCRECT aRect)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_GO_BOOKMARK) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        ASSERT(sPopupMenu);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, this, aRect);
        }
    }
}

void MainFrame::trackWorkingFolderPopupMenu(xpr_sint_t x, xpr_sint_t y, LPCRECT aRect)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_GO_WORKING_FOLDER) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        ASSERT(sPopupMenu);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, this, aRect);
        }
    }
}

xpr_sint_t MainFrame::addBookmark(LPITEMIDLIST aFullPidl, xpr_sint_t aInsert)
{
    if (fxb::BookmarkMgr::instance().getCount() > MAX_BOOKMARK)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("bookmark.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_BOOKMARK);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return -1;
    }

    LPITEMIDLIST sFullPidl2 = XPR_NULL;
    xpr_ulong_t sShellAttributes = fxb::GetItemAttributes(aFullPidl);
    xpr_bool_t sBookmarkItem = XPR_TEST_BITS(sShellAttributes, SFGAO_LINK);
    if (XPR_IS_TRUE(sBookmarkItem))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);
        if (fxb::ResolveShortcut(GetSafeHwnd(), sPath, &sFullPidl2) == XPR_FALSE)
            COM_FREE(sFullPidl2);
    }

    xpr_sint_t sResult = 0;
    BookmarkItemEditDlg sDlg;
    sDlg.add((XPR_IS_TRUE(sBookmarkItem) && XPR_IS_NOT_NULL(sFullPidl2)) ? sFullPidl2 : aFullPidl);
    if (sDlg.DoModal() == IDOK)
    {
        fxb::BookmarkItem *sBookmarkItem = new fxb::BookmarkItem;
        sDlg.copyBookmark(sBookmarkItem);

        fxb::BookmarkMgr &sBookmarkMgr = fxb::BookmarkMgr::instance();

        sBookmarkMgr.insertBookmark(aInsert, sBookmarkItem);

        sResult = 1;

        sBookmarkMgr.save();
    }

    COM_FREE(sFullPidl2);

    return sResult;
}

xpr_bool_t MainFrame::isUpdateBookmark(void) const
{
    if (gOpt->mContentsStyle == CONTENTS_EXPLORER && XPR_IS_TRUE(gOpt->mContentsBookmark))
        return XPR_TRUE;

    if (m_wndReBar.mBookmarkToolBar.IsVisible() == XPR_TRUE)
        return XPR_TRUE;

    return XPR_FALSE;
}

void MainFrame::updateBookmark(void)
{
    if (gOpt->mContentsStyle == CONTENTS_EXPLORER && XPR_IS_TRUE(gOpt->mContentsBookmark))
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = getViewCount();
        ExplorerView *sExplorerView;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerView = getExplorerView(i);
            if (XPR_IS_NOT_NULL(sExplorerView) && XPR_IS_NOT_NULL(sExplorerView->m_hWnd))
            {
                sExplorerView->updateBookmark(XPR_TRUE);
            }
        }
    }

    if (m_wndReBar.mBookmarkToolBar.IsVisible() == XPR_TRUE)
    {
        m_wndReBar.mBookmarkToolBar.updateBookmarkButton();
    }
}

void MainFrame::ActivateFrame(xpr_sint_t aCmdShow)
{
    //aCmdShow = SW_SHOW;
    super::ActivateFrame(aCmdShow);
}

LRESULT MainFrame::OnSingleInstance(WPARAM wParam, LPARAM lParam)
{
    fxb::SetForceForegroundWindow(m_hWnd);

    CWnd::ShowWindow((::IsIconic(m_hWnd) == XPR_TRUE) ? SW_RESTORE : SW_SHOW);
    UpdateWindow();

    return XPR_TRUE;
}

void MainFrame::ShowControlBar(CControlBar *aBar, xpr_bool_t aShow, xpr_bool_t aDelay)
{
    super::ShowControlBar(aBar, aShow, aDelay);

    if (XPR_IS_FALSE(aShow))
        SetFocus();
    else
        aBar->SetFocus();
}

xpr_bool_t MainFrame::isShowFileScrapDrop(void) const
{
    return (mFileScrapDropDlg != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::toggleFileScrapDrop(void)
{
    if (XPR_IS_NULL(mFileScrapDropDlg))
    {
        mFileScrapDropDlg = new FileScrapDropDlg;
        mFileScrapDropDlg->Create(XPR_NULL);
        mFileScrapDropDlg->ShowWindow(SW_SHOW);
    }
    else
    {
        mFileScrapDropDlg->DestroyWindow();
        delete mFileScrapDropDlg;
        mFileScrapDropDlg = XPR_NULL;
    }
}

void MainFrame::OnParentNotify(xpr_uint_t aMessage, LPARAM lParam)
{
    super::OnParentNotify(aMessage, lParam);

    // mouse backward and forward button
    switch (aMessage)
    {
    case WM_XBUTTON1DOWN: executeCommand(ID_GO_BACK);    return;
    case WM_XBUTTON2DOWN: executeCommand(ID_GO_FORWARD); return;
    }
}

void MainFrame::moveFocus(xpr_sint_t aCurWnd)
{
    moveFocus(aCurWnd, GetAsyncKeyState(VK_SHIFT) < 0, XPR_FALSE);
}

void MainFrame::moveFocus(xpr_sint_t aCurWnd, xpr_bool_t aShiftKey, xpr_bool_t aCtrlKey)
{
    aCtrlKey = XPR_FALSE;

    // aCurWnd
    // 0 - CExplroerCtrl
    // 1 - AddressBar
    // 2 - FolderCtrl

    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    getViewSplit(sRowCount, sColumnCount);

    xpr_bool_t sFocus = XPR_TRUE;
    if ((XPR_IS_TRUE (gOpt->mSplitViewTabKey) && sRowCount > 0 && sColumnCount > 0 && XPR_IS_FALSE(aCtrlKey)) ||
        (XPR_IS_FALSE(gOpt->mSplitViewTabKey) && sRowCount > 0 && sColumnCount > 0 && XPR_IS_TRUE (aCtrlKey)))
        sFocus = XPR_FALSE;

    xpr_bool_t sMovePane = XPR_TRUE;
    if (XPR_IS_TRUE(sFocus))
    {
        ExplorerView *sExplorerView = getExplorerView();
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        AddressBar   *sAddressBar   = getAddressBar();
        FolderCtrl   *sFolderCtrl   = getFolderCtrl();

        CWnd *sWnd[3];
        sWnd[0] = sExplorerCtrl;
        if (XPR_IS_TRUE(aShiftKey))
        {
            sWnd[1] = sFolderCtrl;
            sWnd[2] = sAddressBar;
        }
        else
        {
            sWnd[1] = sAddressBar;
            sWnd[2] = sFolderCtrl;
        }

        if (XPR_IS_TRUE(aShiftKey) && aCurWnd != 0)
            aCurWnd = 2 - aCurWnd + 1;

        xpr_sint_t i;
        for (i = aCurWnd + 1; i < 3; ++i)
        {
            if (XPR_IS_NOT_NULL(sWnd[i]) && sWnd[i]->IsWindowVisible() == XPR_TRUE)
            {
                sWnd[i]->SetFocus();
                sMovePane = XPR_FALSE;
                break;
            }
        }

        if (XPR_IS_TRUE(sMovePane) && sRowCount == 1 && sColumnCount == 1)
        {
            sWnd[0]->SetFocus();
            sMovePane = XPR_FALSE;
        }
    }

    if (XPR_IS_TRUE(sMovePane))
    {
        xpr_sint_t sRow = 0, sColumn = 0;
        mSplitter.getActivePane(&sRow, &sColumn);

        xpr_sint_t sViewIndex = 0;
        getViewSplitToViewIndex(sRowCount, sColumnCount, sRow, sColumn, sViewIndex);

        ++sViewIndex;
        if (sViewIndex >= (sRowCount * sColumnCount))
            sViewIndex = 0;

        getViewIndexToViewSplit(sViewIndex, sRowCount, sColumnCount, sRow, sColumn);

        mSplitter.setActivePane(sRow, sColumn);
    }
}

void MainFrame::setAccelerator(ACCEL *aAccel, xpr_sint_t aCount)
{
    if (XPR_IS_NULL(aAccel) || aCount < 0)
        return;

    // save to file
    HACCEL sAccelHandle = ::CreateAcceleratorTable(aAccel, aCount);
    if (sAccelHandle != XPR_NULL)
    {
        if (m_hAccelTable)
            ::DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = sAccelHandle;

        memcpy(mAccel, aAccel, sizeof(ACCEL) * aCount);
        mAccelCount = aCount;

        saveAccelTable();
    }

    // update SearchBar
    SearchDlg *sSearchDlg = mSearchBar.getSearchDlg();
    if (sSearchDlg != XPR_NULL && sSearchDlg->m_hWnd != XPR_NULL)
        sSearchDlg->createAccelTable();

    // reload menu bar for short key update
    m_wndMenuBar.ReloadMenu();
}

void MainFrame::loadAccelTable(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeAccel, sPath, XPR_MAX_PATH);

    if (fxb::IsExistFile(sPath) == XPR_FALSE)
    {
        xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetModuleDir(sOldPath, XPR_MAX_PATH);
        _tcscat(sOldPath, XPR_STRING_LITERAL("\\accel.fac"));

        if (fxb::IsExistFile(sOldPath) == XPR_TRUE)
            ::MoveFile(sOldPath, sPath);
    }

    ACCEL sAccel[MAX_ACCEL];
    xpr_sint_t sCount = 0;
    xpr_bool_t sSucceeded;

    AccelTable sAccelTable;
    sSucceeded = sAccelTable.loadFromFile(sPath, sAccel, &sCount, MAX_ACCEL);
    if (sSucceeded == XPR_TRUE)
    {
        sSucceeded = XPR_FALSE;

        HACCEL sAccelHandle = ::CreateAcceleratorTable(sAccel, sCount);
        if (XPR_IS_NOT_NULL(sAccelHandle))
        {
            if (XPR_IS_NOT_NULL(m_hAccelTable))
                ::DestroyAcceleratorTable(m_hAccelTable);
            m_hAccelTable = sAccelHandle;

            memcpy(mAccel, sAccel, sizeof(ACCEL) * sCount);
            mAccelCount = sCount;

            sSucceeded = XPR_TRUE;
        }
    }

    // default accelerator loading
    if (sSucceeded == XPR_FALSE)
    {
        HACCEL sAccelHandle = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
        if (XPR_IS_NOT_NULL(sAccelHandle))
        {
            if (XPR_IS_NOT_NULL(m_hAccelTable))
                ::DestroyAcceleratorTable(m_hAccelTable);
            m_hAccelTable = sAccelHandle;

            mAccelCount = ::CopyAcceleratorTable(sAccelHandle, mAccel, MAX_ACCEL);
        }
    }
}

void MainFrame::saveAccelTable(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeAccel, sPath, XPR_MAX_PATH);

    ACCEL sAccel[MAX_ACCEL];
    xpr_sint_t sCount;

    sCount = ::CopyAcceleratorTable(m_hAccelTable, sAccel, MAX_ACCEL);

    AccelTable sAccelTable;
    sAccelTable.saveToFile(sPath, sAccel, sCount);
}

void MainFrame::RecalcLayout(xpr_bool_t aNotify)
{
    CRect sClientRect(0,0,0,0);
    GetClientRect(&sClientRect);
    if (sClientRect.IsRectEmpty() == XPR_FALSE)
        super::RecalcLayout(aNotify);

    recalcLayout(SIZE_MINIMIZED);
}

void MainFrame::OnGetMinMaxInfo(MINMAXINFO *aMinMaxInfo)
{
    super::OnGetMinMaxInfo(aMinMaxInfo);
}

void MainFrame::OnTimer(xpr_uint_t aIdEvent)
{

    super::OnTimer(aIdEvent);
}

void MainFrame::setMainTitle(xpr_tchar_t *aTitle)
{
    if (XPR_IS_NULL(aTitle))
        return;

    xpr_tchar_t sTitle[XPR_MAX_PATH * 2 + 1] = {0};
    _tcscpy(sTitle, aTitle);

    _tcscat(sTitle, XPR_STRING_LITERAL(" - "));

    xpr_bool_t sElevated = XPR_FALSE;
    HRESULT sHResult = fxb::IsUACElevated(&sElevated);
    if (SUCCEEDED(sHResult) && XPR_IS_TRUE(sElevated))
    {
        _tcscat(sTitle, theApp.loadString(XPR_STRING_LITERAL("main_frame.title.admin_mode")));
        _tcscat(sTitle, XPR_STRING_LITERAL(": "));
    }

    _tcscat(sTitle, XPR_STRING_LITERAL("flyExplorer"));

    SetWindowText(sTitle);
}

void MainFrame::setMainTitle(LPITEMIDLIST aFullPidl)
{
    xpr_tchar_t sTitle[XPR_MAX_PATH * 2 + 1] = {0};
    if (XPR_IS_TRUE(gOpt->mTitleFullPath))
    {
        fxb::GetName(aFullPidl, SHGDN_FORPARSING, sTitle);
        if (sTitle[0] == XPR_STRING_LITERAL(':') && sTitle[1] == XPR_STRING_LITERAL(':'))
            fxb::GetName(aFullPidl, SHGDN_INFOLDER, sTitle);
    }
    else
    {
        fxb::GetName(aFullPidl, SHGDN_INFOLDER, sTitle);
    }

    setMainTitle(sTitle);
}

void MainFrame::setMainTitle(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return;

    xpr_tchar_t sTitle[XPR_MAX_PATH * 2 + 1] = {0};
    if (XPR_IS_TRUE(gOpt->mTitleFullPath))
    {
        fxb::GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sTitle);
        if (sTitle[0] == XPR_STRING_LITERAL(':') && sTitle[1] == XPR_STRING_LITERAL(':'))
            fxb::GetName(aShellFolder, aPidl, SHGDN_INFOLDER, sTitle);
    }
    else
    {
        fxb::GetName(aShellFolder, aPidl, SHGDN_INFOLDER, sTitle);
    }

    setMainTitle(sTitle);
}

void MainFrame::OnActivateApp(xpr_bool_t aActive, DWORD aThreadId) 
{
    super::OnActivateApp(aActive, aThreadId);
}

void MainFrame::goInitFolder(xpr_sint_t aIndex)
{
    if (aIndex < -1 || aIndex >= MAX_VIEW_SPLIT)
        return;

    xpr_sint_t i, sCount;
    LPITEMIDLIST sFullPidl;
    const xpr_tchar_t *sInitFolder;
    ExplorerCtrl *sExplorerCtrl;

    if (aIndex == -1)
    {
        i = 0;
        sCount = MAX_VIEW_SPLIT;
    }
    else
    {
        i = aIndex;
        sCount = i + 1;
    }

    for (; i < sCount; ++i)
    {
        sInitFolder = gOpt->mExplorerInitFolder[i];
        if (sInitFolder[0] == 0)
            continue;

        sExplorerCtrl = getExplorerCtrl(i);
        if (XPR_IS_NULL(sExplorerCtrl) || XPR_IS_NULL(sExplorerCtrl->m_hWnd))
            continue;

        sFullPidl = fxb::Path2Pidl(sInitFolder);

        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sExplorerCtrl->explore(sFullPidl);
        }
    }
}

void MainFrame::compareWindow(void)
{
    ExplorerCtrl *sExplorerCtrl1 = getExplorerCtrl();
    ExplorerCtrl *sExplorerCtrl2 = getExplorerCtrl(-2);

    if (XPR_IS_NULL(sExplorerCtrl1) || XPR_IS_NULL(sExplorerCtrl2))
        return;

    if (sExplorerCtrl1->isFileSystemFolder() == XPR_FALSE ||
        sExplorerCtrl2->isFileSystemFolder() == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.msg.stop_not_file_system_folder"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_tchar_t sPath[2][XPR_MAX_PATH + 1];
    sExplorerCtrl1->getCurPath(sPath[0]);
    sExplorerCtrl2->getCurPath(sPath[1]);

    mCompareDirs = new fxb::SyncDirs;
    mCompareDirs->setOwner(*this, WM_COMPARE_DIRS_STATUS);

    // scan option
    mCompareDirs->setDir(sPath[0], sPath[1]);
    mCompareDirs->setSubLevel(0);
    mCompareDirs->setExcludeExist(fxb::CompareExistNone);
    mCompareDirs->setIncludeFilter(XPR_NULL);
    mCompareDirs->setExcludeFilter(XPR_NULL);
    mCompareDirs->setExcludeAttributes(FILE_ATTRIBUTE_DIRECTORY);

    // compare option
    mCompareDirs->setDateTime(XPR_TRUE);
    mCompareDirs->setSize(XPR_TRUE);
    mCompareDirs->setAttributes(XPR_TRUE);
    mCompareDirs->setContents(fxb::CompareContentsBytes);

    mCompareDirs->scanCompare();

    WaitDlg::instance().setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.title")));
    if (WaitDlg::instance().DoModal() == IDCANCEL)
    {
        if (XPR_IS_NOT_NULL(mCompareDirs))
        {
            mCompareDirs->stop();
            XPR_SAFE_DELETE(mCompareDirs);
        }
    }
}

LRESULT MainFrame::OnCompareDirsStatus(WPARAM wParam, LPARAM lParam)
{
    fxb::SyncDirs::Status sStatus = (fxb::SyncDirs::Status)wParam;

    if (sStatus != fxb::SyncDirs::StatusCompareCompleted)
        return 0;

    if (WaitDlg::instance().isStopped())
        return 0;

    WaitDlg::instance().end();

    if (XPR_IS_NULL(mCompareDirs))
        return 0;

    if (mCompareDirs->getDiffCount() == 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.msg.equaled_folder"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        ExplorerCtrl *sExplorerCtrl[2];
        sExplorerCtrl[0] = getExplorerCtrl();
        sExplorerCtrl[1] = getExplorerCtrl(-2);

        xpr_sint_t i, j;
        xpr_sint_t sCount, sFind;
        xpr_bool_t sFirst[2];
        fxb::SyncItem *sSyncItem;
        std::tstring sDir[2];
        std::tstring sPath;
        xpr_uint_t sMask, sState;

        mCompareDirs->getDir(sDir[0], sDir[1]);

        for (i = 0; i < 2; ++i)
            sExplorerCtrl[i]->unselectAll();

        sFirst[0] = XPR_TRUE;
        sFirst[1] = XPR_TRUE;

        sCount = (xpr_sint_t)mCompareDirs->getCount();
        for (i = 0; i < sCount; ++i)
        {
            sSyncItem = mCompareDirs->getSyncItem(i);
            if (XPR_IS_NULL(sSyncItem))
                continue;

            for (j = 0; j < 2; ++j)
            {
                if (!XPR_TEST_BITS(sSyncItem->mExist, (j == 0) ? fxb::CompareExistLeft : fxb::CompareExistRight))
                    continue;

                if (XPR_TEST_BITS(sSyncItem->mExist, fxb::CompareExistEqual))
                {
                    if (!XPR_TEST_BITS(sSyncItem->mDiff, fxb::CompareDiffNotEqualed))
                        continue;
                }

                sPath  = sDir[j];
                sPath += XPR_STRING_LITERAL('\\');
                sPath += sSyncItem->mSubPath;

                sFind = sExplorerCtrl[j]->findItemPath(sPath.c_str());
                if (sFind >= 0)
                {
                    sMask  = LVIS_SELECTED;
                    sState = LVIS_SELECTED;

                    if (XPR_IS_TRUE(sFirst[j]))
                    {
                        sMask  |= LVIS_FOCUSED;
                        sState |= LVIS_FOCUSED;

                        sFirst[j] = XPR_FALSE;
                    }

                    sExplorerCtrl[j]->SetItemState(sFind, sState, sMask);
                }
            }
        }
    }

    mCompareDirs->stop();
    XPR_SAFE_DELETE(mCompareDirs);

    return 0;
}

void MainFrame::setSplitRatio(xpr_double_t aRatio)
{
    if (isSingleView() == XPR_TRUE)
        return;

    xpr_sint_t sRow = -1, sColumn = -1;
    mSplitter.getActivePane(&sRow, &sColumn);

    if (sRow < 0 || sColumn < 0)
        return;

    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    if (sRowCount < 0 || sColumnCount < 0)
        return;

    CRect sRect;
    mSplitter.getWindowRect(sRect);

    xpr_sint_t sWidth  = sRect.Width();
    xpr_sint_t sHeight = sRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    if (sRowCount == 1 && sColumnCount == 2)
    {
        xpr_double_t sXRatio = aRatio;

        if (sRow == 0 && sColumn == 1)
            sXRatio = 1.0 - sXRatio;

        gOpt->mViewSplitRatio[0] = sXRatio;
        gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mViewSplitRatio[0] + 0.5);

        mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        mSplitter.resize();
    }
    else if (sRowCount == 2 && sColumnCount == 1)
    {
        xpr_double_t sYRatio = aRatio;

        if (sRow == 1 && sColumn == 0)
            sYRatio = 1.0 - sYRatio;

        gOpt->mViewSplitRatio[0] = sYRatio;
        gOpt->mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[0] + 0.5);

        mSplitter.moveRow(0, gOpt->mViewSplitSize[0]);
        mSplitter.resize();
    }
    else if (sRowCount == 2 && sColumnCount == 2)
    {
        xpr_double_t sXRatio = aRatio;
        xpr_double_t sYRatio = aRatio;

        if (sRow == 0 && sColumn == 1)
            sXRatio = 1.0 - sXRatio;
        else if (sRow == 1 && sColumn == 0)
            sYRatio = 1.0 - sYRatio;
        else if (sRow == 1 && sColumn == 1)
        {
            sXRatio = 1.0 - sXRatio;
            sYRatio = 1.0 - sYRatio;
        }

        gOpt->mViewSplitRatio[0] = sXRatio;
        gOpt->mViewSplitRatio[1] = sYRatio;
        gOpt->mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mViewSplitRatio[0] + 0.5);
        gOpt->mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[1] + 0.5);

        mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        mSplitter.moveRow   (0, gOpt->mViewSplitSize[1]);
        mSplitter.resize();
    }
    else if (sRowCount == 1 && sColumnCount == 3)
    {
        if (sColumn == 0)
        {
            xpr_sint_t sOldPaneSize = gOpt->mViewSplitSize[0];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]);
            xpr_sint_t sDiff1 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[1] / (gOpt->mViewSplitRatio[1] + sSplitRatio2)));

            gOpt->mViewSplitSize[0]  = sNewPaneSize;
            gOpt->mViewSplitSize[1] -= sDiff1;
        }
        else if (sColumn == 1)
        {
            xpr_sint_t sOldPaneSize = gOpt->mViewSplitSize[1];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]);
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[0] / (gOpt->mViewSplitRatio[0] + sSplitRatio2)));

            gOpt->mViewSplitSize[0] -= sDiff0;
            gOpt->mViewSplitSize[1]  = sNewPaneSize;
        }
        else if (sColumn == 2)
        {
            CSize sPaneSize(0, 0);
            mSplitter.getPaneSize(0, 2, sPaneSize);

            xpr_sint_t sOldPaneSize = sPaneSize.cx;
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[0] / (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1])));

            gOpt->mViewSplitSize[0] -= sDiff0;
            gOpt->mViewSplitSize[1] -= sDiff - sDiff0;
        }

        gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;
        gOpt->mViewSplitRatio[1] = (xpr_double_t)gOpt->mViewSplitSize[1] / (xpr_double_t)sWidth;

        mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
        mSplitter.resize();
    }
    else if (sRowCount == 2 && sColumnCount == 3)
    {
        // caculate x axis
        if (sColumn == 0)
        {
            xpr_sint_t sOldPaneSize = gOpt->mViewSplitSize[0];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]);
            xpr_sint_t sDiff1 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[1] / (gOpt->mViewSplitRatio[1] + sSplitRatio2)));

            gOpt->mViewSplitSize[0]  = sNewPaneSize;
            gOpt->mViewSplitSize[1] -= sDiff1;
        }
        else if (sColumn == 1)
        {
            xpr_sint_t sOldPaneSize = gOpt->mViewSplitSize[1];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1]);
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[0] / (gOpt->mViewSplitRatio[0] + sSplitRatio2)));

            gOpt->mViewSplitSize[0] -= sDiff0;
            gOpt->mViewSplitSize[1]  = sNewPaneSize;
        }
        else if (sColumn == 2)
        {
            CSize sPaneSize(0, 0);
            mSplitter.getPaneSize(0, 2, sPaneSize);

            xpr_sint_t sOldPaneSize = sPaneSize.cx;
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mViewSplitRatio[0] / (gOpt->mViewSplitRatio[0] + gOpt->mViewSplitRatio[1])));

            gOpt->mViewSplitSize[0] -= sDiff0;
            gOpt->mViewSplitSize[1] -= sDiff - sDiff0;
        }

        gOpt->mViewSplitRatio[0] = (xpr_double_t)gOpt->mViewSplitSize[0] / (xpr_double_t)sWidth;
        gOpt->mViewSplitRatio[1] = (xpr_double_t)gOpt->mViewSplitSize[1] / (xpr_double_t)sWidth;

        // caculate y axis
        xpr_double_t sYRatio = aRatio;

        if (sRow == 1)
            sYRatio = 1.0 - sYRatio;

        gOpt->mViewSplitRatio[2] = sYRatio;
        gOpt->mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mViewSplitRatio[2] + 0.5);

        mSplitter.moveColumn(0, gOpt->mViewSplitSize[0]);
        mSplitter.moveColumn(1, gOpt->mViewSplitSize[1]);
        mSplitter.moveRow   (0, gOpt->mViewSplitSize[2]);
        mSplitter.resize();
    }
}

void MainFrame::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);

    DropTarget *sDropTarget;

    sDropTarget = m_wndReBar.mMainToolBar.getDropTarget();
    if (XPR_IS_NOT_NULL(sDropTarget))
        sDropTarget->setDragContents(aDragContents);

    sDropTarget = m_wndReBar.mBookmarkToolBar.getDropTarget();
    if (XPR_IS_NOT_NULL(sDropTarget))
        sDropTarget->setDragContents(aDragContents);
}

void MainFrame::loadControlBarState(void)
{
    BarState sBarState;
    sBarState.addBar(&mSearchBar);
    sBarState.loadBarState(this);
}

void MainFrame::saveControlBarState(void)
{
    BarState sBarState;
    sBarState.addBar(&mSearchBar);
    sBarState.saveBarState(this);
}

xpr_bool_t MainFrame::OnQueryRemove(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    std::tstring sPath;
    FolderCtrl *sFolderCtrl;
    ExplorerCtrl *sExplorerCtrl;

    RemovableDeviceObserver::DriveSet::const_iterator sIterator = aDriveSet.begin();
    for (; sIterator != aDriveSet.end(); ++sIterator)
    {
        xpr_tchar_t sDriveChar = *sIterator;
        sPath = sDriveChar; sPath += XPR_STRING_LITERAL(":\\");

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerCtrl = getExplorerCtrl(i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
                sExplorerCtrl->setChangeNotify(sPath, XPR_TRUE, XPR_FALSE);

            sFolderCtrl = getFolderCtrl(i);
            if (XPR_IS_NOT_NULL(sFolderCtrl))
                sFolderCtrl->setChangeNotify(sPath, XPR_TRUE, XPR_FALSE);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t MainFrame::OnQueryRemoveFailed(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    std::tstring sPath;
    xpr_tchar_t sDriveChar;
    FolderCtrl *sFolderCtrl;
    ExplorerCtrl *sExplorerCtrl;
    RemovableDeviceObserver::DriveSet::const_iterator sIterator;

    sIterator = aDriveSet.begin();
    for (; sIterator != aDriveSet.end(); ++sIterator)
    {
        sDriveChar = *sIterator;
        sPath = sDriveChar; sPath += XPR_STRING_LITERAL(":\\");

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerCtrl = getExplorerCtrl(i);
            if (XPR_IS_NOT_NULL(sExplorerCtrl))
                sExplorerCtrl->setChangeNotify(sPath, XPR_TRUE, XPR_TRUE);

            sFolderCtrl = getFolderCtrl(i);
            if (XPR_IS_NOT_NULL(sFolderCtrl))
                sFolderCtrl->setChangeNotify(sPath, XPR_TRUE, XPR_TRUE);
        }
    }

    return XPR_TRUE;
}

void MainFrame::OnRemoveComplete(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet)
{
}

xpr_bool_t MainFrame::onExplore(FolderView &aFolderView, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NULL(sExplorerCtrl))
        return XPR_FALSE;

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

void MainFrame::onRename(FolderView &aFolderView, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    executeCommand(ID_FILE_RENAME, XPR_NULL, &sParameters);
}

void MainFrame::onContextMenuDelete(FolderView &aFolderView, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    executeCommand(ID_FILE_DELETE, XPR_NULL, &sParameters);
}

void MainFrame::onContextMenuRename(FolderView &aFolderView, HTREEITEM aTreeItem)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeSelectedItem, aTreeItem);

    executeCommand(ID_FILE_RENAME, XPR_NULL, &sParameters);
}

void MainFrame::onContextMenuPaste(FolderView &aFolderView, HTREEITEM aTreeItem)
{
    FolderCtrl *sFolderCtrl = getFolderCtrl();
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();

    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdTreeTargetItem, aTreeItem);

    executeCommand(ID_EDIT_PASTE, sFolderCtrl, &sParameters);
}

xpr_sint_t MainFrame::onDrop(FolderView &aFolderView, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
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

void MainFrame::onMoveFocus(FolderView &aFolderView, xpr_sint_t aCurWnd)
{
    moveFocus(aCurWnd);
}

void MainFrame::onMoveFocus(ExplorerView &aExplorerView, xpr_sint_t aCurWnd)
{
    moveFocus(aCurWnd);
}

void MainFrame::OnSetPreviewMode(xpr_bool_t aPreview, CPrintPreviewState *aState)
{
    aState->nIDMainPane = getExplorerView(0)->GetDlgCtrlID();

    if (XPR_IS_TRUE(aPreview))
    {
        mOneFolderSplitter.showPane(XPR_FALSE);
        mSplitter.showPane(XPR_FALSE);
    }

    super::OnSetPreviewMode(aPreview, aState);

    mPreviewMode = aPreview;

    if (XPR_IS_FALSE(aPreview))
    {
        mOneFolderSplitter.showPane(XPR_TRUE);
        mSplitter.showPane(XPR_TRUE);

        recalcLayout();
    }
}

xpr_bool_t MainFrame::isPreviewMode(void) const
{
    return mPreviewMode;
}

void MainFrame::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint)
{

    super::OnLButtonDblClk(aFlags, aPoint);
}

void MainFrame::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.beginTracking(m_hWnd, aPoint) == XPR_FALSE)
    {
        mOneFolderSplitter.beginTracking(m_hWnd, aPoint);
    }

    super::OnLButtonDown(aFlags, aPoint);
}

void MainFrame::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.moveTracking(aPoint);

        // update new split size and ratio
        xpr_sint_t sRowCount = 0, sColumnCount = 0;
        mSplitter.getPaneCount(&sRowCount, &sColumnCount);

        xpr_sint_t i;
        xpr_sint_t sIndex = 0;
        xpr_sint_t sWidth, sHeight;
        CRect sWindowRect;
        CSize sPaneSize;

        mSplitter.getWindowRect(sWindowRect);

        sWidth  = sWindowRect.Width();
        sHeight = sWindowRect.Height();

        for (i = 0; i < (sColumnCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(0, i, sPaneSize);

            gOpt->mViewSplitSize[sIndex] = sPaneSize.cx;
            gOpt->mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mViewSplitSize[sIndex] / (xpr_double_t)sWidth;
            ++sIndex;
        }

        for (i = 0; i < (sRowCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(i, 0, sPaneSize);

            gOpt->mViewSplitSize[sIndex] = sPaneSize.cy;
            gOpt->mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mViewSplitSize[sIndex] / (xpr_double_t)sHeight;
            ++sIndex;
        }

        recalcLayout();
    }
    else if (mOneFolderSplitter.isTracking() == XPR_TRUE)
    {
        mOneFolderSplitter.moveTracking(aPoint);

        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mOneFolderSplitter.getPaneSize(0, sColumn, sPaneSize);

        gOpt->mSingleFolderPaneSize = sPaneSize.cx;

        recalcLayout();
    }
    else
    {
        mSplitter.moveTracking(aPoint);
        mOneFolderSplitter.moveTracking(aPoint);
    }

    super::OnMouseMove(aFlags, aPoint);
}

void MainFrame::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.endTracking(aPoint);

        // update new split size and ratio
        xpr_sint_t sRowCount = 0, sColumnCount = 0;
        mSplitter.getPaneCount(&sRowCount, &sColumnCount);

        xpr_sint_t i;
        xpr_sint_t sIndex = 0;
        xpr_sint_t sWidth, sHeight;
        CRect sWindowRect;
        CSize sPaneSize;

        mSplitter.getWindowRect(sWindowRect);

        sWidth  = sWindowRect.Width();
        sHeight = sWindowRect.Height();

        for (i = 0; i < (sColumnCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(0, i, sPaneSize);

            gOpt->mViewSplitSize[sIndex] = sPaneSize.cx;
            gOpt->mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mViewSplitSize[sIndex] / (xpr_double_t)sWidth;
            ++sIndex;
        }

        for (i = 0; i < (sRowCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(i, 0, sPaneSize);

            gOpt->mViewSplitSize[sIndex] = sPaneSize.cy;
            gOpt->mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mViewSplitSize[sIndex] / (xpr_double_t)sHeight;
            ++sIndex;
        }

        recalcLayout();
    }
    else if (mOneFolderSplitter.isTracking() == XPR_TRUE)
    {
        mOneFolderSplitter.endTracking(aPoint);

        // update new split size
        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mOneFolderSplitter.getPaneSize(sRow, sColumn, sPaneSize);

        gOpt->mSingleFolderPaneSize = sPaneSize.cx;

        recalcLayout();
    }

    super::OnLButtonUp(aFlags, aPoint);
}

void MainFrame::OnCaptureChanged(CWnd *aWnd)
{
    if (mSplitter.isTracking() == XPR_TRUE)
    {
        mSplitter.cancelTracking();
    }
    else if (mOneFolderSplitter.isTracking() == XPR_TRUE)
    {
        mOneFolderSplitter.cancelTracking();
    }

    super::OnCaptureChanged(aWnd);
}
