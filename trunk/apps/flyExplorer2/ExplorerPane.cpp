//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ExplorerPane.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_bookmark.h"
#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_cmd_line_parser.h"
#include "fxb/fxb_program_ass.h"
#include "fxb/fxb_clip_format.h"
#include "fxb/fxb_size_format.h"
#include "fxb/fxb_recent_file_list.h"

#include "rgc/DropTarget.h"
#include "rgc/MemDC.h"

#include "TabPaneObserver.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"
#include "PathBar.h"
#include "DrivePathBar.h"
#include "AddressBar.h"
#include "ContentsWnd.h"
#include "Option.h"
#include "ExplorerStatusBar.h"
#include "TabType.h"

#include "cmd/cmd_parameters.h"
#include "cmd/cmd_parameter_define.h"
#include "cmd/cmd_clipboard.h"

#include "cmd/ParamExecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("ExplorerPane");

#define MAX_CONTENS_SHOW_SEL_ITEMS       17
#define CONTENTS_EXPLORER_STYLE_WIDTH   200
#define CONTENTS_BASIC_STYLE_HEIGHT      36

//
// control id
//
enum
{
    CTRL_ID_ADDRESS_BAR = 50,
    CTRL_ID_PATH_BAR,
    CTRL_ID_ACTIVATE_BAR,
    CTRL_ID_CONTENTS_WND,
    CTRL_ID_STATUS_BAR,
    CTRL_ID_EXPLORER_CTRL = 200,
};

class ExplorerPane::ExplorerCtrlData
{
    friend class ExplorerPane;

protected:
    ExplorerCtrlData(void)
        : mExplorerCtrl(XPR_NULL)
    {
        mStatusPane0[0] = 0;
        mStatusPane1[0] = 0;
    }

    ~ExplorerCtrlData(void)
    {
        DESTROY_DELETE(mExplorerCtrl);
    }

protected:
    ExplorerCtrl *mExplorerCtrl;

    xpr_tchar_t   mStatusPane0[XPR_MAX_PATH + 1];
    xpr_tchar_t   mStatusPane1[XPR_MAX_PATH + 1];
};

ExplorerPane::ExplorerPane(void)
    : TabPane(TabTypeExplorer)
    , mCurExplorerCtrlId(0)
    , mAddressBar(XPR_NULL), mPathBar(XPR_NULL), mDrivePathBar(XPR_NULL), mContentsWnd(XPR_NULL), mStatusBar(XPR_NULL)
    , mIsDrivePathBar(XPR_FALSE)
    , mContentsStyle(CONTENTS_NONE), mMarginRect(CRect(CONTENTS_EXPLORER_STYLE_WIDTH, 0, 0, 0))
{
    registerWindowClass(kClassName);
}

ExplorerPane::~ExplorerPane(void)
{
}

xpr_bool_t ExplorerPane::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(ExplorerPane, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_sint_t ExplorerPane::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    setContentsStyle(gOpt->mConfig.mShowInfoBar, gOpt->mConfig.mContentsStyle);
    visiblePathBar(gOpt->mConfig.mShowPathBar, XPR_TRUE);
    visibleDrivePathBar(gOpt->mConfig.mShowEachDriveBar, XPR_TRUE);
    visibleAddressBar(gOpt->mConfig.mShowAddressBar, XPR_TRUE);
    visibleStatusBar(gOpt->mConfig.mShowStatusBar, XPR_TRUE);

    // register for drag and drop
    mDropTarget.Register(this);

    setDragContents(!gOpt->mConfig.mDragNoContents);

    return 0;
}

void ExplorerPane::OnDestroy(void)
{
    DESTROY_DELETE(mAddressBar);
    DESTROY_DELETE(mPathBar);
    DESTROY_DELETE(mStatusBar);

    destroyPathBar();
    destroyDrivePathBar();
    destroyContentsWnd();
    destroyStatusBar();

    super::OnDestroy();

    mDropTarget.Revoke();
}

void ExplorerPane::setChangedOption(Option &aOption)
{
    // set contents style
    setContentsStyle(aOption.mConfig.mShowInfoBar, aOption.mConfig.mContentsStyle);
    visiblePathBar(aOption.mConfig.mShowPathBar, XPR_FALSE);
    visibleDrivePathBar(aOption.mConfig.mShowEachDriveBar, XPR_FALSE);
    visibleAddressBar(aOption.mConfig.mShowAddressBar, XPR_FALSE);
    visibleStatusBar(aOption.mConfig.mShowStatusBar, XPR_FALSE);

    // set path bar options
    if (XPR_IS_NOT_NULL(mPathBar))
    {
        mPathBar->setMode(aOption.mConfig.mPathBarRealPath);
        mPathBar->setIconDisp(aOption.mConfig.mPathBarIcon);
        mPathBar->setCustomFont(aOption.mConfig.mCustomFontText);
        mPathBar->setHighlightColor(aOption.mConfig.mPathBarHighlight, aOption.mConfig.mPathBarHighlightColor);
    }

    // set address bar options
    if (XPR_IS_NOT_NULL(mAddressBar))
    {
        mAddressBar->setAutoComplete();
        mAddressBar->setCustomFont(aOption.mConfig.mCustomFontText);

        mAddressBar->Invalidate();
        mAddressBar->UpdateWindow();
    }

    // set drive bar options
    if (XPR_IS_NOT_NULL(mDrivePathBar))
    {
        mDrivePathBar->setShortText(gOpt->mConfig.mDriveBarShortText);
    }

    // set explorer control options
    ExplorerCtrlData *sExplorerCtrlData;
    ExplorerCtrlMap::iterator sIterator;

    sIterator = mExplorerCtrlMap.begin();
    for (; sIterator != mExplorerCtrlMap.end(); ++sIterator)
    {
        sExplorerCtrlData = sIterator->second;

        XPR_ASSERT(sExplorerCtrlData != XPR_NULL);

        setExplorerOption(sExplorerCtrlData->mExplorerCtrl, aOption);

        sExplorerCtrlData->mExplorerCtrl->refresh();
    }

    // update layout
    recalcLayout();
    invalidateContentsWnd();
}

void ExplorerPane::saveOption(void)
{
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->saveOption();
    }
}

void ExplorerPane::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    if (cx > 0 && cy > 0)
        recalcLayout();
}

void ExplorerPane::recalcLayout(void)
{
    CRect sClientRect;
    GetClientRect(&sClientRect);

    const xpr_sint_t sWidth  = sClientRect.Width();
    const xpr_sint_t sHeight = sClientRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    CRect sRect(sClientRect);
    HDWP sHdwp = ::BeginDeferWindowPos((xpr_sint_t)getSubPaneCount() + 6);

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
            if (XPR_IS_TRUE(gOpt->mConfig.mDriveBarLeftSide))
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

    // explorer control
    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl) && XPR_IS_NOT_NULL(sExplorerCtrl->m_hWnd))
    {
        ::DeferWindowPos(sHdwp, *sExplorerCtrl, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
    }

    ::EndDeferWindowPos(sHdwp);
}

void ExplorerPane::OnPaint(void)
{
    CPaintDC sDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    CMemDC sMemDC(&sDC);
    sMemDC.FillSolidRect(&sClientRect, ::GetSysColor(COLOR_WINDOW));
}

xpr_bool_t ExplorerPane::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE; // because of thumbnail
    return super::OnEraseBkgnd(aDC);
}

void ExplorerPane::OnContextMenu(CWnd *aWnd, CPoint aPoint)
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

    BCMenu sMenu;
    BCMenu *sPopupMenu = XPR_NULL;

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        // Windows Explorer's background context menu
        if (GetKeyState(VK_CONTROL) < 0)
        {
            LPTVITEMDATA sTvItemData = sExplorerCtrl->getFolderData();

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

xpr_bool_t ExplorerPane::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo)
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

xpr_bool_t ExplorerPane::isSharedPane(void) const
{
    return XPR_TRUE;
}

xpr_bool_t ExplorerPane::canDuplicatePane(void) const
{
    return XPR_TRUE;
}

CWnd *ExplorerPane::newSubPane(xpr_uint_t aId)
{
    if (mExplorerCtrlMap.find(aId) != mExplorerCtrlMap.end())
        return XPR_NULL;

    ExplorerCtrlData *sExplorerCtrlData = new ExplorerCtrlData;
    if (XPR_IS_NULL(sExplorerCtrlData))
        return XPR_NULL;

    ExplorerCtrl *sExplorerCtrl = XPR_NULL;

    sExplorerCtrl = new ExplorerCtrl;
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        sExplorerCtrl->setObserver(dynamic_cast<ExplorerCtrlObserver *>(this));
        sExplorerCtrl->setViewIndex(mViewIndex);

        setExplorerOption(sExplorerCtrl, *gOpt);

        if (sExplorerCtrl->Create(this, aId, CRect(0, 0, 0, 0)) == XPR_TRUE)
        {
            // nothing to do
        }
        else
        {
            XPR_SAFE_DELETE(sExplorerCtrl);
        }
    }

    if (XPR_IS_NULL(sExplorerCtrl))
    {
        XPR_SAFE_DELETE(sExplorerCtrlData);
        return XPR_NULL;
    }

    sExplorerCtrlData->mExplorerCtrl = sExplorerCtrl;

    mExplorerCtrlMap[aId] = sExplorerCtrlData;

    return sExplorerCtrl;
}

void ExplorerPane::setExplorerOption(ExplorerCtrl *aExplorerCtrl, const Option &aOption)
{
    ExplorerCtrl::Option sOption;

    sOption.mMouseClick                       = aOption.mConfig.mMouseClick;
    sOption.mShowHiddenAttribute              = aOption.mConfig.mShowHiddenAttribute;
    sOption.mShowSystemAttribute              = aOption.mConfig.mShowSystemAttribute;
    sOption.mTooltip                          = aOption.mConfig.mTooltip;
    sOption.mTooltipWithFileName              = aOption.mConfig.mTooltipWithFileName;
    sOption.mGridLines                        = aOption.mConfig.mGridLines;
    sOption.mFullRowSelect                    = aOption.mConfig.mFullRowSelect;
    sOption.mFileExtType                      = aOption.mConfig.mFileExtType;
    sOption.mRenameExtType                    = aOption.mConfig.mRenameExtType;
    sOption.mRenameByMouse                    = aOption.mConfig.mRenameByMouse;

    sOption.mCustomFont                       = aOption.mConfig.mCustomFont;
    _tcscpy(sOption.mCustomFontText,            aOption.mConfig.mCustomFontText);

    sOption.mBkgndImage                       = aOption.mConfig.mExplorerBkgndImage[mViewIndex];
    _tcscpy(sOption.mBkgndImagePath,            aOption.mConfig.mExplorerBkgndImagePath[mViewIndex]);
    sOption.mBkgndColorType                   = aOption.mConfig.mExplorerBkgndColorType[mViewIndex];
    sOption.mBkgndColor                       = aOption.mConfig.mExplorerBkgndColor[mViewIndex];
    sOption.mTextColorType                    = aOption.mConfig.mExplorerTextColorType[mViewIndex];
    sOption.mTextColor                        = aOption.mConfig.mExplorerTextColor[mViewIndex];
    sOption.mSizeUnit                         = aOption.mConfig.mExplorerSizeUnit;
    sOption.mListType                         = aOption.mConfig.mExplorerListType;
    sOption.mParentFolder                     = aOption.mConfig.mExplorerParentFolder;
    sOption.mGoUpSelSubFolder                 = aOption.mConfig.mExplorerGoUpSelSubFolder;
    sOption.mCustomIcon                       = aOption.mConfig.mExplorerCustomIcon;
    _tcscpy(sOption.mCustomIconFile[0],         aOption.mConfig.mExplorerCustomIconFile[0]);
    _tcscpy(sOption.mCustomIconFile[1],         aOption.mConfig.mExplorerCustomIconFile[1]);
    sOption.m24HourTime                       = aOption.mConfig.mExplorer24HourTime;
    sOption.m2YearDate                        = aOption.mConfig.mExplorer2YearDate;
    sOption.mShowDrive                        = aOption.mConfig.mExplorerShowDrive;
    sOption.mShowDriveItem                    = aOption.mConfig.mExplorerShowDriveItem;
    sOption.mShowDriveSize                    = aOption.mConfig.mExplorerShowDriveSize;
    sOption.mNameCaseType                     = aOption.mConfig.mExplorerNameCaseType;
    sOption.mCreateAndEditText                = aOption.mConfig.mExplorerCreateAndEditText;
    sOption.mAutoColumnWidth                  = aOption.mConfig.mExplorerAutoColumnWidth;
    sOption.mSaveViewSet                      = aOption.mConfig.mExplorerSaveViewSet;
    sOption.mSaveViewStyle                    = aOption.mConfig.mExplorerSaveViewStyle;
    sOption.mDefaultViewStyle                 = aOption.mConfig.mExplorerDefaultViewStyle;
    sOption.mDefaultSort                      = aOption.mConfig.mExplorerDefaultSort;
    sOption.mDefaultSortOrder                 = aOption.mConfig.mExplorerDefaultSortOrder;
    sOption.mNoSort                           = aOption.mConfig.mExplorerNoSort;
    sOption.mExitVerifyViewSet                = aOption.mConfig.mExplorerExitVerifyViewSet;

    sOption.mThumbnailWidth                   = aOption.mConfig.mThumbnailWidth;
    sOption.mThumbnailHeight                  = aOption.mConfig.mThumbnailHeight;
    sOption.mThumbnailSaveCache               = aOption.mConfig.mThumbnailSaveCache;
    sOption.mThumbnailPriority                = aOption.mConfig.mThumbnailPriority;
    sOption.mThumbnailLoadByExt               = aOption.mConfig.mThumbnailLoadByExt;

    sOption.mFileScrapContextMenu             = aOption.mConfig.mFileScrapContextMenu;

    sOption.mDragType                         = aOption.mConfig.mDragType;
    sOption.mDragDist                         = aOption.mConfig.mDragDist;
    sOption.mDragScrollTime                   = aOption.mConfig.mDragScrollTime;
    sOption.mDragDefaultFileOp                = aOption.mConfig.mDragDefaultFileOp;
    sOption.mDragNoContents                   = aOption.mConfig.mDragNoContents;

    sOption.mExternalCopyFileOp               = aOption.mConfig.mExternalCopyFileOp;
    sOption.mExternalMoveFileOp               = aOption.mConfig.mExternalMoveFileOp;

    sOption.mNoRefresh                        = aOption.mConfig.mNoRefresh;
    sOption.mRefreshSort                      = aOption.mConfig.mRefreshSort;

    sOption.mHistoryCount                     = aOption.mConfig.mHistoryCount;
    sOption.mBackwardCount                    = aOption.mConfig.mBackwardCount;

    aExplorerCtrl->setOption(sOption);
}

void ExplorerPane::destroySubPane(xpr_uint_t aId)
{
    ExplorerCtrlMap::iterator sIterator = mExplorerCtrlMap.find(aId);
    if (sIterator == mExplorerCtrlMap.end())
        return;

    ExplorerCtrlData *sExplorerCtrlData = sIterator->second;
    XPR_SAFE_DELETE(sExplorerCtrlData);

    mExplorerCtrlMap.erase(sIterator);
}

void ExplorerPane::destroySubPane(void)
{
    ExplorerCtrlData *sExplorerCtrlData;
    ExplorerCtrlMap::iterator sIterator;

    sIterator = mExplorerCtrlMap.begin();
    for (; sIterator != mExplorerCtrlMap.end(); ++sIterator)
    {
        sExplorerCtrlData = sIterator->second;
        XPR_SAFE_DELETE(sExplorerCtrlData);
    }

    mExplorerCtrlMap.clear();
}

xpr_size_t ExplorerPane::getSubPaneCount(void) const
{
    return mExplorerCtrlMap.size();
}

CWnd *ExplorerPane::getSubPane(xpr_uint_t aId) const
{
    return getExplorerCtrl(aId);
}

ExplorerCtrl *ExplorerPane::getExplorerCtrl(xpr_uint_t aId) const
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData(aId);
    if (XPR_IS_NULL(sExplorerCtrlData))
        return XPR_NULL;

    return sExplorerCtrlData->mExplorerCtrl;
}

ExplorerPane::ExplorerCtrlData *ExplorerPane::getExplorerCtrlData(xpr_uint_t aId) const
{
    xpr_uint_t sId = aId;
    if (sId == InvalidId)
        sId = mCurExplorerCtrlId;

    ExplorerCtrlMap::const_iterator sIterator = mExplorerCtrlMap.find(sId);
    if (sIterator == mExplorerCtrlMap.end())
        return XPR_NULL;

    return sIterator->second;
}

xpr_uint_t ExplorerPane::getCurSubPaneId(void) const
{
    return mCurExplorerCtrlId;
}

xpr_uint_t ExplorerPane::setCurSubPane(xpr_uint_t aId)
{
    xpr_uint_t sOldId = mCurExplorerCtrlId;

    mCurExplorerCtrlId = aId;

    ExplorerCtrl *sOldExplorerCtrl = getExplorerCtrl(sOldId);
    if (XPR_IS_NOT_NULL(sOldExplorerCtrl))
    {
        sOldExplorerCtrl->ShowWindow(SW_HIDE);
    }

    ExplorerCtrl *sNewExplorerCtrl = getExplorerCtrl(mCurExplorerCtrlId);
    if (XPR_IS_NOT_NULL(sNewExplorerCtrl))
    {
        LPTVITEMDATA sTvItemData = sNewExplorerCtrl->getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
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
        }

        sNewExplorerCtrl->ShowWindow(SW_SHOW);
    }

    recalcLayout();

    return sOldId;
}

AddressBar *ExplorerPane::getAddressBar(void) const
{
    return mAddressBar;
}

PathBar *ExplorerPane::getPathBar(void) const
{
    return mPathBar;
}

void ExplorerPane::visibleAddressBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
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

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerPane::isVisibleAddressBar(void) const
{
    return gOpt->mConfig.mShowAddressBar;
}

xpr_bool_t ExplorerPane::createAddressBar(void)
{
    if (XPR_IS_NOT_NULL(mAddressBar))
        return XPR_TRUE;

    mAddressBar = new AddressBar;
    if (XPR_IS_NOT_NULL(mAddressBar))
    {
        mAddressBar->setObserver(dynamic_cast<AddressBarObserver *>(this));

        if (mAddressBar->Create(this, CTRL_ID_ADDRESS_BAR, CRect(0,0,0,300)) == XPR_TRUE)
        {
            mAddressBar->setSystemImageList(&fxb::SysImgListMgr::instance().mSysImgList16);
            mAddressBar->setCustomFont(gOpt->mConfig.mCustomFontText);
        }
        else
        {
            XPR_SAFE_DELETE(mAddressBar);
        }
    }

    return XPR_IS_NOT_NULL(mAddressBar) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerPane::destroyAddressBar(void)
{
    if (XPR_IS_NULL(mAddressBar))
        return;

    DESTROY_DELETE(mAddressBar);
}

xpr_bool_t ExplorerPane::createPathBar(void)
{
    if (XPR_IS_NOT_NULL(mPathBar))
        return XPR_TRUE;

    mPathBar = new PathBar;
    if (XPR_IS_NOT_NULL(mPathBar))
    {
        mPathBar->setObserver(dynamic_cast<PathBarObserver *>(this));

        if (mPathBar->Create(this, CTRL_ID_PATH_BAR, CRect(0,0,0,0)) == XPR_TRUE)
        {
            mPathBar->setIconDisp(gOpt->mConfig.mPathBarIcon);
            mPathBar->setCustomFont(gOpt->mConfig.mCustomFontText);
            mPathBar->setHighlightColor(gOpt->mConfig.mPathBarHighlight, gOpt->mConfig.mPathBarHighlightColor);
            mPathBar->setMode(gOpt->mConfig.mPathBarRealPath);
        }
        else
        {
            XPR_SAFE_DELETE(mPathBar);
        }
    }

    return XPR_IS_NOT_NULL(mPathBar) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerPane::destroyPathBar(void)
{
    if (XPR_IS_NULL(mPathBar))
        return;

    DESTROY_DELETE(mPathBar);
}

void ExplorerPane::visiblePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
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

    if (XPR_IS_FALSE(aLoading))
    {
        recalcLayout();
        RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
    }
}

xpr_bool_t ExplorerPane::isVisiblePathBar(void) const
{
    return gOpt->mConfig.mShowPathBar;
}

xpr_bool_t ExplorerPane::createDrivePathBar(void)
{
    if (XPR_IS_NOT_NULL(mDrivePathBar))
        return XPR_TRUE;

    mDrivePathBar = new DrivePathBar;
    if (XPR_IS_NOT_NULL(mDrivePathBar))
    {
        if (mDrivePathBar->CreateEx(this) == XPR_TRUE)
        {
            mDrivePathBar->setShortText(gOpt->mConfig.mDriveBarShortText);
            mDrivePathBar->createDriveBar();
        }
        else
        {
            XPR_SAFE_DELETE(mDrivePathBar);
        }
    }

    return XPR_IS_NOT_NULL(mDrivePathBar) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerPane::destroyDrivePathBar(void)
{
    if (XPR_IS_NULL(mDrivePathBar))
        return;

    mDrivePathBar->destroyDriveBar();
    DESTROY_DELETE(mDrivePathBar);
}

DrivePathBar *ExplorerPane::getDrivePathBar(void) const
{
    return mDrivePathBar;
}

void ExplorerPane::visibleDrivePathBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
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

void ExplorerPane::refreshDrivePathBar(void)
{
    if (XPR_IS_NOT_NULL(mDrivePathBar))
    {
        mDrivePathBar->refresh();
    }
}

xpr_bool_t ExplorerPane::createContentsWnd(void)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        return XPR_TRUE;

    mContentsWnd = new ContentsWnd;
    if (XPR_IS_NOT_NULL(mContentsWnd))
    {
        if (mContentsWnd->Create(this, CTRL_ID_CONTENTS_WND, CRect(0,0,0,0)) == XPR_TRUE)
        {
            // nothing to do
        }
        else
        {
            XPR_SAFE_DELETE(mContentsWnd);
        }
    }

    return XPR_IS_NOT_NULL(mContentsWnd) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerPane::destroyContentsWnd(void)
{
    DESTROY_DELETE(mContentsWnd);
}

xpr_bool_t ExplorerPane::isVisibleContentsWnd(void) const
{
    return (mContentsWnd != XPR_NULL);
}

void ExplorerPane::setContentsStyle(xpr_bool_t aShowInfoBar, xpr_sint_t aContentsStyle)
{
    if (XPR_IS_TRUE(aShowInfoBar))
    {
        switch (aContentsStyle)
        {
        case CONTENTS_EXPLORER:
            {
                if (mContentsStyle != aContentsStyle)
                {
                    createContentsWnd();

                    mMarginRect = CRect(CONTENTS_EXPLORER_STYLE_WIDTH, 0, 0, 0);
                }

                ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
                if (XPR_IS_NOT_NULL(sExplorerCtrl))
                    sExplorerCtrl->resetStatus();

                updateBookmark();
                mContentsWnd->setBookmarkOption(gOpt->mConfig.mContentsBookmarkColor, gOpt->mConfig.mBookmarkTooltip);
                enableBookmark(gOpt->mConfig.mContentsBookmark);

                break;
            }

        case CONTENTS_BASIC:
            {
                if (mContentsStyle != aContentsStyle)
                {
                    createContentsWnd();

                    mMarginRect = CRect(0, CONTENTS_BASIC_STYLE_HEIGHT, 0, 0);
                }

                updateBookmark();
                mContentsWnd->setBookmarkOption(XPR_FALSE, XPR_FALSE);
                enableBookmark(XPR_FALSE);

                break;
            }

        default:
            {
                aShowInfoBar = XPR_FALSE;
                break;
            }
        }
    }

    if (XPR_IS_FALSE(aShowInfoBar))
    {
        aContentsStyle = CONTENTS_NONE;

        destroyContentsWnd();

        mMarginRect = CRect(0, 0, 0, 0);
    }

    mContentsStyle = aContentsStyle;
}

void ExplorerPane::setContentsFolder(LPTVITEMDATA aTvItemData)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsFolder(aTvItemData);
}

void ExplorerPane::setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsNormal(aTvItemData, aUpdate);
}

void ExplorerPane::setContentsSingleItem(LPLVITEMDATA       aLvItemData,
                                         const xpr_tchar_t *aName,
                                         const xpr_tchar_t *aType,
                                         const xpr_tchar_t *aDate,
                                         const xpr_tchar_t *aSize,
                                         const xpr_tchar_t *aAttr)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsSingleItem(aLvItemData, aName, aType, aDate, aSize, aAttr);
}

void ExplorerPane::setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->setContentsMultiItem(aCount, aSize, aNames);
}

void ExplorerPane::updateBookmark(xpr_bool_t aUpdatePosition)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
    {
        mContentsWnd->updateBookmark();

        if (XPR_IS_TRUE(aUpdatePosition))
        {
            mContentsWnd->updateBookmarkPosition();
            mContentsWnd->enableBookmark(mContentsWnd->isEnabledBookmark());
        }
    }
}

void ExplorerPane::enableBookmark(xpr_bool_t aEnable)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->enableBookmark(aEnable);
}

xpr_bool_t ExplorerPane::isEnabledBookmark(void) const
{
    if (XPR_IS_NULL(mContentsWnd))
        return XPR_FALSE;

    return mContentsWnd->isEnabledBookmark();
}

void ExplorerPane::invalidateContentsWnd(void)
{
    if (XPR_IS_NOT_NULL(mContentsWnd))
        mContentsWnd->Invalidate(XPR_TRUE);
}

void ExplorerPane::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);
}

xpr_bool_t ExplorerPane::createStatusBar(void)
{
    if (XPR_IS_NOT_NULL(mStatusBar))
        return XPR_TRUE;

    mStatusBar = new ExplorerStatusBar;
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        mStatusBar->setObserver(dynamic_cast<StatusBarObserver *>(this));

        if (mStatusBar->Create(this, CTRL_ID_STATUS_BAR, CRect(0,0,0,0)) == XPR_TRUE)
        {
        }
        else
        {
            XPR_SAFE_DELETE(mStatusBar);
        }
    }

    return XPR_IS_NOT_NULL(mStatusBar) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerPane::destroyStatusBar(void)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    DESTROY_DELETE(mStatusBar);
}

void ExplorerPane::visibleStatusBar(xpr_bool_t aVisible, xpr_bool_t aLoading)
{
    if (XPR_IS_TRUE(aVisible))
    {
        createStatusBar();
    }
    else
    {
        destroyStatusBar();
    }

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

xpr_bool_t ExplorerPane::isVisibleStatusBar(void) const
{
    return gOpt->mConfig.mShowStatusBar;
}

StatusBar *ExplorerPane::getStatusBar(void) const
{
    return mStatusBar;
}

void ExplorerPane::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    ExplorerCtrl *sExplorerCtrl = getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onSetFocus(*this);
        }

        sExplorerCtrl->SetFocus();
    }
}

//
// from AddressBarObserver
//
xpr_bool_t ExplorerPane::onExplore(AddressBar &aAddressBar, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
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

LPITEMIDLIST ExplorerPane::onFailExec(AddressBar &aAddressBar, const xpr_tchar_t *aExecPath)
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

void ExplorerPane::onMoveFocus(AddressBar &aAddressBar)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onMoveFocus(*this, 1);
    }
}

xpr_bool_t ExplorerPane::onExplore(PathBar &aPathBar, LPITEMIDLIST aFullPidl)
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

void ExplorerPane::onExplore(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy, const xpr_tchar_t *aCurPath)
{
    if (XPR_IS_NULL(aFullPidl))
        return;

    xpr_uint_t sExplorerCtrlId = aExplorerCtrl.GetDlgCtrlID();

    if (sExplorerCtrlId == mCurExplorerCtrlId)
    {
        // update address bar
        AddressBar *sAddressBar = getAddressBar();
        if (XPR_IS_NOT_NULL(sAddressBar) && XPR_IS_NOT_NULL(sAddressBar->m_hWnd))
            sAddressBar->explore(aFullPidl);

        // update path bar
        PathBar *sPathBar = getPathBar();
        if (XPR_IS_NOT_NULL(sPathBar) && XPR_IS_NOT_NULL(sPathBar->m_hWnd))
            sPathBar->setPath(aFullPidl);

        // update contents
        if (isVisibleContentsWnd() == XPR_TRUE)
        {
            LPTVITEMDATA sTvItemData = aExplorerCtrl.getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                setContentsFolder(sTvItemData);
                setContentsNormal(sTvItemData, XPR_TRUE);
            }
        }
    }

    // update status bar
    updateStatusBar(sExplorerCtrlId);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onExplored(*this, sExplorerCtrlId, aFullPidl, aUpdateBuddy);
    }
}

void ExplorerPane::onUpdateStatus(ExplorerCtrl &aExplorerCtrl, xpr_size_t aRealSelCount)
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

void ExplorerPane::onUpdateFreeSize(ExplorerCtrl &aExplorerCtrl)
{
    xpr_uint_t sExplorerCtrlId = aExplorerCtrl.GetDlgCtrlID();

    updateStatusBar(sExplorerCtrlId);
}

void ExplorerPane::onSetFocus(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->setActiveView(mViewIndex);
}

void ExplorerPane::onKillFocus(ExplorerCtrl &aExplorerCtrl)
{
}

void ExplorerPane::onRename(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_RENAME);
}

void ExplorerPane::onContextMenuDelete(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_DELETE);
}

void ExplorerPane::onContextMenuRename(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_FILE_RENAME);
}

void ExplorerPane::onContextMenuPaste(ExplorerCtrl &aExplorerCtrl)
{
    cmd::CommandParameters sParameters;
    sParameters.set(cmd::CommandParameterIdIsSelectedItem, (void *)XPR_TRUE);

    gFrame->executeCommand(ID_EDIT_PASTE, &aExplorerCtrl, &sParameters);
}

void ExplorerPane::onGoDrive(ExplorerCtrl &aExplorerCtrl)
{
    gFrame->executeCommand(ID_GO_DRIVE);
}

xpr_sint_t ExplorerPane::onDrop(ExplorerCtrl &aExplorerCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir)
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

xpr_bool_t ExplorerPane::onGetExecParam(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath, xpr_tchar_t *aParam, xpr_size_t aMaxLen)
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

void ExplorerPane::onExecError(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH * 2 + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.msg.wrong_path"), XPR_STRING_LITERAL("%s")), aPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }
}

void ExplorerPane::onSetViewStyle(ExplorerCtrl &aExplorerCtrl, xpr_sint_t aStyle, xpr_bool_t aRefresh)
{
    if (gOpt->mConfig.mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            // TODO: tab
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

void ExplorerPane::onUseColumn(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId)
{
    if (gOpt->mConfig.mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            // TODO: tab
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

void ExplorerPane::onSortItems(ExplorerCtrl &aExplorerCtrl, ColumnId *aColumnId, xpr_bool_t aAscending)
{
    if (gOpt->mConfig.mExplorerSaveViewSet == SAVE_VIEW_SET_SAME_BETWEEN_SPLIT)
    {
        xpr_sint_t i;
        xpr_sint_t sViewCount = gFrame->getViewCount();
        ExplorerCtrl *sExplorerCtrl;

        for (i = 0; i < sViewCount; ++i)
        {
            // TODO: tab
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

void ExplorerPane::onSetColumnWidth(ExplorerCtrl &aExplorerCtrl, xpr_sint_t sColumnIndex, xpr_sint_t sWidth)
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

void ExplorerPane::onMoveFocus(ExplorerCtrl &aExplorerCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onMoveFocus(*this, 0);
    }
}

void ExplorerPane::onRunFile(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NOT_NULL(aFullPidl))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

        fxb::RecentFileList &sRecentFileList = fxb::RecentFileList::instance();
        sRecentFileList.addFile(sPath);
    }
}

void ExplorerPane::OnExpSelNormal(void)
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

        updateStatusBar(sExplorerCtrlId);
    }
}

void ExplorerPane::OnExpSelSingleItem(void)
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData();
    if (XPR_IS_NULL(sExplorerCtrlData))
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
        xpr_sint_t sUnit = gOpt->mConfig.mSingleSelFileSizeUnit;
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
        _tcscpy(sExplorerCtrlData->mStatusPane0, theApp.loadString(XPR_STRING_LITERAL("explorer_window.status.pane1.single_selected")));
        _tcscpy(sExplorerCtrlData->mStatusPane1, XPR_STRING_LITERAL(""));

        if (XPR_IS_TRUE(sVisibleStatusBar))
            setStatusBarText();
    }
    else
    {
        _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.single_selected_file"), XPR_STRING_LITERAL("%s,%s,%s")), sType, sDate, sAttr);
        _tcscpy(sExplorerCtrlData->mStatusPane1, sSize);

        if (XPR_IS_TRUE(sVisibleStatusBar))
            setStatusBarText();
    }
}

void ExplorerPane::OnExpSelMultiItem(void)
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData();
    if (XPR_IS_NULL(sExplorerCtrlData))
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

    xpr_sint_t sUnit = gOpt->mConfig.mMultiSelFileSizeUnit;
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

        switch (gOpt->mConfig.mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder"), XPR_STRING_LITERAL("%s,%s")), sRealSelCountText, sRealSelFolderCountText);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file"), XPR_STRING_LITERAL("%s,%s")), sRealSelCountText, sRealSelFileCountText);
            break;

        default:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file_folder"), XPR_STRING_LITERAL("%s,%s,%s")), sRealSelCountText, sRealSelFolderCountText, sRealSelFileCountText);
            break;
        }
    }
    else
    {
        switch (gOpt->mConfig.mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder"), XPR_STRING_LITERAL("%d,%d")), sRealSelCount, sRealSelFolderCount);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_file"), XPR_STRING_LITERAL("%d,%d")), sRealSelCount, sRealSelFileCount);
            break;

        default:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.multiple_selected_folder_etc"), XPR_STRING_LITERAL("%d,%d,%d")), sRealSelCount, sRealSelFolderCount, sRealSelFileCount);
            break;
        }
    }

    _tcscpy(sExplorerCtrlData->mStatusPane1, sSize);

    if (XPR_IS_TRUE(sVisibleStatusBar))
        setStatusBarText();
}

void ExplorerPane::updateStatusBar(xpr_uint_t aId)
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData(aId);
    if (XPR_IS_NULL(sExplorerCtrlData))
        return;

    ExplorerCtrl *sExplorerCtrl = sExplorerCtrlData->mExplorerCtrl;
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

        switch (gOpt->mConfig.mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder"), XPR_STRING_LITERAL("%s,%s")), sCountText, sFolderCountText);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.file"), XPR_STRING_LITERAL("%s,%s")), sCountText, sFileCountText);
            break;

        default:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.file_folder"), XPR_STRING_LITERAL("%s,%s,%s")), sCountText, sFolderCountText, sFileCountText);
            break;
        }

        xpr_sint_t sUnit = gOpt->mConfig.mMultiSelFileSizeUnit;
        switch (sUnit)
        {
        case SIZE_UNIT_DEFAULT:
            fxb::SizeFormat::getDefSizeFormat(sTotalSize, sExplorerCtrlData->mStatusPane1, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_CUSTOM:
            fxb::SizeFormat::instance().getSizeFormat(sTotalSize, sExplorerCtrlData->mStatusPane1, XPR_MAX_PATH);
            break;

        case SIZE_UNIT_NONE:
            fxb::GetFormatedNumber(sTotalSize, sExplorerCtrlData->mStatusPane1, XPR_MAX_PATH);
            break;

        default:
            fxb::SizeFormat::getSizeUnitFormat(sTotalSize, sUnit, sExplorerCtrlData->mStatusPane1, XPR_MAX_PATH);
            break;
        }
    }
    else
    {
        switch (gOpt->mConfig.mExplorerListType)
        {
        case LIST_TYPE_FOLDER:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder"), XPR_STRING_LITERAL("%d,%d")), sCount, sFolderCount);
            break;

        case LIST_TYPE_FILE:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.etc"), XPR_STRING_LITERAL("%d,%d")), sCount, sFileCount);
            break;

        default:
            _stprintf(sExplorerCtrlData->mStatusPane0, theApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.status.pane1.folder_etc"), XPR_STRING_LITERAL("%d,%d,%d")), sCount, sFolderCount, sFileCount);
            break;
        }

        sExplorerCtrlData->mStatusPane1[0] = XPR_STRING_LITERAL('\0');
    }

    updateStatusBar();
}

void ExplorerPane::updateStatusBar(void)
{
    setStatusBarDrive();
    setStatusBarText();
}

void ExplorerPane::setStatusPaneBookmarkText(xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect)
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
                _stprintf(sText, XPR_STRING_LITERAL("%s: \'%s\'"), theApp.loadString(XPR_STRING_LITERAL("bookmark.status.program_ass")), sBookmarkItem->mPath.c_str());
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

void ExplorerPane::setStatusPaneDriveText(xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect)
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

void ExplorerPane::setStatusPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText)
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

void ExplorerPane::setStatusDisk(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(mStatusBar))
        return;

    mStatusBar->setDiskFreeSpace(aPath);
}

void ExplorerPane::setStatusBarText(void)
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData();
    if (XPR_IS_NULL(sExplorerCtrlData))
        return;

    if (isVisibleStatusBar() == XPR_FALSE)
        return;

    setStatusPaneText(0, sExplorerCtrlData->mStatusPane0);
    setStatusPaneText(1, sExplorerCtrlData->mStatusPane1);
}

void ExplorerPane::setStatusBarDrive(const xpr_tchar_t *aCurPath)
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

const xpr_tchar_t *ExplorerPane::getStatusPaneText(xpr_sint_t aIndex) const
{
    ExplorerCtrlData *sExplorerCtrlData = getExplorerCtrlData();
    if (XPR_IS_NULL(sExplorerCtrlData))
        return XPR_NULL;

    switch (aIndex)
    {
    case 0: return sExplorerCtrlData->mStatusPane0;
    case 1: return sExplorerCtrlData->mStatusPane1;
    }

    return XPR_NULL;
}

void ExplorerPane::onStatusBarRemove(StatusBar &aStatusBar, xpr_size_t aIndex)
{
}

void ExplorerPane::onStatusBarRemoved(StatusBar &aStatusBar)
{
}

void ExplorerPane::onStatusBarRemoveAll(StatusBar &aStatusBar)
{
}

void ExplorerPane::onStatuBarContextMenu(StatusBar &aStatusBar, xpr_size_t aIndex, const POINT &aPoint)
{
}

void ExplorerPane::onStatusBarDoubleClicked(StatusBar &aStatusBar, xpr_size_t sIndex)
{
    if (sIndex == 2 || sIndex == 3)
    {
        mStatusBar->showDriveProperties();
    }
}
