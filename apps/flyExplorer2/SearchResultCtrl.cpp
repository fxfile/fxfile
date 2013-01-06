//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SearchResultCtrl.h"

#include "fxb/fxb_fnmatch.h"        // for Name Selection, match
#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_file_op_thread.h"
#include "fxb/fxb_filter.h"
#include "fxb/fxb_file_ass.h"
#include "fxb/fxb_file_scrap.h"
#include "fxb/fxb_shell_icon.h"

#include "rgc/FlatHeaderCtrl.h"
#include "rgc/DropSource.h"     // for Drag & Drop
#include "rgc/BCMenu.h"

#include "SearchResultCtrlObserver.h"
#include "MainFrame.h"
#include "ExplorerCtrl.h"
#include "command_string_table.h"

#include <intshcut.h>           // for URL
#include <mapi.h>               // for send mail

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// timer id
//
enum
{
    TM_ID_RENAME_VISTA = 20,
};

//
// user defined message
//
enum
{
    WM_SHELL_ASYNC_ICON    = WM_USER + 11,
    WM_SHELL_CHANGE_NOTIFY = WM_USER + 1500,
};

xpr_uint_t SearchResultCtrl::mCodeMgr = 0;

SearchResultCtrl::SearchResultCtrl(void)
{
    mObserver      = XPR_NULL;

    mSortColumn    = 0;
    mSortAscending = XPR_TRUE;

    mHeaderCtrl    = XPR_NULL;
    mShellIcon     = XPR_NULL;

    mResultFileCount = 0;
    mResultDirCount  = 0;
    mResultTotalSize = 0;
    mNotify        = XPR_TRUE;

    mCode          = 0;
    mSignature     = 0;

    mMouseEdit     = XPR_TRUE;
    mEditExt       = XPR_NULL;
    mEditSel       = -1;

    mShcnId        = 0;
}

SearchResultCtrl::~SearchResultCtrl(void)
{
    XPR_SAFE_DELETE_ARRAY(mEditExt);
    mResultDeque.clear();

    XPR_SAFE_DELETE(mShellIcon);
}

void SearchResultCtrl::setObserver(SearchResultCtrlObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t SearchResultCtrl::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;
    sStyle |= LVS_REPORT;
    sStyle |= LVS_SHOWSELALWAYS;
    sStyle |= LVS_EDITLABELS;

    return super::Create(sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(SearchResultCtrl, super)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_CREATE()
    ON_WM_CONTEXTMENU()
    ON_WM_MOUSEACTIVATE()
    ON_WM_INITMENUPOPUP()
    ON_WM_HSCROLL()
    ON_WM_DESTROY()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_SETFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_TIMER()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO,    OnGetdispinfo)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM,     OnDeleteitem)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG,      OnBegindrag)
    ON_NOTIFY_REFLECT(LVN_BEGINRDRAG,     OnBeginrdrag)
    ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE,   OnItemActivate)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT,   OnEndlabeledit)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK,    OnColumnclick)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN,        OnLvnKeyDown)
    ON_MESSAGE(WM_SHELL_CHANGE_NOTIFY, OnShellChangeNotify)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON,    OnShellAsyncIcon)
END_MESSAGE_MAP()

xpr_sint_t SearchResultCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    DWORD sExStyle = GetExtendedStyle();
    sExStyle |= WS_EX_CLIENTEDGE;
    sExStyle |= LVS_EX_DOUBLEBUFFER; // support from WinXP
    SetExtendedStyle(sExStyle);

    // enable explorer theme
    SetWindowTheme(m_hWnd, XPR_WIDE_STRING_LITERAL("explorer"), XPR_NULL);

    if (subclassHeader() == XPR_FALSE)
        return -1;

    InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("search_result.column.name")),     LVCFMT_LEFT,  200);
    InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("search_result.column.location")), LVCFMT_LEFT,  200);
    InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("search_result.column.size")),     LVCFMT_RIGHT, 100);
    InsertColumn(3, theApp.loadString(XPR_STRING_LITERAL("search_result.column.type")),     LVCFMT_LEFT,  100);
    InsertColumn(4, theApp.loadString(XPR_STRING_LITERAL("search_result.column.date")),     LVCFMT_LEFT,  100);

    //createAccelTable();

    if (XPR_IS_NULL(mShellIcon))
    {
        mShellIcon = new fxb::ShellIcon;
        mShellIcon->setOwner(*this, WM_SHELL_ASYNC_ICON);
    }

    xpr_slong_t sEventMask = 0;
    sEventMask |= SHCNE_RENAMEITEM;
    sEventMask |= SHCNE_CREATE;
    sEventMask |= SHCNE_DELETE;
    sEventMask |= SHCNE_MKDIR;
    sEventMask |= SHCNE_RMDIR;

    // TODO: When drive or media eject, search result would clear.
    //sEventMask |= SHCNE_MEDIAREMOVED;
    //sEventMask |= SHCNE_DRIVEREMOVED;

    //sEventMask |= SHCNE_NETSHARE;
    //sEventMask |= SHCNE_NETUNSHARE;
    sEventMask |= SHCNE_ATTRIBUTES;
    sEventMask |= SHCNE_UPDATEDIR;
    sEventMask |= SHCNE_UPDATEITEM;
    //sEventMask |= SHCNE_SERVERDISCONNECT;
    sEventMask |= SHCNE_UPDATEIMAGE;
    sEventMask |= SHCNE_RENAMEFOLDER;

    mShcnId = fxb::ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, XPR_NULL, sEventMask, XPR_TRUE);

    return 0;
}

void SearchResultCtrl::OnDestroy(void)
{
    super::OnDestroy();

    fxb::ShellChangeNotify::instance().unregisterWatch(mShcnId);

    if (XPR_IS_NOT_NULL(mShellIcon))
        mShellIcon->Stop();

    DESTROY_DELETE(mHeaderCtrl);
}

xpr_bool_t SearchResultCtrl::subclassHeader(xpr_bool_t aBoldFont)
{
    mHeaderCtrl = new FlatHeaderCtrl;
    ASSERT(mHeaderCtrl);

    HWND sHwnd = GetDlgItem(0)->GetSafeHwnd();
    if (XPR_IS_NULL(sHwnd))
        return XPR_FALSE;

    if (mHeaderCtrl->SubclassWindow(sHwnd) == XPR_FALSE)
        return XPR_FALSE;

    mHeaderCtrl->initHeader(aBoldFont);

    return XPR_TRUE;
}

void SearchResultCtrl::OnWindowPosChanging(WINDOWPOS FAR *aWindowPos)
{
    super::OnWindowPosChanging(aWindowPos);

    //ShowScrollBar(SB_HORZ, XPR_FALSE);
    //ModifyStyle(WS_HSCROLL, 0, SWP_DRAWFRAME);
}

xpr_bool_t SearchResultCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *aResult)
{
    return super::OnNotify(wParam, lParam, aResult);
}

void SearchResultCtrl::onStartSearch(void)
{
    sortItems(-1);

    {
        fxb::CsLocker aLocker(mCs);

        mResultFileCount = 0;
        mResultDirCount  = 0;
        mResultTotalSize = 0;

        mResultDeque.clear();
    }

    SetRedraw(XPR_FALSE);
    clearList();
    SetRedraw();

    mCode = mCodeMgr++;
    if (mCode == 0)
        mCode = mCodeMgr++;

    mSignature = 0;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onStartSearch(*this);
    }
}

void SearchResultCtrl::onSearch(const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData)
{
    SrItemData *sSrItemData = new SrItemData;
    if (XPR_IS_NULL(sSrItemData))
        return;

    sSrItemData->mDir = new xpr_tchar_t[_tcslen(aDir) + 3];
    if (XPR_IS_NULL(sSrItemData->mDir))
    {
        XPR_SAFE_DELETE(sSrItemData);
        return;
    }

    _tcscpy(sSrItemData->mDir, aDir);
    if (_tcslen(sSrItemData->mDir) == 2)
        _tcscat(sSrItemData->mDir, XPR_STRING_LITERAL("\\"));

    sSrItemData->mFileName = new xpr_tchar_t[_tcslen(aWin32FindData->cFileName) + 1];
    if (XPR_IS_NULL(sSrItemData->mFileName))
    {
        XPR_SAFE_DELETE_ARRAY(sSrItemData->mDir);
        XPR_SAFE_DELETE(sSrItemData);
        return;
    }

    _tcscpy(sSrItemData->mFileName, aWin32FindData->cFileName);

    sSrItemData->mFileSize         = (xpr_uint64_t)aWin32FindData->nFileSizeHigh * (xpr_uint64_t)kuint32max + aWin32FindData->nFileSizeLow;
    sSrItemData->mModifiedFileTime = aWin32FindData->ftLastWriteTime;
    sSrItemData->mFileAttributes   = aWin32FindData->dwFileAttributes;

    {
        fxb::CsLocker aLocker(mCs);

        mResultDeque.push_back(sSrItemData);
    }
}

void SearchResultCtrl::onEndSearch(void)
{
    SetRedraw(XPR_FALSE);

    {
        fxb::CsLocker aLocker(mCs);

        SrItemData *sSrItemData;
        ResultDeque::iterator sIterator;

        sIterator = mResultDeque.begin();
        for (; sIterator != mResultDeque.end(); ++sIterator)
        {
            sSrItemData = *sIterator;

            if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                ++mResultDirCount;
            }
            else
            {
                ++mResultFileCount;
                mResultTotalSize += sSrItemData->mFileSize;
            }

            addList(sSrItemData);
        }

        mResultDeque.clear();
    }

    SetRedraw();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onEndSearch(*this);
    }
}

void SearchResultCtrl::getResultInfo(xpr_sint_t &aResultFileCount, xpr_sint_t &aResultDirCount, xpr_uint64_t &aResultTotalSize) const
{
    aResultFileCount = mResultFileCount;
    aResultDirCount  = mResultDirCount;
    aResultTotalSize = mResultTotalSize;
}

void SearchResultCtrl::addList(const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData)
{
    insertList(-1, aDir, aWin32FindData);
}

void SearchResultCtrl::addList(SrItemData *aSrItemData)
{
    insertList(-1, aSrItemData);
}

void SearchResultCtrl::insertList(xpr_sint_t aIndex, const xpr_tchar_t *aDir, WIN32_FIND_DATA *aWin32FindData)
{
    SrItemData *sSrItemData = new SrItemData;
    if (XPR_IS_NULL(sSrItemData))
        return;

    sSrItemData->mDir = new xpr_tchar_t[_tcslen(aDir) + 3];
    if (XPR_IS_NULL(sSrItemData->mDir))
    {
        XPR_SAFE_DELETE(sSrItemData);
        return;
    }

    _tcscpy(sSrItemData->mDir, aDir);
    if (_tcslen(sSrItemData->mDir) == 2)
        _tcscat(sSrItemData->mDir, XPR_STRING_LITERAL("\\"));

    sSrItemData->mFileName = new xpr_tchar_t[_tcslen(aWin32FindData->cFileName) + 1];
    if (XPR_IS_NULL(sSrItemData->mFileName))
    {
        XPR_SAFE_DELETE_ARRAY(sSrItemData->mDir);
        XPR_SAFE_DELETE(sSrItemData);
        return;
    }

    _tcscpy(sSrItemData->mFileName, aWin32FindData->cFileName);

    sSrItemData->mFileSize         = (xpr_uint64_t)aWin32FindData->nFileSizeHigh * (xpr_uint64_t)kuint32max + aWin32FindData->nFileSizeLow;
    sSrItemData->mModifiedFileTime = aWin32FindData->ftLastWriteTime;
    sSrItemData->mFileAttributes   = aWin32FindData->dwFileAttributes;

    insertList(-1, sSrItemData);
}

void SearchResultCtrl::insertList(xpr_sint_t aIndex, SrItemData *aSrItemData)
{
    if (aIndex == -1)
        aIndex = GetItemCount();

    aSrItemData->mSignature = mSignature++;

    LVITEM sLvItem = {0};
    sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem      = aIndex;
    sLvItem.iSubItem   = 0;
    sLvItem.iImage     = I_IMAGECALLBACK;
    sLvItem.pszText    = LPSTR_TEXTCALLBACK;
    sLvItem.cchTextMax = XPR_MAX_PATH;
    sLvItem.lParam     = (LPARAM)aSrItemData;
    InsertItem(&sLvItem);
}

void SearchResultCtrl::clearList(void)
{
    DeleteAllItems();
}

void SearchResultCtrl::getDispFileName(const xpr_tchar_t *aFileName, xpr_tchar_t *aDispFileName)
{
    if (XPR_IS_NULL(aFileName) || XPR_IS_NULL(aDispFileName))
        return;

    _tcscpy(aDispFileName, aFileName);

    xpr_tchar_t *sExt = (xpr_tchar_t *)fxb::GetFileExt(aDispFileName);
    if (XPR_IS_NOT_NULL(sExt))
    {
        if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
            sExt[0] = XPR_STRING_LITERAL('\0');
    }
}

void SearchResultCtrl::OnGetdispinfo(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO *)aNmHdr;
    *aResult = 0;

    LVITEM &sLvItem = sLvDispInfo->item;

    SrItemData *sSrItemData = (SrItemData *)sLvItem.lParam;
    if (XPR_IS_NULL(sSrItemData))
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    sSrItemData->getPath(sPath);

    if (sLvItem.mask & LVIF_TEXT)
    {
        switch (sLvItem.iSubItem)
        {
        case 0:
            getDispFileName(sSrItemData->mFileName, sLvItem.pszText);
            break;

        case 1:
            _tcscpy(sLvItem.pszText, sSrItemData->mDir);
            break;

        case 2:
            fxb::GetFileSize(sPath, sLvItem.pszText, sLvItem.cchTextMax);
            break;

        case 3:
            fxb::GetFileType(sPath, sLvItem.pszText);
            break;

        case 4:
            fxb::GetFileTime(sPath, sLvItem.pszText);
            break;
        }
    }

    if (sLvItem.mask & LVIF_IMAGE)
    {
        const xpr_tchar_t *sExt = fxb::GetFileExt(sPath);
        if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".exe")) == 0)
        {
            static xpr_sint_t sExeIconIndex = -1;
            if (sExeIconIndex == -1)
                sExeIconIndex = fxb::GetFileExtIconIndex(XPR_STRING_LITERAL(".exe"));

            sLvItem.iImage = sExeIconIndex;

            fxb::ShellIcon::AsyncIcon *sAsyncIcon = new fxb::ShellIcon::AsyncIcon;
            sAsyncIcon->mType              = fxb::ShellIcon::TypeIconIndex;
            sAsyncIcon->mCode              = mCode;
            sAsyncIcon->mItem              = sLvItem.iItem;
            sAsyncIcon->mSignature         = sSrItemData->mSignature;
            sAsyncIcon->mPath              = sPath;
            sAsyncIcon->mResult.mIconIndex = -1;

            if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(sAsyncIcon);
            }
        }
        else
        {
            sLvItem.iImage = fxb::GetItemIconIndex(sPath);

            sLvItem.state = 0;
            sLvItem.stateMask = 0;

            if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_HIDDEN))
            {
                sLvItem.mask      |= LVIF_STATE;
                sLvItem.state     |= LVIS_CUT;
                sLvItem.stateMask |= LVIS_CUT;
            }

            if (XPR_IS_NOT_NULL(sExt))
            {
                if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
                {
                    sLvItem.mask      |= LVIF_STATE;
                    sLvItem.state     |= INDEXTOOVERLAYMASK(2);
                    sLvItem.stateMask |= LVIS_OVERLAYMASK;
                }
            }

            // shell icon custom overlay
            fxb::ShellIcon::AsyncIcon *sAsyncIcon = new fxb::ShellIcon::AsyncIcon;
            sAsyncIcon->mType              = fxb::ShellIcon::TypeOverlayIndex;
            sAsyncIcon->mCode              = mCode;
            sAsyncIcon->mItem              = sLvItem.iItem;
            sAsyncIcon->mSignature         = sSrItemData->mSignature;
            sAsyncIcon->mPath              = sPath;
            sAsyncIcon->mResult.mIconIndex = -1;

            if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
            {
                XPR_SAFE_DELETE(sAsyncIcon);
            }
        }
    }

    sLvItem.mask |= LVIF_DI_SETITEM;
}

void SearchResultCtrl::OnDeleteitem(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    SrItemData *sSrItemData = (SrItemData *)sNmListView->lParam;
    if (XPR_IS_NOT_NULL(sSrItemData))
    {
        if (XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            --mResultDirCount;
        }
        else
        {
            --mResultFileCount;
            mResultTotalSize -= sSrItemData->mFileSize;
        }

        XPR_SAFE_DELETE_ARRAY(sSrItemData->mDir);
        XPR_SAFE_DELETE_ARRAY(sSrItemData->mFileName);
        XPR_SAFE_DELETE_ARRAY(sSrItemData);
    }

    *aResult = 0;
}

LRESULT SearchResultCtrl::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu)
{
    LRESULT sResult;

    if (BCMenu::IsMenu(aMenu) == XPR_TRUE)
        sResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sResult = super::OnMenuChar(aChar, aFlags, aMenu);

    return sResult;
}

void SearchResultCtrl::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    //super::OnInitMenuPopup(aPopupMenu, aIndex, aSysMenu);
    ASSERT(aPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = aPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
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
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = aPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == aPopupMenu->m_hMenu)
        sState.m_pParentMenu = aPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL && (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t sIndex;
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (sIndex = 0; sIndex < sIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == aPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = aPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = aPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = aPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL || (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 || sState.m_nID == (xpr_uint_t)-1)
            {
                continue;       // First item of popup can't be routed to.
            }
            sState.DoUpdate(this, XPR_TRUE);   // Popups are never auto disabled.
        }
        else
        {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            sState.m_pSubMenu = XPR_NULL;
            sState.DoUpdate(this, XPR_FALSE);
        }

        // Adjust for menu deletions and additions.
        xpr_uint_t sCount = aPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount && aPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void SearchResultCtrl::OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct)
{
    xpr_bool_t sSetFlag = XPR_FALSE;

    if (aMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)aMeasureItemStruct->itemID) == XPR_TRUE)
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)aMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu) == XPR_TRUE)
            {
                mMenu.MeasureItem(aMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sSetFlag))
        super::OnMeasureItem(aIdCtl, aMeasureItemStruct);
}

xpr_bool_t SearchResultCtrl::getSelPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount)
{
    *aShellFolder = XPR_NULL;
    aCount = 0;

    xpr_sint_t sIndex = -1;

    xpr_sint_t sSelCount = 0;
    if (GetSelectedCount() > 0)
    {
        POSITION sPosition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = GetNextSelectedItem(sPosition);
            if (sIndex >= 0)
                sSelCount++;
        }
    }

    if (sSelCount == 0)
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (sSelCount == 1)
    {
        LPSHELLFOLDER sShellFolder = XPR_NULL;

        SrItemData *sSrItemData = (SrItemData *)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sSrItemData))
        {
            std::tstring sPath;
            sSrItemData->getPath(sPath);

            LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sPath.c_str());
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                LPITEMIDLIST sPidl = XPR_NULL;
                HRESULT sHResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);

                if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
                    aPidls[0] = fxb::CopyItemIDList(sPidl);
                else
                {
                    COM_RELEASE(sShellFolder);
                }

                COM_FREE(sFullPidl);
            }
        }

        if (XPR_IS_NOT_NULL(sShellFolder))
        {
            aCount = 1;
            *aShellFolder = sShellFolder;

            sResult = XPR_TRUE;
        }
    }
    else
    {
        POSITION sPosition = GetFirstSelectedItemPosition();
        xpr_sint_t sIndex = GetNextSelectedItem(sPosition);

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetItemText(sIndex, 1, sPath, XPR_MAX_PATH + 1);
        sPath[3] = '\0';

        LPITEMIDLIST sParentFullPidl = fxb::SHGetPidlFromPath(sPath);
        if (XPR_IS_NULL(sParentFullPidl))
            return XPR_FALSE;

        LPSHELLFOLDER sShellFolder2 = XPR_NULL;
        HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder2);
        if (SUCCEEDED(sHResult))
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            sHResult = sShellFolder2->BindToObject(sParentFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolder);
            COM_RELEASE(sShellFolder2);

            if (SUCCEEDED(sHResult))
            {
                xpr_sint_t i = 0;
                xpr_tchar_t sPath[XPR_MAX_PATH + 1];
                SrItemData *sSrItemData = XPR_NULL;
                LPITEMIDLIST sFullPidl = XPR_NULL;
                LPITEMIDLIST sChildPidl = XPR_NULL;

                sPosition = GetFirstSelectedItemPosition();
                while (XPR_IS_NOT_NULL(sPosition))
                {
                    sIndex = GetNextSelectedItem(sPosition);

                    sSrItemData = (SrItemData *)GetItemData(sIndex);
                    if (XPR_IS_NOT_NULL(sSrItemData))
                    {
                        sSrItemData->getPath(sPath);

                        sFullPidl = fxb::SHGetPidlFromPath(sPath);
                        if (XPR_IS_NULL(sFullPidl))
                            continue;

                        sChildPidl = fxb::CopyItemIDList(fxb::IL_FindChild(sParentFullPidl, sFullPidl));
                        COM_FREE(sFullPidl);
                        if (XPR_IS_NULL(sChildPidl))
                            continue;

                        aPidls[i++] = sChildPidl;
                    }
                }

                aCount = i;
                *aShellFolder = sShellFolder;

                if (aCount > 0)
                    sResult = XPR_TRUE;
            }
        }

        COM_FREE(sParentFullPidl);
    }

    return sResult;
}

xpr_bool_t SearchResultCtrl::getSelFullPidls(LPSHELLFOLDER *aShellFolder, LPITEMIDLIST *aPidls, xpr_sint_t &aCount)
{
    *aShellFolder = XPR_NULL;
    aCount = 0;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;
    if (SUCCEEDED(sHResult))
    {
        xpr_sint_t i = 0;
        xpr_sint_t sIndex;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        SrItemData *sSrItemData = XPR_NULL;
        LPITEMIDLIST sFullPidl = XPR_NULL;

        POSITION sPosition = GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = GetNextSelectedItem(sPosition);

            sSrItemData = (SrItemData *)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData))
            {
                sSrItemData->getPath(sPath);

                sFullPidl = fxb::IL_CreateFromPath(sPath);
                if (XPR_IS_NULL(sFullPidl))
                    continue;

                aPidls[i++] = sFullPidl;
            }
        }

        aCount = i;
        *aShellFolder = sShellFolder;

        if (aCount > 0)
            sResult = XPR_TRUE;
    }

    return sResult;
}

void SearchResultCtrl::OnContextMenu(CWnd *aWnd, CPoint aPoint) 
{
    CRect sWindowRect;
    GetWindowRect(&sWindowRect);

    const DWORD sStyle = GetStyle();
    const xpr_bool_t sHorzScrl = XPR_TEST_BITS(sStyle, WS_HSCROLL);
    const xpr_bool_t sVertScrl = XPR_TEST_BITS(sStyle, WS_VSCROLL);

    CRect sHorzScrlRect(&sWindowRect);
    CRect sVertScrlRect(&sWindowRect);
    sHorzScrlRect.top  = sHorzScrlRect.bottom - GetSystemMetrics(SM_CYHSCROLL);
    sVertScrlRect.left = sVertScrlRect.right  - GetSystemMetrics(SM_CXVSCROLL);

    if ((XPR_IS_TRUE(sHorzScrl) && sHorzScrlRect.PtInRect(aPoint)) ||
        (XPR_IS_TRUE(sVertScrl) && sVertScrlRect.PtInRect(aPoint)))
    {
        // default scroll bar
        super::OnContextMenu(aWnd, aPoint);
        return;
    }

    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount > 0)
    {
        xpr_sint_t sCount = 0;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];

        if (getSelPidls(&sShellFolder, sPidls, sCount) == XPR_TRUE)
        {
            if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
            {
                CRect sRect;
                xpr_sint_t sFocus = GetSelectionMark();
                GetItemRect(sFocus, sRect, LVIR_BOUNDS);
                ClientToScreen(&sRect);

                aPoint.x = sRect.left;
                aPoint.y = sRect.bottom;
            }

            xpr_uint_t sFlags = TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON;
            if (XPR_IS_FALSE(gOpt->mAnimationMenu))
                sFlags |= 0x4000L;

            CMenu sMenu;
            sMenu.CreatePopupMenu();

            xpr_bool_t sOwnerMenu = XPR_FALSE;
            fxb::ContextMenu sContextMenu(GetSafeHwnd());
            if (sContextMenu.init(sShellFolder, sPidls, sCount) == XPR_TRUE && sContextMenu.getMenu(&sMenu) == XPR_TRUE)
            {
                ::InsertMenu(sMenu.m_hMenu, 0, MF_BYPOSITION, sContextMenu.getIdFirst() + CMID_OPEN_PARENT_FOLDER, theApp.loadString(XPR_STRING_LITERAL("cmd.search_result.open_parent_folder")));
                ::InsertMenu(sMenu.m_hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, XPR_NULL);
                //::SetMenuDefaultItem(sMenu.m_hMenu, 0, XPR_TRUE);

                xpr_uint_t sId = ::TrackPopupMenuEx(sMenu.m_hMenu, sFlags, aPoint.x, aPoint.y, m_hWnd, XPR_NULL);
                if (sId != -1)
                {
                    sId -= sContextMenu.getIdFirst();

                    if (invokeCommandSelf(&sContextMenu, sId) == XPR_FALSE)
                        sContextMenu.invokeCommand(sId);
                }

                sOwnerMenu = XPR_TRUE;
            }

            sMenu.DestroyMenu();
            sContextMenu.destroySubclass();
            sContextMenu.release();

            if (XPR_IS_FALSE(sOwnerMenu))
            {
                fxb::ContextMenu::trackItemMenu(sShellFolder, sPidls, sCount, &aPoint, sFlags, m_hWnd);
            }
        }

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            COM_FREE(sPidls[i]);
        }

        COM_RELEASE(sShellFolder);
        XPR_SAFE_DELETE_ARRAY(sPidls);
    }
    else
    {
        if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
        {
            aPoint.x = sWindowRect.left;
            aPoint.y = sWindowRect.top;
        }

        BCMenu sMenu;
        if (sMenu.LoadMenu(IDR_SEARCH_RESULT) == XPR_TRUE)
        {
            BCMenu *sPopupMenu = (BCMenu*)sMenu.GetSubMenu(0);
            if (XPR_IS_NOT_NULL(sPopupMenu))
            {
                sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
            }
        }
    }
}

xpr_bool_t SearchResultCtrl::invokeCommandSelf(fxb::ContextMenu *aContextMenu, xpr_uint_t aId)
{
    if (XPR_IS_NULL(aContextMenu))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sIndex = GetSelectionMark();

    if (aId == CMID_OPEN_PARENT_FOLDER)
    {
        SrItemData *sSrItemData = (SrItemData *)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sSrItemData))
        {
            xpr_tchar_t sSelPath[XPR_MAX_PATH + 1] = {0};
            sSrItemData->getPath(sSelPath);

            if (XPR_IS_NOT_NULL(mObserver))
            {
                mObserver->onExplore(*this, sSrItemData->mDir, sSelPath);
            }
        }
    }
    else
    {
        xpr_tchar_t sVerb[0xff] = {0};
        aContextMenu->getCommandVerb(aId, sVerb, 0xfe);

        if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
        {
            SrItemData *sSrItemData = (SrItemData *)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sSrItemData) && XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                execute(sIndex);
                sResult = XPR_TRUE;
            }
        }
        else if (_tcsicmp(sVerb, CMID_VERB_RENAME) == 0)
        {
            gFrame->executeCommand(ID_FILE_RENAME);
            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

void SearchResultCtrl::OnBegindrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    beginDragDrop(sNmListView);
}

void SearchResultCtrl::OnBeginrdrag(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;

    beginDragDrop(sNmListView);
}

xpr_bool_t SearchResultCtrl::setDataObject(LPDATAOBJECT *aDataObject, xpr_uint_t SFGAO_TYPE)
{
    xpr_sint_t sSelCount = GetSelectedCount();
    if (sSelCount <= 0)
        return XPR_FALSE;

    LPITEMIDLIST *sPidls = new LPITEMIDLIST[sSelCount];
    if (XPR_IS_NULL(sPidls))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sCount = 0;
    LPSHELLFOLDER sShellFolder = XPR_NULL;

    if (getSelPidls(&sShellFolder, sPidls, sCount) == XPR_TRUE)
    {
        if (sCount > 0)
        {
            HRESULT sHResult;
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
    }

    for (xpr_sint_t i = 0; i < sCount; ++i)
    {
        COM_FREE(sPidls[i]);
    }

    COM_RELEASE(sShellFolder);
    XPR_SAFE_DELETE_ARRAY(sPidls);

    return sResult;
}

void SearchResultCtrl::beginDragDrop(NM_LISTVIEW *aNmListView)
{
    LPDATAOBJECT sDataObject = XPR_NULL;
    DWORD sDropEffect;

    if (setDataObject(&sDataObject, XPR_NULL) == XPR_TRUE)
    {
        LPDROPSOURCE sDataSource = new DropSource();
        if (XPR_IS_NOT_NULL(sDataSource))
        {
            IDragSourceHelper *sDragSourceHelper = XPR_NULL;

            if (XPR_IS_FALSE(gOpt->mDragNoContents))
            {
                ::CoCreateInstance(
                    CLSID_DragDropHelper,
                    XPR_NULL,
                    CLSCTX_INPROC_SERVER,
                    IID_IDragSourceHelper,
                    (LPVOID *)&sDragSourceHelper);
            }

            if (XPR_IS_NOT_NULL(sDragSourceHelper))
            {
                CPoint sPoint(-20,0);
                GetCursorPos(&sPoint);
                ScreenToClient(&sPoint);

                HRESULT sHResult;
                sHResult = sDragSourceHelper->InitializeFromWindow(GetSafeHwnd(), &sPoint, sDataObject);
            }

            ::DoDragDrop(
                sDataObject,
                sDataSource,
                DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK,
                &sDropEffect);

            COM_RELEASE(sDragSourceHelper);
        }

        COM_RELEASE(sDataSource);
    }

    COM_RELEASE(sDataObject);
}

void SearchResultCtrl::executeSelFolder(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (mObserver->onExplore(*this, aPath, XPR_NULL) == XPR_FALSE)
        {
            doExecuteError(aPath);
        }
    }
}

void SearchResultCtrl::executeLink(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (fxb::GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath) == XPR_FALSE)
        return;

    xpr_tchar_t *sExt = (xpr_tchar_t *)fxb::GetFileExt(sPath);
    if (XPR_IS_NOT_NULL(sExt) && _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
    {
        executeUrl(aShellFolder, aPidl);
    }
    else
    {
        LPITEMIDLIST sFullPidl = XPR_NULL;
        xpr_tchar_t sWorking[XPR_MAX_PATH + 1] = {0};

        if (fxb::ResolveShortcut(GetSafeHwnd(), sPath, &sFullPidl, XPR_NULL, sWorking) == XPR_TRUE && XPR_IS_NOT_NULL(sFullPidl))
        {
            HRESULT sHResult;

            LPSHELLFOLDER sShellFolder2 = XPR_NULL;
            LPITEMIDLIST sPidl2 = XPR_NULL;

            sHResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder2, (LPCITEMIDLIST *)&sPidl2);
            if (SUCCEEDED(sHResult))
            {
                xpr_ulong_t sShellAttributes = SFGAO_FOLDER | SFGAO_FILESYSTEM;
                sShellAttributes = fxb::GetItemAttributes(sShellFolder2, sPidl2);

                xpr_bool_t sSucceeded = XPR_TRUE;

                // check for broken link file path
                if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
                {
                    xpr_tchar_t sLink[XPR_MAX_PATH + 1] = {0};
                    fxb::GetName(sShellFolder2, sPidl2, SHGDN_FORPARSING, sLink);

                    if (fxb::IsExistFile(sLink) == XPR_FALSE)
                        sSucceeded = XPR_FALSE;
                }

                if (XPR_IS_TRUE(sSucceeded))
                {
                    if (XPR_TEST_BITS(sShellAttributes, SFGAO_FOLDER)) // case 2: Folder Link
                        executeLinkFolder(sFullPidl);
                    else // case 3: File Link
                        fxb::ExecFile(sPidl2, sWorking);
                }
                else
                {
                    doExecuteError(sShellFolder2, sPidl2);
                }
            }

            COM_RELEASE(sShellFolder2);
        }

        COM_FREE(sFullPidl);
    }
}

void SearchResultCtrl::executeUrl(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    HRESULT sHResult;
    IUniformResourceLocator *sUniformResourceLocator = XPR_NULL;

    sHResult = aShellFolder->GetUIObjectOf(
        m_hWnd,
        1,
        (LPCITEMIDLIST *)&aPidl,
        IID_IUniformResourceLocator,
        XPR_NULL,
        reinterpret_cast<LPVOID *>(&sUniformResourceLocator));

    xpr_tchar_t *sUrl = XPR_NULL;
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sUniformResourceLocator))
    {
        sHResult = sUniformResourceLocator->GetURL(&sUrl);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sUrl))
            fxb::NavigateURL(sUrl);
    }

    COM_RELEASE(sUniformResourceLocator);
    COM_FREE(sUrl);
}

void SearchResultCtrl::executeLinkFolder(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (mObserver->onExplore(*this, aFullPidl) == XPR_FALSE)
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1];
            fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

            doExecuteError(sPath);
        }
    }
}

void SearchResultCtrl::doExecuteError(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    fxb::GetName(aShellFolder, aPidl, SHGDN_FORPARSING, sPath);

    doExecuteError(sPath);
}

void SearchResultCtrl::doExecuteError(const xpr_tchar_t *aPath)
{
    xpr_tchar_t sMsg[XPR_MAX_PATH * 2] = {0};
    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("search_result.msg.wrong_path"), XPR_STRING_LITERAL("%s")), aPath);
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
}

void SearchResultCtrl::executeAllSelItems(void)
{
    xpr_sint_t sIndex;
    POSITION sPosition = GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = GetNextSelectedItem(sPosition);

        execute(sIndex);
    }
}

void SearchResultCtrl::execute(xpr_sint_t aIndex)
{
    // case 1: open folder
    // case 2: open folder linke
    // case 3: open file link
    // case 4: open url
    // case 5: open file

    if (aIndex == -1)
    {
        aIndex = GetSelectionMark();
    }

    SrItemData *sSrItemData = (SrItemData *)GetItemData(aIndex);
    if (XPR_IS_NULL(sSrItemData))
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    sSrItemData->getPath(sPath);

    LPITEMIDLIST sFullPidl = fxb::IL_CreateFromPath(sPath);
    if (XPR_IS_NULL(sFullPidl))
        return;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;
    HRESULT sHResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);
    if (FAILED(sHResult))
    {
        COM_FREE(sFullPidl);
        return;
    }

    xpr_ulong_t sShellAttributes = 0xFFFFFFFF;
    fxb::GetItemAttributes(sShellFolder, sPidl, sShellAttributes);

    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    fxb::GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);

    // check for ZIP file on Windows XP or higher.
    xpr_bool_t sFolder = XPR_FALSE;
    if (sShellAttributes & SFGAO_FOLDER)
    {
        sFolder = XPR_TRUE;
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_FILESYSTEM))
        {
            WIN32_FIND_DATA sWin32FindData = {0};
            HRESULT sHResult = SHGetDataFromIDList(sShellFolder, sPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
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
        executeSelFolder(sPath);
    }
    else
    {
        if (XPR_TEST_BITS(sShellAttributes, SFGAO_LINK))
        {
            executeLink(sShellFolder, sPidl);
        }
        else
        {
            fxb::ExecFile(sFullPidl);
        }
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);
}

void SearchResultCtrl::OnItemActivate(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMITEMACTIVATE sNmItemActivate = (LPNMITEMACTIVATE)aNmHdr;
    *aResult = 0;

    if (GetSelectedCount() <= 0)
        return;

    execute(sNmItemActivate->iItem);
}

xpr_sint_t SearchResultCtrl::OnMouseActivate(CWnd *aDesktopWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage) 
{
    CPoint sPoint;
    GetCursorPos(&sPoint);
    ScreenToClient(&sPoint);

    xpr_sint_t sIndex = HitTest(sPoint);
    if (sIndex >= 0)
        return MA_NOACTIVATE;

    return super::OnMouseActivate(aDesktopWnd, aHitTest, aMessage);
}

xpr_bool_t SearchResultCtrl::PreTranslateMessage(MSG *aMsg) 
{
    static xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
    sClassName[0] = '\0';

    // Skip Edit Control
    static const xpr_tchar_t *sEditClassName = XPR_STRING_LITERAL("Edit");
    ::GetClassName(aMsg->hwnd, sClassName, MAX_CLASS_NAME);
    if (_tcsicmp(sClassName, sEditClassName) == 0)
        return XPR_FALSE;

    if (aMsg->message == WM_KEYDOWN)
    {
        switch (aMsg->wParam)
        {
        case VK_RETURN:
            if (GetAsyncKeyState(VK_CONTROL) < 0)
                gFrame->executeCommand(ID_FILE_EXEC_PARAM);
            else
                gFrame->executeCommand(ID_FILE_EXEC);
            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(aMsg);
}

void SearchResultCtrl::OnHScroll(xpr_uint_t aSBCode, xpr_uint_t aPos, CScrollBar *aScrollBar) 
{
    super::OnHScroll(aSBCode, aPos, aScrollBar);
}

xpr_bool_t SearchResultCtrl::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo) 
{
    return super::OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
}

void SearchResultCtrl::renameDirectly(xpr_sint_t aIndex)
{
    mMouseEdit = XPR_FALSE;

    EditLabel(aIndex);
}

void SearchResultCtrl::setChangeNotify(xpr_bool_t aNotify)
{
    mNotify = aNotify;
}

LRESULT SearchResultCtrl::OnShellChangeNotify(WPARAM wParam, LPARAM lParam)
{
    fxb::Shcn *sShcn = (fxb::Shcn *)wParam;
    xpr_slong_t sEventId = (xpr_slong_t)lParam;

    if (XPR_IS_NULL(sShcn))
        return 0;

    if (mNotify)
    {
        switch (sEventId)
        {
        case SHCNE_RENAMEFOLDER:
        case SHCNE_RENAMEITEM:
            OnShcnRename(sShcn);
            break;

        case SHCNE_DELETE:
        case SHCNE_RMDIR:
        case SHCNE_UPDATEDIR:
            OnShcnUpdateDir(sShcn);
            break;

        default:
            break;
        }
    }

    XPR_SAFE_DELETE(sShcn);

    return 0;
}

void SearchResultCtrl::OnShcnRename(fxb::Shcn *aShcn)
{
    xpr_tchar_t sPath1[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sPath2[XPR_MAX_PATH + 1] = {0};

    fxb::GetName(aShcn->mPidl1, SHGDN_FORPARSING, sPath1);
    fxb::GetName(aShcn->mPidl2, SHGDN_FORPARSING, sPath2);

    xpr_sint_t i;
    xpr_sint_t sCount;
    SrItemData *sSrItemData;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    xpr_tchar_t sDir[XPR_MAX_PATH + 1];
    xpr_tchar_t *sFindDir;
    WIN32_FIND_DATA sWin32FindData = {0};
    HANDLE sFindFile;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSrItemData = (SrItemData *)GetItemData(i);
        if (XPR_IS_NULL(sSrItemData))
            continue;

        sSrItemData->getPath(sPath);
        if (_tcsicmp(sPath, sPath1) == 0)
        {
            sFindDir = _tcsrchr(sPath2, XPR_STRING_LITERAL('\\'));
            if (XPR_IS_NOT_NULL(sFindDir))
            {
                sFindDir[0] = XPR_STRING_LITERAL('\0');
                _tcscpy(sDir, sPath2);
                sFindDir[0] = XPR_STRING_LITERAL('\\');

                if (_tcsicmp(sSrItemData->mDir, sDir) == 0)
                {
                    SetRedraw(XPR_FALSE);

                    DeleteItem(i);

                    sFindFile = ::FindFirstFile(sPath2, &sWin32FindData);
                    if (sFindFile != INVALID_HANDLE_VALUE)
                    {
                        insertList(i, sDir, &sWin32FindData);

                        ::FindClose(sFindFile);
                    }

                    SetRedraw();
                    break;
                }
            }
        }
    }
}

void SearchResultCtrl::OnShcnUpdateDir(fxb::Shcn *aShcn)
{
    refresh();
}

void SearchResultCtrl::refresh(void)
{
    xpr_sint_t i, sCount;
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    SrItemData *sSrItemData = XPR_NULL;
    xpr_bool_t sChangedItemCount = XPR_FALSE;
    WIN32_FIND_DATA sWin32FindData;
    HANDLE sFindFile;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSrItemData = (SrItemData *)GetItemData(i);
        if (XPR_IS_NULL(sSrItemData))
            continue;

        sSrItemData->getPath(sPath);

        // check file exist
        if (fxb::IsExistFile(sPath) == XPR_FALSE)
        {
            DeleteItem(i--);
            sCount--;

            sChangedItemCount = XPR_TRUE;
        }

        // check text case sensitivity
        sFindFile = ::FindFirstFile(sPath, &sWin32FindData);
        if (sFindFile != INVALID_HANDLE_VALUE)
        {
            if (_tcscmp(sSrItemData->mFileName, sWin32FindData.cFileName) != 0)
            {
                _tcscpy(sSrItemData->mFileName, sWin32FindData.cFileName);

                getDispFileName(sSrItemData->mFileName, sText);

                SetItemText(i, 0, sText);
            }

            ::FindClose(sFindFile);
        }
    }

    if (XPR_IS_TRUE(sChangedItemCount))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onUpdatedResultInfo(*this);
        }
    }
}

void SearchResultCtrl::OnBeginlabeledit(NMHDR *aNmHdr, LRESULT *aResult)
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO *)aNmHdr;
    *aResult = 0;

    if (XPR_IS_FALSE(gOpt->mRenameByMouse) && XPR_IS_TRUE(mMouseEdit))
    {
        *aResult = 1;
        mMouseEdit = XPR_TRUE;
        return;
    }

    mMouseEdit = XPR_TRUE;

    XPR_SAFE_DELETE_ARRAY(mEditExt);
    mEditSel = -1;

    SrItemData *sSrItemData = (SrItemData *)sLvDispInfo->item.lParam;
    if (XPR_IS_NULL(sSrItemData))
    {
        *aResult = 1;
        return;
    }

    xpr_bool_t sCtrlKey = GetAsyncKeyState(VK_CONTROL) < 0;

    if (XPR_IS_FALSE(sCtrlKey) &&
        gOpt->mRenameExtType > RENAME_EXT_TYPE_DEFAULT &&
        !XPR_TEST_BITS(sSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
    {
        xpr_tchar_t sEditName[XPR_MAX_PATH + 1] = {0};
        _tcscpy(sEditName, sSrItemData->mFileName);

        xpr_tchar_t *sExt = (xpr_tchar_t *)fxb::GetFileExt(sEditName);
        if (XPR_IS_NOT_NULL(sExt))
        {
            if (gOpt->mRenameExtType == RENAME_EXT_TYPE_KEEP)
            {
                XPR_SAFE_DELETE_ARRAY(mEditExt);
                mEditExt = new xpr_tchar_t[_tcslen(sExt)+1];
                _tcscpy(mEditExt, sExt);

                sExt[0] = XPR_STRING_LITERAL('\0');

                CEdit *sEditCtrl = GetEditControl();
                if (XPR_IS_NOT_NULL(sEditCtrl))
                    sEditCtrl->SetWindowText(sEditName);
            }
            else
            {
                mEditSel = (xpr_sint_t)(sExt - sEditName);
                SetTimer(TM_ID_RENAME_VISTA, 0, XPR_NULL);
            }
        }
    }

    *aResult = 0;
}

void SearchResultCtrl::OnEndlabeledit(NMHDR *aNmHdr, LRESULT *aResult)
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO*)aNmHdr;
    *aResult = 0;

    xpr_sint_t sIndex = sLvDispInfo->item.iItem;
    xpr_tchar_t *sNewFileName = sLvDispInfo->item.pszText;

    if (sIndex < 0 || XPR_IS_NULL(sNewFileName))
        return;

    xpr_tchar_t sNewName[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sNewName, sNewFileName);
    if (XPR_IS_NOT_NULL(mEditExt))
    {
        _tcscat(sNewName, mEditExt);
        XPR_SAFE_DELETE_ARRAY(mEditExt);
    }
    mEditSel = -1;

    SrItemData *sOldSrItemData = (SrItemData *)GetItemData(sIndex);
    if (XPR_IS_NOT_NULL(sOldSrItemData))
    {
        xpr_tchar_t sOldPath[XPR_MAX_PATH + 1];
        sOldSrItemData->getPath(sOldPath);

        xpr_tchar_t *sSplit = _tcsrchr(sOldPath, '\\');
        if (XPR_IS_NOT_NULL(sSplit))
        {
            *sSplit = '\0';

            SrItemData *sNewSrItemData = new SrItemData;

            sNewSrItemData->mDir = new xpr_tchar_t[_tcslen(sOldPath) + 3];
            _tcscpy(sNewSrItemData->mDir, sOldPath);
            if (_tcslen(sNewSrItemData->mDir) == 2)
                _tcscat(sNewSrItemData->mDir, XPR_STRING_LITERAL("\\"));

            sNewSrItemData->mFileName = new xpr_tchar_t[_tcslen(sNewName) + 1];
            _tcscpy(sNewSrItemData->mFileName, sNewName);

            xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sNewPath, XPR_STRING_LITERAL("%s\\%s"), sOldPath, sNewName);

            ::MoveFile(sOldPath, sNewPath);

            WIN32_FIND_DATA sWin32FindData = {0};
            HANDLE sFindFile = ::FindFirstFile(sNewPath, &sWin32FindData);
            if (sFindFile == INVALID_HANDLE_VALUE)
            {
                sNewSrItemData->mFileSize = (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max + sWin32FindData.nFileSizeLow;
                sNewSrItemData->mModifiedFileTime = sWin32FindData.ftLastWriteTime;
                sNewSrItemData->mFileAttributes = sWin32FindData.dwFileAttributes;

                *sSplit = '\\';

                if (::MoveFile(sOldPath, sNewPath) == XPR_TRUE)
                {
                    // item data
                    SetItemData(sIndex, (DWORD_PTR)sNewSrItemData);

                    // type name
                    xpr_tchar_t sTypeName[XPR_MAX_PATH + 1] = {0};
                    fxb::GetFileType(sNewPath, sTypeName);
                    SetItemText(sIndex, 3, sTypeName);

                    // icon index
                    LVITEM sLvItem = {0};
                    sLvItem.mask     = LVIF_IMAGE;
                    sLvItem.iItem    = sIndex;
                    sLvItem.iSubItem = 0;
                    sLvItem.iImage   = fxb::GetItemIconIndex(sNewPath);
                    SetItem(&sLvItem);

                    setChangeNotify(XPR_FALSE);

                    DWORD sEventId = SHCNE_RENAMEITEM;
                    if (XPR_TEST_BITS(sOldSrItemData->mFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                        sEventId = SHCNE_RENAMEFOLDER;
                    ::SHChangeNotify(sEventId, SHCNF_PATH | SHCNF_FLUSH, sOldPath, sNewPath);

                    setChangeNotify();

                    XPR_SAFE_DELETE_ARRAY(sOldSrItemData->mDir);
                    XPR_SAFE_DELETE_ARRAY(sOldSrItemData->mFileName);
                    XPR_SAFE_DELETE_ARRAY(sOldSrItemData);

                    sNewSrItemData = XPR_NULL;
                    *aResult = 1;
                }
            }
            else
            {
                SetItemData(sIndex, (DWORD_PTR)sNewSrItemData);

                xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
                getDispFileName(sNewSrItemData->mFileName, sText);

                SetItemText(sIndex, 0, sText);

                ::FindClose(sFindFile);

                sNewSrItemData = XPR_NULL;
                *aResult = 1;
            }

            if (XPR_IS_NOT_NULL(sNewSrItemData))
            {
                XPR_SAFE_DELETE_ARRAY(sNewSrItemData->mDir);
                XPR_SAFE_DELETE_ARRAY(sNewSrItemData->mFileName);
                XPR_SAFE_DELETE(sNewSrItemData);
            }
        }
    }
}

void SearchResultCtrl::OnTimer(xpr_uint_t aIdEvent)
{
    if (aIdEvent == TM_ID_RENAME_VISTA)
    {
        KillTimer(aIdEvent);

        CEdit *sEditCtrl = GetEditControl();
        if (XPR_IS_NOT_NULL(sEditCtrl) && XPR_IS_NOT_NULL(sEditCtrl->m_hWnd))
        {
            if (mEditSel >= 0)
            {
                sEditCtrl->SetSel(0, mEditSel);
            }
        }
    }

    super::OnTimer(aIdEvent);
}

void SearchResultCtrl::sortItems(xpr_sint_t aColumn)
{
    mSortAscending = (aColumn == mSortColumn) ? !mSortAscending : XPR_TRUE;
    sortItems(aColumn, mSortAscending);
}

void SearchResultCtrl::sortItems(xpr_sint_t aColumn, xpr_bool_t aAscending)
{
    if (gOpt->mExplorerNoSort == XPR_TRUE)
        return;

    mSortColumn = aColumn;
    mSortAscending = aAscending;

    SortItems(DefaultItemCompareProc, MAKELONG(mSortColumn, mSortAscending));

    if (XPR_IS_NOT_NULL(mHeaderCtrl))
        mHeaderCtrl->setSortImage(mSortColumn, mSortAscending);
}

void SearchResultCtrl::OnColumnclick(NMHDR *aNmHdr, LRESULT *aResult) 
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;

    xpr_sint_t sColumn = sNmListView->iSubItem;
    sortItems(sColumn);

    *aResult = 0;
}

xpr_sint_t CALLBACK SearchResultCtrl::DefaultItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    xpr_sint_t sResult = 0;

    SrItemData *sSrItemData1 = (SrItemData *)lParam1;
    SrItemData *sSrItemData2 = (SrItemData *)lParam2;
    xpr_bool_t sAscending = HIWORD(lParamSort);
    xpr_sint_t sColumn = LOWORD(lParamSort);

    if (sColumn == 0)
    {
        if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
            if (sResult == 0)
                sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
        }
        else if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 1)
    {
        sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
        if (sResult == 0)
        {
            if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
            }
            else if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = -1;
            }
            else if (((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = 1;
            }
        }
    }
    else if (sColumn == 2)
    {
        if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if (sSrItemData1->mFileSize > sSrItemData2->mFileSize)
                return -1;
            else if (sSrItemData1->mFileSize < sSrItemData2->mFileSize)
                return 1;
            else
            {
                sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
            }
        }
        else if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 3)
    {
        if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
            }
            else
            {
                xpr_tchar_t *sExt1 = (xpr_tchar_t *)fxb::GetFileExt(sSrItemData1->mFileName);
                xpr_tchar_t *sExt2 = (xpr_tchar_t *)fxb::GetFileExt(sSrItemData2->mFileName);

                if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2))
                {
                    sResult = lstrcmpi(sExt1, sExt2);
                    if (sResult == 0)
                    {
                        sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
                        if (sResult == 0)
                            sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
                    }
                }
                else if (XPR_IS_NULL(sExt1) && XPR_IS_NOT_NULL(sExt2))
                    sResult = -1;
                else if (XPR_IS_NOT_NULL(sExt1) && XPR_IS_NULL(sExt2))
                    sResult = 1;
            }
        }
        else if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }
    else if (sColumn == 4)
    {
        if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = ::CompareFileTime(&sSrItemData1->mModifiedFileTime, &sSrItemData2->mModifiedFileTime);
            if (sResult == 0)
            {
                sResult = lstrcmpi(sSrItemData1->mFileName, sSrItemData2->mFileName);
                if (sResult == 0)
                    sResult = lstrcmpi(sSrItemData1->mDir, sSrItemData2->mDir);
            }
        }
        else if ((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ((sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = -1;
        }
        else if (((sSrItemData1->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) != FILE_ATTRIBUTE_DIRECTORY && (sSrItemData2->mFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = 1;
        }
    }

    return (sResult * (XPR_IS_TRUE(sAscending) ? 1 : -1));
}

void SearchResultCtrl::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (mObserver != XPR_NULL)
    {
        mObserver->onSetFocus(*this);
    }
}

void SearchResultCtrl::OnKeyDown(xpr_uint_t aChar, xpr_uint_t aRepCnt, xpr_uint_t aFlags) 
{
    super::OnKeyDown(aChar, aRepCnt, aFlags);
}

void SearchResultCtrl::OnLvnKeyDown(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_KEYDOWN *sLvKeyDown = (LV_KEYDOWN*)aNmHdr;
    *aResult = 0;
}

LRESULT SearchResultCtrl::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    fxb::ShellIcon::AsyncIcon *sAsyncIcon = (fxb::ShellIcon::AsyncIcon *)wParam;
    if (XPR_IS_NULL(sAsyncIcon))
        return 0;

    if (sAsyncIcon->mCode == mCode)
    {
        xpr_sint_t sIndex = -1;
        SrItemData *sSrItemData = XPR_NULL;

        if (sAsyncIcon->mItem != -1 && sAsyncIcon->mItem >= 0)
        {
            sSrItemData = (SrItemData *)GetItemData((xpr_sint_t)sAsyncIcon->mItem);
            if (XPR_IS_NOT_NULL(sSrItemData) && sSrItemData->mSignature == sAsyncIcon->mSignature)
                sIndex = (xpr_sint_t)sAsyncIcon->mItem;
        }

        if (sIndex < 0)
        {
            sIndex = findItemSignature(sAsyncIcon->mSignature);
            if (sIndex >= 0)
                sSrItemData = (SrItemData *)GetItemData(sIndex);
        }

        if (sIndex >= 0 && XPR_IS_NOT_NULL(sSrItemData))
        {
            switch (sAsyncIcon->mType)
            {
            case fxb::ShellIcon::TypeIconIndex:
                {
                    LVITEM sLvItem = {0};
                    sLvItem.mask   = LVIF_IMAGE;
                    sLvItem.iItem  = sIndex;
                    sLvItem.iImage = sAsyncIcon->mResult.mIconIndex;
                    SetItem(&sLvItem);
                    break;
                }

            case fxb::ShellIcon::TypeOverlayIndex:
                {
                    xpr_uint_t sState = 0;

                    if (sAsyncIcon->mResult.mIconIndex >= 0)
                        sState = INDEXTOOVERLAYMASK(sAsyncIcon->mResult.mIconIndex);
                    else
                    {
                        std::tstring sPath;
                        sSrItemData->getPath(sPath);

                        const xpr_tchar_t *sExt = fxb::GetFileExt(sPath);
                        if (XPR_IS_NOT_NULL(sExt))
                        {
                            if (_tcsicmp(sExt, XPR_STRING_LITERAL(".lnk")) == 0 || _tcsicmp(sExt, XPR_STRING_LITERAL(".url")) == 0)
                                sState = INDEXTOOVERLAYMASK(2);
                        }
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

xpr_sint_t SearchResultCtrl::findItemSignature(xpr_uint_t aSignature)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    SrItemData *sSrItemData;

    sCount = GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSrItemData = (SrItemData *)GetItemData(i);
        if (XPR_IS_NULL(sSrItemData))
            continue;

        if (sSrItemData->mSignature == aSignature)
            return i;
    }

    return -1;
}

void SearchResultCtrl::selectAll(void)
{
    xpr_sint_t sCount = GetItemCount();

    xpr_sint_t nFocus = GetSelectionMark();
    for (xpr_sint_t i = 0; i < sCount; ++i)
        SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

    SetItemState(nFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void SearchResultCtrl::unselectAll(void)
{
    xpr_sint_t sCount = GetItemCount();
    for (xpr_sint_t i = 0; i < sCount; ++i)
        SetItemState(i, 0, LVIS_SELECTED);
}
