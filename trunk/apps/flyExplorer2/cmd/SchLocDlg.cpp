//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SchLocDlg.h"

#include "resource.h"
#include "SchLocEditDlg.h"
#include "InputDlg.h"
#include "DlgState.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SchLocDlg::SchLocDlg(void)
    : super(IDD_SEARCH_LOC, XPR_NULL)
    , mSchUserLoc(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

SchLocDlg::~SchLocDlg(void)
{
    if (mSchUserLoc != XPR_NULL)
    {
        fxb::SchUserLoc *sSchUserLoc;
        fxb::SchUserLocDeque::iterator sIterator;

        sIterator = mSchUserLoc->begin();
        for (; sIterator != mSchUserLoc->end(); ++sIterator)
        {
            sSchUserLoc = *sIterator;
            if (sSchUserLoc == XPR_NULL)
                continue;

            sSchUserLoc->clear();
            XPR_SAFE_DELETE(sSchUserLoc);
        }

        mSchUserLoc->clear();
        XPR_SAFE_DELETE(mSchUserLoc);
    }

    DESTROY_ICON(mIcon);
}

void SchLocDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEARCH_LOC_ITEM_LIST, mListCtrl);
    DDX_Control(pDX, IDC_SEARCH_LOC_LIST,      mLocComboBox);
    DDX_Control(pDX, IDC_SEARCH_LOC_LIST_NEW,  mNewDropButton);
}

BEGIN_MESSAGE_MAP(SchLocDlg, super)
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

xpr_bool_t SchLocDlg::OnInitDialog(void) 
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
        IDI_SCH_INC,
        IDI_SCH_EXC,
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

    mNewDropButton.LoadMenu(IDR_SCH_LOC, 0);

    if (mSchUserLoc != XPR_NULL)
    {
        fxb::SchUserLoc *sSchUserLoc;
        fxb::SchUserLocDeque::iterator sIterator;

        sIterator = mSchUserLoc->begin();
        for (; sIterator != mSchUserLoc->end(); ++sIterator)
        {
            sSchUserLoc = *sIterator;
            if (sSchUserLoc == XPR_NULL)
                continue;

            addLoc(sSchUserLoc);
        }

        mSchUserLoc->clear();
        XPR_SAFE_DELETE(mSchUserLoc);
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
    mState.setSection(XPR_STRING_LITERAL("SchLoc"));
    mState.setDialog(this, XPR_TRUE);
    mState.setListCtrl(&mListCtrl);
    mState.load();

    return XPR_TRUE;
}

fxb::SchUserLocDeque *SchLocDlg::getSchLoc(xpr_bool_t aNull) const
{
    fxb::SchUserLocDeque *sSchUserLoc = mSchUserLoc;

    if (aNull == XPR_TRUE)
        sSchUserLoc = XPR_NULL;

    return sSchUserLoc;
}

void SchLocDlg::setSchLoc(fxb::SchUserLocDeque *aSchUserLoc)
{
    fxb::SchUserLoc *sSchUserLoc;
    fxb::SchUserLoc *sSchUserLoc2;
    fxb::SchUserLocDeque::iterator sIterator;
    fxb::UserLocDeque::iterator sUserLocIterator;
    fxb::SchUserLocItem *sSchUserLocItem;
    fxb::SchUserLocItem *sSchUserLocItem2;

    if (mSchUserLoc != XPR_NULL)
    {
        sIterator = mSchUserLoc->begin();
        for (; sIterator != mSchUserLoc->end(); ++sIterator)
        {
            sSchUserLoc = *sIterator;
            if (sSchUserLoc == XPR_NULL)
                continue;

            sSchUserLoc->clear();
            XPR_SAFE_DELETE(sSchUserLoc);
        }

        mSchUserLoc->clear();
        XPR_SAFE_DELETE(mSchUserLoc);
    }

    if (aSchUserLoc == XPR_NULL)
        return;

    mSchUserLoc = new fxb::SchUserLocDeque;

    sIterator = aSchUserLoc->begin();
    for (; sIterator != aSchUserLoc->end(); ++sIterator)
    {
        sSchUserLoc = *sIterator;
        if (sSchUserLoc == XPR_NULL)
            continue;

        sSchUserLoc2 = new fxb::SchUserLoc;
        if (sSchUserLoc2 == XPR_NULL)
            continue;

        sSchUserLoc2->mName = sSchUserLoc->mName;

        sUserLocIterator = sSchUserLoc->mUserLocDeque.begin();
        for (; sUserLocIterator != sSchUserLoc->mUserLocDeque.end(); ++sUserLocIterator)
        {
            sSchUserLocItem = *sUserLocIterator;
            if (sSchUserLocItem == XPR_NULL)
                continue;

            sSchUserLocItem2 = new fxb::SchUserLocItem;
            sSchUserLocItem2->mPath      = sSchUserLocItem->mPath;
            sSchUserLocItem2->mInclude   = sSchUserLocItem->mInclude;
            sSchUserLocItem2->mSubFolder = sSchUserLocItem->mSubFolder;

            sSchUserLoc2->mUserLocDeque.push_back(sSchUserLocItem2);
        }

        mSchUserLoc->push_back(sSchUserLoc2);
    }
}

void SchLocDlg::addLoc(fxb::SchUserLoc *aSchUserLoc, xpr_sint_t aIndex)
{
    if (aSchUserLoc == XPR_NULL)
        return;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = (aIndex == -1) ? mLocComboBox.GetCount() : aIndex;
    sComboBoxExItem.iImage         = 0;
    sComboBoxExItem.iSelectedImage = 0;
    sComboBoxExItem.pszText        = (xpr_tchar_t *)aSchUserLoc->mName.c_str();
    sComboBoxExItem.lParam         = (LPARAM)aSchUserLoc;
    mLocComboBox.InsertItem(&sComboBoxExItem);
}

xpr_sint_t SchLocDlg::addLocPath(fxb::SchUserLocItem *aSchUserLocItem, xpr_sint_t aIndex)
{
    if (aSchUserLocItem == XPR_NULL)
        return -1;

    fxb::SchUserLocItem *sSchUserLocItem = new fxb::SchUserLocItem;
    sSchUserLocItem->mPath      = aSchUserLocItem->mPath;
    sSchUserLocItem->mInclude   = aSchUserLocItem->mInclude;
    sSchUserLocItem->mSubFolder = aSchUserLocItem->mSubFolder;

    xpr_sint_t sImage = 2;

    LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sSchUserLocItem->mPath.c_str());
    if (sFullPidl != XPR_NULL)
    {
        HICON sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mListImgList.Add(sIcon);
        ::DestroyIcon(sIcon);

        COM_FREE(sFullPidl);
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, sSchUserLocItem->mPath.c_str());

    LVITEM sLvItem = {0};
    sLvItem.mask      = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem     = (aIndex == -1) ? mListCtrl.GetItemCount() : aIndex;
    sLvItem.iSubItem  = 0;
    sLvItem.iImage    = sImage;
    sLvItem.pszText   = sPath;
    sLvItem.lParam    = (LPARAM)sSchUserLocItem;

    {
        sLvItem.mask      |= LVIF_STATE;
        sLvItem.state      = INDEXTOOVERLAYMASK(sSchUserLocItem->mInclude ? 1 : 2);
        sLvItem.stateMask  = LVIS_OVERLAYMASK;
    }

    aIndex = mListCtrl.InsertItem(&sLvItem);

    sLvItem.mask      = LVIF_TEXT;
    sLvItem.iSubItem  = 1;
    sLvItem.pszText   = sSchUserLocItem->mSubFolder ? XPR_STRING_LITERAL("O") : XPR_STRING_LITERAL("");
    mListCtrl.SetItem(&sLvItem);

    return aIndex;
}

void SchLocDlg::updateStatus(void)
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    SetDlgItemText(IDC_SEARCH_LOC_STATUS, sStatusText);
}

void SchLocDlg::OnDestroy(void)
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

    // Save Dialog State
    mState.reset();
    mState.save();
}

xpr_bool_t SchLocDlg::PreTranslateMessage(MSG* pMsg) 
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

void SchLocDlg::setLocPath(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SchUserLoc *sSchUserLoc = (fxb::SchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSchUserLoc == XPR_NULL)
        return;

    sSchUserLoc->clear();

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SchUserLocItem *sSchUserLocItem;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchUserLocItem = new fxb::SchUserLocItem;
        sSchUserLocItem->mPath      = mListCtrl.GetItemText(i, 0);
        sSchUserLocItem->mSubFolder = mListCtrl.GetItemText(i, 1) == XPR_STRING_LITERAL("O");
        sSchUserLocItem->mInclude   = mListCtrl.GetItemState(i, LVIS_OVERLAYMASK) == INDEXTOOVERLAYMASK(1);

        sSchUserLoc->mUserLocDeque.push_back(sSchUserLocItem);
    }
}

void SchLocDlg::OnItemAdd(void) 
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sCount >= MAX_SCH_LOC)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_SCH_LOC);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t i;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    fxb::SchUserLocPathSet sUserLocPathSet;

    for (i = 0; i < sCount; ++i)
    {
        mListCtrl.GetItemText(i, 0, sPath, XPR_MAX_PATH);

        _tcsupr(sPath);
        sUserLocPathSet.insert(sPath);
    }

    SchLocEditDlg sDlg;
    sDlg.setPathSet(&sUserLocPathSet);
    if (sDlg.DoModal() == IDOK)
    {
        fxb::SchUserLocItem sSchUserLocItem;
        sSchUserLocItem.mPath      = sDlg.getPath();
        sSchUserLocItem.mInclude   = sDlg.isInclude();
        sSchUserLocItem.mSubFolder = sDlg.isSubFolder();

        xpr_sint_t sIndex = addLocPath(&sSchUserLocItem);
        setItemFocus(sIndex);

        setLocPath();
    }

    sUserLocPathSet.clear();

    updateStatus();
}

void SchLocDlg::OnItemModify(void) 
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    fxb::SchUserLocItem *sSchUserLocItem = (fxb::SchUserLocItem *)mListCtrl.GetItemData(sIndex);
    if (sSchUserLocItem == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    fxb::SchUserLocPathSet sUserLocPathSet;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sIndex)
            continue;

        mListCtrl.GetItemText(i, 0, sPath, XPR_MAX_PATH);

        _tcsupr(sPath);
        sUserLocPathSet.insert(sPath);
    }

    SchLocEditDlg sDlg;
    sDlg.setPathSet(&sUserLocPathSet);
    sDlg.setPath(sSchUserLocItem->mPath.c_str());
    sDlg.setInclude(sSchUserLocItem->mInclude);
    sDlg.setSubFolder(sSchUserLocItem->mSubFolder);
    if (sDlg.DoModal() == IDOK)
    {
        sSchUserLocItem->mPath      = sDlg.getPath();
        sSchUserLocItem->mInclude   = sDlg.isInclude();
        sSchUserLocItem->mSubFolder = sDlg.isSubFolder();

        mListCtrl.SetItemText(sIndex, 0, sSchUserLocItem->mPath.c_str());
        mListCtrl.SetItemText(sIndex, 1, sSchUserLocItem->mSubFolder ? XPR_STRING_LITERAL("O") : XPR_STRING_LITERAL(""));
        mListCtrl.SetItemState(sIndex, INDEXTOOVERLAYMASK(sSchUserLocItem->mInclude ? 1 : 2), LVIS_OVERLAYMASK);

        setLocPath();

        setItemFocus(sIndex);
    }

    sUserLocPathSet.clear();
}

void SchLocDlg::OnItemDelete(void) 
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

void SchLocDlg::OnItemMoveUp(void) 
{
    xpr_sint_t sSelCount = mListCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == 0)
        return;

    fxb::SchUserLocItem *sSchUserLocItem = (fxb::SchUserLocItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex--);

    addLocPath(sSchUserLocItem, sIndex);

    setItemFocus(sIndex);
}

void SchLocDlg::OnItemMoveDown(void) 
{
    xpr_sint_t sSelCount = mListCtrl.GetSelectedCount();
    if (sSelCount <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == (mListCtrl.GetItemCount()-1))
        return;

    fxb::SchUserLocItem *sSchUserLocItem = (fxb::SchUserLocItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex++);

    addLocPath(sSchUserLocItem, sIndex);

    setItemFocus(sIndex);
}

void SchLocDlg::OnDeleteitemItemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    fxb::SchUserLocItem *sSchUserLocItem = (fxb::SchUserLocItem *)pNMListView->lParam;
    if (sSchUserLocItem == XPR_NULL)
        return;

    XPR_SAFE_DELETE(sSchUserLocItem);
}

void SchLocDlg::OnDblclkItemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnItemModify();
    *pResult = 0;
}

void SchLocDlg::OnSetFocus(CWnd* pOldWnd) 
{
    super::OnSetFocus(pOldWnd);
    mListCtrl.SetFocus();
}

void SchLocDlg::OnOK(void) 
{
    if (mListCtrl.GetItemCount() >= MAX_SCH_LOC)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search_user_location.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_SCH_LOC);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    mSchUserLoc = new fxb::SchUserLocDeque;

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SchUserLoc *sSchUserLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchUserLoc = (fxb::SchUserLoc *)mLocComboBox.GetItemData(i);
        if (sSchUserLoc == XPR_NULL)
            continue;

        mSchUserLoc->push_back(sSchUserLoc);

        mLocComboBox.SetItemData(i, XPR_NULL);
    }

    super::OnOK();
}

xpr_sint_t SchLocDlg::findLoc(LPITEMIDLIST aFullPidl)
{
    if (aFullPidl == XPR_NULL)
        return -1;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);

    return findLoc(sPath);
}

xpr_sint_t SchLocDlg::findLoc(const xpr_tchar_t *aPath)
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

xpr_sint_t SchLocDlg::findLocName(const xpr_tchar_t *aName)
{
    if (aName == XPR_NULL)
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::SchUserLoc *sSchUserLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchUserLoc = (fxb::SchUserLoc *)mLocComboBox.GetItemData(i);
        if (sSchUserLoc == XPR_NULL)
            continue;

        if (_tcsicmp(sSchUserLoc->mName.c_str(), aName) == 0)
            return i;
    }

    return -1;
}

void SchLocDlg::setItemFocus(xpr_sint_t aIndex)
{
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);

    mListCtrl.SetFocus();
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void SchLocDlg::OnPaint(void)
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

HBRUSH SchLocDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor)
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

void SchLocDlg::OnCbnSelchangeList(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SchUserLoc *sSchUserLoc = (fxb::SchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSchUserLoc == XPR_NULL)
        return;

    mListCtrl.DeleteAllItems();

    fxb::UserLocDeque::iterator sIterator = sSchUserLoc->mUserLocDeque.begin();
    for (; sIterator != sSchUserLoc->mUserLocDeque.end(); ++sIterator)
        addLocPath(*sIterator);

    updateStatus();
}

void SchLocDlg::OnCbenDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
    PNMCOMBOBOXEX pCBEx = reinterpret_cast<PNMCOMBOBOXEX>(pNMHDR);
    *pResult = 0;

    fxb::SchUserLoc *sSchUserLoc = (fxb::SchUserLoc *)pCBEx->ceItem.lParam;
    if (sSchUserLoc == XPR_NULL)
        return;

    sSchUserLoc->clear();
    XPR_SAFE_DELETE(sSchUserLoc);
}

void SchLocDlg::OnListNew(void)
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

    fxb::SchUserLoc *sSchUserLoc = new fxb::SchUserLoc;
    if (sSchUserLoc == XPR_NULL)
        return;

    sSchUserLoc->mName = sText;

    mListCtrl.DeleteAllItems();

    addLoc(sSchUserLoc);

    mLocComboBox.SetCurSel(mLocComboBox.GetCount()-1);

    updateStatus();
}

void SchLocDlg::OnListModify(void)
{
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::SchUserLoc *sSchUserLoc = (fxb::SchUserLoc *)mLocComboBox.GetItemData(sCurSel);
    if (sSchUserLoc == XPR_NULL)
        return;

    InputDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.modify.title")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.list.modify.desc")));
    sDlg.setText(sSchUserLoc->mName.c_str());
    sDlg.setCheckEmpty();

    const xpr_tchar_t *sText = XPR_NULL;

    while (true)
    {
        if (sDlg.DoModal() != IDOK)
            return;

        sText = sDlg.getText();

        if (_tcsicmp(sText, sSchUserLoc->mName.c_str()) == 0)
            return;

        if (findLocName(sText) == -1)
            break;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.search_user_location.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    if (sText == XPR_NULL)
        return;

    sSchUserLoc->mName = sText;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask    = CBEIF_TEXT;
    sComboBoxExItem.iItem   = sCurSel;
    sComboBoxExItem.pszText = (xpr_tchar_t *)sSchUserLoc->mName.c_str();
    mLocComboBox.SetItem(&sComboBoxExItem);

    mLocComboBox.Invalidate();
}

void SchLocDlg::OnListDelete(void)
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

LRESULT SchLocDlg::OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(pMenu))
        sResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
        sResult = CDialog::OnMenuChar(nChar, nFlags, pMenu);

    return sResult;
}

void SchLocDlg::OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t aIndex, xpr_bool_t bSysMenu) 
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

void SchLocDlg::OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
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
