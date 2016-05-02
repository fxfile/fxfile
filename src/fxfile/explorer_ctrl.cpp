//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "explorer_ctrl.h"
#include "explorer_ctrl_observer.h"

#include "thumbnail.h"
#include "file_op_thread.h"
#include "context_menu.h"
#include "file_op_undo.h"
#include "drive_shcn.h"
#include "rename_helper.h"
#include "file_filter.h"
#include "shell_column_manager.h"
#include "file_change_watcher.h"
#include "adv_file_change_watcher.h"
#include "shell_icon.h"
#include "size_format.h"
#include "clip_format.h"
#include "img_list_manager.h"
#include "clipboard.h"
#include "app_ver.h"
#include "main_frame.h"
#include "picture_viewer.h"
#include "shell_enumerator.h"
#include "fnmatch.h"
#include "folder_layout.h"

#include "gui/gdi.h"
#include "gui/FlatHeaderCtrl.h"
#include "gui/DropSource.h"
#include "gui/DragImage.h"
#include "gui/MemDC.h"
#include "gui/BCMenu.h"
#include "gui/FileDialogST.h"
#include "gui/WindowScroller.h"

#include <Intshcut.h> // for IUniformResourceLocator
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
//
// user defined timer
//
enum
{
    TM_ID_DRAG_SCROLL_BEGIN = 0,
    TM_ID_DRAG_SCROLL_UP = TM_ID_DRAG_SCROLL_BEGIN,
    TM_ID_DRAG_SCROLL_DOWN,
    TM_ID_DRAG_SCROLL_LEFT,
    TM_ID_DRAG_SCROLL_RIGHT,
    TM_ID_DRAG_SCROLL_END = TM_ID_DRAG_SCROLL_RIGHT,

    TM_ID_SEL_VIEW = 10,

    TM_ID_RENAME_VISTA = 20,
    TM_ID_CLICK_RENAME = 42, // internal rename timer id (label click)
};

//
// item data type
//
//       0 : IShellFolder, ItemIDList
//      10 : Go Up
// 100~129 : Drive (ex: 100-A, 125-Z)
//     130 : Drive Selection
//
enum
{
    IDT_SHELL     = 0,
    IDT_PARENT    = 10,
    IDT_DRIVE     = 100,
    IDT_DRIVE_END = 125,
    IDT_DRIVE_SEL = 130,
};

//
// excution item type
//
enum
{
    EXEC_ERROR = -1,
    EXEC_NONE,
    EXEC_FOLDER,
    EXEC_LINK_FOLDER,
    EXEC_LINK_FILE,
    EXEC_LINK_URL,
    EXEC_FILE,
    EXEC_PARENT_FOLDER,
    EXEC_EACH_DRIVE,
    EXEC_DRIVE_SELECTION,
};

//
// user defined message
//
enum
{
    WM_THUMBNAIL_PROC            = WM_USER + 10,
    WM_SHELL_ASYNC_ICON          = WM_USER + 11,
    WM_SHELL_COLUMN_PROC         = WM_USER + 12,
    WM_PASTE_SELITEM             = WM_USER + 20,
    WM_DRIVE_SHELL_CHANGE_NOTIFY = WM_USER + 30,
    WM_SHELL_CHANGE_NOTIFY       = WM_USER + 1500,
    WM_FILE_CHANGE_NOTIFY        = WM_USER + 1501,
    WM_ADV_FILE_CHANGE_NOTIFY    = WM_USER + 1502,
};
} // namespace anonymous

//
// default column information
//
struct ExplorerCtrl::DefColumnInfo
{
    xpr::Guid          mFormatId;     // column foramt id
    xpr_sint_t         mPropertyId;   // column property id
    const xpr_tchar_t *mNameStringId; // column name
    xpr_sint_t         mFormat;       // column format
    xpr_sint_t         mWidth;        // column width
    xpr_bool_t         mDefault;      // column default
};

xpr_sint_t ExplorerCtrl::mRefCount = 0;
xpr_uint_t ExplorerCtrl::mCodeMgr = 0;

ExplorerCtrl::ExplorerCtrl(void)
    : mViewIndex(-1)
    , mNewOption(XPR_NULL)
{
    mRefCount++;

    mObserver           = XPR_NULL;

    // Folder Type
    mFolderType         = FOLDER_DEFAULT;

    // Item Selection
    mRealSelCount       = 0;
    mRealSelFolderCount = 0;
    mRealSelFileCount   = 0;

    // Code
    mCode               = 0;
    mSignature          = 0;
    mSorted             = XPR_FALSE;
    mUpdated            = XPR_FALSE;
    mVisible            = XPR_FALSE;
    mFirstExplore       = XPR_TRUE;

    // Drag & Drop
    mDropIndex          = -1;
    mScrollUpTimer      = XPR_FALSE;
    mScrollDownTimer    = XPR_FALSE;
    mScrollLeftTimer    = XPR_FALSE;
    mScrollRightTimer   = XPR_FALSE;
    mIntraDrag          = XPR_FALSE;

    mInsUnsellAll       = XPR_FALSE;
    mInsSelEdit         = XPR_FALSE;
    mRenameHelper       = XPR_NULL;
    mMouseEdit          = XPR_TRUE;
    mRenameResorting    = XPR_FALSE;

    // folder layout
    mViewStyle            = VIEW_STYLE_DETAILS;
    mFolderLayout         = XPR_NULL;
    mDefaultFolderLayout  = XPR_NULL;
    mComputerFolderLayout = XPR_NULL;
    mVirtualFolderLayout  = XPR_NULL;

    mSortColumnId.mPropertyId = 0;
    mSortAscending            = XPR_TRUE;
    mSortColumn               = -1;
    mDefColumnInfo            = XPR_NULL;
    mDefColumnCount           = 0;

    // history
    mGo                 = XPR_TRUE;
    mHistory            = XPR_NULL;
    mTvItemData         = XPR_NULL;

    // icon & font
    mShellIcon          = XPR_NULL;
    mExtraLargeImgList  = XPR_NULL;
    mLargeImgList       = XPR_NULL;
    mMediumImgList      = XPR_NULL;
    mNormalImgList      = XPR_NULL;
    mSmallImgList       = XPR_NULL;
    mCustomFont         = XPR_NULL;

    mCopyFullPidl       = XPR_NULL;
    mMoveFullPidl       = XPR_NULL;

    // file change notification
    mNotify             = XPR_TRUE;
    mShcnId             = 0;
    mWatchId            = FileChangeWatcher::InvalidWatchId;
    mAdvWatchId         = AdvFileChangeWatcher::InvalidAdvWatchId;
}

ExplorerCtrl::~ExplorerCtrl(void)
{
    mRefCount--;

    COM_FREE(mCopyFullPidl);
    COM_FREE(mMoveFullPidl);

    if (XPR_IS_NOT_NULL(mTvItemData))
    {
        COM_RELEASE(mTvItemData->mShellFolder);
        COM_FREE(mTvItemData->mPidl);
        COM_FREE(mTvItemData->mFullPidl);
    }

    XPR_SAFE_DELETE(mHistory);
    XPR_SAFE_DELETE(mTvItemData);

    XPR_SAFE_DELETE(mDefaultFolderLayout);
    XPR_SAFE_DELETE(mComputerFolderLayout);
    XPR_SAFE_DELETE(mVirtualFolderLayout);
    mFolderLayout = XPR_NULL;

    XPR_SAFE_DELETE(mRenameHelper);
    XPR_SAFE_DELETE(mShellIcon);

    XPR_SAFE_DELETE(mNewOption);
}

ExplorerCtrl::DefColumnInfo ExplorerCtrl::mDefColumnDefault[] = 
{
    { xpr::GuidNone, 0, XPR_STRING_LITERAL("explorer_window.column.name"),             LVCFMT_LEFT,  150, XPR_TRUE  },
    { xpr::GuidNone, 1, XPR_STRING_LITERAL("explorer_window.column.size"),             LVCFMT_RIGHT,  80, XPR_TRUE  },
    { xpr::GuidNone, 2, XPR_STRING_LITERAL("explorer_window.column.type"),             LVCFMT_LEFT,  150, XPR_TRUE  },
    { xpr::GuidNone, 3, XPR_STRING_LITERAL("explorer_window.column.modified_time"),    LVCFMT_LEFT,  140, XPR_TRUE  },
    { xpr::GuidNone, 4, XPR_STRING_LITERAL("explorer_window.column.attributes"),       LVCFMT_LEFT,   80, XPR_FALSE },
    { xpr::GuidNone, 5, XPR_STRING_LITERAL("explorer_window.column.extension"),        LVCFMT_LEFT,   80, XPR_FALSE },
};

ExplorerCtrl::DefColumnInfo ExplorerCtrl::mDefColumnComputer[] = 
{
    { xpr::GuidNone, 0, XPR_STRING_LITERAL("explorer_window.column.name"),             LVCFMT_LEFT,  150, XPR_TRUE  },
    { xpr::GuidNone, 1, XPR_STRING_LITERAL("explorer_window.column.type"),             LVCFMT_LEFT,  150, XPR_TRUE  },
    { xpr::GuidNone, 2, XPR_STRING_LITERAL("explorer_window.column.total_size"),       LVCFMT_RIGHT, 100, XPR_TRUE  },
    { xpr::GuidNone, 3, XPR_STRING_LITERAL("explorer_window.column.free_size"),        LVCFMT_RIGHT, 100, XPR_TRUE  },
    { xpr::GuidNone, 4, XPR_STRING_LITERAL("explorer_window.column.file_system"),      LVCFMT_RIGHT, 100, XPR_FALSE },
    { xpr::GuidNone, 5, XPR_STRING_LITERAL("explorer_window.column.description"),      LVCFMT_RIGHT, 100, XPR_FALSE },
};

ExplorerCtrl::DefColumnInfo ExplorerCtrl::mDefColumnVirtual[] = 
{
    { xpr::GuidNone, 0, XPR_STRING_LITERAL("explorer_window.column.name"),             LVCFMT_LEFT,  150, XPR_TRUE  },
    { xpr::GuidNone, 1, XPR_STRING_LITERAL("explorer_window.column.description"),      LVCFMT_LEFT,  300, XPR_TRUE  },
};

BEGIN_MESSAGE_MAP(ExplorerCtrl, super)
    ON_WM_CREATE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_KEYDOWN()
    ON_WM_CONTEXTMENU()
    ON_WM_SETFOCUS()
    ON_WM_KEYUP()
    ON_WM_KILLFOCUS()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEACTIVATE()
    ON_WM_MBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_VSCROLL()
    ON_NOTIFY_REFLECT(NM_CLICK,           OnClick)
    ON_NOTIFY_REFLECT(NM_DBLCLK,          OnDblclk)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,      OnCustomdraw)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetdispinfo)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,    OnColumnclick)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG,      OnBegindrag)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,   OnEndlabeledit)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM,     OnDeleteitem)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED,    OnItemchanged)
    ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnDeleteallitems)
    ON_NOTIFY_REFLECT(LVN_BEGINRDRAG,     OnBeginrdrag)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN,        OnLvnKeyDown)
    ON_NOTIFY_REFLECT(LVN_SETDISPINFO,    OnSetdispinfo)
    ON_NOTIFY_REFLECT(LVN_INSERTITEM,     OnInsertitem)
    ON_NOTIFY_REFLECT(LVN_GETINFOTIP,     OnGetinfotip)
    ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE,   OnItemActivate)
    ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN,   OnMarqueebegin)
    ON_MESSAGE(WM_DRIVE_SHELL_CHANGE_NOTIFY, OnDriveShellChangeNotify)
    ON_MESSAGE(WM_SHELL_CHANGE_NOTIFY,       OnShellChangeNotify)
    ON_MESSAGE(WM_FILE_CHANGE_NOTIFY,        OnFileChangeNotify)
    ON_MESSAGE(WM_ADV_FILE_CHANGE_NOTIFY,    OnAdvFileChangeNotify)
    ON_MESSAGE(WM_THUMBNAIL_PROC,            OnThumbnailProc)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON,          OnShellAsyncIcon)
    ON_MESSAGE(WM_SHELL_COLUMN_PROC,         OnShellColumnProc)
    ON_MESSAGE(WM_PASTE_SELITEM,             OnPasteSelItem)
END_MESSAGE_MAP()

void ExplorerCtrl::setObserver(ExplorerCtrlObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t ExplorerCtrl::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;
    sStyle |= WS_HSCROLL;
    sStyle |= WS_VSCROLL;

    return super::Create(sStyle, aRect, aParentWnd, aId);
}

xpr_bool_t ExplorerCtrl::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    aCreateStruct.style &= ~LVS_TYPEMASK;
    aCreateStruct.style |= LVS_ALIGNTOP;
    aCreateStruct.style |= LVS_REPORT;
    aCreateStruct.style |= LVS_SHAREIMAGELISTS;
    aCreateStruct.style |= LVS_AUTOARRANGE;
    aCreateStruct.style |= LVS_EDITLABELS;
    aCreateStruct.style |= LVS_SHOWSELALWAYS;

    aCreateStruct.dwExStyle |= LVS_EX_INFOTIP;
    aCreateStruct.dwExStyle |= 0x00040000;

    return super::PreCreateWindow(aCreateStruct);
}

xpr_sint_t ExplorerCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    SetWindowText(FXFILE_PROGRAM_NAME);

    // enable vist enhanced control
    enableVistaEnhanced(XPR_TRUE);

    // Register Drag & Drop
    mDropTarget.registerObserver(this);
    mDropTarget.Register(this);

    // create file change watcher
    //if (UserEnv::instance().bPlatformNT)
    //{
    //    // create advanced file change watcher
    //    if (!m_pAdvWatcher)
    //        m_pAdvWatcher = new AdvFileChangeWatcher;

    //    m_pAdvWatcher->SetOwner(m_hWnd, WM_ADV_FILE_CHANGE_NOTIFY);
    //    m_pAdvWatcher->Create();
    //}

    if (XPR_IS_NULL(mHistory))
        mHistory = new History;

    if (XPR_IS_NULL(mShellIcon))
    {
        mShellIcon = new ShellIcon;
        mShellIcon->setOwner(m_hWnd, WM_SHELL_ASYNC_ICON);
    }

    // Drive Shell Change Notification
    xpr_slong_t sEventMask = 0;
    sEventMask |= SHCNE_MEDIAINSERTED;
    sEventMask |= SHCNE_MEDIAREMOVED;
    sEventMask |= SHCNE_DRIVEADD;
    sEventMask |= SHCNE_DRIVEREMOVED;
    sEventMask |= SHCNE_FREESPACE;
    sEventMask |= SHCNE_RENAMEFOLDER;
    DriveShcn::instance().registerWatch(*this, WM_DRIVE_SHELL_CHANGE_NOTIFY, sEventMask);

    // thumbnail
    Thumbnail &sThumbnail = Thumbnail::instance();

    sThumbnail.setThumbSize(CSize(mOption.mThumbnailWidth, mOption.mThumbnailHeight));
    sThumbnail.setThumbPriority(mOption.mThumbnailPriority);

    return 0;
}

void ExplorerCtrl::setOption(Option &aOption)
{
    if (XPR_IS_NULL(mNewOption))
        mNewOption = new Option;

    *mNewOption = aOption;
}

void ExplorerCtrl::setImageList(CImageList *aSmallImgList,
                                CImageList *aMediumImgList,
                                CImageList *aLargeImgList,
                                CImageList *aExtraLargeImgList)
{
    xpr_sint_t sViewStyle = getViewStyle();

    mExtraLargeImgList = aExtraLargeImgList;
    mLargeImgList      = aLargeImgList;
    mMediumImgList     = aMediumImgList;
    mSmallImgList      = aSmallImgList;

    if (sViewStyle == VIEW_STYLE_THUMBNAIL)
    {
        setViewStyle(VIEW_STYLE_THUMBNAIL, XPR_FALSE);
    }
    else
    {
        switch (sViewStyle)
        {
        case VIEW_STYLE_EXTRA_LARGE_ICONS:
            mNormalImgList = mExtraLargeImgList;
            break;

        case VIEW_STYLE_LARGE_ICONS:
            mNormalImgList = mLargeImgList;
            break;

        case VIEW_STYLE_MEDIUM_ICONS:
            mNormalImgList = mMediumImgList;
            break;

        default:
            mNormalImgList = mMediumImgList;
            break;
        }
    }

    super::SetImageList(mNormalImgList, LVSIL_NORMAL);
    super::SetImageList(mSmallImgList,  LVSIL_SMALL);
}

CImageList *ExplorerCtrl::getImageList(xpr_bool_t aNormal) const
{
    return XPR_IS_TRUE(aNormal) ? mNormalImgList : mSmallImgList;
}

void ExplorerCtrl::loadHistory(HistoryDeque *aBackwardDeque, HistoryDeque *aForwardDeque, HistoryDeque *aHistoryDeque)
{
    mHistory->setBackwardDeque(aBackwardDeque);
    mHistory->setForwardDeque(aForwardDeque);
    mHistory->setHistoryDeque(aHistoryDeque);
}

void ExplorerCtrl::OnDestroy(void)
{
    // stop & unregister shell change notification
    ShellChangeNotify::instance().unregisterWatch(mShcnId);
    DriveShcn::instance().unregisterWatch(this);

    // stop & unregister file change notification
    FileChangeWatcher::instance().unregisterWatch(mWatchId);
    mWatchId = FileChangeWatcher::InvalidWatchId;

    // stop & unregister advanced file change notification
    AdvFileChangeWatcher::instance().unregisterWatch(mAdvWatchId);
    mAdvWatchId = AdvFileChangeWatcher::InvalidAdvWatchId;

    // save folder layout
    saveFolderLayout();
    deleteAllColumns();

    // delete all items
    DeleteAllItems();

    mDropTarget.Revoke();

    // stop icon thread
    if (XPR_IS_NOT_NULL(mShellIcon))
    {
        mShellIcon->stopThread();
    }

    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->clear();

    if (XPR_IS_NOT_NULL(mCustomFont))
    {
        DELETE_OBJECT(*mCustomFont);
        XPR_SAFE_DELETE(mCustomFont);
    }

    // stop thumbnail thread
    if (mRefCount <= 1)
    {
        Thumbnail &sThumbnail = Thumbnail::instance();

        sThumbnail.stop();

        if (mOption.mThumbnailSaveCache == XPR_TRUE)
            sThumbnail.saveCache();

        sThumbnail.destroy();
    }
}

xpr_sint_t ExplorerCtrl::getViewIndex(void) const
{
    return mViewIndex;
}

void ExplorerCtrl::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

LPARAM ExplorerCtrl::GetItemData(xpr_sint_t aItemIndex) const
{
    LVITEM sLvItem = {0};
    sLvItem.iItem = aItemIndex;
    sLvItem.mask  = LVIF_PARAM;
    ::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)&sLvItem);

    if (XPR_IS_NOT_NULL(sLvItem.lParam) && ((LPLVITEMDATA)sLvItem.lParam)->mItemType != IDT_SHELL)
        return 0;

    return sLvItem.lParam;
}

DWORD ExplorerCtrl::getItemDataType(xpr_sint_t aItemIndex) const
{
    LVITEM sLvItem = {0};
    sLvItem.iItem = aItemIndex;
    sLvItem.mask  = LVIF_PARAM;
    ::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)&sLvItem);

    if (XPR_IS_NOT_NULL(sLvItem.lParam))
        return ((LPLVITEMDATA)sLvItem.lParam)->mItemType;

    return 0;
}

xpr_bool_t ExplorerCtrl::isShellItem(xpr_sint_t aItemIndex) const
{
    DWORD sItemDataType = getItemDataType(aItemIndex);
    return (sItemDataType == IDT_SHELL) ? XPR_TRUE : XPR_FALSE;
}

xpr_tchar_t ExplorerCtrl::getDriveFromItemType(DWORD aItemDataType) const
{
    return XPR_STRING_LITERAL('A') + (xpr_tchar_t)(aItemDataType - IDT_DRIVE);
}

void ExplorerCtrl::verifyItemData(LPLVITEMDATA *aLvItemData)
{
    if (XPR_IS_NOT_NULL(aLvItemData) && XPR_IS_NOT_NULL(*aLvItemData) && (*aLvItemData)->mItemType != IDT_SHELL)
        *aLvItemData = XPR_NULL;
}

xpr_sint_t ExplorerCtrl::getViewStyle(void) const
{
    return mViewStyle;
}

void ExplorerCtrl::setViewStyle(xpr_sint_t aViewStyle, xpr_bool_t aRefresh, xpr_bool_t aNotifyToObserver)
{
    if (aViewStyle == VIEW_STYLE_THUMBNAIL)
    {
        Thumbnail &sThumbnail = Thumbnail::instance();

        xpr_bool_t sLoad = XPR_FALSE;
        if (mOption.mThumbnailSaveCache == XPR_TRUE)
            sLoad = sThumbnail.loadCache();

        if (XPR_IS_FALSE(sLoad))
            sThumbnail.create();

        sThumbnail.stop();
        sThumbnail.start();
        sThumbnail.setThumbPriority(mOption.mThumbnailPriority);

        xpr_bool_t sVisible = IsWindowVisible();
        if (XPR_IS_TRUE(sVisible))
        {
            ShowWindow(SW_HIDE);
            SetRedraw(XPR_FALSE);
        }

        CSize sThumbSize = sThumbnail.getThumbSize();
        sThumbSize.cx += 18;
        sThumbSize.cy += 40;

        SetIconSpacing(sThumbSize);
        mNormalImgList = mMediumImgList;
        super::SetImageList(sThumbnail.getThumbImgList()->getImageList(), LVSIL_NORMAL);

        ModifyStyle(LVS_TYPEMASK, LVS_ICON);

        if (XPR_IS_TRUE(sVisible))
        {
            SetRedraw();
            UpdateWindow();
            ShowWindow(SW_SHOW);
        }

        if (XPR_IS_TRUE(aRefresh))
        {
            refresh();
        }
    }
    else
    {
        xpr_bool_t sVisible = IsWindowVisible();
        if (XPR_IS_TRUE(sVisible))
        {
            ShowWindow(SW_HIDE);
            SetRedraw(XPR_FALSE);
        }

        SetIconSpacing(-1, -1);

        DWORD sStyle = LVS_ICON;
        switch (aViewStyle)
        {
        case VIEW_STYLE_EXTRA_LARGE_ICONS:
            mNormalImgList = mExtraLargeImgList;
            sStyle = LVS_ICON;
            break;

        case VIEW_STYLE_LARGE_ICONS:
            mNormalImgList = mLargeImgList;
            sStyle = LVS_ICON;
            break;

        case VIEW_STYLE_MEDIUM_ICONS:
            mNormalImgList = mMediumImgList;
            sStyle = LVS_ICON;
            break;

        default:
            mNormalImgList = mMediumImgList;
            switch (aViewStyle)
            {
            case VIEW_STYLE_SMALL_ICONS: sStyle = LVS_SMALLICON; break;
            case VIEW_STYLE_LIST:        sStyle = LVS_LIST;      break;

            case VIEW_STYLE_DETAILS:
            default:                     sStyle = LVS_REPORT;    break;
            }
            break;
        }

        super::SetImageList(mNormalImgList, LVSIL_NORMAL);

        ModifyStyle(LVS_TYPEMASK, sStyle);
        if (sStyle == LVS_ICON)
        {
            Arrange(LVA_DEFAULT);
        }

        if (XPR_IS_TRUE(sVisible))
        {
            SetRedraw();
            UpdateWindow();
            ShowWindow(SW_SHOW);
        }

        if (XPR_IS_TRUE(aRefresh))
        {
            refresh();
        }

        if (XPR_IS_TRUE(mOption.mAutoColumnWidth) && sStyle == LVS_REPORT)
            SetColumnWidth(0, LVSCW_AUTOSIZE);
    }

    // insert at before drive item when view style is icon, small icon and thumbnail
    if (XPR_IS_FALSE(aRefresh))
    {
        xpr_bool_t sIconsViewStyle = (aViewStyle == VIEW_STYLE_EXTRA_LARGE_ICONS ||
                                      aViewStyle == VIEW_STYLE_LARGE_ICONS       ||
                                      aViewStyle == VIEW_STYLE_MEDIUM_ICONS      ||
                                      aViewStyle == VIEW_STYLE_SMALL_ICONS       ||
                                      aViewStyle == VIEW_STYLE_THUMBNAIL);

        if (XPR_IS_TRUE(sIconsViewStyle) && (mOption.mShowDrive == XPR_TRUE || mOption.mShowDriveItem == XPR_TRUE))
        {
            CPoint sPoint(0,0);
            xpr_sint_t sLastInsertIndex = getLastInsertIndex();
            xpr_sint_t i, sCount = GetItemCount();
            for (i = sCount - 1; i >= sLastInsertIndex; --i)
            {
                if (GetItemPosition(sLastInsertIndex-1, &sPoint) == XPR_TRUE)
                    SetItemPosition(i, sPoint);
            }
        }
    }

    mViewStyle = aViewStyle;

    if (XPR_IS_TRUE(aNotifyToObserver))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            FolderLayoutChange sFolderLayoutChange;
            sFolderLayoutChange.mChangeMode = FolderLayoutChange::ChangeModeViewStyle;
            sFolderLayoutChange.mViewStyle  = aViewStyle;
            sFolderLayoutChange.mRefresh    = aRefresh;

            mObserver->onFolderLayoutChange(*this, sFolderLayoutChange);
        }
    }
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoParentItem(LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    if (XPR_TEST_BITS(aLvItem.mask, LVIF_TEXT))
    {
        switch (aLvItem.iSubItem)
        {
        case 0:
            _stprintf(aLvItem.pszText, XPR_STRING_LITERAL("[..] %s"), gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.up_folder")));
            break;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoDriveItem(LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    xpr_tchar_t sDrive[4] = {0};
    _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(aLvItemData->mItemType));

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_TEXT))
    {
        switch (aLvItem.iSubItem)
        {
        case 0:
            if (aLvItemData->mItemType == IDT_DRIVE_SEL)
            {
                _stprintf(aLvItem.pszText, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.drive")));
            }
            else
            {
                _stprintf(aLvItem.pszText, XPR_STRING_LITERAL("[-%c-] "), sDrive[0]);
                LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
                if (XPR_IS_NOT_NULL(sFullPidl))
                    GetName(sFullPidl, SHGDN_INFOLDER, aLvItem.pszText+_tcslen(aLvItem.pszText));
                COM_FREE(sFullPidl);
            }
            break;

        case 1:
            if (mOption.mShowDriveSize == XPR_TRUE && aLvItemData->mItemType != IDT_DRIVE_SEL)
            {
                getDriveItemTotalSize(sDrive, aLvItem.pszText, aLvItem.cchTextMax);
            }
            break;

        case 2:
            if (mOption.mShowDriveSize == XPR_TRUE && aLvItemData->mItemType != IDT_DRIVE_SEL)
            {
                getDriveItemFreeSize(sDrive, aLvItem.pszText, aLvItem.cchTextMax);
            }
            break;
        }
    }

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_TEXT))
    {
        if (mOption.mNameCaseType == NAME_CASE_TYPE_UPPER)
            _tcsupr(aLvItem.pszText);
        else if (mOption.mNameCaseType == NAME_CASE_TYPE_LOWER)
            _tcslwr(aLvItem.pszText);
    }

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_IMAGE))
    {
        if (sDrive[0] > XPR_STRING_LITERAL('Z'))
            sDrive[0] = XPR_STRING_LITERAL('C');

        if (mOption.mCustomIcon == XPR_TRUE)
        {
            aLvItem.iImage = getFileIconIndex(aLvItemData, sDrive);
        }
        else
        {
            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
            if (XPR_IS_NOT_NULL(sFullPidl))
                aLvItem.iImage = GetItemIconIndex(sFullPidl, XPR_FALSE);
            COM_FREE(sFullPidl);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoShellItemDefault(const ColumnId &aColumnId, LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    // Column 0 - name
    // Column 1 - size
    // Column 2 - type
    // Column 3 - modified time
    // Column 4 - attributes
    // Column 5 - extension

    switch (aColumnId.mPropertyId)
    {
    case 0: getItemName(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);      break;
    case 1: getFileSize(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);      break;
    case 2: getFileType(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);      break;
    case 3: getFileTime(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);      break;
    case 4: getFileAttr(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);      break;
    case 5: getFileExtension(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax); break;
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoShellItemComputer(const ColumnId &aColumnId, LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    switch (aColumnId.mPropertyId)
    {
    case 0: // name
        {
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_INFOLDER, aLvItem.pszText);
            break;
        }

    case 1: // type
        {
            xpr::string sPath;
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            if (sPath.length() <= 3)
                getRootDriveType(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItem.pszText, aLvItem.cchTextMax);
            else
                getFileType(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);
            break;
        }

    case 2: // total size
        {
            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                getDriveTotalSize(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItem.pszText, aLvItem.cchTextMax);
            break;
        }

    case 3: // free size
        {
            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                getDriveFreeSize(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItem.pszText, aLvItem.cchTextMax);
            break;
        }

        //case 4: // file system
        //{
        //    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        //    {
        //        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        //        GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sDrive);

        //        ::GetDriveFileSystem(sDrive, aLvItem.pszText, XPR_MAX_PATH);
        //    }

        //    break;
        //}

        //case 5: // description
        //{
        //    if (IsVirtualItem(aLvItemData->mShellFolder, aLvItemData->mPidl) == XPR_TRUE)
        //        getDescription(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItem.pszText, aLvItem.cchTextMax, XPR_TRUE);
        //    break;
        //}
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoShellItemVirtual(const ColumnId &aColumnId, LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    switch (aColumnId.mPropertyId)
    {
    case 0: // name
        {
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_INFOLDER, aLvItem.pszText);
            break;
        }

    case 1: // description
        {
            if (getDescription(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax) == XPR_FALSE)
                getNetDescription(aLvItemData, aLvItem.pszText, aLvItem.cchTextMax);

            xpr_sint_t i;
            xpr_sint_t sLen = (xpr_sint_t)_tcslen(aLvItem.pszText);
            for (i = 0; i < sLen; ++i)
            {
                switch (aLvItem.pszText[i])
                {
                case XPR_STRING_LITERAL('\t'):
                case XPR_STRING_LITERAL('\n'):
                    aLvItem.pszText[i] = XPR_STRING_LITERAL(' ');
                    break;

                case XPR_STRING_LITERAL('\r'):
                    aLvItem.pszText[i] = XPR_STRING_LITERAL(',');
                }
            }
            break;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnGetdispinfoShellItem(LVITEM &aLvItem, LPLVITEMDATA aLvItemData)
{
    verifyItemData(&aLvItemData);
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    ColumnId *sColumnId = getColumnId(aLvItem.iSubItem);
    if (XPR_IS_NULL(sColumnId))
        return XPR_FALSE;

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_TEXT))
    {
        aLvItem.pszText[0] = XPR_STRING_LITERAL('\0');

        if (XPR_IS_TRUE(sColumnId->mFormatId.none()))
        {
            switch (mFolderType)
            {
            case FOLDER_DEFAULT:  OnGetdispinfoShellItemDefault (*sColumnId, aLvItem, aLvItemData); break;
            case FOLDER_COMPUTER: OnGetdispinfoShellItemComputer(*sColumnId, aLvItem, aLvItemData); break;
            case FOLDER_VIRTUAL:  OnGetdispinfoShellItemVirtual (*sColumnId, aLvItem, aLvItemData); break;
            }
        }
        else
        {
            LPSHELLFOLDER2 sShellFolder2 = aLvItemData->mShellFolder2;
            if (XPR_IS_NULL(sShellFolder2))
            {
                aLvItemData->mShellFolder->QueryInterface(IID_IShellFolder2, (LPVOID *)&sShellFolder2);
                aLvItemData->mShellFolder2 = sShellFolder2;
            }

            if (XPR_IS_NOT_NULL(sShellFolder2))
            {
                ShellColumnManager &sShellColumn = ShellColumnManager::instance();

                if (sShellColumn.isAsyncColumn(sShellFolder2, *sColumnId) == XPR_TRUE)
                {
                    ShellColumnManager::AsyncInfo *sAsyncInfo = new ShellColumnManager::AsyncInfo;
                    sAsyncInfo->mCode         = mCode;
                    sAsyncInfo->mHwnd         = m_hWnd;
                    sAsyncInfo->mMsg          = WM_SHELL_COLUMN_PROC;
                    sAsyncInfo->mIndex        = aLvItem.iItem;
                    sAsyncInfo->mSignature    = aLvItemData->mSignature;
                    sAsyncInfo->mColumnId     = *sColumnId;
                    sAsyncInfo->mColumnIndex  = sShellColumn.getDetailColumn(sAsyncInfo->mColumnId);
                    sAsyncInfo->mShellFolder2 = sShellFolder2;
                    sAsyncInfo->mPidl         = fxfile::base::Pidl::clone(aLvItemData->mPidl);
                    sAsyncInfo->mOldText      = XPR_NULL;
                    sAsyncInfo->mText         = new xpr_tchar_t[aLvItem.cchTextMax+1];
                    sAsyncInfo->mMaxLen       = aLvItem.cchTextMax;
                    sAsyncInfo->mFlags        = ShellColumnManager::FlagsNone;
                    sAsyncInfo->mShellFolder2->AddRef();
                    sAsyncInfo->mText[0] = XPR_STRING_LITERAL('\0');

                    if (sShellColumn.getAsyncColumnText(sAsyncInfo) == XPR_FALSE)
                    {
                        XPR_SAFE_DELETE(sAsyncInfo);
                    }
                }
                else
                {
                    sShellColumn.getColumnText(sShellFolder2, aLvItemData->mPidl, *sColumnId, aLvItem.pszText, aLvItem.cchTextMax);
                }
            }
        }
    }

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_TEXT))
    {
        if (mOption.mNameCaseType == NAME_CASE_TYPE_UPPER)
            _tcsupr(aLvItem.pszText);
        else if (mOption.mNameCaseType == NAME_CASE_TYPE_LOWER)
            _tcslwr(aLvItem.pszText);
    }

    if (XPR_TEST_BITS(aLvItem.mask, LVIF_IMAGE))
    {
        static xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sPath[0] = XPR_STRING_LITERAL('\0');

        if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        if (mOption.mCustomIcon == XPR_TRUE)
        {
            aLvItem.iImage = getFileIconIndex(aLvItemData, sPath);
        }
        else
        {
            // The icon of a executable file(.EXE) get by other thread.
            xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sPath);
            if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".exe")) == 0)
            {
                static xpr_sint_t sExeIconIndex = -1;
                if (sExeIconIndex == -1)
                    sExeIconIndex = GetFileExtIconIndex(XPR_STRING_LITERAL(".exe"));

                aLvItem.iImage = sExeIconIndex;

                ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
                sAsyncIcon->mType              = ShellIcon::TypeIconIndex;
                sAsyncIcon->mCode              = mCode;
                sAsyncIcon->mItem              = aLvItem.iItem;
                sAsyncIcon->mSignature         = aLvItemData->mSignature;
                sAsyncIcon->mShellFolder       = aLvItemData->mShellFolder;
                sAsyncIcon->mPidl              = fxfile::base::Pidl::clone(aLvItemData->mPidl);
                sAsyncIcon->mResult.mIconIndex = -1;
                sAsyncIcon->mShellFolder->AddRef();

                if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
                {
                    XPR_SAFE_DELETE(sAsyncIcon);
                }
            }
            else
            {
                aLvItem.iImage = getFileIconIndex(aLvItemData);
            }
        }

        // The thumbnail function process through thread.
        if (getViewStyle() == VIEW_STYLE_THUMBNAIL)
        {
            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                static xpr_tchar_t sPath[XPR_MAX_PATH + 1];

                sPath[0] = XPR_STRING_LITERAL('\0');
                GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                aLvItemData->mThumbImageId = getThumbImageId(sPath);
            }
        }

        aLvItem.state = 0;
        aLvItem.stateMask = 0;

        if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_GHOSTED))
        {
            aLvItem.mask      |= LVIF_STATE;
            aLvItem.state     |= LVIS_CUT;
            aLvItem.stateMask |= LVIS_CUT;
        }

        if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_SHARE))
        {
            aLvItem.mask      |= LVIF_STATE;
            aLvItem.state     |= INDEXTOOVERLAYMASK(1);
            aLvItem.stateMask |= LVIS_OVERLAYMASK;
        }

        if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_LINK))
        {
            aLvItem.mask      |= LVIF_STATE;
            aLvItem.state     |= INDEXTOOVERLAYMASK(2);
            aLvItem.stateMask |= LVIS_OVERLAYMASK;
        }

        // shell icon custom overlay
        ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
        sAsyncIcon->mType              = ShellIcon::TypeOverlayIndex;
        sAsyncIcon->mCode              = mCode;
        sAsyncIcon->mItem              = aLvItem.iItem;
        sAsyncIcon->mSignature         = aLvItemData->mSignature;
        sAsyncIcon->mShellFolder       = aLvItemData->mShellFolder;
        sAsyncIcon->mPidl              = fxfile::base::Pidl::clone(aLvItemData->mPidl);
        sAsyncIcon->mResult.mIconIndex = -1;
        sAsyncIcon->mShellFolder->AddRef();

        if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(sAsyncIcon);
        }
    }

    return XPR_TRUE;
}

void ExplorerCtrl::OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult)
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO*)aNmHdr;
    *aResult = 0;

    LVITEM &sLvItem = sLvDispInfo->item;
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sLvDispInfo->item.lParam;

    if (XPR_IS_NULL(sLvItemData))
        return;

    // 3 type item - Parent, Drive, Shell
    if (sLvItemData->mItemType == IDT_PARENT)
    {
        OnGetdispinfoParentItem(sLvItem, sLvItemData);
    }
    else if (XPR_IS_RANGE(IDT_DRIVE, sLvItemData->mItemType, IDT_DRIVE_SEL))
    {
        OnGetdispinfoDriveItem(sLvItem, sLvItemData);
    }
    else
    {
        OnGetdispinfoShellItem(sLvItem, sLvItemData);
    }

    if (XPR_IS_TRUE(mSorted))
        sLvItem.mask |= LVIF_DI_SETITEM;
}

LRESULT ExplorerCtrl::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    ShellIcon::AsyncIcon *sAsyncIcon = (ShellIcon::AsyncIcon *)wParam;
    if (XPR_IS_NULL(sAsyncIcon))
        return 0;

    if (sAsyncIcon->mCode == mCode)
    {
        xpr_sint_t sIndex = -1;
        LPLVITEMDATA sLvItemData = XPR_NULL;

        if (sAsyncIcon->mItem != -1 && sAsyncIcon->mItem >= 0)
        {
            sLvItemData = (LPLVITEMDATA)GetItemData((xpr_sint_t)sAsyncIcon->mItem);
            if (XPR_IS_NOT_NULL(sLvItemData) && sLvItemData->mSignature == sAsyncIcon->mSignature)
                sIndex = (xpr_sint_t)sAsyncIcon->mItem;
        }

        if (sIndex < 0)
        {
            sIndex = findItemSignature(sAsyncIcon->mSignature);
            if (sIndex >= 0)
                sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        }

        if (sIndex >= 0 && XPR_IS_NOT_NULL(sLvItemData))
        {
            switch (sAsyncIcon->mType)
            {
            case ShellIcon::TypeIconIndex:
                {
                    LVITEM sLvItem = {0};
                    sLvItem.mask   = LVIF_IMAGE;
                    sLvItem.iItem  = sIndex;
                    sLvItem.iImage = sAsyncIcon->mResult.mIconIndex;
                    SetItem(&sLvItem);
                    break;
                }

            case ShellIcon::TypeOverlayIndex:
                {
                    xpr_uint_t sState = 0;

                    if (sAsyncIcon->mResult.mIconIndex >= 0)
                        sState = INDEXTOOVERLAYMASK(sAsyncIcon->mResult.mIconIndex);
                    else
                    {
                        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_SHARE)) sState = INDEXTOOVERLAYMASK(1);
                        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_LINK))  sState = INDEXTOOVERLAYMASK(2);
                    }

                    SetItemState(sIndex, sState, LVIS_OVERLAYMASK);
                    break;
                }
            }
        }
    }

    XPR_SAFE_DELETE(sAsyncIcon);

    return 0;
}

LRESULT ExplorerCtrl::OnShellColumnProc(WPARAM wParam, LPARAM lParam)
{
    ShellColumnManager::AsyncInfo *sAsyncInfo = (ShellColumnManager::AsyncInfo *)wParam;
    if (XPR_IS_NULL(sAsyncInfo))
        return 0;

    if (sAsyncInfo->mCode == mCode)
    {
        xpr_sint_t sIndex = -1;

        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sAsyncInfo->mIndex);
        if (XPR_IS_NOT_NULL(sLvItemData) && sLvItemData->mSignature == sAsyncInfo->mSignature)
            sIndex = sAsyncInfo->mIndex;

        if (sIndex < 0)
        {
            sIndex = findItemSignature(sAsyncInfo->mSignature);
            sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        }

        if (sIndex >= 0 && XPR_IS_NOT_NULL(sLvItemData))
        {
            xpr_sint_t sSubItem = getColumnFromId(&sAsyncInfo->mColumnId);
            if (sSubItem >= 0)
            {
                SetItemText(sIndex, sSubItem, sAsyncInfo->mText);
            }
        }
    }

    XPR_SAFE_DELETE(sAsyncInfo);

    return 0;
}

void ExplorerCtrl::OnSetdispinfo(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO*)aNmHdr;
    LPLVITEM sLvItem = &sLvDispInfo->item;

    *aResult = 0;
}

xpr_bool_t ExplorerCtrl::getItemName(LPLVITEMDATA aLvItemData, xpr_tchar_t *aName, const xpr_size_t aMaxLen) const
{
    return getItemName(aLvItemData, aName, aMaxLen, mOption.mFileExtType);
}

xpr_bool_t ExplorerCtrl::getItemName(LPLVITEMDATA aLvItemData, xpr_tchar_t *aName, const xpr_size_t aMaxLen, xpr_sint_t aExtensionType) const
{
    return getItemName(aLvItemData->mShellFolder, aLvItemData->mPidl, aLvItemData->mShellAttributes, aName, aMaxLen, aExtensionType);
}

xpr_bool_t ExplorerCtrl::getItemName(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aName, const xpr_size_t aMaxLen) const
{
    return getItemName(aFullPidl, aName, aMaxLen, mOption.mFileExtType);
}

xpr_bool_t ExplorerCtrl::getItemName(LPCITEMIDLIST aFullPidl, xpr_tchar_t *aName, const xpr_size_t aMaxLen, xpr_sint_t aExtensionType) const
{
    if (XPR_IS_NULL(aName))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    if (fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl) == XPR_FALSE)
        return XPR_FALSE;

    xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
    GetItemAttributes(sShellFolder, sPidl, sShellAttributes);

    xpr_bool_t sResult = getItemName(sShellFolder, sPidl, sShellAttributes, aName, aMaxLen, aExtensionType);

    COM_RELEASE(sShellFolder);

    return sResult;
}

xpr_bool_t ExplorerCtrl::getItemName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aName, const xpr_size_t aMaxLen) const
{
    return getItemName(aShellFolder, aPidl, aName, aMaxLen, mOption.mFileExtType);
}

xpr_bool_t ExplorerCtrl::getItemName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aName, const xpr_size_t aMaxLen, xpr_sint_t aExtensionType) const
{
    xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
    GetItemAttributes(aShellFolder, aPidl, sShellAttributes);

    return getItemName(aShellFolder, aPidl, sShellAttributes, aName, aMaxLen, aExtensionType);
}

xpr_bool_t ExplorerCtrl::getItemName(LPSHELLFOLDER  aShellFolder,
                                     LPCITEMIDLIST  aPidl,
                                     xpr_ulong_t    aShellAttributes,
                                     xpr_tchar_t   *aName,
                                     xpr_size_t     aMaxLen,
                                     xpr_sint_t     aExtensionType) const
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aName))
        return XPR_FALSE;

    if (!XPR_TEST_BITS(aShellAttributes, SFGAO_FOLDER) &&
         XPR_TEST_BITS(aShellAttributes, SFGAO_FILESYSTEM))
    {
        xpr::string sFileName;
        if (GetName(aShellFolder, aPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName) == XPR_FALSE)
            return XPR_FALSE;

        return getItemName(sFileName.c_str(), aName, aMaxLen, aExtensionType);
    }

    return GetName(aShellFolder, aPidl, SHGDN_INFOLDER, aName);
}

xpr_bool_t ExplorerCtrl::getItemName(const xpr_tchar_t *aFileName, xpr_tchar_t *aName, const xpr_size_t aMaxLen, xpr_sint_t aExtensionType) const
{
    XPR_ASSERT(aFileName != XPR_NULL);
    XPR_ASSERT(aName != XPR_NULL);

    xpr_size_t sFileNameLen = _tcslen(aFileName);
    if (sFileNameLen > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aName, aFileName);

    xpr_tchar_t *sExt = XPR_NULL;

    switch (aExtensionType)
    {
    case EXT_TYPE_HIDE:
    case EXT_TYPE_KNOWN:
        {
            sExt = (xpr_tchar_t *)GetFileExt(aName);
            if (XPR_IS_NOT_NULL(sExt))
            {
                if ((aExtensionType == EXT_TYPE_HIDE) ||
                    (aExtensionType == EXT_TYPE_KNOWN && IsFileKnownExt(sExt) == XPR_TRUE))
                {
                    *sExt = XPR_STRING_LITERAL('\0');
                }
            }
            break;
        }

    case EXT_TYPE_ALWAYS:
        {
            sExt = (xpr_tchar_t *)GetFileExt(aName);
            break;
        }
    }

    if (XPR_IS_NOT_NULL(sExt))
    {
        if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 ||
            _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
        {
            *sExt = XPR_STRING_LITERAL('\0');
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::getItemName(const xpr_tchar_t *aFileName, xpr_tchar_t *aName, const xpr_size_t aMaxLen) const
{
    return getItemName(aFileName, aName, aMaxLen, mOption.mFileExtType);
}

xpr_bool_t ExplorerCtrl::getItemName(xpr_sint_t aIndex, xpr_tchar_t *aName, const xpr_size_t aMaxLen, xpr_sint_t aExtensionType) const
{
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
        return XPR_FALSE;

    return getItemName(sLvItemData->mShellFolder, sLvItemData->mPidl, aName, aMaxLen, aExtensionType);
}

xpr_sint_t ExplorerCtrl::getFileIconIndex(LPLVITEMDATA aLvItemData, const xpr_tchar_t *aPath) const
{
    if (XPR_IS_NULL(aLvItemData))
        return -1;

    xpr_sint_t sIconIndex = -1;

    if (mOption.mCustomIcon == XPR_TRUE)
    {
        if (aLvItemData->mItemType >= IDT_DRIVE || mFolderType == FOLDER_COMPUTER)
        {
            if (XPR_IS_NOT_NULL(aPath) && _tcslen(aPath) <= 3 && aPath[1] == XPR_STRING_LITERAL(':') && aPath[2] == XPR_STRING_LITERAL('\\'))
            {
                xpr_uint_t sDriveType = GetDriveType(aPath);

                sIconIndex = 4;
                switch (sDriveType)
                {
                case DRIVE_REMOVABLE: sIconIndex = 3; break;
                case DRIVE_CDROM:     sIconIndex = 5; break;
                }
            }
        }

        if (sIconIndex == -1)
            sIconIndex = FileFilterMgr::instance().getIconIndex(aPath, aLvItemData->mShellAttributes & SFGAO_FOLDER);

        if (sIconIndex <= 0)
            sIconIndex = (aLvItemData->mShellAttributes & SFGAO_FOLDER) ? 6 : 7;
    }
    else
    {
        sIconIndex = GetItemIconIndex(aLvItemData->mShellFolder, aLvItemData->mPidl);
        if (sIconIndex < 0)
        {
            // [2008/11/06] bug patched
            // This bug is occured in v1.90 beta version.
            // The items belongs to "C:\Windows\Downloaded Program Files" folder
            // don't get aFullPidl using SH_GetRealIDL function.
            // In Addition, it don't have CFSTR_SHELLIDLIST clipboard foramt for IDataObject interface.
            // So, I use fxfile::base::Pidl::concat function.
            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(mTvItemData->mFullPidl, aLvItemData->mPidl);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                sIconIndex = GetItemIconIndex(sFullPidl);
                COM_FREE(sFullPidl);
            }
        }
    }

    return sIconIndex;
}

// caution   : SHGetDataFromIDList API function don't use becuase may not fill for full fields.
// recommend : FindFirstFile or getFileTime
// reference : SHGetDataFromIDList MSDN
xpr_bool_t ExplorerCtrl::getFileSize(LPLVITEMDATA aLvItemData, xpr_uint64_t &aFileSize) const
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    aFileSize = 0;

    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    if (!XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        return XPR_FALSE;

    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FOLDER))
        return XPR_FALSE;

    static WIN32_FIND_DATA sWin32FindData = {0};
    static xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    sPath[0] = XPR_STRING_LITERAL('\0');

    HRESULT sHResult = GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);
    if (FAILED(sHResult))
        return XPR_FALSE;

    // check drive
    if (sPath[2] == XPR_STRING_LITERAL('\0') || sPath[3] == XPR_STRING_LITERAL('\0'))
        return XPR_FALSE;

    HANDLE sFindFile = ::FindFirstFile(sPath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    if (!XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
    {
        aFileSize = sWin32FindData.nFileSizeLow;
        aFileSize += (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;
        sResult = XPR_TRUE;
    }

    ::FindClose(sFindFile);

    return sResult;
}

void ExplorerCtrl::getFileSize(LPLVITEMDATA aLvItemData, xpr_tchar_t *aFileSizeText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData) || XPR_IS_NULL(aFileSizeText))
        return;

    aFileSizeText[0] = XPR_STRING_LITERAL('\0');

    xpr_uint64_t sFileSize = 0;
    if (getFileSize(aLvItemData, sFileSize) == XPR_FALSE)
        return;

    xpr_sint_t sSizeUnit = mOption.mSizeUnit;
    switch (sSizeUnit)
    {
    case SIZE_UNIT_DEFAULT:
        SizeFormat::getDefSizeFormat(sFileSize, aFileSizeText, aMaxLen);
        break;

    case SIZE_UNIT_CUSTOM:
        SizeFormat::instance().getSizeFormat(sFileSize, aFileSizeText, aMaxLen);
        break;

    case SIZE_UNIT_NONE:
        SizeFormat::getFormatedNumber(sFileSize, aFileSizeText, aMaxLen);
        break;

    default:
        SizeFormat::getSizeUnitFormat(sFileSize, sSizeUnit, aFileSizeText, aMaxLen);
        break;
    }
}

xpr_bool_t ExplorerCtrl::getFileSize(xpr_sint_t aIndex, xpr_uint64_t &aFileSize) const
{
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
        return XPR_FALSE;

    return getFileSize(sLvItemData, aFileSize);
}

void ExplorerCtrl::getFileType(LPLVITEMDATA aLvItemData, xpr_tchar_t *aTypeName, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData))
        return;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(getFolderData()->mFullPidl, aLvItemData->mPidl);

    static SHFILEINFO sShFileInfo = {0};
    memset(&sShFileInfo, 0, sizeof(SHFILEINFO));

    ::SHGetFileInfo((const xpr_tchar_t *)sFullPidl, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_TYPENAME);

    COM_FREE(sFullPidl);

    if (sShFileInfo.szTypeName[0] == 0)
    {
        if (!XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_CANCOPY))
        {
            if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                _tcscpy(aTypeName, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.type.file_folder")));
            else
                _tcscpy(aTypeName, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.type.system_folder")));
            return;
        }
    }

    _tcscpy(aTypeName, sShFileInfo.szTypeName);
}

void ExplorerCtrl::getFileType(xpr_sint_t aIndex, xpr_tchar_t *aTypeName, const xpr_size_t aMaxLen) const
{
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
        return;

    getFileType(sLvItemData, aTypeName, aMaxLen);
}

void ExplorerCtrl::getFileAttr(LPLVITEMDATA aLvItemData, xpr_tchar_t *aFileAttributesText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData) || XPR_IS_NULL(aFileAttributesText))
        return;

    aFileAttributesText[0] = 0;

    if (mFolderType == FOLDER_DEFAULT)
    {
        if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            _tcscpy(aFileAttributesText, XPR_STRING_LITERAL("____"));

            if (XPR_TEST_BITS(aLvItemData->mFileAttributes, FILE_ATTRIBUTE_ARCHIVE))  aFileAttributesText[0] = XPR_STRING_LITERAL('A');
            if (XPR_TEST_BITS(aLvItemData->mFileAttributes, FILE_ATTRIBUTE_READONLY)) aFileAttributesText[1] = XPR_STRING_LITERAL('R');
            if (XPR_TEST_BITS(aLvItemData->mFileAttributes, FILE_ATTRIBUTE_HIDDEN))   aFileAttributesText[2] = XPR_STRING_LITERAL('H');
            if (XPR_TEST_BITS(aLvItemData->mFileAttributes, FILE_ATTRIBUTE_SYSTEM))   aFileAttributesText[3] = XPR_STRING_LITERAL('S');
        }
    }
}

void ExplorerCtrl::getFileAttr(xpr_sint_t aIndex, xpr_tchar_t *aFileAttributesText, const xpr_size_t aMaxLen) const
{
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
        return;

    getFileAttr(sLvItemData, aFileAttributesText, aMaxLen);
}

void ExplorerCtrl::getFileExtension(LPLVITEMDATA aLvItemData, xpr_tchar_t *aExt, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData) || XPR_IS_NULL(aExt))
        return;

    aExt[0] = 0;

    if (mFolderType == FOLDER_DEFAULT)
    {
        if (!XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FOLDER) &&
             XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            xpr::string sPath;
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            const xpr_tchar_t *sExt = GetFileExt(sPath);
            if (XPR_IS_NOT_NULL(sExt))
                _tcscpy(aExt, sExt + 1);
        }
    }
}

xpr_bool_t ExplorerCtrl::getDescription(LPLVITEMDATA aLvItemData, xpr_tchar_t *aDesc, const xpr_sint_t aMaxDescLen, xpr_bool_t aOriginal) const
{
    if (XPR_IS_NULL(aLvItemData) || XPR_IS_NULL(aDesc))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = aLvItemData->mShellFolder;
    LPITEMIDLIST sPidl = aLvItemData->mPidl;

    xpr_bool_t sResult = XPR_FALSE;
    IQueryInfo *sQueryInfo = XPR_NULL;
    xpr_wchar_t *sInfoTipW = XPR_NULL;
    aDesc[0] = XPR_STRING_LITERAL('\0');

    HRESULT sHResult = sShellFolder->GetUIObjectOf(
        m_hWnd, 
        1, 
        (LPCITEMIDLIST *)&sPidl, 
        IID_IQueryInfo,
        0,
        (LPVOID *)&sQueryInfo);

    if (XPR_IS_NOT_NULL(sQueryInfo) && SUCCEEDED(sHResult))
        sQueryInfo->GetInfoTip(0, &sInfoTipW);

    if (XPR_IS_NOT_NULL(sInfoTipW))
    {
        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};

        if (XPR_IS_FALSE(aOriginal) && mOption.mTooltipWithFileName == XPR_TRUE)
        {
            if (IsVirtualItem(sShellFolder, sPidl) == XPR_TRUE)
                GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);
            else
            {
                if (IsFileSystemFolder(sShellFolder, sPidl) == XPR_TRUE)
                {
                    GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);
                }
                else
                {
                    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                    GetName(sShellFolder, sPidl, SHGDN_FORPARSING, sPath);
                    PathToNormal(sPath, sName);
                }
            }
        }

        xpr_sint_t   sNameLen    = (xpr_sint_t)_tcslen(sName);
        xpr_sint_t   sInfoTipLen = (xpr_sint_t)wcslen(sInfoTipW);
        xpr_tchar_t *sInfoTip    = XPR_NULL;

        if (sInfoTipLen > 0)
        {
            if (sNameLen > 0)
            {
                _tcscat(sName, XPR_STRING_LITERAL("\r\n"));
                sNameLen = (xpr_sint_t)_tcslen(sName);
            }

            sInfoTip = new xpr_tchar_t[sInfoTipLen * 2 + 1];
            if (XPR_IS_NOT_NULL(sInfoTip))
            {
                xpr_size_t sInputBytes = wcslen(sInfoTipW) * sizeof(xpr_wchar_t);
                xpr_size_t sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
                XPR_UTF16_TO_TCS(sInfoTipW, &sInputBytes, sInfoTip, &sOutputBytes);
                sInfoTip[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
            }
        }

        xpr_sint_t sMaxDescLen = min(sInfoTipLen + sNameLen + 1, aMaxDescLen - 1);

        _tcscpy(aDesc, sName);

        if (XPR_IS_NOT_NULL(sInfoTip))
        {
            _tcsncat(aDesc, sInfoTip, sMaxDescLen - sNameLen - 1);
        }

        aDesc[sMaxDescLen] = 0;

        XPR_SAFE_DELETE_ARRAY(sInfoTip);

        sResult = XPR_TRUE;
    }

    COM_RELEASE(sQueryInfo);
    COM_FREE(sInfoTipW);

    return sResult;
}

xpr_bool_t ExplorerCtrl::getNetDescription(LPLVITEMDATA aLvItemData, xpr_tchar_t *aDesc, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData))
        return XPR_FALSE;

    if (!XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sServer[XPR_MAX_PATH + 1] = {0};
    GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sServer);
    if (sServer[0] == XPR_STRING_LITERAL('\\') && sServer[1] == XPR_STRING_LITERAL('\\'))
    {
        NETRESOURCE sNetResource = {0};
        sNetResource.dwScope       = RESOURCE_GLOBALNET;
        sNetResource.dwType        = RESOURCETYPE_ANY;
        sNetResource.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
        sNetResource.dwUsage       = RESOURCEUSAGE_CONTAINER;
        sNetResource.lpRemoteName  = sServer;

        DWORD sBufferLen = sizeof(NETRESOURCE);
        xpr_byte_t *sBuffer = new xpr_byte_t[sBufferLen];

        DWORD sNetResult = NO_ERROR;
        LPNETRESOURCE sSubNetResource = (LPNETRESOURCE)sBuffer;

        try
        {
            xpr_tchar_t *sSystem = XPR_NULL;
            while ((sNetResult = WNetGetResourceInformation(&sNetResource, sBuffer, &sBufferLen, &sSystem)) == ERROR_MORE_DATA)
            {
                XPR_SAFE_DELETE_ARRAY(sBuffer);
                sBuffer = new xpr_byte_t[sBufferLen];
            }
        }
        catch (...)
        {
        }

        if (sNetResult == NO_ERROR)
        {
            if (XPR_IS_NOT_NULL(sNetResource.lpComment))
            {
                _tcscpy(aDesc, sNetResource.lpComment);
                sResult = XPR_TRUE;
            }
        }

        XPR_SAFE_DELETE_ARRAY(sBuffer);
    }

    return sResult;
}

void ExplorerCtrl::getFileTime(LPLVITEMDATA aLvItemData, xpr_tchar_t *aModifiedTime, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aLvItemData) || XPR_IS_NULL(aModifiedTime))
        return;

    aModifiedTime[0] = XPR_STRING_LITERAL('\0');

    if (!XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_CANCOPY))
        return;

    static WIN32_FIND_DATA sWin32FindData = {0};
    static xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    sPath[0] = XPR_STRING_LITERAL('\0');

    HRESULT sHResult = GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_FORPARSING, sPath);
    if (FAILED(sHResult))
        return;

    // check drive
    if (sPath[2] == XPR_STRING_LITERAL('\0') || sPath[3] == XPR_STRING_LITERAL('\0'))
        return;

    HANDLE sFindFile = ::FindFirstFile(sPath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return;

    xpr_tchar_t sDateText[0xff] = {0};
    xpr_tchar_t sTimeText[0xff] = {0};
    FILETIME sFileTime = {0};
    SYSTEMTIME sSystemTime = {0};

    if (::FileTimeToLocalFileTime(&sWin32FindData.ftLastWriteTime, &sFileTime) == XPR_TRUE)
    {
        if (::FileTimeToSystemTime(&sFileTime, &sSystemTime) == XPR_TRUE)
        {
            const xpr_tchar_t *sDateFormat = XPR_IS_TRUE(mOption.m2YearDate)  ? XPR_STRING_LITERAL("yy-MM-dd") : XPR_STRING_LITERAL("yyyy-MM-dd");
            const xpr_tchar_t *sTimeFormat = XPR_IS_TRUE(mOption.m24HourTime) ? XPR_STRING_LITERAL("HH:mm")    : XPR_STRING_LITERAL("tt hh:mm");
            GetDateFormat(XPR_NULL, 0, &sSystemTime, sDateFormat, sDateText, 0xfe);
            GetTimeFormat(XPR_NULL, 0, &sSystemTime, sTimeFormat, sTimeText, 0xfe);
            _stprintf(aModifiedTime, XPR_STRING_LITERAL("%s %s"), sDateText, sTimeText);
        }
    }

    ::FindClose(sFindFile);
}

void ExplorerCtrl::getFileTime(xpr_sint_t aIndex, xpr_tchar_t *aModifiedTime, const xpr_size_t aMaxLen) const
{
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
        return;

    getFileTime(sLvItemData, aModifiedTime, aMaxLen);
}

void ExplorerCtrl::getRootDriveType(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_tchar_t *aDriveType, const xpr_size_t aMaxLen) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);

    if (sPath[1] == XPR_STRING_LITERAL(':'))
    {
        xpr_sint_t sDriveType = GetRootDriveType(sPath[0]);
        switch (sDriveType)
        {
        case DriveType8InchFloppyDisk:   _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.8_inch_floppy")));    break;
        case DriveType35InchFloppyDisk:  _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.3.5_inch_floppy")));  break;
        case DriveType525InchFloppyDisk: _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.5.25_inch_floppy"))); break;
        case DriveTypeRemovableDisk:     _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.removable")));        break;
        case DriveTypeLocal:             _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.local")));            break;
        case DriveTypeNetwork:           _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.network")));          break;
        case DriveTypeCdRom:             _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.cdrom")));            break;
        case DriveTypeRam:               _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.ram")));              break;

        case DriveTypeUnknown:
        default:                         _tcscpy(aDriveType, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.unknown")));          break;
        }
    }
}

xpr_bool_t ExplorerCtrl::getDriveTotalSize(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_tchar_t *aTotalSizeText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aTotalSizeText) || aMaxLen <= 0)
        return XPR_FALSE;

    aTotalSizeText[0] = 0;

    xpr_tchar_t sDrive[XPR_MAX_PATH + 1];
    GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sDrive);

    if (GetDriveType(sDrive) == DRIVE_REMOVABLE)
        return XPR_FALSE;

    xpr_uint64_t sTotalSize = 0;
    if (GetDiskTotalSize(sDrive, sTotalSize) == XPR_TRUE)
        SizeFormat::getSizeUnitFormat(sTotalSize, SIZE_UNIT_AUTO, aTotalSizeText, aMaxLen);

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::getDriveFreeSize(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_tchar_t *aFreeSizeText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aShellFolder) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aFreeSizeText) || aMaxLen <= 0)
        return XPR_FALSE;

    aFreeSizeText[0] = 0;

    xpr_tchar_t sDrive[XPR_MAX_PATH + 1];
    GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sDrive);

    if (GetDriveType(sDrive) == DRIVE_REMOVABLE)
        return XPR_FALSE;

    xpr_uint64_t sFreeSize = 0;
    if (GetDiskFreeSize(sDrive, sFreeSize) == XPR_TRUE)
        SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, aFreeSizeText, aMaxLen);

    return XPR_TRUE;
}

void ExplorerCtrl::OnKeyDown(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags) 
{
    if (aChar == VK_RETURN)
    {
        if (GetAsyncKeyState(VK_CONTROL) < 0)
        {
            execute(-1, XPR_TRUE);
        }
        else
        {
            executeAllSelItems();
        }
        return;
    }
    //else if (aChar == VK_SPACE)
    //{
    //    DoSeriesSelection();
    //    return;
    //}
    else if (aChar == VK_TAB)
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->onMoveFocus(*this);
        return;
    }

    super::OnKeyDown(aChar, aRepCnt, aFlags);
}

xpr_sint_t ExplorerCtrl::addColumn(ColumnId *aColumnId, xpr_sint_t aInsert)
{
    if (XPR_IS_NULL(aColumnId))
        return -1;

    if (aColumnId->mPropertyId <= 0)
        return -1;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return -1;

    xpr_sint_t sIndex = aInsert;
    ColumnId *sColumnId2;

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_LPARAM;

    if (aInsert == -1)
    {
        xpr_sint_t i;
        xpr_sint_t sCount = sHeaderCtrl->GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            if (sHeaderCtrl->GetItem(i, &sHdItem) == XPR_FALSE)
                break;

            sColumnId2 = (ColumnId *)sHdItem.lParam;
            if (XPR_IS_NULL(sColumnId2))
                continue;

            if (sColumnId2->mFormatId == aColumnId->mFormatId && sColumnId2->mPropertyId < aColumnId->mPropertyId)
                sIndex = i + 1;
        }

        if (sIndex < 0)
            sIndex = sCount;
    }

    LVCOLUMN sLvColumn = {0};
    sLvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;

    if (XPR_IS_TRUE(aColumnId->mFormatId.none()))
    {
        if (aColumnId->mPropertyId >= mDefColumnCount)
            return -1;

        sLvColumn.pszText = (xpr_tchar_t *)gApp.loadString(mDefColumnInfo[aColumnId->mPropertyId].mNameStringId);
        sLvColumn.fmt     = mDefColumnInfo[aColumnId->mPropertyId].mFormat;
        sLvColumn.cx      = mDefColumnInfo[aColumnId->mPropertyId].mWidth;
    }
    else
    {
        ColumnInfo *sColumnInfo = ShellColumnManager::instance().getColumnInfo(*aColumnId);
        if (XPR_IS_NULL(sColumnInfo))
            return -1;

        sLvColumn.pszText = (xpr_tchar_t *)sColumnInfo->mName.c_str();
        sLvColumn.fmt     = sColumnInfo->mAlign;
        sLvColumn.cx      = sColumnInfo->mWidth * ShellColumnManager::instance().getAvgCharWidth(this);
    }

    InsertColumn(sIndex, &sLvColumn);

    sColumnId2 = new ColumnId;
    *sColumnId2 = *aColumnId;

    sHdItem.mask   = HDI_LPARAM;
    sHdItem.lParam = (LPARAM)sColumnId2;
    GetHeaderCtrl()->SetItem(sIndex, &sHdItem);

    return sIndex;
}

xpr_bool_t ExplorerCtrl::deleteColumn(ColumnId *aColumnId, xpr_sint_t *aIndex)
{
    if (XPR_IS_NULL(aColumnId))
        return XPR_FALSE;

    if (aColumnId->mPropertyId <= 0)
        return XPR_FALSE;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return XPR_FALSE;

    xpr_sint_t sIndex = getColumnFromId(aColumnId);

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_LPARAM;
    sHeaderCtrl->GetItem(sIndex, &sHdItem);

    ColumnId *sColumnId2 = (ColumnId *)sHdItem.lParam;
    XPR_SAFE_DELETE(sColumnId2);

    if (XPR_IS_NOT_NULL(aIndex))
        *aIndex = sIndex;

    return DeleteColumn(sIndex);
}

void ExplorerCtrl::useColumn(ColumnId *aColumnId, xpr_bool_t aNotifyToObserver)
{
    if (XPR_IS_NULL(aColumnId))
        return;

    if (aColumnId->mPropertyId <= 0)
        return;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return;

    xpr_bool_t sUse = !isUseColumn(aColumnId);
    if (XPR_IS_FALSE(sUse))
    {
        xpr_sint_t sIndex;
        if (deleteColumn(aColumnId, &sIndex) == XPR_FALSE)
            return;

        if (mSortColumnId == *aColumnId)
        {
            sIndex--;

            xpr_sint_t sCount = GetHeaderCtrl()->GetItemCount();
            if (sIndex < 0) sIndex = 0;
            if (sIndex >= sCount) sIndex = sCount - 1;

            sortItems(getColumnId(sIndex));
        }
        else
        {
            resortItems();
        }
    }
    else
    {
        xpr_sint_t sIndex = addColumn(aColumnId);
        if (sIndex < 0)
            return;

        resortItems();
    }

    saveFolderLayout();

    if (XPR_IS_TRUE(aNotifyToObserver))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            FolderLayoutChange sFolderLayoutChange;
            sFolderLayoutChange.mChangeMode = FolderLayoutChange::ChangeModeColumnUse;
            sFolderLayoutChange.mColumnId   = aColumnId;

            mObserver->onFolderLayoutChange(*this, sFolderLayoutChange);
        }
    }
}

xpr_bool_t ExplorerCtrl::isUseColumn(ColumnId *aColumnId) const
{
    if (XPR_IS_NULL(aColumnId))
        return XPR_FALSE;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return XPR_FALSE;

    xpr_sint_t i;
    xpr_sint_t sCount;
    ColumnId *sColumnId2;

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_LPARAM;

    sCount = sHeaderCtrl->GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sHeaderCtrl->GetItem(i, &sHdItem);

        sColumnId2 = (ColumnId *)sHdItem.lParam;
        if (XPR_IS_NULL(sColumnId2))
            continue;

        if (*aColumnId == *sColumnId2)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

void ExplorerCtrl::loadFolderLayout(xpr_sint_t &aDefColumnCount, DefColumnInfo **aDefColumnInfo)
{
    xpr_sint_t i;
    xpr_bool_t sLoadDefault = XPR_TRUE;
    xpr_bool_t sNeedLoad = XPR_FALSE;

    if (XPR_IS_TRUE(mFirstExplore))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            mDefaultFolderLayout = new FolderLayout;

            if (mObserver->onLoadFolderLayout(const_cast<ExplorerCtrl &>(*this), FOLDER_DEFAULT, *mDefaultFolderLayout) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(mDefaultFolderLayout);
            }

            mComputerFolderLayout = new FolderLayout;

            if (mObserver->onLoadFolderLayout(const_cast<ExplorerCtrl &>(*this), FOLDER_COMPUTER, *mComputerFolderLayout) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(mComputerFolderLayout);
            }

            mVirtualFolderLayout = new FolderLayout;

            if (mObserver->onLoadFolderLayout(const_cast<ExplorerCtrl &>(*this), FOLDER_VIRTUAL, *mVirtualFolderLayout) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(mVirtualFolderLayout);
            }
        }
    }

    // load folder layout
    mFolderLayout = getFolderLayout();
    if (XPR_IS_NULL(mFolderLayout))
    {
        sNeedLoad = XPR_TRUE;
        mFolderLayout = newFolderLayout();

        XPR_ASSERT(mFolderLayout != XPR_NULL);
    }

    if (XPR_IS_TRUE(sNeedLoad) || mOption.mSaveFolderLayout == SAVE_FOLDER_LAYOUT_EACH_FOLDER)
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            if (mObserver->onLoadFolderLayout(const_cast<ExplorerCtrl &>(*this), mFolderType, *mFolderLayout) == XPR_TRUE)
            {
                sLoadDefault = XPR_FALSE;
            }
        }
    }
    else
    {
        sLoadDefault = XPR_FALSE;
    }

    // get default column info
    getDefColumnInfo(aDefColumnCount, aDefColumnInfo);

    // load default folder layout
    if (XPR_IS_TRUE(sLoadDefault))
    {
        mFolderLayout->mAllSubApply = XPR_FALSE;

        if (mFolderType == FOLDER_COMPUTER)
        {
            mFolderLayout->mViewStyle = VIEW_STYLE_DETAILS;

            mFolderLayout->mSortColumnId.mFormatId.clear();
            mFolderLayout->mSortColumnId.mPropertyId = 0;
            mFolderLayout->mSortAscending            = XPR_TRUE;
        }
        else
        {
            mFolderLayout->mViewStyle   = mOption.mDefaultViewStyle;
            mFolderLayout->mAllSubApply = XPR_FALSE;

            mFolderLayout->mSortColumnId.mFormatId.clear();
            mFolderLayout->mSortColumnId.mPropertyId = mOption.mDefaultSort;
            mFolderLayout->mSortAscending            = XPR_IS_TRUE(mOption.mDefaultSortOrder) ? XPR_TRUE : XPR_FALSE;
        }
    }

    if (XPR_IS_TRUE(sLoadDefault) || mFolderLayout->mColumnCount == 0)
    {
        xpr_sint_t sColumnCount = 0;
        for (i = 0; i < aDefColumnCount; ++i)
        {
            if (XPR_IS_TRUE((*aDefColumnInfo)[i].mDefault))
                ++sColumnCount;
        }

        mFolderLayout->mColumnCount = sColumnCount;
        mFolderLayout->mColumnItem  = new FolderLayout::ColumnItem[sColumnCount];

        xpr_sint_t j;
        for (i = 0, j = 0; i < aDefColumnCount; ++i)
        {
            if (XPR_IS_FALSE((*aDefColumnInfo)[i].mDefault))
                continue;

            mFolderLayout->mColumnItem[j].mWidth      = (*aDefColumnInfo)[i].mWidth;
            mFolderLayout->mColumnItem[j].mFormatId   = (*aDefColumnInfo)[i].mFormatId;
            mFolderLayout->mColumnItem[j].mPropertyId = (*aDefColumnInfo)[i].mPropertyId;
            j++;
        }
    }

    // validate column width
    for (i = 0; i < mFolderLayout->mColumnCount; ++i)
    {
        if (mFolderLayout->mColumnItem[i].mWidth <= 0) // auto column width is 0
        {
            if (i < aDefColumnCount)
                mFolderLayout->mColumnItem[i].mWidth = (*aDefColumnInfo)[i].mWidth;
            else
                mFolderLayout->mColumnItem[i].mWidth = DEF_COLUMN_SIZE;
        }

        if (mFolderLayout->mColumnItem[i].mWidth > MAX_COLUMN_SIZE)
            mFolderLayout->mColumnItem[i].mWidth = MAX_COLUMN_SIZE;
    }

    // first column should be 'name' column
    if (mFolderLayout->mColumnItem[0].mFormatId   != (*aDefColumnInfo)[0].mFormatId ||
        mFolderLayout->mColumnItem[0].mPropertyId != (*aDefColumnInfo)[0].mPropertyId)
    {
        mFolderLayout->mColumnItem[0].mWidth      = (*aDefColumnInfo)[0].mWidth;
        mFolderLayout->mColumnItem[0].mFormatId   = (*aDefColumnInfo)[0].mFormatId;
        mFolderLayout->mColumnItem[0].mPropertyId = (*aDefColumnInfo)[0].mPropertyId;
    }
}

void ExplorerCtrl::loadFolderLayout(void)
{
    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return;

    // delete all columns
    deleteAllColumns();

    // load folder layout
    loadFolderLayout(mDefColumnCount, &mDefColumnInfo);

    // set view style
    setViewStyle(mFolderLayout->mViewStyle, XPR_FALSE, XPR_FALSE);

    // set sort column
    mSortColumnId  = mFolderLayout->mSortColumnId;
    mSortAscending = mFolderLayout->mSortAscending;
    mSortColumn    = -1;

    // insert columns
    xpr_sint_t  i;
    xpr_sint_t  sIndex = 0;
    HDITEM      sHdItem = {0};
    LVCOLUMN    sLvColumn = {0};
    ColumnId   *sColumnId;
    ColumnInfo *sColumnInfo;
    ShellColumnManager &sShellColum = ShellColumnManager::instance();

    for (i = 0; i < mFolderLayout->mColumnCount; ++i)
    {
        if (mFolderLayout->mColumnItem[i].mPropertyId != -1)
        {
            sColumnId = new ColumnId;
            sColumnId->mFormatId   = mFolderLayout->mColumnItem[i].mFormatId;
            sColumnId->mPropertyId = mFolderLayout->mColumnItem[i].mPropertyId;

            sLvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
            sLvColumn.cx   = mFolderLayout->mColumnItem[i].mWidth;

            if (XPR_IS_TRUE(sColumnId->mFormatId.none()))
            {
                if (i >= mDefColumnCount)
                {
                    XPR_SAFE_DELETE(sColumnId);
                    continue;
                }

                sLvColumn.pszText = (xpr_tchar_t *)gApp.loadString(mDefColumnInfo[mFolderLayout->mColumnItem[i].mPropertyId].mNameStringId);
                sLvColumn.fmt     = mDefColumnInfo[mFolderLayout->mColumnItem[i].mPropertyId].mFormat;
            }
            else
            {
                sColumnInfo = sShellColum.getColumnInfo(*sColumnId);
                if (XPR_IS_NULL(sColumnInfo))
                {
                    XPR_SAFE_DELETE(sColumnId);
                    continue;
                }

                sLvColumn.pszText = (xpr_tchar_t *)sColumnInfo->mName.c_str();
                sLvColumn.fmt     = sColumnInfo->mAlign;
            }

            InsertColumn(sIndex, &sLvColumn);

            sHdItem.mask   = HDI_LPARAM;
            sHdItem.lParam = (LPARAM)sColumnId;
            sHeaderCtrl->SetItem(sIndex, &sHdItem);

            sIndex++;
        }
    }
}

void ExplorerCtrl::saveFolderLayout(void)
{
    saveFolderLayout(mOption.mSaveFolderLayout);
}

void ExplorerCtrl::saveFolderLayout(xpr_sint_t aSaveFolderLayout)
{
    if (aSaveFolderLayout == SAVE_FOLDER_LAYOUT_NONE)
        return;

    // skip, if first explored
    if (XPR_IS_TRUE(mFirstExplore))
        return;

    saveToFolderLayout(*mFolderLayout);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSaveFolderLayout(*this, mFolderType, aSaveFolderLayout, *mFolderLayout);
    }
}

void ExplorerCtrl::saveToFolderLayout(FolderLayout &aFolderLayout) const
{
    aFolderLayout.clear();
    aFolderLayout.mViewStyle     = getViewStyle();
    aFolderLayout.mSortColumnId  = mSortColumnId;
    aFolderLayout.mSortAscending = mSortAscending;
    aFolderLayout.mColumnCount   = 0;
    aFolderLayout.mColumnItem    = XPR_NULL;
    aFolderLayout.mAllSubApply   = XPR_FALSE;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NOT_NULL(sHeaderCtrl))
    {
        aFolderLayout.mColumnCount = sHeaderCtrl->GetItemCount();
    }

    if (aFolderLayout.mColumnCount > 0)
    {
        xpr_sint_t i;
        ColumnId *sColumnId;
        LVCOLUMN sLvColumn = {0};

        aFolderLayout.mColumnItem = new FolderLayout::ColumnItem[aFolderLayout.mColumnCount];

        for (i = 0; i < aFolderLayout.mColumnCount; ++i)
        {
            if (XPR_IS_TRUE(mOption.mAutoColumnWidth) && i == 0)
                continue;

            sLvColumn.mask = LVCF_WIDTH;
            GetColumn(i, &sLvColumn);

            sColumnId = getColumnId(i);
            aFolderLayout.mColumnItem[i].mWidth      = sLvColumn.cx;
            aFolderLayout.mColumnItem[i].mFormatId   = sColumnId->mFormatId;
            aFolderLayout.mColumnItem[i].mPropertyId = sColumnId->mPropertyId;
        }

        if (mOption.mAutoColumnWidth == XPR_TRUE)
        {
            aFolderLayout.mColumnItem[0].mWidth = 0;
        }
    }
}

void ExplorerCtrl::syncFolderLayout(void)
{
    if (XPR_IS_TRUE(mFirstExplore))
        return;

    mFolderLayout = getFolderLayout();
    if (XPR_IS_NULL(mFolderLayout))
    {
        mFolderLayout = newFolderLayout();

        XPR_ASSERT(mFolderLayout != XPR_NULL);
    }

    saveToFolderLayout(*mFolderLayout);
}

FolderLayout *ExplorerCtrl::getFolderLayout(FolderType aFolderType) const
{
    switch (aFolderType)
    {
    case FOLDER_DEFAULT:  return mDefaultFolderLayout;
    case FOLDER_COMPUTER: return mComputerFolderLayout;
    case FOLDER_VIRTUAL:  return mVirtualFolderLayout;

    default:
        XPR_ASSERT(0);
        break;
    }

    return XPR_NULL;
}

FolderLayout *ExplorerCtrl::getFolderLayout(void) const
{
    return getFolderLayout(mFolderType);
}

FolderLayout *ExplorerCtrl::newFolderLayout(FolderType aFolderType)
{
    FolderLayout *sFolderLayout = XPR_NULL;

    switch (aFolderType)
    {
    case FOLDER_DEFAULT:
        XPR_SAFE_DELETE(mDefaultFolderLayout);
        sFolderLayout = mDefaultFolderLayout = new FolderLayout;
        break;

    case FOLDER_COMPUTER:
        XPR_SAFE_DELETE(mComputerFolderLayout);
        sFolderLayout = mComputerFolderLayout = new FolderLayout;
        break;

    case FOLDER_VIRTUAL:
        XPR_SAFE_DELETE(mVirtualFolderLayout);
        sFolderLayout = mVirtualFolderLayout = new FolderLayout;

    default:
        XPR_ASSERT(0);
        break;
    }

    return sFolderLayout;
}

FolderLayout *ExplorerCtrl::newFolderLayout(void)
{
    return newFolderLayout(mFolderType);
}

void ExplorerCtrl::getDefColumnInfo(xpr_sint_t &aDefColumnCount, DefColumnInfo **aDefColumnInfo) const
{
    aDefColumnCount = 0;

    switch (mFolderType)
    {
    case FOLDER_DEFAULT:
        aDefColumnCount = sizeof(mDefColumnDefault) / sizeof(DefColumnInfo);
        *aDefColumnInfo = mDefColumnDefault;
        break;

    case FOLDER_COMPUTER:
        aDefColumnCount = sizeof(mDefColumnComputer) / sizeof(DefColumnInfo);
        *aDefColumnInfo = mDefColumnComputer;
        break;

    case FOLDER_VIRTUAL:
        aDefColumnCount = sizeof(mDefColumnVirtual) / sizeof(DefColumnInfo);
        *aDefColumnInfo = mDefColumnVirtual;
        break;

    default:
        XPR_ASSERT(0);
        break;
    }
}

void ExplorerCtrl::deleteAllColumns(void)
{
    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return;

    xpr_sint_t sColumnCount = sHeaderCtrl->GetItemCount();
    if (sColumnCount > 0)
    {
        xpr_sint_t i;
        ColumnId *sColumnId;
        HDITEM sHdItem = {0};
        sHdItem.mask = HDI_LPARAM;

        for (i = sColumnCount - 1; i >= 0; --i)
        {
            sHeaderCtrl->GetItem(i, &sHdItem);

            sColumnId = (ColumnId *)sHdItem.lParam;
            XPR_SAFE_DELETE(sColumnId);

            DeleteColumn(i);
        }
    }
}

void ExplorerCtrl::getColumnDataList(LPSHELLFOLDER2 &aShellFolder2, ColumnDataList &aColumnList) const
{
    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return;

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPSHELLFOLDER2 sShellFolder2 = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return;

    sHResult = sShellFolder->QueryInterface(IID_IShellFolder2, (LPVOID *)&sShellFolder2);
    if (FAILED(sHResult) || XPR_IS_NULL(sShellFolder2))
        return;

    typedef std::map<ColumnId, ColumnInfo *> ColumnInfoMap;

    ColumnInfoMap::iterator sIterator;
    ColumnInfoMap sDefColumnInfoMap;

    xpr_sint_t i;
    xpr_sint_t sDefColumnCount;
    ColumnInfo *sColumnInfo;
    DefColumnInfo *sDefColumnInfo;

    sDefColumnInfo  = mDefColumnInfo;
    sDefColumnCount = mDefColumnCount;

    if (XPR_IS_NOT_NULL(sDefColumnInfo))
    {
        for (i = 0; i < sDefColumnCount; ++i)
        {
            ColumnId sColumnId;
            sColumnId.mPropertyId = i;

            sColumnInfo = new ColumnInfo;
            sColumnInfo->mFormatId   = sColumnId.mFormatId;
            sColumnInfo->mPropertyId = sColumnId.mPropertyId;
            sColumnInfo->mName       = gApp.loadString(sDefColumnInfo[i].mNameStringId);
            sColumnInfo->mAlign      = sDefColumnInfo[i].mFormat;
            sColumnInfo->mWidth      = sDefColumnInfo[i].mWidth;

            sDefColumnInfoMap[sColumnId] = sColumnInfo;
        }
    }

    xpr_sint_t sCount;
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    ColumnItemData *sColumnItemData;
    HDITEM sHdiItem = {0};

    sCount = sHeaderCtrl->GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sHdiItem.mask       = HDI_TEXT | HDI_WIDTH | HDI_FORMAT | HDI_LPARAM;
        sHdiItem.pszText    = sText;
        sHdiItem.cchTextMax = XPR_MAX_PATH;

        if (sHeaderCtrl->GetItem(i, &sHdiItem) == XPR_FALSE)
            continue;

        ColumnId *sColumnId = (ColumnId *)sHdiItem.lParam;
        if (XPR_IS_NULL(sColumnId))
            continue;

        sColumnItemData = new ColumnItemData;
        sColumnItemData->mUse        = XPR_TRUE;
        sColumnItemData->mAlign      = sHdiItem.fmt;
        sColumnItemData->mWidth      = sHdiItem.cxy;
        sColumnItemData->mName       = sHdiItem.pszText;
        sColumnItemData->mFormatId   = sColumnId->mFormatId;
        sColumnItemData->mPropertyId = sColumnId->mPropertyId;

        aColumnList.push_back(sColumnItemData);

        // delete default column info
        sIterator = sDefColumnInfoMap.find(*sColumnId);
        if (sIterator != sDefColumnInfoMap.end())
        {
            sColumnInfo = sIterator->second;
            XPR_SAFE_DELETE(sColumnInfo);

            sDefColumnInfoMap.erase(sIterator);
        }
    }

    sIterator = sDefColumnInfoMap.begin();
    for (; sIterator != sDefColumnInfoMap.end(); ++sIterator)
    {
        sColumnInfo = sIterator->second;

        sColumnItemData = new ColumnItemData;
        sColumnItemData->mUse        = XPR_FALSE;
        sColumnItemData->mAlign      = sColumnInfo->mAlign;
        sColumnItemData->mWidth      = sColumnInfo->mWidth;
        sColumnItemData->mName       = sColumnInfo->mName;
        sColumnItemData->mFormatId   = sColumnInfo->mFormatId;
        sColumnItemData->mPropertyId = sColumnInfo->mPropertyId;

        aColumnList.push_back(sColumnItemData);

        XPR_SAFE_DELETE(sColumnInfo);
    }

    sShellFolder2->AddRef();
    aShellFolder2 = sShellFolder2;

    COM_RELEASE(sShellFolder2);
}

void ExplorerCtrl::setColumnDataList(ColumnDataList &aUseColumnList)
{
    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return;

    LVCOLUMN sLvColumn = {0};
    HDITEM sHdiItem = {0};
    sHdiItem.mask = HDI_LPARAM;

    ColumnId *sColumnId;
    xpr_sint_t i, sCount;

    xpr_bool_t sSortColumn;
    ColumnId sSortColumnId;

    sCount = sHeaderCtrl->GetItemCount();
    for (i = 1; i < sCount; ++i)
    {
        sHeaderCtrl->GetItem(1, &sHdiItem);

        sColumnId = (ColumnId *)sHdiItem.lParam;
        XPR_SAFE_DELETE(sColumnId);

        DeleteColumn(1);
    }

    xpr_bool_t sExistNameColumn = XPR_FALSE;
    if (sHeaderCtrl->GetItem(0, &sHdiItem) == XPR_TRUE)
    {
        sColumnId = (ColumnId *)sHdiItem.lParam;
        if (sColumnId)
            sExistNameColumn = (XPR_IS_TRUE(sColumnId->mFormatId.none()) && sColumnId->mPropertyId == 0) ? XPR_TRUE : XPR_FALSE;
    }

    sSortColumn   = XPR_FALSE;
    sSortColumnId = mSortColumnId;

    ColumnDataList::const_iterator sIterator = aUseColumnList.begin();
    for (i = 0; sIterator != aUseColumnList.end(); ++sIterator, ++i)
    {
        ColumnItemData *const &sColumnItemData = *sIterator;
        if (XPR_IS_NULL(sColumnItemData))
            continue;

        if (sColumnItemData->mUse != XPR_TRUE)
            continue;

        if (sExistNameColumn == XPR_TRUE)
        {
            if (XPR_IS_TRUE(sColumnItemData->mFormatId.none()) && sColumnItemData->mPropertyId == 0)
                continue;
        }

        sLvColumn.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
        sLvColumn.pszText = (xpr_tchar_t *)sColumnItemData->mName.c_str();
        sLvColumn.fmt     = sColumnItemData->mAlign;
        sLvColumn.cx      = sColumnItemData->mWidth;
        InsertColumn((xpr_sint_t)i, &sLvColumn);

        sColumnId = new ColumnId;
        sColumnId->mFormatId   = sColumnItemData->mFormatId;
        sColumnId->mPropertyId = sColumnItemData->mPropertyId;

        sHdiItem.mask   = HDI_LPARAM;
        sHdiItem.lParam = (LPARAM)sColumnId;
        sHeaderCtrl->SetItem((xpr_sint_t)i, &sHdiItem);

        if (sSortColumnId == *sColumnId)
            sSortColumn = XPR_TRUE;
    }

    if ((sSortColumn == XPR_TRUE) || (XPR_IS_TRUE(sSortColumnId.mFormatId.none()) && sSortColumnId.mPropertyId == 0))
        resortItems();
    else
        sortItems(getColumnId(0), XPR_TRUE);

    saveFolderLayout();
}

void ExplorerCtrl::setViewStyleChange(const FolderLayoutChange &aFolderLayoutChange)
{
    switch (aFolderLayoutChange.mChangeMode)
    {
    case FolderLayoutChange::ChangeModeViewStyle:
        setViewStyle(aFolderLayoutChange.mViewStyle, aFolderLayoutChange.mRefresh, XPR_FALSE);
        break;

    case FolderLayoutChange::ChangeModeColumnUse:
        useColumn(aFolderLayoutChange.mColumnId, XPR_FALSE);
        break;

    case FolderLayoutChange::ChangeModeColumnSize:
        SetColumnWidth(aFolderLayoutChange.mColumnIndex, aFolderLayoutChange.mColumnWidth);
        break;

    case FolderLayoutChange::ChangeModeSortItems:
        sortItems(aFolderLayoutChange.mColumnId, aFolderLayoutChange.mSortAscending, XPR_FALSE);
        break;
    }
}

FolderType ExplorerCtrl::getFolderType(void) const
{
    return mFolderType;
}

xpr_bool_t ExplorerCtrl::isFolderType(FolderType aFolderType) const
{
    return (mFolderType == aFolderType);
}

LPITEMIDLIST ExplorerCtrl::getDefInitFolder(void)
{
    HRESULT       sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST  sFullPidl    = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
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

    return sFullPidl;
}

void ExplorerCtrl::applyOption(Option &aNewOption)
{
    // set extended style
    DWORD sExStyle = GetExtendedStyle();

    if (aNewOption.mMouseClick == MOUSE_ONE_CLICK)
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

    XPR_SET_OR_CLR_BITS(sExStyle, LVS_EX_INFOTIP,       aNewOption.mTooltip);
    XPR_SET_OR_CLR_BITS(sExStyle, LVS_EX_GRIDLINES,     aNewOption.mGridLines);
    XPR_SET_OR_CLR_BITS(sExStyle, LVS_EX_FULLROWSELECT, aNewOption.mFullRowSelect);

    SetExtendedStyle(sExStyle);

    // enable vista enhanced control
    enableVistaEnhanced(!aNewOption.mClassicThemeStyle);

    // redraw header control
    mHeaderCtrl->Invalidate();

    // set custom text color, text background color, background color and background image
    if (XPR_IS_TRUE(aNewOption.mBkgndImage))
    {
        SetBkImage(aNewOption.mBkgndImagePath);
    }
    else
    {
        SetBkImage(XPR_STRING_LITERAL(""));
    }

    if (aNewOption.mBkgndColorType == COLOR_TYPE_CUSTOM)
    {
        SetBkColor(aNewOption.mBkgndColor);
        SetTextBkColor(aNewOption.mBkgndColor);
    }
    else
    {
        SetBkColor(::GetSysColor(COLOR_WINDOW));
        SetTextBkColor(::GetSysColor(COLOR_WINDOW));
    }

    if (aNewOption.mTextColorType == COLOR_TYPE_CUSTOM)
    {
        SetTextColor(aNewOption.mTextColor);
    }
    else
    {
        SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
    }

    // set system image list or custom image list
    ImgListManager &sImgListManager = SingletonManager::get<ImgListManager>();

    sImgListManager.loadCustomImgList(aNewOption.mCustomIconFile16,
                                      aNewOption.mCustomIconFile32,
                                      aNewOption.mCustomIconFile48,
                                      aNewOption.mCustomIconFile256);

    if (XPR_IS_TRUE(aNewOption.mCustomIcon))
    {
        setImageList(&sImgListManager.mCustomImgList16,
                     &sImgListManager.mCustomImgList32,
                     &sImgListManager.mCustomImgList48,
                     &sImgListManager.mCustomImgList256);
    }
    else
    {
        setImageList(&sImgListManager.mSysImgList16,
                     &sImgListManager.mSysImgList32,
                     &sImgListManager.mSysImgList48,
                     &sImgListManager.mSysImgList256);
    }

    // set custom font
    setCustomFont(aNewOption.mCustomFont, aNewOption.mCustomFontText);

    // set history count
    setMaxHistory(aNewOption.mHistoryCount);
    setMaxBackward(aNewOption.mBackwardCount);

    // set contents display while dragging
    setDragContents(!aNewOption.mDragNoContents);

    if (mOption.mSaveFolderLayout != aNewOption.mSaveFolderLayout)
    {
        // save current folder layout
        saveFolderLayout();
    }
    else
    {
        // save when refreshes
    }

    // set new option
    mOption = aNewOption;

    // set thumbnail options
    Thumbnail &sThumbnail = Thumbnail::instance();

    sThumbnail.setThumbSize(CSize(aNewOption.mThumbnailWidth, aNewOption.mThumbnailHeight));
    sThumbnail.setThumbPriority(aNewOption.mThumbnailPriority);

    //  update view style, if one is thumbnail
    if (getViewStyle() == VIEW_STYLE_THUMBNAIL)
        setViewStyle(VIEW_STYLE_THUMBNAIL, XPR_TRUE);
}

xpr_bool_t ExplorerCtrl::explore(LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        return XPR_FALSE;
    }

    // apply new option
    if (XPR_IS_NOT_NULL(mNewOption))
    {
        applyOption(*mNewOption);

        XPR_SAFE_DELETE(mNewOption);
    }

    xpr_bool_t sResult = exploreItem(aFullPidl, aUpdateBuddy);

    return sResult;
}

xpr_bool_t ExplorerCtrl::exploreItem(LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy)
{
    XPR_ASSERT(aFullPidl != XPR_NULL);

    xpr_bool_t    sResult            = XPR_FALSE;
    LPSHELLFOLDER sShellFolder       = XPR_NULL;
    LPSHELLFOLDER sParentShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl              = XPR_NULL;

    if (base::Pidl::getSimplePidl(aFullPidl, sParentShellFolder, sPidl) == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    if (base::Pidl::getShellFolder(sParentShellFolder, sPidl, sShellFolder) == XPR_FALSE)
    {
        COM_RELEASE(sParentShellFolder);
        return XPR_FALSE;
    }

    //CWaitCursor waitCursor;

    LPTVITEMDATA sNewTvItemData  = XPR_NULL;
    xpr_sint_t   sIndex          = 0;
    LPITEMIDLIST sEnumPidl       = XPR_NULL;
    xpr_sint_t   sEnumListType   = 0;
    xpr_sint_t   sEnumAttributes = 0;

    sNewTvItemData = new TVITEMDATA;
    memset(sNewTvItemData, 0, sizeof(TVITEMDATA));
    sNewTvItemData->mShellFolder     = sParentShellFolder;
    sNewTvItemData->mPidl            = fxfile::base::Pidl::clone(sPidl);
    sNewTvItemData->mFullPidl        = aFullPidl;
    sNewTvItemData->mShellAttributes = 0;

    switch (mOption.mListType)
    {
    case LIST_TYPE_ALL:    sEnumListType = base::ShellEnumerator::ListTypeAll;        break;
    case LIST_TYPE_FILE:   sEnumListType = base::ShellEnumerator::ListTypeOnlyFile;   break;
    case LIST_TYPE_FOLDER: sEnumListType = base::ShellEnumerator::ListTypeOnlyFolder; break;
    }

    if (XPR_IS_TRUE(mOption.mShowHiddenAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeHidden;
    if (XPR_IS_TRUE(mOption.mShowSystemAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeSystem;

    base::ShellEnumerator sShellEnumerator;

    if (sShellEnumerator.enumerate(m_hWnd, sShellFolder, sEnumListType, sEnumAttributes) == XPR_TRUE)
    {
        preEnumeration(sNewTvItemData);

        while (sShellEnumerator.next(&sEnumPidl) == XPR_TRUE)
        {
            if (insertPidlItem(sShellFolder, sEnumPidl, sIndex) == XPR_FALSE)
            {
                break;
            }

            ++sIndex;
        }

        postEnumeration(aUpdateBuddy);

        sResult = XPR_TRUE;
    }
    else
    {
        // failed to enumerate
        sResult = XPR_FALSE;
    }

    if (XPR_IS_FALSE(sResult))
    {
        COM_RELEASE(sNewTvItemData->mShellFolder);
        COM_FREE(sNewTvItemData->mPidl);
        XPR_SAFE_DELETE(sNewTvItemData);
    }

    COM_RELEASE(sShellFolder);

    return sResult;
}

void ExplorerCtrl::preEnumeration(LPTVITEMDATA aNewTvItemData)
{
    XPR_ASSERT(aNewTvItemData != XPR_NULL);

    // check to equal old folder
    xpr_bool_t sEqualFolder = XPR_FALSE;
    if (XPR_IS_NOT_NULL(mTvItemData))
    {
        sEqualFolder = (fxfile::base::Pidl::compare(aNewTvItemData->mFullPidl, mTvItemData->mFullPidl) == 0) ? XPR_TRUE : XPR_FALSE;
    }

    // save folder layout
    saveFolderLayout();

    if (XPR_IS_FALSE(sEqualFolder))
    {
        deleteAllColumns();
    }

    // free old TVITEMDATA
    if (XPR_IS_NOT_NULL(mTvItemData))
    {
        // add history
        if (XPR_IS_TRUE(mGo) && XPR_IS_FALSE(sEqualFolder))
        {
            clearForward();

            if (XPR_IS_NOT_NULL(mTvItemData->mFullPidl))
            {
                mHistory->addBackward(mTvItemData->mFullPidl);
                mHistory->addHistory(mTvItemData->mFullPidl);
            }
        }

        // free old TVITEMDATA
        COM_RELEASE(mTvItemData->mShellFolder);
        COM_FREE(mTvItemData->mPidl);
        COM_FREE(mTvItemData->mFullPidl);
        XPR_SAFE_DELETE(mTvItemData);
    }

    // replace new TVITEMDATA and get shell item attributes of folder
    mTvItemData = aNewTvItemData;
    mTvItemData->mShellAttributes = GetItemAttributes(mTvItemData->mShellFolder, mTvItemData->mPidl);

    // get current path and current tree path
    GetFullPath(mTvItemData->mFullPidl, mCurFullPath);
    GetFolderPath(mTvItemData->mShellFolder, mTvItemData->mPidl, mCurPath, mTvItemData->mShellAttributes); // Get Current Directory

    // get folder type
    mFolderType = FOLDER_DEFAULT;
    if (!XPR_TEST_BITS(mTvItemData->mShellAttributes, SFGAO_FILESYSTEM) && !XPR_TEST_BITS(mTvItemData->mShellAttributes, SFGAO_CANCOPY))
    {
        mFolderType = fxfile::base::Pidl::compare(mTvItemData->mFullPidl, CSIDL_DRIVES) ? FOLDER_VIRTUAL : FOLDER_COMPUTER;
    }

    //-------------------------------------------------------------------------

    ShellColumnManager::instance().clearAsyncColumn(mCode);

    mCodeMgr++;
    if (mCodeMgr == 0)
    {
        mCodeMgr++;
    }

    mUpdated   = XPR_FALSE;
    mSorted    = XPR_FALSE;
    mCode      = mCodeMgr;
    mSignature = 0;

    mRealSelCount       = 0;
    mRealSelFolderCount = 0;
    mRealSelFileCount   = 0;

    mInsSel.clear();

    //-------------------------------------------------------------------------

    if (gFrame->mPictureViewer != XPR_NULL && gFrame->mPictureViewer->isDocking() == XPR_TRUE)
    {
        gFrame->mPictureViewer->ShowWindow(SW_HIDE);
    }

    // hide window and lock to redraw to insert a lot of items to list view
    mVisible = IsWindowVisible();
    ShowWindow(SW_HIDE);

    SetRedraw(XPR_FALSE);

    // delete all old items
    DeleteAllItems();

    if (XPR_IS_FALSE(sEqualFolder))
    {
        // load folder layout
        loadFolderLayout();
    }

    mFirstExplore = XPR_FALSE;
}

xpr_bool_t ExplorerCtrl::insertPidlItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_sint_t aIndex)
{
    xpr_ulong_t  sShellAttributes = 0;
    DWORD        sFileAttributes  = 0;
    LPLVITEMDATA sLvItemData;

    // get shell item attributes
    getItemAttributes(aShellFolder, aPidl, sShellAttributes, sFileAttributes);

    if (mOption.mShowSystemAttribute == XPR_FALSE)
    {
        if (XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM))
        {
            COM_FREE(aPidl);
            return XPR_TRUE;
        }
    }

    // allocate new LVITEMDATA to insert to list view
    sLvItemData                   = new LVITEMDATA;
    sLvItemData->mSignature       = mSignature++;
    sLvItemData->mItemType        = IDT_SHELL;
    sLvItemData->mShellFolder     = aShellFolder;
    sLvItemData->mShellFolder2    = XPR_NULL;
    sLvItemData->mPidl            = aPidl;
    sLvItemData->mShellAttributes = sShellAttributes;
    sLvItemData->mFileAttributes  = sFileAttributes;
    sLvItemData->mThumbImageId    = Thumbnail::InvalidThumbImageId;
    aShellFolder->AddRef();

    // name hashing to find faster item by name
    insertNameHash(sLvItemData);

    // insert to list view
    static LVITEM sLvItem = {0};
    sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
    sLvItem.iItem      = aIndex;
    sLvItem.iSubItem   = 0;
    sLvItem.iImage     = I_IMAGECALLBACK;
    sLvItem.pszText    = LPSTR_TEXTCALLBACK;
    sLvItem.cchTextMax = XPR_MAX_PATH;
    sLvItem.lParam     = (LPARAM)sLvItemData;

    InsertItem(&sLvItem);

    return XPR_TRUE;
}

void ExplorerCtrl::postEnumeration(xpr_bool_t aUpdateBuddy)
{
    xpr_bool_t sAddedParentItem = XPR_FALSE;

    // watch file change
    watchFileChange();

    // [..] parent folder
    if (XPR_IS_TRUE(mOption.mParentFolder))
    {
        if (XPR_IS_FALSE(base::Pidl::isDesktopFolder(mTvItemData->mFullPidl)))
        {
            addParentItem();
            sAddedParentItem = XPR_TRUE;
        }
    }

    // add drive box item and each drvie items
    addDriveItem();

    // move to first scroll position
    EnsureVisible(0, XPR_FALSE);

    // unlock to redraw and show window
    SetRedraw();

    if (XPR_IS_TRUE(mVisible))
    {
        ShowWindow(SW_SHOW);
    }

    //-------------------------------------------------------------------------

    //waitCursor.Restore();

    // sortItems
    resortItems();
    mSorted = XPR_TRUE;
    mUpdated = XPR_TRUE;

    // adjust automatic column width
    if (XPR_IS_TRUE(mOption.mAutoColumnWidth) && getViewStyle() == VIEW_STYLE_DETAILS)
    {
        SetColumnWidth(0, LVSCW_AUTOSIZE);
    }

    // update notify message
    if (XPR_IS_NOT_NULL(mObserver))
    {
        const xpr_tchar_t *sCurPath = mCurPath.c_str();
        mObserver->onExplore(*this, mTvItemData->mFullPidl, aUpdateBuddy, sCurPath);
    }

    // update status
    updateStatus();

    // select parent folder, if go up.
    if (XPR_IS_TRUE(sAddedParentItem))
    {
        if (XPR_IS_TRUE(mSubFolder.empty()))
        {
            selectItem(0);
        }
    }
}

void ExplorerCtrl::getItemAttributes(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes, DWORD &aFileAttributes)
{
    aShellAttributes = 
        SFGAO_FILESYSTEM |
        SFGAO_FOLDER     |
        SFGAO_CANRENAME  |
        SFGAO_CANCOPY    |
        SFGAO_CANMOVE    |
        SFGAO_CANDELETE  |
        SFGAO_LINK       |
        SFGAO_SHARE      |
        SFGAO_GHOSTED;

    if (mFolderType != FOLDER_COMPUTER)
    {
        aShellAttributes |= SFGAO_READONLY;
    }

    //base::Pidl::getAttributes(aShellFolder, aPidl, sShellAttributes);

    aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &aShellAttributes);

    static HRESULT sComResult;
    static WIN32_FIND_DATA sWin32FindData;

    aFileAttributes = 0;

    if (XPR_TEST_BITS(aShellAttributes, SFGAO_FILESYSTEM))
    {
        sComResult = ::SHGetDataFromIDList(aShellFolder, aPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
        if (SUCCEEDED(sComResult))
        {
            if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_HIDDEN))
            {
                aShellAttributes |= SFGAO_GHOSTED;
            }

            if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                aShellAttributes |= SFGAO_FOLDER;
            }
            else
            {
                aShellAttributes &= ~SFGAO_FOLDER;
            }

            aFileAttributes = sWin32FindData.dwFileAttributes;
        }
    }
}

void ExplorerCtrl::watchFileChange(void)
{
    // shell change notification
    if (ShellChangeNotify::instance().modifyWatch(mShcnId, mTvItemData->mFullPidl, XPR_FALSE) == XPR_FALSE)
    {
        ShellChangeNotify::instance().unregisterWatch(mShcnId);
        mShcnId = ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, mTvItemData->mFullPidl, SHCNE_ALLEVENTS, XPR_FALSE);
    }

    // advanced file change watcher
    AdvFileChangeWatcher::AdvWatchItem *sAdvWatchItem = new AdvFileChangeWatcher::AdvWatchItem;
    if (XPR_IS_NOT_NULL(sAdvWatchItem))
    {
        sAdvWatchItem->mHwnd = m_hWnd;
        sAdvWatchItem->mMsg  = WM_ADV_FILE_CHANGE_NOTIFY;
        GetName(mTvItemData->mFullPidl, SHGDN_FORPARSING, sAdvWatchItem->mPath);
        sAdvWatchItem->mSubPath = XPR_FALSE;
    }

    if (mAdvWatchId == AdvFileChangeWatcher::InvalidAdvWatchId)
        mAdvWatchId = AdvFileChangeWatcher::instance().registerWatch(sAdvWatchItem);
    else
        mAdvWatchId = AdvFileChangeWatcher::instance().modifyWatch(mAdvWatchId, sAdvWatchItem);

    if (mAdvWatchId == AdvFileChangeWatcher::InvalidAdvWatchId)
    {
        XPR_SAFE_DELETE(sAdvWatchItem);

        // file change notification
        FileChangeWatcher::WatchItem sWatchItem;
        sWatchItem.mHwnd = m_hWnd;
        sWatchItem.mMsg  = WM_FILE_CHANGE_NOTIFY;
        GetName(mTvItemData->mFullPidl, SHGDN_FORPARSING, sWatchItem.mPath);
        sWatchItem.mSubPath = XPR_FALSE;

        if (mWatchId == FileChangeWatcher::InvalidWatchId)
            mWatchId = FileChangeWatcher::instance().registerWatch(&sWatchItem);
        else
            mWatchId = FileChangeWatcher::instance().modifyWatch(mWatchId, &sWatchItem);
    }
}

void ExplorerCtrl::addParentItem(void)
{
    LPLVITEMDATA sLvItemData = new LVITEMDATA;

    memset(sLvItemData, 0, sizeof(LVITEMDATA));
    sLvItemData->mItemType = IDT_PARENT;

    LVITEM sLvItem = {0};
    sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem      = 0;
    sLvItem.iSubItem   = 0;
    sLvItem.iImage     = -1;
    sLvItem.pszText    = LPSTR_TEXTCALLBACK;
    sLvItem.cchTextMax = XPR_MAX_PATH;
    sLvItem.lParam     = (LPARAM)sLvItemData;
    InsertItem(&sLvItem);
}

void ExplorerCtrl::addDriveItem(void)
{
    // each drive item
    if (mFolderType != FOLDER_COMPUTER && mOption.mShowDrive == XPR_TRUE)
    {
        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        GetDriveStrings(sDrive, XPR_MAX_PATH);

        LVITEM sLvItem = {0};
        sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        sLvItem.iSubItem = 0;
        sLvItem.iImage   = I_IMAGECALLBACK;

        xpr_tchar_t *sDriveEnum = sDrive;
        LPLVITEMDATA sLvItemData;
        xpr_sint_t i, sIndex;

        sIndex = GetItemCount();
        for (i = 0; *sDriveEnum; ++i)
        {
            sLvItemData = new LVITEMDATA;
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                memset(sLvItemData, 0, sizeof(LVITEMDATA));
                sLvItemData->mItemType = IDT_DRIVE + (*sDriveEnum - XPR_STRING_LITERAL('A'));

                sLvItem.iItem      = sIndex++;
                sLvItem.pszText    = LPSTR_TEXTCALLBACK;
                sLvItem.cchTextMax = XPR_MAX_PATH;
                sLvItem.lParam     = (LPARAM)sLvItemData;
                InsertItem(&sLvItem);
            }

            sDriveEnum += _tcslen(sDriveEnum) + 1;
        }
    }

    // drive seleciont item
    if (mOption.mShowDriveItem == XPR_TRUE)
    {
        LPLVITEMDATA sLvItemData = new LVITEMDATA;
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            memset(sLvItemData, 0, sizeof(LVITEMDATA));
            sLvItemData->mItemType = IDT_DRIVE_SEL;

            LVITEM sLvItem = {0};
            sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
            sLvItem.iItem      = GetItemCount();
            sLvItem.iSubItem   = 0;
            sLvItem.iImage     = I_IMAGECALLBACK;
            sLvItem.pszText    = LPSTR_TEXTCALLBACK;
            sLvItem.cchTextMax = XPR_MAX_PATH;
            sLvItem.lParam     = (LPARAM)sLvItemData;
            InsertItem(&sLvItem);
        }
    }
}

void ExplorerCtrl::getDriveItemTotalSize(xpr_tchar_t *aDrive, xpr_tchar_t *aText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aDrive) || XPR_IS_NULL(aText))
        return;

    aText[0] = XPR_STRING_LITERAL('\0');

    if (GetDriveType(aDrive) != DRIVE_FIXED)
        return;

    // total size
    xpr_uint64_t sTotalSize = 0;
    GetDiskTotalSize(aDrive, sTotalSize);
    SizeFormat::getSizeUnitFormat(sTotalSize, SIZE_UNIT_AUTO, aText, aMaxLen);
}

void ExplorerCtrl::getDriveItemFreeSize(xpr_tchar_t *aDrive, xpr_tchar_t *aText, const xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aDrive) || XPR_IS_NULL(aText))
        return;

    aText[0] = XPR_STRING_LITERAL('\0');

    if (GetDriveType(aDrive) != DRIVE_FIXED)
        return;

    // free size
    xpr_uint64_t sFreeSize = 0ui64;
    GetDiskFreeSize(aDrive, sFreeSize);

    _tcscpy(aText, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.drive.free_size_prefix")));
    xpr_size_t sPrefixLen = _tcslen(aText);

    SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, aText + sPrefixLen, aMaxLen - sPrefixLen);
    _tcscat(aText, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.drive.free_size_suffix")));
}

ColumnId *ExplorerCtrl::getColumnId(xpr_sint_t aColumnIndex) const
{
    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_LPARAM;

    if (!GetHeaderCtrl()->GetItem(aColumnIndex, &sHdItem))
        return XPR_NULL;

    return (ColumnId *)sHdItem.lParam;
}

xpr_sint_t ExplorerCtrl::getColumnFromId(ColumnId *aColumnId) const
{
    if (XPR_IS_NULL(aColumnId))
        return -1;

    CHeaderCtrl *sHeaderCtrl = GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    ColumnId *sColumnId2;

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_LPARAM;

    sCount = sHeaderCtrl->GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (!GetHeaderCtrl()->GetItem(i, &sHdItem))
            break;

        sColumnId2 = (ColumnId *)sHdItem.lParam;
        if (XPR_IS_NULL(sColumnId2))
            continue;

        if (*aColumnId == *sColumnId2)
            return i;
    }

    return -1;
}

void ExplorerCtrl::OnColumnclick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    xpr_sint_t sColumnIndex = sNmListView->iSubItem;
    sortItems(getColumnId(sColumnIndex));

    *aResult = 0;
}

void ExplorerCtrl::resortItems(void)
{
    ColumnId   sSortColumnId  = mSortColumnId;
    xpr_bool_t sSortAscending = mSortAscending;

    sortItems(&sSortColumnId, sSortAscending, XPR_FALSE);
}

void ExplorerCtrl::sortItems(ColumnId *aColumnId)
{
    xpr_bool_t sAscending = XPR_TRUE;
    if (mSortColumnId == *aColumnId)
        sAscending = !mSortAscending;

    sortItems(aColumnId, sAscending);
}

struct ExplorerCtrl::SortData
{
    ColumnId   mColumnId;
    xpr_bool_t mAscending;
    xpr_sint_t mColumn;
};

void ExplorerCtrl::sortItems(ColumnId *aColumnId, xpr_bool_t aAscending, xpr_bool_t aNotifyToObserver)
{
    if (mOption.mNoSort == XPR_TRUE)
        return;

    if (XPR_IS_NULL(aColumnId))
    {
        mSortColumnId.mFormatId.clear();
        mSortColumnId.mPropertyId = 0;
        mSortAscending            = XPR_TRUE;
        mSortColumn               = -1;
    }
    else
    {
        mSortColumnId  = *aColumnId;
        mSortAscending = aAscending;
        mSortColumn    = -1;
    }

    if (mSortColumn == -1)
    {
        if (XPR_IS_TRUE(mSortColumnId.mFormatId.none()))
            mSortColumn = mSortColumnId.mPropertyId;
        else
        {
            mSortColumn = ShellColumnManager::instance().getDetailColumn(mSortColumnId);
        }
    }

    SortData sSortData;
    sSortData.mColumnId  = mSortColumnId;
    sSortData.mAscending = mSortAscending;
    sSortData.mColumn    = mSortColumn;

    SortItems(DefaultItemCompareProc, (DWORD_PTR)&sSortData);

    setSortImage(getColumnFromId(&mSortColumnId), mSortAscending);

    if (XPR_IS_TRUE(aNotifyToObserver))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            FolderLayoutChange sFolderLayoutChange;
            sFolderLayoutChange.mChangeMode    = FolderLayoutChange::ChangeModeSortItems;
            sFolderLayoutChange.mColumnId      = aColumnId;
            sFolderLayoutChange.mSortAscending = aAscending;

            mObserver->onFolderLayoutChange(*this, sFolderLayoutChange);
        }
    }
}

const ColumnId &ExplorerCtrl::getSortColumnId(void) const
{
    return mSortColumnId;
}

xpr_bool_t ExplorerCtrl::getSortAscending(void) const
{
    return mSortAscending;
}

xpr_sint_t CALLBACK ExplorerCtrl::DefaultItemCompareProc(LPARAM aLParam1, LPARAM aLParam2, LPARAM aLParamSort)
{
    static LPLVITEMDATA sLvItemData1;
    static LPLVITEMDATA sLvItemData2;
    static xpr_bool_t sAscending;
    static HRESULT sHResult;
    static SortData *sSortData;

    sLvItemData1 = (LPLVITEMDATA)aLParam1;
    sLvItemData2 = (LPLVITEMDATA)aLParam2;
    sSortData    = (SortData   *)aLParamSort;

    sAscending  = sSortData->mAscending;
    aLParamSort = sSortData->mColumn;

    if (XPR_IS_NULL(sLvItemData1)) return -1;
    if (XPR_IS_NULL(sLvItemData2)) return 1;

    if (sLvItemData1->mItemType >= IDT_PARENT || sLvItemData2->mItemType >= IDT_PARENT)
    {
        sHResult = 0;
        if (sLvItemData1->mItemType == IDT_PARENT || sLvItemData2->mItemType == IDT_PARENT)
            sHResult = (sLvItemData1->mItemType == IDT_PARENT) ? -1 : 1; 
        else
            sHResult = (sLvItemData1->mItemType > sLvItemData2->mItemType) ? 1 : -1;

        return (xpr_sshort_t)sHResult;
    }

    ExplorerCtrl::verifyItemData(&sLvItemData1);
    ExplorerCtrl::verifyItemData(&sLvItemData2);

    if (XPR_IS_NULL(sLvItemData1)) return -1;
    if (XPR_IS_NULL(sLvItemData2)) return 1;

    if (XPR_IS_TRUE(sSortData->mColumnId.mFormatId.none()))
    {
        if (aLParamSort == 4)
        {
            sHResult = 0;
            static xpr_sint_t i;
            static DWORD sFileAttributes[4] = { FILE_ATTRIBUTE_ARCHIVE, FILE_ATTRIBUTE_READONLY, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_SYSTEM };

            if ((sLvItemData1->mShellAttributes & SFGAO_FOLDER) > (sLvItemData2->mShellAttributes & SFGAO_FOLDER))
                sHResult = -1;
            else if ((sLvItemData1->mShellAttributes & SFGAO_FOLDER) < (sLvItemData2->mShellAttributes & SFGAO_FOLDER))
                sHResult = 1;
            else
            {
                for (i = 0; i < 4; ++i)
                {
                    sHResult = 0;
                    if ((sLvItemData1->mFileAttributes & sFileAttributes[i]) > (sLvItemData2->mFileAttributes & sFileAttributes[i]))
                        sHResult = 1;
                    else if ((sLvItemData1->mFileAttributes & sFileAttributes[i]) < (sLvItemData2->mFileAttributes & sFileAttributes[i]))
                        sHResult = -1;

                    if (sHResult != 0)
                        break;
                }
            }

            if (sHResult != 0)
                return (xpr_sshort_t)sHResult * (sAscending ? 1 : -1);
            else
                aLParamSort = 0;
        }
        else if (aLParamSort == 5)
        {
            if ((sLvItemData1->mShellAttributes & SFGAO_FOLDER) > (sLvItemData2->mShellAttributes & SFGAO_FOLDER))
                sHResult = -1;
            else if ((sLvItemData1->mShellAttributes & SFGAO_FOLDER) < (sLvItemData2->mShellAttributes & SFGAO_FOLDER))
                sHResult = 1;
            else
            {
                static xpr_tchar_t sFileName1[XPR_MAX_PATH + 1];
                static xpr_tchar_t sFileName2[XPR_MAX_PATH + 1];

                sFileName1[0] = sFileName2[0] = 0;
                GetName(sLvItemData1->mShellFolder, sLvItemData1->mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName1);
                GetName(sLvItemData2->mShellFolder, sLvItemData2->mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName2);

                const xpr_tchar_t *sExt1 = GetFileExt(sFileName1);
                const xpr_tchar_t *sExt2 = GetFileExt(sFileName2);

                sHResult = 0;
                if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2))
                    sHResult = _tcsicmp(sExt1, sExt2);
                else
                {
                    if (XPR_IS_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2)) sHResult = -1;
                    if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NULL(sExt2)) sHResult = 1;
                }
            }

            if (sHResult != 0)
                return (xpr_sshort_t)sHResult * (XPR_IS_TRUE(sAscending) ? 1 : -1);
            else
                aLParamSort = 0;
        }
    }

    sHResult = sLvItemData2->mShellFolder->CompareIDs(
        aLParamSort,
        (LPCITEMIDLIST)sLvItemData1->mPidl,
        (LPCITEMIDLIST)sLvItemData2->mPidl);

    if (FAILED(sHResult))
        return 0;

    return ((xpr_sshort_t)SCODE_CODE(GetScode(sHResult)) * (sAscending ? 1 : -1));
}

void ExplorerCtrl::OnDblclk(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LPNMITEMACTIVATE sNmItemActivate = (LPNMITEMACTIVATE)aNmHdr;

    *aResult = 0;
}

void ExplorerCtrl::OnItemActivate(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMITEMACTIVATE sNmItemActivate = (LPNMITEMACTIVATE)aNmHdr;
    *aResult = 0;

    if (GetSelectedCount() == 0)
        return;

    execute(sNmItemActivate->iItem);
}

void ExplorerCtrl::executeSelFolder(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(mTvItemData->mFullPidl, aPidl);
    if (explore(sFullPidl) == XPR_FALSE)
    {
        COM_FREE(sFullPidl);
    }
}

xpr_sint_t ExplorerCtrl::executeLink(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath) == XPR_FALSE)
        return EXEC_NONE;

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sPath);
    if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
    {
        executeUrl(aShellFolder, aPidl);
        return EXEC_LINK_URL;
    }

    xpr_sint_t sResult = EXEC_NONE;

    LPITEMIDLIST sFullPidl = XPR_NULL;
    if (ResolveShortcut(GetSafeHwnd(), sPath, &sFullPidl) == XPR_TRUE && XPR_IS_NOT_NULL(sFullPidl))
    {
        LPSHELLFOLDER sShellFolder2 = XPR_NULL;
        LPCITEMIDLIST sPidl2        = XPR_NULL;
        if (fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder2, sPidl2) == XPR_TRUE)
        {
            xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
            sShellAttributes = GetItemAttributes(sShellFolder2, sPidl2);
            //if (sShellAttributes != 0 && !(sShellAttributes & FILE_ATTRIBUTE_DIRECTORY))
            //    sShellAttributes = 0;

            xpr_bool_t sSucceeded = XPR_TRUE;

            // You make a shortcut after then, if the shorcut execute on other OS environment, the path of the one may differ.
            if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
            {
                xpr_tchar_t sLink[XPR_MAX_PATH + 1] = {0};
                GetName(sShellFolder2, sPidl2, SHGDN_FORPARSING, sLink);

                if (IsExistFile(sLink) == XPR_FALSE)
                    sSucceeded = XPR_FALSE;
            }

            if (XPR_IS_TRUE(sSucceeded))
            {
                if (XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER))
                {
                    // case 2: Folder Link
                    sResult = executeLinkFolder(sFullPidl);
                }
                else
                {
                    // case 3: File Link
                    executeFile(aShellFolder, aPidl, sShellAttributes, XPR_NULL);
                    sResult = EXEC_LINK_FILE;
                }
            }
            else
            {
                doExecuteError(sShellFolder2, sPidl2);
                sResult = EXEC_ERROR;
            }
        }

        COM_RELEASE(sShellFolder2);
    }

    COM_FREE(sFullPidl);

    return sResult;
}

void ExplorerCtrl::executeUrl(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    HRESULT sHResult;
    IUniformResourceLocator *sUniformResourceLocator = XPR_NULL;

    sHResult = aShellFolder->GetUIObjectOf(
        m_hWnd,
        1,
        (LPCITEMIDLIST*)&aPidl, 
        IID_IUniformResourceLocator,
        XPR_NULL,
        reinterpret_cast<LPVOID*>(&sUniformResourceLocator));

    xpr_tchar_t *sUrl = XPR_NULL;
    if (SUCCEEDED(sHResult))
    {
        sHResult = sUniformResourceLocator->GetURL(&sUrl);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sUrl))
            NavigateURL(sUrl);

    }

    COM_RELEASE(sUniformResourceLocator);
    COM_FREE(sUrl);
}

xpr_sint_t ExplorerCtrl::executeLinkFolder(LPITEMIDLIST aFullPidl)
{
    // if linked folder is file system and not existed
    if (IsFileSystem(aFullPidl) == XPR_TRUE)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        HRESULT sHResult = GetName(aFullPidl, SHGDN_FORPARSING, sPath);
        if (SUCCEEDED(sHResult))
        {
            if (IsExistFile(sPath) == XPR_FALSE)
            {
                doExecuteError(sPath);
                return EXEC_ERROR;
            }
        }
    }

    LPITEMIDLIST sUpdateFullPidl = fxfile::base::Pidl::clone(aFullPidl);

    if (explore(sUpdateFullPidl) == XPR_FALSE)
    {
        COM_FREE(sUpdateFullPidl);

        xpr_ulong_t sShellAttributes = SFGAO_FILESYSTEM;
        GetItemAttributes(aFullPidl, sShellAttributes);

        DWORD sFlags = SHGDN_FORPARSING;
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
            sFlags = SHGDN_INFOLDER;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        HRESULT sHResult = GetName(aFullPidl, sFlags, sPath);
        if (SUCCEEDED(sHResult))
        {
            doExecuteError(sPath);
            return EXEC_ERROR;
        }
    }

    return EXEC_LINK_FOLDER;
}

void ExplorerCtrl::executeLinkFile(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
}

void ExplorerCtrl::executeFile(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t aShellAttributes, xpr_tchar_t *aStartLocation, xpr_bool_t aIsParameter)
{
    xpr_tchar_t sParameter[XPR_MAX_ARG + 1] = {0};

    if (XPR_IS_TRUE(aIsParameter) && IsVirtualItem(aShellFolder, aPidl) == XPR_FALSE)
    {
        if (XPR_TEST_BITS(aShellAttributes, SFGAO_FILESYSTEM))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);
            if (IsEqualFileExt(sPath, XPR_STRING_LITERAL(".exe")) == XPR_TRUE)
            {
                if (XPR_IS_NOT_NULL(mObserver))
                {
                    if (mObserver->onGetExecParam(*this, sPath, sParameter, XPR_MAX_ARG + 1) == XPR_FALSE)
                        return;
                }
            }
        }
    }

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(getFolderData()->mFullPidl, aPidl);

    SHELLEXECUTEINFO sShellExecuteInfo = {0};
    sShellExecuteInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
    sShellExecuteInfo.fMask        = SEE_MASK_INVOKEIDLIST;
    sShellExecuteInfo.hwnd         = m_hWnd;
    sShellExecuteInfo.nShow        = SW_SHOWDEFAULT;
    sShellExecuteInfo.lpIDList     = sFullPidl;
    sShellExecuteInfo.lpParameters = (sParameter[0] != 0) ? sParameter : XPR_NULL;
    sShellExecuteInfo.hInstApp     = ::AfxGetInstanceHandle();

    if (XPR_IS_NULL(aStartLocation) && IsVirtualItem(aShellFolder, aPidl) == XPR_FALSE)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);

        const xpr_tchar_t *sSplit = _tcsrchr(sPath, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NOT_NULL(sSplit))
        {
            sPath[sSplit - sPath] = XPR_STRING_LITERAL('\0');
            sShellExecuteInfo.lpDirectory = sPath;
        }
    }
    else
    {
        sShellExecuteInfo.lpDirectory = aStartLocation;
    }

    xpr_bool_t sSuccess = ::ShellExecuteEx(&sShellExecuteInfo);
    if (XPR_IS_FALSE(sSuccess) && (aShellAttributes & SFGAO_FILESYSTEM))
    {
        sShellExecuteInfo.lpVerb = XPR_STRING_LITERAL("openas");
        ShellExecuteEx(&sShellExecuteInfo);
    }
    else if (XPR_IS_FALSE(sSuccess) && !(aShellAttributes & SFGAO_FILESYSTEM))
    {
        doExecuteError(aShellFolder, aPidl);
    }

    if (XPR_IS_TRUE(sSuccess))
    {
        if (IsFileSystem(sFullPidl) == XPR_TRUE)
        {
            if (XPR_IS_NOT_NULL(mObserver))
            {
                mObserver->onRunFile(*this, sFullPidl);
            }
        }
    }

    COM_FREE(sFullPidl);
}

void ExplorerCtrl::doExecuteError(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    xpr_ulong_t aShellAttributes = SFGAO_FILESYSTEM;
    GetItemAttributes(aShellFolder, aPidl, aShellAttributes);

    DWORD sFlags = SHGDN_FORPARSING;
    if (XPR_TEST_BITS(aShellAttributes, SFGAO_FILESYSTEM))
        sFlags = SHGDN_INFOLDER;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetName(aShellFolder, aPidl, sFlags, sPath);

    doExecuteError(sPath);
}

void ExplorerCtrl::doExecuteError(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onExecError(*this, aPath);
}

xpr_sint_t ExplorerCtrl::execute(xpr_sint_t aIndex, xpr_bool_t aParamExec)
{
    // case 1: Folder
    // case 2: Folder(.lnk)
    // case 3: File(.lnk)
    // case 4: URL(.url)
    // case 5: File
    // case 6: Parent Folder
    // case 7: Each Drive
    // case 8: Drive Selection

    if (aIndex == -1)
    {
        aIndex = GetSelectionMark();
    }

    xpr_sint_t sResult = 0;
    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NULL(sLvItemData))
    {
        DWORD sItemDataType = getItemDataType(aIndex);
        if (sItemDataType == IDT_PARENT)
        {
            goUp();
            sResult = EXEC_PARENT_FOLDER;
        }
        else if (IDT_DRIVE <= sItemDataType && sItemDataType < IDT_DRIVE_SEL)
        {
            xpr_tchar_t sDriveChar = getDriveFromItemType(sItemDataType);
            gFrame->gotoDrive(sDriveChar);

            sResult = EXEC_EACH_DRIVE;
        }
        else if (sItemDataType == IDT_DRIVE_SEL)
        {
            if (mObserver != XPR_NULL)
            {
                mObserver->onGoDrive(*this);
            }

            sResult = EXEC_DRIVE_SELECTION;
        }

        return sResult;
    }

    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_INFOLDER, sName);

    // Because of zip file on Windows XP
    xpr_bool_t sFolder = XPR_FALSE;
    if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
    {
        sFolder = XPR_TRUE;
        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            WIN32_FIND_DATA sWin32FindData = {0};
            HRESULT sHResult = ::SHGetDataFromIDList(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
            if (SUCCEEDED(sHResult))
            {
                sFolder = XPR_FALSE;
                if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                    sFolder = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_TRUE(sFolder))
    {
        executeSelFolder(sLvItemData->mShellFolder, sLvItemData->mPidl);
        sResult = EXEC_FOLDER;
    }
    else
    {
        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_LINK))
        {
            executeLink(sLvItemData->mShellFolder, sLvItemData->mPidl);
            sResult = EXEC_LINK_FOLDER;
        }
        else
        {
            xpr_tchar_t *sStartLocation = XPR_NULL;
            if (sLvItemData->mShellAttributes & SFGAO_FILESYSTEM)
                sStartLocation = (xpr_tchar_t *)getCurPath();

            executeFile(sLvItemData->mShellFolder, sLvItemData->mPidl, sLvItemData->mShellAttributes, sStartLocation, aParamExec);
            sResult = EXEC_FILE;
        }
    }

    return sResult;
}

void ExplorerCtrl::executeAllSelItems(void)
{
    xpr_sint_t sExec;
    xpr_bool_t sBreak;

    xpr_sint_t sIndex;
    LPLVITEMDATA sLvItemData;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);

        sExec = execute(sIndex);

        sBreak = XPR_FALSE;
        switch (sExec)
        {
        case EXEC_FOLDER:
        case EXEC_LINK_FOLDER:
        case EXEC_PARENT_FOLDER:
        case EXEC_EACH_DRIVE:
        case EXEC_DRIVE_SELECTION:
            {
                sBreak = XPR_TRUE;
                break;
            }
        }

        if (XPR_IS_TRUE(sBreak))
            break;

        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER) || XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_LINK))
                break;
        }
    }
}

void ExplorerCtrl::OnContextMenu(CWnd *aWnd, CPoint aPoint) 
{
    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    const DWORD sStyle = GetStyle();
    const xpr_bool_t bHorzScrl = XPR_TEST_BITS(sStyle, WS_HSCROLL);
    const xpr_bool_t bVertScrl = XPR_TEST_BITS(sStyle, WS_VSCROLL);

    CRect rcHorzScrl(&sWindowRect);
    CRect rcVertScrl(&sWindowRect);
    rcHorzScrl.top  = rcHorzScrl.bottom - GetSystemMetrics(SM_CYHSCROLL);
    rcVertScrl.left = rcVertScrl.right  - GetSystemMetrics(SM_CXVSCROLL);

    if ((bHorzScrl && rcHorzScrl.PtInRect(aPoint)) ||
        (bVertScrl && rcVertScrl.PtInRect(aPoint)))
    {
        // default scroll bar
        super::OnContextMenu(aWnd, aPoint);
        return;
    }

    CRect rcHeader;
    GetHeaderCtrl()->GetWindowRect(rcHeader);

    if (GetHeaderCtrl()->IsWindowVisible() && rcHeader.PtInRect(aPoint))
    {
        OnContextMenuHeader(aWnd, aPoint, sWindowRect);
        return;
    }

    xpr_bool_t sResult = XPR_FALSE;

    if (GetSelectedCount() > 0)
    {
        if (OnContextMenuShell(aWnd, aPoint, sWindowRect) == XPR_FALSE)
        {
            xpr_sint_t sIndex = GetSelectionMark();
            DWORD sItemDataType = getItemDataType(sIndex);

            if (sItemDataType == IDT_PARENT)
            {
                OnContextMenuParent(aWnd, aPoint, sWindowRect);
            }
            else if (IDT_DRIVE <= sItemDataType && sItemDataType < IDT_DRIVE_SEL)
            {
                OnContextMenuDrive(aWnd, aPoint, sWindowRect, sItemDataType);
            }
        }

        mHeaderCtrl->Invalidate(XPR_FALSE);

        return;
    }
    else if (GetKeyState(VK_CONTROL) < 0)
    {
        sResult = OnContextMenuShellBkgnd(aWnd, aPoint, sWindowRect);
    }

    if (XPR_IS_FALSE(sResult))
    {
        LPTVITEMDATA sTvItemData = getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            OnContextMenuBkgnd(aWnd, aPoint, sWindowRect);
        }
        else
        {
            OnContextMenuBkgndNone(aWnd, aPoint, sWindowRect);
        }
    }
}

xpr_bool_t ExplorerCtrl::OnContextMenuShell(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    if (GetSelectedCount() <= 0)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST *sPidls = XPR_NULL;
    xpr_sint_t sCount = 0;

    getSelItemData(&sShellFolder, &sPidls, sCount);

    if (sCount != 0)
    {
        if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
        {
            CRect sRect;
            xpr_sint_t sFocus = GetSelectionMark();
            GetItemRect(sFocus, sRect, LVIR_BOUNDS);
            ClientToScreen(&sRect);

            aPoint.x = sRect.left;
            aPoint.y = sRect.bottom;
        }

        xpr_tchar_t sVerb[0xff] = {0};
        xpr_bool_t  sInvokeCommandSelf = XPR_FALSE;
        ContextMenu sContextMenu(GetSafeHwnd());

        if (sContextMenu.init(sShellFolder, sPidls, sCount) == XPR_TRUE)
        {
            xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &aPoint);
            if (sId != -1)
            {
                sId -= sContextMenu.getFirstId();

                if (sContextMenu.getCommandVerb(sId, sVerb, 0xfe) == XPR_TRUE)
                {
                    sInvokeCommandSelf = canInvokeCommandSelf(sVerb);
                }

                if (XPR_IS_FALSE(sInvokeCommandSelf))
                {
                    sContextMenu.invokeCommand(sId);
                }
            }
        }

        sContextMenu.destroy();

        if (XPR_IS_TRUE(sInvokeCommandSelf))
        {
            invokeCommandSelf(sContextMenu, sVerb);
        }

        sResult = XPR_TRUE;
    }

    XPR_SAFE_DELETE_ARRAY(sPidls);

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnContextMenuParent(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NOT_NULL(sTvItemData) && XPR_IS_NOT_NULL(sTvItemData->mFullPidl))
    {
        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sTvItemData->mFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
        {
            if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
            {
                CRect sRect;
                xpr_sint_t sFocus = GetSelectionMark();
                GetItemRect(sFocus, sRect, LVIR_BOUNDS);
                ClientToScreen(&sRect);

                aPoint.x = sRect.left;
                aPoint.y = sRect.bottom;
            }

            ContextMenu sContextMenu(GetSafeHwnd());
            if (sContextMenu.init(sShellFolder, &sPidl, 1))
            {
                xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &aPoint, CMF_EXPLORE);
                if (sId != -1)
                {
                    sId -= sContextMenu.getFirstId();

                    xpr_tchar_t sVerb[0xff] = {0};
                    sContextMenu.getCommandVerb(sId, sVerb, 0xfe);

                    xpr_bool_t sSelfInvoke = XPR_FALSE;

                    if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
                    {
                        execute(GetSelectionMark());
                        sSelfInvoke = XPR_TRUE;
                    }

                    if (XPR_IS_FALSE(sSelfInvoke))
                        sContextMenu.invokeCommand(sId);
                }
            }

            sContextMenu.destroy();
        }

        COM_RELEASE(sShellFolder);
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnContextMenuDrive(CWnd *aWnd, CPoint aPoint, CRect aWindowRect, DWORD aItemDataType)
{
    xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
    _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(aItemDataType));

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
        {
            if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
            {
                CRect sRect;
                xpr_sint_t sFocus = GetSelectionMark();
                GetItemRect(sFocus, sRect, LVIR_BOUNDS);
                ClientToScreen(&sRect);

                aPoint.x = sRect.left;
                aPoint.y = sRect.bottom;
            }

            ContextMenu sContextMenu(GetSafeHwnd());
            if (sContextMenu.init(sShellFolder, &sPidl, 1))
            {
                xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &aPoint, CMF_EXPLORE);
                if (sId != -1)
                {
                    sId -= sContextMenu.getFirstId();

                    xpr_tchar_t sVerb[0xff] = {0};
                    sContextMenu.getCommandVerb(sId, sVerb, 0xfe);

                    xpr_bool_t sSelfInvoke = XPR_FALSE;

                    if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
                    {
                        execute(GetSelectionMark());
                        sSelfInvoke = XPR_TRUE;
                    }

                    if (XPR_IS_FALSE(sSelfInvoke))
                        sContextMenu.invokeCommand(sId);
                }
            }

            sContextMenu.destroy();
        }

        COM_RELEASE(sShellFolder);
        COM_FREE(sFullPidl);
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnContextMenuShellBkgnd(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    HRESULT sHResult = sTvItemData->mShellFolder->BindToObject(
        (LPCITEMIDLIST)sTvItemData->mPidl,
        0,
        IID_IShellFolder,
        reinterpret_cast<LPVOID*>(&sShellFolder));

    if (SUCCEEDED(sHResult))
    {
        if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
        {
            aPoint.x = aWindowRect.left;
            aPoint.y = aWindowRect.top;
        }

        ContextMenu::trackBackMenu(sShellFolder, &aPoint, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, GetSafeHwnd());
        sResult = XPR_TRUE;
    }

    COM_RELEASE(sShellFolder);

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnContextMenuBkgnd(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_EXP_FILESYSTEM_POPUP) == XPR_FALSE)
        return XPR_FALSE;

    BCMenu *sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
    if (XPR_IS_NULL(sPopupMenu))
        return XPR_FALSE;

    BCMenu *sSubMenu = XPR_NULL;
    if (mOption.mFileScrapContextMenu == XPR_TRUE)
        sSubMenu = (BCMenu *)sPopupMenu->GetSubMenu(14);
    else
    {
        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_CUR_COPY,        MF_BYCOMMAND);
        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_CUR_MOVE,        MF_BYCOMMAND);
        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_DELETE,          MF_BYCOMMAND);
        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_VIEW,            MF_BYCOMMAND);
        sPopupMenu->DeleteMenu(ID_EDIT_FILE_SCRAP_REMOVE_ALL_LIST, MF_BYCOMMAND);
        sPopupMenu->DeleteMenu(8, MF_BYPOSITION); // seperator

        sSubMenu = (BCMenu *)sPopupMenu->GetSubMenu(8);
    }

    if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
    {
        aPoint.x = aWindowRect.left;
        aPoint.y = aWindowRect.top;
    }

    sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
    sMenu.DestroyMenu();

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnContextMenuBkgndNone(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_EXP_NONFILESYSTEM_POPUP) == XPR_FALSE)
        return XPR_FALSE;

    BCMenu* sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
    if (XPR_IS_NULL(sPopupMenu))
        return XPR_FALSE;

    if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
    {
        aPoint.x = aWindowRect.left;
        aPoint.y = aWindowRect.top;
    }

    sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
    sMenu.DestroyMenu();

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnContextMenuHeader(CWnd *aWnd, CPoint aPoint, CRect aWindowRect)
{
    if (mFolderType == FOLDER_DEFAULT)
    {
        BCMenu sMenu;
        if (sMenu.LoadMenu(IDR_EXP_COLUMN) == XPR_TRUE)
        {
            BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
            if (XPR_IS_NOT_NULL(sPopupMenu))
            {
                if (aWindowRect.PtInRect(aPoint) == XPR_FALSE)
                {
                    aPoint.x = aWindowRect.left;
                    aPoint.y = aWindowRect.top;
                }

                sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint.x, aPoint.y, AfxGetMainWnd());
            }
        }
    }

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::canInvokeCommandSelf(const xpr_tchar_t *aVerb)
{
    XPR_ASSERT(aVerb != XPR_NULL);

    xpr_bool_t sResult = XPR_FALSE;

    if (_tcsicmp(aVerb, CMID_VERB_OPEN) == 0)
    {
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(GetSelectionMark());
        if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            sResult = XPR_TRUE;
        }
    }
    else if (_tcsicmp(aVerb, CMID_VERB_DELETE) == 0)
    {
        sResult = XPR_TRUE;
    }
    else if (_tcsicmp(aVerb, CMID_VERB_RENAME) == 0)
    {
        sResult = XPR_TRUE;
    }
    else if (_tcsicmp(aVerb, CMID_VERB_PASTE) == 0)
    {
        sResult = XPR_TRUE;
    }

    return XPR_FALSE;
}

void ExplorerCtrl::invokeCommandSelf(ContextMenu &aContextMenu, const xpr_tchar_t *aVerb)
{
    XPR_ASSERT(aVerb != XPR_NULL);

    if (_tcsicmp(aVerb, CMID_VERB_OPEN) == 0)
    {
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(GetSelectionMark());
        if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            execute(GetSelectionMark());
        }
    }
    else if (_tcsicmp(aVerb, CMID_VERB_DELETE) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuDelete(*this);
        }
    }
    else if (_tcsicmp(aVerb, CMID_VERB_RENAME) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuRename(*this);
        }
    }
    else if (_tcsicmp(aVerb, CMID_VERB_PASTE) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuPaste(*this);
        }
    }
    else
    {
        XPR_ASSERT(0);
    }
}

void ExplorerCtrl::getSelItemData(LPSHELLFOLDER *aShellFolder, LPCITEMIDLIST **aPidls, xpr_sint_t &aCount, xpr_ulong_t aShellAttributes) const
{
    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount > 0)
    {
        aCount = 0;
        *aPidls = new LPCITEMIDLIST[sSelCount];

        xpr_sint_t sIndex;
        LPLVITEMDATA sLvItemData = XPR_NULL;

        POSITION sPosition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = GetNextSelectedItem(sPosition);

            sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
            if ((XPR_IS_NOT_NULL(sLvItemData)) && (sLvItemData->mShellAttributes & aShellAttributes || aShellAttributes == 0))
            {
                if (!(*aShellFolder))
                    *aShellFolder = sLvItemData->mShellFolder;
                (*aPidls)[aCount++] = sLvItemData->mPidl;
            }
        }
    }
    else
    {
        *aShellFolder = XPR_NULL;
        *aPidls = XPR_NULL;
        aCount = 0;
    }
}

xpr_bool_t ExplorerCtrl::getSelItemPath(xpr_tchar_t **aPaths, xpr_sint_t &aCount, xpr_ulong_t aShellAttributes) const
{
    xpr_sint_t sSelCount = GetSelectedCount();
    xpr_tchar_t *sSource = new xpr_tchar_t[XPR_MAX_PATH * sSelCount + 1];
    *sSource = 0;

    xpr_sint_t sIndex;
    xpr_sint_t sSourceCount = 0;
    xpr_tchar_t *sSourceEnum = sSource;
    LPLVITEMDATA sLvItemData = XPR_NULL;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sLvItemData) && aShellAttributes != 0 && sLvItemData->mShellAttributes & aShellAttributes)
        {
            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sSourceEnum);
            sSourceEnum += _tcslen(sSourceEnum) + 1;
            sSourceCount++;
        }
    }
    *sSourceEnum = XPR_STRING_LITERAL('\0');

    if (sSourceCount > 0)
    {
        *aPaths = sSource;
        aCount = sSourceCount;
        return XPR_TRUE;
    }

    XPR_SAFE_DELETE_ARRAY(sSource);
    return XPR_FALSE;
}

void ExplorerCtrl::refresh(void)
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return;

    if (explore(sFullPidl) == XPR_FALSE)
    {
        COM_FREE(sFullPidl);
    }
}

xpr_sint_t ExplorerCtrl::getClipFormatCount(void) const
{
    return 4;
}

void ExplorerCtrl::getClipFormat(CLIPFORMAT *aClipFormat) const
{
    if (XPR_IS_NULL(aClipFormat))
        return;

    ClipFormat &sClipFormat = ClipFormat::instance();

    aClipFormat[0] = sClipFormat.mShellIDList;
    aClipFormat[1] = sClipFormat.mDropEffect;
    aClipFormat[2] = sClipFormat.mFileName;
    aClipFormat[3] = sClipFormat.mFileNameW;
}

CLIPFORMAT ExplorerCtrl::getClipFormatDropEffect(void) const
{
    ClipFormat &sClipFormat = ClipFormat::instance();
    return sClipFormat.mDropEffect;
}

CLIPFORMAT ExplorerCtrl::getClipFormatShellIDList(void) const
{
    ClipFormat &sClipFormat = ClipFormat::instance();
    return sClipFormat.mShellIDList;
}

xpr_bool_t ExplorerCtrl::getSelDataObject(LPDATAOBJECT *aDataObject, xpr_uint_t aShellAttributes, DROPEFFECT *aOkDropEffect)
{
    xpr_uint_t sSelCount = GetSelectedCount();
    if (sSelCount <= 0)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    LPLVITEMDATA   sLvItemData = XPR_NULL;
    LPSHELLFOLDER  sShellFolder = XPR_NULL;
    LPITEMIDLIST  *sPidls = XPR_NULL;
    xpr_uint_t     i, sCount = 0;
    HRESULT        sHResult;

    sPidls = new LPITEMIDLIST[sSelCount];

    if (aShellAttributes == SFGAO_CANMOVE || aShellAttributes == SFGAO_CANCOPY)
    {
        sCount = GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            sLvItemData = (LPLVITEMDATA)GetItemData(i);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                if ((sLvItemData->mShellAttributes & SFGAO_GHOSTED) != SFGAO_GHOSTED)
                    SetItemState(i, 0, LVIS_CUT);
            }
        }
    }
    sCount = 0;

    if (aOkDropEffect != 0)
        *aOkDropEffect = DROPEFFECT_LINK | DROPEFFECT_COPY | DROPEFFECT_MOVE;

    xpr_sint_t sIndex;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            if (!XPR_ANY_BITS(sLvItemData->mShellAttributes, aShellAttributes) && aShellAttributes != 0)
            {
                sCount = 0;
                break;
            }

            if (aOkDropEffect != 0)
            {
                if ((sLvItemData->mShellAttributes & SFGAO_CANMOVE) != SFGAO_CANMOVE)
                    *aOkDropEffect &= ~SFGAO_CANMOVE;

                if ((sLvItemData->mShellAttributes & SFGAO_CANCOPY) != SFGAO_CANCOPY)
                    *aOkDropEffect &= ~SFGAO_CANCOPY;
            }

            if (aShellAttributes == SFGAO_CANMOVE)
                SetItemState(sIndex, LVIS_CUT, LVIS_CUT);

            sPidls[sCount++] = (LPITEMIDLIST)sLvItemData->mPidl;
            sShellFolder = sLvItemData->mShellFolder;
        }
        else
        {
            SetItemState(sIndex, 0, LVIS_SELECTED);
        }
    }

    if (sCount > 0 && XPR_IS_NOT_NULL(sShellFolder))
    {
        sHResult = sShellFolder->GetUIObjectOf(
            m_hWnd,
            sCount,
            (LPCITEMIDLIST *)sPidls,
            IID_IDataObject,
            0,
            (LPVOID *)aDataObject);

        if (SUCCEEDED(sHResult))
            sResult = XPR_TRUE;
    }

    XPR_SAFE_DELETE_ARRAY(sPidls);

    return sResult;
}

CImageList* ExplorerCtrl::createDragImageEx(LPPOINT aPoint)
{
    CRect sClientRect;
    GetClientRect(&sClientRect);
    CRect sSingleRect;
    CRect sCompleteRect(0,0,0,0);
    xpr_sint_t sIndex;

    // Calcualte all area of the selected items.
    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        GetItemRect(sIndex, sSingleRect, LVIR_BOUNDS);
        sCompleteRect.UnionRect(sCompleteRect, sSingleRect);
    }

    if (XPR_IS_NOT_NULL(aPoint))
    {
        CPoint sCursorPoint;
        GetCursorPos(&sCursorPoint);
        ScreenToClient(&sCursorPoint);
        aPoint->x = sCursorPoint.x - sCompleteRect.left;
        aPoint->y = sCursorPoint.y - sCompleteRect.top;
    }

    CClientDC sDC(this);
    CDC sMemDC;
    sMemDC.CreateCompatibleDC(&sDC);
    CBitmap sBitmap, *sOldBitmap;
    sBitmap.CreateCompatibleBitmap(&sDC, sCompleteRect.Width(), sCompleteRect.Height());
    sOldBitmap = sMemDC.SelectObject(&sBitmap);
    sMemDC.FillSolidRect(0,0,sCompleteRect.Width(), sCompleteRect.Height(), RGB(0, 255, 0)); 

    CImageList *sSingleImageList = XPR_NULL;
    CPoint sPoint;

    sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        GetItemRect(sIndex, sSingleRect, LVIR_BOUNDS);

        sSingleImageList = CreateDragImage(sIndex, &sPoint);
        if (XPR_IS_NOT_NULL(sSingleImageList))
        {
            IMAGEINFO sImageInfo;
            sSingleImageList->GetImageInfo(0, &sImageInfo);
            sSingleRect.right = sSingleRect.left + (sImageInfo.rcImage.right - sImageInfo.rcImage.left);

            sSingleImageList->DrawIndirect(
                &sMemDC, 
                0, 
                CPoint(sSingleRect.left-sCompleteRect.left, sSingleRect.top-sCompleteRect.top),
                sSingleRect.Size(), 
                CPoint(0,0));

            delete sSingleImageList;
        }
    }

    CDC sDragDC;
    sDragDC.CreateCompatibleDC(&sDC);
    CBitmap sDragBitmap, *sOldDragBitmap;
    sDragBitmap.CreateCompatibleBitmap(&sDC, 200, 200);
    sOldDragBitmap = sDragDC.SelectObject(&sDragBitmap);
    sDragDC.FillSolidRect(CRect(0,0,200,200), RGB(0, 255, 0)); 

    xpr_tchar_t sTitle[0xff] = {0};
    _stprintf(sTitle, gApp.loadFormatString(XPR_STRING_LITERAL("explorer_window.drag_drop.title"), XPR_STRING_LITERAL("%d")), GetSelectedCount());
    CFont sFont, *sOldFont;
    sFont.CreateStockObject(DEFAULT_GUI_FONT);
    sOldFont = sDragDC.SelectObject(&sFont);
    sDragDC.SetBkMode(TRANSPARENT);
    sDragDC.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    sDragDC.FillRect(CRect(0,0,200,16), &CBrush::CBrush(GetSysColor(COLOR_HIGHLIGHT)));
    sDragDC.DrawText(sTitle, CRect(5,0,158,16), DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_VCENTER);

    sDragDC.BitBlt(0, 18, 200, 182, &sMemDC, 0, 0, SRCCOPY);

    sMemDC.SelectObject(sOldBitmap);
    sDragDC.SelectObject(sOldDragBitmap);

    CImageList *sCompleteImageList = new CImageList;
    sCompleteImageList->Create(200, 200, ILC_COLORDDB | ILC_MASK, 0, 1);
    sCompleteImageList->Add(&sDragBitmap, RGB(0, 255, 0)); 

    sBitmap.DeleteObject();
    sDragBitmap.DeleteObject();

    return sCompleteImageList;
}

//----------------------------------------------------------------------------------------------------------
// OLE Drag & Drop
//----------------------------------------------------------------------------------------------------------
void ExplorerCtrl::OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    if ((mOption.mDragType == DRAG_START_DEFAULT) ||
        (mOption.mDragType == DRAG_START_CTRL && GetAsyncKeyState(VK_CONTROL) < 0))
    {
        beginDragDrop(sNmListView);
    }

    *aResult = 0;
}

void ExplorerCtrl::OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    if ((mOption.mDragType == DRAG_START_DEFAULT) ||
        (mOption.mDragType == DRAG_START_CTRL && GetAsyncKeyState(VK_CONTROL) < 0))
    {
        beginDragDrop(sNmListView);
    }

    *aResult = 0;
}

void ExplorerCtrl::beginDragDrop(NM_LISTVIEW *aNmListView)
{
    LPDATAOBJECT sDataObject = XPR_NULL;
    DWORD sDropEffect;
    DWORD sOkDropEffect = DROPEFFECT_LINK;

    if (getSelDataObject(&sDataObject, 0, &sOkDropEffect) == XPR_TRUE)
    {
        LPDROPSOURCE sDataSource = new DropSource;

        if (XPR_IS_NOT_NULL(sDataSource))
        {
            DragImage &sDragImage = DragImage::instance();

            if (mOption.mDragNoContents == XPR_FALSE)
            {
                if (sDragImage.beginDrag(m_hWnd, XPR_NULL, sDataObject) == XPR_FALSE)
                {
                    CPoint sPoint(8,8);
                    CImageList *sMyDragImage = createDragImageEx(&sPoint);
                    if (XPR_IS_NOT_NULL(sMyDragImage))
                    {
                        sDragImage.beginDrag(m_hWnd, sMyDragImage, sDataObject);
                    }
                }
            }

            mIntraDrag = XPR_TRUE;
            mDropIndex = -1;

            ::DoDragDrop(sDataObject, sDataSource, sOkDropEffect, &sDropEffect);

            sDragImage.endDrag();

            mIntraDrag = XPR_FALSE;
        }

        COM_RELEASE(sDataSource);
    }

    COM_RELEASE(sDataObject);

    //SHDRAGIMAGE sShDragImage;
    //HBITMAP     sBitmapHandle;
    //BITMAP      sBitmap;

    //IMAGEINFO sImageInfo;
    //pFrame->mDragImage->GetImageInfo(0, &sImageInfo);
    //GetObject(sBitmapHandle, sizeof(sBitmap), &sBitmap);
    //sShDragImage.sizeDragImage.cx = sBitmap.bmWidth;
    //sShDragImage.sizeDragImage.cy = sBitmap.bmHeight;
    //sShDragImage.hbmpDragImage = sImageInfo.hbmImage;
    //sShDragImage.crColorKey = RGB(0,250,0); //transparent color
    //sShDragImage.ptOffset = CPoint(-20,0);

    //pDragSourceHelper->InitializeFromBitmap(&sShDragImage, sDataObject);
}

DROPEFFECT ExplorerCtrl::OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->DragEnter(GetDesktopWindow(), aPoint);
            sMyDragImage->DragShowNolock(XPR_TRUE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint sDragPoint(aPoint);
            IDataObject *sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
            mDropTarget.getDropHelper()->DragEnter(GetSafeHwnd(), sDataObject, &sDragPoint, sDropEffect);
        }
    }

    return sDropEffect;
}

void ExplorerCtrl::OnDragLeave(void)
{
    xpr_sint_t sTimerId;
    for (sTimerId = TM_ID_DRAG_SCROLL_BEGIN; sTimerId < TM_ID_DRAG_SCROLL_END; ++sTimerId)
        KillTimer(sTimerId);

    mScrollUpTimer    = XPR_FALSE;
    mScrollDownTimer  = XPR_FALSE;
    mScrollLeftTimer  = XPR_FALSE;
    mScrollRightTimer = XPR_FALSE;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        sDragImage.showMyDragNoLock(XPR_FALSE);
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    SetItemState(mDropIndex, 0, LVIS_DROPHILITED);
    RedrawItems(mDropIndex, mDropIndex);
    UpdateWindow();

    mDropIndex = -1;
}

DROPEFFECT ExplorerCtrl::OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_NONE;

    ClipFormat &sClipFormat = ClipFormat::instance();

    if (aOleDataObject->IsDataAvailable((CLIPFORMAT)sClipFormat.mShellIDList)  == XPR_FALSE &&
        aOleDataObject->IsDataAvailable(CF_HDROP)                              == XPR_FALSE &&
        aOleDataObject->IsDataAvailable((CLIPFORMAT)sClipFormat.mFileContents) == XPR_FALSE &&
        aOleDataObject->IsDataAvailable((CLIPFORMAT)sClipFormat.mInetUrl)      == XPR_FALSE)
    {
        DragImage &sDragImage = DragImage::instance();

        if (sDragImage.isMyDragging() == XPR_TRUE)
        {
            CImageList *sMyDragImage = sDragImage.getMyDragImage();
            if (XPR_IS_NOT_NULL(sMyDragImage))
            {
                CPoint sDragPoint = aPoint;
                ClientToScreen(&sDragPoint);
                sMyDragImage->DragMove(sDragPoint);
            }
        }
        else
        {
            if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            {
                CPoint sDragPoint = aPoint;
                //ClientToScreen(&sDragPoint);
                mDropTarget.getDropHelper()->DragOver(&sDragPoint, sDropEffect);
            }
        }

        return sDropEffect;
    }

    if (aKeyState & MK_ALT)          sDropEffect = DROPEFFECT_LINK;
    else if (aKeyState & MK_CONTROL) sDropEffect = DROPEFFECT_COPY;
    else if (aKeyState & MK_SHIFT)   sDropEffect = DROPEFFECT_MOVE;
    else
    {
        xpr_bool_t sCopy = XPR_TRUE;
        if (aOleDataObject->IsDataAvailable(sClipFormat.mShellIDList) == XPR_TRUE)
        {
            STGMEDIUM sStgMedium = {0};
            FORMATETC sFormatEtc = {(CLIPFORMAT)sClipFormat.mShellIDList, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

            if (aOleDataObject->GetData(sClipFormat.mShellIDList, &sStgMedium, &sFormatEtc) == XPR_TRUE)
            {
                LPIDA sIda = (LPIDA)::GlobalLock(sStgMedium.hGlobal);
                LPITEMIDLIST sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                GetName(sPidl1, SHGDN_FORPARSING, sPath);

                const xpr_tchar_t *sCurPath = getCurPath();
                if (sPath[0] == sCurPath[0])
                    sCopy = XPR_FALSE;

                ::GlobalUnlock(sStgMedium.hGlobal);

                if (XPR_IS_NOT_NULL(sStgMedium.pUnkForRelease))
                {
                    ::ReleaseStgMedium(&sStgMedium);
                }
            }
        }

        // Background Default Drop Effect
        if (mOption.mDragDefaultFileOp == DRAG_FILE_OP_DEFAULT)
        {
            sDropEffect = sCopy ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
        }
        else if (mOption.mDragDefaultFileOp == DRAG_FILE_OP_COPY)
        {
            sDropEffect = DROPEFFECT_COPY;
        }
        else if (mOption.mDragDefaultFileOp == DRAG_FILE_OP_LINK)
        {
            sDropEffect = DROPEFFECT_LINK;
        }
        else // 1
        {
            sDropEffect = DROPEFFECT_MOVE;
        }
    }

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            CPoint sDragPoint = aPoint;
            ClientToScreen(&sDragPoint);
            sMyDragImage->DragMove(sDragPoint);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint sDragPoint = aPoint;
            //ClientToScreen(&sDragPoint);
            mDropTarget.getDropHelper()->DragOver(&sDragPoint, sDropEffect);
        }
    }

    xpr_uint_t sFlags = 0;
    xpr_sint_t sIndex = HitTest(aPoint, &sFlags);
    if (((sFlags & LVHT_ONITEMICON) != LVHT_ONITEMICON) && ((sFlags & LVHT_ONITEMLABEL) != LVHT_ONITEMLABEL))
        sIndex = -1;

    if (sIndex >= 0)
    {
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        DWORD sItemDataType = getItemDataType(sIndex);
        xpr_uint_t sState = GetItemState(sIndex, LVIS_SELECTED) & LVIS_SELECTED;

        xpr_ulong_t sShellAttributes = 0;
        if (sItemDataType == IDT_PARENT)
        {
            // Parent Item
            LPTVITEMDATA sTvItemData = getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                LPITEMIDLIST sParentFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
                if (fxfile::base::Pidl::removeLastItem(sParentFullPidl) == XPR_TRUE)
                {
                    sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
                    if (GetItemAttributes(sParentFullPidl, sShellAttributes) == XPR_FALSE)
                        sShellAttributes = 0;
                }
                COM_FREE(sParentFullPidl);
            }
        }
        else if (IDT_DRIVE <= sItemDataType && sItemDataType < IDT_DRIVE_SEL)
        {
            // Drive Item
            xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
            _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(sItemDataType));

            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
                if (GetItemAttributes(sFullPidl, sShellAttributes) == XPR_FALSE)
                    sShellAttributes = 0;
            }
            COM_FREE(sFullPidl);
        }
        else
        {
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                sShellAttributes = sLvItemData->mShellAttributes;
            }
        }

        if (XPR_IS_TRUE(mIntraDrag) && sState != 0)
        {
            // When the select folder drag,
            sDropEffect = DROPEFFECT_NONE;
            sIndex = -1;
        }
        else if (XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER) && XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
        {
            if (mDropIndex != sIndex)
            {
                DragImage &sDragImage = DragImage::instance();

                if (sDragImage.isMyDragging() == XPR_TRUE)
                {
                    sDragImage.showMyDragNoLock(XPR_FALSE);
                }
                else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                {
                    mDropTarget.getDropHelper()->Show(XPR_FALSE);
                }

                SetItemState(mDropIndex, 0, LVIS_DROPHILITED);
                SetItemState(sIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
                UpdateWindow();

                if (sDragImage.isMyDragging() == XPR_TRUE)
                {
                    sDragImage.showMyDragNoLock(XPR_TRUE);
                }
                else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                {
                    mDropTarget.getDropHelper()->Show(XPR_TRUE);
                }
            }
        }
        else
        {
            xpr_bool_t    sResult;
            HRESULT       sComResult;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPCITEMIDLIST sPidl        = XPR_NULL;
            LPITEMIDLIST  sFullPidl    = XPR_NULL;

            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_LINK))
                {
                    xpr_tchar_t sLinkPath[XPR_MAX_PATH + 1] = {0};
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sLinkPath);

                    if (ResolveShortcut(GetSafeHwnd(), sLinkPath, &sFullPidl) == XPR_TRUE)
                    {
                        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
                        if (XPR_IS_FALSE(sResult))
                        {
                            sPidl = XPR_NULL;
                            COM_RELEASE(sShellFolder);
                            COM_FREE(sFullPidl);
                        }
                    }
                }

                if (XPR_IS_NULL(sShellFolder) && XPR_IS_NULL(sPidl))
                {
                    sShellFolder = sLvItemData->mShellFolder;
                    sPidl = sLvItemData->mPidl;
                }
            }

            sComResult = E_FAIL;
            LPDROPTARGET sDropTarget = XPR_NULL;

            if (XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
            {
                sComResult = sShellFolder->GetUIObjectOf(
                    m_hWnd,
                    1,
                    (LPCITEMIDLIST *)&sPidl,
                    IID_IDropTarget,
                    XPR_NULL,
                    (LPVOID *)&sDropTarget);
            }

            xpr_bool_t sSucceeded = XPR_FALSE;

            if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sDropTarget))
            {
                CPoint sPoint(aPoint);

                POINTL sPointL = {0};
                ClientToScreen(&sPoint);
                sPointL.x = sPoint.x;
                sPointL.y = sPoint.y;

                DWORD sTempDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

                sComResult = sDropTarget->DragEnter(aOleDataObject->m_lpDataObject, aKeyState, sPointL, &sTempDropEffect);
                if (SUCCEEDED(sComResult))
                {
                    sComResult = sDropTarget->DragOver(aKeyState, sPointL, &sTempDropEffect);
                    sSucceeded = SUCCEEDED(sComResult) && sTempDropEffect != 0;
                }
            }

            COM_RELEASE(sDropTarget);

            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                COM_RELEASE(sShellFolder);
                COM_FREE(sFullPidl);
            }

            if (XPR_IS_TRUE(sSucceeded))
            {
                sDropEffect = DROPEFFECT_MOVE;

                if (mDropIndex != sIndex)
                {
                    DragImage &sDragImage = DragImage::instance();

                    if (sDragImage.isMyDragging() == XPR_TRUE)
                    {
                        sDragImage.showMyDragNoLock(XPR_FALSE);
                    }
                    else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                    {
                        mDropTarget.getDropHelper()->Show(XPR_FALSE);
                    }

                    SetItemState(mDropIndex, 0, LVIS_DROPHILITED);
                    SetItemState(sIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
                    UpdateWindow();

                    if (sDragImage.isMyDragging() == XPR_TRUE)
                    {
                        sDragImage.showMyDragNoLock(XPR_TRUE);
                    }
                    else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                    {
                        mDropTarget.getDropHelper()->Show(XPR_TRUE);
                    }
                }
            }
            else
            {
                // When It cannot drag,
                sIndex = -1;
                if (XPR_IS_TRUE(mIntraDrag) && sDropEffect == DROPEFFECT_MOVE)
                {
                    sDropEffect = DROPEFFECT_NONE;
                }
            }
        }
    }
    else
    {
        // Whe it drag on background
        if (XPR_IS_TRUE(mIntraDrag) && sDropEffect == DROPEFFECT_MOVE)
        {
            sDropEffect = mDropTarget.isRightDrag() ? DROPEFFECT_COPY : DROPEFFECT_NONE;
            sIndex = -1;
        }
    }

    if (sIndex < 0)
    {
        LPTVITEMDATA sTvItemData = getFolderData();
        if ((XPR_IS_NOT_NULL(sTvItemData)) && (sTvItemData->mShellAttributes & SFGAO_FILESYSTEM) != SFGAO_FILESYSTEM)
        {
            sDropEffect = DROPEFFECT_NONE;
        }

        if (mDropIndex != sIndex)
        {
            DragImage &sDragImage = DragImage::instance();

            if (sDragImage.isMyDragging() == XPR_TRUE)
            {
                sDragImage.showMyDragNoLock(XPR_FALSE);
            }
            else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            {
                mDropTarget.getDropHelper()->Show(XPR_FALSE);
            }

            SetItemState(mDropIndex, 0, LVIS_DROPHILITED);
            UpdateWindow();

            if (sDragImage.isMyDragging() == XPR_TRUE)
            {
                sDragImage.showMyDragNoLock(XPR_TRUE);
            }
            else if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            {
                mDropTarget.getDropHelper()->Show(XPR_TRUE);
            }
        }
    }

    mDropIndex = sIndex;

    return sDropEffect;
}

void ExplorerCtrl::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
    CRect sRect;
    GetClientRect(&sRect);

    xpr_sint_t sVertCurPos, sVertMinPos, sVertMaxPos;
    GetScrollRange(SB_VERT, &sVertMinPos, &sVertMaxPos);
    sVertCurPos = GetScrollPos(SB_VERT);

    xpr_sint_t sHorzCurPos, sHorzMinPos, sHorzMaxPos;
    GetScrollRange(SB_HORZ, &sHorzMinPos, &sHorzMaxPos);
    sHorzCurPos = GetScrollPos(SB_HORZ);

    //TRACE(XPR_STRING_LITERAL("Horz = %d, %d, %d, Vert = %d, %d, %d\n"), sHorzMinPos, sHorzMaxPos, sHorzCurPos, sVertMinPos, sVertMaxPos, sVertCurPos);

    if (sVertCurPos != sVertMinPos && sRect.top+40 > aPoint.y)
    {
        if (XPR_IS_FALSE(mScrollUpTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_UP, mOption.mDragScrollTime, XPR_NULL);
            mScrollUpTimer = XPR_TRUE;
        }
    }
    else if (sVertCurPos != sVertMaxPos && sRect.bottom-40 < aPoint.y)
    {
        if (XPR_IS_FALSE(mScrollDownTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_DOWN, mOption.mDragScrollTime, XPR_NULL);
            mScrollDownTimer = XPR_TRUE;
        }
    }

    if ((getViewStyle() != VIEW_STYLE_DETAILS) && (sHorzMinPos != 0 || sHorzMaxPos != 100))
    {
        //TRACE(XPR_STRING_LITERAL("** Horz\n"));
        if (sHorzCurPos != sHorzMinPos && sRect.left+40 > aPoint.x)
        {
            if (XPR_IS_FALSE(mScrollLeftTimer))
            {
                SetTimer(TM_ID_DRAG_SCROLL_LEFT, mOption.mDragScrollTime, XPR_NULL);
                mScrollLeftTimer = XPR_TRUE;
            }
        }
        else if (sHorzCurPos != sHorzMaxPos && sRect.right-40 < aPoint.x)
        {
            if (XPR_IS_FALSE(mScrollRightTimer))
            {
                SetTimer(TM_ID_DRAG_SCROLL_RIGHT, mOption.mDragScrollTime, XPR_NULL);
                mScrollRightTimer = XPR_TRUE;
            }
        }
    }
}

//
// further: CF_HDROP
//
void ExplorerCtrl::OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->EndDrag();
            sMyDragImage->DragShowNolock(XPR_FALSE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    xpr_sint_t sTimerId;
    for (sTimerId = TM_ID_DRAG_SCROLL_BEGIN; sTimerId < TM_ID_DRAG_SCROLL_END; ++sTimerId)
        KillTimer(sTimerId);

    mScrollUpTimer    = XPR_FALSE;
    mScrollDownTimer  = XPR_FALSE;
    mScrollLeftTimer  = XPR_FALSE;
    mScrollRightTimer = XPR_FALSE;

    SetItemState(mDropIndex, 0, LVIS_DROPHILITED);
    RedrawItems(mDropIndex, mDropIndex);
    UpdateWindow();

    // Get Target Directory
    xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1] = {0};
    if (mDropIndex >= 0)
    {
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(mDropIndex);
        DWORD sItemDataType = getItemDataType(mDropIndex);

        if (sItemDataType == IDT_PARENT)
        {
            // Parent Item
            LPTVITEMDATA sTvItemData = getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                LPITEMIDLIST sParentFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
                if (fxfile::base::Pidl::removeLastItem(sParentFullPidl) == XPR_TRUE)
                {
                    GetFolderPath(sParentFullPidl, sTargetDir);
                }
                COM_FREE(sParentFullPidl);
            }
        }
        else if (IDT_DRIVE <= sItemDataType && sItemDataType < IDT_DRIVE_SEL)
        {
            // Drive Item
            xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
            _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(sItemDataType));

            LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                GetName(sFullPidl, SHGDN_FORPARSING, sTargetDir);
            }
            COM_FREE(sFullPidl);
        }
        else
        {
            if (XPR_IS_NOT_NULL(sLvItemData) && (sLvItemData->mShellAttributes & SFGAO_FOLDER) && (sLvItemData->mShellAttributes & SFGAO_FILESYSTEM))
            {
                GetFolderPath(sLvItemData->mShellFolder, sLvItemData->mPidl, sTargetDir, sLvItemData->mShellAttributes);
            }
        }
    }
    else
    {
        _tcscpy(sTargetDir, getCurPath());
    }

    // Drop Implementation
    // 1. Internet Url
    // 2. Outlook File
    // 3. Bitmap Image
    // 4. DIB Image
    // 5. Text
    // 6. General Folder, File
    // 7. Right Drop

    ClipFormat &sClipFormat = ClipFormat::instance();

    xpr_bool_t sFolderFileSystem = IsFileSystemFolder(sTargetDir);
    LPDATAOBJECT sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);

    if (XPR_IS_TRUE(sFolderFileSystem))
    {
        if (mObserver != XPR_NULL)
        {
            xpr_sint_t sResult = mObserver->onDrop(*this, aOleDataObject, sTargetDir);
            if (sResult != 0)
                return;
        }
    }

    xpr_bool_t sExternalCopyFileOp = (aDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(mOption.mExternalCopyFileOp);
    xpr_bool_t sExternalMoveFileOp = (aDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(mOption.mExternalMoveFileOp);

    if (mDropTarget.isRightDrag() == XPR_FALSE && _tcslen(sTargetDir) > 0 && XPR_IS_FALSE(sExternalCopyFileOp) && XPR_IS_FALSE(sExternalMoveFileOp))
    {
        sTargetDir[_tcslen(sTargetDir)+1] = XPR_STRING_LITERAL('\0');
        doPasteSelect(GetSafeHwnd(), aOleDataObject, XPR_TRUE, sClipFormat.mShellIDList, aDropEffect, sTargetDir);
    }
    else
    {
        xpr_bool_t    sResult;
        HRESULT       sComResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;
        LPITEMIDLIST  sFullPidl    = XPR_NULL;

        if (mDropIndex != -1)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(mDropIndex);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_LINK))
                {
                    xpr_tchar_t sLinkPath[XPR_MAX_PATH + 1] = {0};
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sLinkPath);

                    if (ResolveShortcut(GetSafeHwnd(), sLinkPath, &sFullPidl) == XPR_TRUE)
                    {
                        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
                        if (XPR_IS_TRUE(sResult))
                        {
                            GetName(sFullPidl, SHGDN_FORPARSING, sTargetDir);
                        }
                        else
                        {
                            sPidl = XPR_NULL;
                            COM_RELEASE(sShellFolder);
                            COM_FREE(sFullPidl);
                        }
                    }
                }

                if (XPR_IS_NULL(sShellFolder) && XPR_IS_NULL(sPidl))
                {
                    sShellFolder = sLvItemData->mShellFolder;
                    sPidl = sLvItemData->mPidl;
                }
            }
        }

        if (XPR_IS_NULL(sShellFolder) || XPR_IS_NULL(sPidl))
        {
            LPTVITEMDATA sTvItemData = getFolderData();
            sShellFolder = sTvItemData->mShellFolder;
            sPidl = sTvItemData->mPidl;
        }

        POINTL sPointL = {0};
        ClientToScreen(&aPoint);
        sPointL.x = aPoint.x;
        sPointL.y = aPoint.y;
        DWORD sKeyState = mDropTarget.isRightDrag() ? MK_RBUTTON : MK_LBUTTON;
        DWORD sTempDropEffect = DROPEFFECT_MOVE;

        switch (aDropEffect)
        {
        case DROPEFFECT_MOVE: sKeyState |= MK_SHIFT;              break;
        case DROPEFFECT_COPY: sKeyState |= MK_CONTROL;            break;
        case DROPEFFECT_LINK: sKeyState |= MK_SHIFT | MK_CONTROL; break;
        }

        LPDROPTARGET sDropTarget = XPR_NULL;

        sComResult = sShellFolder->GetUIObjectOf(
            m_hWnd,
            1,
            (LPCITEMIDLIST *)&sPidl,
            IID_IDropTarget,
            XPR_NULL,
            (LPVOID *)&sDropTarget);

        if (SUCCEEDED(sComResult) && XPR_IS_NOT_NULL(sDropTarget))
        {
            LPDATAOBJECT sDataObject = aOleDataObject->Detach();
            if (XPR_IS_NOT_NULL(sDataObject))
            {
                sDropTarget->DragEnter(sDataObject, sKeyState, sPointL, &sTempDropEffect);
                sDropTarget->DragOver(sKeyState, sPointL, &sTempDropEffect);

                if (mDropTarget.isRightDrag() == XPR_TRUE)
                {
                    if (XPR_IS_TRUE(mIntraDrag))
                        aDropEffect |= DROPEFFECT_LINK;
                    else
                        aDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

                    // if target path differ from current path
                    if (_tcsicmp(sTargetDir, getCurPath()) != 0)
                        aDropEffect |= DROPEFFECT_MOVE | DROPEFFECT_COPY;
                }

                sDropTarget->Drop(sDataObject, sKeyState, sPointL, &aDropEffect);
            }
        }

        COM_RELEASE(sDropTarget);

        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            COM_RELEASE(sShellFolder);
            COM_FREE(sFullPidl);
        }
    }
}

void ExplorerCtrl::OnTimer(UINT_PTR aIdEvent) 
{
    if (XPR_IS_RANGE(TM_ID_DRAG_SCROLL_BEGIN, aIdEvent, TM_ID_DRAG_SCROLL_END))
    {
        KillTimer(aIdEvent);

        DragImage &sDragImage = DragImage::instance();

        if (sDragImage.isMyDragging() == XPR_TRUE)
        {
            sDragImage.showMyDragNoLock(XPR_FALSE);
        }
        else
        {
            if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                mDropTarget.getDropHelper()->Show(XPR_FALSE);
        }

        xpr_sint_t sSbCode = -1;
        switch (aIdEvent)
        {
        case TM_ID_DRAG_SCROLL_UP:    sSbCode = SB_LINEUP;    break;
        case TM_ID_DRAG_SCROLL_DOWN:  sSbCode = SB_LINEDOWN;  break;
        case TM_ID_DRAG_SCROLL_LEFT:  sSbCode = SB_LINELEFT;  break;
        case TM_ID_DRAG_SCROLL_RIGHT: sSbCode = SB_LINERIGHT; break;
        }

        if (sSbCode != -1)
            ::SendMessage(GetSafeHwnd(), WM_VSCROLL, (WPARAM)sSbCode, (LPARAM)0L);

        if (sDragImage.isMyDragging() == XPR_TRUE)
        {
            sDragImage.showMyDragNoLock(XPR_TRUE);
        }
        else
        {
            if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                mDropTarget.getDropHelper()->Show(XPR_TRUE);
        }

        switch (aIdEvent)
        {
        case TM_ID_DRAG_SCROLL_UP:    mScrollUpTimer    = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_DOWN:  mScrollDownTimer  = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_LEFT:  mScrollLeftTimer  = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_RIGHT: mScrollRightTimer = XPR_FALSE; break;
        }
    }
    //else if (aIdEvent == 4)
    //{
    //    KillTimer(4);
    //    if (mOption.mTotalCmdSelSytle)
    //    {
    //        CPoint sPoint;
    //        GetCursorPos(&sPoint);

    //        CPoint pt2(sPoint);
    //        ScreenToClient(&pt2);
    //        xpr_sint_t sIndex = HitTest(pt2);
    //        if (sIndex >= 0)
    //        {
    //            SetSelectionMark(sIndex);
    //            SetItemState(sIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    //        }

    //        OnContextMenu(this, sPoint);
    //    }
    //}
    else if (aIdEvent == TM_ID_SEL_VIEW)
    {
        KillTimer(aIdEvent);

        xpr_size_t sRealSelCount = getRealSelCount(XPR_TRUE);

        if (mObserver != XPR_NULL)
        {
            mObserver->onUpdateStatus(*this, sRealSelCount);
        }
    }
    else if (aIdEvent == TM_ID_RENAME_VISTA)
    {
        KillTimer(aIdEvent);

        CEdit *sEdit = GetEditControl();
        if (XPR_IS_NOT_NULL(sEdit) && XPR_IS_NOT_NULL(sEdit->m_hWnd))
        {
            xpr_sint_t sEditSel = -1;

            if (XPR_IS_NOT_NULL(mRenameHelper))
                mRenameHelper->getEditSel(XPR_NULL, &sEditSel);

            if (sEditSel >= 0)
            {
                sEdit->SetSel(0, sEditSel);
            }
        }
    }
    else if (aIdEvent == TM_ID_CLICK_RENAME)
    {
        KillTimer(aIdEvent);

        xpr_bool_t sRename = XPR_TRUE;

        if (GetSelectedCount() == 1)
        {
            xpr_sint_t sIndex;

            POSITION sPosition = GetFirstSelectedItemPosition();
            if (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = GetNextSelectedItem(sPosition);

                LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
                if (XPR_IS_NULL(sLvItemData))
                    sRename = XPR_FALSE;
            }
        }

        if (XPR_IS_TRUE(sRename))
        {
            if (mObserver != XPR_NULL)
            {
                mObserver->onRename(*this);
            }
        }

        return;
    }

    super::OnTimer(aIdEvent);
}

xpr_bool_t ExplorerCtrl::doPasteSelect(HWND            aHwnd,
                                       COleDataObject *aOleDataObject,
                                       xpr_bool_t      aDragDrop,
                                       CLIPFORMAT      aShellIdListClipFormat,
                                       DROPEFFECT      aDropEffect,
                                       xpr_tchar_t    *aTarget)
{
    const xpr_tchar_t *sLinkSuffix = gApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

    return DoPaste(aHwnd, aOleDataObject, aDragDrop, aShellIdListClipFormat, aDropEffect, aTarget, sLinkSuffix, WM_PASTE_SELITEM);
}

xpr_bool_t ExplorerCtrl::doPasteSelect(HWND         aHwnd,
                                       CLIPFORMAT   aShellIdListClipFormat,
                                       CLIPFORMAT   aDropEffect,
                                       xpr_tchar_t *aTarget,
                                       xpr_bool_t   aCopy)
{
    const xpr_tchar_t *sLinkSuffix = gApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

    return DoPaste(aHwnd, aShellIdListClipFormat, aDropEffect, aTarget, sLinkSuffix, WM_PASTE_SELITEM, aCopy);
}

xpr_uint_t ExplorerCtrl::getPasteSelItemMessage(void) const
{
    return WM_PASTE_SELITEM;
}

LRESULT ExplorerCtrl::OnPasteSelItem(WPARAM wParam, LPARAM lParam)
{
    PasteSelItems     *sPasteSelItems    = (PasteSelItems *)wParam;
    const xpr_tchar_t *sSource           = sPasteSelItems->mSource;
    const xpr_tchar_t *sTarget           = sPasteSelItems->mTarget;
    HandleToMappings  *sHandleToMappings = sPasteSelItems->mHandleToMappings;

    if (XPR_IS_NULL(sTarget) || _tcsicmp(getCurPath(), sTarget) != 0)
        return 0;

    xpr_sint_t i, sIndex;
    const xpr_size_t sLen = _tcslen(sTarget);
    xpr::string sPath;
    xpr_tchar_t *sSplit;
    const xpr_tchar_t *sParsing = sSource;

    SetRedraw(XPR_FALSE);

    while (*(sParsing) != XPR_STRING_LITERAL('\0'))
    {
        sSplit = (xpr_tchar_t *)_tcsrchr(sParsing, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NOT_NULL(sSplit))
        {
            sPath  = sTarget;
            sPath += (sLen <= 3) ? (sSplit+1) : (sSplit);

            if (XPR_IS_NOT_NULL(sHandleToMappings))
            {
                for (i = 0; i < (xpr_sint_t)sHandleToMappings->mNumberOfMappings; ++i)
                {
                    if (_tcsicmp(sPath.c_str(), sHandleToMappings->mSHNameMapping[i].pszOldPath) == 0)
                    {
                        sPath = sHandleToMappings->mSHNameMapping[i].pszNewPath;
                        break;
                    }
                }
            }

            sIndex = findItemPath(sPath.c_str());
            if (sIndex >= 0)
            {
                if (sParsing == sSource)
                {
                    unselectAll();

                    SetFocus();
                    selectItem(sIndex);
                }
                else
                    SetItemState(sIndex, LVIS_SELECTED, LVIS_SELECTED);
            }
        }

        sParsing += _tcslen(sParsing) + 1;
    }

    // [2007/11/25] bug patched
    // redraw problem when view style is list. In particular, this bug occurs when many items will be insert by refresh.
    SetRedraw();
    Invalidate(XPR_FALSE);
    UpdateWindow();

    return 0;
}

LPITEMIDLIST ExplorerCtrl::getCopyToPidl(void) const
{
    return mCopyFullPidl;
}

LPITEMIDLIST ExplorerCtrl::getMoveToPidl(void) const
{
    return mMoveFullPidl;
}

void ExplorerCtrl::setCopyToPidl(LPITEMIDLIST aPidl)
{
    COM_FREE(mCopyFullPidl);
    mCopyFullPidl = aPidl;
}

void ExplorerCtrl::setMoveToPidl(LPITEMIDLIST aPidl)
{
    COM_FREE(mMoveFullPidl);
    mMoveFullPidl = aPidl;
}

void ExplorerCtrl::OnBeginlabeledit(NMHDR *aNmHdr, LRESULT *aResult) 
{
    if (mOption.mRenameByMouse == XPR_FALSE && XPR_IS_TRUE(mMouseEdit))
    {
        *aResult = 1;
        mMouseEdit = XPR_TRUE;
        return;
    }

    mMouseEdit = XPR_TRUE;

    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO *)aNmHdr;

    XPR_SAFE_DELETE(mRenameHelper);

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sLvDispInfo->item.lParam;
    verifyItemData(&sLvItemData);
    if (XPR_IS_NULL(sLvItemData) || !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_CANRENAME))
    {
        *aResult = 1;
        return;
    }

    xpr_bool_t sCtrlKey = GetAsyncKeyState(VK_CONTROL) < 0;

    RenameHelper::RenameStyle sRenameStyle = RenameHelper::RenameStyleNone;
    if (XPR_IS_FALSE(sCtrlKey))
    {
        switch (mOption.mRenameExtType)
        {
        case RENAME_EXT_TYPE_KEEP:               sRenameStyle = RenameHelper::RenameStyleKeepExt;         break;
        case RENAME_EXT_TYPE_SEL_EXCEPT_FOR_EXT: sRenameStyle = RenameHelper::RenameStyleSelExceptForExt; break;
        }
    }

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::concat(getFolderData()->mFullPidl, sLvItemData->mPidl);

    mRenameHelper = new RenameHelper;
    mRenameHelper->setRenameStyle(sRenameStyle);
    mRenameHelper->setItem(sLvItemData->mShellFolder, sLvItemData->mPidl, sFullPidl, sLvItemData->mShellAttributes);

    COM_FREE(sFullPidl);

    const xpr_tchar_t *sEditName = mRenameHelper->getEditName();

    {
        CEdit *sEdit = GetEditControl();
        if (XPR_IS_NOT_NULL(sEdit))
        {
            //if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            //{
            //    xpr::string sCurPath;
            //    getCurPath(sCurPath);

            //    if (sCurPath[sCurPath.length() - 1] != '\\')
            //        sCurPath += '\\';

            //    sEdit->LimitText(XPR_MAX_PATH - 1 - (xpr_sint_t)sCurPath.length()); // maximum file path length = 259
            //}

            sEdit->SetWindowText(sEditName);
        }

        xpr_sint_t sEditSel = -1;
        mRenameHelper->getEditSel(XPR_NULL, &sEditSel);

        if (sEditSel >= 0)
            SetTimer(TM_ID_RENAME_VISTA, 0, XPR_NULL);
    }

    *aResult = 0;
}

void ExplorerCtrl::OnEndlabeledit(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO*)aNmHdr;
    const xpr_tchar_t *sNewName = sLvDispInfo->item.pszText;

    if (XPR_IS_NULL(sNewName))
    {
        if (XPR_IS_TRUE(mRenameResorting))
            resortItems();

        return;
    }

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sLvDispInfo->item.lParam;
    verifyItemData(&sLvItemData);
    if (XPR_IS_NULL(sLvItemData))
    {
        if (XPR_IS_TRUE(mRenameResorting))
            resortItems();

        return;
    }

    xpr_bool_t sReEdited = XPR_FALSE;

    RenameHelper::Result sResult = mRenameHelper->rename(m_hWnd, sNewName);
    if (sResult != RenameHelper::ResultSucceeded)
    {
        DWORD sEditSel = 0xffffffff;

        CEdit *sEdit = GetEditControl();
        if (XPR_IS_NOT_NULL(sEdit))
            sEditSel = sEdit->GetSel();

        mMouseEdit = XPR_FALSE;
        sEdit = EditLabel(sLvDispInfo->item.iItem);
        if (XPR_IS_NOT_NULL(sEdit))
        {
            KillTimer(TM_ID_RENAME_VISTA);

            sEdit->SetWindowText(sNewName);
            if (sEditSel != 0xffffffff)
                sEdit->SetSel(sEditSel);

            sReEdited = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sReEdited))
    {
        if (XPR_IS_TRUE(mRenameResorting))
            resortItems();
    }

    *aResult = 0;
}

xpr_bool_t ExplorerCtrl::hasSelShellAttributes(xpr_uint_t aShellAttributes) const
{
    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount <= 0)
        return XPR_FALSE;

    xpr_sint_t sIndex;
    xpr_sint_t sCount = 0;
    LPLVITEMDATA sLvItemData = XPR_NULL;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            xpr_uint_t sShellAttributes = sLvItemData->mShellAttributes;
            if (!XPR_ANY_BITS(sShellAttributes, aShellAttributes) && aShellAttributes != 0)
                break;

            sCount++;
        }
    }

    return sCount > 0;
}

void ExplorerCtrl::OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sNmListView->lParam;
    if (XPR_IS_NOT_NULL(sLvItemData))
    {
        eraseNameHash(sLvItemData);

        COM_RELEASE(sLvItemData->mShellFolder);
        COM_RELEASE(sLvItemData->mShellFolder2);
        COM_FREE(sLvItemData->mPidl);
        XPR_SAFE_DELETE(sLvItemData);
    }

    sNmListView->lParam = XPR_NULL;
    *aResult = 0;
}

inline void ExplorerCtrl::selectItem(xpr_sint_t aIndex)
{
    EnsureVisible(aIndex, XPR_FALSE);
    SetSelectionMark(aIndex);
    SetItemState(aIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void ExplorerCtrl::setInsSelPath(const xpr_tchar_t *aInsSelPath, xpr_bool_t bUnsellAll, xpr_bool_t bEdit)
{
    if (XPR_IS_NULL(aInsSelPath))
        return;

    mInsSel       = aInsSelPath;
    mInsUnsellAll = bUnsellAll;
    mInsSelEdit   = bEdit;
}

void ExplorerCtrl::OnInsertitem(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    xpr_sint_t sIndex = sNmListView->iItem;

    // if the view style is icon or small icon or thumbnail,
    // then new item is rearranged before drive item.
    // But it activates when IShellFolder isn't enumerate (shell change notify).
    if (XPR_IS_TRUE(mUpdated))
    {
        if (mOption.mShowDrive == XPR_TRUE || mOption.mShowDriveItem == XPR_TRUE)
        {
            xpr_sint_t sViewStyle = getViewStyle();
            xpr_bool_t sIconsViewStyle = (sViewStyle == VIEW_STYLE_EXTRA_LARGE_ICONS ||
                                          sViewStyle == VIEW_STYLE_LARGE_ICONS       ||
                                          sViewStyle == VIEW_STYLE_MEDIUM_ICONS      ||
                                          sViewStyle == VIEW_STYLE_SMALL_ICONS       ||
                                          sViewStyle == VIEW_STYLE_THUMBNAIL);

            if (XPR_IS_TRUE(sIconsViewStyle))
            {
                CPoint sPoint(0,0);
                if (GetItemPosition(sIndex - 1, &sPoint))
                    SetItemPosition(sIndex, sPoint);
            }
        }
    }

    if (!mInsSel.empty())
    {
        LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            xpr::string sPath;
            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

            if (_tcsicmp(mInsSel.c_str(), sPath.c_str()) == 0)
            {
                if (XPR_IS_TRUE(mInsUnsellAll))
                    unselectAll();

                selectItem(sIndex);

                if (mOption.mCreateAndEditText == XPR_TRUE && IsEqualFileExt(mInsSel.c_str(), XPR_STRING_LITERAL(".txt")) == XPR_TRUE)
                {
                    execute(sIndex);
                }
                else
                {
                    if (XPR_IS_TRUE(mInsSelEdit))
                    {
                        mMouseEdit = XPR_FALSE;
                        EditLabel(sIndex);
                    }
                }

                mInsSel.clear();
            }
        }
    }

    *aResult = 0;
}

void ExplorerCtrl::createFolder(const xpr::string &aNewFolder, xpr_bool_t aEditDirectly)
{
    if (mFolderType != FOLDER_DEFAULT)
        return;

    SetFocus();

    unselectAll();

    xpr_sint_t sCount = GetItemCount();
    if (::CreateDirectory(aNewFolder.c_str(), XPR_NULL) == XPR_FALSE)
        return;

    setInsSelPath(aNewFolder.c_str(), XPR_TRUE, aEditDirectly);

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aNewFolder.c_str());
    ::SHChangeNotify(SHCNE_MKDIR, SHCNF_IDLIST | SHCNF_FLUSH, (LPCVOID)sFullPidl, XPR_NULL);
    COM_FREE(sFullPidl);
}

void ExplorerCtrl::createTextFile(const xpr::string &aNewTextFile, xpr_bool_t aEditDirectly)
{
    if (mFolderType != FOLDER_DEFAULT)
        return;

    SetFocus();

    unselectAll();

    xpr_rcode_t sRcode;
    xpr_sint_t sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aNewTextFile, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    sFileIo.close();

    setInsSelPath(aNewTextFile.c_str(), XPR_TRUE, aEditDirectly);

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aNewTextFile.c_str());
    ::SHChangeNotify(SHCNE_CREATE , SHCNF_IDLIST | SHCNF_FLUSH, (LPCVOID)sFullPidl, XPR_NULL);
    COM_FREE(sFullPidl);
}

void ExplorerCtrl::createGeneralFile(const xpr::string &aNewGeneralFile, xpr_bool_t aEditDirectly)
{
    if (mFolderType != FOLDER_DEFAULT)
        return;

    SetFocus();

    unselectAll();

    xpr_rcode_t sRcode;
    xpr_sint_t sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aNewGeneralFile, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    sFileIo.close();

    setInsSelPath(aNewGeneralFile.c_str(), XPR_TRUE, aEditDirectly);

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aNewGeneralFile.c_str());
    ::SHChangeNotify(SHCNE_CREATE , SHCNF_IDLIST | SHCNF_FLUSH, (LPCVOID)sFullPidl, XPR_NULL);
    COM_FREE(sFullPidl);
}

void ExplorerCtrl::renameDirectly(xpr_sint_t aIndex)
{
    mMouseEdit = XPR_FALSE;
    EditLabel(aIndex);
}

void ExplorerCtrl::selectNextItemWithFocus(void)
{
    xpr_sint_t sIndex = -1;
    xpr_sint_t i, sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (GetItemState(i, LVIS_FOCUSED) == LVIS_FOCUSED)
        {
            sIndex = i;
            break;
        }
    }

    //xpr_sint_t sIndex = GetSelectionMark();
    if (sIndex < 0)
        sIndex = 0;

    if (GetItemState(sIndex, LVIS_SELECTED) != LVIS_SELECTED)
    {
        SetItemState(sIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        EnsureVisible(sIndex, XPR_TRUE);
        return;
    }

    sIndex++;
    if (sIndex < GetItemCount())
    {
        SetItemState(sIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        SetSelectionMark(sIndex);
        EnsureVisible(sIndex, XPR_TRUE);
    }
}

void ExplorerCtrl::selectAll(void)
{
    xpr_sint_t sFocus = GetSelectionMark();

    xpr_sint_t i;
    xpr_sint_t sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (GetItemData(i))
            SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
    }

    SetItemState(sFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void ExplorerCtrl::unselectAll(void)
{
    //xpr_sint_t i;
    //xpr_sint_t sCount = GetItemCount();
    //for (i = 0; i < sCount; ++i)
    //    SetItemState(i, 0, LVIS_SELECTED);

    xpr_sint_t sIndex;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        SetItemState(sIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

xpr_bool_t ExplorerCtrl::isFileSystem(void) const
{
    return hasSelShellAttributes(SFGAO_FILESYSTEM);
}

xpr_bool_t ExplorerCtrl::isFileSystem(xpr_sint_t aIndex) const
{
    if (aIndex < 0)
        return XPR_FALSE;

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(aIndex);
    if (sLvItemData != XPR_NULL && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t ExplorerCtrl::isFileSystemFolder(void) const
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        return XPR_TRUE;

    return XPR_FALSE;
}

xpr_bool_t ExplorerCtrl::isSelected(void) const
{
    xpr_sint_t sSelCount = GetSelectedCount();
    return (sSelCount > 0) ? XPR_TRUE : XPR_FALSE;
}

void ExplorerCtrl::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (mObserver != XPR_NULL)
    {
        mObserver->onSetFocus(*this);
    }

    if (GetSelectionMark() == -1)
        SetItemState(0, LVIS_FOCUSED, LVIS_FOCUSED);

    if (XPR_IS_NOT_NULL(gFrame->mPictureViewer))
    {
        xpr_bool_t sSetPicture = XPR_FALSE;

        if (GetSelectedCount() == 1)
        {
            POSITION sPosition = GetFirstSelectedItemPosition();
            if (XPR_IS_NOT_NULL(sPosition))
            {
                xpr_sint_t sIndex = GetNextSelectedItem(sPosition);
                if (sIndex >= 0)
                {
                    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
                    if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    {
                        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                        gFrame->mPictureViewer->setPicture(sPath);
                        sSetPicture = XPR_TRUE;
                    }
                }
            }
        }

        if (XPR_IS_FALSE(sSetPicture))
        {
            gFrame->mPictureViewer->setPicture(XPR_NULL);
        }

    }
}

void ExplorerCtrl::OnKillFocus(CWnd *aNewWnd) 
{
    super::OnKillFocus(aNewWnd);

    if (mObserver != XPR_NULL)
    {
        mObserver->onKillFocus(*this);
    }
}

void ExplorerCtrl::OnKeyUp(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags) 
{
    super::OnKeyUp(aChar, aRepCnt, aFlags);
}

void ExplorerCtrl::OnMarqueebegin(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;
}

void ExplorerCtrl::updateStatus(void)
{
    SetTimer(TM_ID_SEL_VIEW, 0, XPR_NULL);
}

void ExplorerCtrl::resetStatus(void)
{
    KillTimer(TM_ID_SEL_VIEW);
}

void ExplorerCtrl::OnItemchanged(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    if (sNmListView->uChanged != LVIF_STATE)
        return;
    if (sNmListView->uNewState == LVIS_DROPHILITED)
        return;
    if (sNmListView->uOldState == LVIS_DROPHILITED)
        return;

    if (XPR_TEST_BITS(sNmListView->uOldState, LVIS_SELECTED) ||
        XPR_TEST_BITS(sNmListView->uNewState, LVIS_SELECTED))
    {
        updateStatus();
    }

    TRACE(XPR_STRING_LITERAL("OldState %d, NewState = %d\n"), sNmListView->uOldState, sNmListView->uNewState);

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sNmListView->lParam;
    verifyItemData(&sLvItemData);
    if (XPR_IS_NULL(sLvItemData))
        return;

    // Explorer Window Customizing
    if (XPR_IS_TRUE(mOption.mBkgndImage) || mOption.mBkgndColorType == COLOR_TYPE_CUSTOM || mOption.mTextColorType == COLOR_TYPE_CUSTOM)
        RedrawItems(sNmListView->iItem, sNmListView->iItem);

    // Image & Picture Viewer
    if (XPR_IS_NOT_NULL(gFrame->mPictureViewer))
    {
        if (XPR_TEST_BITS(sNmListView->uNewState, LVIS_FOCUSED) && GetSelectedCount() == 1)
        {
            xpr_sint_t sMark = sNmListView->iItem;
            if (sMark >= 0)
            {
                static xpr_tchar_t sPath[XPR_MAX_PATH + 1];
                sPath[0] = XPR_STRING_LITERAL('\0');

                LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sMark);
                if (XPR_IS_NOT_NULL(sLvItemData) && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                gFrame->mPictureViewer->setPicture(sPath);
            }
        }
        else
        {
            if (gFrame->mPictureViewer->isDocking() == XPR_TRUE)
                gFrame->mPictureViewer->ShowWindow(SW_HIDE);
        }
    }
}

xpr_size_t ExplorerCtrl::getRealSelCount(xpr_bool_t aCalculate, xpr_size_t *aRealSelFolderCount, xpr_size_t *aRealSelFileCount)
{
    if (XPR_IS_TRUE(aCalculate))
    {
        LPTVITEMDATA sTvItemData = getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            mRealSelCount = 0;
            mRealSelFolderCount = 0;
            mRealSelFileCount = 0;

            xpr_sint_t sIndex;
            LPLVITEMDATA sLvItemData;

            POSITION sPosition = GetFirstSelectedItemPosition();
            while (XPR_IS_NOT_NULL(sPosition))
            {
                sIndex = GetNextSelectedItem(sPosition);

                sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
                if (XPR_IS_NULL(sLvItemData))
                    continue;

                mRealSelCount++;

                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
                {
                    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    {
                        if (!XPR_TEST_BITS(sLvItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                        {
                            mRealSelFileCount++;
                            continue;
                        }
                    }

                    mRealSelFolderCount++;
                }
                else
                {
                    mRealSelFileCount++;
                }
            }
        }
    }

    if (XPR_IS_NOT_NULL(aRealSelFolderCount)) *aRealSelFolderCount = mRealSelFolderCount;
    if (XPR_IS_NOT_NULL(aRealSelFileCount))   *aRealSelFileCount   = mRealSelFileCount;

    return mRealSelCount;
}

void ExplorerCtrl::getAllFileSysItems(FileSysItemDeque &aFileSysItemDeque, xpr_bool_t aOnlyFileName, xpr_ulong_t aMask, xpr_ulong_t aUnmask) const
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    xpr_size_t sOffset;
    LPLVITEMDATA sLvItemData;
    FileSysItem *sFileSysItem;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, aMask) && XPR_TEST_NONE_BITS(sLvItemData->mShellAttributes, aUnmask))
        {
            sFileSysItem = new FileSysItem;

            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sFileSysItem->mPath);

            if (XPR_IS_TRUE(aOnlyFileName))
            {
                sOffset = sFileSysItem->mPath.rfind(XPR_STRING_LITERAL('\\'));
                if (sOffset == xpr::string::npos)
                {
                    XPR_SAFE_DELETE(sFileSysItem);
                    continue;
                }

                sFileSysItem->mPath = sFileSysItem->mPath.substr(sOffset + 1);
            }

            sFileSysItem->mFileAttributes = sLvItemData->mFileAttributes;
            aFileSysItemDeque.push_back(sFileSysItem);
        }
    }
}

xpr_bool_t ExplorerCtrl::getSelFileSysItems(FileSysItemDeque &aFileSysItemDeque, xpr_bool_t aOnlyFileName, xpr_ulong_t aMask, xpr_ulong_t aUnmask)
{
    if (getRealSelCount() > 0)
    {
        xpr_sint_t sIndex;
        LPLVITEMDATA sLvItemData;
        xpr_size_t sOffset;
        FileSysItem *sFileSysItem;
        POSITION sPosisition;

        sPosisition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosisition))
        {
            sIndex = GetNextSelectedItem(sPosisition);

            sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
            if (XPR_IS_NULL(sLvItemData))
                continue;

            if (XPR_TEST_BITS(sLvItemData->mShellAttributes, aMask) && XPR_TEST_NONE_BITS(sLvItemData->mShellAttributes, aUnmask))
            {
                sFileSysItem = new FileSysItem;

                GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sFileSysItem->mPath);

                if (XPR_IS_TRUE(aOnlyFileName))
                {
                    sOffset = sFileSysItem->mPath.rfind(XPR_STRING_LITERAL('\\'));
                    if (sOffset == xpr::string::npos)
                    {
                        XPR_SAFE_DELETE(sFileSysItem);
                        continue;
                    }

                    sFileSysItem->mPath = sFileSysItem->mPath.substr(sOffset + 1);
                }

                sFileSysItem->mFileAttributes = sLvItemData->mFileAttributes;
                aFileSysItemDeque.push_back(sFileSysItem);
            }
        }
    }

    return (aFileSysItemDeque.empty() == true) ? XPR_FALSE : XPR_TRUE;
}

xpr_uint64_t ExplorerCtrl::getSelFileSize(void) const
{
    xpr_uint64_t sFileSize = 0;
    xpr_uint64_t sTotalSize = 0;

    xpr_sint_t sIndex;
    LPLVITEMDATA sLvItemData;

    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        if (!XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                getFileSize(sLvItemData, sFileSize);
                sTotalSize += sFileSize;
            }
        }
    }

    return sTotalSize;
}

xpr_uint64_t ExplorerCtrl::getTotalFileSize(xpr_sint_t *aTotalCount, xpr_sint_t *aFileCount, xpr_sint_t *aFolderCount) const
{
    xpr_uint64_t sTotalSize = 0;
    xpr_uint64_t sFileSize = 0;
    xpr_sint_t sTotalCount = 0;
    xpr_sint_t sFileCount = 0;
    xpr_sint_t sFolderCount = 0;

    LPTVITEMDATA sTvItemData = getFolderData();

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPLVITEMDATA sLvItemData;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        sTotalCount++;

        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                if (!XPR_TEST_BITS(sLvItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    sFileCount++;
                    continue;
                }
            }

            sFolderCount++;
        }
        else
        {
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                getFileSize(sLvItemData, sFileSize);
                sTotalSize += sFileSize;
            }

            sFileCount++;
        }
    }

    if (XPR_IS_NOT_NULL(aTotalCount )) *aTotalCount  = sTotalCount;
    if (XPR_IS_NOT_NULL(aFileCount  )) *aFileCount   = sFileCount;
    if (XPR_IS_NOT_NULL(aFolderCount)) *aFolderCount = sFolderCount;

    return sTotalSize;
}

void ExplorerCtrl::updateDriveItem(void)
{
    if (mOption.mShowDrive == XPR_FALSE || mOption.mShowDriveSize == XPR_FALSE)
        return;

    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};

    xpr_sint_t i;
    xpr_uint64_t sSize = 0ui64;
    xpr_size_t sMaxLen;
    xpr_size_t sPrefixLen;
    DWORD sItemDataType = 0;

    for (i = GetItemCount() - 1; i >= 0; --i)
    {
        sItemDataType = getItemDataType(i);
        if ((sItemDataType == IDT_DRIVE_SEL) || (IDT_DRIVE <= sItemDataType && sItemDataType <= IDT_DRIVE_END))
        {
            if (sItemDataType != IDT_DRIVE_SEL)
            {
                _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(sItemDataType));

                if (GetDriveType(sDrive) == DRIVE_FIXED)
                {
                    GetDiskTotalSize(sDrive, sSize);
                    SizeFormat::getSizeUnitFormat(sSize, SIZE_UNIT_AUTO, sText, XPR_MAX_PATH);
                    SetItemText(i, 1, sText);

                    GetDiskFreeSize(sDrive, sSize);

                    sMaxLen = XPR_MAX_PATH;
                    _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.drive.free_size_prefix")));
                    sPrefixLen = _tcslen(sText);

                    SizeFormat::getSizeUnitFormat(sSize, SIZE_UNIT_AUTO, sText + sPrefixLen, sMaxLen - sPrefixLen);
                    _tcscat(sText, gApp.loadString(XPR_STRING_LITERAL("explorer_window.item.drive.free_size_suffix")));
                    SetItemText(i, 2, sText);
                }
            }
        }
        else
        {
            break;
        }
    }
}

xpr_bool_t ExplorerCtrl::PreTranslateMessage(MSG *aMsg) 
{
    return super::PreTranslateMessage(aMsg);
}

void ExplorerCtrl::OnCustomdraw(NMHDR *aNmHdr, LRESULT *aResult)
{
    NMLVCUSTOMDRAW *sNmLvCustomDraw = reinterpret_cast<NMLVCUSTOMDRAW *>(aNmHdr);
    *aResult = 0;

    if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *aResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREERASE)
    {
        *aResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        static xpr_tchar_t sImage[XPR_MAX_PATH + 1];
        sImage[0] = XPR_STRING_LITERAL('\0');

        LVBKIMAGE sLvBkImage = {0};
        sLvBkImage.ulFlags     = LVBKIF_SOURCE_MASK;
        sLvBkImage.pszImage    = sImage;
        sLvBkImage.cchImageMax = XPR_MAX_PATH;
        if (GetBkImage(&sLvBkImage) == XPR_TRUE)
            sNmLvCustomDraw->clrTextBk = sImage[0] == XPR_STRING_LITERAL('\0') ? GetTextBkColor() : CLR_NONE;

        if (mOption.mTextColorType == COLOR_TYPE_FILTERING || mOption.mBkgndColorType == COLOR_TYPE_FILTERING)
        {
            LPLVITEMDATA sLvItemData = ((LPLVITEMDATA)sNmLvCustomDraw->nmcd.lItemlParam);
            verifyItemData(&sLvItemData);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                FileFilterMgr &sFileFilterMgr = FileFilterMgr::instance();

                static xpr_tchar_t sParsing[XPR_MAX_PATH + 1];
                sParsing[0] = XPR_STRING_LITERAL('\0');

                if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                    GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sParsing);

                if (mOption.mTextColorType == COLOR_TYPE_FILTERING)
                    sNmLvCustomDraw->clrText = sFileFilterMgr.getColor(sParsing, sLvItemData->mShellAttributes & SFGAO_FOLDER);

                if (mOption.mBkgndColorType == COLOR_TYPE_FILTERING)
                    sNmLvCustomDraw->clrTextBk = sFileFilterMgr.getColor(sParsing, sLvItemData->mShellAttributes & SFGAO_FOLDER);
            }
        }

        // Only if view style is thumbnail, it draw all of things (image, icon, text and so on...).
        if (getViewStyle() != VIEW_STYLE_THUMBNAIL)
        {
            *aResult = XPR_IS_TRUE(mOption.mParentFolder) ? CDRF_NOTIFYPOSTPAINT : CDRF_DODEFAULT;//CDRF_NOTIFYITEMDRAW;//CDRF_DODEFAULT;
        }
        else
        {
            OnCustomdrawThumbnail(sNmLvCustomDraw);
            *aResult = CDRF_SKIPDEFAULT;
        }
    }
    else if (sNmLvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
    {
        if (mOption.mParentFolder)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sNmLvCustomDraw->nmcd.lItemlParam;
            if (XPR_IS_NOT_NULL(sLvItemData) && sLvItemData->mItemType == IDT_PARENT)
            {
                CRect sIconRect;
                CDC *sDC = CDC::FromHandle(sNmLvCustomDraw->nmcd.hdc);
                xpr_sint_t sItemIndex = static_cast<xpr_sint_t>(sNmLvCustomDraw->nmcd.dwItemSpec);
                GetItemRect(sItemIndex, &sIconRect, LVIR_ICON);

                CPoint sIconPoint;
                xpr_sint_t sViewStyle = getViewStyle();
                xpr_bool_t sLargeIcon = (sViewStyle == VIEW_STYLE_EXTRA_LARGE_ICONS ||
                                         sViewStyle == VIEW_STYLE_LARGE_ICONS       ||
                                         sViewStyle == VIEW_STYLE_MEDIUM_ICONS      ||
                                         sViewStyle == VIEW_STYLE_THUMBNAIL);

                if (XPR_IS_TRUE(sLargeIcon))
                {
                    sIconPoint.x = sIconRect.left + ((sIconRect.Width()  - 32) / 2);
                    sIconPoint.y = sIconRect.top  + ((sIconRect.Height() - 32) / 2);
                }
                else
                {
                    sIconPoint.x = sIconRect.left + ((sIconRect.Width()  - 16) / 2);
                    sIconPoint.y = sIconRect.top  + ((sIconRect.Height() - 16) / 2);
                }

                HICON sIcon = AfxGetApp()->LoadIcon(IDI_GO_UP);
                if (XPR_IS_TRUE(sLargeIcon))
                    DrawIconEx(sDC->m_hDC, sIconPoint.x, sIconPoint.y, sIcon, 32, 32, 0, XPR_NULL, DI_NORMAL);
                else
                    DrawIconEx(sDC->m_hDC, sIconPoint.x, sIconPoint.y, sIcon, 16, 16, 0, XPR_NULL, DI_NORMAL);
                DESTROY_ICON(sIcon);
            }
        }

        *aResult = CDRF_DODEFAULT;
    }
}

void ExplorerCtrl::OnCustomdrawThumbnail(LPNMLVCUSTOMDRAW aNmLvCustomDraw)
{
    LVITEM       sLvItem = {0};
    xpr_sint_t   sItemIndex = static_cast<xpr_sint_t>(aNmLvCustomDraw->nmcd.dwItemSpec);
    CDC         *sDC = CDC::FromHandle(aNmLvCustomDraw->nmcd.hdc);
    COLORREF     sBkgndColor;
    xpr_bool_t   sListHasFocus;
    CRect        sBoundsRect, sIconRect, sLabelRect;
    xpr::string  sLabel = GetItemText(sItemIndex, 0);
    xpr_sint_t   sOffset = sDC->GetTextExtent(XPR_STRING_LITERAL(" ")).cx * 2;

    LPLVITEMDATA sLvItemData = ((LPLVITEMDATA)aNmLvCustomDraw->nmcd.lItemlParam);
    verifyItemData(&sLvItemData);

    TRACE(XPR_STRING_LITERAL("NM_CUSTOMDRAW=%d\n"), sItemIndex);

    sListHasFocus = (GetSafeHwnd() == ::GetFocus());

    sLvItem.mask  = LVIF_IMAGE | LVIF_STATE;
    sLvItem.iItem = sItemIndex;
    sLvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED | LVIS_DROPHILITED | LVIS_OVERLAYMASK | LVIS_CUT;
    GetItem(&sLvItem);

    GetItemRect(sItemIndex, &sBoundsRect, LVIR_BOUNDS);
    GetItemRect(sItemIndex, &sIconRect,   LVIR_ICON);
    GetItemRect(sItemIndex, &sLabelRect,  LVIR_LABEL);

    // Draw the icon & picture.
    xpr_uint_t sFormat = ILD_TRANSPARENT; // IDL_NORMAL
    if (sLvItem.state & INDEXTOOVERLAYMASK(1)) sFormat |= INDEXTOOVERLAYMASK(1);
    if (sLvItem.state & INDEXTOOVERLAYMASK(2)) sFormat |= INDEXTOOVERLAYMASK(2);

    if ((sLvItem.state & LVIS_SELECTED) == LVIS_SELECTED ||
        (sLvItem.state & LVIS_DROPHILITED) == LVIS_DROPHILITED)
    {
        CRect sSelRect(sIconRect);
        sSelRect.bottom += 2;
        sDC->FillSolidRect(sSelRect, ::GetSysColor(COLOR_HIGHLIGHT));
    }

    sIconRect.top += 2;
    sIconRect.left += 2;
    sIconRect.right -= 2;
    sDC->FillSolidRect(sIconRect, RGB(255,255,255));
    sDC->Draw3dRect(sIconRect, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DLIGHT));

    if (XPR_IS_FALSE(mOption.mParentFolder) || XPR_IS_NOT_NULL(sLvItemData))
    {
        xpr_uint_t sThumbImageId = sLvItemData->mThumbImageId;
        if (sThumbImageId == Thumbnail::InvalidThumbImageId)
        {
            CPoint sIconPoint;
            sIconPoint.x = sIconRect.left + ((sIconRect.Width() - 32) / 2);
            sIconPoint.y = sIconRect.top + ((sIconRect.Height() - 32) / 2);
            if (sLvItem.state & LVIS_CUT) // The hidden item give a translucent effect.
            {
                CDC sTempDC;
                CBitmap sBitmap, *sOldBitmap;

                sTempDC.CreateCompatibleDC(sDC);
                sBitmap.CreateCompatibleBitmap(sDC, 32, 32);
                sOldBitmap = sTempDC.SelectObject(&sBitmap);
                sTempDC.FillSolidRect(0,0,32,32,::GetSysColor(COLOR_WINDOW));

                mNormalImgList->DrawIndirect(&sTempDC, sLvItem.iImage, CPoint(0,0), CSize(32,32), CPoint(0,0), ILD_NORMAL, SRCCOPY);
                BLENDFUNCTION sBlendFunc = { AC_SRC_OVER, 0, 128, 0 };
                ::AlphaBlend(sDC->m_hDC, sIconPoint.x, sIconPoint.y, 32, 32, sTempDC.m_hDC, 0, 0, 32, 32, sBlendFunc);

                sTempDC.SelectObject(sOldBitmap);
                sBitmap.DeleteObject();
            }
            else
                mNormalImgList->Draw(sDC, sLvItem.iImage, sIconPoint, sFormat);
        }
        else
        {
            CPoint sIconPoint, sImagePoint;
            CSize sThumbSize;

            sThumbSize = Thumbnail::instance().getThumbSize();

            // Draw Image
            sImagePoint.x = sIconRect.left + ((sIconRect.Width() - sThumbSize.cx) / 2);
            sImagePoint.y = sIconRect.top + ((sIconRect.Height() - sThumbSize.cy) / 2);
            Thumbnail::instance().drawImageId(sDC, sThumbImageId, sImagePoint, sFormat);

            // Draw Icon
            sIconPoint.x = sIconRect.right  - 20;
            sIconPoint.y = sIconRect.bottom - 20;
            mSmallImgList->Draw(sDC, sLvItem.iImage, sIconPoint, sFormat);
        }
    }
    else
    {
        LPLVITEMDATA sLvItemData2 = ((LPLVITEMDATA)aNmLvCustomDraw->nmcd.lItemlParam);
        if (sLvItemData2->mItemType == IDT_PARENT)
        {
            CPoint sIconPoint;
            sIconPoint.x = sIconRect.left + ((sIconRect.Width()  - 32) / 2);
            sIconPoint.y = sIconRect.top  + ((sIconRect.Height() - 32) / 2);

            HICON sIcon = AfxGetApp()->LoadIcon(IDI_GO_UP);
            DrawIconEx(sDC->m_hDC, sIconPoint.x, sIconPoint.y, sIcon, 32, 32, 0, XPR_NULL, DI_NORMAL);
            DESTROY_ICON(sIcon);
        }
        else
        {
            CPoint sIconPoint;
            sIconPoint.x = sIconRect.left + ((sIconRect.Width() - 32) / 2);
            sIconPoint.y = sIconRect.top + ((sIconRect.Height() - 32) / 2);
            if (sLvItem.state & LVIS_CUT) // The hidden item give a blur effect.
            {
                CDC sTempDC;
                CBitmap sBitmap, *sOldBitmap;

                sTempDC.CreateCompatibleDC(sDC);
                sBitmap.CreateCompatibleBitmap(sDC, 32, 32);
                sOldBitmap = sTempDC.SelectObject(&sBitmap);

                mNormalImgList->DrawIndirect(&sTempDC, sLvItem.iImage, CPoint(0,0), CSize(32,32), CPoint(0,0), ILD_NORMAL, SRCCOPY);
                BLENDFUNCTION sBlendFunc = {AC_SRC_OVER, 0, 128, 0};
                AlphaBlend(sDC->m_hDC, sIconPoint.x, sIconPoint.y, 32, 32, sTempDC.m_hDC, 0, 0, 32, 32, sBlendFunc);

                sTempDC.SelectObject(sOldBitmap);
                sBitmap.DeleteObject();
            }
            else
            {
                mNormalImgList->Draw(sDC, sLvItem.iImage, sIconPoint, sFormat);
            }
        }
    }

    sDC->SetBkMode(TRANSPARENT);

    CRect sSelRect = sLabelRect;
    xpr_sint_t sRight = sLabelRect.left + sDC->GetTextExtent(GetItemText(sItemIndex, 0)).cx + sOffset;
    if (sSelRect.right > sRight)
        sSelRect.right = sRight;

    //if (sLvItem.state & LVIS_DROPHILITED)
    //{
    //    sBkgndColor = GetSysColor(COLOR_HIGHLIGHT);
    //    sDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    //    sDC->FillSolidRect(sSelRect, sBkgndColor);
    //}

    if ((sLvItem.state & LVIS_SELECTED) == LVIS_SELECTED ||
        (sLvItem.state & LVIS_DROPHILITED) == LVIS_DROPHILITED)
    {
        if (XPR_IS_TRUE(sListHasFocus))
        {
            sBkgndColor = GetSysColor(COLOR_HIGHLIGHT);
            sDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
            sDC->FillSolidRect(sSelRect, sBkgndColor);
        }
        else // case : Killed Focus
        {
            sBkgndColor = GetSysColor(COLOR_BTNFACE);
            sDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
            sDC->FillSolidRect(sSelRect, sBkgndColor);
        }
    }
    else
    {
        sBkgndColor = GetSysColor(COLOR_WINDOW);

        CBrush sBrush;
        LOGBRUSH sLogBrush = {0};
        sLogBrush.lbStyle = BS_NULL;
        sBrush.CreateBrushIndirect(&sLogBrush);
        sDC->FillRect(sBoundsRect, &sBrush);

        sDC->SetTextColor(aNmLvCustomDraw->clrText);
    }

    // Draw the text.
    CRect sTextRect = sLabelRect;
    sTextRect.left += 2;
    sTextRect.right -= 1;

    if (sLvItem.state & LVIS_FOCUSED)
        sDC->DrawText(sLabel.c_str(), sTextRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL);
    else
        sDC->DrawText(sLabel.c_str(), sTextRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS);

    // Draw the focus
    if (sLvItem.state & LVIS_FOCUSED && XPR_IS_TRUE(sListHasFocus))
    {
        sDC->DrawFocusRect(&sSelRect);
    }
    else if (sLvItem.state & LVIS_FOCUSED && !(sLvItem.state & LVIS_SELECTED))
    {
        sDC->DrawFocusRect(&sSelRect);
    }
}

LRESULT ExplorerCtrl::OnThumbnailProc(WPARAM wParam, LPARAM lParam)
{
    xpr_uint_t sCode = (xpr_uint_t)wParam;
    ThumbImage *sThumbImage = (ThumbImage *)lParam;

    if (sCode == mCode && XPR_IS_NOT_NULL(sThumbImage))
    {
        xpr_sint_t sIndex = findItemPath(sThumbImage->mPath.c_str());
        if (sIndex >= 0)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                xpr_uint_t sThumbImageId = Thumbnail::instance().addImage(sThumbImage);
                if (sThumbImageId >= 0)
                {
                    sLvItemData->mThumbImageId = sThumbImageId;

                    RedrawItems(sIndex, sIndex);
                }
            }
        }
    }

    if (XPR_IS_NOT_NULL(sThumbImage) && XPR_IS_NOT_NULL(sThumbImage->mImage))
    {
        sThumbImage->mImage->DeleteObject();
        XPR_SAFE_DELETE(sThumbImage->mImage);
    }

    XPR_SAFE_DELETE(sThumbImage);

    return 0;
}

//----------------------------------------------------------------------
// Go Up, Back, Forward
//----------------------------------------------------------------------
xpr_bool_t ExplorerCtrl::goUp(void)
{
    LPITEMIDLIST sFullPidl = mTvItemData->mFullPidl;
    if (sFullPidl->mkid.cb == 0) // desktop folder
        return XPR_TRUE;

    mSubFolder.clear();
    if (XPR_IS_TRUE(mOption.mGoUpSelSubFolder))
        GetName(mTvItemData->mShellFolder, mTvItemData->mPidl, SHGDN_INFOLDER, mSubFolder);

    LPITEMIDLIST sParentFullPidl = fxfile::base::Pidl::clone(sFullPidl);
    fxfile::base::Pidl::removeLastItem(sParentFullPidl);

    xpr_bool_t sResult = explore(sParentFullPidl);

    if (mOption.mGoUpSelSubFolder)
    {
        if (mSubFolder.empty() == XPR_FALSE)
        {
            LVFINDINFO sLvFindInfo = {0};
            sLvFindInfo.flags = LVFI_STRING;
            sLvFindInfo.psz   = mSubFolder.c_str();
            xpr_sint_t sFind = FindItem(&sLvFindInfo);
            if (sFind >= 0)
                selectItem(sFind);
        }
    }

    mSubFolder.clear();

    return sResult;
}

xpr_bool_t ExplorerCtrl::goUp(xpr_uint_t aUp)
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return XPR_FALSE;

    LPITEMIDLIST sSeqFullPidl = sFullPidl;
    for (xpr_uint_t i = 0; i < aUp; ++i)
    {
        if (sSeqFullPidl->mkid.cb == 0)
            break;

        sSeqFullPidl = (LPITEMIDLIST)((xpr_byte_t *)sSeqFullPidl + sSeqFullPidl->mkid.cb);
    }

    xpr_sint_t sCbSize = sSeqFullPidl->mkid.cb;
    sSeqFullPidl->mkid.cb = 0;
    LPITEMIDLIST sUpFullPidl = fxfile::base::Pidl::clone(sFullPidl);
    sSeqFullPidl->mkid.cb = sCbSize;

    xpr_bool_t sResult = explore(sUpFullPidl);
    if (XPR_IS_FALSE(sResult))
        COM_FREE(sUpFullPidl);

    COM_FREE(sFullPidl);

    return sResult;
}

void ExplorerCtrl::goBackward(void)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = mHistory->popBackward(mTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

void ExplorerCtrl::goBackward(xpr_size_t aBack)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = mHistory->popBackward(mTvItemData->mFullPidl, aBack);
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

void ExplorerCtrl::goForward(void)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = mHistory->popForward(mTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

void ExplorerCtrl::goForward(xpr_size_t aForward)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = mHistory->popForward(mTvItemData->mFullPidl, aForward);
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

void ExplorerCtrl::goHistory(void)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(mHistory->getHistory());
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

void ExplorerCtrl::goHistory(xpr_size_t aHistory)
{
    if (XPR_IS_NULL(mHistory))
        return;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(mHistory->getHistory(aHistory));
    if (XPR_IS_NULL(sFullPidl))
        return;

    mGo = XPR_FALSE;

    explore(sFullPidl);

    mGo = XPR_TRUE;
}

xpr_uint_t ExplorerCtrl::getUpCount(void)
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return 0;

    if (XPR_IS_NULL(sTvItemData->mFullPidl))
        return 0;

    return (xpr_uint_t)fxfile::base::Pidl::getItemCount(sTvItemData->mFullPidl);
}

xpr_size_t ExplorerCtrl::getBackwardCount(void)
{
    if (XPR_IS_NULL(mHistory))
        return 0;

    return mHistory->getBackwardCount();
}

xpr_size_t ExplorerCtrl::getForwardCount(void)
{
    if (XPR_IS_NULL(mHistory))
        return 0;

    return mHistory->getForwardCount();
}

xpr_size_t ExplorerCtrl::getHistoryCount(void)
{
    if (XPR_IS_NULL(mHistory))
        return 0;

    return mHistory->getHistoryCount();
}

void ExplorerCtrl::clearBackward(void)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->clearBackward();
}

void ExplorerCtrl::clearForward(void)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->clearForward();
}

void ExplorerCtrl::clearHistory(void)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->clearHistory();
}

void ExplorerCtrl::clearAllHistory(void)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->clear();
}

void ExplorerCtrl::setMaxBackward(xpr_size_t aBackwardCount)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->setMaxBackward(aBackwardCount);
}

void ExplorerCtrl::setMaxHistory(xpr_size_t aHistoryCount)
{
    if (XPR_IS_NOT_NULL(mHistory))
        mHistory->setMaxHistory(aHistoryCount);
}

const HistoryDeque *ExplorerCtrl::getBackwardDeque(void) const
{
    XPR_ASSERT(mHistory);

    return mHistory->getBackwardDeque();
}

const HistoryDeque *ExplorerCtrl::getForwardDeque(void) const
{
    XPR_ASSERT(mHistory);

    return mHistory->getForwardDeque();
}

const HistoryDeque *ExplorerCtrl::getHistoryDeque(void) const
{
    XPR_ASSERT(mHistory);

    return mHistory->getHistoryDeque();
}

xpr_sint_t ExplorerCtrl::findItemName(const xpr_tchar_t *aName, xpr_sint_t aStart) const
{
    XPR_ASSERT(aName != XPR_NULL);

    xpr_sint_t sIndex = -1;
    xpr_tchar_t sName[XPR_MAX_PATH + 1];

    LVFINDINFO sLvFindInfo = {0};
    sLvFindInfo.flags = LVFI_STRING;
    sLvFindInfo.psz   = aName;

    do
    {
        sIndex = FindItem(&sLvFindInfo, aStart);
        if (sIndex >= 0)
        {
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sLvItemData))
            {
                getItemName(sLvItemData, sName, XPR_MAX_PATH);
                if (_tcsicmp(aName, sName) == 0)
                    break;
            }
            aStart = sIndex;
        }
    } while (sIndex != -1);

    return sIndex;
}

xpr_sint_t ExplorerCtrl::findItemFileName(const xpr_tchar_t *aName, const xpr_tchar_t *aFileName) const
{
    XPR_ASSERT(aName != XPR_NULL);
    XPR_ASSERT(aFileName != XPR_NULL);

    xpr_sint_t  sIndex = -1;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
    NameMap::const_iterator sIterator;
    NameMapPairConstIterator sPairRangeIterator;

    sPairRangeIterator = mNameMap.equal_range(aName);

    sIterator = sPairRangeIterator.first;
    for (; sIterator != sPairRangeIterator.second; ++sIterator)
    {
        LPLVITEMDATA sLvItemData = sIterator->second;

        XPR_ASSERT(sLvItemData != XPR_NULL);

        getItemName(sLvItemData, sFileName, EXT_TYPE_ALWAYS);

        if (_tcsicmp(aFileName, sFileName) == 0)
        {
            LVFINDINFO sLvFindInfo = {0};
            sLvFindInfo.flags  = LVFI_PARAM;
            sLvFindInfo.lParam = (LPARAM)sLvItemData;

            sIndex = FindItem(&sLvFindInfo);
            if (sIndex >= 0)
                break;
        }
    }

    return sIndex;
}

xpr_sint_t ExplorerCtrl::findItemFileName(const xpr_tchar_t *aFileName) const
{
    XPR_ASSERT(aFileName != XPR_NULL);

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    getItemName(aFileName, sName, XPR_MAX_PATH);

    return findItemFileName(sName, aFileName);
}

xpr_sint_t ExplorerCtrl::findItemPath(const xpr_tchar_t *aPath) const
{
    XPR_ASSERT(aPath != XPR_NULL);

    xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};

    SplitPath(aPath, XPR_NULL, sFileName);
    getItemName(sFileName, sName, XPR_MAX_PATH);

    return findItemPath(sName, aPath);
}

xpr_sint_t ExplorerCtrl::findItemPath(LPITEMIDLIST aFullPidl) const
{
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    getItemName(aFullPidl, sName, XPR_MAX_PATH);
    GetName(aFullPidl, SHGDN_FORPARSING, sPath);

    return findItemPath(sName, sPath);
}

xpr_sint_t ExplorerCtrl::findItemPath(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl) const
{
    XPR_ASSERT(aShellFolder != XPR_NULL);

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    getItemName(aShellFolder, aPidl, sName, XPR_MAX_PATH);
    GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);

    return findItemPath(sName, sPath);
}

xpr_sint_t ExplorerCtrl::findItemPath(const xpr_tchar_t *aName, const xpr_tchar_t *aPath) const
{
    XPR_ASSERT(aName != XPR_NULL);
    XPR_ASSERT(aPath != XPR_NULL);

    xpr_sint_t  sIndex = -1;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    NameMap::const_iterator sIterator;
    NameMapPairConstIterator sPairRangeIterator;

    sPairRangeIterator = mNameMap.equal_range(aName);

    sIterator = sPairRangeIterator.first;
    for (; sIterator != sPairRangeIterator.second; ++sIterator)
    {
        LPLVITEMDATA sLvItemData = sIterator->second;

        XPR_ASSERT(sLvItemData != XPR_NULL);

        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

        if (_tcsicmp(aPath, sPath) == 0)
        {
            LVFINDINFO sLvFindInfo = {0};
            sLvFindInfo.flags  = LVFI_PARAM;
            sLvFindInfo.lParam = (LPARAM)sLvItemData;

            sIndex = FindItem(&sLvFindInfo);
            if (sIndex >= 0)
                break;
        }
    }

    return sIndex;
}

xpr_sint_t ExplorerCtrl::findItemFolder(const xpr_tchar_t *aFolderName) const
{
    XPR_ASSERT(aFolderName != XPR_NULL);

    xpr_sint_t  sIndex = -1;
    xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
    NameMap::const_iterator sIterator;
    NameMapPairConstIterator sPairRangeIterator;

    sPairRangeIterator = mNameMap.equal_range(aFolderName);

    sIterator = sPairRangeIterator.first;
    for (; sIterator != sPairRangeIterator.second; ++sIterator)
    {
        LPLVITEMDATA sLvItemData = sIterator->second;

        XPR_ASSERT(sLvItemData != XPR_NULL);

        if (XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FOLDER))
        {
            GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_INFOLDER | SHGDN_FORPARSING, sFileName);

            if (_tcsicmp(aFolderName, sFileName) == 0)
            {
                LVFINDINFO sLvFindInfo = {0};
                sLvFindInfo.flags  = LVFI_PARAM;
                sLvFindInfo.lParam = (LPARAM)sLvItemData;

                sIndex = FindItem(&sLvFindInfo);
                if (sIndex >= 0)
                    break;
            }
        }
    }

    return sIndex;
}

xpr_sint_t ExplorerCtrl::findItemSignature(xpr_uint_t aSignature) const
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    LPLVITEMDATA sLvItemData;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        if (sLvItemData->mSignature == aSignature)
            return i;
    }

    return -1;
}

void ExplorerCtrl::insertNameHash(LPLVITEMDATA aLvItemData)
{
    static xpr_tchar_t sName[XPR_MAX_PATH + 1];

    sName[0] = 0;
    getItemName(aLvItemData, sName, XPR_MAX_PATH);

    aLvItemData->mName = sName;

    mNameMap.insert(NameMap::value_type(aLvItemData->mName, aLvItemData));
}

void ExplorerCtrl::eraseNameHash(LPLVITEMDATA aLvItemData)
{
    NameMap::const_iterator sIterator;
    NameMapPairConstIterator sPairRangeIterator;

    sPairRangeIterator = mNameMap.equal_range(aLvItemData->mName);

    sIterator = sPairRangeIterator.first;
    for (; sIterator != sPairRangeIterator.second; ++sIterator)
    {
        LPLVITEMDATA sLvItemData = sIterator->second;
        if (sLvItemData == aLvItemData)
        {
            mNameMap.erase(sIterator);
            break;
        }
    }
}

void ExplorerCtrl::clearNameHash(void)
{
    mNameMap.clear();
}

void ExplorerCtrl::enableChangeNotify(xpr_bool_t bNotify)
{
    mNotify = bNotify;
}

LRESULT ExplorerCtrl::OnShellChangeNotify(WPARAM wParam, LPARAM lParam)
{
    //if (!mNotify)
    //    return 0;

    Shcn *sShcn = (Shcn *)wParam;
    DWORD sEventId = (DWORD)lParam;

    if (XPR_IS_NULL(sShcn))
        return 0;

#ifdef XPR_CFG_BUILD_DEBUG
    xpr::string sMsg(XPR_STRING_LITERAL(""));
#endif

    if (beginShcn(sEventId))
    {
        xpr_bool_t sResult = XPR_FALSE;
        switch (sEventId)
        {
        case SHCNE_CREATE:           sResult = OnShcnCreateItem(sShcn);    break;
        case SHCNE_MKDIR:            sResult = OnShcnCreateItem(sShcn);    break;
        case SHCNE_RENAMEITEM:       sResult = OnShcnRenameItem(sShcn);    break;
        case SHCNE_RENAMEFOLDER:     sResult = OnShcnRenameItem(sShcn);    break;
        case SHCNE_DELETE:           sResult = OnShcnDeleteItem(sShcn);    break;
        case SHCNE_RMDIR:            sResult = OnShcnDeleteItem(sShcn);    break;
        case SHCNE_UPDATEDIR:        sResult = OnShcnUpdateDir(sShcn);     break;
        case SHCNE_UPDATEITEM:       sResult = OnShcnUpdateItem(sShcn);    break;
        case SHCNE_FREESPACE:        sResult = OnShcnFreeSize(sShcn);      break;
        case SHCNE_NETSHARE:
        case SHCNE_NETUNSHARE:       sResult = OnShcnNetShare(sShcn);      break;
        case SHCNE_DRIVEADD:         sResult = OnShcnDriveAdd(sShcn);      break;
        case SHCNE_DRIVEREMOVED:     sResult = OnShcnDriveRemove(sShcn);   break;
        case SHCNE_MEDIAINSERTED:    sResult = OnShcnMediaInserted(sShcn); break;
        case SHCNE_MEDIAREMOVED:     sResult = OnShcnMediaRemoved(sShcn);  break;

#ifdef XPR_CFG_BUILD_DEBUG
        case SHCNE_ALLEVENTS:        sMsg = XPR_STRING_LITERAL("SHCNE_ALLEVENTS");         break;
        case SHCNE_ASSOCCHANGED:     sMsg = XPR_STRING_LITERAL("SHCNE_ASSOCCHANGED");      break;
        case SHCNE_ATTRIBUTES:       sMsg = XPR_STRING_LITERAL("SHCNE_ATTRIBUTES");        break;
        case SHCNE_DRIVEADDGUI:      sMsg = XPR_STRING_LITERAL("SHCNE_DRIVEADDGUI");       break;
        case SHCNE_EXTENDED_EVENT:   sMsg = XPR_STRING_LITERAL("SHCNE_EXTENDED_EVENT");    break;
        case SHCNE_SERVERDISCONNECT: sMsg = XPR_STRING_LITERAL("SHCNE_SERVERDISCONNECT");  break;
        case SHCNE_UPDATEIMAGE:      sMsg = XPR_STRING_LITERAL("SHCNE_UPDATEIMAGE");       break;
#endif
        default:
            break;
        }

        endShcn(sEventId, sResult);
    }

    sShcn->Free();
    XPR_SAFE_DELETE(sShcn);

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::beginShcn(DWORD aEventId)
{
    xpr_bool_t sResult = XPR_TRUE;
    switch (aEventId)
    {
    case SHCNE_CREATE:       sResult = mOption.mListType != LIST_TYPE_FOLDER; break;
    case SHCNE_MKDIR:        sResult = mOption.mListType != LIST_TYPE_FILE;   break;
    case SHCNE_RENAMEITEM:   sResult = mOption.mListType != LIST_TYPE_FOLDER; break;
    case SHCNE_RENAMEFOLDER: sResult = mOption.mListType != LIST_TYPE_FILE;   break;
    case SHCNE_DELETE:       sResult = mOption.mListType != LIST_TYPE_FOLDER; break;
    case SHCNE_RMDIR:        sResult = mOption.mListType != LIST_TYPE_FILE;   break;
    }

    return sResult;
}

void ExplorerCtrl::endShcn(DWORD aEventId, xpr_bool_t aResult)
{
    if (mOption.mRefreshSort == XPR_TRUE && XPR_IS_TRUE(aResult))
    {
        // If refresh and auto-arrange option enable and is renaming any item, auto-arrange function delay.
        CEdit *sEdit = GetEditControl();
        mRenameResorting = (XPR_IS_NOT_NULL(sEdit) && XPR_IS_NOT_NULL(sEdit->m_hWnd));

        if (XPR_IS_FALSE(mRenameResorting))
            resortItems();
    }
}

LRESULT ExplorerCtrl::OnFileChangeNotify(WPARAM wParam, LPARAM lParam)
{
    if (mOption.mNoRefresh == XPR_TRUE)
        return 0;

    FileChangeWatcher::WatchId sWatchId = (FileChangeWatcher::WatchId)wParam;
    if (mWatchId != sWatchId)
        return 0;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(mTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return 0;

    Shcn sShcn = {0};
    sShcn.mEventId = SHCNE_UPDATEDIR;
    sShcn.mPidl1   = sFullPidl;
    sShcn.mItem1   = (uintptr_t)sFullPidl;
    sShcn.mHwnd    = m_hWnd;

    OnShcnUpdateDir(&sShcn);

    COM_FREE(sFullPidl);

    return 0;
}

LRESULT ExplorerCtrl::OnAdvFileChangeNotify(WPARAM wParam, LPARAM lParam)
{
    if (mOption.mNoRefresh == XPR_TRUE)
        return 0;

    AdvFileChangeWatcher::NotifyInfo *sNotifyInfo = reinterpret_cast<AdvFileChangeWatcher::NotifyInfo *>(wParam);
    if (XPR_IS_NULL(sNotifyInfo))
        return 0;

    if (sNotifyInfo->mAdvWatchId != mAdvWatchId)
    {
        XPR_SAFE_DELETE(sNotifyInfo);
        return 0;
    }

    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
    {
        XPR_SAFE_DELETE(sNotifyInfo);
        return 0;
    }

    LPITEMIDLIST sFullPidl = XPR_NULL;

    xpr::string sPath;
    CombinePath(sPath, sNotifyInfo->mDir, sNotifyInfo->mFileName);

    switch (sNotifyInfo->mEvent)
    {
    case AdvFileChangeWatcher::EventCreated:
        {
            if (sNotifyInfo->mFileName.empty() == false)
            {
                LPITEMIDLIST sRealFullPidl = fxfile::base::Pidl::create(sPath.c_str());
                if (XPR_IS_NOT_NULL(sRealFullPidl))
                {
                    sFullPidl = fxfile::base::Pidl::concat(sTvItemData->mFullPidl, fxfile::base::Pidl::findLastItem(sRealFullPidl));
                    COM_FREE(sRealFullPidl);
                }
            }

            if (XPR_IS_NULL(sFullPidl))
                break;

            Shcn sShcn = {0};
            sShcn.mEventId = SHCNE_CREATE;
            sShcn.mPidl1   = sFullPidl;
            sShcn.mItem1   = reinterpret_cast<uintptr_t>(sFullPidl);
            sShcn.mHwnd    = m_hWnd;

            OnShcnCreateItem(&sShcn);
            break;
        }

    case AdvFileChangeWatcher::EventDeleted:
        {
            OnShcnDeleteItem(XPR_NULL, sPath.c_str());
            break;
        }

    case AdvFileChangeWatcher::EventRenamed:
        {
            if (sNotifyInfo->mFileName.empty() == false)
            {
                LPITEMIDLIST sRealFullPidl = fxfile::base::Pidl::create(sPath.c_str());
                if (XPR_IS_NOT_NULL(sRealFullPidl))
                {
                    sFullPidl = fxfile::base::Pidl::concat(sTvItemData->mFullPidl, fxfile::base::Pidl::findLastItem(sRealFullPidl));
                    COM_FREE(sRealFullPidl);
                }
            }

            xpr::string sOldPath;
            CombinePath(sOldPath, sNotifyInfo->mDir, sNotifyInfo->mOldFileName);

            if (XPR_IS_NULL(sFullPidl))
                OnShcnDeleteItem(XPR_NULL, sOldPath.c_str());
            else
                OnShcnRenameItem(sOldPath.c_str(), sFullPidl);
            break;
        }

    case AdvFileChangeWatcher::EventModified:
    default:
        {
            sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);

            Shcn sShcn = {0};
            sShcn.mEventId = SHCNE_UPDATEDIR;
            sShcn.mPidl1   = sFullPidl;
            sShcn.mItem1   = reinterpret_cast<uintptr_t>(sFullPidl);
            sShcn.mHwnd    = m_hWnd;

            OnShcnUpdateDir(&sShcn);

            COM_FREE(sFullPidl);

            break;
        }
    }

    COM_FREE(sFullPidl);

    XPR_SAFE_DELETE(sNotifyInfo);

    return 0;
}

void ExplorerCtrl::enumerateShcn(ShNotifyInfo *aShNotifyInfo, xpr_bool_t aHiddenAdd)
{
    LPTVITEMDATA  sTvItemData  = getFolderData();
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    XPR_ASSERT(sTvItemData != XPR_NULL);

    if (base::Pidl::getShellFolder(sTvItemData->mShellFolder, sTvItemData->mPidl, sShellFolder) == XPR_FALSE)
    {
        return;
    }

    LPITEMIDLIST sEnumPidl       = XPR_NULL;
    xpr_sint_t   sEnumListType   = 0;
    xpr_sint_t   sEnumAttributes = 0;

    switch (mOption.mListType)
    {
    case LIST_TYPE_ALL:    sEnumListType = base::ShellEnumerator::ListTypeAll;        break;
    case LIST_TYPE_FILE:   sEnumListType = base::ShellEnumerator::ListTypeOnlyFile;   break;
    case LIST_TYPE_FOLDER: sEnumListType = base::ShellEnumerator::ListTypeOnlyFolder; break;
    }

    if (XPR_IS_TRUE(mOption.mShowHiddenAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeHidden;
    if (XPR_IS_TRUE(mOption.mShowSystemAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeSystem;

    base::ShellEnumerator sShellEnumerator;

    if (sShellEnumerator.enumerate(m_hWnd, sShellFolder, sEnumListType, sEnumAttributes) == XPR_TRUE)
    {
        preShcnEnumeration();

        while (sShellEnumerator.next(&sEnumPidl) == XPR_TRUE)
        {
            if (updateShcnPidlItem(sShellFolder, sEnumPidl, aShNotifyInfo) == XPR_FALSE)
            {
                break;
            }
        }

        postShcnEnumeration();
    }
    else
    {
        // failed to enumerate
    }

    COM_RELEASE(sShellFolder);
}

void ExplorerCtrl::preShcnEnumeration(void)
{
    SetRedraw(XPR_FALSE);
}

xpr_bool_t ExplorerCtrl::updateShcnPidlItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, ShNotifyInfo *aShNotifyInfo)
{
    xpr_bool_t   sResult          = XPR_FALSE;
    xpr_ulong_t  sShellAttributes = 0;
    DWORD        sFileAttributes  = 0;
    LPLVITEMDATA sLvItemData;

    // check list type
    if (mOption.mListType == LIST_TYPE_FOLDER && IsFileSystemFolder(aShellFolder, aPidl) == XPR_FALSE)
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    if (mOption.mListType == LIST_TYPE_FILE && IsFileSystemFolder(aShellFolder, aPidl) == XPR_TRUE)
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    // get shell item attributes
    getItemAttributes(aShellFolder, aPidl, sShellAttributes, sFileAttributes);

    // check hidden attribute
    if (mOption.mShowSystemAttribute == XPR_FALSE && mOption.mShowHiddenAttribute == XPR_FALSE && XPR_TEST_BITS(sShellAttributes, SFGAO_GHOSTED))
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    // check system attribute
    if (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM))
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    // allocate new LVITEMDATA
    sLvItemData                   = new LVITEMDATA;
    sLvItemData->mSignature       = mSignature++;
    sLvItemData->mItemType        = IDT_SHELL;
    sLvItemData->mShellFolder     = aShellFolder;
    sLvItemData->mShellFolder2    = XPR_NULL;
    sLvItemData->mPidl            = aPidl;
    sLvItemData->mShellAttributes = sShellAttributes;
    sLvItemData->mFileAttributes  = sFileAttributes;
    sLvItemData->mThumbImageId    = Thumbnail::InvalidThumbImageId;
    aShellFolder->AddRef();

    if (aShNotifyInfo->mEventId == SHCNE_NETSHARE || aShNotifyInfo->mEventId == SHCNE_NETUNSHARE)
    {
        static xpr::string sName;
        GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_INFOLDER, sName);

        if (_tcsicmp(sName.c_str(), aShNotifyInfo->mNewName) == 0)
        {
            if (OnShcnEnumNetShare(sLvItemData, aShNotifyInfo))
            {
                // stop to enumerate
                return XPR_FALSE;
            }
        }
    }
    else
    {
        sResult = updateShcnLvItemData(sLvItemData);
    }

    if (XPR_IS_FALSE(sResult))
    {
        COM_RELEASE(sLvItemData->mShellFolder);
        COM_RELEASE(sLvItemData->mShellFolder2);
        COM_FREE(sLvItemData->mPidl);
        XPR_SAFE_DELETE(sLvItemData);
    }

    return XPR_TRUE;
}

void ExplorerCtrl::postShcnEnumeration(void)
{
    // [2010/06/15] bug patched
    // redraw problem when view style is list. In particular, this bug occurs when many items will be insert by refresh.
    SetRedraw();

    Invalidate(XPR_FALSE);
    UpdateWindow();
}

xpr_sint_t ExplorerCtrl::getLastInsertIndex(void) const
{
    xpr_sint_t i;
    for (i = GetItemCount() - 1; i >= 0; --i)
    {
        if (getItemDataType(i) == IDT_SHELL)
            break;
    }

    if (mOption.mParentFolder == XPR_TRUE)
    {
        if (i == -1)
            i++;
    }

    return ++i;
}

xpr_bool_t ExplorerCtrl::OnShcnCreateItem(Shcn *aShcn)
{
    if (XPR_IS_NULL(aShcn))
        return XPR_FALSE;

    {
        HRESULT sHResult;
        LPITEMIDLIST sShcnPidl;

        sShcnPidl = fxfile::base::Pidl::findLastItem(aShcn->mPidl1);
        if (XPR_IS_NOT_NULL(sShcnPidl))
        {
            LPTVITEMDATA sTvItemData = getFolderData();
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                sHResult = sTvItemData->mShellFolder->CompareIDs(SHCIDS_ALLFIELDS, sTvItemData->mPidl, sShcnPidl);

                xpr_bool_t sEqualedFolder = ((xpr_sshort_t)SCODE_CODE(GetScode(sHResult)) == 0) ? XPR_TRUE : XPR_FALSE;
                if (XPR_IS_TRUE(sEqualedFolder))
                    return XPR_FALSE;
            }
        }
    }

    HRESULT sHResult;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath);
    if (FAILED(sHResult))
        return XPR_FALSE;

    if (findItemPath(sPath) != -1)
        return XPR_FALSE;

    LPITEMIDLIST sFullPidl = XPR_NULL;
    sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t    sResult      = XPR_FALSE;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;
    if (fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl) == XPR_TRUE)
    {
        sPidl = fxfile::base::Pidl::clone(sPidl);

        xpr_sint_t sLastInsertIndex = getLastInsertIndex();
        sResult = insertPidlItem(sShellFolder, (LPITEMIDLIST)sPidl, sLastInsertIndex);
        Arrange(LVA_DEFAULT);
    }

    COM_FREE(sFullPidl);

    return sResult;
}

// rename : szParsing1 -> szParsing2
xpr_bool_t ExplorerCtrl::OnShcnRenameItem(Shcn *aShcn)
{
    xpr_bool_t sRename = XPR_FALSE;

    HRESULT sHResult;
    xpr_tchar_t sParsing1[XPR_MAX_PATH + 1];
    xpr_tchar_t sParsing2[XPR_MAX_PATH + 1];
    sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sParsing1);
    if (FAILED(sHResult))
        return XPR_FALSE;

    sHResult = GetName(aShcn->mPidl2, SHGDN_FORPARSING, sParsing2);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_sint_t sSplitLen1 = 0, sSplitLen2 = 0;
    xpr_tchar_t *sSplit1 = _tcsrchr(sParsing1, XPR_STRING_LITERAL('\\'));
    xpr_tchar_t *sSplit2 = _tcsrchr(sParsing2, XPR_STRING_LITERAL('\\'));
    if (XPR_IS_NOT_NULL(sSplit1) && XPR_IS_NOT_NULL(sSplit2))
    {
        sSplitLen1 = (xpr_sint_t)(sSplit1 - sParsing1);
        sSplitLen2 = (xpr_sint_t)(sSplit2 - sParsing2);
    }

    // test - rename
    // 1. "c:\\test.txt" -> "c:\\test\\test.txt"
    // 2. "c:\\test\\text.txt" -> "c:\\test.txt"
    if (sSplitLen1 != sSplitLen2 || _tcsnicmp(sParsing1, sParsing2, sSplitLen1) != 0)
    {
        // delete "c:\\test.txt"
        OnShcnDeleteItem(XPR_NULL, sParsing1);

        // create "c:\\test\\test.txt"
        xpr_tchar_t sCurDir[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sCurDir, getCurPath());

        xpr_tchar_t *sSplit = _tcsrchr(sCurDir, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NOT_NULL(sSplit))
            *sSplit = XPR_STRING_LITERAL('\0');

        if (_tcsnicmp(sParsing2, sCurDir, (sSplitLen1 > sSplitLen2) ? sSplitLen1 : sSplitLen2) == 0)
        {
            Shcn sShcn = {0};
            sShcn.mPidl1 = aShcn->mPidl2;
            OnShcnCreateItem(&sShcn);
        }
    }
    else
    {
        // rename - "flychk (C:)" -> "flychk2 (C:)"
        if (_tcslen(sParsing1) == 3)
            OnShcnUpdateName();
        else
            sRename = XPR_TRUE;
    }

    if (XPR_IS_FALSE(sRename))
        return XPR_TRUE;

    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    LPITEMIDLIST sFullPidl;
    LPITEMIDLIST sParsingFullPidl;

    sParsingFullPidl = fxfile::base::Pidl::create(sParsing2);
    if (XPR_IS_NOT_NULL(sParsingFullPidl))
    {
        sFullPidl = fxfile::base::Pidl::concat(sTvItemData->mFullPidl, fxfile::base::Pidl::findLastItem(sParsingFullPidl));
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            sResult = OnShcnRenameItem(sParsing1, sFullPidl);
            COM_FREE(sFullPidl);
        }

        COM_FREE(sParsingFullPidl);
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnRenameItem(const xpr_tchar_t *aOldPath, LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aOldPath) || XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
    GetName(aFullPidl, SHGDN_FORPARSING, sNewPath);

    xpr_sint_t sOldIndex = findItemPath(aOldPath);
    xpr_sint_t sNewIndex = findItemPath(sNewPath);
    if (sOldIndex >= 0 || sNewIndex >= 0)
    {
        if (sOldIndex < 0)
            sOldIndex = sNewIndex;

        SetRedraw(XPR_FALSE);

        xpr_uint_t sState = GetItemState(sOldIndex, LVIS_SELECTED | LVIS_FOCUSED);
        DeleteItem(sOldIndex);

        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl         = XPR_NULL;
        if (fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl) == XPR_TRUE)
        {
            if (insertPidlItem(sShellFolder, fxfile::base::Pidl::clone(sPidl), sOldIndex) == XPR_TRUE)
            {
                if (sState != 0)
                {
                    SetItemState(sOldIndex, sState, LVIS_SELECTED | LVIS_FOCUSED);
                    SetSelectionMark(sOldIndex);
                }

                sResult = XPR_TRUE;
            }
        }

        SetRedraw();
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnUpdateDir(Shcn *aShcn)
{
    HRESULT sHResult;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    sHResult = GetFolderPath(aShcn->mPidl1, sPath);
    if (FAILED(sHResult))
        return XPR_FALSE;

    if (_tcsicmp(sPath, getCurPath()) == 0 && IsExistFile(sPath) == XPR_TRUE)
    {
        OnShcnDeleteItem(XPR_NULL);

        ShNotifyInfo sShNotifyInfo = {0};
        sShNotifyInfo.mEventId = aShcn->mEventId;
        enumerateShcn(&sShNotifyInfo, XPR_FALSE);

        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_uint_t ExplorerCtrl::getThumbImageId(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return Thumbnail::InvalidThumbImageId;

    Thumbnail &sThumbnail = Thumbnail::instance();

    xpr_uint_t sThumbImageId = Thumbnail::InvalidThumbImageId;

    if (sThumbnail.getImageId(aPath, sThumbImageId) == XPR_FALSE)
        sThumbImageId = Thumbnail::InvalidThumbImageId;

    if (sThumbImageId == Thumbnail::InvalidThumbImageId)
        sThumbnail.getAsyncImage(m_hWnd, WM_THUMBNAIL_PROC, mCode, aPath, mOption.mThumbnailLoadByExt);

    return sThumbImageId;
}

xpr_bool_t ExplorerCtrl::updateShcnLvItemData(LPLVITEMDATA aLvItemData)
{
    xpr_sint_t   sIndex;
    xpr_sint_t   sFindStartIndex;
    LPLVITEMDATA sTempLvItemData;
    xpr_bool_t   sChanged;
    HRESULT      sComResult;

    sIndex = sFindStartIndex = -1;
    sChanged = XPR_TRUE;

    xpr_bool_t sVirtualItem = IsVirtualItem(aLvItemData->mShellFolder, aLvItemData->mPidl);
    if (XPR_IS_TRUE(sVirtualItem))
    {
        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};

        while (true)
        {
            // virtual item
            GetName(aLvItemData->mShellFolder, aLvItemData->mPidl, SHGDN_INFOLDER, sName);
            sIndex = findItemName(sName, sFindStartIndex);

            if (sIndex != -1)
            {
                sTempLvItemData = (LPLVITEMDATA)GetItemData(sIndex);

                sComResult = aLvItemData->mShellFolder->CompareIDs(0, aLvItemData->mPidl, sTempLvItemData->mPidl);
                if (sComResult != S_OK) // S_FALSE, Windows7 have 2 Control Panels in desktop folder.
                {
                    sFindStartIndex = sIndex + 1;
                    continue;
                }

                sChanged = ((xpr_sshort_t)SCODE_CODE(GetScode(sComResult)) == 0) ? XPR_TRUE : XPR_FALSE;
            }

            break;
        }
    }
    else
    {
        // file system item
        sIndex = findItemPath(aLvItemData->mShellFolder, aLvItemData->mPidl);
        if (sIndex != -1)
        {
            sTempLvItemData = (LPLVITEMDATA)GetItemData(sIndex);

            // SHCIDS_ALLFIELDS : internet explorer 5.0 more
            sComResult = aLvItemData->mShellFolder->CompareIDs(SHCIDS_ALLFIELDS, aLvItemData->mPidl, sTempLvItemData->mPidl);
            sChanged = ((xpr_sshort_t)SCODE_CODE(GetScode(sComResult)) == 0) ? XPR_FALSE : XPR_TRUE;

            if (XPR_IS_FALSE(sChanged))
                return XPR_FALSE;
        }
    }

    xpr_bool_t sResult  = XPR_FALSE;
    xpr_bool_t sNewItem = XPR_TRUE;

    if (sIndex != -1)
    {
        if (XPR_IS_TRUE(sChanged))
        {
            if (XPR_IS_NOT_NULL(sTempLvItemData) && XPR_TEST_BITS(sTempLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                xpr_sint_t i, sCount;
                LVITEM sLvItem = {0};

                // update name hash
                eraseNameHash(sTempLvItemData);
                insertNameHash(aLvItemData);

                // update LVITEMDATA
                sLvItem.mask   = LVIF_PARAM;
                sLvItem.iItem  = sIndex;
                sLvItem.lParam = (LPARAM)aLvItemData;
                SetItem(&sLvItem);

                // recall LVN_GETDISPINFO for text
                sCount = GetHeaderCtrl()->GetItemCount();
                for (i = 0; i < sCount; ++i)
                {
                    SetItemText(sIndex, i, LPSTR_TEXTCALLBACK);
                }

                // recall LVN_GETDISPINFO for image
                sLvItem.mask   = LVIF_IMAGE;
                sLvItem.iItem  = sIndex;
                sLvItem.iImage = I_IMAGECALLBACK;
                SetItem(&sLvItem);

                // update state
                if (sTempLvItemData->mShellAttributes != aLvItemData->mShellAttributes)
                {
                    sLvItem.mask = LVIF_STATE;

                    if (aLvItemData->mShellAttributes & SFGAO_GHOSTED)
                    {
                        sLvItem.state     |= LVIS_CUT;
                        sLvItem.stateMask |= LVIS_CUT;
                    }
                    else
                    {
                        sLvItem.state     &= ~LVIS_CUT;
                        sLvItem.stateMask |= LVIS_CUT;
                    }

                    if (aLvItemData->mShellAttributes & SFGAO_SHARE)
                    {
                        sLvItem.state     |= INDEXTOOVERLAYMASK(1);
                        sLvItem.stateMask |= LVIS_OVERLAYMASK;
                    }
                    else
                    {
                        sLvItem.state     &= ~(INDEXTOOVERLAYMASK(1));
                        sLvItem.stateMask |= LVIS_OVERLAYMASK;
                    }

                    SetItemState(sIndex, &sLvItem);
                }

                // free old LVITEMDATA
                COM_RELEASE(sTempLvItemData->mShellFolder);
                COM_RELEASE(sTempLvItemData->mShellFolder2);
                COM_FREE(sTempLvItemData->mPidl);
                XPR_SAFE_DELETE(sTempLvItemData);

                sResult = XPR_TRUE;
            }

            sNewItem = XPR_FALSE;
        }
    }

    if (XPR_IS_TRUE(sNewItem))
    {
        LV_ITEM sLvItem = {0};
        sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        sLvItem.iItem      = getLastInsertIndex();
        sLvItem.iSubItem   = 0;
        sLvItem.cchTextMax = XPR_MAX_PATH;
        sLvItem.pszText    = LPSTR_TEXTCALLBACK;
        sLvItem.iImage     = I_IMAGECALLBACK;
        sLvItem.lParam     = (LPARAM)aLvItemData;

        if (InsertItem(&sLvItem) != -1)
        {
            insertNameHash(aLvItemData);

            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnDeleteItem(Shcn *aShcn, const xpr_tchar_t *aPath)
{
    xpr_bool_t sResult = XPR_FALSE;
    xpr_sint_t sIndex = 0;
    HRESULT sHResult;

    SetRedraw(XPR_FALSE);

    if (XPR_IS_NOT_NULL(aShcn))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath);
        if (SUCCEEDED(sHResult))
        {
            sIndex = findItemPath(sPath);
            if (sIndex != -1)
                sResult = DeleteItem(sIndex);
        }
    }
    else if (XPR_IS_NOT_NULL(aPath))
    {
        sIndex = findItemPath(aPath);
        if (sIndex != -1)
            sResult = DeleteItem(sIndex);
    }
    else
    {
        sIndex = -1;

        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        LPLVITEMDATA sLvItemData = XPR_NULL;
        xpr_sint_t i, sCount;

        sCount = GetItemCount();
        for (i = sCount - 1; i >= 0; --i)
        {
            sLvItemData = (LPLVITEMDATA)GetItemData(i);
            if (XPR_IS_NULL(sLvItemData))
                continue;

            sHResult = GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);
            if (FAILED(sHResult) || !XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
                continue;

            if (IsExistFile(sPath) == XPR_FALSE)
            {
                if (DeleteItem(i) == XPR_TRUE)
                {
                    sResult = XPR_TRUE;
                    sIndex = i;
                }
            }
        }

        if (sIndex != -1)
        {
            sResult = XPR_TRUE;

            // [2008/02/17] bug patched
            // When dwg/docx file execute, program execute and after temp files is created.
            // Focus move problem on next item, when that program close and delete that temp files.
            if (getViewStyle() == VIEW_STYLE_LIST)
                EnsureVisible(sIndex-1, XPR_FALSE);
        }
    }

    SetRedraw();

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnUpdateItem(Shcn *aShcn)
{
    xpr_bool_t sResult = XPR_FALSE;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    HRESULT sComResult = GetFolderPath(aShcn->mPidl1, sPath);
    if (FAILED(sComResult))
        return XPR_FALSE;

    if (_tcsicmp(sPath, getCurPath()) == 0)
    {
        sResult = OnShcnUpdateDir(aShcn);
    }
    else
    {
        GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath);

        // get the full qualified pidl once more because aShcn->mPidl1 is simple PIDL.
        LPITEMIDLIST sPidl = base::Pidl::create(sPath);
        if (XPR_IS_NOT_NULL(sPidl))
        {
            xpr_sint_t sIndex = findItemPath(sPath);
            if (sIndex == -1)
            {
                LPITEMIDLIST sOrgPidl = aShcn->mPidl1;

                aShcn->mPidl1 = sPidl;

                sResult = OnShcnCreateItem(aShcn);

                aShcn->mPidl1 = sOrgPidl;
            }
            else
            {
                sResult = OnShcnRenameItem(sPath, sPidl);
            }

            COM_FREE(sPidl);
        }
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnUpdateName(void)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t i, sCount;
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sItemText[XPR_MAX_PATH + 1] = {0};
    LPLVITEMDATA sLvItemData = XPR_NULL;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sLvItemData))
            continue;

        if (getItemName(sLvItemData, sName, XPR_MAX_PATH) == XPR_FALSE)
            continue;

        GetItemText(i, 0, sItemText, XPR_MAX_PATH);

        if (_tcscmp(sName, sItemText) != NAME_CASE_TYPE_DEFAULT)
        {
            if (mOption.mNameCaseType == NAME_CASE_TYPE_UPPER)
                _tcsupr(sName);
            else if (mOption.mNameCaseType == NAME_CASE_TYPE_LOWER)
                _tcslwr(sName);

            sResult = SetItemText(i, 0, sName);
            break;
        }
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnNetShare(Shcn *aShcn)
{
    ShNotifyInfo sShNotifyInfo = {0};
    sShNotifyInfo.mEventId = aShcn->mEventId;
    HRESULT sHResult = GetName(aShcn->mPidl1, SHGDN_INFOLDER, sShNotifyInfo.mNewName);
    if (FAILED(sHResult))
        return XPR_FALSE;

    enumerateShcn(&sShNotifyInfo, XPR_FALSE);

    return XPR_TRUE;
}

xpr_bool_t ExplorerCtrl::OnShcnEnumNetShare(LPLVITEMDATA aLvItemData, ShNotifyInfo *aShNotifyInfo)
{
    xpr_bool_t sResult = XPR_FALSE;

    LVFINDINFO sLvFindInfo = {0};
    sLvFindInfo.flags = LVFI_STRING;
    sLvFindInfo.psz   = aShNotifyInfo->mNewName;

    xpr_sint_t sIndex = FindItem(&sLvFindInfo);
    if (sIndex != -1)
    {
        LV_ITEM sLvItem = {0};
        sLvItem.mask      = LVIF_STATE | LVIF_PARAM;
        sLvItem.stateMask = LVIS_OVERLAYMASK;
        sLvItem.iItem     = sIndex;
        if (GetItem(&sLvItem) == XPR_TRUE)
        {
            LPLVITEMDATA sTempLvItemData = (LPLVITEMDATA)sLvItem.lParam;
            verifyItemData(&sTempLvItemData);
            if (XPR_IS_NOT_NULL(aLvItemData) && XPR_IS_NOT_NULL(sTempLvItemData))
            {
                if (sTempLvItemData->mShellAttributes != aLvItemData->mShellAttributes)
                {
                    sLvItem.state = 0;

                    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_SHARE)) sLvItem.state = INDEXTOOVERLAYMASK(1);
                    if (XPR_TEST_BITS(aLvItemData->mShellAttributes, SFGAO_LINK))  sLvItem.state = INDEXTOOVERLAYMASK(2);

                    // shell icon custom overlay
                    ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
                    sAsyncIcon->mType              = ShellIcon::TypeOverlayIndex;
                    sAsyncIcon->mCode              = mCode;
                    sAsyncIcon->mItem              = sIndex;
                    sAsyncIcon->mSignature         = aLvItemData->mSignature;
                    sAsyncIcon->mShellFolder       = aLvItemData->mShellFolder;
                    sAsyncIcon->mPidl              = fxfile::base::Pidl::clone(aLvItemData->mPidl);
                    sAsyncIcon->mResult.mIconIndex = -1;
                    sAsyncIcon->mShellFolder->AddRef();

                    if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
                    {
                        XPR_SAFE_DELETE(sAsyncIcon);
                    }

                    sLvItem.lParam = (LPARAM)aLvItemData;
                    SetItem(&sLvItem);

                    sResult = XPR_TRUE;
                }
            }
        }
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnDriveAdd(Shcn *aShcn)
{
    if (mFolderType != FOLDER_COMPUTER)
        return XPR_FALSE;

    xpr_bool_t sResult = OnShcnCreateItem(aShcn);
    if (XPR_IS_TRUE(sResult))
    {
        resortItems();
    }

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnDriveRemove(Shcn *aShcn)
{
    if (mFolderType != FOLDER_COMPUTER)
        return XPR_FALSE;

    SetRedraw(XPR_FALSE);

    xpr_tchar_t sDrive[XPR_MAX_PATH + 1];
    HRESULT sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sDrive);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    xpr_tchar_t sDrive2[XPR_MAX_PATH + 1];
    LPLVITEMDATA sLvItemData = XPR_NULL;
    xpr_sint_t i, sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            sHResult = GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sDrive2);
            if (SUCCEEDED(sHResult) && _tcsicmp(sDrive, sDrive2) == 0)
            {
                sResult = DeleteItem(i);
                break;
            }
        }
    }

    SetRedraw();

    return sResult;
}

xpr_bool_t ExplorerCtrl::OnShcnMediaInserted(Shcn *aShcn)
{
    if (mFolderType == FOLDER_COMPUTER)
        refresh();

    return XPR_FALSE;
}

xpr_bool_t ExplorerCtrl::OnShcnMediaRemoved(Shcn *aShcn)
{
    if (mFolderType == FOLDER_COMPUTER)
        refresh();

    return XPR_FALSE;
}

xpr_bool_t ExplorerCtrl::OnShcnFreeSize(Shcn *aShcn)
{
    if (mObserver != XPR_NULL)
    {
        mObserver->onUpdateFreeSize(*this);
    }

    updateDriveItem();

    return OnShcnBitBucket(aShcn);
}

xpr_bool_t ExplorerCtrl::OnShcnBitBucket(Shcn *aShcn)
{
    xpr_bool_t sBitBucket = XPR_FALSE;

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (SUCCEEDED(sHResult))
    {
        LPTVITEMDATA sTvItemData = getFolderData();
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_IS_NOT_NULL(sTvItemData->mFullPidl))
            sBitBucket = fxfile::base::Pidl::compare(sTvItemData->mFullPidl, CSIDL_BITBUCKET) == 0;
    }
    COM_RELEASE(sShellFolder);

    if (XPR_IS_TRUE(sBitBucket))
        refresh();

    return XPR_FALSE;
}

void ExplorerCtrl::OnGetinfotip(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMLVGETINFOTIP sNmLvGetInfoTip = (LPNMLVGETINFOTIP)aNmHdr;
    *aResult = 0;

    LPLVITEMDATA sLvItemData = (LPLVITEMDATA)GetItemData(sNmLvGetInfoTip->iItem);
    if (XPR_IS_NULL(sLvItemData))
        return;

    if (getDescription(sLvItemData, sNmLvGetInfoTip->pszText, sNmLvGetInfoTip->cchTextMax) == XPR_FALSE)
        getNetDescription(sLvItemData, sNmLvGetInfoTip->pszText, sNmLvGetInfoTip->cchTextMax);
}

void ExplorerCtrl::setHiddenSystem(xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    xpr_bool_t sShow = XPR_FALSE;
    xpr_bool_t sHide = XPR_FALSE;

    if (XPR_IS_TRUE(aModifiedHidden))
    {
        if (mOption.mShowHiddenAttribute == XPR_TRUE) sShow = XPR_TRUE;
        else                                          sHide = XPR_TRUE;
    }

    if (XPR_IS_TRUE(aModifiedSystem))
    {
        if (mOption.mShowSystemAttribute == XPR_TRUE) sShow = XPR_TRUE;
        else                                          sHide = XPR_TRUE;
    }

    if (XPR_IS_TRUE(sHide) && XPR_IS_TRUE(mOption.mShowSystemAttribute))
        sHide = XPR_FALSE;

    if (XPR_IS_TRUE(sShow))
        showHiddenSystem(aModifiedHidden, aModifiedSystem);

    if (XPR_IS_TRUE(sHide))
        hideHiddenSystem(aModifiedHidden, aModifiedSystem);

    updateStatus();
}

void ExplorerCtrl::showHiddenSystem(xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    ShNotifyInfo sShNotifyInfo = {0};
    sShNotifyInfo.mEventId = SHCNE_UPDATEDIR;
    enumerateShcn(&sShNotifyInfo, XPR_TRUE);

    resortItems();
}

void ExplorerCtrl::hideHiddenSystem(xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    xpr_bool_t sDelete;
    LPLVITEMDATA sLvItemData;
    xpr_sint_t i, sCount;

    i = 0;
    sCount = GetItemCount();
    while (i < sCount)
    {
        sLvItemData = (LPLVITEMDATA)GetItemData(i);
        if (XPR_IS_NOT_NULL(sLvItemData))
        {
            sDelete = XPR_FALSE;

            if (mOption.mShowSystemAttribute == XPR_FALSE && mOption.mShowHiddenAttribute  == XPR_FALSE && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_GHOSTED))
            {
                sDelete = XPR_TRUE;
            }

            if (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sLvItemData->mFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sLvItemData->mFileAttributes, FILE_ATTRIBUTE_SYSTEM))
            {
                sDelete = XPR_TRUE;
            }

            if (XPR_IS_TRUE(sDelete))
            {
                if (DeleteItem(i) == XPR_TRUE)
                {
                    sCount--;
                    continue;
                }
            }
        }

        i++;
    }
}

LPTVITEMDATA ExplorerCtrl::getFolderData(void) const
{
    return mTvItemData;
}

const xpr_tchar_t *ExplorerCtrl::getCurPath(void) const
{
    return mCurPath.c_str();
}

const xpr_tchar_t *ExplorerCtrl::getCurFullPath(void) const
{
    return mCurFullPath.c_str();
}

void ExplorerCtrl::getCurPath(xpr_tchar_t *aCurPath) const
{
    if (XPR_IS_NOT_NULL(aCurPath))
    {
        _tcscpy(aCurPath, mCurPath.c_str());
    }
}

void ExplorerCtrl::getCurFullPath(xpr_tchar_t *aCurFullPath) const
{
    if (XPR_IS_NOT_NULL(aCurFullPath))
    {
        _tcscpy(aCurFullPath, mCurFullPath.c_str());
    }
}

void ExplorerCtrl::getCurPath(xpr::string &aCurPath) const
{
    aCurPath = mCurPath;
}

void ExplorerCtrl::getCurFullPath(xpr::string &aCurFullPath) const
{
    aCurFullPath = mCurFullPath;
}

LPITEMIDLIST ExplorerCtrl::getFolderGroup(void) const
{
    LPTVITEMDATA sTvItemData = getFolderData();
    if (XPR_IS_NULL(sTvItemData))
        return XPR_NULL;

    LPITEMIDLIST sFolderGroupFullPidl = XPR_NULL;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
    LPITEMIDLIST sTempFullPidl = sFullPidl;

    xpr_sint_t i;
    for (i = 0; i < 1; ++i)
    {
        if (sTempFullPidl->mkid.cb == 0)
            break;

        sTempFullPidl = (LPITEMIDLIST)((xpr_byte_t *)sTempFullPidl + sTempFullPidl->mkid.cb);
    }

    xpr_ushort_t sTempSize = sTempFullPidl->mkid.cb;
    sTempFullPidl->mkid.cb = 0;

    sFolderGroupFullPidl = fxfile::base::Pidl::clone(sFullPidl);

    sTempFullPidl->mkid.cb = sTempSize;
    COM_FREE(sFullPidl);

    return sFolderGroupFullPidl;
}

void ExplorerCtrl::OnSysColorChange(void) 
{
    super::OnSysColorChange();

    COLORREF sWindowColor = ::GetSysColor(COLOR_WINDOW);
    mExtraLargeImgList->SetBkColor(sWindowColor);
    mLargeImgList->SetBkColor(sWindowColor);
    mMediumImgList->SetBkColor(sWindowColor);
    mSmallImgList->SetBkColor(sWindowColor);

    if (mOption.mBkgndColorType != COLOR_TYPE_CUSTOM)
    {
        SetTextBkColor(sWindowColor);
        SetBkColor(sWindowColor);
    }
}

void ExplorerCtrl::OnDeleteallitems(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW *)aNmHdr;

    Thumbnail::instance().clearAysncImage();

    if (XPR_IS_NOT_NULL(mShellIcon))
    {
        mShellIcon->clear();
    }

    clearNameHash();

    *aResult = 0;
}

xpr_bool_t ExplorerCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *aResult) 
{
    NMHDR *sNmHdr = (NMHDR *)lParam;

    // header control's id is 0.
    if (XPR_IS_NOT_NULL(sNmHdr) && sNmHdr->idFrom == 0)
    {
        switch (sNmHdr->code)
        {
        case HDN_ITEMCHANGED:
            {
                OnHdnItemChanged(sNmHdr);
                break;
            }
        }
    }

    return super::OnNotify(wParam, lParam, aResult);
}

void ExplorerCtrl::OnHdnItemChanged(NMHDR *aNmHdr)
{
    if (XPR_IS_NULL(aNmHdr))
        return;

    // recursive prevention
    static xpr_bool_t sSetColumnWidth = XPR_FALSE;
    if (XPR_IS_TRUE(sSetColumnWidth))
        return;

    NMHEADER *sNmHeader = (NMHEADER *)aNmHdr;

    if (XPR_TEST_BITS(sNmHeader->pitem->mask, HDI_WIDTH))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            xpr_sint_t sColumnIndex = sNmHeader->iItem;
            xpr_sint_t sWidth = GetColumnWidth(sColumnIndex);

            sSetColumnWidth = XPR_TRUE;

            FolderLayoutChange sFolderLayoutChange;
            sFolderLayoutChange.mChangeMode  = FolderLayoutChange::ChangeModeColumnSize;
            sFolderLayoutChange.mColumnIndex = sColumnIndex;
            sFolderLayoutChange.mColumnWidth = sWidth;

            mObserver->onFolderLayoutChange(*this, sFolderLayoutChange);

            sSetColumnWidth = XPR_FALSE;
        }
    }
}

xpr_bool_t ExplorerCtrl::OnEraseBkgnd(CDC *aDC)
{
    return super::OnEraseBkgnd(aDC);
}

xpr_sint_t ExplorerCtrl::OnMouseActivate(CWnd *aDesktopWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage) 
{
    CPoint sPoint;
    GetCursorPos(&sPoint);
    ScreenToClient(&sPoint);

    xpr_sint_t sIndex = HitTest(sPoint);
    if (sIndex >= 0)
        return MA_NOACTIVATE;

    return super::OnMouseActivate(aDesktopWnd, aHitTest, aMessage);
}

void ExplorerCtrl::OnMButtonDown(xpr_uint_t aFlags, CPoint aPoint) 
{
    CWindowScroller *ws = new CWindowScroller(this, aPoint, 5, 10);

    super::OnMButtonDown(aFlags, aPoint);
}

void ExplorerCtrl::OnLvnKeyDown(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_KEYDOWN *sLvKeyDown = (LV_KEYDOWN*)aNmHdr;
    *aResult = 0;
}

// LBUTTONUP
void ExplorerCtrl::OnClick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NMITEMACTIVATE *sNmItemActivate = (NMITEMACTIVATE *)aNmHdr;
    *aResult = 0;
}

void ExplorerCtrl::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint) 
{
    super::OnLButtonDblClk(aFlags, aPoint);
}

void ExplorerCtrl::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint) 
{
    super::OnLButtonDown(aFlags, aPoint);
}

void ExplorerCtrl::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint) 
{
    //xpr_sint_t sIndex = HitTest(aPoint);
    //if (sIndex == -1)
    //    return;

    if (mOption.mDragType == DRAG_START_DIST)
    {
        static xpr_bool_t sDrag = XPR_FALSE;
        static CPoint sDragPoint(0,0);

        if ((XPR_IS_FALSE(sDrag)) && (aFlags & MK_LBUTTON || aFlags & MK_RBUTTON))
        {
            xpr_sint_t sIndex = HitTest(aPoint);
            if (sIndex >= 0)
            {
                sDragPoint = aPoint;
                sDrag = XPR_TRUE;

                SetCapture();
            }
        }

        if (XPR_IS_TRUE(sDrag))
        {
            xpr_sint_t sXDist = sDragPoint.x - aPoint.x;
            xpr_sint_t sYDist = sDragPoint.y - aPoint.y;
            xpr_sint_t sRDist = (xpr_sint_t)sqrt((xpr_float_t)(sXDist * sXDist + sYDist * sYDist));

            if (sRDist > mOption.mDragDist)
            {
                ReleaseCapture();

                beginDragDrop(XPR_NULL);
                sDrag = XPR_FALSE;
                return;
            }
        }
    }

    super::OnMouseMove(aFlags, aPoint);
}

void ExplorerCtrl::OnVScroll(xpr_uint_t aSbCode, xpr_uint_t aPos, CScrollBar *aScrollBar) 
{
    super::OnVScroll(aSbCode, aPos, aScrollBar);

    // [2009/02/17] bug patched
    // If view style is detail and grid line option enable, window painting is broken.
    if (getViewStyle() == VIEW_STYLE_DETAILS)
    {
        if (XPR_TEST_BITS(GetExtendedStyle(), LVS_EX_GRIDLINES))
        {
            if (aSbCode != SB_THUMBTRACK)
                Invalidate(XPR_FALSE);
        }
    }
}

void ExplorerCtrl::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint) 
{
    super::OnLButtonUp(aFlags, aPoint);
}

void ExplorerCtrl::OnRButtonDown(xpr_uint_t aFlags, CPoint aPoint) 
{
    super::OnRButtonDown(aFlags, aPoint);
}

void ExplorerCtrl::OnRButtonUp(xpr_uint_t aFlags, CPoint aPoint) 
{
    super::OnRButtonUp(aFlags, aPoint);
}

void ExplorerCtrl::setCustomFont(xpr_bool_t aCustomFont, xpr_tchar_t *aFontText)
{
    if (XPR_IS_FALSE(aCustomFont))
    {
        SetFont(XPR_NULL);
        return;
    }

    if (XPR_IS_NULL(aFontText))
        return;

    if (XPR_IS_NOT_NULL(mCustomFont))
        mCustomFont->DeleteObject();
    else
        mCustomFont = new CFont;

    LOGFONT sLogFont = {0};
    StringToLogFont(aFontText, sLogFont);

    mCustomFont->CreateFontIndirect(&sLogFont);
    if (XPR_IS_NOT_NULL(mCustomFont->m_hObject))
        SetFont(mCustomFont);
}

void ExplorerCtrl::setCustomFont(xpr_bool_t aCustomFont, CFont *aFont)
{
    if (XPR_IS_FALSE(aCustomFont))
    {
        SetFont(XPR_NULL);
        return;
    }

    if (XPR_IS_NULL(aFont) || XPR_IS_NULL(aFont->m_hObject))
        return;

    if (XPR_IS_NOT_NULL(mCustomFont))
        mCustomFont->DeleteObject();
    else
        mCustomFont = new CFont;

    LOGFONT sLogFont = {0};
    aFont->GetLogFont(&sLogFont);

    mCustomFont->CreateFontIndirect(&sLogFont);
    if (XPR_IS_NOT_NULL(mCustomFont->m_hObject))
        SetFont(mCustomFont);
}

void ExplorerCtrl::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);
}

LRESULT ExplorerCtrl::OnDriveShellChangeNotify(WPARAM wParam, LPARAM lParam)
{
    if (mFolderType == FOLDER_COMPUTER)
        return 0;

    Shcn *sShcn = (Shcn *)wParam;
    DWORD sEventId = (DWORD)lParam;

    if (XPR_IS_NULL(sShcn))
        return 0;

    switch (sShcn->mEventId)
    {
    case SHCNE_DRIVEREMOVED:
    case SHCNE_MEDIAREMOVED:
        {
            xpr::string sDrive;
            GetName(sShcn->mPidl1, SHGDN_FORPARSING, sDrive);

            xpr::string sCurPath;
            getCurPath(sCurPath);

            if (sDrive.empty() == XPR_FALSE && sCurPath.empty() == XPR_FALSE && sCurPath[0] == sDrive[0])
            {
                HRESULT sHResult;
                LPITEMIDLIST sFullPidl = XPR_NULL;

                sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sFullPidl);
                if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sFullPidl))
                {
                    if (explore(sFullPidl) == XPR_TRUE)
                        return 0;
                }

                COM_FREE(sFullPidl);

                return 0;
            }
            break;
        }
    }

    if (mOption.mShowDrive == XPR_FALSE)
        return 0;

    if (sEventId == SHCNE_FREESPACE)
    {
        DWORD sDriveBits = sShcn->mDrives.mDriveBits1;

        xpr_sint_t i, j;
        xpr_sint_t sCount;
        DWORD sItemDataType;
        xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};

        sCount = GetItemCount();

        for (i = 0; i < 26; ++i)
        {
            if (!XPR_TEST_BITS(sDriveBits, (0x1<<i)))
                continue;

            for (j = sCount - 1; j >= 0; --j)
            {
                sItemDataType = getItemDataType(j);
                if (sItemDataType == IDT_SHELL)
                    break;

                if (sItemDataType != (IDT_DRIVE+i))
                    continue;

                _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), getDriveFromItemType(sItemDataType));

                if (GetDriveType(sDrive) != DRIVE_FIXED)
                    continue;

                if (XPR_IS_TRUE(mOption.mShowDriveSize))
                {
                    // total size
                    getDriveItemTotalSize(sDrive, sText, XPR_MAX_PATH);
                    SetItemText(j, 1, sText);

                    // free size
                    getDriveItemFreeSize(sDrive, sText, XPR_MAX_PATH);
                    SetItemText(j, 2, sText);
                }
            }
        }
    }
    else
    {
        SetRedraw(XPR_FALSE);

        xpr_sint_t i;
        xpr_sint_t sCount;
        DWORD sItemDataType;

        sCount = GetItemCount();
        for (i = sCount - 1; i >= 0; --i)
        {
            sItemDataType = getItemDataType(i);
            if (sItemDataType == IDT_SHELL)
                break;

            if (XPR_IS_RANGE(IDT_DRIVE, sItemDataType, IDT_DRIVE_END) || sItemDataType == IDT_DRIVE_SEL)
                DeleteItem(i);
        }

        addDriveItem();

        SetRedraw();
    }

    return 0;
}

void ExplorerCtrl::doShChangeNotifyUpdateDir(xpr_bool_t aPidl, xpr_bool_t aFlush)
{
    xpr_uint_t sFlags = 0;
    if (XPR_IS_TRUE(aFlush))
        sFlags |= SHCNF_FLUSH;

    LPVOID sItem1 = XPR_NULL;

    if (XPR_IS_TRUE(aPidl))
    {
        LPTVITEMDATA sTvItemData = getFolderData();
        if (XPR_IS_NULL(sTvItemData))
            return;

        LPITEMIDLIST sFullPidl = sTvItemData->mFullPidl;
        if (XPR_IS_NULL(sFullPidl))
            return;

        sFlags = SHCNF_IDLIST;
        sItem1 = (LPVOID)sFullPidl;
    }
    else
    {
        sFlags = SHCNF_PATH;
        sItem1 = (LPVOID)getCurPath();
    }

    ::SHChangeNotify(SHCNE_UPDATEDIR, sFlags, (LPCVOID)sItem1, XPR_NULL);
}

void ExplorerCtrl::setChangeNotify(xpr::string aPath, xpr_bool_t aAllSubTree, xpr_bool_t aEnable)
{
    if (isFileSystemFolder() == XPR_FALSE)
        return;

    if (aPath.length() < 2)
        return;

    xpr::string sCurPath;
    getCurPath(sCurPath);

    RemoveLastSplit(aPath);
    RemoveLastSplit(sCurPath);

    if (XPR_IS_TRUE(aAllSubTree))
    {
        if (_tcsnicmp(sCurPath.c_str(), aPath.c_str(), aPath.length()) != 0)
            return;
    }
    else
    {
        if (_tcsicmp(sCurPath.c_str(), aPath.c_str()) != 0)
            return;
    }

    if (XPR_IS_FALSE(aEnable))
    {
        // stop shell change notification
        ShellChangeNotify::instance().unregisterWatch(mShcnId);
        mShcnId = 0;

        // destroy file change notification
        FileChangeWatcher::instance().unregisterWatch(mWatchId);
        mWatchId = FileChangeWatcher::InvalidWatchId;

        // destroy advanced file change wather
        AdvFileChangeWatcher::instance().unregisterWatch(mAdvWatchId);
        mAdvWatchId = AdvFileChangeWatcher::InvalidAdvWatchId;
    }
    else
    {
        LPTVITEMDATA sTvItemData = getFolderData();

        // shell change notification
        if (mShcnId == 0)
        {
            if (ShellChangeNotify::instance().modifyWatch(mShcnId, sTvItemData->mFullPidl, XPR_FALSE) == XPR_FALSE)
            {
                ShellChangeNotify::instance().unregisterWatch(mShcnId);
                mShcnId = ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, sTvItemData->mFullPidl, SHCNE_ALLEVENTS, XPR_FALSE);
            }
        }

        // file change notification
        if (mWatchId == FileChangeWatcher::InvalidWatchId)
        {
            FileChangeWatcher::WatchItem sWatchItem;
            sWatchItem.mHwnd = m_hWnd;
            sWatchItem.mMsg  = WM_FILE_CHANGE_NOTIFY;
            GetName(sTvItemData->mFullPidl, SHGDN_FORPARSING, sWatchItem.mPath);
            sWatchItem.mSubPath = XPR_FALSE;

            if (mWatchId == FileChangeWatcher::InvalidWatchId)
                mWatchId = FileChangeWatcher::instance().registerWatch(&sWatchItem);
            else
                mWatchId = FileChangeWatcher::instance().modifyWatch(mWatchId, &sWatchItem);
        }

        // advanced file change watcher
        if (mAdvWatchId == AdvFileChangeWatcher::InvalidAdvWatchId)
        {
            AdvFileChangeWatcher::AdvWatchItem *sAdvWatchItem = new AdvFileChangeWatcher::AdvWatchItem;
            if (XPR_IS_NOT_NULL(sAdvWatchItem))
            {
                sAdvWatchItem->mHwnd = m_hWnd;
                sAdvWatchItem->mMsg  = WM_ADV_FILE_CHANGE_NOTIFY;
                GetName(sTvItemData->mFullPidl, SHGDN_FORPARSING, sAdvWatchItem->mPath);
                sAdvWatchItem->mSubPath = XPR_FALSE;
            }

            if (mAdvWatchId == AdvFileChangeWatcher::InvalidAdvWatchId)
                mAdvWatchId = AdvFileChangeWatcher::instance().registerWatch(sAdvWatchItem);
            else
                mAdvWatchId = AdvFileChangeWatcher::instance().modifyWatch(mAdvWatchId, sAdvWatchItem);

            if (mAdvWatchId == AdvFileChangeWatcher::InvalidAdvWatchId)
            {
                XPR_SAFE_DELETE(sAdvWatchItem);
            }
        }
    }
}
} // namespace fxfile
