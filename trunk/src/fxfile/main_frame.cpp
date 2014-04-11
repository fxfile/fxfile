//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "main_frame.h"

#include "file_scrap.h"
#include "bookmark.h"
#include "file_op_thread.h"
#include "file_op_undo.h"
#include "sync_dirs.h"
#include "program_opts.h"
#include "shell_new.h"
#include "size_format.h"
#include "wnet_mgr.h"
#include "clip_format.h"
#include "sys_img_list.h"
#include "context_menu.h"
#include "recent_file_list.h"
#include "winapi_ex.h"
#include "keyboard.h"
#include "clipboard.h"
#include "app_ver.h"

#include "gui/FileDialogST.h"
#include "gui/SysTray.h"

#include "option.h"
#include "folder_view.h"
#include "folder_pane.h"
#include "folder_ctrl.h"
#include "explorer_view.h"
#include "explorer_pane.h"
#include "explorer_ctrl.h"
#include "search_result_ctrl.h"
#include "bookmark_item_edit_dlg.h"
#include "accel_table.h"
#include "file_scrap_drop_dlg.h"
#include "path_bar.h"
#include "address_bar.h"
#include "view_set.h"
#include "picture_viewer.h"
#include "search_dlg.h"
#include "wait_dlg.h"
#include "option_manager.h"
#include "conf_dir.h"
#include "print_preview_view_ex.h"
#include "ctrl_id.h"
#include "dlg_state_manager.h"
#include "single_process.h"

#include "cmd/router/cmd_command.h"
#include "cmd/router/cmd_context.h"
#include "cmd/router/cmd_executor.h"
#include "cmd/router/cmd_command_map.h"
#include "cmd/router/cmd_parameters.h"
#include "cmd/router/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"
#include "cmd/trash.h"

#include "command_string_table.h"

#include <htmlhelp.h>           // for Html Help
#include <wininet.h>            // for URL
#include <ExDisp.h>             // for ShellUIHelper
#include <comdef.h>
#include <Imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
MainFrame *gFrame;

namespace
{
#define DEFAULT_WINDOW_WIDTH  800
#define DEFAULT_WINDOW_HEIGHT 600

xpr_uint_t WM_TASK_RESTARTED = ::RegisterWindowMessage(XPR_STRING_LITERAL("TaskbarCreated"));
xpr_uint_t WM_SINGLE_PROCESS = SingleProcess::getMsg();

//
// user defined message
//
enum
{
    WM_TRAY_MESSAGE         = WM_USER + 1,
    WM_COMPARE_DIRS_STATUS  = WM_USER + 2,
};
} // namespace anonymous

MainFrame::MainFrame(void)
    : mExit(XPR_FALSE)
    , mSysTray(XPR_NULL)
    , mCompareDirs(XPR_NULL)
    , mPictureViewer(XPR_NULL)
    , mSearchDlg(XPR_NULL)
    , mFileScrapPane(XPR_NULL)
    , mFileScrapDropDlg(XPR_NULL)
    , mAccelCount(0)
    , mCommandExecutor(XPR_NULL)
    , mPreviewMode(XPR_FALSE)
{
    mSplitFullPidl[0] = XPR_NULL;
    mSplitFullPidl[1] = XPR_NULL;

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
    HICON sIcon = ::LoadIcon(gApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
    aCreateStruct.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, sCursor, sBrush, sIcon);
    DESTROY_CURSOR(sCursor);
    DESTROY_ICON(sIcon);

    xpr_sint_t sCmdShow = gOpt->mMain.mWindowStatus;
    CRect sWindowRect = gOpt->mMain.mWindowRect;

    xpr_bool_t sDefWindow = XPR_TRUE;

    if (sWindowRect.IsRectEmpty() == XPR_FALSE)
    {
        if ((sWindowRect.right  - sWindowRect.left) > 0 && (sWindowRect.bottom - sWindowRect.top ) > 0)
            sDefWindow = XPR_FALSE;
    }

    if (XPR_IS_FALSE(sDefWindow))
    {
        CRect sWorkAreaRect;
        GetDesktopRect(sWindowRect, sWorkAreaRect, XPR_TRUE);

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
        GetDesktopRect(sWindowRect, sDesktopRect, XPR_FALSE);

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
        GetPrimaryDesktopRect(sWorkAreaRect, XPR_TRUE);

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

    gApp.m_nCmdShow = sCmdShow;

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
    ON_REGISTERED_MESSAGE(WM_SINGLE_PROCESS, OnSingleProcess)
    ON_REGISTERED_MESSAGE(WM_TASK_RESTARTED, OnTaskRestarted)
    ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayNotify)
    ON_MESSAGE(WM_COMPARE_DIRS_STATUS, OnCompareDirsStatus)
END_MESSAGE_MAP()

void MainFrame::init(void)
{
    gFrame = this;

    SysImgListMgr::instance().getSystemImgList();

    // load file scrap
    if (XPR_IS_TRUE(gOpt->mConfig.mFileScrapSave))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getLoadPath(ConfDir::TypeFileScrap, sPath, XPR_MAX_PATH);

        FileScrap::instance().load(sPath);
    }

    // load view set
    ViewSetMgr &sViewSetMgr = ViewSetMgr::instance();
    sViewSetMgr.load();

    // set option
    setOption(*gOpt);

    ClipFormat::instance().registerClipFormat();
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

    // enable docking control bar
    EnableDocking(CBRS_ALIGN_ANY);

    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    if (mDropTarget.Register(this) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("main_frame.msg.warning_not_supported_drag_drop_clipboard"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING);
    }

    if (XPR_IS_TRUE(gOpt->mMain.mPictureViewer))
    {
        mPictureViewer = new PictureViewer;
        mPictureViewer->Create(this);

        if (gOpt->mConfig.mContentsStyle != CONTENTS_EXPLORER)
            mPictureViewer->setDocking(XPR_FALSE);
    }

    if (XPR_IS_TRUE(gOpt->mMain.mFileScrapDrop))
    {
        mFileScrapDropDlg = new FileScrapDropDlg;
        mFileScrapDropDlg->Create(this);
        mFileScrapDropDlg->ShowWindow(SW_SHOW);
    }

    RemovableDevice &sRemovableDevice = RemovableDevice::instance();
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
    xpr_sint_t sRowCount    = gOpt->mMain.mViewSplitRowCount;
    xpr_sint_t sColumnCount = gOpt->mMain.mViewSplitColumnCount;

    {
        ProgramOpts &sProgramOpts = SingletonManager::get<ProgramOpts>();

        xpr_sint_t sCmdRowCount = -1, sCmdColumnCount = -1;
        sProgramOpts.getWindowSplit(sCmdRowCount, sCmdColumnCount);

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

    mSingleFolderSplitter.setObserver(dynamic_cast<SplitterObserver *>(this));
    mSingleFolderSplitter.setMaxSplitCount(1, 2);
    mSingleFolderSplitter.setPaneSizeLimit(MIN_PANE_SIZE, ksintmax);
    mSingleFolderSplitter.setSplitSize(SPLITTER_SIZE);

    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode) && XPR_IS_TRUE(gOpt->mMain.mShowSingleFolderPane))
        mSingleFolderSplitter.split(1, 2);
    else
        mSingleFolderSplitter.split(0, 0);

    splitView(sRowCount, sColumnCount, XPR_TRUE);

    mSplitter.setActivePane(0, 0);
    mSplitter.setFocus();

    return XPR_TRUE;
}

// from SplitterObserver
CWnd *MainFrame::onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn)
{
    if (aSplitter == mSingleFolderSplitter)
    {
        if ((XPR_IS_TRUE (gOpt->mMain.mLeftSingleFolderPane) && aRow == 0 && aColumn == 0) ||
            (XPR_IS_FALSE(gOpt->mMain.mLeftSingleFolderPane) && aRow == 0 && aColumn == 1))
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
    gApp.saveAllOptions();

    destroy();
}

xpr_bool_t MainFrame::confirmToClose(xpr_bool_t aForce)
{
    if (XPR_IS_FALSE(aForce))
    {
        // Animation Minimization Tray
        if (XPR_IS_TRUE(gOpt->mConfig.mTray) && XPR_IS_TRUE(gOpt->mConfig.mTrayOnClose) && XPR_IS_FALSE(mExit))
        {
            if (XPR_IS_NOT_NULL(mSysTray))
                mSysTray->hideToTray();

            SetForegroundWindow();

            return XPR_FALSE;
        }

        // File Operation Thread Referece Counting
        xpr_sint_t sRefCount = FileOpThread::getRefCount();
        if (sRefCount > 0)
        {
            xpr_tchar_t sMsg[0xff] = {0};
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("main_frame.msg.exit_file_operation_in_progress"), XPR_STRING_LITERAL("%d")), sRefCount);
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_ICONWARNING | MB_YESNO);
            if (sMsgId != IDYES)
                return XPR_FALSE;
        }

        // Confirm Exit
        if (XPR_IS_TRUE(gOpt->mConfig.mConfirmExit))
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("main_frame.msg.confirm_exit"));
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNO);
            if (sMsgId != IDYES)
                return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

void MainFrame::saveOption(void)
{
    // save options of all of explorer views
    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    ExplorerView *sExplorerView;

    gOpt->mMain.clearView();

    for (i = 0; i < sViewCount; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            sExplorerView->saveOption();
        }
    }

    // save visible state for one folder pane
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        gOpt->mMain.mShowSingleFolderPane = isVisibleFolderPane();
    }

    // save file scrap
    if (XPR_IS_TRUE(gOpt->mConfig.mFileScrapSave))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getSavePath(ConfDir::TypeFileScrap, sPath, XPR_MAX_PATH);

        FileScrap &sFileScrap = FileScrap::instance();
        sFileScrap.save(sPath);
    }

    ViewSetMgr &sViewSetMgr = ViewSetMgr::instance();

    // verify view set
    if (XPR_IS_TRUE(gOpt->mConfig.mExplorerExitVerifyViewSet))
    {
        sViewSetMgr.verify();
    }

    // save view set
    sViewSetMgr.save();

    // save dialog states
    DlgStateManager &sDlgStateManager = DlgStateManager::instance();
    sDlgStateManager.save();

    gOpt->mMain.mPictureViewer = XPR_IS_NOT_NULL(mPictureViewer   ) && XPR_IS_NOT_NULL(mPictureViewer->m_hWnd   );
    gOpt->mMain.mFileScrapDrop = XPR_IS_NOT_NULL(mFileScrapDropDlg) && XPR_IS_NOT_NULL(mFileScrapDropDlg->m_hWnd);

    // save view split state
    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    mSplitter.getPaneCount(&sRowCount, &sColumnCount);

    gOpt->mMain.mViewSplitRowCount    = sRowCount;
    gOpt->mMain.mViewSplitColumnCount = sColumnCount;

    // save main frame position
    WINDOWPLACEMENT sWindowPlacement = {0};
    GetWindowPlacement(&sWindowPlacement);

    gOpt->mMain.mWindowStatus = sWindowPlacement.showCmd;
    gOpt->mMain.mWindowRect   = sWindowPlacement.rcNormalPosition;
}

void MainFrame::setOption(Option &aOption)
{
    // tray
    if (XPR_IS_FALSE(aOption.mConfig.mTray))
    {
        destroyTray();
    }
    else
    {
        createTray();
    }

    // set shell context menu option
    ContextMenu::setFileScrapMenu(aOption.mConfig.mFileScrapContextMenu);
    ContextMenu::setAnimationMenu(aOption.mConfig.mAnimationMenu);

    // set bookmark option
    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();
    sBookmarkMgr.setFastNetIcon(aOption.mConfig.mBookmarkFastNetIcon);

    // set task button flash when file operation complete
    FileOpThread::setCompleteFlash(aOption.mConfig.mFileOpCompleteFlash);

    // set auto refresh option
    ShellChangeNotify::setNoRefresh(aOption.mConfig.mNoRefresh);

    // set menu option
    BCMenu::setStandardMenuStyle(aOption.mConfig.mStandardMenu);
    BCMenu::setAnimationMenu(aOption.mConfig.mAnimationMenu);

    // set contents display while dragging
    setDragContents(!aOption.mConfig.mDragNoContents);
}

void MainFrame::setChangedOption(Option &aOption)
{
    // set new option
    setOption(aOption);

    // update bookmark
    updateBookmark(XPR_FALSE);

    // release docking if contents is not explorer style.
    if (aOption.mConfig.mContentsStyle != CONTENTS_EXPLORER)
    {
        if (XPR_IS_NOT_NULL(gFrame->mPictureViewer))
            mPictureViewer->setDocking(XPR_FALSE);
    }

    // apply new option to folder view
    FolderView *sFolderView = getFolderView();
    if (XPR_IS_NOT_NULL(sFolderView))
    {
        sFolderView->setChangedOption(aOption);
    }

    // apply new option to explorer view
    xpr_sint_t i;
    ExplorerView *sExplorerView;

    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        sExplorerView = getExplorerView(i);
        if (XPR_IS_NOT_NULL(sExplorerView))
        {
            sExplorerView->setChangedOption(aOption);
        }
    }
}

void MainFrame::saveAllOptions(void)
{
    // get main frame options for save
    saveOption();

    // save coolbar and toolbar
    m_wndReBar.saveStateFile();

    // save main options and config options
    OptionManager &sOptionManager = OptionManager::instance();
    sOptionManager.save();
}

void MainFrame::destroy(void)
{
    destroyTray();

    DESTROY_DELETE(mFileScrapDropDlg);
    DESTROY_DELETE(mPictureViewer);
    DESTROY_DELETE(mSearchDlg);

    xpr_sint_t i;
    for (i = 0; i < MAX_VIEW_SPLIT; ++i)
    {
        mDrivePathMap[i].clear();
    }

    m_wndReBar.DestroyWindow();

    RemovableDevice::instance().destroy();

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

        sSplitterCount = mSingleFolderSplitter.getSplitterCount();
        if (sSplitterCount > 0)
        {
            for (i = 0; i < sSplitterCount; ++i)
            {
                mSingleFolderSplitter.getSplitterRect(i, sSplitterRect);

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
    CRect sSingleFolderSplitterRect(sClientRect);
    mSingleFolderSplitter.setWindowRect(sSingleFolderSplitterRect);

    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        xpr_sint_t sPaneSize = gOpt->mMain.mSingleFolderPaneSize;
        if (XPR_IS_FALSE(gOpt->mMain.mLeftSingleFolderPane))
        {
            xpr_sint_t sSplitSize = mSingleFolderSplitter.getSplitSize();
            sPaneSize = sSingleFolderSplitterRect.Width() - sPaneSize - sSplitSize;
        }

        mSingleFolderSplitter.moveColumn(0, sPaneSize);
    }

    mSingleFolderSplitter.resize(sHdwp);

    // adjust view splitter size
    CRect sSplitterRect(sClientRect);

    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        xpr_sint_t sColumn = 1;
        if (XPR_IS_FALSE(gOpt->mMain.mLeftSingleFolderPane))
            sColumn = 0;

        mSingleFolderSplitter.getPaneRect(0, sColumn, sSplitterRect);
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
    else if (XPR_IS_TRUE(gOpt->mConfig.mViewSplitByRatio) && (sRowCount >= 1 || sColumnCount >= 1))
    {
        if (sRowCount == 1 && sColumnCount == 2)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mMain.mViewSplitRatio[0] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 1)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[0] + 0.5);

            mSplitter.moveRow(0, gOpt->mMain.mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 2)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[1]);
        }
        else if (sRowCount == 1 && sColumnCount == 3)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
        }
        else if (sRowCount == 2 && sColumnCount == 3)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);
            gOpt->mMain.mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[2] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
        }
    }
    else if (sRowCount >= 1 || sColumnCount >= 1)
    {
        if (sRowCount == 1 && sColumnCount == 2)
        {
            gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 1)
        {
            gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sHeight;

            mSplitter.moveRow(0, gOpt->mMain.mViewSplitSize[0]);
        }
        else if (sRowCount == 2 && sColumnCount == 2)
        {
            gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mMain.mViewSplitRatio[1] = (xpr_double_t)gOpt->mMain.mViewSplitSize[1] / (xpr_double_t)sHeight;

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[1]);
        }
        else if (sRowCount == 1 && sColumnCount == 3)
        {
            gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mMain.mViewSplitRatio[1] = (xpr_double_t)gOpt->mMain.mViewSplitSize[1] / (xpr_double_t)sWidth;

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
        }
        else if (sRowCount == 2 && sColumnCount == 3)
        {
            gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;
            gOpt->mMain.mViewSplitRatio[1] = (xpr_double_t)gOpt->mMain.mViewSplitSize[1] / (xpr_double_t)sWidth;
            gOpt->mMain.mViewSplitRatio[2] = (xpr_double_t)gOpt->mMain.mViewSplitSize[2] / (xpr_double_t)sHeight;

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
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
    PrintPreviewViewEx *sPrintPreviewViewEx = dynamic_cast<PrintPreviewViewEx *>(sView);
    if (XPR_IS_NOT_NULL(sPrintPreviewViewEx))
    {
        sPrintPreviewViewEx->Close();
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
    if (XPR_IS_TRUE(gOpt->mConfig.mTray) && XPR_IS_TRUE(gOpt->mConfig.mTrayOnClose) && aId == SC_CLOSE)
    {
        if (XPR_IS_NOT_NULL(mSysTray))
            mSysTray->hideToTray();

        SetForegroundWindow();
    }
    else if (XPR_IS_TRUE(gOpt->mConfig.mTrayOnMinmize) && aId == SC_MINIMIZE)
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

        xpr_sint_t sBookmarkInsert             = -1;
        xpr_sint_t sDriveInsert                = -1;
        xpr_sint_t sGoUpInsert                 = -1;
        xpr_sint_t sGoBackwardInsert           = -1;
        xpr_sint_t sGoForwardInsert            = -1;
        xpr_sint_t sGoHistoryInsert            = -1;
        xpr_sint_t sGoWorkingFolderInsert      = -1;
        xpr_sint_t sGoWorkingFolderSetInsert   = -1;
        xpr_sint_t sGoWorkingFolderResetInsert = -1;
        xpr_sint_t sShellNewInsert             = -1;
        xpr_sint_t sRecentFileListInsert       = -1;

        typedef std::deque<xpr_uint_t> CmdIdDeque;
        CmdIdDeque sDelCmdIdDeque;
        xpr_sint_t sDelCount = 0;

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

                    sString = gApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = gApp.loadString(sStringId);

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
                        _tcscat(sHotKey, GetKeyName(mAccel[j].key));
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
                FileOpUndo sFileOpUndo;
                xpr_sint_t sUndoMode = sFileOpUndo.getMode();

                const xpr_tchar_t *sText = XPR_NULL;
                switch (sUndoMode)
                {
                case MODE_COPY:   sText = gApp.loadString(XPR_STRING_LITERAL("cmd.undo.copy"));   break;
                case MODE_MOVE:   sText = gApp.loadString(XPR_STRING_LITERAL("cmd.undo.move"));   break;
                case MODE_RENAME: sText = gApp.loadString(XPR_STRING_LITERAL("cmd.undo.rename")); break;
                case MODE_TRASH:  sText = gApp.loadString(XPR_STRING_LITERAL("cmd.undo.trash"));  break;
                default:          sText = gApp.loadString(XPR_STRING_LITERAL("cmd.undo.none"));   break;
                }

                if (sText != XPR_NULL)
                    sBCPopupMenu->SetMenuText(ID_EDIT_UNDO, (xpr_tchar_t *)sText, MF_BYCOMMAND);
            }

            if (XPR_IS_RANGE(ID_DRIVE_FIRST, sId, ID_DRIVE_LAST))
            {
                if (sDriveInsert == -1)
                    sDriveInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_BOOKMARK_FIRST, sId, ID_BOOKMARK_LAST))
            {
                if (sBookmarkInsert == -1)
                    sBookmarkInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_UP_FIRST, sId, ID_GO_UP_LAST))
            {
                if (sGoUpInsert == -1)
                    sGoUpInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_BACKWARD_FIRST, sId, ID_GO_BACKWARD_LAST))
            {
                if (sGoBackwardInsert == -1)
                    sGoBackwardInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_FORWARD_FIRST, sId, ID_GO_FORWARD_LAST))
            {
                if (sGoForwardInsert == -1)
                    sGoForwardInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_HISTORY_FIRST, sId, ID_GO_HISTORY_LAST))
            {
                if (sGoHistoryInsert == -1)
                    sGoHistoryInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, sId, ID_GO_WORKING_FOLDER_LAST))
            {
                if (sGoWorkingFolderInsert == -1)
                    sGoWorkingFolderInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_SET_FIRST, sId, ID_GO_WORKING_FOLDER_SET_LAST))
            {
                if (sGoWorkingFolderSetInsert == -1)
                    sGoWorkingFolderSetInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_RESET_FIRST, sId, ID_GO_WORKING_FOLDER_RESET_LAST))
            {
                if (sGoWorkingFolderResetInsert == -1)
                    sGoWorkingFolderResetInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_FILE_NEW_FIRST, sId, ID_FILE_NEW_LAST))
            {
                if (sShellNewInsert == -1)
                    sShellNewInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (XPR_IS_RANGE(ID_FILE_RECENT_FILE_LIST_FIRST, sId, ID_FILE_RECENT_FILE_LIST_LAST))
            {
                if (sRecentFileListInsert == -1)
                    sRecentFileListInsert = i - sDelCount;

                sDelCmdIdDeque.push_back(sId);
                ++sDelCount;
            }

            if (sDriveInsert == -1)
            {
                if (sId == ID_DRIVE_DYNAMIC_MENU)
                {
                    sDriveInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sBookmarkInsert == -1)
            {
                if (sId == ID_BOOKMARK_DYNAMIC_MENU)
                {
                    sBookmarkInsert = i - sDelCount;

                    if (BookmarkMgr::instance().getCount() > 0)
                    {
                        sDelCmdIdDeque.push_back(sId);
                        ++sDelCount;
                    }
                }
            }

            if (sGoUpInsert == -1)
            {
                if (sId == ID_GO_UP_DYNAMIC_MENU)
                {
                    sGoUpInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoBackwardInsert == -1)
            {
                if (sId == ID_GO_BACKWARD_DYNAMIC_MENU)
                {
                    sGoBackwardInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoForwardInsert == -1)
            {
                if (sId == ID_GO_FORWARD_DYNAMIC_MENU)
                {
                    sGoForwardInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoHistoryInsert == -1)
            {
                if (sId == ID_GO_HISTORY_DYNAMIC_MENU)
                {
                    sGoHistoryInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoWorkingFolderInsert == -1)
            {
                if (sId == ID_GO_WORKING_FOLDER_DYNAMIC_MENU)
                {
                    sGoWorkingFolderInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoWorkingFolderSetInsert == -1)
            {
                if (sId == ID_GO_WORKING_FOLDER_SET_DYNAMIC_MENU)
                {
                    sGoWorkingFolderSetInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sGoWorkingFolderResetInsert == -1)
            {
                if (sId == ID_GO_WORKING_FOLDER_RESET_DYNAMIC_MENU)
                {
                    sGoWorkingFolderResetInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sShellNewInsert == -1)
            {
                if (sId == ID_FILE_NEW_DYNAMIC_MENU)
                {
                    sShellNewInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
                }
            }

            if (sRecentFileListInsert == -1)
            {
                if (sId == ID_FILE_RECENT_DYNAMIC_MENU)
                {
                    sRecentFileListInsert = i - sDelCount;

                    sDelCmdIdDeque.push_back(sId);
                    ++sDelCount;
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

        if (sDriveInsert                >= 0) { insertDrivePopupMenu               (sBCPopupMenu, sDriveInsert               ); }
        if (sBookmarkInsert             >= 0) { insertBookmarkPopupMenu            (sBCPopupMenu, sBookmarkInsert            ); }
        if (sGoUpInsert                 >= 0) { insertGoUpPopupMenu                (sBCPopupMenu, sGoUpInsert                ); }
        if (sGoBackwardInsert           >= 0) { insertGoBackwardPopupMenu          (sBCPopupMenu, sGoBackwardInsert          ); }
        if (sGoForwardInsert            >= 0) { insertGoForwardPopupMenu           (sBCPopupMenu, sGoForwardInsert           ); }
        if (sGoHistoryInsert            >= 0) { insertGoHistoryPopupMenu           (sBCPopupMenu, sGoHistoryInsert           ); }
        if (sGoWorkingFolderSetInsert   >= 0) { insertGoWorkingFolderSetPopupMenu  (sBCPopupMenu, sGoWorkingFolderSetInsert  ); }
        if (sGoWorkingFolderInsert      >= 0) { insertGoWorkingFolderPopupMenu     (sBCPopupMenu, sGoWorkingFolderInsert     ); }
        if (sGoWorkingFolderResetInsert >= 0) { insertGoWorkingFolderResetPopupMenu(sBCPopupMenu, sGoWorkingFolderResetInsert); }
        if (sRecentFileListInsert       >= 0) { insertRecentFileListPopupMenu      (sBCPopupMenu, sRecentFileListInsert      ); }

        if (XPR_IS_TRUE(gOpt->mConfig.mShellNewMenu) && sShellNewInsert >= 0)
        {
            insertShellNewPopupMenu(sBCPopupMenu, sShellNewInsert);
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
    GetDriveStrings(sDriveStrings);

    LPITEMIDLIST sFullPidl;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    xpr_tchar_t sTemp[XPR_MAX_PATH + 1];
    xpr_tchar_t *sColon = XPR_NULL;
    SHFILEINFO sShFileInfo = {0};
    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

    xpr_sint_t i, sCount = 0;
    xpr_sint_t nPos = 0;
    for (i = 0; ; ++i)
    {
        if (sDriveStrings[nPos] == 0)
            break;

        sDrive = sDriveStrings + nPos;
        sId = ID_DRIVE_FIRST + (*sDrive - XPR_STRING_LITERAL('A'));

        sFullPidl = fxfile::base::Pidl::create(sDrive);
        GetName(sFullPidl, SHGDN_INFOLDER, sText);
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

    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

    xpr_sint_t i, sCount;
    BookmarkItem *sBookmarkItem;
    BookmarkMgr::Result sResult;

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
    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);

    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

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
        sRemoved = fxfile::base::Pidl::removeLastItem(sFullPidl);
        if (XPR_IS_FALSE(sRemoved))
            break;

        GetName(sFullPidl, SHGDN_INFOLDER, sText);

        sIconIndex = GetItemIconIndex(sFullPidl, XPR_FALSE);
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

    const HistoryDeque *sBackwardDeque = sExplorerCtrl->getBackwardDeque();
    XPR_ASSERT(sBackwardDeque != XPR_NULL);

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    HistoryDeque::const_reverse_iterator sReverseIterator;

    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

    sCount = (xpr_sint_t)sBackwardDeque->size();
    if (sCount > gOpt->mConfig.mBackwardMenuCount)
        sCount = gOpt->mConfig.mBackwardMenuCount;

    sReverseIterator = sBackwardDeque->rbegin();
    for (i = 0; i < sCount && sReverseIterator != sBackwardDeque->rend(); ++sReverseIterator, ++i)
    {
        sFullPidl = *sReverseIterator;

        GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_BACKWARD_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);
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

    const HistoryDeque *sForwardDeque = sExplorerCtrl->getForwardDeque();
    XPR_ASSERT(sForwardDeque != XPR_NULL);

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    HistoryDeque::const_reverse_iterator sReverseIterator;

    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

    sCount = (xpr_sint_t)sForwardDeque->size();
    if (sCount > gOpt->mConfig.mBackwardMenuCount)
        sCount = gOpt->mConfig.mBackwardMenuCount;

    sReverseIterator = sForwardDeque->rbegin();
    for (i = 0; i < sCount && sReverseIterator != sForwardDeque->rend(); ++sReverseIterator, ++i)
    {
        sFullPidl = *sReverseIterator;

        GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_FORWARD_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);
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

    const HistoryDeque *sHistoryDeque = sExplorerCtrl->getHistoryDeque();
    XPR_ASSERT(sHistoryDeque != XPR_NULL);

    xpr_sint_t i, sCount, sIconIndex;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    HistoryDeque::const_reverse_iterator sReverseIterator;

    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

    sCount = (xpr_sint_t)sHistoryDeque->size();
    if (sCount > gOpt->mConfig.mHistoryMenuCount)
        sCount = gOpt->mConfig.mHistoryMenuCount;

    sReverseIterator = sHistoryDeque->rbegin();
    for (i = 0; i < sCount && sReverseIterator != sHistoryDeque->rend(); ++sReverseIterator, ++i)
    {
        sFullPidl = *sReverseIterator;

        GetName(sFullPidl, SHGDN_INFOLDER, sText);
        sIconIndex = GetItemIconIndex(sFullPidl, XPR_FALSE);

        aPopupMenu->InsertMenu(aInsert + i, MF_STRING | MF_BYPOSITION, ID_GO_HISTORY_FIRST+i, sText, &sSysImgListMgr.mSysImgList16, sIconIndex);
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
        _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.go"), XPR_STRING_LITERAL("%d,%d")), i+1, i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_FIRST+i, sText, &sToolBarImgList, 44+i);
    }

    return sCount;
}

xpr_sint_t MainFrame::insertGoWorkingFolderSetPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[0xff] = {0};

    sCount = MAX_WORKING_FOLDER;
    for (i = 0; i < sCount; ++i)
    {
        _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.set"), XPR_STRING_LITERAL("%d")), i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_SET_FIRST+i, sText);
    }

    return sCount;
}

xpr_sint_t MainFrame::insertGoWorkingFolderResetPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[0xff] = {0};

    sCount = MAX_WORKING_FOLDER;
    for (i = 0; i < sCount; ++i)
    {
        _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("cmd.working_folder.reset"), XPR_STRING_LITERAL("%d")), i+1);
        aPopupMenu->InsertMenu(aInsert++, MF_STRING | MF_BYPOSITION, ID_GO_WORKING_FOLDER_RESET_FIRST+i, sText);
    }

    return sCount;
}

xpr_sint_t MainFrame::insertRecentFileListPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    if (XPR_IS_FALSE(gOpt->mConfig.mRecentFile))
    {
        const xpr_tchar_t *sText = gApp.loadString(XPR_STRING_LITERAL("cmd.recent_files.popup.not_used"));

        aPopupMenu->InsertMenu(aInsert, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_DYNAMIC_MENU, (xpr_tchar_t *)sText);
        aPopupMenu->EnableMenuItem(aInsert, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
        return 1;
    }

    RecentFileList &sRecentFileList = RecentFileList::instance();

    xpr_size_t sCount = sRecentFileList.getFileCount();
    if (sCount == 0)
    {
        const xpr_tchar_t *sText = gApp.loadString(XPR_STRING_LITERAL("cmd.recent_files.popup.none"));

        aPopupMenu->InsertMenu(aInsert, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_DYNAMIC_MENU, (xpr_tchar_t *)sText);
        aPopupMenu->EnableMenuItem(aInsert, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
        return 1;
    }

    sCount = min(sCount, DEF_RECENT_FILE_LIST_MENU);

    xpr_size_t         i;
    const xpr_tchar_t *sPath;
    xpr_tchar_t        sDispPath[(XPR_MAX_PATH * 2) + 1];
    xpr_tchar_t        sText[XPR_MAX_PATH + 1];
    const xpr_tchar_t *sSrc;
    xpr_tchar_t       *sDest;

    for (i = 0; i < sCount; ++i)
    {
        sPath = sRecentFileList.getFile(i);
        XPR_ASSERT(sPath != XPR_NULL);

        sSrc  = sPath;
        sDest = sDispPath;

        while (*sSrc != 0)
        {
            if (*sSrc == '&')
                *sDest++ = '&';

            if (_istlead(*sSrc))
                *sDest++ = *sSrc++;

            *sDest++ = *sSrc++;
        }

        *sDest = 0;

        _stprintf(sText, XPR_STRING_LITERAL("%d %s"), i + 1, sDispPath);
        aPopupMenu->InsertMenu(aInsert + (xpr_uint_t)i, MF_STRING | MF_BYPOSITION, ID_FILE_RECENT_FILE_LIST_FIRST + i, sText);
    }

    return (xpr_sint_t)sCount;
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

FileScrapPane *MainFrame::getFileScrapPane(void) const
{
    return mFileScrapPane;
}

void MainFrame::setFileScrapPane(FileScrapPane *aFileScrapPane)
{
    mFileScrapPane = aFileScrapPane;
}

FolderView *MainFrame::getFolderView(void) const
{
    xpr_sint_t sRow = 0, sColumn = 0;
    if (XPR_IS_FALSE(gOpt->mMain.mLeftSingleFolderPane))
        sColumn = 1;

    return dynamic_cast<FolderView *>(mSingleFolderSplitter.getPaneWnd(sRow, sColumn));
}

Splitter *MainFrame::getSplitter(void) const
{
    return const_cast<Splitter *>(&mSplitter);
}

FolderPane *MainFrame::getFolderPane(xpr_sint_t aIndex) const
{
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
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
            sExplorerView = dynamic_cast<ExplorerView *>(sWnd);
        }
    }
    else if (aIndex == -2) // other focused explorer view
    {
        xpr_sint_t sRow, sColumn;
        mSplitter.getLastActivedPane(&sRow, &sColumn);

        if (sRow != -1 && sColumn != -1)
        {
            CWnd *sWnd = mSplitter.getPaneWnd(sRow, sColumn);
            sExplorerView = dynamic_cast<ExplorerView *>(sWnd);
        }
    }
    else // explicitly explorer view
    {
        xpr_sint_t sRowCount = 0, sColumnCount = 0;
        mSplitter.getPaneCount(&sRowCount, &sColumnCount);

        xpr_sint_t sRow = 0, sColumn = 0;
        getViewIndexToViewSplit(aIndex, sRowCount, sColumnCount, sRow, sColumn);

        CWnd *sWnd = mSplitter.getPaneWnd(sRow, sColumn);
        sExplorerView = dynamic_cast<ExplorerView *>(sWnd);
    }

    return sExplorerView;
}

ExplorerPane *MainFrame::getExplorerPane(xpr_sint_t aIndex) const
{
    ExplorerView *sExplorerView = getExplorerView(aIndex);
    if (XPR_IS_NULL(sExplorerView))
        return XPR_NULL;

    return sExplorerView->getExplorerPane();
}

SysTray *MainFrame::createTray(void)
{
    if (XPR_IS_NOT_NULL(mSysTray))
        return mSysTray;

    xpr_uint_t sIconId = (xpr::getOsVer() <= xpr::kOsVerWin2000) ? IDI_TRAY : IDI_TRAY_XP;
    HICON sIcon = (HICON)::LoadImage(gApp.m_hInstance, MAKEINTRESOURCE(sIconId), IMAGE_ICON, 0, 0, 0);

    mSysTray = new SysTray;
    if (mSysTray->createTray(GetSafeHwnd(), WM_TRAY_MESSAGE, IDS_TRAY_NOTIFY, FXFILE_PROGRAM_NAME, sIcon) == XPR_FALSE)
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
                if (XPR_IS_TRUE(gOpt->mConfig.mTrayOneClick))
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
                if (XPR_IS_FALSE(gOpt->mConfig.mTrayOneClick))
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
    if (XPR_IS_TRUE(gOpt->mConfig.mTray))
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

CWnd* MainFrame::GetMessageBar(void)
{
    ExplorerView *sExplorerView = getExplorerView();
    if (XPR_IS_NULL(sExplorerView))
        return XPR_NULL;

    return (CWnd *)sExplorerView->getStatusBar();
}

void MainFrame::GetMessageString(xpr_uint_t aId, CString &aMessage) const
{
    aMessage = XPR_STRING_LITERAL("");

    static xpr_tchar_t sMessage[XPR_MAX_PATH * 2 + 1];
    static xpr_tchar_t sReady[XPR_MAX_PATH * 2 + 1];
    sMessage[0] = sReady[0] = XPR_STRING_LITERAL('\0');

    if (XPR_IS_RANGE(ID_BOOKMARK_FIRST, aId, ID_BOOKMARK_LAST))
    {
        xpr_sint_t sIndex = aId - ID_BOOKMARK_FIRST;

        BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();
        aMessage = sBookmarkMgr.getBookmark(sIndex)->mPath.c_str();
    }
    else if (XPR_IS_RANGE(ID_DRIVE_FIRST, aId, ID_DRIVE_LAST))
    {
        xpr_sint_t sViewIndex = 0;
        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
            sViewIndex = sExplorerCtrl->getViewIndex();

        xpr_tchar_t sDriveChar = (aId - ID_DRIVE_FIRST) + XPR_STRING_LITERAL('A');
        const xpr_tchar_t *sDriveLastPath = getDrivePath(sViewIndex, aId);

        _stprintf(sMessage, gApp.loadFormatString(XPR_STRING_LITERAL("drive.status.desc"), XPR_STRING_LITERAL("%c")), sDriveChar);

        if (XPR_IS_NOT_NULL(sDriveLastPath) && sDriveLastPath[0] != 0)
        {
            _stprintf(sMessage + _tcslen(sMessage), gApp.loadFormatString(XPR_STRING_LITERAL("drive.status.memorized_last_folder"), XPR_STRING_LITERAL("%s")), sDriveLastPath );
        }

        aMessage = sMessage;
    }
    else if (XPR_IS_RANGE(ID_GO_UP_FIRST, aId, ID_GO_UP_LAST))
    {
        xpr_sint_t sIndex = aId - ID_GO_UP_FIRST;

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_IS_NOT_NULL(sTvItemData->mFullPidl))
            {
                LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
                if (XPR_IS_NOT_NULL(sFullPidl))
                {
                    xpr_sint_t sItemIdCount = fxfile::base::Pidl::getItemCount(sFullPidl);

                    xpr_sint_t i;
                    xpr_bool_t sRemoved;
                    for (i = sItemIdCount - 1; i >= sIndex; --i)
                    {
                        sRemoved = fxfile::base::Pidl::removeLastItem(sFullPidl);
                        if (XPR_IS_FALSE(sRemoved))
                            break;
                    }

                    GetFullPath(sFullPidl, sMessage);
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

        xpr_sint_t sIndex = aId - ID_GO_BACKWARD_FIRST;
        switch (sType)
        {
        case 1: sIndex = aId - ID_GO_FORWARD_FIRST; break;
        case 2: sIndex = aId - ID_GO_HISTORY_FIRST; break;
        }

        ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
        if (XPR_IS_NOT_NULL(sExplorerCtrl))
        {
            const HistoryDeque *sHistoryDeque = sExplorerCtrl->getBackwardDeque();
            switch (sType)
            {
            case 1: sHistoryDeque = sExplorerCtrl->getForwardDeque(); break;
            case 2: sHistoryDeque = sExplorerCtrl->getHistoryDeque(); break;
            }

            if (XPR_IS_NOT_NULL(sHistoryDeque))
            {
                HistoryDeque::const_reverse_iterator sReverseIterator = sHistoryDeque->rbegin() + sIndex;
                if (sReverseIterator != sHistoryDeque->rend())
                {
                    LPITEMIDLIST sFullPidl = *sReverseIterator;
                    if (XPR_IS_NOT_NULL(sFullPidl))
                    {
                        if (IsFileSystem(sFullPidl) == XPR_TRUE)
                            GetName(sFullPidl, SHGDN_FORPARSING, sMessage);
                        else
                            GetFullPath(sFullPidl, sMessage);

                        aMessage = sMessage;
                    }
                }
            }
        }
    }
    else if (XPR_IS_RANGE(ID_GO_WORKING_FOLDER_FIRST, aId, ID_GO_WORKING_FOLDER_LAST))
    {
        xpr_sint_t sIndex = aId - ID_GO_WORKING_FOLDER_FIRST;
        aMessage = gOpt->mMain.mWorkingFolder[sIndex];
    }
    else if (XPR_IS_RANGE(ID_FILE_RECENT_FILE_LIST_FIRST, aId, ID_FILE_RECENT_FILE_LIST_LAST))
    {
        xpr_sint_t sIndex = aId - ID_FILE_RECENT_FILE_LIST_FIRST;

        RecentFileList &sRecentFileList = RecentFileList::instance();

        const xpr_tchar_t *sPath = sRecentFileList.getFile(sIndex);
        XPR_ASSERT(sPath != XPR_NULL);

        aMessage = sPath;
    }
    else
    {
        ExplorerView *sExplorerView = getExplorerView();
        if (XPR_IS_NOT_NULL(sExplorerView))
            aMessage = sExplorerView->getStatusPaneText(0);
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
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
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
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
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
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        return gOpt->mMain.mLeftSingleFolderPane;
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
                if (XPR_IS_FALSE(gOpt->mConfig.mFolderTreeInitNoExpand))
                    sFolderCtrl->Expand(sTreeItem, TVE_EXPAND);

                sFolderCtrl->EnsureVisible(sTreeItem);
            }
        }
    }
}

void MainFrame::showFolderPane(xpr_bool_t aShow)
{
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        if (XPR_IS_TRUE(aShow))
        {
            mSingleFolderSplitter.split(1, 2);

            initFolderCtrl();
        }
        else
        {
            mSingleFolderSplitter.split(0, 0);
        }

        gOpt->mMain.mShowSingleFolderPane = aShow;

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
    return XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::setSingleFolderPaneMode(xpr_bool_t aSingleFolderPaneMode)
{
    if (gOpt->mMain.mSingleFolderPaneMode == aSingleFolderPaneMode)
        return;

    gOpt->mMain.mSingleFolderPaneMode = aSingleFolderPaneMode;

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

        mSingleFolderSplitter.split(1, 2);

        // initialize folder control for one folder pane
        initFolderCtrl();
    }
    else
    {
        mSingleFolderSplitter.split(0, 0);

        xpr_sint_t i;
        xpr_sint_t sViewCount = getViewCount();
        ExplorerView *sExplorerView;

        for (i = 0; i < sViewCount; ++i)
        {
            sExplorerView = getExplorerView(i);
            if (XPR_IS_NOT_NULL(sExplorerView))
            {
                if (XPR_IS_TRUE(gOpt->mMain.mShowEachFolderPane[i]))
                    sExplorerView->visibleFolderPane(XPR_TRUE);
            }
        }
    }

    recalcLayout();
}

void MainFrame::setNoneFolderPane(void)
{
    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
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

    if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
    {
        if (gOpt->mMain.mLeftSingleFolderPane != aLeft)
        {
            gOpt->mMain.mLeftSingleFolderPane = aLeft;

            mSingleFolderSplitter.switchPane(0, 0, 0, 1);

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

xpr_bool_t MainFrame::canExecute(xpr_uint_t aCommandId, CCmdUI *aCmdUI, CWnd *aTargetWnd, cmd::CommandParameters *aParameters)
{
    if (mCommandExecutor == XPR_NULL)
        return XPR_FALSE;

    cmd::CommandContext sCommandContext;
    sCommandContext.setCommandId(aCommandId);
    sCommandContext.setMainFrame(this);
    sCommandContext.setTargetWnd(aTargetWnd);
    sCommandContext.setParameters(aParameters);

    xpr_sint_t sCommandState = mCommandExecutor->canExecute(sCommandContext);
    if (sCommandState != cmd::Command::StateUnbinded)
    {
        if (sCommandState == cmd::Command::StateDisable)
        {
            aCmdUI->Enable(XPR_FALSE);
        }
        else
        {
            if (XPR_TEST_BITS(sCommandState, cmd::Command::StateEnable))
            {
                aCmdUI->Enable(XPR_TRUE);
            }
            else if (XPR_TEST_BITS(sCommandState, cmd::Command::StateDisable))
            {
                aCmdUI->Enable(XPR_FALSE);
            }

            if (XPR_TEST_BITS(sCommandState, cmd::Command::StateCheck) || XPR_TEST_BITS(sCommandState, cmd::Command::StateRadio))
            {
                if (XPR_TEST_BITS(sCommandState, cmd::Command::StateCheck)) aCmdUI->SetCheck(XPR_TRUE);
                if (XPR_TEST_BITS(sCommandState, cmd::Command::StateRadio)) aCmdUI->SetRadio(XPR_TRUE);
            }
            else
            {
                aCmdUI->SetCheck(XPR_FALSE);
            }
        }

        return XPR_TRUE;
    }

    return XPR_FALSE;
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
            CCmdUI *sCmdUI = (CCmdUI *)aExtra;

            if (canExecute(aId, sCmdUI, this) == XPR_TRUE)
            {
                return XPR_TRUE;
            }
        }
        else if (sCode == CN_COMMAND)
        {
            if (executeCommand(aId) == XPR_TRUE)
            {
                return XPR_TRUE;
            }
        }
    }

    if (ID_VIEW_PIC_BEGIN <= aId && aId <= ID_VIEW_PIC_END)
    {
        if (XPR_IS_NOT_NULL(mPictureViewer) && XPR_IS_NOT_NULL(mPictureViewer->m_hWnd))
        {
            static xpr_bool_t sNoRecursive = XPR_FALSE;
            if (XPR_IS_FALSE(sNoRecursive))
            {
                sNoRecursive = XPR_TRUE;
                xpr_bool_t sResult = mPictureViewer->OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
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
                BookmarkItem *sBookmarkItem;

                BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

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
        if (XPR_IS_TRUE(gOpt->mConfig.mDriveShiftKey))
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

    if (gOpt->mMain.mWorkingFolder[aIndex][0] == 0)
        return XPR_FALSE;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        LPITEMIDLIST sFullPidl = Path2Pidl(gOpt->mMain.mWorkingFolder[aIndex]);
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

    if (XPR_IS_TRUE(gOpt->mConfig.mWorkingFolderRealPath))
        sOnlyFileSystemPath = XPR_TRUE;
    else
    {
        sOnlyFileSystemPath = XPR_TRUE;

        xpr_bool_t sParentVirtualItem = IsParentVirtualItem(aFullPidl);
        if (XPR_IS_TRUE(sParentVirtualItem) && IsFileSystem(aFullPidl) == XPR_TRUE)
        {
            xpr::string sPath;
            xpr::string sFullPath;

            GetName(aFullPidl, SHGDN_FORPARSING, sPath);
            GetFullPath(aFullPidl, sFullPath);

            xpr_tchar_t sMsg[XPR_MAX_PATH * 3 + 1] = {0};
            _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("working_folder.msg.question_real_path"), XPR_STRING_LITERAL("%s,%s")), sPath.c_str(), sFullPath.c_str());
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNOCANCEL | MB_ICONQUESTION);
            if (sMsgId == IDCANCEL)
                return XPR_FALSE;

            sOnlyFileSystemPath = (sMsgId == IDYES) ? XPR_TRUE : XPR_FALSE;
        }
    }

    return Pidl2Path(aFullPidl, gOpt->mMain.mWorkingFolder[aIndex], sOnlyFileSystemPath);
}

void MainFrame::resetWorkingFolder(xpr_size_t aIndex)
{
    if (!XPR_IS_RANGE(0, aIndex, MAX_WORKING_FOLDER - 1))
        return;

    xpr_tchar_t sMsg[0xff] = {0};
    _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("working_folder.msg.question_initialize"), XPR_STRING_LITERAL("%d")), aIndex + 1);
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    gOpt->mMain.mWorkingFolder[aIndex][0] = XPR_STRING_LITERAL('\0');
}

void MainFrame::resetWorkingFolder(void)
{
    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("working_folder.msg.question_initialize_all"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    xpr_size_t i;
    for (i = 0; i < MAX_WORKING_FOLDER; ++i)
    {
        gOpt->mMain.mWorkingFolder[i][0] = XPR_STRING_LITERAL('\0');
    }
}

xpr_sint_t MainFrame::insertShellNewPopupMenu(BCMenu *aPopupMenu, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aPopupMenu))
        return 0;

    ShellNew &sShellNew = ShellNew::instance();

    sShellNew.clear();
    sShellNew.setBaseCommandId(ID_FILE_NEW_FIRST);
    sShellNew.setText(gApp.loadString(XPR_STRING_LITERAL("cmd.new.shell_item_prefix")), gApp.loadString(XPR_STRING_LITERAL("cmd.new.shell_item_suffix")));
    sShellNew.getRegShellNew();

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    ShellNew::Item *sItem;

    sCount = (xpr_sint_t)sShellNew.getCount();
    if (sCount <= 0)
        return 0;

    SysImgListMgr &sSysImgListMgr = SysImgListMgr::instance();

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
    if (XPR_IS_TRUE(gOpt->mConfig.mTrayRestoreInitFolder))
    {
        goInitFolder();
    }

    SetForegroundWindow();

    mSysTray->showFromTray();

    // minimize to tray on file menu
    if (XPR_IS_FALSE(gOpt->mConfig.mTray))
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
                gOpt->mMain.mViewSplitRatio[0] = 0.5;
                gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mMain.mViewSplitRatio[0] + 0.5);

                mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 1)
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.5;
                gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[0] + 0.5);

                mSplitter.moveRow(0, gOpt->mMain.mViewSplitSize[0]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 2)
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.5;
                gOpt->mMain.mViewSplitRatio[1] = 0.5;
                gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
                gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[1] + 0.5);

                mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
                mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[1]);
                mSplitter.resize();
            }
            else if (sRowCount == 1 && sColumnCount == 3)
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.333;
                gOpt->mMain.mViewSplitRatio[1] = 0.333;
                gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
                gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);

                mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
                mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
                mSplitter.resize();
            }
            else if (sRowCount == 2 && sColumnCount == 3)
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.333;
                gOpt->mMain.mViewSplitRatio[1] = 0.333;
                gOpt->mMain.mViewSplitRatio[2] = 0.5;
                gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
                gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);
                gOpt->mMain.mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[2] + 0.5);

                mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
                mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
                mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
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
        if (XPR_IS_TRUE(gOpt->mMain.mSingleFolderPaneMode))
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
                            mSplitFullPidl[0] = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
                    }

                    if (XPR_IS_NOT_NULL(sExplorerCtrl1))
                    {
                        LPTVITEMDATA sTvItemData = sExplorerCtrl1->getFolderData();
                        if (XPR_IS_NOT_NULL(sTvItemData))
                            mSplitFullPidl[1] = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
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
        if (gOpt->mMain.mViewSplitRatio[0] <= 0.0 || gOpt->mMain.mViewSplitRatio[0] >= 1.0)
            gOpt->mMain.mViewSplitRatio[0] = 0.5;
    }
    else if (aRowCount == 2 && aColumnCount == 1)
    {
        if (gOpt->mMain.mViewSplitRatio[0] <= 0.0 || gOpt->mMain.mViewSplitRatio[0] >= 1.0)
            gOpt->mMain.mViewSplitRatio[0] = 0.5;
    }
    else if (aRowCount == 2 && aColumnCount == 2)
    {
        if (gOpt->mMain.mViewSplitRatio[0] <= 0.0 || gOpt->mMain.mViewSplitRatio[0] >= 1.0)
            gOpt->mMain.mViewSplitRatio[0] = 0.5;

        if (gOpt->mMain.mViewSplitRatio[1] <= 0.0 || gOpt->mMain.mViewSplitRatio[1] >= 1.0)
            gOpt->mMain.mViewSplitRatio[1] = 0.5;
    }
    else if (aRowCount == 1 && aColumnCount == 3)
    {
        if (((gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]) <= 0.0) ||
            ((gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]) >= 1.0))
        {
            gOpt->mMain.mViewSplitRatio[0] = 0.333;
            gOpt->mMain.mViewSplitRatio[1] = 0.333;
        }
    }
    else if (aRowCount == 2 && aColumnCount == 3)
    {
        if (((gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]) <= 0.0) ||
            ((gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]) >= 1.0))
        {
            gOpt->mMain.mViewSplitRatio[0] = 0.333;
            gOpt->mMain.mViewSplitRatio[1] = 0.333;
        }

        if (gOpt->mMain.mViewSplitRatio[2] <= 0.0 || gOpt->mMain.mViewSplitRatio[2] >= 1.0)
            gOpt->mMain.mViewSplitRatio[2] = 0.5;
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
                gOpt->mMain.mViewSplitRatio[0] = 0.333;
                gOpt->mMain.mViewSplitRatio[1] = 0.333;
                gOpt->mMain.mViewSplitRatio[2] = 0.5;
            }
        }
        else if (aRowCount == 2 && aColumnCount == 2)
        {
            if (sOldRowCount == 2 && sOldColumnCount == 1)
            {
                gOpt->mMain.mViewSplitRatio[1] = gOpt->mMain.mViewSplitRatio[0];
            }
            else if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                     (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.5;
                gOpt->mMain.mViewSplitRatio[1] = 0.5;
                gOpt->mMain.mViewSplitRatio[2] = 0;
            }
        }
        else if ((aRowCount == 1 && aColumnCount == 2) ||
                 (aRowCount == 2 && aColumnCount == 1))
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mMain.mViewSplitRatio[0] = 0.5;
                gOpt->mMain.mViewSplitRatio[1] = 0;
                gOpt->mMain.mViewSplitRatio[2] = 0;
            }
        }
        else if (aRowCount == 1 && aColumnCount == 1)
        {
            if ((sOldRowCount == 1 && sOldColumnCount == 3) ||
                (sOldRowCount == 2 && sOldColumnCount == 3))
            {
                gOpt->mMain.mViewSplitRatio[0] = 0;
                gOpt->mMain.mViewSplitRatio[1] = 0;
                gOpt->mMain.mViewSplitRatio[2] = 0;
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
            if (XPR_IS_TRUE(gOpt->mConfig.mSingleViewSplitAsActivedView) && XPR_IS_FALSE(sActivedView00))
            {
                std::swap(mSplitFullPidl[0], mSplitFullPidl[1]);

                ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                {
                    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(mSplitFullPidl[0]);
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
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mMain.mViewSplitRatio[0] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 1)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[0] + 0.5);

            mSplitter.moveRow(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 2)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.resize();
        }
        else if (aRowCount == 1 && aColumnCount == 3)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.resize();
        }
        else if (aRowCount == 2 && aColumnCount == 3)
        {
            gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
            gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[1] + 0.5);
            gOpt->mMain.mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[2] + 0.5);

            mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
            mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
            mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
            mSplitter.resize();
        }
    }

    if (aRowCount > 1 || aColumnCount > 1)
    {
        if (XPR_IS_NOT_NULL(mPictureViewer))
            mPictureViewer->setDocking(XPR_FALSE);
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
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
        setDrivePath(sViewIndex, sPath);
    }

    // Load
    if (XPR_IS_TRUE(sCtrlKey) || XPR_IS_TRUE(gOpt->mConfig.mDriveLastFolder))
    {
        const xpr_tchar_t *sDrivePath = getDrivePath(sViewIndex, aDriveChar);
        if (XPR_IS_NOT_NULL(sDrivePath) && IsExistFile(sDrivePath) == XPR_TRUE)
        {
            _tcscpy(sPath, sDrivePath);
            sMemorize = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sMemorize))
        _stprintf(sPath, XPR_STRING_LITERAL("%c:\\"), aDriveChar);

    LPITEMIDLIST sFullPidl = XPR_NULL;
    fxfile::base::Pidl::create(sPath, sFullPidl);

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
    gOpt->mMain.mDriveBar = aVisible;
}

xpr_bool_t MainFrame::isDriveBar(void) const
{
    return gOpt->mMain.mDriveBar;
}

xpr_bool_t MainFrame::isDriveShortText(void) const
{
    return gOpt->mMain.mDriveBarShortText;
}

DriveToolBar *MainFrame::getDriveBar(void) const
{
    DriveToolBar *sDriveBar = (DriveToolBar *)&m_wndReBar.mDriveToolBar;
    return sDriveBar;
}

void MainFrame::setDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (!XPR_IS_RANGE(0, aIndex, 1) || XPR_IS_NULL(aPath))
        return;

    xpr_tchar_t sDriveChar = aPath[0];
    mDrivePathMap[aIndex][sDriveChar] = aPath;
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, xpr_tchar_t aDriveChar) const
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return XPR_NULL;

    DrivePathMap::const_iterator sIterator = mDrivePathMap[aIndex].find(aDriveChar);
    if (sIterator == mDrivePathMap[aIndex].end())
        return XPR_NULL;

    return sIterator->second.c_str();
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, const xpr_tchar_t *aPath) const
{
    if (XPR_IS_NULL(aPath))
        return XPR_NULL;

    return getDrivePath(aIndex, aPath[0]);
}

const xpr_tchar_t *MainFrame::getDrivePath(xpr_sint_t aIndex, xpr_uint_t aId) const
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

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aBookmarkIndex);
    const xpr_tchar_t *sBookmarkPath = sBookmarkItem->mPath.c_str();
    const xpr_tchar_t *sBookmakrParam = sBookmarkItem->mParam.c_str();
    const xpr_tchar_t *sBookmarkStartup = sBookmarkItem->mStartup.c_str();
    xpr_sint_t sBookmarkShowCmd = sBookmarkItem->mShowCmd;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_bool_t sFile = XPR_FALSE;
    LPITEMIDLIST sFullPidl = sBookmarkItem->getPidl();
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        if (IsFileSystem(sFullPidl) == XPR_TRUE)
        {
            DWORD dwAttribs = GetFileAttributes(sBookmarkPath);
            if (!XPR_TEST_BITS(dwAttribs, FILE_ATTRIBUTE_DIRECTORY))
                sFile = XPR_TRUE;
        }

        if (XPR_IS_TRUE(sFile))
        {
            // file system - file

            ExecFile(sBookmarkPath, sBookmarkStartup, sBookmakrParam, sBookmarkShowCmd);
            sResult = XPR_TRUE;
        }
        else
        {
            if (XPR_IS_FALSE(IsFolder(sFullPidl)))
            {
                // virtual item such as date/time item of control panels.

                ExecFile(sFullPidl, sBookmarkStartup, sBookmakrParam, sBookmarkShowCmd);
                sResult = XPR_TRUE;
            }
            else
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                GetName(sFullPidl, SHGDN_FORPARSING, sPath);

                if ((_tcslen(sPath) > 3) || (_tcslen(sPath) <= 3 && IsExistFile(sPath) == XPR_TRUE))
                {
                    // folder

                    xpr_sint_t sIndex = -1;
                    //if (XPR_IS_TRUE(sShiftKey))
                    //    sIndex = -2;

                    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl(sIndex);
                    if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
                    {
                        LPITEMIDLIST sUpdateFullPidl = fxfile::base::Pidl::clone(sFullPidl);

                        sResult = sExplorerCtrl->explore(sUpdateFullPidl);
                        if (XPR_IS_TRUE(sResult))
                        {
                            if (XPR_IS_TRUE(gOpt->mConfig.mBookmarkExpandFolder) && XPR_IS_FALSE(gOpt->mMain.mSingleFolderPaneMode))
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
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("bookmark.status.msg.wrong_path"), XPR_STRING_LITERAL("%s")), sBookmarkPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
}

xpr_bool_t MainFrame::isVisiblePictureViewer(void) const
{
    return (mPictureViewer != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::togglePictureViewer(void)
{
    if (XPR_IS_NOT_NULL(mPictureViewer))
    {
        DESTROY_DELETE(mPictureViewer)
    }
    else
    {
        HWND sFocusHwnd = ::GetFocus();

        mPictureViewer = new PictureViewer;
        mPictureViewer->Create(this);

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
    if (BookmarkMgr::instance().getCount() > MAX_BOOKMARK)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("bookmark.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_BOOKMARK);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return -1;
    }

    LPITEMIDLIST sFullPidl2 = XPR_NULL;
    xpr_ulong_t sShellAttributes = GetItemAttributes(aFullPidl);
    xpr_bool_t sBookmarkItem = XPR_TEST_BITS(sShellAttributes, SFGAO_LINK);
    if (XPR_IS_TRUE(sBookmarkItem))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        GetName(aFullPidl, SHGDN_FORPARSING, sPath);
        if (ResolveShortcut(GetSafeHwnd(), sPath, &sFullPidl2) == XPR_FALSE)
            COM_FREE(sFullPidl2);
    }

    xpr_sint_t sResult = 0;
    BookmarkItemEditDlg sDlg;
    sDlg.add((XPR_IS_TRUE(sBookmarkItem) && XPR_IS_NOT_NULL(sFullPidl2)) ? sFullPidl2 : aFullPidl);
    if (sDlg.DoModal() == IDOK)
    {
        BookmarkItem *sBookmarkItem = new BookmarkItem;
        sDlg.copyBookmark(sBookmarkItem);

        BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

        sBookmarkMgr.insertBookmark(aInsert, sBookmarkItem);

        sResult = 1;

        sBookmarkMgr.save();
    }

    COM_FREE(sFullPidl2);

    return sResult;
}

void MainFrame::updateBookmark(xpr_bool_t aAllBookmarks)
{
    if (gOpt->mConfig.mContentsStyle == CONTENTS_EXPLORER && XPR_IS_TRUE(gOpt->mConfig.mContentsBookmark))
    {
        if (XPR_IS_TRUE(aAllBookmarks))
        {
            xpr_sint_t i;
            xpr_sint_t sViewCount = getViewCount();
            ExplorerPane *sExplorerPane;

            for (i = 0; i < sViewCount; ++i)
            {
                sExplorerPane = getExplorerPane(i);
                if (XPR_IS_NOT_NULL(sExplorerPane) && XPR_IS_NOT_NULL(sExplorerPane->m_hWnd))
                {
                    sExplorerPane->updateBookmark(XPR_TRUE);
                }
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

LRESULT MainFrame::OnSingleProcess(WPARAM wParam, LPARAM lParam)
{
    SetForceForegroundWindow(m_hWnd);

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
    // 0 - ExplroerCtrl
    // 1 - AddressBar
    // 2 - FolderCtrl

    xpr_sint_t sRowCount = 0, sColumnCount = 0;
    getViewSplit(sRowCount, sColumnCount);

    xpr_bool_t sFocus = XPR_TRUE;
    if (sRowCount > 0 && sColumnCount > 0 && XPR_IS_TRUE (aCtrlKey))
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

    // reload menu bar for short key update
    m_wndMenuBar.ReloadMenu();
}

void MainFrame::loadAccelTable(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeAccel, sPath, XPR_MAX_PATH);

    if (IsExistFile(sPath) == XPR_FALSE)
    {
        xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
        GetModuleDir(sOldPath, XPR_MAX_PATH);
        _tcscat(sOldPath, XPR_STRING_LITERAL("\\accel.fac"));

        if (IsExistFile(sOldPath) == XPR_TRUE)
            ::MoveFile(sOldPath, sPath);
    }

    ACCEL sAccel[MAX_ACCEL];
    xpr_sint_t sCount = 0;
    xpr_bool_t sSucceeded;

    AccelTable sAccelTable;
    sSucceeded = sAccelTable.load(sPath, sAccel, &sCount, MAX_ACCEL);
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
        HACCEL sAccelHandle = ::LoadAccelerators(gApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
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
    ConfDir::instance().getSavePath(ConfDir::TypeAccel, sPath, XPR_MAX_PATH);

    ACCEL sAccel[MAX_ACCEL];
    xpr_sint_t sCount;

    sCount = ::CopyAcceleratorTable(m_hAccelTable, sAccel, MAX_ACCEL);

    AccelTable sAccelTable;
    sAccelTable.save(sPath, sAccel, sCount);
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

void MainFrame::OnTimer(UINT_PTR aIdEvent)
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
    HRESULT sHResult = IsUACElevated(&sElevated);
    if (SUCCEEDED(sHResult) && XPR_IS_TRUE(sElevated))
    {
        _tcscat(sTitle, gApp.loadString(XPR_STRING_LITERAL("main_frame.title.admin_mode")));
        _tcscat(sTitle, XPR_STRING_LITERAL(": "));
    }

    _tcscat(sTitle, FXFILE_PROGRAM_NAME);

    SetWindowText(sTitle);
}

void MainFrame::setMainTitle(LPITEMIDLIST aFullPidl)
{
    xpr_tchar_t sTitle[XPR_MAX_PATH * 2 + 1] = {0};
    if (XPR_IS_TRUE(gOpt->mConfig.mTitleFullPath))
    {
        GetName(aFullPidl, SHGDN_FORPARSING, sTitle);
        if (sTitle[0] == XPR_STRING_LITERAL(':') && sTitle[1] == XPR_STRING_LITERAL(':'))
            GetName(aFullPidl, SHGDN_INFOLDER, sTitle);
    }
    else
    {
        GetName(aFullPidl, SHGDN_INFOLDER, sTitle);
    }

    setMainTitle(sTitle);
}

void MainFrame::setMainTitle(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl))
        return;

    xpr_tchar_t sTitle[XPR_MAX_PATH * 2 + 1] = {0};
    if (XPR_IS_TRUE(gOpt->mConfig.mTitleFullPath))
    {
        GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sTitle);
        if (sTitle[0] == XPR_STRING_LITERAL(':') && sTitle[1] == XPR_STRING_LITERAL(':'))
            GetName(aShellFolder, aPidl, SHGDN_INFOLDER, sTitle);
    }
    else
    {
        GetName(aShellFolder, aPidl, SHGDN_INFOLDER, sTitle);
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
        sInitFolder = gOpt->mConfig.mExplorerInitFolder[i];
        if (sInitFolder[0] == 0)
            continue;

        sExplorerCtrl = getExplorerCtrl(i);
        if (XPR_IS_NULL(sExplorerCtrl) || XPR_IS_NULL(sExplorerCtrl->m_hWnd))
            continue;

        sFullPidl = Path2Pidl(sInitFolder);

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
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.msg.stop_not_file_system_folder"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_tchar_t sPath[2][XPR_MAX_PATH + 1];
    sExplorerCtrl1->getCurPath(sPath[0]);
    sExplorerCtrl2->getCurPath(sPath[1]);

    mCompareDirs = new SyncDirs;
    mCompareDirs->setOwner(*this, WM_COMPARE_DIRS_STATUS);

    // scan option
    mCompareDirs->setDir(sPath[0], sPath[1]);
    mCompareDirs->setSubLevel(0);
    mCompareDirs->setExcludeExist(CompareExistNone);
    mCompareDirs->setIncludeFilter(XPR_NULL);
    mCompareDirs->setExcludeFilter(XPR_NULL);
    mCompareDirs->setExcludeAttributes(FILE_ATTRIBUTE_DIRECTORY);

    // compare option
    mCompareDirs->setDateTime(XPR_TRUE);
    mCompareDirs->setSize(XPR_TRUE);
    mCompareDirs->setAttributes(XPR_TRUE);
    mCompareDirs->setContents(CompareContentsBytes);

    mCompareDirs->scanCompare();

    WaitDlg::instance().setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.title")));
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
    SyncDirs::Status sStatus = (SyncDirs::Status)wParam;

    if (sStatus != SyncDirs::StatusCompareCompleted)
        return 0;

    if (WaitDlg::instance().isStopped())
        return 0;

    WaitDlg::instance().end();

    if (XPR_IS_NULL(mCompareDirs))
        return 0;

    if (mCompareDirs->getDiffCount() == 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.folder_compare.msg.equaled_folder"));
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
        SyncItem *sSyncItem;
        xpr::string sDir[2];
        xpr::string sPath;
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
                if (!XPR_TEST_BITS(sSyncItem->mExist, (j == 0) ? CompareExistLeft : CompareExistRight))
                    continue;

                if (XPR_TEST_BITS(sSyncItem->mExist, CompareExistEqual))
                {
                    if (!XPR_TEST_BITS(sSyncItem->mDiff, CompareDiffNotEqualed))
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

        gOpt->mMain.mViewSplitRatio[0] = sXRatio;
        gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth * gOpt->mMain.mViewSplitRatio[0] + 0.5);

        mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        mSplitter.resize();
    }
    else if (sRowCount == 2 && sColumnCount == 1)
    {
        xpr_double_t sYRatio = aRatio;

        if (sRow == 1 && sColumn == 0)
            sYRatio = 1.0 - sYRatio;

        gOpt->mMain.mViewSplitRatio[0] = sYRatio;
        gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[0] + 0.5);

        mSplitter.moveRow(0, gOpt->mMain.mViewSplitSize[0]);
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

        gOpt->mMain.mViewSplitRatio[0] = sXRatio;
        gOpt->mMain.mViewSplitRatio[1] = sYRatio;
        gOpt->mMain.mViewSplitSize[0] = (xpr_sint_t)(sWidth  * gOpt->mMain.mViewSplitRatio[0] + 0.5);
        gOpt->mMain.mViewSplitSize[1] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[1] + 0.5);

        mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[1]);
        mSplitter.resize();
    }
    else if (sRowCount == 1 && sColumnCount == 3)
    {
        if (sColumn == 0)
        {
            xpr_sint_t sOldPaneSize = gOpt->mMain.mViewSplitSize[0];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]);
            xpr_sint_t sDiff1 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[1] / (gOpt->mMain.mViewSplitRatio[1] + sSplitRatio2)));

            gOpt->mMain.mViewSplitSize[0]  = sNewPaneSize;
            gOpt->mMain.mViewSplitSize[1] -= sDiff1;
        }
        else if (sColumn == 1)
        {
            xpr_sint_t sOldPaneSize = gOpt->mMain.mViewSplitSize[1];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]);
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[0] / (gOpt->mMain.mViewSplitRatio[0] + sSplitRatio2)));

            gOpt->mMain.mViewSplitSize[0] -= sDiff0;
            gOpt->mMain.mViewSplitSize[1]  = sNewPaneSize;
        }
        else if (sColumn == 2)
        {
            CSize sPaneSize(0, 0);
            mSplitter.getPaneSize(0, 2, sPaneSize);

            xpr_sint_t sOldPaneSize = sPaneSize.cx;
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[0] / (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1])));

            gOpt->mMain.mViewSplitSize[0] -= sDiff0;
            gOpt->mMain.mViewSplitSize[1] -= sDiff - sDiff0;
        }

        gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;
        gOpt->mMain.mViewSplitRatio[1] = (xpr_double_t)gOpt->mMain.mViewSplitSize[1] / (xpr_double_t)sWidth;

        mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
        mSplitter.resize();
    }
    else if (sRowCount == 2 && sColumnCount == 3)
    {
        // caculate x axis
        if (sColumn == 0)
        {
            xpr_sint_t sOldPaneSize = gOpt->mMain.mViewSplitSize[0];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]);
            xpr_sint_t sDiff1 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[1] / (gOpt->mMain.mViewSplitRatio[1] + sSplitRatio2)));

            gOpt->mMain.mViewSplitSize[0]  = sNewPaneSize;
            gOpt->mMain.mViewSplitSize[1] -= sDiff1;
        }
        else if (sColumn == 1)
        {
            xpr_sint_t sOldPaneSize = gOpt->mMain.mViewSplitSize[1];
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;

            xpr_double_t sSplitRatio2 = 1.0 - (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1]);
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[0] / (gOpt->mMain.mViewSplitRatio[0] + sSplitRatio2)));

            gOpt->mMain.mViewSplitSize[0] -= sDiff0;
            gOpt->mMain.mViewSplitSize[1]  = sNewPaneSize;
        }
        else if (sColumn == 2)
        {
            CSize sPaneSize(0, 0);
            mSplitter.getPaneSize(0, 2, sPaneSize);

            xpr_sint_t sOldPaneSize = sPaneSize.cx;
            xpr_sint_t sNewPaneSize = (xpr_sint_t)(sWidth * aRatio + 0.5);

            xpr_sint_t sDiff = sNewPaneSize - sOldPaneSize;
            xpr_sint_t sDiff0 = (xpr_sint_t)((xpr_double_t)sDiff * (gOpt->mMain.mViewSplitRatio[0] / (gOpt->mMain.mViewSplitRatio[0] + gOpt->mMain.mViewSplitRatio[1])));

            gOpt->mMain.mViewSplitSize[0] -= sDiff0;
            gOpt->mMain.mViewSplitSize[1] -= sDiff - sDiff0;
        }

        gOpt->mMain.mViewSplitRatio[0] = (xpr_double_t)gOpt->mMain.mViewSplitSize[0] / (xpr_double_t)sWidth;
        gOpt->mMain.mViewSplitRatio[1] = (xpr_double_t)gOpt->mMain.mViewSplitSize[1] / (xpr_double_t)sWidth;

        // caculate y axis
        xpr_double_t sYRatio = aRatio;

        if (sRow == 1)
            sYRatio = 1.0 - sYRatio;

        gOpt->mMain.mViewSplitRatio[2] = sYRatio;
        gOpt->mMain.mViewSplitSize[2] = (xpr_sint_t)(sHeight * gOpt->mMain.mViewSplitRatio[2] + 0.5);

        mSplitter.moveColumn(0, gOpt->mMain.mViewSplitSize[0]);
        mSplitter.moveColumn(1, gOpt->mMain.mViewSplitSize[1]);
        mSplitter.moveRow   (0, gOpt->mMain.mViewSplitSize[2]);
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

xpr_bool_t MainFrame::OnQueryRemove(xpr_sint_t aDeviceNumber, const RemovableDeviceObserver::DriveSet &aDriveSet)
{
    xpr_sint_t i;
    xpr_sint_t sViewCount = getViewCount();
    xpr::string sPath;
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
    xpr::string sPath;
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

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(aFullPidl);
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
        mSingleFolderSplitter.showPane(XPR_FALSE);
        mSplitter.showPane(XPR_FALSE);
    }

    super::OnSetPreviewMode(aPreview, aState);

    mPreviewMode = aPreview;

    if (XPR_IS_FALSE(aPreview))
    {
        mSingleFolderSplitter.showPane(XPR_TRUE);
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
        mSingleFolderSplitter.beginTracking(m_hWnd, aPoint);
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

            gOpt->mMain.mViewSplitSize[sIndex] = sPaneSize.cx;
            gOpt->mMain.mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mMain.mViewSplitSize[sIndex] / (xpr_double_t)sWidth;
            ++sIndex;
        }

        for (i = 0; i < (sRowCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(i, 0, sPaneSize);

            gOpt->mMain.mViewSplitSize[sIndex] = sPaneSize.cy;
            gOpt->mMain.mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mMain.mViewSplitSize[sIndex] / (xpr_double_t)sHeight;
            ++sIndex;
        }

        recalcLayout();
    }
    else if (mSingleFolderSplitter.isTracking() == XPR_TRUE)
    {
        mSingleFolderSplitter.moveTracking(aPoint);

        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mSingleFolderSplitter.getPaneSize(0, sColumn, sPaneSize);

        gOpt->mMain.mSingleFolderPaneSize = sPaneSize.cx;

        recalcLayout();
    }
    else
    {
        mSplitter.moveTracking(aPoint);
        mSingleFolderSplitter.moveTracking(aPoint);
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

            gOpt->mMain.mViewSplitSize[sIndex] = sPaneSize.cx;
            gOpt->mMain.mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mMain.mViewSplitSize[sIndex] / (xpr_double_t)sWidth;
            ++sIndex;
        }

        for (i = 0; i < (sRowCount - 1); ++i)
        {
            sPaneSize = CSize(0, 0);
            mSplitter.getPaneSize(i, 0, sPaneSize);

            gOpt->mMain.mViewSplitSize[sIndex] = sPaneSize.cy;
            gOpt->mMain.mViewSplitRatio[sIndex] = (xpr_double_t)gOpt->mMain.mViewSplitSize[sIndex] / (xpr_double_t)sHeight;
            ++sIndex;
        }

        recalcLayout();
    }
    else if (mSingleFolderSplitter.isTracking() == XPR_TRUE)
    {
        mSingleFolderSplitter.endTracking(aPoint);

        // update new split size
        xpr_sint_t sRow = 0, sColumn = 0;
        if (isLeftFolderPane() == XPR_FALSE)
            sColumn = 1;

        CSize sPaneSize(0, 0);
        mSingleFolderSplitter.getPaneSize(sRow, sColumn, sPaneSize);

        gOpt->mMain.mSingleFolderPaneSize = sPaneSize.cx;

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
    else if (mSingleFolderSplitter.isTracking() == XPR_TRUE)
    {
        mSingleFolderSplitter.cancelTracking();
    }

    super::OnCaptureChanged(aWnd);
}

xpr_bool_t MainFrame::isVisibleSearchDlg(void) const
{
    return XPR_IS_NOT_NULL(mSearchDlg) ? XPR_TRUE : XPR_FALSE;
}

void MainFrame::showSearchDlg(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(mSearchDlg))
    {
        mSearchDlg = new SearchDlg;
        mSearchDlg->setObserver(dynamic_cast<SearchDlgObserver *>(this));
        mSearchDlg->Create(this);
    }

    if (XPR_IS_NOT_NULL(aFullPidl))
        mSearchDlg->insertLocation(aFullPidl);

    mSearchDlg->ShowWindow(SW_SHOW);
}

xpr_bool_t MainFrame::insertSearchLocation(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(mSearchDlg))
        return XPR_FALSE;

    return mSearchDlg->insertLocation(aFullPidl);
}

void MainFrame::destroySearchDlg(void)
{
    DESTROY_DELETE(mSearchDlg)
}

void MainFrame::onPostNcDestroy(SearchDlg &aSearchDlg)
{
    mSearchDlg = XPR_NULL;
}
} // namespace fxfile
