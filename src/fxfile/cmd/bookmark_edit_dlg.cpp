//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "bookmark_edit_dlg.h"

#include "shell_icon.h"
#include "resource.h"
#include "option.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "main_frame.h"
#include "bookmark_item_edit_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
// user defined message
enum
{
    WM_SHELL_ASYNC_ICON    = WM_USER + 11,
    WM_BOOKMARK_ASYNC_ICON = WM_USER + 100,
};

BookmarkEditDlg::BookmarkEditDlg(void)
    : super(IDD_BOOKMARK_EDIT, XPR_NULL)
    , mIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME))
    , mShellIcon(XPR_NULL)
    , mDlgState(XPR_NULL)
{
}

BookmarkEditDlg::~BookmarkEditDlg(void)
{
    XPR_SAFE_DELETE(mShellIcon);
    DESTROY_ICON(mIcon);
}

void BookmarkEditDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BOOKMARK_EDIT_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(BookmarkEditDlg, super)
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_NOTIFY(LVN_DELETEITEM, IDC_BOOKMARK_EDIT_LIST, OnDeleteitemList)
    ON_NOTIFY(NM_DBLCLK,      IDC_BOOKMARK_EDIT_LIST, OnDblclkList)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_ITEM_ADD,     OnItemAdd)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_ITEM_MODIFY,  OnItemModify)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_ITEM_DELETE,  OnItemDelete)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_ITEM_UP,      OnItemMoveUp)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_ITEM_DOWN,    OnItemMoveDown)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_QUICK_LAUNCH, OnQuickLaunch)
    ON_BN_CLICKED(IDC_BOOKMARK_EDIT_DEFAULT,      OnDefault)
    ON_MESSAGE(WM_BOOKMARK_ASYNC_ICON, OnBookmarkAsyncIcon)
    ON_MESSAGE(WM_SHELL_ASYNC_ICON,    OnShellAsyncIcon)
END_MESSAGE_MAP()

xpr_bool_t BookmarkEditDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_BOOKMARK_EDIT_LIST,         sizeResize, sizeResize);
    AddControl(IDC_BOOKMARK_EDIT_ITEM_UP,      sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_ITEM_DOWN,    sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_ITEM_ADD,     sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_ITEM_MODIFY,  sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_ITEM_DELETE,  sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_QUICK_LAUNCH, sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_DEFAULT,      sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_BOOKMARK_EDIT_STATUS,       sizeResize, sizeRepos);
    AddControl(IDC_BOOKMARK_EDIT_LINE,         sizeResize, sizeRepos);
    AddControl(IDOK,                           sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                       sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    DWORD sExStyle = mListCtrl.GetExtendedStyle();
    sExStyle |= LVS_EX_FULLROWSELECT;
    sExStyle |= LVS_EX_INFOTIP;
    mListCtrl.SetExtendedStyle(sExStyle);

    mImgList.Create(16, 16, ILC_COLOR16 | ILC_MASK, -1, -1);
    mListCtrl.SetImageList(&mImgList, LVSIL_SMALL);

    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.list.column.name")),        LVCFMT_LEFT,  80, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.list.column.file_folder")), LVCFMT_LEFT, 197, -1);
    mListCtrl.InsertColumn(2, gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.list.column.hotkey")),      LVCFMT_LEFT,  50, -1);

    BookmarkMgr &theBookmarkMgr = BookmarkMgr::instance();
    theBookmarkMgr.regAsyncIcon(*this, WM_BOOKMARK_ASYNC_ICON);

    xpr_sint_t i;
    xpr_sint_t sCount;
    BookmarkItem *sBookmarkItem;
    BookmarkItem *sBookmarkItem2;

    sCount = theBookmarkMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = theBookmarkMgr.getBookmark(i);

        sBookmarkItem2 = new BookmarkItem;
        *sBookmarkItem2 = *sBookmarkItem;

        addBookmark(sBookmarkItem2, IconReqBookmarkMgr, -1, i);
    }

    if (mListCtrl.GetItemCount() > 0)
        setItemFocus(0);

    updateStatus();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.title")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_LABEL_LIST,   gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.label.list")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_ITEM_ADD,     gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.add")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_ITEM_MODIFY,  gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.modify")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_ITEM_DELETE,  gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.delete")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_ITEM_UP,      gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.move_up")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_ITEM_DOWN,    gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.move_down")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_QUICK_LAUNCH, gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.quick_launch")));
    SetDlgItemText(IDC_BOOKMARK_EDIT_DEFAULT,      gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.button.default")));
    SetDlgItemText(IDOK,                           gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BookmarkEdit"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void BookmarkEditDlg::OnDestroy(void)
{
    BookmarkMgr::instance().unregAsyncIcon(*this);

    super::OnDestroy();

    if (mShellIcon != XPR_NULL)
        mShellIcon->Stop(1000);

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = sCount - 1; i >= 0; --i)
        mListCtrl.DeleteItem(i);

    mImgList.DeleteImageList();
    DESTROY_ICON(mIcon);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void BookmarkEditDlg::addBookmark(BookmarkItem *aBookmarkItem, IconReq aIconReq, xpr_sint_t aIndex, xpr_sint_t aBookmarkIndex)
{
    if (aBookmarkItem == XPR_NULL)
        return;

    if (aIndex == -1)
        aIndex = mListCtrl.GetItemCount();

    // HotKey
    xpr_tchar_t sHotKey[0xff] = {0};
    if (aBookmarkItem->mHotKey > 0)
        _stprintf(sHotKey, XPR_STRING_LITERAL("Ctrl+%c"), LOWORD(aBookmarkItem->mHotKey));

    // Image List
    xpr_sint_t sImage = -1;
    if (_tcscmp(aBookmarkItem->mPath.c_str(), BOOKMARK_SEPARATOR) != 0)
    {
        HICON sIcon = aBookmarkItem->getIcon();
        if (sIcon == XPR_NULL)
        {
            if (aIconReq == IconReqBookmarkMgr)
            {
                BookmarkMgr::instance().getIcon(aBookmarkIndex, sIcon);
            }
            else if (aIconReq == IconReqShellIcon)
            {
                if (mShellIcon == XPR_NULL)
                {
                    mShellIcon = new ShellIcon;
                    mShellIcon->setOwner(*this, WM_SHELL_ASYNC_ICON);
                }

                ShellIcon::AsyncIcon *sAsyncIcon = new ShellIcon::AsyncIcon;
                sAsyncIcon->mType       = ShellIcon::TypeIcon;
                sAsyncIcon->mFlags      = XPR_IS_TRUE(gOpt->mConfig.mBookmarkFastNetIcon) ? ShellIcon::FlagFastNetIcon : ShellIcon::FlagNone;
                sAsyncIcon->mCode       = 0;
                sAsyncIcon->mItem       = aBookmarkIndex;
                sAsyncIcon->mSignature  = aBookmarkItem->getSignature();
                sAsyncIcon->mPath       = aBookmarkItem->mPath;
                sAsyncIcon->mIconPath   = aBookmarkItem->mIconPath;
                sAsyncIcon->mIconIndex  = aBookmarkItem->mIconIndex;

                if (mShellIcon->getAsyncIcon(sAsyncIcon) == XPR_FALSE)
                {
                    XPR_SAFE_DELETE(sAsyncIcon);
                }
                else
                {
                    aBookmarkItem->setPending(XPR_TRUE);
                }
            }
        }

        if (sIcon != XPR_NULL)
            sImage = mImgList.Add(sIcon);
    }

    LV_ITEM sLvItem = {0};
    sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 0;
    sLvItem.iImage   = sImage;
    sLvItem.pszText  = (xpr_tchar_t *)aBookmarkItem->mName.c_str();
    sLvItem.lParam   = (LPARAM)aBookmarkItem;
    mListCtrl.InsertItem(&sLvItem);

    sLvItem.mask     = LVIF_TEXT;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 1;
    sLvItem.pszText  = (xpr_tchar_t *)aBookmarkItem->mPath.c_str();
    mListCtrl.SetItem(&sLvItem);

    sLvItem.mask     = LVIF_TEXT;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 2;
    sLvItem.pszText  = sHotKey;
    mListCtrl.SetItem(&sLvItem);
}

xpr_sint_t BookmarkEditDlg::findSignature(xpr_uint_t aSignature)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    BookmarkItem *sBookmarkItem;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(i);
        if (sBookmarkItem == XPR_NULL)
            continue;

        if (sBookmarkItem->getSignature() == aSignature)
            return i;
    }

    return -1;
}

void BookmarkEditDlg::updateStatus(void)
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.bookmark_edit.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    SetDlgItemText(IDC_BOOKMARK_EDIT_STATUS, sStatusText);
}

xpr_bool_t BookmarkEditDlg::PreTranslateMessage(MSG *pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == mListCtrl.m_hWnd)
    {
        switch (pMsg->wParam)
        {
        case VK_INSERT:
            OnItemAdd();
            return XPR_TRUE;

        case VK_RETURN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemModify();
                return XPR_TRUE;
            }
            break;

        case VK_DELETE:
            OnItemDelete();
            return XPR_TRUE;

        case VK_UP:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemMoveUp();
                return XPR_TRUE;
            }
            break;

        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemMoveDown();
                return XPR_TRUE;
            }
            break;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void BookmarkEditDlg::OnItemAdd(void) 
{
    if (mListCtrl.GetItemCount() > MAX_BOOKMARK)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.bookmark_edit.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_BOOKMARK);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    BookmarkItemEditDlg sDlg;
    if (sDlg.DoModal() == IDOK)
    {
        if (sDlg.mBookmark.mPath.empty() == XPR_FALSE)
        {
            BookmarkItem *sBookmarkItem = new BookmarkItem;

            sDlg.copyBookmark(sBookmarkItem);
            addBookmark(sBookmarkItem, IconReqShellIcon);

            setItemFocus(mListCtrl.GetItemCount() - 1);
        }
    }

    updateStatus();
}

void BookmarkEditDlg::OnItemModify(void) 
{
    if (mListCtrl.GetSelectedCount() > 0)
    {
        xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
        BookmarkItem *sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(sIndex);

        BookmarkItemEditDlg sDlg;
        sDlg.add(sBookmarkItem);
        if (sDlg.DoModal() == IDOK)
        {
            if (sDlg.mBookmark.mPath.empty() == XPR_FALSE)
            {
                sDlg.copyBookmark(sBookmarkItem);

                sBookmarkItem->newSignature();
                sBookmarkItem->destroyIcon();

                mListCtrl.SetItemData(sIndex, XPR_NULL);
                mListCtrl.DeleteItem(sIndex);
                addBookmark(sBookmarkItem, IconReqShellIcon, sIndex);

                setItemFocus(sIndex);
            }
        }
    }

    updateStatus();
}

void BookmarkEditDlg::OnItemDelete(void) 
{
    if (mListCtrl.GetSelectedCount() > 0)
    {
        xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
        mListCtrl.DeleteItem(sIndex);

        xpr_sint_t sCount = mListCtrl.GetItemCount();
        if (sCount > 0)
        {
            if (sIndex >= sCount)
                sIndex = sCount - 1;

            setItemFocus(sIndex);
        }
    }

    updateStatus();
}

void BookmarkEditDlg::OnItemMoveUp(void) 
{
    if (mListCtrl.GetSelectedCount() > 0)
    {
        xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
        if (sIndex == 0)
            return;

        BookmarkItem *sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(sIndex);
        mListCtrl.SetItemData(sIndex, XPR_NULL);
        mListCtrl.DeleteItem(sIndex--);

        addBookmark(sBookmarkItem, IconReqNone, sIndex);

        setItemFocus(sIndex);
    }
}

void BookmarkEditDlg::OnItemMoveDown(void) 
{
    if (mListCtrl.GetSelectedCount() > 0)
    {
        xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
        if (sIndex == (mListCtrl.GetItemCount()-1))
            return;

        BookmarkItem *sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(sIndex);
        mListCtrl.SetItemData(sIndex, XPR_NULL);
        mListCtrl.DeleteItem(sIndex++);

        addBookmark(sBookmarkItem, IconReqNone, sIndex);

        setItemFocus(sIndex);
    }
}

void BookmarkEditDlg::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    BookmarkItem *sBookmarkItem = (BookmarkItem *)pNMListView->lParam;
    XPR_SAFE_DELETE(sBookmarkItem);

    *pResult = 0;
}

void BookmarkEditDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnItemModify();
    *pResult = 0;
}

void BookmarkEditDlg::OnSetFocus(CWnd* pOldWnd) 
{
    super::OnSetFocus(pOldWnd);

    mListCtrl.SetFocus();
}

void BookmarkEditDlg::OnOK(void) 
{
    if (mListCtrl.GetItemCount() > MAX_BOOKMARK)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.bookmark_edit.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_BOOKMARK);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    apply();

    super::OnOK();
}

void BookmarkEditDlg::apply(void)
{
    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

    sBookmarkMgr.clear();

    xpr_sint_t i, sCount;
    BookmarkItem *sBookmarkItem;
    BookmarkItem *sBookmarkItem2;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(i);
        if (sBookmarkItem == XPR_NULL)
            continue;

        sBookmarkItem2 = new BookmarkItem;
        *sBookmarkItem2 = *sBookmarkItem;

        sBookmarkMgr.addBookmark(sBookmarkItem2);
    }

    sBookmarkMgr.save();
}

void BookmarkEditDlg::setItemFocus(xpr_sint_t aIndex)
{
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);

    mListCtrl.SetFocus();
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void BookmarkEditDlg::OnQuickLaunch(void) 
{
    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.msg.question_quick_launch"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    CWaitCursor sWaitCursor;

    HRESULT sHResult;
    xpr_tchar_t sQuickLaunch[XPR_MAX_PATH + 1] = {0};
    sHResult = ::SHGetSpecialFolderPath(XPR_NULL, sQuickLaunch, CSIDL_APPDATA, XPR_FALSE);
    if (FAILED(sHResult))
        return;

    _tcscat(sQuickLaunch, XPR_STRING_LITERAL("\\Microsoft\\Internet Explorer\\Quick Launch"));

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, sQuickLaunch);

    xpr_tchar_t *sFileName = sPath + _tcslen(sPath) + 1;
    _tcscat(sPath, XPR_STRING_LITERAL("\\*.*"));

    WIN32_FIND_DATA sFindData = {0};
    HANDLE sFindHandle = ::FindFirstFile(sPath, &sFindData);
    if (sFindHandle == INVALID_HANDLE_VALUE)
        return;

    xpr_tchar_t *sExt;
    LPITEMIDLIST sFullPidl = XPR_NULL;
    LPITEMIDLIST sResolvedFullPidl = XPR_NULL;
    BookmarkItem *sBookmarkItem;
    xpr_bool_t sResolved;

    do
    {
        if (_tcscmp(sFindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sFindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
            continue;

        _tcscpy(sFileName, sFindData.cFileName);
        sHResult = fxfile::base::Pidl::create(sPath, sFullPidl);
        if (SUCCEEDED(sHResult) && sFullPidl != XPR_NULL)
        {
            sExt = (xpr_tchar_t *)GetFileExt(sFindData.cFileName);
            if (sExt != XPR_NULL && !_tcsicmp(sExt+1, XPR_STRING_LITERAL("lnk")))
            {
                sResolved = ResolveShortcut(GetSafeHwnd(), sPath, &sResolvedFullPidl);
                if (sResolved && sResolvedFullPidl)
                {
                    COM_FREE(sFullPidl);
                    sFullPidl = sResolvedFullPidl;
                }
                else
                {
                    COM_FREE(sResolvedFullPidl);
                }
            }

            sBookmarkItem = new BookmarkItem;

            Pidl2Path(sFullPidl, sBookmarkItem->mPath);
            GetName(sFullPidl, SHGDN_INFOLDER, sBookmarkItem->mName);

            addBookmark(sBookmarkItem, IconReqShellIcon);
        }

        COM_FREE(sFullPidl);
    }
    while (::FindNextFile(sFindHandle, &sFindData) == XPR_TRUE);

    ::FindClose(sFindHandle);

    if (mListCtrl.GetItemCount() > 0)
        setItemFocus(0);

    updateStatus();
}

void BookmarkEditDlg::OnDefault(void)
{
    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.bookmark_edit.msg.question_default"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    CWaitCursor sWaitCursor;

    mListCtrl.DeleteAllItems();

    BookmarkMgr::BookmarkDeque sBookmarkDeque;
    BookmarkMgr::getDefaultBookmark(sBookmarkDeque);

    BookmarkItem *sBookmarkItem;
    BookmarkMgr::BookmarkDeque::iterator sIterator;

    sIterator = sBookmarkDeque.begin();
    for (; sIterator != sBookmarkDeque.end(); ++sIterator)
    {
        sBookmarkItem = *sIterator;
        addBookmark(sBookmarkItem, IconReqShellIcon);
    }

    sBookmarkDeque.clear();

    if (mListCtrl.GetItemCount() > 0)
        setItemFocus(0);

    updateStatus();
}

LRESULT BookmarkEditDlg::OnBookmarkAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    xpr_sint_t sBookmarkIndex = (xpr_sint_t)wParam;
    BookmarkItem *sBookmarkItem = (BookmarkItem *)lParam;

    if (sBookmarkItem == XPR_NULL)
        return 0;

    xpr_sint_t sIndex = -1;
    BookmarkItem *sBookmarkItem2 = XPR_NULL;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (XPR_IS_RANGE(0, sBookmarkIndex, sCount-1))
    {
        sBookmarkItem2 = (BookmarkItem *)mListCtrl.GetItemData(sBookmarkIndex);
        if (sBookmarkItem2 != XPR_NULL && sBookmarkItem2->getSignature() == sBookmarkItem->getSignature())
            sIndex = sBookmarkIndex;
    }

    if (sIndex < 0)
    {
        sIndex = findSignature(sBookmarkItem->getSignature());
        if (sIndex >= 0)
            sBookmarkItem2 = (BookmarkItem *)mListCtrl.GetItemData(sIndex);
    }

    if (sIndex >= 0 && sBookmarkItem2 != XPR_NULL)
    {
        HICON sIcon = sBookmarkItem->getIcon();
        if (sIcon != XPR_NULL)
            sIcon = ::DuplicateIcon(XPR_NULL, sIcon);

        sBookmarkItem2->setIcon(sIcon);

        xpr_sint_t sImage = mImgList.Add(sIcon);

        LVITEM sLvItem = {0};
        sLvItem.mask   = LVIF_IMAGE;
        sLvItem.iItem  = sIndex;
        sLvItem.iImage = sImage;
        mListCtrl.SetItem(&sLvItem);
    }

    return 0;
}

LRESULT BookmarkEditDlg::OnShellAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    ShellIcon::AsyncIcon *sAsyncIcon = (ShellIcon::AsyncIcon *)wParam;
    if (sAsyncIcon == XPR_NULL)
        return 0;

    //if (sAsyncIcon->uCode == m_uCode)
    {
        xpr_sint_t sIndex = -1;
        BookmarkItem *sBookmarkItem = XPR_NULL;

        if (sAsyncIcon->mItem != -1 && sAsyncIcon->mItem >= 0)
        {
            sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData((xpr_sint_t)sAsyncIcon->mItem);
            if (sBookmarkItem != XPR_NULL && sBookmarkItem->getSignature() == sAsyncIcon->mSignature)
                sIndex = (xpr_sint_t)sAsyncIcon->mItem;
        }

        if (sIndex < 0)
        {
            sIndex = findSignature(sAsyncIcon->mSignature);
            if (sIndex >= 0)
                sBookmarkItem = (BookmarkItem *)mListCtrl.GetItemData(sIndex);
        }

        if (sIndex >= 0 && sBookmarkItem != XPR_NULL)
        {
            HICON sIcon = sAsyncIcon->mResult.mIcon;
            if (sIcon == XPR_NULL)
                sIcon = BookmarkMgr::instance().getTypeIcon(BookmarkMgr::IconTypeNot, XPR_TRUE);

            sBookmarkItem->setIcon(sIcon);

            sAsyncIcon->mResult.mIcon = XPR_NULL;

            xpr_sint_t sImage = mImgList.Add(sIcon);

            LVITEM sLvItem = {0};
            sLvItem.mask   = LVIF_IMAGE;
            sLvItem.iItem  = sIndex;
            sLvItem.iImage = sImage;
            mListCtrl.SetItem(&sLvItem);
        }
    }

    XPR_SAFE_DELETE(sAsyncIcon);

    return 0;
}
} // namespace cmd
} // namespace fxfile
