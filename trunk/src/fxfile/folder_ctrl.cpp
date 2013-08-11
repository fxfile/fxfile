//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_ctrl.h"
#include "folder_ctrl_observer.h"

#include "file_change_watcher.h" // for File Change Watcher
#include "file_op_thread.h"      // for File Operation Thread
#include "context_menu.h"        // for Context Menu
#include "file_op_undo.h"        // for File Operation Undo
#include "shell_icon.h"
#include "wnet_mgr.h"
#include "clip_format.h"
#include "clipboard.h"
#include "winapi_ex.h"
#include "app_ver.h"
#include "shell_enumerator.h"

#include "gui/gdi.h"
#include "gui/DragImage.h"
#include "gui/DropSource.h"
#include "gui/WindowScroller.h"    // for Wheel Scroller

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
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

    TM_ID_DRAG_EXPAND_ITEM,
    TM_ID_SEL_ITEM,

    TM_ID_CLICK_RENAME = 42,
};

//
// user defined message
//
enum
{
    WM_SHELL_ASYNC_ICON    = WM_USER + 11,
    WM_SHELL_CHANGE_NOTIFY = WM_USER + 1500,
    WM_FILE_CHANGE_NOTIFY  = WM_USER + 1501,
};

struct FolderCtrl::EnumData
{
    ShNotifyInfo *mShNotifyInfo;

    typedef std::deque<LPTVITEMDATA> TvItemDataDeque;
    TvItemDataDeque mTvItemDataDeque;

    xpr_bool_t isDuplicatedItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
    {
        if (mTvItemDataDeque.empty() == true)
        {
            return XPR_FALSE;
        }

        TvItemDataDeque::iterator sIterator;
        LPTVITEMDATA sTvItemData;
        HRESULT      sComResult;
        xpr_bool_t   sEqualed;

        sIterator = mTvItemDataDeque.begin();
        for (; sIterator != mTvItemDataDeque.end(); ++sIterator)
        {
            sTvItemData = *sIterator;

            sComResult = aShellFolder->CompareIDs(0, aPidl, sTvItemData->mPidl);
            sEqualed = ((xpr_sshort_t)SCODE_CODE(GetScode(sComResult)) == 0) ? XPR_TRUE : XPR_FALSE;

            if (XPR_IS_TRUE(sEqualed))
            {
                return XPR_TRUE;
            }
        }

        return XPR_FALSE;
    }
};

FolderCtrl::FolderCtrl(void)
    : mViewIndex(-1)
{
    mObserver           = XPR_NULL;

    mInit               = XPR_FALSE;
    mUpdate             = XPR_TRUE;

    mSmallImgList       = XPR_NULL;
    mShellIcon          = XPR_NULL;

    mDropTreeItem       = XPR_NULL;
    mDragExpandTreeItem = XPR_NULL;

    mScrollUpTimer      = XPR_FALSE;
    mScrollDownTimer    = XPR_FALSE;
    mScrollLeftTimer    = XPR_FALSE;
    mScrollRightTimer   = XPR_FALSE;

    mCustomFont         = XPR_NULL;

    mMouseEdit          = XPR_TRUE;
    mSkipExpand         = XPR_FALSE;

    mSelTimer           = XPR_FALSE;
    mTimerSelTreeItem   = XPR_NULL;
    mOldSelTreeItem     = XPR_NULL;

    mShcnId             = 0;
}

FolderCtrl::~FolderCtrl(void)
{
    XPR_SAFE_DELETE(mShellIcon);
}

BEGIN_MESSAGE_MAP(FolderCtrl, super)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_MOUSEACTIVATE()
    ON_WM_MBUTTONDOWN()
    ON_NOTIFY_REFLECT(NM_CLICK,           OnClick)
    ON_NOTIFY_REFLECT(NM_RCLICK,          OnRclick)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW,      OnCustomdraw)
    ON_NOTIFY_REFLECT(TVN_BEGINRDRAG,     OnBeginrdrag)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG,      OnBegindrag)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED,     OnSelchanged)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING,  OnItemexpanding)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED,   OnItemExpanded)
    ON_NOTIFY_REFLECT(TVN_DELETEITEM,     OnDeleteitem)
    ON_NOTIFY_REFLECT(TVN_GETDISPINFO,    OnGetdispinfo)
    ON_NOTIFY_REFLECT(TVN_SELCHANGING,    OnSelchanging)
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT,   OnEndlabeledit)
    ON_NOTIFY_REFLECT(TVN_KEYDOWN,        OnKeydown)
    ON_MESSAGE(WM_SHELL_CHANGE_NOTIFY, OnShellChangeNotify)
    ON_MESSAGE(WM_FILE_CHANGE_NOTIFY,  OnFileChangeNotify)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON,    OnShellAsyncIcon)
END_MESSAGE_MAP()

void FolderCtrl::setObserver(FolderCtrlObserver *aObserver)
{
    mObserver = aObserver;
}

void FolderCtrl::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

xpr_sint_t FolderCtrl::getViewIndex(void) const
{
    return mViewIndex;
}

xpr_bool_t FolderCtrl::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect) 
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

    return super::Create(sStyle, aRect, aParentWnd, aId);
}

xpr_bool_t FolderCtrl::PreCreateWindow(CREATESTRUCT &aCreateStruct) 
{
    aCreateStruct.style |= TVS_HASLINES;
    aCreateStruct.style |= TVS_HASBUTTONS;
    aCreateStruct.style |= TVS_SHOWSELALWAYS;
    aCreateStruct.style |= TVS_EDITLABELS;
    aCreateStruct.style |= TVS_NONEVENHEIGHT;

    return super::PreCreateWindow(aCreateStruct);
}

xpr_sint_t FolderCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    SetWindowText(FXFILE_PROGRAM_NAME);

    // enable vista enhanced control
    enableVistaEnhanced(XPR_TRUE);

    mDropTarget.registerObserver(this);
    mDropTarget.Register(this);

    if (XPR_IS_NULL(mShellIcon))
    {
        mShellIcon = new ShellIcon;
        mShellIcon->setOwner(*this, WM_SHELL_ASYNC_ICON);
    }

    return 0;
}

void FolderCtrl::OnDestroy(void) 
{
    super::OnDestroy();

    ShellChangeNotify::instance().unregisterWatch(mShcnId);

    if (XPR_IS_NOT_NULL(mShellIcon))
    {
        mShellIcon->Stop();
        XPR_SAFE_DELETE(mShellIcon);
    }

    mDropTarget.Revoke();

    if (XPR_IS_NOT_NULL(mCustomFont))
    {
        DELETE_OBJECT(*mCustomFont);
        XPR_SAFE_DELETE(mCustomFont);
    }
}

void FolderCtrl::setOption(Option &aOption)
{
    xpr_bool_t sModifiedHidden = (mOption.mShowSystemAttribute != aOption.mShowHiddenAttribute) ? XPR_TRUE : XPR_FALSE;
    xpr_bool_t sModifiedSystem = (mOption.mShowSystemAttribute != aOption.mShowSystemAttribute) ? XPR_TRUE : XPR_FALSE;

    // set new option
    mOption = aOption;

    // modify style
    DWORD sStyle = TVS_SINGLEEXPAND;
    if (isVistaEnhanced() == XPR_FALSE)
        sStyle |= TVS_TRACKSELECT;

    if (mOption.mMouseClick == MOUSE_ONE_CLICK)
    {
        ModifyStyle(0, sStyle);
    }
    else
    {
        ModifyStyle(sStyle, 0);
    }

    // update hidden or system attribute
    if ((XPR_IS_FALSE(mInit)) && (XPR_IS_TRUE(sModifiedHidden) || XPR_IS_TRUE(sModifiedSystem)))
        setHiddenSystem(sModifiedHidden, sModifiedSystem);

    // set item height
    if (XPR_IS_TRUE(mOption.mIsItemHeight))
    {
        SetItemHeight(mOption.mItemHeight);
    }
    else
    {
        SetItemHeight(-1);
    }

    // set background color
    if (mOption.mBkgndColorType == COLOR_TYPE_CUSTOM)
    {
        SetBkColor(mOption.mBkgndColor);
    }
    else
    {
        SetBkColor(::GetSysColor(COLOR_WINDOW));
    }

    // set text color
    if (mOption.mTextColorType == COLOR_TYPE_CUSTOM)
    {
        SetTextColor(mOption.mTextColor);
    }
    else
    {
        SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
    }

    // set custom font
    setCustomFont(mOption.mCustomFont, mOption.mCustomFontText);

    // update selected tree item
    HTREEITEM sTreeItem = GetSelectedItem();
    if (XPR_IS_NOT_NULL(sTreeItem))
    {
        CRect sRect;
        GetItemRect(sTreeItem, &sRect, XPR_FALSE);
        InvalidateRect(sRect, XPR_FALSE);
    }

    // set contents display while dragging
    setDragContents(!mOption.mDragNoContents);
}

LPARAM FolderCtrl::GetItemData(HTREEITEM aTreeItem) const
{
    if (XPR_IS_NULL(aTreeItem))
        return XPR_NULL;

    TVITEM sTvItem = {0};
    sTvItem.hItem = aTreeItem;
    sTvItem.mask  = TVIF_PARAM;
    if (::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&sTvItem) == 0)
        return XPR_NULL;

    return sTvItem.lParam;
}

void FolderCtrl::setImageList(CImageList *aSmallImgList)
{
    mSmallImgList = aSmallImgList;

    super::SetImageList(mSmallImgList, TVSIL_NORMAL);
}

void FolderCtrl::OnContextMenu(CWnd* pWnd, CPoint pt) 
{
    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    CRect sHorzScrollRect(&sWindowRect);
    CRect sVertScrollRect(&sWindowRect);
    sHorzScrollRect.top  = sHorzScrollRect.bottom - GetSystemMetrics(SM_CYHSCROLL);
    sVertScrollRect.left = sVertScrollRect.right  - GetSystemMetrics(SM_CXVSCROLL);

    if (sHorzScrollRect.PtInRect(pt) == XPR_TRUE || sVertScrollRect.PtInRect(pt) == XPR_TRUE)
    {
        // default scroll bar
        super::OnContextMenu(pWnd, pt);
        return;
    }

    trackContextMenu(XPR_FALSE);
}

void FolderCtrl::OnRclick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    trackContextMenu(XPR_TRUE);

    *aResult = 1; // nonzero to prevent the default processing
}

void FolderCtrl::trackContextMenu(xpr_bool_t aRightClick)
{
    HTREEITEM sTreeItem = XPR_NULL;

    CPoint sPoint;
    ::GetCursorPos(&sPoint);
    ScreenToClient(&sPoint);

    if (XPR_IS_TRUE(aRightClick))
    {
        sTreeItem = HitTest(sPoint);
        ClientToScreen(&sPoint);
    }
    else
    {
        sTreeItem = GetSelectedItem();

        CRect sRect;
        GetItemRect(sTreeItem, sRect, XPR_TRUE);
        ClientToScreen(&sRect);
        if (!(sRect.left < sPoint.x && sPoint.x < sRect.right) || !(sRect.top < sPoint.y && sRect.bottom < sPoint.y))
        {
            sPoint.x = sRect.left + (sRect.right-sRect.left)/2;
            sPoint.y = sRect.bottom;
        }
    }

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NOT_NULL(sTvItemData))
    {
        //ContextMenu::TrackItemMenu(sTvItemData->mShellFolder, &sTvItemData->mPidl, 1, &sPoint, sFlags, m_hWnd);

        ContextMenu sContextMenu(GetSafeHwnd());
        if (sContextMenu.init(sTvItemData->mShellFolder, (LPCITEMIDLIST *)&sTvItemData->mPidl, 1) == XPR_TRUE)
        {
            xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &sPoint);
            if (sId != -1)
            {
                xpr_uint_t sCmdId = sId - sContextMenu.getFirstId();

                if (invokeCommandSelf(&sContextMenu, sCmdId, sTreeItem) == XPR_FALSE)
                {
                    sContextMenu.invokeCommand(sCmdId);
                }
            }
        }

        sContextMenu.destroySubclass();
        sContextMenu.release();
    }
}

xpr_bool_t FolderCtrl::invokeCommandSelf(ContextMenu *aContextMenu, xpr_uint_t aId, HTREEITEM aTreeItem)
{
    if (XPR_IS_NULL(aContextMenu))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sVerb[0xff] = {0};
    aContextMenu->getCommandVerb(aId, sVerb, 0xfe);

    if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
    {
        SelectItem(aTreeItem);
        return XPR_TRUE;
    }
    else if (_tcsicmp(sVerb, CMID_VERB_DELETE) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuDelete(*this, aTreeItem);
        }

        sResult = XPR_TRUE;
    }
    else if (_tcsicmp(sVerb, CMID_VERB_RENAME) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuRename(*this, aTreeItem);
        }

        sResult = XPR_TRUE;
    }
    else if (_tcsicmp(sVerb, CMID_VERB_PASTE) == 0)
    {
        if (mObserver != XPR_NULL)
        {
            mObserver->onContextMenuPaste(*this, aTreeItem);
        }

        sResult = XPR_TRUE;
    }

    return sResult;
}

void FolderCtrl::OnSelchanging(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW*)aNmHdr;

    xpr_bool_t sAllowSel;
    sAllowSel = (sNmTreeView->itemNew.lParam != 0xcccccccc && sNmTreeView->itemNew.lParam != XPR_NULL);

    *aResult = XPR_IS_TRUE(sAllowSel) ? 0 : 1;
}

void FolderCtrl::OnSelchanged(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW*)aNmHdr;
    *aResult = XPR_TRUE;

    HTREEITEM sTreeItem = sNmTreeView->itemNew.hItem;

    xpr_bool_t sSelTimer = mSelTimer;
    mSelTimer = XPR_FALSE;

    if (XPR_IS_FALSE(mUpdate))
    {
        if (*aResult != 0)
            mOldSelTreeItem = sTreeItem;

        return;
    }

    if (XPR_IS_TRUE(sSelTimer))
    {
        mTimerSelTreeItem = sTreeItem;
        SetTimer(TM_ID_SEL_ITEM, mOption.mSelDelayTime, XPR_NULL);
    }
    else
    {
        explore(sTreeItem);
    }
}

xpr_bool_t FolderCtrl::explore(HTREEITEM aTreeItem)
{
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    xpr_bool_t sResult = exploreItem(sTvItemData);
    if (XPR_IS_FALSE(sResult))
    {
        if (XPR_IS_NOT_NULL(mOldSelTreeItem))
        {
            setUpdate(XPR_FALSE);

            sResult = SelectItem(mOldSelTreeItem);
            if (XPR_IS_TRUE(sResult))
                aTreeItem = mOldSelTreeItem;

            setUpdate(XPR_TRUE);
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        HTREEITEM sRootTreeItem = GetRootItem();
        if (XPR_IS_NOT_NULL(sRootTreeItem))
        {
            sTvItemData = (LPTVITEMDATA)GetItemData(sRootTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData))
                sResult = exploreItem(sTvItemData);

            if (XPR_IS_TRUE(sResult))
                aTreeItem = sRootTreeItem;
        }
    }

    if (XPR_IS_TRUE(sResult))
        mOldSelTreeItem = aTreeItem;

    return sResult;
}

xpr_bool_t FolderCtrl::exploreItem(LPTVITEMDATA aTvItemData)
{
    if (XPR_IS_NULL(aTvItemData))
        return XPR_FALSE;

    if (fxfile::base::Pidl::compare(aTvItemData->mFullPidl, CSIDL_INTERNET) == 0)
        return XPR_FALSE;

    // update notify message
    xpr_bool_t sResult = XPR_TRUE;
    if (mObserver != XPR_NULL)
    {
        sResult = mObserver->onExplore(*this, aTvItemData->mFullPidl, XPR_FALSE);
    }

    return sResult;
}

void FolderCtrl::getFullPath(HTREEITEM aTreeItem, xpr::tstring &aFullPath) const
{
    aFullPath.clear();

    while (XPR_IS_NOT_NULL(aTreeItem))
    {
        aFullPath.insert(0, GetItemText(aTreeItem));

        aTreeItem = GetParentItem(aTreeItem);
        if (XPR_IS_NOT_NULL(aTreeItem))
            aFullPath.insert(0, XPR_STRING_LITERAL("\\"));
    }
}

void FolderCtrl::getFullPath(HTREEITEM aTreeItem, xpr_tchar_t *aFullPath, xpr_size_t aMaxLen) const
{
    xpr::tstring sFullPath;
    getFullPath(aTreeItem, sFullPath);

    if (sFullPath.length() <= aMaxLen)
        _tcscpy(aFullPath, sFullPath.c_str());
}

void FolderCtrl::OnItemexpanding(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW*)aNmHdr;
    *aResult = 0;

    if (XPR_IS_TRUE(mSkipExpand))
        return;

    HTREEITEM sTreeItem = sNmTreeView->itemNew.hItem;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sNmTreeView->itemNew.lParam;
    if (XPR_IS_NULL(sTvItemData))
        return;

    xpr_bool_t sExpandedPartial = XPR_FALSE;

    if (XPR_TEST_BITS(sNmTreeView->itemNew.state, TVIS_EXPANDPARTIAL))
    {
        sTvItemData->mExpandPartial = XPR_FALSE;
        sExpandedPartial = XPR_TRUE;
    }
    else
    {
        // collapse
        if (XPR_TEST_BITS(sNmTreeView->itemNew.state, TVIS_EXPANDEDONCE))
            return;
    }

    if (XPR_IS_TRUE(sTvItemData->mExpandPartial))
        return;

    EnumData sEnumData;

    if (XPR_IS_TRUE(sExpandedPartial))
    {
        LPTVITEMDATA sChildTvItemData;
        HTREEITEM sChildTreeItem;

        sChildTreeItem = GetChildItem(sTreeItem);
        while (XPR_IS_NOT_NULL(sChildTreeItem))
        {
            sChildTvItemData = (LPTVITEMDATA)GetItemData(sChildTreeItem);
            if (XPR_IS_NOT_NULL(sChildTvItemData))
                sEnumData.mTvItemDataDeque.push_back(sChildTvItemData);

            sChildTreeItem = GetNextSiblingItem(sChildTreeItem);
        }
    }

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder2 = XPR_NULL;
    if (sTvItemData->mFullPidl->mkid.cb == 0) // desktop folder
    {
        sShellFolder2 = sTvItemData->mShellFolder;
        sHResult = sShellFolder2->AddRef();
    }
    else
    {
        sHResult = sTvItemData->mShellFolder->BindToObject(sTvItemData->mPidl, 0, IID_IShellFolder, (LPVOID *)&sShellFolder2);
    }

    if (SUCCEEDED(sHResult))
    {
        enumerate(sShellFolder2,
                  sTvItemData->mFullPidl,
                  sNmTreeView->itemNew.hItem,
                  &sEnumData);
    }

    TVSORTCB sTvSortCb = {0};
    sTvSortCb.hParent     = sNmTreeView->itemNew.hItem;
    sTvSortCb.lParam      = 0;
    sTvSortCb.lpfnCompare = TreeViewCompareProc;
    SortChildrenCB(&sTvSortCb /*, XPR_FALSE*/);

    COM_RELEASE(sShellFolder2);

    HTREEITEM sChildTreeItem = GetChildItem(sTreeItem);
    if (XPR_IS_NULL(sChildTreeItem))
    {
        TVITEM sTvItem = {0};
        sTvItem.mask      = TVIF_HANDLE | TVIF_CHILDREN;
        sTvItem.hItem     = sTreeItem;
        sTvItem.cChildren = 0;
        SetItem(&sTvItem);

        Expand(sTreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
    }
}

void FolderCtrl::OnItemExpanded(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW *)aNmHdr;
    *aResult = 0;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sNmTreeView->itemNew.lParam;
    if (XPR_IS_NULL(sTvItemData))
        return;

    if (XPR_TEST_BITS(sNmTreeView->itemNew.state, TVIS_EXPANDED))
    {
        if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            //if (sTvItemData->uWatchId == FileChangeWatcher::InvalidWatchId)
            //{
            //    FileChangeWatcher::CWatchItem sWatchItem;
            //    sWatchItem.hWnd = m_hWnd;
            //    sWatchItem.uMsg = WM_FILE_CHANGE_NOTIFY;
            //    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sWatchItem.strPath);
            //    sWatchItem.bSubPath = XPR_FALSE;
            //    sWatchItem.lParam = (LPARAM)sNmTreeView->itemNew.hItem;

            //    sTvItemData->uWatchId = FileChangeWatcher::instance().Register(&sWatchItem);
            //}
        }
    }
}

void FolderCtrl::setUpdate(xpr_bool_t aUpdate)
{
    mUpdate = aUpdate;
}

xpr_bool_t FolderCtrl::init(LPITEMIDLIST aRootFullPidl, xpr_bool_t aSelDefItem, const xpr_tchar_t *aSelFullPath, xpr_bool_t *aSelItem)
{
    HRESULT       sComResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPSHELLFOLDER sRootShellFolder = XPR_NULL;
    TVSORTCB      sTvSortCb;
    HTREEITEM     sPrevTreeItem;

    sComResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sComResult))
    {
        return XPR_FALSE;
    }

    // delete all items
    DeleteAllItems();

    // build root item
    if (XPR_IS_NOT_NULL(aRootFullPidl))
    {
        // root item: specific folder

        mShcnId = ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, aRootFullPidl, SHCNE_ALLEVENTS, XPR_TRUE);

        sShellFolder->BindToObject(aRootFullPidl, 0, IID_IShellFolder, (LPVOID *)&sRootShellFolder);

        enumerate(sRootShellFolder,
                  aRootFullPidl,
                  TVI_ROOT,
                  XPR_NULL);
    }
    else
    {
        // root item: default desktop folder

        LPITEMIDLIST   sPidl            = XPR_NULL;
        LPITEMIDLIST   sFullPidl        = XPR_NULL;
        xpr_ulong_t    sShellAttributes = SFGAO_FILESYSTEM | SFGAO_HASSUBFOLDER | SFGAO_FOLDER;
        DWORD          sFileAttributes  = 0;
        LPTVITEMDATA   sTvItemData;
        TVINSERTSTRUCT sTvInsertStruct  = {0};
        xpr::tstring   sName;

        ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DESKTOP, &sPidl);

        mShcnId = ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, XPR_NULL, SHCNE_ALLEVENTS, XPR_TRUE);

        getItemAttributes(sShellFolder, sPidl, sShellAttributes, sFileAttributes);

        sTvItemData                   = new TVITEMDATA;
        sTvItemData->mShellFolder     = sShellFolder;
        sTvItemData->mPidl            = fxfile::base::Pidl::clone(sPidl);
        sTvItemData->mFullPidl        = fxfile::base::Pidl::concat(sFullPidl, sPidl);
        sTvItemData->mShellAttributes = sShellAttributes;
        sTvItemData->mFileAttributes  = sFileAttributes;
        //sTvItemData->uWatchId       = FileChangeWatcher::InvalidWatchId;
        sTvItemData->mExpandPartial   = XPR_FALSE;
        sShellFolder->AddRef();

        GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);

        sTvInsertStruct.item.mask       = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_HANDLE;
        sTvInsertStruct.item.pszText    = (xpr_tchar_t *)sName.c_str();
        sTvInsertStruct.item.cchTextMax = XPR_MAX_PATH;
        sTvInsertStruct.item.lParam     = (LPARAM)sTvItemData;
        getTreeIcon(sTvItemData->mShellFolder, sTvItemData->mPidl, sTvItemData->mFullPidl, &sTvInsertStruct.item);

        sTvInsertStruct.hInsertAfter    = XPR_NULL;
        sTvInsertStruct.hParent         = XPR_NULL;
        sPrevTreeItem = InsertItem(&sTvInsertStruct);
        COM_FREE(sPidl);

        ::SHGetDesktopFolder(&sRootShellFolder);

        enumerate(sRootShellFolder,
                  sPidl,
                  sPrevTreeItem,
                  XPR_NULL);
    }

    COM_RELEASE(sShellFolder);
    COM_RELEASE(sRootShellFolder);

    // sort
    sTvSortCb.hParent     = sPrevTreeItem;
    sTvSortCb.lParam      = 0;
    sTvSortCb.lpfnCompare = TreeViewCompareProc;
    SortChildrenCB(&sTvSortCb);

    // expand folder
    mSkipExpand = XPR_TRUE;
    Expand(GetRootItem(), TVE_EXPAND);
    mSkipExpand = XPR_FALSE;

    // select specific folder
    xpr_bool_t sSelItem = XPR_FALSE;

    if (XPR_IS_FALSE(sSelItem))
    {
        if (XPR_IS_NOT_NULL(aSelFullPath))
        {
            HTREEITEM sTreeItem = searchTree(aSelFullPath, XPR_FALSE, XPR_TRUE);
            if (XPR_IS_NOT_NULL(sTreeItem))
            {
                Expand(GetRootItem(), TVE_EXPAND);
                SelectItem(sTreeItem);

                if (mOption.mInitNoExpand == XPR_FALSE)
                {
                    Expand(sTreeItem, TVE_EXPAND);
                }

                aSelDefItem = XPR_FALSE;
                sSelItem = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sSelItem))
    {
        if (XPR_IS_TRUE(aSelDefItem))
        {
            HTREEITEM sTreeItem = GetChildItem(GetRootItem());
            if (XPR_IS_NOT_NULL(sTreeItem))
            {
                SelectItem(sTreeItem);

                if (mOption.mInitNoExpand == XPR_FALSE)
                {
                    Expand(sTreeItem, TVE_EXPAND);
                }

                sSelItem = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_TRUE(sSelItem))
    {
        if (XPR_IS_NOT_NULL(aSelItem))
        {
            *aSelItem = sSelItem;
        }
    }

    mInit = XPR_TRUE;

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::enumerate(LPSHELLFOLDER  aShellFolder,
                                 LPITEMIDLIST   aFullPidl,
                                 HTREEITEM      aParentTreeItem,
                                 EnumData      *aEnumData)
{
    xpr_bool_t   sResult         = XPR_FALSE;
    LPITEMIDLIST sEnumPidl       = XPR_NULL;
    xpr_sint_t   sEnumListType   = 0;
    xpr_sint_t   sEnumAttributes = 0;

    sEnumListType = base::ShellEnumerator::ListTypeOnlyFolder;

    if (XPR_IS_TRUE(mOption.mShowHiddenAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeHidden;
    if (XPR_IS_TRUE(mOption.mShowSystemAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeSystem;

    base::ShellEnumerator sShellEnumerator;

    if (sShellEnumerator.enumerate(m_hWnd, aShellFolder, sEnumListType, sEnumAttributes) == XPR_TRUE)
    {
        CWaitCursor sWaitCursor;

        while (sShellEnumerator.next(&sEnumPidl) == XPR_TRUE)
        {
            if (insertPidlItem(aShellFolder, sEnumPidl, aFullPidl, aParentTreeItem, aEnumData) == XPR_FALSE)
            {
                break;
            }
        }

        sResult = XPR_TRUE;
    }
    else
    {
        // failed to enumerate
    }

    return sResult;
}

xpr_bool_t FolderCtrl::insertPidlItem(LPSHELLFOLDER  aShellFolder,
                                      LPITEMIDLIST   aPidl,
                                      LPITEMIDLIST   aFullPidl,
                                      HTREEITEM      aParentTreeItem,
                                      EnumData      *aEnumData)
{
    if (XPR_IS_NOT_NULL(aEnumData))
    {
        xpr_bool_t sDuplicated = aEnumData->isDuplicatedItem(aShellFolder, aPidl);
        if (XPR_IS_TRUE(sDuplicated))
        {
            COM_FREE(aPidl);
            return XPR_TRUE;
        }
    }

    xpr_ulong_t  sShellAttributes = 0;
    DWORD        sFileAttributes  = 0;
    LPTVITEMDATA sTvItemData;

    if (getItemAttributes(aShellFolder, aPidl, sShellAttributes, sFileAttributes) == XPR_FALSE)
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    if (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM))
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    sTvItemData                   = new TVITEMDATA;
    sTvItemData->mShellFolder     = aShellFolder;
    sTvItemData->mPidl            = aPidl;
    sTvItemData->mFullPidl        = fxfile::base::Pidl::concat(aFullPidl, aPidl);
    sTvItemData->mShellAttributes = sShellAttributes;
    sTvItemData->mFileAttributes  = sFileAttributes;
    //sTvItemData->uWatchId       = FileChangeWatcher::InvalidWatchId;
    sTvItemData->mExpandPartial   = XPR_FALSE;
    aShellFolder->AddRef();

    static TVINSERTSTRUCT sTvInsertStruct = {0};
    sTvInsertStruct.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    sTvInsertStruct.item.iImage         = I_IMAGECALLBACK;
    sTvInsertStruct.item.iSelectedImage = I_IMAGECALLBACK;
    sTvInsertStruct.item.cChildren      = I_CHILDRENCALLBACK;
    sTvInsertStruct.item.pszText        = LPSTR_TEXTCALLBACK;
    sTvInsertStruct.item.cchTextMax     = XPR_MAX_PATH;
    sTvInsertStruct.item.lParam         = (LPARAM)sTvItemData;

    sTvInsertStruct.hInsertAfter = TVI_LAST;
    sTvInsertStruct.hParent      = aParentTreeItem;

    InsertItem(&sTvInsertStruct);

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::getItemAttributes(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes, DWORD &aFileAttributes)
{
    // [2008/12/09] bug patched
    // Since SFGAO_HASSUBFOLDER attribute is checked whether exists folder into ZIP file,
    // it is responsed very slow.
    // So, it should check file system attribute, after then it check SFGAO_HASSUBFOLDER attribute.

    if (aShellAttributes == 0)
    {
        aShellAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | //SFGAO_HASSUBFOLDER | SFGAO_CONTENTSMASK | 
            SFGAO_CANRENAME | SFGAO_CANCOPY | SFGAO_CANMOVE | SFGAO_CANDELETE | SFGAO_LINK |
            SFGAO_SHARE | SFGAO_GHOSTED | SFGAO_REMOVABLE | SFGAO_NONENUMERATED;
    }

    aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &aShellAttributes);

    if (!XPR_TEST_BITS(aShellAttributes, SFGAO_FOLDER))
    {
        return XPR_FALSE;
    }

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

    if (!XPR_TEST_BITS(aShellAttributes, SFGAO_FOLDER))
    {
        return XPR_FALSE;
    }

    xpr_ulong_t sHasSubFolderShellAttribute = SFGAO_HASSUBFOLDER;
    if (aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &sHasSubFolderShellAttribute) == S_OK)
    {
        aShellAttributes |= sHasSubFolderShellAttribute;
    }

    return XPR_TRUE;
}

void FolderCtrl::getTreeIcon(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, LPTVITEM aTvItem) const
{
    aTvItem->iImage         = GetItemIconIndex(aFullPidl, XPR_FALSE);
    aTvItem->iSelectedImage = GetItemIconIndex(aFullPidl, XPR_TRUE);
}

xpr_sint_t CALLBACK FolderCtrl::TreeViewCompareProc(LPARAM aLParam1, LPARAM aLParam2, LPARAM aLParamSort)
{
    LPTVITEMDATA sTvItemData1 = (LPTVITEMDATA)aLParam1;
    LPTVITEMDATA sTvItemData2 = (LPTVITEMDATA)aLParam2;
    HRESULT sHResult;

    sHResult = sTvItemData1->mShellFolder->CompareIDs(0, sTvItemData1->mPidl, sTvItemData2->mPidl);

    if (FAILED(sHResult))
        return 0;

    return (xpr_sshort_t)SCODE_CODE(GetScode(sHResult));
}

HTREEITEM FolderCtrl::searchTree(const CString &aSearchName, xpr_bool_t aWithinSelItem, xpr_bool_t aExpand)
{
    xpr_sint_t i;
    xpr_sint_t sLen;
    xpr_sint_t sDivision = 0;
    xpr_sint_t sDivisionNumber = 0;

    sLen = aSearchName.GetLength();
    for (i = 0; i < sLen; ++i)
    {
        if (aSearchName[i] == XPR_STRING_LITERAL('\\'))
            sDivisionNumber++;
    }

    HTREEITEM sRootTreeItem;
    HTREEITEM sChildTreeItem;
    //SetRedraw(XPR_FALSE);
    if (XPR_IS_TRUE(aWithinSelItem))
    {
        sRootTreeItem = GetSelectedItem();
        sChildTreeItem = GetChildItem(sRootTreeItem);
        if (XPR_IS_NULL(sChildTreeItem))
        {
            if (hasChildItem(sRootTreeItem) == XPR_TRUE)
            {
                SetRedraw(XPR_FALSE);

                Expand(sRootTreeItem, TVE_EXPAND);
                Expand(sRootTreeItem, TVE_COLLAPSE);
                sChildTreeItem = GetChildItem(sRootTreeItem);

                SetRedraw();
                UpdateWindow();
            }
            else
            {
                sChildTreeItem = XPR_NULL;
            }
        }
    }
    else
    {
        sChildTreeItem = sRootTreeItem = GetRootItem();

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sRootTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            xpr::tstring sFullPath;
            GetDispFullPath(sTvItemData->mFullPidl, sFullPath);

            const xpr_tchar_t *sSplit = sFullPath.c_str();
            while (XPR_IS_NOT_NULL(sSplit))
            {
                sSplit = _tcschr(sSplit, XPR_STRING_LITERAL('\\'));
                if (XPR_IS_NOT_NULL(sSplit))
                {
                    sSplit++;
                    sDivision++;
                }
            }
        }
    }

    CString sSubPath;
    CString sCurrentPath;
    HTREEITEM sFiniteLoopTreeItem = sChildTreeItem;
    xpr_sint_t sFiniteLoop = 0;
    while (XPR_IS_NOT_NULL(sChildTreeItem))
    {
        if (XPR_IS_NOT_NULL(sFiniteLoopTreeItem) && sFiniteLoopTreeItem == sChildTreeItem)
        {
            sFiniteLoop++;
            if (sFiniteLoop > 5)
            {
                sChildTreeItem = XPR_NULL;
                break;
            }
        }
        else
        {
            sFiniteLoop = 0;
        }

        sFiniteLoopTreeItem = sChildTreeItem;

        sCurrentPath = GetItemText(sChildTreeItem);
        AfxExtractSubString(sSubPath, (const xpr_tchar_t *)aSearchName, sDivision, XPR_STRING_LITERAL('\\'));
        sSubPath.MakeUpper();
        sCurrentPath.MakeUpper();

        if (sSubPath == sCurrentPath)
        {
            if (sDivision == sDivisionNumber)
                break;

            if (GetChildItem(sChildTreeItem) == XPR_NULL)
            {
                if (XPR_IS_TRUE(aExpand) && hasChildItem(sChildTreeItem) == XPR_TRUE)
                {
                    SetRedraw(XPR_FALSE);

                    Expand(sChildTreeItem, TVE_EXPAND);
                    Expand(sChildTreeItem, TVE_COLLAPSE);

                    SetRedraw(XPR_TRUE);
                    UpdateWindow();
                }
                else
                {
                    sChildTreeItem = XPR_NULL;
                    break;
                }
            }

            sChildTreeItem = GetChildItem(sChildTreeItem);
            sDivision++;
            continue;
        }

        sChildTreeItem = GetNextSiblingItem(sChildTreeItem);
        if (sChildTreeItem == XPR_NULL && aWithinSelItem == XPR_TRUE)
        {
            sChildTreeItem = XPR_NULL;
            break;
        }
    }

    //SetRedraw(XPR_TRUE);
    //UpdateWindow();

    return sChildTreeItem;
}

void FolderCtrl::OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW *)aNmHdr;
    *aResult = 0;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sNmTreeView->itemOld.lParam;
    if (XPR_IS_NOT_NULL(sTvItemData))
    {
        //if (sTvItemData->uWatchId != FileChangeWatcher::InvalidWatchId)
        //{
        //    FileChangeWatcher::instance().Unregister(sTvItemData->uWatchId);
        //    sTvItemData->uWatchId = FileChangeWatcher::InvalidWatchId;
        //}

        COM_RELEASE(sTvItemData->mShellFolder);
        COM_FREE(sTvItemData->mPidl);
        COM_FREE(sTvItemData->mFullPidl);
        XPR_SAFE_DELETE(sTvItemData);
    }
}

void FolderCtrl::OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW*)aNmHdr;

    if ((mOption.mDragType == DRAG_START_DEFAULT) ||
        (mOption.mDragType == DRAG_START_CTRL && GetAsyncKeyState(VK_CONTROL) < 0))
    {
        beginDragDrop(sNmTreeView);
    }

    *aResult = 0;
}

void FolderCtrl::OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_TREEVIEW *sNmTreeView = (NM_TREEVIEW*)aNmHdr;

    if ((mOption.mDragType == DRAG_START_DEFAULT) ||
        (mOption.mDragType == DRAG_START_CTRL && GetAsyncKeyState(VK_CONTROL) < 0))
    {
        beginDragDrop(sNmTreeView);
    }

    *aResult = 0;
}

void FolderCtrl::beginDragDrop(NM_TREEVIEW* sNmTreeView)
{
    LPTVITEMDATA sTvItemData = XPR_NULL;
    DROPEFFECT sDropEffect;

    TV_ITEM sTvItem = {0};
    sTvItem.mask  = TVIF_PARAM | TVIF_HANDLE;
    sTvItem.hItem = sNmTreeView->itemNew.hItem;
    if (GetItem(&sTvItem) == XPR_TRUE)
        sTvItemData = (LPTVITEMDATA)sTvItem.lParam;

    if (XPR_IS_NOT_NULL(sTvItemData))
    {
        LPDATAOBJECT sDataObject = XPR_NULL;
        HRESULT sHResult;

        sHResult = sTvItemData->mShellFolder->GetUIObjectOf(
            ::GetParent(m_hWnd),
            1,
            (const struct _ITEMIDLIST **)&sTvItemData->mPidl,
            IID_IDataObject,
            0,
            (LPVOID *)&sDataObject);

        if (SUCCEEDED(sHResult))
        {
            LPDROPSOURCE sDataSource = new DropSource();

            if (XPR_IS_NOT_NULL(sDataSource))
            {
                mDropTreeItem = XPR_NULL;

                DragImage &sDragImage = DragImage::instance();

                if (mOption.mDragNoContents == XPR_FALSE)
                {
                    if (sDragImage.beginDrag(m_hWnd, XPR_NULL, sDataObject) == XPR_FALSE)
                    {
                        CImageList *sMyDragImage = CreateDragImage(sTvItem.hItem);
                        if (XPR_IS_NOT_NULL(sMyDragImage))
                        {
                            sDragImage.beginDrag(m_hWnd, sMyDragImage, sDataObject);
                        }
                    }
                }

                DROPEFFECT sOkDropEffect = DROPEFFECT_LINK;
                if (sTvItemData->mShellAttributes & SFGAO_FILESYSTEM)
                    sOkDropEffect |= DROPEFFECT_COPY | DROPEFFECT_MOVE;

                ::DoDragDrop(sDataObject, sDataSource, sOkDropEffect, &sDropEffect);

                sDragImage.endDrag();
            }

            COM_RELEASE(sDataSource);
        }

        COM_RELEASE(sDataObject);
    }
}

void FolderCtrl::OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult) 
{
    TV_DISPINFO *sTvDispInfo = (TV_DISPINFO*)aNmHdr;
    *aResult = 0;

    TVITEM &sTvItem = sTvDispInfo->item;
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sTvDispInfo->item.lParam;

    if (XPR_IS_NULL(sTvItemData))
        return;

    if (sTvItem.mask & TVIF_TEXT)
    {
        sTvItem.pszText[0] = XPR_STRING_LITERAL('\0');
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sTvItem.pszText);
    }

    if (sTvItem.mask & TVIF_IMAGE || sTvItem.mask & TVIF_SELECTEDIMAGE)
    {
        getTreeIcon(sTvItemData->mShellFolder, sTvItemData->mPidl, sTvItemData->mFullPidl, &sTvItem);

        sTvItem.state = 0;
        sTvItem.stateMask = 0;

        if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_GHOSTED))
        {
            sTvItem.mask      |= TVIF_STATE;
            sTvItem.state     |= TVIS_CUT;
            sTvItem.stateMask |= TVIS_CUT;
        }

        if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_SHARE))
        {
            sTvItem.mask      |= TVIF_STATE;
            sTvItem.state     |= INDEXTOOVERLAYMASK(1);
            sTvItem.stateMask |= LVIS_OVERLAYMASK;
        }

        //if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_LINK))
        //{
        //     sTvItem.mask      |= LVIF_STATE;
        //     sTvItem.state     |= INDEXTOOVERLAYMASK(2);
        //     sTvItem.stateMask |= LVIS_OVERLAYMASK;
        //}

        // shell icon custom overlay
        ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
        sAsyncIcon->mType              = ShellIcon::TypeOverlayIndex;
        sAsyncIcon->mCode              = 0;
        sAsyncIcon->mItem              = (uintptr_t)sTvItem.hItem;
        sAsyncIcon->mSignature         = 0;
        sAsyncIcon->mShellFolder       = sTvItemData->mShellFolder;
        sAsyncIcon->mPidl              = fxfile::base::Pidl::clone(sTvItemData->mPidl);
        sAsyncIcon->mResult.mIconIndex = -1;
        sAsyncIcon->mShellFolder->AddRef();

        if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(sAsyncIcon);
        }
    }

    if (sTvItem.mask & TVIF_CHILDREN)
        sTvItem.cChildren = (sTvItemData->mShellAttributes & SFGAO_HASSUBFOLDER);

    sTvItem.mask |= TVIF_DI_SETITEM;
}

void FolderCtrl::sortItem(HTREEITEM aParentTreeItem)
{
    if (XPR_IS_NULL(aParentTreeItem))
        return;

    TVSORTCB sTvSortCb = {0};
    sTvSortCb.hParent     = aParentTreeItem;
    sTvSortCb.lParam      = 0;
    sTvSortCb.lpfnCompare = TreeViewCompareProc;
    SortChildrenCB(&sTvSortCb);
}

void FolderCtrl::OnSetFocus(CWnd* pOldWnd) 
{
    setUpdate(XPR_FALSE);

    super::OnSetFocus(pOldWnd);

    setUpdate(XPR_TRUE);
}

void FolderCtrl::OnKillFocus(CWnd* pNewWnd) 
{
    super::OnKillFocus(pNewWnd);
}

xpr_sint_t FolderCtrl::getClipFormatCount(void) const
{
    return 4;
}

void FolderCtrl::getClipFormat(CLIPFORMAT *aClipFormat) const
{
    if (aClipFormat == XPR_NULL)
        return;

    ClipFormat &sClipFormat = ClipFormat::instance();

    aClipFormat[0] = sClipFormat.mShellIDList;
    aClipFormat[1] = sClipFormat.mDropEffect;
    aClipFormat[2] = sClipFormat.mFileName;
    aClipFormat[3] = sClipFormat.mFileNameW;
}

xpr_bool_t FolderCtrl::getDataObject(LPDATAOBJECT *aDataObject) const
{
    HTREEITEM sTreeItem = GetSelectedItem();
    if (XPR_IS_NOT_NULL(sTreeItem))
    {
        HRESULT sHResult;
        LPTVITEMDATA sTvItemData = XPR_NULL;
        sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);

        if (XPR_IS_NULL(sTvItemData) || !XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            return XPR_FALSE;

        if (XPR_IS_NOT_NULL(sTvItemData->mShellFolder))
        {
            sHResult = sTvItemData->mShellFolder->GetUIObjectOf(
                ::GetParent(m_hWnd),
                1,
                (const struct _ITEMIDLIST **)&sTvItemData->mPidl,
                IID_IDataObject,
                0,
                (LPVOID *)aDataObject);

            if (SUCCEEDED(sHResult))
                return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

void FolderCtrl::renameDirectly(HTREEITEM aTreeItem)
{
    mMouseEdit = XPR_FALSE;
    EditLabel(aTreeItem);
}

xpr_bool_t FolderCtrl::getSelItemPath(xpr_tchar_t **aPaths, xpr_sint_t &aCount, xpr_ulong_t aShellAttributes) const
{
    HTREEITEM sTreeItem = GetSelectedItem();
    if (XPR_IS_NULL(sTreeItem))
        return XPR_FALSE;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    if (!XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        return XPR_FALSE;

    if (aShellAttributes != 0 && !XPR_ANY_BITS(sTvItemData->mShellAttributes, aShellAttributes))
        return XPR_FALSE;

    xpr_tchar_t *sSource = new xpr_tchar_t[XPR_MAX_PATH + 1];
    *sSource = XPR_STRING_LITERAL('\0');

    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sSource);
    sSource[_tcslen(sSource)+1] = XPR_STRING_LITERAL('\0');

    *aPaths = sSource;
    aCount = 1;

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::isFileSystem(void) const
{
    return hasSelShellAttributes(SFGAO_FILESYSTEM);
}

xpr_bool_t FolderCtrl::hasSelShellAttributes(xpr_ulong_t aShellAttributes) const
{
    TV_ITEM sTvItem = {0};
    sTvItem.mask  = TVIF_PARAM;
    sTvItem.hItem = GetSelectedItem();

    if (GetItem(&sTvItem) == XPR_TRUE)
    {
        xpr_uint_t sShellAttributes = ((LPTVITEMDATA)sTvItem.lParam)->mShellAttributes;
        if (!(sShellAttributes & aShellAttributes) && aShellAttributes != 0)
            return XPR_FALSE;
    }

    return XPR_TRUE;
}

void FolderCtrl::OnKeyDown(xpr_uint_t nChar, xpr_uint_t nRepCnt, xpr_uint_t nFlags) 
{
    if (GetKeyState(VK_CONTROL) < 0 && nChar == VK_ADD)
    {
        expandAll(GetSelectedItem());
        return;
    }
    else if (GetKeyState(VK_CONTROL) < 0 && nChar == VK_SUBTRACT)
    {
        collapseAll(GetSelectedItem());
        return;
    }
    else if (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_LEFT || nChar == VK_RIGHT)
    {
        if (mOption.mSelDelay == XPR_TRUE)
            mSelTimer = XPR_TRUE;
    }

    super::OnKeyDown(nChar, nRepCnt, nFlags);
}

xpr_bool_t FolderCtrl::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_TAB)
        {
            if (XPR_IS_NOT_NULL(mObserver))
                mObserver->onMoveFocus(*this);
            return XPR_TRUE;
        }
        else if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE && pMsg->hwnd == GetEditControl()->GetSafeHwnd())
        {
            ::SendNotifyMessage(GetEditControl()->GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

LRESULT FolderCtrl::OnFileChangeNotify(WPARAM wParam, LPARAM lParam)
{
    FileChangeWatcher::WatchId sWatchId = (FileChangeWatcher::WatchId)wParam;
    HTREEITEM sTreeItem = (HTREEITEM)lParam;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return 0;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sTvItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return 0;

    Shcn sShcn = {0};
    sShcn.mEventId = SHCNE_UPDATEDIR;
    sShcn.mPidl1   = sFullPidl;
    sShcn.mItem1   = (uintptr_t)sFullPidl;
    sShcn.mHwnd    = m_hWnd;

    OnShcnUpdateDir(&sShcn);

    COM_FREE(sFullPidl);

    //HTREEITEM sChildTreeItem = GetChildItem(sTreeItem);

    return 0;
}

LRESULT FolderCtrl::OnShellChangeNotify(WPARAM wParam, LPARAM lParam)
{
    Shcn *sShcn = (Shcn *)wParam;
    xpr_slong_t sEventId = (xpr_slong_t)lParam;

    if (XPR_IS_NULL(sShcn))
        return 0;

#ifdef XPR_CFG_BUILD_DEBUG
    xpr::tstring sMsg(XPR_STRING_LITERAL(""));
#endif

    switch (sEventId)
    {
    case SHCNE_MKDIR:         OnShcnCreateItem(sShcn);             break;
    case SHCNE_RENAMEFOLDER:  OnShcnRenameItem(sShcn);             break;
    case SHCNE_RMDIR:         OnShcnDeleteItem(sShcn);             break;
    case SHCNE_UPDATEDIR:     OnShcnUpdateDir(sShcn);              break;
    case SHCNE_UPDATEITEM:    OnShcnUpdateItem(sShcn);             break;
    case SHCNE_NETSHARE:
    case SHCNE_NETUNSHARE:    OnShcnNetShare(sShcn);               break;
    case SHCNE_MEDIAINSERTED: OnShcnMediaInsRem(sShcn, XPR_TRUE);  break;
    case SHCNE_MEDIAREMOVED:  OnShcnMediaInsRem(sShcn, XPR_FALSE); break;
    case SHCNE_DRIVEADD:      OnShcnDriveAdd(sShcn);               break;
    case SHCNE_DRIVEREMOVED:  OnShcnDriveRemove(sShcn);            break;

        //-- ¹ÌÃ³¸® --
#ifdef XPR_CFG_BUILD_DEBUG
    case SHCNE_FREESPACE:
        break;

    case SHCNE_ASSOCCHANGED: // A file type association has changed.
        sMsg = XPR_STRING_LITERAL("SHCNE_FREESPACE");
        break;

    case SHCNE_UPDATEIMAGE: // An image in the system image list has changed.
        sMsg = XPR_STRING_LITERAL("SHCNE_UPDATEIMAGE");
        break;

    case SHCNE_ALLEVENTS:
        sMsg = XPR_STRING_LITERAL("SHCNE_ALLEVENTS");
        break;
    case SHCNE_ATTRIBUTES:
        sMsg = XPR_STRING_LITERAL("SHCNE_ATTRIBUTES");
        break;
    case SHCNE_DRIVEADDGUI:
        sMsg = XPR_STRING_LITERAL("SHCNE_DRIVEADDGUI");
        break;
    case SHCNE_EXTENDED_EVENT:
        sMsg = XPR_STRING_LITERAL("SHCNE_EXTENDED_EVENT");
        break;
    case SHCNE_SERVERDISCONNECT:
        sMsg = XPR_STRING_LITERAL("SHCNE_SERVERDISCONNECT");
        break;
#endif
    default:
        break;
    }

#ifdef XPR_CFG_BUILD_DEBUG
    //if (sMsg.empty() == XPR_FALSE)
    //    MessageBox(sMsg.c_str());
#endif

    sShcn->Free();
    XPR_SAFE_DELETE(sShcn);

    return 0;
}

xpr_bool_t FolderCtrl::OnShcnCreateItem(Shcn *aShcn)
{
    xpr_tchar_t sFullPath[XPR_MAX_PATH * 2 + 1] = {0};
    GetDispFullPath(aShcn->mPidl1, sFullPath);

    if (searchTree(sFullPath, XPR_FALSE, XPR_FALSE) != XPR_NULL)
        return XPR_FALSE;

    ShNotifyInfo sShNotifyInfo = {0};
    if (beginShcn(aShcn->mEventId, sFullPath, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
    {
        if (GetChildItem(sShNotifyInfo.mParentTreeItem) == XPR_NULL)
        {
            endShcn(sShNotifyInfo.mParentTreeItem);
            return XPR_FALSE;
        }

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        HRESULT sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath);
        if (FAILED(sHResult))
            return XPR_FALSE;

        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(aShcn->mPidl1);

        //LPITEMIDLIST sFullPidl = XPR_NULL;
        //sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
        //if (FAILED(sHResult))
        //    return XPR_FALSE;

        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult))
        {
            LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sShNotifyInfo.mParentTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                LPITEMIDLIST sFullPidl2 = sTvItemData->mFullPidl;
                insertPidlItem(sShellFolder, fxfile::base::Pidl::clone(sPidl), sFullPidl2, sShNotifyInfo.mParentTreeItem, XPR_NULL);

                sortItem(sShNotifyInfo.mParentTreeItem);
                endShcn(sShNotifyInfo.mParentTreeItem);
            }
        }

        COM_FREE(sFullPidl);
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t FolderCtrl::OnShcnRenameItem(Shcn *aShcn)
{
    xpr_bool_t sRename = XPR_FALSE;
    ShNotifyInfo sShNotifyInfo = {0};

    xpr_tchar_t sParsing1[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sParsing2[XPR_MAX_PATH + 1] = {0};
    if (FAILED(GetName(aShcn->mPidl1, SHGDN_FORPARSING, sParsing1)))
        return XPR_FALSE;
    if (FAILED(GetName(aShcn->mPidl2, SHGDN_FORPARSING, sParsing2)))
        return XPR_FALSE;

    xpr_tchar_t sFullPath1[XPR_MAX_PATH * 2 + 1];
    xpr_tchar_t sFullPath2[XPR_MAX_PATH * 2 + 1];
    GetDispFullPath(aShcn->mPidl1, sFullPath1);
    GetDispFullPath(aShcn->mPidl2, sFullPath2);

    xpr_sint_t sSplitLen1 = 0, sSplitLen2 = 0;
    xpr_tchar_t *sSplit1 = _tcsrchr(sFullPath1, XPR_STRING_LITERAL('\\'));
    xpr_tchar_t *sSplit2 = _tcsrchr(sFullPath2, XPR_STRING_LITERAL('\\'));
    if (XPR_IS_NOT_NULL(sSplit1) && XPR_IS_NOT_NULL(sSplit2))
    {
        sSplitLen1 = (xpr_sint_t)(sSplit1 - sFullPath1);
        sSplitLen2 = (xpr_sint_t)(sSplit2 - sFullPath2);
    }

    // ex) D:\test folder\new folder (4) -> D:\test folder 22222\new folder (4)
    // ex) D:\test folder\new folder (5) -> C:\test folder\new folder (5)
    if (sSplitLen1 != sSplitLen2 || _tcsnicmp(sFullPath1, sFullPath2, sSplitLen1) != 0)
    {
        if (beginShcn(SHCNE_RMDIR, sFullPath1, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
        {
            Shcn sShcn = {0};
            sShcn.mPidl1 = aShcn->mPidl1;
            OnShcnDeleteItem(&sShcn);
        }

        if (beginShcn(SHCNE_MKDIR, sFullPath2, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
        {
            Shcn sShcn = {0};
            sShcn.mPidl1 = aShcn->mPidl2;
            OnShcnCreateItem(&sShcn);
        }
    }
    else
    {
        if (_tcslen(sParsing1) == 3) // Drive Rename, C:\, D:\, ... etc
        {
            OnShcnUpdateDrive(sParsing1);
        }
        else
        {
            sRename = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sRename))
        return XPR_TRUE;

    HTREEITEM sTreeItem = searchTree(sFullPath1, XPR_FALSE, XPR_FALSE);
    if (XPR_IS_NULL(sTreeItem))
        return XPR_FALSE;

    HTREEITEM sParentTreeItem = GetParentItem(sTreeItem);
    if (XPR_IS_NULL(sParentTreeItem))
        return XPR_FALSE;

    LPTVITEMDATA sParentTvItemData = (LPTVITEMDATA)GetItemData(sParentTreeItem);
    if (XPR_IS_NULL(sParentTvItemData))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    //SetRedraw(XPR_FALSE);

    //xpr_bool_t sSelItem = (sTreeItem == GetSelectedItem()) ? XPR_TRUE : XPR_FALSE;

    //if (XPR_IS_TRUE(sSelItem))
    //    setUpdate(XPR_FALSE);

    DeleteItem(sTreeItem);

    LPITEMIDLIST sFullPidl;
    LPITEMIDLIST sParsingFullPidl;

    sParsingFullPidl = fxfile::base::Pidl::create(sParsing2);
    if (XPR_IS_NOT_NULL(sParsingFullPidl))
    {
        sFullPidl = fxfile::base::Pidl::concat(sParentTvItemData->mFullPidl, fxfile::base::Pidl::findLastItem(sParsingFullPidl));
        if (XPR_IS_NOT_NULL(sFullPidl))
        {
            xpr_bool_t    sSuccess;
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            LPCITEMIDLIST sPidl        = XPR_NULL;

            sSuccess = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
            if (XPR_IS_TRUE(sSuccess))
            {
                LPITEMIDLIST sParentFullPidl = fxfile::base::Pidl::clone(sFullPidl);
                fxfile::base::Pidl::removeLastItem(sParentFullPidl);

                insertPidlItem(sShellFolder, fxfile::base::Pidl::clone(sPidl), sParentFullPidl, sParentTreeItem, XPR_NULL);

                sortItem(sParentTreeItem);
                endShcn(sParentTreeItem);

                COM_FREE(sParentFullPidl);

                sResult = XPR_TRUE;
            }

            COM_FREE(sFullPidl);
        }

        COM_FREE(sParsingFullPidl);
    }

    //if (XPR_IS_TRUE(sSelItem))
    //{
    //    HTREEITEM sNewTreeItem = searchTree(sFullPath2, XPR_FALSE, XPR_FALSE);
    //    if (XPR_IS_NOT_NULL(sNewTreeItem))
    //        SelectItem(sNewTreeItem);

    //    setUpdate(XPR_TRUE);
    //}

    //SetRedraw();
    //UpdateWindow();

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnUpdateDir(Shcn *aShcn)
{
    xpr_bool_t sResult = XPR_FALSE;
    ShNotifyInfo sShNotifyInfo = {0};
    sShNotifyInfo.mShcn = aShcn;

    xpr_tchar_t sFullPath1[XPR_MAX_PATH * 2 + 1];
    GetDispFullPath(aShcn->mPidl1, sFullPath1);

    if (beginShcn(aShcn->mEventId, sFullPath1, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
    {
        refresh(sShNotifyInfo.mParentTreeItem);

        enumerateShcn(sShNotifyInfo.mParentTreeItem, &sShNotifyInfo);

        endShcn(sShNotifyInfo.mParentTreeItem);
        sResult = XPR_TRUE;
    }

    // update notify message
    if (mObserver != XPR_NULL)
    {
        mObserver->onUpdateStatus(*this);
    }

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnDriveAdd(Shcn *aShcn)
{
    xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1];
    GetDispFullPath(aShcn->mPidl1, sPath);

    if (searchTree(sPath, XPR_FALSE, XPR_FALSE))
        return XPR_FALSE;

    ShNotifyInfo sShNotifyInfo = {0};
    if (beginShcn(aShcn->mEventId, sPath, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
    {
        if (GetChildItem(sShNotifyInfo.mParentTreeItem) == XPR_NULL)
        {
            endShcn(sShNotifyInfo.mParentTreeItem);
            return XPR_FALSE;
        }

        HRESULT sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath);
        if (FAILED(sHResult))
            return XPR_FALSE;

        LPITEMIDLIST sFullPidl = XPR_NULL;
        sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
        if (FAILED(sHResult))
            return XPR_FALSE;

        GetDispFullPath(sFullPidl, sPath);

        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult))
        {
            LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sShNotifyInfo.mParentTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                LPITEMIDLIST sFullPidl2 = sTvItemData->mFullPidl;
                insertPidlItem(sShellFolder, fxfile::base::Pidl::clone(sPidl), sFullPidl2, sShNotifyInfo.mParentTreeItem, XPR_NULL);

                sortItem(sShNotifyInfo.mParentTreeItem);
                endShcn(sShNotifyInfo.mParentTreeItem);
            }
        }

        COM_FREE(sFullPidl);
        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t FolderCtrl::OnShcnDriveRemove(Shcn *aShcn)
{
    xpr::tstring sDrive;
    HRESULT sHResult = GetName(aShcn->mPidl1, SHGDN_FORPARSING, sDrive);
    if (FAILED(sHResult))
        return XPR_FALSE;

    LPITEMIDLIST sFullPidl = XPR_NULL;
    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sFullPidl);
    if (FAILED(sHResult) || XPR_IS_NULL(sFullPidl))
        return XPR_FALSE;

    HTREEITEM sTreeItem = searchTree(GetDispFullPath(sFullPidl), XPR_FALSE, XPR_TRUE);
    COM_FREE(sFullPidl);
    if (XPR_IS_NULL(sTreeItem))
        return XPR_FALSE;

    sTreeItem = GetChildItem(sTreeItem);
    if (XPR_IS_NULL(sTreeItem))
        return XPR_FALSE;

    xpr_tchar_t sSelPath[XPR_MAX_PATH + 1] = {0};
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(GetSelectedItem());
    if (sTvItemData->mShellAttributes & SFGAO_FILESYSTEM)
    {
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sSelPath);
        if (IsFileSystem(sSelPath) == XPR_FALSE)
            sSelPath[0] = XPR_STRING_LITERAL('\0');
    }

    xpr_bool_t sResult = XPR_FALSE;
    xpr_sint_t sColon = 0;
    CString sName;
    while (XPR_IS_NOT_NULL(sTreeItem))
    {
        sName = GetItemText(sTreeItem);
        sColon = sName.ReverseFind(XPR_STRING_LITERAL(':'));
        if (sColon > 0)
        {
            if (sName[--sColon] == sDrive[0])
            {
                if (sSelPath[0] == sDrive[0])
                {
                    HTREEITEM aParentTreeItem = GetParentItem(sTreeItem);
                    if (XPR_IS_NOT_NULL(aParentTreeItem))
                        SelectItem(aParentTreeItem);
                }

                sResult = DeleteItem(sTreeItem);
                break;
            }
        }

        sTreeItem = GetNextSiblingItem(sTreeItem);
    }

    return sResult;
}

xpr_bool_t FolderCtrl::beginShcn(xpr_slong_t aEventId, xpr_tchar_t *aFullPath1, xpr_tchar_t *aFullPath2, ShNotifyInfo *aShNotifyInfo)
{
    // Search Parent Path
    HTREEITEM sParentTreeItem = XPR_NULL;
    if (aEventId == SHCNE_UPDATEDIR)
    {
        //xpr_tchar_t *p = _tcsrchr(aFullPath1, XPR_STRING_LITERAL('\\'));
        //if (p) p[0] = XPR_STRING_LITERAL('\0');
        sParentTreeItem = searchTree(aFullPath1, XPR_FALSE, XPR_FALSE);
    }
    else
    {
        xpr_tchar_t sParent[XPR_MAX_PATH + 1];
        _tcscpy(sParent, aFullPath1);

        xpr_tchar_t *sSplit = _tcsrchr(sParent, XPR_STRING_LITERAL('\\'));
        if (XPR_IS_NOT_NULL(sSplit))
            sSplit[0] = XPR_STRING_LITERAL('\0');

        sParentTreeItem = searchTree(sParent, XPR_FALSE, XPR_FALSE);
    }

    if (XPR_IS_NULL(sParentTreeItem))
        return XPR_FALSE;

    // Set Normal Name
    const xpr_tchar_t *sName1, *sName2;
    sName1 = _tcsrchr(aFullPath1, XPR_STRING_LITERAL('\\')) + 1;
    if (aEventId == SHCNE_RENAMEFOLDER)
        sName2 = _tcsrchr(aFullPath2, XPR_STRING_LITERAL('\\')) + 1;

    if (aEventId == SHCNE_MKDIR || aEventId == SHCNE_RMDIR || aEventId == SHCNE_NETSHARE)
    {
        _tcscpy(aShNotifyInfo->mNewName, sName1);
    }
    else if (aEventId == SHCNE_RENAMEFOLDER)
    {
        _tcscpy(aShNotifyInfo->mOldName, sName1);
        _tcscpy(aShNotifyInfo->mNewName, sName2);
    }

    // Set Etc Information
    aShNotifyInfo->mEventId = aEventId;
    aShNotifyInfo->mParentTreeItem = sParentTreeItem;

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::endShcn(HTREEITEM aTreeItem)
{
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return XPR_FALSE;

    xpr::tstring sName;
    GetName(sTvItemData->mFullPidl, SHGDN_INFOLDER, sName);

    xpr_bool_t sResult = XPR_FALSE;
    xpr_ulong_t sShellAttributes = GetItemAttributes(sTvItemData->mShellFolder, sTvItemData->mPidl);
    if (sTvItemData->mShellAttributes != sShellAttributes)
    {
        sTvItemData->mShellAttributes = sShellAttributes;
        SetItemData(aTreeItem, (DWORD_PTR)sTvItemData);
    }

    TV_ITEM sTvItem = {0};
    sTvItem.mask      = TVIF_HANDLE | TVIF_CHILDREN;
    sTvItem.hItem     = aTreeItem;
    sTvItem.cChildren = I_CHILDRENCALLBACK;
    SetItem(&sTvItem);

    sResult = XPR_TRUE;

    return sResult;
}

void FolderCtrl::enumerateShcn(HTREEITEM aParentTreeItem, ShNotifyInfo *aShNotifyInfo)
{
    if (XPR_IS_NULL(aParentTreeItem))
    {
        return;
    }

    HTREEITEM sTreeItem = GetChildItem(aParentTreeItem);
    if (XPR_IS_NULL(sTreeItem))
    {
        return;
    }

    LPTVITEMDATA sParentTvItemData = (LPTVITEMDATA)GetItemData(aParentTreeItem);
    if (XPR_IS_NULL(sParentTvItemData))
    {
        return;
    }

    HRESULT       sComResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    if (fxfile::base::Pidl::compare(sParentTvItemData->mFullPidl, CSIDL_DESKTOP) == 0)
    {
        sComResult = ::SHGetDesktopFolder(&sShellFolder);
    }
    else
    {
        sComResult = sParentTvItemData->mShellFolder->BindToObject(sParentTvItemData->mPidl, 0, IID_IShellFolder, (LPVOID*)&sShellFolder);
    }

    LPITEMIDLIST sFullPidl = sParentTvItemData->mFullPidl;
    if (FAILED(sComResult))
    {
        return;
    }

    LPITEMIDLIST sEnumPidl       = XPR_NULL;
    xpr_sint_t   sEnumListType   = 0;
    xpr_sint_t   sEnumAttributes = 0;

    sEnumListType = base::ShellEnumerator::ListTypeOnlyFolder;

    if (XPR_IS_TRUE(mOption.mShowHiddenAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeHidden;
    if (XPR_IS_TRUE(mOption.mShowSystemAttribute)) sEnumAttributes |= base::ShellEnumerator::AttributeSystem;

    base::ShellEnumerator sShellEnumerator;

    if (sShellEnumerator.enumerate(m_hWnd, sShellFolder, sEnumListType, sEnumAttributes) == XPR_TRUE)
    {
        CWaitCursor sWaitCursor;

        while (sShellEnumerator.next(&sEnumPidl) == XPR_TRUE)
        {
            if (updateShcnPidlItem(sShellFolder, sEnumPidl, sFullPidl, aParentTreeItem, aShNotifyInfo) == XPR_FALSE)
            {
                break;
            }
        }
    }
    else
    {
        // failed to enumerate
    }

    COM_RELEASE(sShellFolder);
}

xpr_bool_t FolderCtrl::updateShcnPidlItem(LPSHELLFOLDER  aShellFolder,
                                          LPITEMIDLIST   aPidl,
                                          LPITEMIDLIST   aFullPidl,
                                          HTREEITEM      aParentTreeItem,
                                          ShNotifyInfo  *aShNotifyInfo)
{
    xpr_ulong_t  sShellAttributes = 0;
    DWORD        sFileAttributes  = 0;
    LPTVITEMDATA sTvItemData;

    if (getItemAttributes(aShellFolder, aPidl, sShellAttributes, sFileAttributes) == XPR_FALSE)
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    if (mOption.mShowSystemAttribute == XPR_FALSE && mOption.mShowHiddenAttribute == XPR_FALSE && XPR_TEST_BITS(sShellAttributes, SFGAO_GHOSTED))
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    if (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM))
    {
        COM_FREE(aPidl);
        return XPR_TRUE;
    }

    sTvItemData                   = new TVITEMDATA;
    sTvItemData->mShellFolder     = aShellFolder;
    sTvItemData->mPidl            = aPidl;
    sTvItemData->mFullPidl        = fxfile::base::Pidl::concat(aFullPidl, aPidl);
    sTvItemData->mShellAttributes = sShellAttributes;
    sTvItemData->mFileAttributes  = sFileAttributes;
    //sTvItemData->uWatchId       = FileChangeWatcher::InvalidWatchId;
    sTvItemData->mExpandPartial   = XPR_FALSE;
    aShellFolder->AddRef();

    xpr_bool_t sResult = XPR_FALSE;

    if (aShNotifyInfo->mEventId == SHCNE_NETSHARE)
    {
        if (OnShcnEnumNetShare(sTvItemData, aParentTreeItem, aShNotifyInfo) == XPR_TRUE)
        {
            // stop to enumerate
            return XPR_FALSE;
        }
    }
    else if (aShNotifyInfo->mEventId == SHCNE_UPDATEDIR)
    {
        sResult = updateShcnTvItemData(sTvItemData, aParentTreeItem, aShNotifyInfo);
    }

    if (XPR_IS_FALSE(sResult))
    {
        COM_RELEASE(sTvItemData->mShellFolder);
        COM_FREE(sTvItemData->mFullPidl);
        COM_FREE(sTvItemData->mPidl);
        XPR_SAFE_DELETE(sTvItemData);
    }

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::updateShcnTvItemData(LPTVITEMDATA aTvItemData, HTREEITEM aParentTreeItem, ShNotifyInfo *aShNotifyInfo)
{
    xpr_tchar_t sParsing1[XPR_MAX_PATH + 1];
    GetName(aShNotifyInfo->mShcn->mPidl1, SHGDN_FORPARSING, sParsing1);
    GetName(aTvItemData->mShellFolder, aTvItemData->mPidl, SHGDN_INFOLDER, aShNotifyInfo->mNewName);

    if (GetChildItem(aParentTreeItem) == XPR_NULL)
        return XPR_FALSE;

    TVITEM sTvItem = {0};
    LPTVITEMDATA sTvItemData2;
    xpr_sint_t sIconIndex;

    CString sName;
    HTREEITEM sTreeItem = GetChildItem(aParentTreeItem);
    while (XPR_IS_NOT_NULL(sTreeItem))
    {
        sName = GetItemText(sTreeItem);
        if (_tcsicmp(sName, aShNotifyInfo->mNewName) == 0)
        {
            // folder icon change
            sTvItem.mask  = TVIF_IMAGE | TVIF_HANDLE;
            sTvItem.hItem = sTreeItem;
            GetItem(&sTvItem);

            sIconIndex = GetItemIconIndex(aTvItemData->mFullPidl, XPR_FALSE);

            // folder name upper/lower case
            if (_tcscmp(sName, aShNotifyInfo->mNewName) != 0 || sIconIndex != sTvItem.iImage)
            {
                SetItemText(sTreeItem, aShNotifyInfo->mNewName);

                sTvItemData2 = (LPTVITEMDATA)GetItemData(sTreeItem);
                SetItemData(sTreeItem, (DWORD_PTR)aTvItemData);

                getTreeIcon(aTvItemData->mShellFolder, aTvItemData->mPidl, aTvItemData->mFullPidl, &sTvItem);
                SetItem(&sTvItem);

                COM_RELEASE(sTvItemData2->mShellFolder);
                COM_FREE(sTvItemData2->mFullPidl);
                COM_FREE(sTvItemData2->mPidl);
                XPR_SAFE_DELETE(sTvItemData2);

                return XPR_TRUE;
            }

            return XPR_FALSE;
        }

        sTreeItem = GetNextSiblingItem(sTreeItem);
    }

    xpr_bool_t sResult = XPR_FALSE;

    TVINSERTSTRUCT sTvInsertStruct = {0};
    TVSORTCB sTvSortCb = {0};

    sTvInsertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    sTvInsertStruct.item.cchTextMax     = XPR_MAX_PATH;
    sTvInsertStruct.item.pszText        = LPSTR_TEXTCALLBACK;
    sTvInsertStruct.item.iImage         = I_IMAGECALLBACK;
    sTvInsertStruct.item.iSelectedImage = I_IMAGECALLBACK;
    sTvInsertStruct.item.cChildren      = I_CHILDRENCALLBACK;
    sTvInsertStruct.item.lParam         = (LPARAM)aTvItemData;

    sTvInsertStruct.hInsertAfter = TVI_LAST;
    sTvInsertStruct.hParent      = aParentTreeItem;
    if (InsertItem(&sTvInsertStruct) != XPR_NULL)
        sResult = XPR_TRUE;

    sTvSortCb.hParent     = aParentTreeItem;
    sTvSortCb.lParam      = 0;
    sTvSortCb.lpfnCompare = TreeViewCompareProc;
    SortChildrenCB(&sTvSortCb);

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnDeleteItem(Shcn *aShcn)
{
    xpr_tchar_t sFullPath1[XPR_MAX_PATH * 2 + 1];
    GetDispFullPath(aShcn->mPidl1, sFullPath1);

    ShNotifyInfo sShNotifyInfo = {0};
    if (beginShcn(aShcn->mEventId, sFullPath1, XPR_NULL, &sShNotifyInfo) == XPR_FALSE)
        return XPR_FALSE;

    xpr_tchar_t sParsing1[XPR_MAX_PATH + 1];
    GetName(aShcn->mPidl1, SHGDN_FORPARSING, sParsing1);

    xpr_bool_t sResult = XPR_FALSE;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    //SetRedraw(XPR_FALSE);

    LPTVITEMDATA sTvItemData = XPR_NULL;
    HTREEITEM sTreeItem = GetChildItem(sShNotifyInfo.mParentTreeItem);
    if (sShNotifyInfo.mNewName[0] == XPR_STRING_LITERAL('\0'))
    {
        while (XPR_IS_NOT_NULL(sTreeItem))
        {
            sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
                if (IsExistFile(sPath) == XPR_FALSE)
                {
                    HTREEITEM sNextTreeItem = GetNextSiblingItem(sTreeItem);

                    if (GetSelectedItem() == sTreeItem)
                        SelectItem(GetParentItem(sTreeItem));

                    DeleteItem(sTreeItem);

                    sResult = XPR_TRUE;
                    sTreeItem = sNextTreeItem;
                    continue;
                }
            }

            sTreeItem = GetNextSiblingItem(sTreeItem);
        }
    }
    else
    {
        while (XPR_IS_NOT_NULL(sTreeItem))
        {
            if (_tcscmp(GetItemText(sTreeItem), sShNotifyInfo.mNewName) == 0)
            {
                sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
                if (XPR_IS_NOT_NULL(sTvItemData))
                {
                    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
                    if (_tcsicmp(sPath, sParsing1) == 0)
                    {
                        if (GetSelectedItem() == sTreeItem)
                            SelectItem(GetParentItem(sTreeItem));

                        DeleteItem(sTreeItem);

                        sResult = XPR_TRUE;
                        break;
                    }
                }
            }

            sTreeItem = GetNextSiblingItem(sTreeItem);
        }
    }

    endShcn(sShNotifyInfo.mParentTreeItem);

    //SetRedraw();
    //UpdateWindow();

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnMediaInsRem(Shcn *aShcn, xpr_bool_t aInserted)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;
    HRESULT sHResult = ::SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &sFullPidl);
    if (FAILED(sHResult))
        return XPR_FALSE;

    CString sFullPath;
    sFullPath = GetDispFullPath(sFullPidl);

    HTREEITEM sParentTreeItem = searchTree(sFullPath, XPR_FALSE, XPR_FALSE);
    if (XPR_IS_NULL(sParentTreeItem))
        return XPR_TRUE;

    CString sName;
    //xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    LPTVITEMDATA sTvItemData = XPR_NULL;
    xpr_sint_t sImageNew, sSelImageNew;
    HTREEITEM sTreeItem;

    sTreeItem = GetChildItem(sParentTreeItem);
    while (XPR_IS_NOT_NULL(sTreeItem))
    {
        sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            //GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
            //if (IsExistFile(sPath) == XPR_FALSE)
            //{
            //    DeleteItem(sTreeItem);
            //    sTreeItem = GetChildItem(sParentTreeItem);
            //    continue;
            //}

            // nImage Update
            sImageNew = GetItemIconIndex(sTvItemData->mFullPidl, XPR_FALSE);
            sSelImageNew = GetItemIconIndex(sTvItemData->mFullPidl, XPR_TRUE);
            SetItemImage(sTreeItem, sImageNew, sSelImageNew);

            // pszText Update
            sName.Empty();
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);
            if (XPR_IS_FALSE(aInserted))
            {
                CString sOldName = GetItemText(sTreeItem);
                if (sOldName != sName)
                {
                    collapseAll(sTreeItem);

                    HTREEITEM sSelTreeItem = GetSelectedItem();
                    if (sSelTreeItem == sTreeItem)
                        SelectItem(GetParentItem(sSelTreeItem));
                }
            }

            SetItemText(sTreeItem, sName);
        }

        sTreeItem = GetNextSiblingItem(sTreeItem);
    }

    return XPR_TRUE;
}

xpr_bool_t FolderCtrl::OnShcnUpdateDrive(const xpr_tchar_t *aDrive)
{
    xpr_bool_t sResult = XPR_FALSE;

    LPITEMIDLIST sPidl = XPR_NULL;
    HTREEITEM sTreeItem = XPR_NULL;
    CString sPath;
    HRESULT sHResult = ::SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &sPidl);
    if (SUCCEEDED(sHResult))
    {
        sPath = GetDispFullPath(sPidl);
        sTreeItem = searchTree(sPath, XPR_FALSE, XPR_FALSE);
    }
    COM_FREE(sPidl);

    if (XPR_IS_NULL(sTreeItem))
        return sResult;

    LPTVITEMDATA sTvItemData = XPR_NULL;
    HTREEITEM sChildTreeItem = GetChildItem(sTreeItem);
    CString sTemp = GetItemText(sTreeItem);
    if (XPR_IS_NULL(sChildTreeItem))
        return sResult;

    while (XPR_IS_NOT_NULL(sChildTreeItem))
    {
        sTvItemData = (LPTVITEMDATA)GetItemData(sChildTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sPath);
            CString sText = GetItemText(sChildTreeItem);
            if (sText != sPath)
            {
                SetItemText(sChildTreeItem, sPath);
                sResult = XPR_TRUE;
                break;
            }
        }

        sChildTreeItem = GetNextSiblingItem(sChildTreeItem);
    }

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnUpdateItem(Shcn *aShcn)
{
    xpr_bool_t sResult = XPR_FALSE;

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    HTREEITEM sTreeItem = GetChildItem(GetRootItem());
    xpr_uint_t sDepth = 1;

    HRESULT sHResult;
    LPTVITEMDATA sTvItemData = XPR_NULL;
    while (XPR_IS_NOT_NULL(sTreeItem))
    {
        sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            sHResult = GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sName);
            if (SUCCEEDED(sHResult) && GetItemText(sTreeItem) != sName)
                SetItemText(sTreeItem, sName);
        }

        if (GetChildItem(sTreeItem) != XPR_NULL && sDepth < 3)
        {
            sTreeItem = GetChildItem(sTreeItem);
            sDepth++;
        }
        else
        {
            if (GetNextSiblingItem(sTreeItem) != XPR_NULL)
                sTreeItem = GetNextSiblingItem(sTreeItem);
            else
            {
                if (GetParentItem(sTreeItem) == GetRootItem())
                    break;
                else
                {
                    sTreeItem = GetNextSiblingItem(GetParentItem(sTreeItem));
                    sDepth--;
                }
            }
        }
    }

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnNetShare(Shcn *aShcn)
{
    xpr_bool_t sResult = XPR_FALSE;
    xpr_tchar_t sFullPath1[XPR_MAX_PATH * 2 + 1];
    GetDispFullPath(aShcn->mPidl1, sFullPath1);

    ShNotifyInfo sShNotifyInfo = {0};
    if (beginShcn(aShcn->mEventId, sFullPath1, XPR_NULL, &sShNotifyInfo) == XPR_TRUE)
    {
        enumerateShcn(sShNotifyInfo.mParentTreeItem, &sShNotifyInfo);
        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FolderCtrl::OnShcnEnumNetShare(LPTVITEMDATA aTvItemData, HTREEITEM aTreeItem, ShNotifyInfo *aShNotifyInfo)
{
    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    GetName(aTvItemData->mShellFolder, aTvItemData->mPidl, SHGDN_INFOLDER, sName);
    if (_tcscmp(sName, aShNotifyInfo->mNewName) != 0)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    aTreeItem = GetChildItem(aTreeItem);
    while (XPR_IS_NOT_NULL(aTreeItem))
    {
        if (XPR_IS_NOT_NULL(aTreeItem) && GetItemText(aTreeItem).Compare(aShNotifyInfo->mNewName) == 0)
            break;

        aTreeItem = GetNextSiblingItem(aTreeItem);
    }

    if (XPR_IS_NOT_NULL(aTreeItem))
    {
        LPTVITEMDATA sTempTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
        if (XPR_IS_NOT_NULL(sTempTvItemData) && aTvItemData->mShellAttributes != sTempTvItemData->mShellAttributes)
        {
            TVITEM sTvItem = {0};
            sTvItem.mask      = TVIF_HANDLE | TVIF_PARAM | TVIF_STATE;
            sTvItem.stateMask = TVIS_OVERLAYMASK;
            sTvItem.hItem     = aTreeItem;
            if (GetItem(&sTvItem) == XPR_TRUE)
            {
                sTvItem.state = 0;

                if (XPR_TEST_BITS(aTvItemData->mShellAttributes, SFGAO_SHARE))
                    sTvItem.state = INDEXTOOVERLAYMASK(1);

                // shell icon custom overlay
                ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
                sAsyncIcon->mType              = ShellIcon::TypeOverlayIndex;
                sAsyncIcon->mCode              = 0;
                sAsyncIcon->mItem              = (uintptr_t)aTreeItem;
                sAsyncIcon->mSignature         = 0;
                sAsyncIcon->mShellFolder       = aTvItemData->mShellFolder;
                sAsyncIcon->mPidl              = fxfile::base::Pidl::clone(aTvItemData->mPidl);
                sAsyncIcon->mResult.mIconIndex = -1;
                sAsyncIcon->mShellFolder->AddRef();

                if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
                {
                    XPR_SAFE_DELETE(sAsyncIcon);
                }

                sTvItem.lParam = (LPARAM)aTvItemData;
                SetItem(&sTvItem);

                sResult = XPR_TRUE;
            }
        }
    }

    return sResult;
}

void FolderCtrl::setHiddenSystem(xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    SetRedraw(XPR_FALSE);

    HTREEITEM sTreeItem = GetChildItem(GetRootItem());

    xpr_bool_t sShow = XPR_FALSE;
    xpr_bool_t sHide = XPR_FALSE;

    if (XPR_IS_TRUE(aModifiedHidden))
    {
        if (XPR_IS_TRUE(mOption.mShowHiddenAttribute)) sShow = XPR_TRUE;
        else                                           sHide = XPR_TRUE;
    }

    if (XPR_IS_TRUE(aModifiedSystem))
    {
        if (XPR_IS_TRUE(mOption.mShowSystemAttribute)) sShow = XPR_TRUE;
        else                                           sHide = XPR_TRUE;
    }

    if (XPR_IS_TRUE(sHide) && XPR_IS_TRUE(mOption.mShowSystemAttribute))
        sHide = XPR_FALSE;

    if (XPR_IS_TRUE(sShow))
        showHiddenSystem(sTreeItem, aModifiedHidden, aModifiedSystem);

    if (XPR_IS_TRUE(sHide))
        hideHiddenSystem(sTreeItem, aModifiedHidden, aModifiedSystem);

    SetRedraw(XPR_TRUE);
    UpdateWindow();
}

void FolderCtrl::showHiddenSystem(HTREEITEM aTreeItem, xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    if (XPR_IS_NULL(aTreeItem))
        return;

    {
        ShNotifyInfo sShNotifyInfo = {0};
        sShNotifyInfo.mEventId = SHCNE_UPDATEDIR;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);
            if ((sPath[1] == XPR_STRING_LITERAL(':') && sPath[2] == XPR_STRING_LITERAL('\\') && _tcslen(sPath) == 3) && IsVirtualItem(sTvItemData->mShellFolder, sTvItemData->mPidl) == XPR_FALSE)
            {
                xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
                _stprintf(sDrive, XPR_STRING_LITERAL("%c:"), sPath[0]);

                if (GetDriveType(sDrive) == DRIVE_FIXED)
                {
                    Shcn sShcn = {0};
                    sShcn.mPidl1 = sTvItemData->mFullPidl;
                    sShNotifyInfo.mShcn = &sShcn;
                    enumerateShcn(aTreeItem, &sShNotifyInfo);
                }
            }
            else
            {
                Shcn sShcn = {0};
                sShcn.mPidl1 = sTvItemData->mFullPidl;
                sShNotifyInfo.mShcn = &sShcn;
                enumerateShcn(aTreeItem, &sShNotifyInfo);
            }
        }
    }

    if (GetChildItem(aTreeItem) != XPR_NULL)
        showHiddenSystem(GetChildItem(aTreeItem), aModifiedHidden, aModifiedSystem);

    if (GetNextSiblingItem(aTreeItem) != XPR_NULL)
        showHiddenSystem(GetNextSiblingItem(aTreeItem), aModifiedHidden, aModifiedSystem);
}

void FolderCtrl::hideHiddenSystem(HTREEITEM aTreeItem, xpr_bool_t aModifiedHidden, xpr_bool_t aModifiedSystem)
{
    if (XPR_IS_NULL(aTreeItem))
        return;

    {
        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            if ((mOption.mShowSystemAttribute == XPR_FALSE && mOption.mShowHiddenAttribute == XPR_FALSE && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_GHOSTED)) ||
                (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sTvItemData->mFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sTvItemData->mFileAttributes, FILE_ATTRIBUTE_SYSTEM)))
            {
                HTREEITEM sParentTreeItem = GetParentItem(aTreeItem);
                DeleteItem(aTreeItem);
                aTreeItem = sParentTreeItem;
            }
        }
    }

    if (GetChildItem(aTreeItem) != XPR_NULL)
        hideHiddenSystem(GetChildItem(aTreeItem), aModifiedHidden, aModifiedSystem);

    if (GetNextSiblingItem(aTreeItem) != XPR_NULL)
        hideHiddenSystem(GetNextSiblingItem(aTreeItem), aModifiedHidden, aModifiedSystem);
}

void FolderCtrl::OnShcnUpdateImage(HTREEITEM aTreeItem, xpr_sint_t aImageIndex)
{
    if (XPR_IS_NOT_NULL(aTreeItem))
    {
        TVITEM sTvItem;
        sTvItem.mask  = TVIF_IMAGE;
        sTvItem.hItem = aTreeItem;
        if (GetItem(&sTvItem) == XPR_TRUE && sTvItem.iImage == aImageIndex)
        {
            sTvItem.iImage = 0;
            SetItem(&sTvItem);

            sTvItem.iImage = aImageIndex;
            SetItem(&sTvItem);

            //aTreeItem = GetParentItem(aTreeItem);
            //Expand(aTreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
            //Expand(aTreeItem, TVE_EXPAND);
            //SHChangeNotify_UpdateImage(GetNextSiblingItem(aTreeItem), aImageIndex);
        }

        if (GetChildItem(aTreeItem) != XPR_NULL)
            OnShcnUpdateImage(GetChildItem(aTreeItem), aImageIndex);

        if (GetNextSiblingItem(aTreeItem) != XPR_NULL)
            OnShcnUpdateImage(GetNextSiblingItem(aTreeItem), aImageIndex);
    }
}

void FolderCtrl::OnBeginlabeledit(NMHDR *aNmHdr, LRESULT *aResult) 
{
    if (XPR_IS_FALSE(mOption.mRenameByMouse) && XPR_IS_TRUE(mMouseEdit))
    {
        *aResult = 1;
        mMouseEdit = XPR_TRUE;
        return;
    }

    mMouseEdit = XPR_TRUE;

    *aResult = 1;

    TV_DISPINFO *sTvDispInfo = (TV_DISPINFO*)aNmHdr;
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sTvDispInfo->item.lParam;
    if (XPR_IS_NULL(sTvItemData))
        return;

    if (!XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_CANRENAME))
        return;

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sName);
    if (_tcslen(sName) > 3 || sName[2] == XPR_STRING_LITERAL(':'))
    {
        CEdit *sEdit = GetEditControl();
        if (XPR_IS_NOT_NULL(sEdit))
        {
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER | SHGDN_FOREDITING, sName);
            sEdit->SetWindowText(sName);
        }

        *aResult = 0;
    }
}

void FolderCtrl::OnEndlabeledit(NMHDR *aNmHdr, LRESULT *aResult) 
{
    TV_DISPINFO *sTvDispInfo = (TV_DISPINFO*)aNmHdr;
    *aResult = 0;

    if (sTvDispInfo->item.pszText == XPR_NULL)
        return;

    HTREEITEM sTreeItem = sTvDispInfo->item.hItem;
    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)sTvDispInfo->item.lParam;

    xpr_tchar_t sOldName[XPR_MAX_PATH + 1] = {0};
    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_INFOLDER, sOldName);

    if (_tcscmp(sOldName, sTvDispInfo->item.pszText) == 0)
        return;

    LPITEMIDLIST sOldPidl = sTvItemData->mPidl;
    LPITEMIDLIST sNewPidl = XPR_NULL;
    LPITEMIDLIST sOldFullPidl = sTvItemData->mFullPidl;

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sOldPath);

    xpr_wchar_t sNewNameW[XPR_MAX_PATH + 1] = {0};
    xpr_size_t sInputBytes = _tcslen(sTvDispInfo->item.pszText) * sizeof(xpr_tchar_t);
    xpr_size_t sOutputBytes = XPR_MAX_PATH * sizeof(xpr_wchar_t);
    XPR_TCS_TO_UTF16(sTvDispInfo->item.pszText, &sInputBytes, sNewNameW, &sOutputBytes);
    sNewNameW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

    HRESULT sHResult = sTvItemData->mShellFolder->SetNameOf(m_hWnd, sTvItemData->mPidl, sNewNameW, SHGDN_INFOLDER, &sNewPidl);
    if (FAILED(sHResult))
    {
        EditLabel(sTvDispInfo->item.hItem);
        return;
    }

    if (XPR_IS_NOT_NULL(sNewPidl))
    {
        sTvItemData->mPidl     = fxfile::base::Pidl::clone(sNewPidl);
        sTvItemData->mFullPidl = fxfile::base::Pidl::getFullPidl(sTvItemData->mShellFolder, sNewPidl);

        SetItemData(sTreeItem, (DWORD_PTR)sTvItemData);
        SetItemText(sTreeItem, sTvDispInfo->item.pszText);

        sortItem(GetParentItem(sTvDispInfo->item.hItem));

        if (GetChildItem(sTreeItem) != XPR_NULL)
        {
            SetRedraw(XPR_FALSE);

            Expand(sTreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET);

            SetRedraw(XPR_TRUE);
            UpdateWindow();
        }

        if (GetSelectedItem() == sTreeItem)
        {
            explore(sTreeItem);
        }

        xpr_bool_t sFileSystem = XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM);

        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        GetName(sTvItemData->mShellFolder, sNewPidl, SHGDN_FORPARSING, sNewPath);

        ::SHChangeNotify(SHCNE_RENAMEFOLDER, SHCNF_IDLIST | SHCNF_FLUSH, sOldFullPidl, sTvItemData->mFullPidl);

        if (XPR_IS_TRUE(sFileSystem))
        {
            FileOpUndo sFileOpUndo;
            sFileOpUndo.addRename(sOldPath, sNewPath);
        }
    }

    COM_FREE(sOldPidl);
    COM_FREE(sOldFullPidl);
    COM_FREE(sNewPidl);
}

void FolderCtrl::OnSysColorChange(void)
{
    super::OnSysColorChange();

    COLORREF sColor = ::GetSysColor(COLOR_WINDOW);
    mSmallImgList->SetBkColor(sColor);
}

DROPEFFECT FolderCtrl::OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
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
            IDataObject* pIDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
            mDropTarget.getDropHelper()->DragEnter(GetSafeHwnd(), pIDataObject, &aPoint, sDropEffect);
        }
    }

    mDragExpandTreeItem = XPR_NULL;
    SelectDropTarget(XPR_NULL);

    return sDropEffect;
}

void FolderCtrl::OnDragLeave(void)
{
    xpr_sint_t sTimerId;
    for (sTimerId = TM_ID_DRAG_SCROLL_BEGIN; sTimerId < TM_ID_DRAG_SCROLL_END; ++sTimerId)
        KillTimer(sTimerId);

    mScrollUpTimer = XPR_FALSE;
    mScrollDownTimer = XPR_FALSE;
    mScrollLeftTimer = XPR_FALSE;
    mScrollRightTimer = XPR_FALSE;

    mDropTreeItem = XPR_NULL;
    mDragExpandTreeItem = XPR_NULL;

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

    SelectDropTarget(XPR_NULL);
}

DROPEFFECT FolderCtrl::OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
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

    HTREEITEM sTreeItem = HitTest(aPoint, XPR_NULL);

    if (aKeyState & MK_ALT)          sDropEffect = DROPEFFECT_LINK;
    else if (aKeyState & MK_CONTROL) sDropEffect = DROPEFFECT_COPY;
    else if (aKeyState & MK_SHIFT)   sDropEffect = DROPEFFECT_MOVE;
    else
    {
        xpr_bool_t sCopy = XPR_TRUE;
        if (aOleDataObject->IsDataAvailable(sClipFormat.mShellIDList) == XPR_TRUE)
        {
            STGMEDIUM sStgMedium = {0};
            FORMATETC sFormatEtc = {sClipFormat.mShellIDList, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

            if (aOleDataObject->GetData(sClipFormat.mShellIDList, &sStgMedium, &sFormatEtc) == XPR_TRUE)
            {
                LPIDA sIda = (LPIDA)::GlobalLock(sStgMedium.hGlobal);
                LPITEMIDLIST sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

                xpr_tchar_t sPath[XPR_MAX_PATH + 1];
                GetName(sPidl1, SHGDN_FORPARSING, sPath);

                xpr_tchar_t sHitPath[XPR_MAX_PATH + 1];
                LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
                if (XPR_IS_NOT_NULL(sTvItemData))
                {
                    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sHitPath);
                    if (sPath[0] == sHitPath[0])
                        sCopy = XPR_FALSE;
                }

                ::GlobalUnlock(sStgMedium.hGlobal);
                //::GlobalFree(sStgMedium.hGlobal);
                ::ReleaseStgMedium(&sStgMedium);
            }
        }

        //sDropEffect = sCopy ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
        if (mOption.mDragDefaultFileOp == DRAG_FILE_OP_DEFAULT)
        {
            sDropEffect = XPR_IS_TRUE(sCopy) ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
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

    // If mouse stay this folder for 1 second, that folder expand.
    if (XPR_IS_NOT_NULL(sTreeItem) && sTreeItem != mDragExpandTreeItem)
    {
        mDragExpandTreeItem = sTreeItem;

        KillTimer(TM_ID_DRAG_EXPAND_ITEM);
        SetTimer(TM_ID_DRAG_EXPAND_ITEM, mOption.mDragFolderTreeExpandTime, XPR_NULL);
    }

    if (XPR_IS_NOT_NULL(sTreeItem))
    {
        if (mDropTreeItem != sTreeItem)
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

            SelectDropTarget(sTreeItem);

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

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NOT_NULL(sTvItemData) && !XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        HRESULT sHResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPITEMIDLIST sPidl = XPR_NULL;

        if (XPR_IS_NOT_NULL(sTvItemData))
        {
            sShellFolder = sTvItemData->mShellFolder;
            sPidl  = sTvItemData->mPidl;
        }

        sHResult = E_FAIL;
        LPDROPTARGET sDropTarget = XPR_NULL;

        if (XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
        {
            sHResult = sShellFolder->GetUIObjectOf(
                GetSafeHwnd(),
                1,
                (LPCITEMIDLIST *)&sPidl,
                IID_IDropTarget,
                XPR_NULL,
                (LPVOID *)&sDropTarget);
        }

        xpr_bool_t sSucceeded = XPR_FALSE;

        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sDropTarget))
        {
            POINTL sPointL = {0};
            ClientToScreen(&aPoint);
            sPointL.x = aPoint.x;
            sPointL.y = aPoint.y;

            DWORD sTempDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

            sHResult = sDropTarget->DragEnter(aOleDataObject->m_lpDataObject, aKeyState, sPointL, &sTempDropEffect);
            if (SUCCEEDED(sHResult))
            {
                sHResult = sDropTarget->DragOver(aKeyState, sPointL, &sTempDropEffect);
                sSucceeded = SUCCEEDED(sHResult) && sTempDropEffect != 0;
            }
        }

        COM_RELEASE(sDropTarget);

        if (XPR_IS_TRUE(sSucceeded))
            sDropEffect = DROPEFFECT_MOVE;
        else
        {
            sTreeItem = XPR_NULL;
            sDropEffect = DROPEFFECT_NONE;
        }
    }

    mDropTreeItem = sTreeItem;

    if (XPR_IS_NULL(sTreeItem))
        sDropEffect = DROPEFFECT_NONE;

    return sDropEffect;
}

void FolderCtrl::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
    CRect sRect;
    GetClientRect(&sRect);

    xpr_sint_t sVertCurPos, sVertMinPos, sVertMaxPos;
    GetScrollRange(SB_VERT, &sVertMinPos, &sVertMaxPos);
    sVertCurPos = GetScrollPos(SB_VERT);

    xpr_sint_t sHorzCurPos, sHorzMinPos, sHorzMaxPos;
    GetScrollRange(SB_HORZ, &sHorzMinPos, &sHorzMaxPos);
    sHorzCurPos = GetScrollPos(SB_HORZ);

    if (sVertCurPos != sVertMinPos && sRect.top+20 > aPoint.y)
    {
        if (XPR_IS_FALSE(mScrollUpTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_UP, mOption.mDragScrollTime, XPR_NULL);
            mScrollUpTimer = XPR_TRUE;
        }
    }
    else
    {
        mScrollUpTimer = XPR_FALSE;
    }

    if (sVertCurPos != sVertMaxPos && sRect.bottom-20 < aPoint.y)
    {
        if (XPR_IS_FALSE(mScrollDownTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_DOWN, mOption.mDragScrollTime, XPR_NULL);
            mScrollDownTimer = XPR_TRUE;
        }
    }
    else
    {
        mScrollDownTimer = XPR_FALSE;
    }

    if (sHorzCurPos != sHorzMinPos && sRect.left+20 > aPoint.x)
    {
        if (XPR_IS_FALSE(mScrollLeftTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_LEFT, mOption.mDragScrollTime, XPR_NULL);
            mScrollLeftTimer = XPR_TRUE;
        }
    }
    else
    {
        mScrollLeftTimer = XPR_FALSE;
    }

    if (sHorzCurPos != sHorzMaxPos && sRect.right-20 < aPoint.x)
    {
        if (XPR_IS_FALSE(mScrollRightTimer))
        {
            SetTimer(TM_ID_DRAG_SCROLL_RIGHT, mOption.mDragScrollTime, XPR_NULL);
            mScrollRightTimer = XPR_TRUE;
        }
    }
    else
    {
        mScrollRightTimer = XPR_FALSE;
    }
}

void FolderCtrl::OnDrop(COleDataObject *pOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
            sMyDragImage->EndDrag();
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

    mDropTreeItem       = XPR_NULL;
    mDragExpandTreeItem = XPR_NULL;

    SelectDropTarget(XPR_NULL);

    CWaitCursor sWaitCursor;

    HTREEITEM sTreeItem = HitTest(aPoint, XPR_NULL);
    if (XPR_IS_NULL(sTreeItem))
        return;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        return;

    // Get Target Directory
    xpr_tchar_t sTargetDir[XPR_MAX_PATH + 1] = {0};
    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
    {
        GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sTargetDir);
    }

    // Drop Implementation
    // 1. Internet Url
    // 2. Outlook File
    // 3. General Folder, File
    // 4. Right Drop

    ClipFormat &sClipFormat = ClipFormat::instance();

    xpr_bool_t sFolderFileSystem = IsFileSystemFolder(sTargetDir);
    LPDATAOBJECT sDataObject = pOleDataObject->GetIDataObject(XPR_FALSE);

    if (XPR_IS_TRUE(sFolderFileSystem))
    {
        if (mObserver != XPR_NULL)
        {
            xpr_sint_t sResult = mObserver->onDrop(*this, pOleDataObject, sTargetDir);
            if (sResult != 0)
                return;
        }
    }

    xpr_bool_t sExternalCopyFileOp = (aDropEffect == DROPEFFECT_COPY) && XPR_IS_TRUE(mOption.mExternalCopyFileOp);
    xpr_bool_t sExternalMoveFileOp = (aDropEffect == DROPEFFECT_MOVE) && XPR_IS_TRUE(mOption.mExternalMoveFileOp);

    if (mDropTarget.isRightDrag() == XPR_FALSE && _tcslen(sTargetDir) > 0 && XPR_IS_FALSE(sExternalCopyFileOp) && XPR_IS_FALSE(sExternalMoveFileOp))///* && !aOleDataObject->IsDataAvailable(CF_HDROP)*/)
    {
        const xpr_tchar_t *sLinkSuffix = theApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

        sTargetDir[_tcslen(sTargetDir) + 1] = XPR_STRING_LITERAL('\0');
        DoPaste(GetSafeHwnd(), pOleDataObject, XPR_TRUE, sClipFormat.mShellIDList, aDropEffect, sTargetDir, sLinkSuffix);
    }
    else
    {
        LPSHELLFOLDER sShellFolder = sTvItemData->mShellFolder;
        LPITEMIDLIST sPidl = sTvItemData->mPidl;

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
        HRESULT sHResult = sShellFolder->GetUIObjectOf(
            GetSafeHwnd(),
            1,
            (LPCITEMIDLIST*)&sPidl,
            IID_IDropTarget,
            XPR_NULL,
            (LPVOID*)&sDropTarget);

        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sDropTarget))
        {
            LPDATAOBJECT sDataObject = pOleDataObject->Detach();
            if (XPR_IS_NOT_NULL(sDataObject))
            {
                sDropTarget->DragEnter(sDataObject, sKeyState, sPointL, &sTempDropEffect);
                sDropTarget->DragOver(sKeyState, sPointL, &sTempDropEffect);

                if (mDropTarget.isRightDrag() == XPR_TRUE)
                    aDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

                sDropTarget->Drop(sDataObject, sKeyState, sPointL, &aDropEffect);
            }
        }

        COM_RELEASE(sDropTarget);
    }
}

void FolderCtrl::OnTimer(xpr_uint_t nIDEvent) 
{
    if (XPR_IS_RANGE(TM_ID_DRAG_SCROLL_BEGIN, nIDEvent, TM_ID_DRAG_SCROLL_END))
    {
        KillTimer(nIDEvent);

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
        xpr_bool_t sHorzScroll = XPR_FALSE;
        xpr_bool_t sScrollTimer = XPR_FALSE;

        switch (nIDEvent)
        {
        case TM_ID_DRAG_SCROLL_UP:    sSbCode = SB_LINEUP;    sHorzScroll = XPR_FALSE; if (XPR_IS_TRUE(mScrollUpTimer   )) sScrollTimer = XPR_TRUE; break;
        case TM_ID_DRAG_SCROLL_DOWN:  sSbCode = SB_LINEDOWN;  sHorzScroll = XPR_FALSE; if (XPR_IS_TRUE(mScrollDownTimer )) sScrollTimer = XPR_TRUE; break;
        case TM_ID_DRAG_SCROLL_LEFT:  sSbCode = SB_LINELEFT;  sHorzScroll = XPR_TRUE;  if (XPR_IS_TRUE(mScrollLeftTimer )) sScrollTimer = XPR_TRUE; break;
        case TM_ID_DRAG_SCROLL_RIGHT: sSbCode = SB_LINERIGHT; sHorzScroll = XPR_TRUE;  if (XPR_IS_TRUE(mScrollRightTimer)) sScrollTimer = XPR_TRUE; break;
        }

        if (sSbCode != -1 && XPR_IS_TRUE(sScrollTimer))
            ::SendMessage(GetSafeHwnd(), sHorzScroll ? WM_HSCROLL : WM_VSCROLL, (WPARAM)sSbCode, (LPARAM)0L);

        if (sDragImage.isMyDragging() == XPR_TRUE)
        {
            sDragImage.showMyDragNoLock(XPR_TRUE);
        }
        else
        {
            if (mDropTarget.isUseDropHelper() == XPR_TRUE)
                mDropTarget.getDropHelper()->Show(XPR_TRUE);
        }

        switch (nIDEvent)
        {
        case TM_ID_DRAG_SCROLL_UP:    mScrollUpTimer    = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_DOWN:  mScrollDownTimer  = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_LEFT:  mScrollLeftTimer  = XPR_FALSE; break;
        case TM_ID_DRAG_SCROLL_RIGHT: mScrollRightTimer = XPR_FALSE; break;
        }
    }
    else if (nIDEvent == TM_ID_DRAG_EXPAND_ITEM)
    {
        KillTimer(nIDEvent);

        Expand(mDragExpandTreeItem, TVE_EXPAND);
    }
    else if (nIDEvent == TM_ID_SEL_ITEM)
    {
        KillTimer(nIDEvent);

        HTREEITEM sSelTreeItem = mTimerSelTreeItem;
        mTimerSelTreeItem = XPR_NULL;

        explore(sSelTreeItem);
    }
    else if (nIDEvent == TM_ID_CLICK_RENAME)
    {
        KillTimer(nIDEvent);

        if (mObserver != XPR_NULL)
        {
            mObserver->onRename(*this, XPR_NULL);
        }

        return;
    }

    super::OnTimer(nIDEvent);
}

void FolderCtrl::refresh(HTREEITEM aTreeItem)
{
    ScanDeque sScanDeque;
    scanRecursiveExist(sScanDeque, aTreeItem);

    if (sScanDeque.empty() == false)
    {
        SetRedraw(XPR_FALSE);

        HTREEITEM sTreeItem;
        HTREEITEM sParentTreeItem;
        HTREEITEM sSelTreeItem = XPR_NULL;
        HTREEITEM sOldSelTreeItem = GetSelectedItem();
        ScanDeque::iterator sIterator;

        sIterator = sScanDeque.begin();
        for (; sIterator != sScanDeque.end(); ++sIterator)
        {
            sTreeItem = *sIterator;
            if (XPR_IS_NULL(sTreeItem))
                continue;

            sParentTreeItem = GetParentItem(sTreeItem);

            if (GetSelectedItem() == sTreeItem || sSelTreeItem == sTreeItem || isChildItem(sParentTreeItem, sTreeItem) || isChildItem(sSelTreeItem, sTreeItem))
            {
                sSelTreeItem = sParentTreeItem;

                setUpdate(XPR_FALSE);
                SelectItem(GetRootItem());
                setUpdate(XPR_TRUE);
            }

            DeleteItem(sTreeItem);

            if (XPR_IS_NOT_NULL(sParentTreeItem))
                endShcn(sParentTreeItem);
        }

        if (XPR_IS_NOT_NULL(sSelTreeItem))
        {
            if (sOldSelTreeItem != sSelTreeItem)
                SelectItem(sSelTreeItem);
        }

        SetRedraw();
        UpdateWindow();
    }

    sScanDeque.clear();
}

void FolderCtrl::scanRecursiveExist(ScanDeque &aScanDeque, HTREEITEM aTreeItem)
{
    if (XPR_IS_NOT_NULL(aTreeItem))
    {
        xpr_bool_t sExisted = XPR_TRUE;

        LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
        {
            static xpr::tstring sPath;
            sPath.clear();

            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sPath);

            XPR_TRACE(XPR_STRING_LITERAL("Scan - %s\n"), sPath.c_str());
            if (IsExistFile(sPath) == XPR_FALSE)
            {
                aScanDeque.push_back(aTreeItem);
                sExisted = XPR_FALSE;
            }
        }

        if (XPR_IS_TRUE(sExisted))
        {
            if (GetChildItem(aTreeItem) != XPR_NULL)
                scanRecursiveExist(aScanDeque, GetChildItem(aTreeItem));
        }

        if (GetNextSiblingItem(aTreeItem) != XPR_NULL)
            scanRecursiveExist(aScanDeque, GetNextSiblingItem(aTreeItem));
    }
}

void FolderCtrl::getCurPath(xpr_tchar_t *aPath) const
{
    if (XPR_IS_NOT_NULL(aPath))
    {
        LPTVITEMDATA sTvItemData = XPR_NULL;
        HTREEITEM sTreeItem = GetSelectedItem();

        sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
        if (XPR_IS_NOT_NULL(sTvItemData) && XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, aPath);
    }
}

void FolderCtrl::getCurPath(xpr::tstring &aCurPath) const
{
    xpr_tchar_t sCurPath[XPR_MAX_PATH + 1] = {0};
    getCurPath(sCurPath);

    aCurPath = sCurPath;
}

void FolderCtrl::goSiblingUp(void)
{
    HTREEITEM sTreeItem = GetSelectedItem();
    if (XPR_IS_NULL(sTreeItem))
        return;

    sTreeItem = GetPrevSiblingItem(sTreeItem);
    if (XPR_IS_NULL(sTreeItem))
        return;

    SelectItem(sTreeItem);
}

void FolderCtrl::goSiblingDown(void)
{
    HTREEITEM sTreeItem = GetSelectedItem();
    if (XPR_IS_NULL(sTreeItem))
        return;

    sTreeItem = GetNextSiblingItem(sTreeItem);
    if (XPR_IS_NULL(sTreeItem))
        return;

    SelectItem(sTreeItem);
}

void FolderCtrl::expandAll(HTREEITEM aTreeItem)
{
    if (hasChildItem(aTreeItem) == XPR_FALSE)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("folder_tree.msg.confirm_expand"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    SetRedraw(XPR_FALSE);

    expandRecursiveAll(aTreeItem);

    SetRedraw();
    UpdateWindow();
}

void FolderCtrl::expandRecursiveAll(HTREEITEM aTreeItem)
{
    Expand(aTreeItem, TVE_EXPAND);

    aTreeItem = GetChildItem(aTreeItem);
    while (XPR_IS_NOT_NULL(aTreeItem))
    {
        if (hasChildItem(aTreeItem) == XPR_TRUE)
            expandRecursiveAll(aTreeItem);

        aTreeItem = GetNextSiblingItem(aTreeItem);
    }
}

void FolderCtrl::collapseAll(HTREEITEM aTreeItem)
{
    SetRedraw(XPR_FALSE);

    Expand(aTreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET);

    SetRedraw();
    UpdateWindow();
}

void FolderCtrl::OnKeydown(NMHDR *aNmHdr, LRESULT *aResult) 
{
    TV_KEYDOWN *sTvKeyDown = (TV_KEYDOWN*)aNmHdr;
    *aResult = 0;
}

xpr_sint_t FolderCtrl::OnMouseActivate(CWnd* pDesktopWnd, xpr_uint_t nHitTest, xpr_uint_t message) 
{
    CPoint sPoint;
    GetCursorPos(&sPoint);
    ScreenToClient(&sPoint);

    HTREEITEM sTreeItem = HitTest(sPoint);
    if (XPR_IS_NOT_NULL(sTreeItem))
        return MA_NOACTIVATE;

    return super::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void FolderCtrl::OnMButtonDown(xpr_uint_t nFlags, CPoint pt) 
{
    CWindowScroller *ws = new CWindowScroller(this, pt);

    super::OnMButtonDown(nFlags, pt);
}

void FolderCtrl::OnPaint(void)
{
    COLORREF sSysImgListBkColor;
    CImageList *sImgList = XPR_NULL;

    xpr_bool_t sCustomBkgndColor = (mOption.mBkgndColorType == COLOR_TYPE_CUSTOM) ? XPR_TRUE : XPR_FALSE;
    COLORREF sBkgndColor = mOption.mBkgndColor;

    if (XPR_IS_TRUE(sCustomBkgndColor))
    {
        sImgList = GetImageList(TVSIL_NORMAL);
        if (XPR_IS_NOT_NULL(sImgList))
        {
            sSysImgListBkColor = sImgList->GetBkColor();
            sImgList->SetBkColor(sBkgndColor);
        }
    }

    Default();

    if (XPR_IS_TRUE(sCustomBkgndColor))
    {
        if (XPR_IS_NOT_NULL(sImgList))
        {
            sImgList->SetBkColor(sSysImgListBkColor);
        }
    }
}

void FolderCtrl::OnCustomdraw(NMHDR *aNmHdr, LRESULT *aResult)
{
    NMTVCUSTOMDRAW *sNmTvCustomDraw = reinterpret_cast<NMTVCUSTOMDRAW *>(aNmHdr);

    *aResult = CDRF_DODEFAULT;

    if (sNmTvCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *aResult = CDRF_NOTIFYITEMDRAW;
    }
    if (sNmTvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        *aResult = CDRF_NOTIFYPOSTPAINT;
    }
    else if (sNmTvCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
    {
        if (XPR_TEST_BITS(sNmTvCustomDraw->nmcd.uItemState, CDIS_SELECTED))
        {
            if (mOption.mHighlight == XPR_TRUE)
            {
                if (GetFocus() != this)
                {
                    HTREEITEM sTreeItem = (HTREEITEM)sNmTvCustomDraw->nmcd.dwItemSpec;
                    CDC *sDC = CDC::FromHandle(sNmTvCustomDraw->nmcd.hdc);

                    CRect sRect;
                    if (GetItemRect(sTreeItem, sRect, XPR_TRUE) == XPR_TRUE)
                    {
                        CPen sPen(PS_SOLID, 1, mOption.mHighlightColor);
                        CPen *sOldPen = sDC->SelectObject(&sPen);

                        CBrush *sBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
                        CBrush *sOldBrush = sDC->SelectObject(sBrush);

                        sDC->Rectangle(sRect);

                        sDC->SelectObject(sOldBrush);
                        sDC->SelectObject(sOldPen);
                    }
                }
            }
        }

        *aResult = CDRF_DODEFAULT;
    }
}

void FolderCtrl::OnClick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    *aResult = 0;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onClick(*this);
    }
}

void FolderCtrl::setCustomFont(xpr_bool_t aCustomFont, xpr_tchar_t *aFontText)
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

void FolderCtrl::setCustomFont(xpr_bool_t aCustomFont, CFont *aFont)
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

void FolderCtrl::setDragContents(xpr_bool_t aDragContents)
{
    mDropTarget.setDragContents(aDragContents);
}

LRESULT FolderCtrl::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    ShellIcon::AsyncIcon *sAsyncIcon = (ShellIcon::AsyncIcon *)wParam;
    if (XPR_IS_NULL(sAsyncIcon))
        return 0;

    HTREEITEM sTreeItem = (HTREEITEM)sAsyncIcon->mItem;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sTreeItem);
    if (XPR_IS_NULL(sTvItemData))
        sTreeItem = XPR_NULL;

    if (XPR_IS_NOT_NULL(sTreeItem) && XPR_IS_NOT_NULL(sTvItemData))
    {
        switch (sAsyncIcon->mType)
        {
        case ShellIcon::TypeIconIndex:
            {
                break;
            }

        case ShellIcon::TypeOverlayIndex:
            {
                xpr_uint_t sState = 0;

                if (sAsyncIcon->mResult.mIconIndex >= 0)
                    sState = INDEXTOOVERLAYMASK(sAsyncIcon->mResult.mIconIndex);
                else
                {
                    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_SHARE)) sState = INDEXTOOVERLAYMASK(1);
                    if (XPR_TEST_BITS(sTvItemData->mShellAttributes, SFGAO_LINK))  sState = INDEXTOOVERLAYMASK(2);
                }

                SetItemState(sTreeItem, sState, LVIS_OVERLAYMASK);
                break;
            }
        }
    }

    XPR_SAFE_DELETE(sAsyncIcon);

    return 0;
}

void FolderCtrl::setChangeNotify(xpr::tstring aPath, xpr_bool_t aAllSubTree, xpr_bool_t aEnable)
{
    if (aPath.length() == 2) // 'c:' -> 'c:\\'
        aPath += XPR_STRING_LITERAL('\\');

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(aPath.c_str());
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr::tstring sFullPath;
        GetDispFullPath(sFullPidl, sFullPath);

        HTREEITEM sTreeItem = searchTree(sFullPath.c_str(), XPR_FALSE, XPR_FALSE);
        if (XPR_IS_NOT_NULL(sTreeItem))
        {
            changeRecursiveNotify(sTreeItem, aAllSubTree, aEnable);
        }

        COM_FREE(sFullPidl);
    }
}

void FolderCtrl::changeRecursiveNotify(HTREEITEM aTreeItem, xpr_bool_t aAllSubTree, xpr_bool_t aEnable)
{
    if (XPR_IS_NULL(aTreeItem))
        return;

    LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(aTreeItem);
    if (XPR_IS_NOT_NULL(sTvItemData))
    {
        if (XPR_IS_FALSE(aEnable))
        {
            //FileChangeWatcher::instance().Unregister(sTvItemData->uWatchId);
            //sTvItemData->uWatchId = FileChangeWatcher::InvalidWatchId;
        }
        else
        {
            if (GetChildItem(aTreeItem) != XPR_NULL)
            {
                //if (sTvItemData->uWatchId == FileChangeWatcher::InvalidWatchId)
                //{
                //    FileChangeWatcher::CWatchItem sWatchItem;
                //    sWatchItem.hWnd = m_hWnd;
                //    sWatchItem.uMsg = WM_FILE_CHANGE_NOTIFY;
                //    GetName(sTvItemData->mShellFolder, sTvItemData->mPidl, SHGDN_FORPARSING, sWatchItem.strPath);
                //    sWatchItem.bSubPath = XPR_FALSE;
                //    sWatchItem.lParam = (LPARAM)aTreeItem;

                //    sTvItemData->uWatchId = FileChangeWatcher::instance().Register(&sWatchItem);
                //}
            }
        }
    }

    if (XPR_IS_FALSE(aAllSubTree))
        return;

    if (GetChildItem(aTreeItem) != XPR_NULL)
        changeRecursiveNotify(GetChildItem(aTreeItem), aAllSubTree, aEnable);

    if (GetNextSiblingItem(aTreeItem) != XPR_NULL)
        changeRecursiveNotify(GetNextSiblingItem(aTreeItem), aAllSubTree, aEnable);
}

xpr_bool_t FolderCtrl::isChildItem(HTREEITEM aTreeItem, HTREEITEM aChildTreeItem)
{
    if (XPR_IS_NULL(aTreeItem) || XPR_IS_NULL(aChildTreeItem))
        return XPR_FALSE;

    while (true)
    {
        aChildTreeItem = GetParentItem(aChildTreeItem);
        if (aChildTreeItem == aTreeItem)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

HTREEITEM FolderCtrl::insertPartialNetComRootItem(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
    {
        return XPR_NULL;
    }

    if (IsNetItem(aFullPidl) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    xpr::tstring sFullPath;
    GetDispFullPath(aFullPidl, sFullPath);

    HTREEITEM sTreeItem;
    sTreeItem = searchTree(sFullPath.c_str(), XPR_FALSE, XPR_FALSE);
    if (XPR_IS_NOT_NULL(sTreeItem))
    {
        return sTreeItem;
    }

    // network neighborhood folder
    xpr::tstring sNetFolder;
    WnetMgr::instance().getNetFullPath(sNetFolder);

    HTREEITEM sParentTreeItem;
    sParentTreeItem = searchTree(sNetFolder.c_str(), XPR_FALSE, XPR_TRUE);
    if (XPR_IS_NULL(sParentTreeItem))
    {
        return XPR_NULL;
    }

    LPTVITEMDATA sNetTvItemData = (LPTVITEMDATA)GetItemData(sParentTreeItem);
    if (XPR_IS_NULL(sNetTvItemData))
    {
        return XPR_NULL;
    }

    sNetTvItemData->mExpandPartial = XPR_TRUE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    sNetTvItemData->mShellFolder->BindToObject(sNetTvItemData->mPidl, 0, IID_IShellFolder, (LPVOID *)&sShellFolder);

    LPITEMIDLIST sPidl = ::ILFindLastID(aFullPidl);

    xpr_ulong_t    sShellAttributes = 0;
    DWORD          sFileAttributes  = 0;
    LPTVITEMDATA   sTvItemData;
    TVINSERTSTRUCT sTvInsertStruct = {0};

    if (getItemAttributes(sShellFolder, sPidl, sShellAttributes, sFileAttributes) == XPR_FALSE)
    {
        return XPR_NULL;
    }

    if (mOption.mShowSystemAttribute == XPR_FALSE && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_HIDDEN) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_SYSTEM))
    {
        return XPR_NULL;
    }

    sTvItemData                   = new TVITEMDATA;
    sTvItemData->mShellFolder     = sShellFolder;
    sTvItemData->mPidl            = fxfile::base::Pidl::clone(sPidl);
    sTvItemData->mFullPidl        = fxfile::base::Pidl::concat(sNetTvItemData->mFullPidl, sPidl);
    sTvItemData->mShellAttributes = sShellAttributes;
    sTvItemData->mFileAttributes  = sFileAttributes;
    //sTvItemData->uWatchId       = FileChangeWatcher::InvalidWatchId;
    sTvItemData->mExpandPartial   = XPR_FALSE;
    sShellFolder->AddRef();

    sTvInsertStruct.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
    sTvInsertStruct.item.iImage         = I_IMAGECALLBACK;
    sTvInsertStruct.item.iSelectedImage = I_IMAGECALLBACK;
    sTvInsertStruct.item.cChildren      = I_CHILDRENCALLBACK;
    sTvInsertStruct.item.pszText        = LPSTR_TEXTCALLBACK;
    sTvInsertStruct.item.cchTextMax     = XPR_MAX_PATH;
    sTvInsertStruct.item.lParam         = (LPARAM)sTvItemData;

    sTvInsertStruct.hInsertAfter = TVI_LAST;
    sTvInsertStruct.hParent      = sParentTreeItem;

    sTreeItem = InsertItem(&sTvInsertStruct);
    if (XPR_IS_NULL(sTreeItem))
    {
        COM_RELEASE(sTvItemData->mShellFolder);
        COM_FREE(sTvItemData->mPidl);
        COM_FREE(sTvItemData->mFullPidl);
        XPR_SAFE_DELETE(sTvItemData);

        return XPR_NULL;
    }

    Expand(sParentTreeItem, TVE_EXPAND | TVE_EXPANDPARTIAL);

    return sTreeItem;
}

HTREEITEM FolderCtrl::searchSel(LPITEMIDLIST aFullPidl, xpr_bool_t aUpdate)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_NULL;

    xpr::tstring sFullPath;
    GetDispFullPath(aFullPidl, sFullPath);

    HTREEITEM sTreeItem = XPR_NULL;

    xpr_size_t sFind = sFullPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != xpr::tstring::npos)
    {
        xpr::tstring sParentFullPath;
        sParentFullPath = sFullPath.substr(0, sFind);

        HTREEITEM sSelTreeItem = GetSelectedItem();
        if (XPR_IS_NOT_NULL(sSelTreeItem))
        {
            LPTVITEMDATA sTvItemData = (LPTVITEMDATA)GetItemData(sSelTreeItem);
            if (XPR_IS_NOT_NULL(sTvItemData))
            {
                xpr::tstring sSelFullPath;
                GetDispFullPath(sTvItemData->mFullPidl, sSelFullPath);

                if (_tcsicmp(sParentFullPath.c_str(), sSelFullPath.c_str()) == 0)
                {
                    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
                    GetName(aFullPidl, SHGDN_INFOLDER, sName);

                    Expand(sSelTreeItem, TVE_EXPAND);

                    sTreeItem = searchTree(sName, XPR_TRUE, XPR_TRUE);
                    if (XPR_IS_NULL(sTreeItem))
                    {
                        SetRedraw(XPR_FALSE);

                        sSelTreeItem = GetSelectedItem();
                        if (Expand(sSelTreeItem, TVE_COLLAPSE | TVE_COLLAPSERESET) == XPR_FALSE)
                        {
                            TVITEM sTvItem = {0};
                            sTvItem.mask  = TVIF_HANDLE;
                            sTvItem.hItem = sSelTreeItem;
                            GetItem(&sTvItem);

                            sTvItem.mask      = TVIF_HANDLE | TVIF_CHILDREN;
                            sTvItem.hItem     = sSelTreeItem;
                            sTvItem.cChildren = 1;
                            SetItem(&sTvItem);
                        }

                        Expand(sSelTreeItem, TVE_EXPAND);
                        sTreeItem = searchTree(sName, XPR_TRUE, XPR_TRUE);

                        SetRedraw();
                    }
                }
            }
        }
    }

    if (IsNetItem(aFullPidl) == XPR_TRUE)
    {
        // network neighborhodd folder
        xpr::tstring sNetFullPath;
        WnetMgr::instance().getNetFullPath(sNetFullPath);

        // network share computer
        xpr::tstring sNetComFullPath(sFullPath);
        xpr_size_t sFind = sNetComFullPath.find(XPR_STRING_LITERAL('\\'), sNetFullPath.length()+1);
        if (sFind != xpr::tstring::npos)
            sNetComFullPath.erase(sFind);

        if (searchTree(sNetComFullPath.c_str(), XPR_FALSE, XPR_FALSE) == XPR_FALSE)
        {
            xpr_sint_t sSplitCount = 0;

            sFind = 0;
            while (true)
            {
                sFind = sNetComFullPath.find(XPR_STRING_LITERAL('\\'), sFind);
                if (sFind == xpr::tstring::npos)
                    break;

                sSplitCount++;
                sFind++;
            }

            LPITEMIDLIST sNetComPidl = fxfile::base::Pidl::clone(aFullPidl);

            xpr_sint_t i;
            xpr_sint_t sCount = fxfile::base::Pidl::getItemCount(aFullPidl) - sSplitCount;
            for (i = 0; i < sCount; ++i)
                fxfile::base::Pidl::removeLastItem(sNetComPidl);

            sTreeItem = insertPartialNetComRootItem(sNetComPidl);
            if (sFullPath != sNetComFullPath)
                sTreeItem = XPR_NULL;

            COM_FREE(sNetComPidl);
        }
    }

    if (XPR_IS_NULL(sTreeItem))
        sTreeItem = searchTree(sFullPath.c_str(), XPR_FALSE, XPR_TRUE);

    if (XPR_IS_NOT_NULL(sTreeItem))
    {
        if (XPR_IS_FALSE(aUpdate))
            setUpdate(XPR_FALSE);

        SelectItem(sTreeItem);

        if (XPR_IS_FALSE(aUpdate))
            setUpdate(XPR_TRUE);
    }

    return sTreeItem;
}
} // namespace fxfile
