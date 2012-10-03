//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SearchLocDlg.h"

#include "resource.h"
#include "SearchLocEditDlg.h"
#include "InputDlg.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SearchLocDlg::SearchLocDlg(void)
    : super(IDD_SEARCH_LOC, XPR_NULL)
    , mSearchUserLoc(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

SearchLocDlg::~SearchLocDlg(void)
{
    if (mSearchUserLoc != XPR_NULL)
    {
        fxb::SearchUserLoc *sSearchUserLoc;
        fxb::SearchUserLocDeque::iterator sIterator;

        sIterator = mSearchUserLoc->begin();
        for (; sIterator != mSearchUserLoc->end(); ++sIterator)
        {
            sSearchUserLoc = *sIterator;
            if (sSearchUserLoc == XPR_NULL)
                continue;

            sSearchUserLoc->clear();
            XPR_SAFE_DELETE(sSearchUserLoc);
        }

        mSearchUserLoc->clear();
        XPR_SAFE_DELETE(mSearchUserLoc);
    }

    DESTROY_ICON(mIcon);
}

void SearchLocDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEARCH_LOC_ITEM_LIST, mListCtrl);
    DDX_Control(pDX, IDC_SEARCH_LOC_LIST,      mLocComboBox);
    DDX_Control(pDX, IDC_SEARCH_LOC_LIST_NEW,  mNewDropButton);
}

BEGIN_MESSAGE_MAP(SearchLocDlg, super)
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_TIMER()
    ON_WM_CAPTURECHANGED()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_INITMENUPOPUP()
    ON_WM_MENUCHAR()
    ON_COMMAND(IDC_SEARCH_LOC_ITEM_ADD,    OnItemAdd)
    ON_COMMAND(IDC_SEARCH_LOC_ITEM_MODIFY, OnItemModify)
    ON_COMMAND(IDC_SEARCH_LOC_ITEM_DELETE, OnItemDelete)
    ON_COMMAND(IDC_SEARCH_LOC_ITEM_UP,     OnItemMoveUp)
    ON_COMMAND(IDC_SEARCH_LOC_ITEM_DOWN,   OnItemMoveDown)
    ON_COMMAND(IDC_SEARCH_LOC_LIST_NEW,    OnListNew)
    ON_COMMAND(ID_LOC_NEW,                 OnListNew)
    ON_COMMAND(ID_LOC_MODIFY,              OnListModify)
    ON_COMMAND(ID_LOC_DELETE,              OnListDelete)
    ON_CBN_SELCHANGE(IDC_SEARCH_LOC_LIST,               OnCbnSelchangeList)
    ON_NOTIFY(CBEN_DELETEITEM, IDC_SEARCH_LOC_LIST,     OnCbenDeleteitemList)
    ON_NOTIFY(NM_DBLCLK, IDC_SEARCH_LOC_ITEM_LIST,      OnDblclkItemList)
    ON_NOTIFY(LVN_DELETEITEM, IDC_SEARCH_LOC_ITEM_LIST, OnDeleteitemItemList)
END_MESSAGE_MAP()

xpr_bool_t SearchLocDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_SEARCH_LOC_ITEM_LIST,   sizeResize, sizeResize);
    AddControl(IDC_SEARCH_LOC_ITEM_UP,     sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_SEARCH_LOC_ITEM_DOWN,   sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_SEARCH_LOC_ITEM_ADD,    sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_SEARCH_LOC_ITEM_MODIFY, sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_SEARCH_LOC_ITEM_DELETE, sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_SEARCH_LOC_STATUS,      sizeResize, sizeRepos);
    AddControl(IDC_SEARCH_LOC_LINE,        sizeResize, sizeRepos);
    AddControl(IDOK,                       sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                   sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    DWORD sExStyle = mListCtrl.GetExtendedStyle();
    sExStyle |= LVS_EX_FULLROWSELECT;
    sExStyle |= LVS_EX_INFOTIP;
    mListCtrl.SetExtendedStyle(sExStyle);

    xpr_sint_t i;
    HICON sIcon;

    mLocImgList.Create(16, 16, ILC_COLOR16 | ILC_MASK, -1, -1);
    sIcon = theApp.LoadIcon(MAKEINTRESOURCE(IDI_SEARCH));
    mLocImgList.Add(sIcon);
    ::DestroyIcon(sIcon);

    mLocComboBox.SetImageList(&mLocImgList);

    xpr_uint_t sIconIds[] = {
        IDI_SEARCH_INC,
        IDI_SEARCH_EXC,
        IDI_NOT,
        0};

    mListImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    for (i = 0; sIconIds[i]; ++i)
    {
        sIcon = theApp.LoadIcon(MAKEINTRESOURCE(sIconIds[i]));
        mListImgList.Add(sIcon);
        ::DestroyIcon(sIcon);
    }
    mListImgList.SetOverlayImage(0, 1);
    mListImgList.SetOverlayImage(1, 2);

    mListCtrl.SetImageList(&mListImgList, LVSIL_SMALL);

    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.item_list.column.location")),   LVCFMT_LEFT,   200);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.item_list.column.sub-folder")), LVCFMT_CENTER,  70);

    mNewDropButton.LoadMenu(IDR_SEARCH_LOC, 0);

    if (mSearchUserLoc != XPR_NULL)
    {
        fxb::SearchUserLoc *sSearchUserLoc;
        fxb::SearchUserLocDeque::iterator sIterator;

        sIterator = mSearchUserLoc->begin();
        for (; sIterator != mSearchUserLoc->end(); ++sIterator)
        {
            sSearchUserLoc = *sIterator;
            if (sSearchUserLoc == XPR_NULL)
                continue;

            addLoc(sSearchUserLoc);
        }

        mSearchUserLoc->clear();
        XPR_SAFE_DELETE(mSearchUserLoc);
    }

    if (mLocComboBox.GetCount() > 0)
    {
        mLocComboBox.SetCurSel(0);
        OnCbnSelchangeList();
    }

    updateStatus();

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.title")));
    SetDlgItemText(IDC_SEARCH_LOC_LABEL_LIST,      theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.label.list")));
    SetDlgItemText(IDC_SEARCH_LOC_LIST_NEW,        theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.new_list")));
    SetDlgItemText(IDC_SEARCH_LOC_LABEL_ITEM_LIST, theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.label.item_list")));
    SetDlgItemText(IDC_SEARCH_LOC_ITEM_ADD,        theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.add")));
    SetDlgItemText(IDC_SEARCH_LOC_ITEM_MODIFY,     theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.modify")));
    SetDlgItemText(IDC_SEARCH_LOC_ITEM_DELETE,     theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.delete")));
    SetDlgItemText(IDC_SEARCH_LOC_ITEM_UP,         theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.move_up")));
    SetDlgItemText(IDC_SEARCH_LOC_ITEM_DOWN,       theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.button.move_down")));
    SetDlgItemText(IDOK,                           theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    // Load Dialog State
    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("SearchLoc"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

fxb::SearchUserLocDeque *SearchLocDlg::getSearchLoc(xpr_bool_t aNull) const
{
    fxb::SearchUserLocDeque *sSearchUserLoc = mSearchUserLoc;

    if (aNull == XPR_TRUE)
        sSearchUserLoc = XPR_NULL;

    return sSearchUserLoc;
}

void SearchLocDlg::setSearchLoc(fxb::SearchUserLocDeque *aSearchUserLoc)
{
    fxb::SearchUserLoc *sSearchUserLoc;
    fxb::SearchUserLoc *sSearchUserLoc2;
    fxb::SearchUserLocDeque::iterator sIterator;
    fxb::UserLocDeque::iterator sUserLocIterator;
    fxb::SearchUserLocItem *sSearchUserLocItem;
    fxb::SearchUserLocItem *sSearchUserLocItem2;

    if (mSearchUserLoc != XPR_NULL)
    {
        sIterator = mSearchUserLoc->begin();
        for (; sIterator != mSearchUserLoc->end(); ++sIterator)
        {
            sSearchUserLoc = *sIterator;
            if (sSearchUserLoc == XPR_NULL)
                continue;

            sSearchUserLoc->clear();
            XPR_SAFE_DELETE(sSearchUserLoc);
        }

        mSearchUserLoc->clear();
        XPR_SAFE_DELETE(mSearchUserLoc);
    }

    if (aSearchUserLoc == XPR_NULL)
        return;

    mSearchUserLoc = new fxb::SearchUserLocDeque;

    sIterator = aSearchUserLoc->begin();
    for (; sIterator != aSearchUserLoc->end(); ++sIterator)
    {
        sSearchUserLoc = *sIterator;
        if (sSearchUserLoc == XPR_NULL)
            continue;

        sSearchUserLoc2 = new fxb::SearchUserLoc;
        if (sSearchUserLoc2 == XPR_NULL)
            continue;

        sSearchUserLoc2->mName = sSearchUserLoc->mName;

        sUserLocIterator = sSearchUserLoc->mUserLocDeque.begin();
        for (; sUserLocIterator != sSearchUserLoc->mUserLocDeque.end(); ++sUserLocIterator)
        {
            sSearchUserLocItem = *sUserLocIterator;
            if (sSearchUserLocItem == XPR_NULL)
                continue;

            sSearchUserLocItem2 = new fxb::SearchUserLocItem;
            sSearchUserLocItem2->mPath      = sSearchUserLocItem->mPath;
            sSearchUserLocItem2->mInclude   = sSearchUserLocItem->mInclude;
            sSearchUserLocItem2->mSubFolder = sSearchUserLocItem->mSubFolder;

            sSearchUserLoc2->mUserLocDeque.push_back(sSearchUserLocItem2);
        }

        mSearchUserLoc->push_back(sSearchUserLoc2);
    }
}

void SearchLocDlg::addLoc(fxb::SearchUserLoc *aSearchUserLoc, xpr_sint_t aIndex)
{
    if (aSearchUserLoc == XPR_NULL)
        return;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = (aIndex == -1) ? mLocComboBox.GetCount() : aIndex;
    sComboBoxExItem.iImage         = 0;
    sComboBoxExItem.iSelectedImage = 0;
    sComboBoxExItem.pszText        = (xpr_tchar_t *)aSearchUserLoc->mName.c_str();
    sComboBoxExItem.lParam         = (LPARAM)aSearchUserLoc;
    mLocComboBox.InsertItem(&sComboBoxExItem);
}

xpr_sint_t SearchLocDlg::addLocPath(fxb::SearchUserLocItem *aSearchUserLocItem, xpr_sint_t aIndex)
{
    if (aSearchUserLocItem == XPR_NULL)
        return -1;

    fxb::SearchUserLocItem *sSearchUserLocItem = new fxb::SearchUserLocItem;
    sSearchUserLocItem->mPath      = aSearchUserLocItem->mPath;
    sSearchUserLocItem->mInclude   = aSearchUserLocItem->mInclude;
    sSearchUserLocItem->mSubFolder = aSearchUserLocItem->mSubFolder;

    xpr_sint_t sImage = 2;

    LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sSearchUserLocItem->mPath.c_str());
    if (sFullPidl != XPR_NULL)
    {
        HICON sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mListImgList.Add(sIcon);
        ::DestroyIcon(sIcon);

        COM_FREE(sFullPidl);
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, sSearchUserLocItem->mPath.c_str());

    LVITEM sLvItem = {0};
    sLvItem.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem     = (aIndex == -1) ? mListCtrl.GetItemCount() : aIndex;
    sLvItem.iSubItem  = 0;
    sLvItem.iImage    = sImage;
    sLvItem.pszText   = sPath;
    sLvItem.lParam    = (LPARAM)sSearchUserLocItem;

    {
        sLvItem.mask      |= LVIF_STATE;
        sLvItem.state      = INDEXTOOVERLAYMASK(sSearchUserLocItem->mInclude ? 1 : 2);
        sLvItem.stateMask  = LVIS_OVERLAYMASK;
    }

    aIndex = mListCtrl.InsertItem(&sLvItem);

    sLvItem.mask      = LVIF_TEXT;
    sLvItem.iSubItem  = 1;
    sLvItem.pszText   = sSearchUserLocItem->mSubFolder ? XPR_STRING_LITERAL("O") : XPR_STRING_LITERAL("");
    mListCtrl.SetItem(&sLvItem);

    return aIndex;
}

void SearchLocDlg::updateStatus(void)
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    SetDlgItemText(IDC_SEARCH_LOC_STATUS, sStatusText);
}

void SearchLocDlg::OnDestroy(void)
{
    super::OnDestroy();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mLocComboBox.GetCount();
    for (i = sCount - 1; i >= 0; --i)
        mLocComboBox.DeleteItem(i);

    sCount = mListCtrl.GetItemCount();
    for (i = sCount - 1; i >= 0; --i)
        mListCtrl.DeleteItem(i);

    mLocImgList.DeleteImageList();
    mListImgList.DeleteImageList();
    DESTROY_ICON(mIcon);

    if (XPR_IS_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

xpr_bool_t SearchLocDlg::PreTranslateMessage(MSG* pMsg) 
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

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void SearchLocDlg::setLocPath(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SearchUserLoc *sSearchUserLoc = (fxb::SearchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSearchUserLoc == XPR_NULL)
        return;

    sSearchUserLoc->clear();

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SearchUserLocItem *sSearchUserLocItem;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchUserLocItem = new fxb::SearchUserLocItem;
        sSearchUserLocItem->mPath      = mListCtrl.GetItemText(i, 0);
        sSearchUserLocItem->mSubFolder = mListCtrl.GetItemText(i, 1) == XPR_STRING_LITERAL("O");
        sSearchUserLocItem->mInclude   = mListCtrl.GetItemState(i, LVIS_OVERLAYMASK) == INDEXTOOVERLAYMASK(1);

        sSearchUserLoc->mUserLocDeque.push_back(sSearchUserLocItem);
    }
}

void SearchLocDlg::OnItemAdd(void) 
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sCount >= MAX_SEARCH_LOC)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_SEARCH_LOC);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t i;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    fxb::SearchUserLocPathSet sUserLocPathSet;

    for (i = 0; i < sCount; ++i)
    {
        mListCtrl.GetItemText(i, 0, sPath, XPR_MAX_PATH);

        _tcsupr(sPath);
        sUserLocPathSet.insert(sPath);
    }

    SearchLocEditDlg sDlg;
    sDlg.setPathSet(&sUserLocPathSet);
    if (sDlg.DoModal() == IDOK)
    {
        fxb::SearchUserLocItem sSearchUserLocItem;
        sSearchUserLocItem.mPath      = sDlg.getPath();
        sSearchUserLocItem.mInclude   = sDlg.isInclude();
        sSearchUserLocItem.mSubFolder = sDlg.isSubFolder();

        xpr_sint_t sIndex = addLocPath(&sSearchUserLocItem);
        setItemFocus(sIndex);

        setLocPath();
    }

    sUserLocPathSet.clear();

    updateStatus();
}

void SearchLocDlg::OnItemModify(void) 
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    fxb::SearchUserLocItem *sSearchUserLocItem = (fxb::SearchUserLocItem *)mListCtrl.GetItemData(sIndex);
    if (sSearchUserLocItem == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    fxb::SearchUserLocPathSet sUserLocPathSet;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sIndex)
            continue;

        mListCtrl.GetItemText(i, 0, sPath, XPR_MAX_PATH);

        _tcsupr(sPath);
        sUserLocPathSet.insert(sPath);
    }

    SearchLocEditDlg sDlg;
    sDlg.setPathSet(&sUserLocPathSet);
    sDlg.setPath(sSearchUserLocItem->mPath.c_str());
    sDlg.setInclude(sSearchUserLocItem->mInclude);
    sDlg.setSubFolder(sSearchUserLocItem->mSubFolder);
    if (sDlg.DoModal() == IDOK)
    {
        sSearchUserLocItem->mPath      = sDlg.getPath();
        sSearchUserLocItem->mInclude   = sDlg.isInclude();
        sSearchUserLocItem->mSubFolder = sDlg.isSubFolder();

        mListCtrl.SetItemText(sIndex, 0, sSearchUserLocItem->mPath.c_str());
        mListCtrl.SetItemText(sIndex, 1, sSearchUserLocItem->mSubFolder ? XPR_STRING_LITERAL("O") : XPR_STRING_LITERAL(""));
        mListCtrl.SetItemState(sIndex, INDEXTOOVERLAYMASK(sSearchUserLocItem->mInclude ? 1 : 2), LVIS_OVERLAYMASK);

        setLocPath();

        setItemFocus(sIndex);
    }

    sUserLocPathSet.clear();
}

void SearchLocDlg::OnItemDelete(void) 
{
    xpr_sint_t sSelCount = mListCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    mListCtrl.DeleteItem(sIndex);

    setLocPath();

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sCount > 0)
    {
        if (sIndex >= sCount)
            sIndex = sCount - 1;

        setItemFocus(sIndex);
    }

    updateStatus();
}

void SearchLocDlg::OnItemMoveUp(void) 
{
    xpr_sint_t sSelCount = mListCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == 0)
        return;

    fxb::SearchUserLocItem *sSearchUserLocItem = (fxb::SearchUserLocItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex--);

    addLocPath(sSearchUserLocItem, sIndex);

    setItemFocus(sIndex);
}

void SearchLocDlg::OnItemMoveDown(void) 
{
    xpr_sint_t sSelCount = mListCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == (mListCtrl.GetItemCount()-1))
        return;

    fxb::SearchUserLocItem *sSearchUserLocItem = (fxb::SearchUserLocItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex++);

    addLocPath(sSearchUserLocItem, sIndex);

    setItemFocus(sIndex);
}

void SearchLocDlg::OnDeleteitemItemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    fxb::SearchUserLocItem *sSearchUserLocItem = (fxb::SearchUserLocItem *)pNMListView->lParam;
    if (sSearchUserLocItem == XPR_NULL)
        return;

    XPR_SAFE_DELETE(sSearchUserLocItem);
}

void SearchLocDlg::OnDblclkItemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnItemModify();
    *pResult = 0;
}

void SearchLocDlg::OnSetFocus(CWnd* pOldWnd) 
{
    super::OnSetFocus(pOldWnd);
    mListCtrl.SetFocus();
}

void SearchLocDlg::OnOK(void) 
{
    if (mListCtrl.GetItemCount() >= MAX_SEARCH_LOC)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_SEARCH_LOC);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    mSearchUserLoc = new fxb::SearchUserLocDeque;

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SearchUserLoc *sSearchUserLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchUserLoc = (fxb::SearchUserLoc *)mLocComboBox.GetItemData(i);
        if (sSearchUserLoc == XPR_NULL)
            continue;

        mSearchUserLoc->push_back(sSearchUserLoc);

        mLocComboBox.SetItemData(i, XPR_NULL);
    }

    super::OnOK();
}

xpr_sint_t SearchLocDlg::findLoc(LPITEMIDLIST aFullPidl)
{
    if (aFullPidl == XPR_NULL)
        return -1;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

    return findLoc(sPath);
}

xpr_sint_t SearchLocDlg::findLoc(const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL)
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        mListCtrl.GetItemText(i, 0, sPath, XPR_MAX_PATH);
        if (_tcsicmp(sPath, aPath) == 0)
            return i;
    }

    return -1;
}

xpr_sint_t SearchLocDlg::findLocName(const xpr_tchar_t *aName)
{
    if (aName == XPR_NULL)
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SearchUserLoc *sSearchUserLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchUserLoc = (fxb::SearchUserLoc *)mLocComboBox.GetItemData(i);
        if (sSearchUserLoc == XPR_NULL)
            continue;

        if (_tcsicmp(sSearchUserLoc->mName.c_str(), aName) == 0)
            return i;
    }

    return -1;
}

void SearchLocDlg::setItemFocus(xpr_sint_t aIndex)
{
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);

    mListCtrl.SetFocus();
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void SearchLocDlg::OnPaint(void)
{
    CPaintDC sPaintDc(this);

    COLORREF sWndColor  = GetSysColor(COLOR_WINDOW);
    COLORREF sBkgndColor = GetSysColor(COLOR_3DFACE);

    xpr_sint_t sTop = 0;
    {
        CRect sRect1, sRect2;
        GetDlgItem(IDC_SEARCH_LOC_LIST)->GetWindowRect(&sRect1);
        GetDlgItem(IDC_SEARCH_LOC_ITEM_LIST)->GetWindowRect(&sRect2);

        ScreenToClient(&sRect1);
        ScreenToClient(&sRect2);

        sTop = (sRect1.bottom + sRect2.top) / 2;
        sTop++;
        sTop++;
    }

    CRect sRect;
    CRect sClientRect;
    GetClientRect(&sClientRect);

    sRect = sClientRect;
    sRect.bottom = sTop;
    sPaintDc.FillSolidRect(&sRect, sWndColor);

    sPaintDc.DrawEdge(sRect, EDGE_ETCHED, BF_BOTTOM);

    sRect = sClientRect;
    sRect.top = sTop;
    sPaintDc.FillSolidRect(&sRect, sBkgndColor);
}

HBRUSH SearchLocDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor)
{
    HBRUSH sBrush = super::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd != XPR_NULL)
    {
        switch (pWnd->GetDlgCtrlID())
        {
        case IDC_SEARCH_LOC_LABEL_LIST:
        case IDC_SEARCH_LOC_LIST:
        case IDC_SEARCH_LOC_LIST_NEW:
            {
                pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
                pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));

                if (mBrush.m_hObject == XPR_NULL)
                    mBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

                return mBrush;
            }
        }
    }

    return sBrush;
}

void SearchLocDlg::OnCbnSelchangeList(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SearchUserLoc *sSearchUserLoc = (fxb::SearchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSearchUserLoc == XPR_NULL)
        return;

    mListCtrl.DeleteAllItems();

    fxb::UserLocDeque::iterator sIterator = sSearchUserLoc->mUserLocDeque.begin();
    for (; sIterator != sSearchUserLoc->mUserLocDeque.end(); ++sIterator)
        addLocPath(*sIterator);

    updateStatus();
}

void SearchLocDlg::OnCbenDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
    PNMCOMBOBOXEX pCBEx = reinterpret_cast<PNMCOMBOBOXEX>(pNMHDR);
    *pResult = 0;

    fxb::SearchUserLoc *sSearchUserLoc = (fxb::SearchUserLoc *)pCBEx->ceItem.lParam;
    if (sSearchUserLoc == XPR_NULL)
        return;

    sSearchUserLoc->clear();
    XPR_SAFE_DELETE(sSearchUserLoc);
}

void SearchLocDlg::OnListNew(void)
{
    InputDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.new.title")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.new.desc")));
    sDlg.setCheckEmpty();

    const xpr_tchar_t *sText = XPR_NULL;

    while (true)
    {
        if (sDlg.DoModal() != IDOK)
            return;

        sText = sDlg.getText();

        if (findLocName(sText) == -1)
            break;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    if (sText == XPR_NULL)
        return;

    fxb::SearchUserLoc *sSearchUserLoc = new fxb::SearchUserLoc;
    if (sSearchUserLoc == XPR_NULL)
        return;

    sSearchUserLoc->mName = sText;

    mListCtrl.DeleteAllItems();

    addLoc(sSearchUserLoc);

    mLocComboBox.SetCurSel(mLocComboBox.GetCount()-1);

    updateStatus();
}

void SearchLocDlg::OnListModify(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SearchUserLoc *sSearchUserLoc = (fxb::SearchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSearchUserLoc == XPR_NULL)
        return;

    InputDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.modify.title")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.modify.desc")));
    sDlg.setText(sSearchUserLoc->mName.c_str());
    sDlg.setCheckEmpty();

    const xpr_tchar_t *sText = XPR_NULL;

    while (true)
    {
        if (sDlg.DoModal() != IDOK)
            return;

        sText = sDlg.getText();

        if (_tcsicmp(sText, sSearchUserLoc->mName.c_str()) == 0)
            return;

        if (findLocName(sText) == -1)
            break;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    if (sText == XPR_NULL)
        return;

    sSearchUserLoc->mName = sText;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask    = CBEIF_TEXT;
    sComboBoxExItem.iItem   = sCurSel;
    sComboBoxExItem.pszText = (xpr_tchar_t *)sSearchUserLoc->mName.c_str();
    mLocComboBox.SetItem(&sComboBoxExItem);

    mLocComboBox.Invalidate();
}

void SearchLocDlg::OnListDelete(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.msg.question_delete"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    mLocComboBox.DeleteItem(sCurSel);

    xpr_sint_t sCount = mLocComboBox.GetCount();
    if (sCount == 0)
    {
        mListCtrl.DeleteAllItems();

        mLocComboBox.SetCurSel(-1);
        return;
    }

    if (sCurSel >= sCount)
        sCurSel--;

    mLocComboBox.SetCurSel(sCurSel);
    OnCbnSelchangeList();

    updateStatus();
}

LRESULT SearchLocDlg::OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(pMenu))
        sResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
        sResult = CDialog::OnMenuChar(nChar, nFlags, pMenu);

    return sResult;
}

void SearchLocDlg::OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t aIndex, xpr_bool_t bSysMenu) 
{
    //CDialog::OnInitMenuPopup(pPopupMenu, aIndex, bSysMenu);
    ASSERT(pPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *pBCPopupMenu = dynamic_cast<BCMenu *>(pPopupMenu);
    if (pBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = pBCPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = pBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if sId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if sId(xpr_uint_t) is -1, it's sub-menu.
                    pBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    pBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

                        pBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = pPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        sState.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL &&
            (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t nIndexMax = ::GetMenuItemCount(sParentMenu);
            for (xpr_sint_t aIndex = 0; aIndex < nIndexMax; ++aIndex)
            {
                if (::GetSubMenu(sParentMenu, aIndex) == pPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = pPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = pPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL ||
                (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 ||
                sState.m_nID == (xpr_uint_t)-1)
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
        xpr_uint_t sCount = pPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount && pPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void SearchLocDlg::OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
    xpr_bool_t sSetFlag = XPR_FALSE;
    if (lpMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)lpMeasureItemStruct->itemID))
        {
            CMenu *pMenu = CMenu::FromHandle((HMENU)(uintptr_t)lpMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(pMenu))
            {
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (sSetFlag == XPR_FALSE)
        CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
