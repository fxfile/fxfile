//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FileScrapDlg.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"
#include "InputDlg.h"

#include "../command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FileScrapDlg::FileScrapDlg(void)
    : super(IDD_FILE_SCRAP, XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FileScrapDlg::~FileScrapDlg(void)
{
    DESTROY_ICON(mIcon);
}

void FileScrapDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILE_SCRAP_LIST,      mListCtrl);
    DDX_Control(pDX, IDC_FILE_SCRAP_GROUP,     mGroupWnd);
    DDX_Control(pDX, IDC_FILE_SCRAP_NEW_GROUP, mNewButton);
}

BEGIN_MESSAGE_MAP(FileScrapDlg, super)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_INITMENUPOPUP()
    ON_WM_MENUCHAR()
    ON_WM_MEASUREITEM()
    ON_UPDATE_COMMAND_UI(ID_SCRAP_GROUP_MODIFY, OnUpdateGroupModify)
    ON_UPDATE_COMMAND_UI(ID_SCRAP_GROUP_DELETE, OnUpdateGroupDelete)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_FILE_SCRAP_LIST, OnGetdispinfoList)
    ON_NOTIFY(LVN_DELETEITEM, IDC_FILE_SCRAP_LIST, OnDeleteItem)
    ON_CBN_SELCHANGE(IDC_FILE_SCRAP_GROUP, OnCbnSelchangeGroup)
    ON_COMMAND(ID_SCRAP_GROUP_NEW,     OnGroupNew)
    ON_COMMAND(ID_SCRAP_GROUP_MODIFY,  OnGroupModify)
    ON_COMMAND(ID_SCRAP_GROUP_DELETE,  OnGroupDelete)
    ON_COMMAND(ID_SCRAP_GROUP_DEFAULT, OnGroupDefault)
    ON_BN_CLICKED(IDC_FILE_SCRAP_NEW_GROUP,       OnGroupNew)
    ON_BN_CLICKED(IDC_FILE_SCRAP_COPY_TO_FOLDER,  OnCopyToFolder)
    ON_BN_CLICKED(IDC_FILE_SCRAP_MOVE_TO_FOLDER,  OnMoveToFolder)
    ON_BN_CLICKED(IDC_FILE_SCRAP_DELETE,          OnDelete)
    ON_BN_CLICKED(IDC_FILE_SCRAP_CLEAR,           OnClear)
    ON_BN_CLICKED(IDC_FILE_SCRAP_CLEAR_ALL,       OnClearAll)
    ON_BN_CLICKED(IDC_FILE_SCRAP_VALIDATE,        OnValidate)
END_MESSAGE_MAP()

xpr_bool_t FileScrapDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_FILE_SCRAP_LIST,           sizeResize, sizeResize);
    AddControl(IDC_FILE_SCRAP_COPY_TO_FOLDER, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_MOVE_TO_FOLDER, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_DELETE,         sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_CLEAR,          sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_CLEAR_ALL,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_VALIDATE,       sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SCRAP_STATUS,         sizeResize, sizeRepos);
    AddControl(IDC_FILE_SCRAP_UNDERLINE,      sizeResize, sizeRepos);
    AddControl(IDCANCEL,                      sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    mListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.list.column.no")),        LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.list.column.filename")),  LVCFMT_LEFT,  150, -1);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.list.column.location")),  LVCFMT_LEFT,  500, -1);
    mListCtrl.InsertColumn(3, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.list.column.full_path")), LVCFMT_LEFT,  500, -1);

    mGroupImgList.Create(16, 16, ILC_COLOR16 | ILC_MASK, -1, -1);
    HICON sIcon = theApp.LoadIcon(MAKEINTRESOURCE(IDI_FILE_SCRAP));
    mGroupImgList.Add(sIcon);
    ::DestroyIcon(sIcon);

    mGroupWnd.SetImageList(&mGroupImgList);

    mNewButton.LoadMenu(IDR_FILE_SCRAP, 0);

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    xpr_sint_t sCurSel = 0;
    fxb::FileScrap::Group *sGroup;

    sCount = sFileScrap.getGroupCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = sFileScrap.getGroup(i);
        if (XPR_IS_NULL(sGroup))
            continue;

        addGroup(sGroup);

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
            sCurSel = (xpr_sint_t)i;
    }

    updateDefGroup();

    mGroupWnd.SetCurSel(sCurSel);
    OnCbnSelchangeGroup();

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.title")));
    SetDlgItemText(IDC_FILE_SCRAP_GROUP_LABEL,    theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.label.group")));
    SetDlgItemText(IDC_FILE_SCRAP_NEW_GROUP,      theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.new_group")));
    SetDlgItemText(IDC_FILE_SCRAP_LIST_LABEL,     theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.label.list")));
    SetDlgItemText(IDC_FILE_SCRAP_COPY_TO_FOLDER, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.copy_to")));
    SetDlgItemText(IDC_FILE_SCRAP_MOVE_TO_FOLDER, theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.move_to")));
    SetDlgItemText(IDC_FILE_SCRAP_DELETE,         theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.delete")));
    SetDlgItemText(IDC_FILE_SCRAP_VALIDATE,       theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.validate")));
    SetDlgItemText(IDC_FILE_SCRAP_CLEAR,          theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.clear")));
    SetDlgItemText(IDC_FILE_SCRAP_CLEAR_ALL,      theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.clear_all")));
    SetDlgItemText(IDCANCEL,                      theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.button.close")));

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("FileScrap"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void FileScrapDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mIcon);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

HCURSOR FileScrapDlg::OnQueryDragIcon(void) 
{
    return (HCURSOR)mIcon;
}

void FileScrapDlg::OnPaint(void)
{
    if (IsIconic() == XPR_TRUE)
    {
        CPaintDC sPaintDC(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) sPaintDC.GetSafeHdc(), 0);

        // Center icon in client rectangle
        xpr_sint_t sCxIcon = GetSystemMetrics(SM_CXICON);
        xpr_sint_t sCyIcon = GetSystemMetrics(SM_CYICON);

        CRect sRect;
        GetClientRect(&sRect);
        xpr_sint_t x = (sRect.Width() - sCxIcon + 1) / 2;
        xpr_sint_t y = (sRect.Height() - sCyIcon + 1) / 2;

        // Draw the icon
        sPaintDC.DrawIcon(x, y, mIcon);
    }
    else
    {
        CPaintDC sPaintDC(this);

        COLORREF sWindowColor = GetSysColor(COLOR_WINDOW);
        COLORREF sBackColor   = GetSysColor(COLOR_3DFACE);

        xpr_sint_t sTop = 0;
        {
            CRect sRect1, sRect2;
            GetDlgItem(IDC_FILE_SCRAP_GROUP)->GetWindowRect(&sRect1);
            GetDlgItem(IDC_FILE_SCRAP_LIST_LABEL)->GetWindowRect(&sRect2);

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
        sPaintDC.FillSolidRect(&sRect, sWindowColor);

        sPaintDC.DrawEdge(sRect, EDGE_ETCHED, BF_BOTTOM);

        sRect = sClientRect;
        sRect.top = sTop;
        sPaintDC.FillSolidRect(&sRect, sBackColor);
    }
}

HBRUSH FileScrapDlg::OnCtlColor(CDC *aDC, CWnd *aWnd, xpr_uint_t aCtlColor)
{
    HBRUSH sBrush = super::OnCtlColor(aDC, aWnd, aCtlColor);

    if (XPR_IS_NOT_NULL(aWnd))
    {
        switch (aWnd->GetDlgCtrlID())
        {
        case IDC_FILE_SCRAP_GROUP_LABEL:
        case IDC_FILE_SCRAP_GROUP:
        case IDC_FILE_SCRAP_NEW_GROUP:
            {
                aDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
                aDC->SetBkColor(::GetSysColor(COLOR_WINDOW));

                if (XPR_IS_NULL(mBrush.m_hObject))
                    mBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

                return mBrush;
            }
        }
    }

    return sBrush;
}

LRESULT FileScrapDlg::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu)
{
    LRESULT sResult;

    if (BCMenu::IsMenu(aMenu) == XPR_TRUE)
        sResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sResult = CDialog::OnMenuChar(aChar, aFlags, aMenu);

    return sResult;
}

void FileScrapDlg::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    ASSERT(aPopupMenu != XPR_NULL);
    // Check the enabled state of various menu items.

    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = sBCPopupMenu->GetMenuItemCount();

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
        if (sParentWnd != XPR_NULL &&
            (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
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

void FileScrapDlg::OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct) 
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
        CDialog::OnMeasureItem(aIdCtl, aMeasureItemStruct);
}

void FileScrapDlg::initList(fxb::FileScrap::Group *aGroup, xpr_bool_t aRedraw)
{
    CWaitCursor sWaitCursor;

    if (XPR_IS_TRUE(aRedraw))
        mListCtrl.SetRedraw(XPR_FALSE);

    mListCtrl.DeleteAllItems();

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    LVITEM sLvItem = {0};
    fxb::FileScrap::Item *sItem;
    fxb::FileScrap::ItemDeque::iterator sIterator;

    sIterator = aGroup->mItemDeque.begin();
    for (; sIterator != aGroup->mItemDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        sLvItem.mask        = LVIF_TEXT | LVIF_PARAM;
        sLvItem.iItem       = mListCtrl.GetItemCount();
        sLvItem.iSubItem    = 0;
        sLvItem.pszText     = LPSTR_TEXTCALLBACK;
        sLvItem.cchTextMax  = XPR_MAX_PATH;
        sLvItem.lParam      = (LPARAM)sItem;
        mListCtrl.InsertItem(&sLvItem);
    }

    updateStatus();

    if (XPR_IS_TRUE(aRedraw))
        mListCtrl.SetRedraw();
}

void FileScrapDlg::OnGetdispinfoList(NMHDR *aNmHdr, LRESULT *aResult) 
{
    LV_DISPINFO *sLvDispInfo = (LV_DISPINFO *)aNmHdr;
    *aResult = 0;

    fxb::FileScrap::Item *sItem = (fxb::FileScrap::Item *)sLvDispInfo->item.lParam;
    if (XPR_IS_NULL(sItem))
        return;

    LVITEM &sLvItem = sLvDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        sLvItem.pszText[0] = '\0';

        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                _stprintf(sLvItem.pszText, XPR_STRING_LITERAL("%d"), sLvItem.iItem + 1);
                break;
            }

        case 1:
            {
                xpr_size_t sFind = sItem->mPath.rfind(XPR_STRING_LITERAL('\\'));
                if (sFind != std::tstring::npos)
                    _tcscpy(sLvItem.pszText, sItem->mPath.substr(sFind+1).c_str());
                break;
            }

        case 2:
            {
                xpr_size_t sFind = sItem->mPath.rfind(XPR_STRING_LITERAL('\\'));
                if (sFind != std::tstring::npos)
                    _tcscpy(sLvItem.pszText, sItem->mPath.substr(0, sFind).c_str());
                break;
            }

        case 3:
            {
                _tcscpy(sLvItem.pszText, sItem->mPath.c_str());
                break;
            }
        }
    }
}

void FileScrapDlg::OnDeleteItem(NMHDR *aNmHdr, LRESULT *aResult)
{
    NM_LISTVIEW *sNmListView = (NM_LISTVIEW*)aNmHdr;
    *aResult = 0;
}

void FileScrapDlg::updateStatus(void)
{
    xpr_tchar_t sStatus[0xff] = {0};
    _stprintf(sStatus, theApp.loadFormatString(XPR_STRING_LITERAL("popup.file_scrap.status.count"), XPR_STRING_LITERAL("%d")), mListCtrl.GetItemCount());

    GetDlgItem(IDC_FILE_SCRAP_STATUS)->SetWindowText(sStatus);
}

xpr_sint_t FileScrapDlg::addGroup(fxb::FileScrap::Group *aGroup, xpr_sint_t aIndex)
{
    if (XPR_IS_NULL(aGroup))
        return -1;

    std::tstring sGroupName(aGroup->mGroupName);
    if (sGroupName == fxb::kFileScrapNoGroupName)
        sGroupName = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.group.no_group"));

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = (aIndex == -1) ? mGroupWnd.GetCount() : aIndex;
    sComboBoxExItem.iImage         = 0;
    sComboBoxExItem.iSelectedImage = 0;
    sComboBoxExItem.pszText        = (xpr_tchar_t *)sGroupName.c_str();
    sComboBoxExItem.lParam         = (LPARAM)aGroup;

    return mGroupWnd.InsertItem(&sComboBoxExItem);
}

void FileScrapDlg::OnCbnSelchangeGroup(void)
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    initList(sGroup, XPR_TRUE);
}

fxb::FileScrap::Group *FileScrapDlg::getCurGroup(void)
{
    xpr_sint_t sIndex = mGroupWnd.GetCurSel();
    if (sIndex == CB_ERR)
        return XPR_NULL;

    return (fxb::FileScrap::Group *)mGroupWnd.GetItemData(sIndex);
}

void FileScrapDlg::updateDefGroup(void)
{
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    std::tstring sGroupName;
    fxb::FileScrap::Group *sGroup;

    sCount = mGroupWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = (fxb::FileScrap::Group *)mGroupWnd.GetItemData((xpr_sint_t)i);
        if (XPR_IS_NULL(sGroup))
            continue;

        sGroupName = sGroup->mGroupName;
        if (sGroupName == fxb::kFileScrapNoGroupName)
            sGroupName = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.group.no_group"));

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
            sGroupName += theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.group.default"));

        COMBOBOXEXITEM sComboBoxExItem = {0};
        sComboBoxExItem.mask    = CBEIF_TEXT;
        sComboBoxExItem.iItem   = i;
        sComboBoxExItem.pszText = (xpr_tchar_t *)sGroupName.c_str();
        mGroupWnd.SetItem(&sComboBoxExItem);
    }

    mGroupWnd.Invalidate();
}

void FileScrapDlg::OnGroupNew(void)
{
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    if (sFileScrap.getGroupCount() >= fxb::MAX_FILE_SCRAP_GROUP)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.file_scrap.msg.limit_group_count"), XPR_STRING_LITERAL("%d")), fxb::MAX_FILE_SCRAP_GROUP);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    InputDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.new_group.title")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.new_group.desc")));
    sDlg.setCheckEmpty();
    sDlg.setLimitText(fxb::MAX_FILE_SCRAP_GROUP_NAME);

    xpr_uint_t sGroupId = fxb::FileScrap::InvalidGroupId;
    const xpr_tchar_t *sText;

    while (true)
    {
        if (sDlg.DoModal() != IDOK)
            return;

        sText = sDlg.getText();

        if (fxb::VerifyFileName(sText) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.wrong_group_name"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            continue;
        }

        sGroupId = sFileScrap.addGroup(sText);
        if (sGroupId != fxb::FileScrap::InvalidGroupId)
            break;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.duplicated_group_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    fxb::FileScrap::Group *sGroup = sFileScrap.findGroup(sGroupId);
    if (XPR_IS_NULL(sGroup))
        return;

    xpr_sint_t sIndex = addGroup(sGroup);
    if (sIndex != CB_ERR)
    {
        mGroupWnd.SetCurSel(sIndex);
        OnCbnSelchangeGroup();
    }
}

void FileScrapDlg::OnGroupModify(void)
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    InputDlg sDlg;
    sDlg.setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.group_rename.title")));
    sDlg.setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.group_rename.desc")));
    sDlg.setText(sGroup->mGroupName.c_str());
    sDlg.setCheckEmpty();
    sDlg.setLimitText(fxb::MAX_FILE_SCRAP_GROUP_NAME);

    xpr_uint_t sGroupId = fxb::FileScrap::InvalidGroupId;
    const xpr_tchar_t *sText;

    while (true)
    {
        if (sDlg.DoModal() != IDOK)
            return;

        sText = sDlg.getText();

        if (fxb::VerifyFileName(sText) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.wrong_group_name"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            continue;
        }

        if (_tcsicmp(sText, sGroup->mGroupName.c_str()) == 0)
            return;

        if (sFileScrap.findGroup(sText) == XPR_NULL)
            break;

        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.duplicated_group_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    sFileScrap.renameGroup(sGroup->mGroupName.c_str(), sText);

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask    = CBEIF_TEXT;
    sComboBoxExItem.iItem   = mGroupWnd.GetCurSel();
    sComboBoxExItem.pszText = (xpr_tchar_t *)sGroup->mGroupName.c_str();
    mGroupWnd.SetItem(&sComboBoxExItem);

    mGroupWnd.Invalidate();
}

void FileScrapDlg::OnGroupDelete(void)
{
    xpr_sint_t sCurSel = mGroupWnd.GetCurSel();
    if (sCurSel < 0)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.confirm_delete"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NOT_NULL(sGroup))
        sFileScrap.removeGroup(sGroup->mGroupId);

    mGroupWnd.DeleteItem(sCurSel);

    xpr_sint_t sCount = mGroupWnd.GetCount();
    if (sCount == 0)
    {
        mListCtrl.DeleteAllItems();
        mGroupWnd.SetCurSel(-1);
        updateStatus();
        return;
    }

    xpr_sint_t i;
    for (i = 0; i < sCount; ++i)
    {
        sGroup = (fxb::FileScrap::Group *)mGroupWnd.GetItemData(i);
        if (XPR_IS_NULL(sGroup))
            continue;

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
        {
            sCurSel = i;
            break;
        }
    }

    if (sCurSel >= sCount)
        sCurSel--;

    mGroupWnd.SetCurSel(sCurSel);
    OnCbnSelchangeGroup();

    updateDefGroup();
}

void FileScrapDlg::OnGroupDefault(void)
{
    xpr_sint_t sCurSel = mGroupWnd.GetCurSel();
    if (sCurSel < 0)
        return;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NOT_NULL(sGroup))
        sFileScrap.setCurGourpId(sGroup->mGroupId);

    updateDefGroup();
}

void FileScrapDlg::OnUpdateGroupModify(CCmdUI *aCmdUI)
{
    xpr_bool_t sEnable = XPR_TRUE;

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NOT_NULL(sGroup))
        sEnable = (sGroup->mGroupId != fxb::FileScrap::instance().getDefGroupId());

    aCmdUI->Enable(sEnable);
}

void FileScrapDlg::OnUpdateGroupDelete(CCmdUI *aCmdUI)
{
    xpr_bool_t sEnable = XPR_TRUE;

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NOT_NULL(sGroup))
        sEnable = (sGroup->mGroupId != fxb::FileScrap::instance().getDefGroupId());

    aCmdUI->Enable(sEnable);
}

void FileScrapDlg::OnCopyToFolder(void) 
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    operate(sGroup, FO_COPY);
}

void FileScrapDlg::OnMoveToFolder(void) 
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    operate(sGroup, FO_MOVE);

    initList(sGroup, XPR_TRUE);
}

void FileScrapDlg::OnDelete(void) 
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    operate(sGroup, FO_DELETE);

    initList(sGroup, XPR_TRUE);
}

xpr_bool_t FileScrapDlg::operate(fxb::FileScrap::Group *aGroup, xpr_sint_t aFunc)
{
    if (XPR_IS_NULL(aGroup))
        return XPR_FALSE;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    if (sFileScrap.getItemCount(aGroup->mGroupId) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.no_file"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return XPR_FALSE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    CWaitCursor sWaitCursor;
    if (aFunc == FO_DELETE)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.confirm_delete_or_trash"));
        xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_OK | MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (sMsgId != IDCANCEL)
        {
            if (sMsgId == IDYES) sFileScrap.deleteOperation(aGroup->mGroupId);
            else                 sFileScrap.recycleBinOperation(aGroup->mGroupId);

            sResult = XPR_TRUE;
        }
    }
    else
    {
        xpr_tchar_t sTarget[XPR_MAX_PATH + 1] = {0};
        if (browse(sTarget))
        {
            if (aFunc == FO_COPY) sFileScrap.copyOperation(aGroup->mGroupId, sTarget);
            else                  sFileScrap.moveOperation(aGroup->mGroupId, sTarget);

            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND aHwnd, xpr_uint_t aMsg, LPARAM lParam, LPARAM aData)
{
    if (aMsg == BFFM_INITIALIZED)
        ::SendMessage(aHwnd, BFFM_SETSELECTION, XPR_FALSE, aData);

    return 0;
}

xpr_bool_t FileScrapDlg::browse(xpr_tchar_t *aTarget)
{
    xpr_bool_t sResult = XPR_FALSE;

    static LPITEMIDLIST sFullPidl = XPR_NULL;
    COM_FREE(sFullPidl);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.folder_browse.title"));
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sFullPidl;

    sFullPidl = SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        SHGetPathFromIDList(sFullPidl, aTarget);
        sResult = XPR_TRUE;
    }

    return sResult;
}

void FileScrapDlg::OnClear(void)
{
    xpr_sint_t sCount = mListCtrl.GetSelectedCount();
    if (sCount <= 0)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.confirm_clear"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    typedef std::deque<xpr_sint_t> IndexDeque;
    IndexDeque sIndexDeque;

    xpr_sint_t sIndex;
    CWaitCursor sWaitCursor;

    POSITION sPosition = mListCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = mListCtrl.GetNextSelectedItem(sPosition);
        if (sIndex < 0)
            continue;

        sIndexDeque.push_front(sIndex);
    }

    mListCtrl.SetRedraw(XPR_FALSE);

    IndexDeque::iterator sIterator = sIndexDeque.begin();
    for (; sIterator != sIndexDeque.end(); ++sIterator)
    {
        sIndex = *sIterator;

        mListCtrl.DeleteItem(sIndex);

        sFileScrap.removeItem(sGroup->mGroupId, sIndex);
    }

    mListCtrl.EnsureVisible(sIndex, XPR_TRUE);
    mListCtrl.SetRedraw();

    updateStatus();

    sIndexDeque.clear();
}

void FileScrapDlg::OnClearAll(void) 
{
    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sCount <= 0)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.confirm_clear_all"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);
    mListCtrl.DeleteAllItems();
    mListCtrl.SetRedraw();

    fxb::FileScrap::instance().removeAllItems(sGroup->mGroupId);

    updateStatus();
}

void FileScrapDlg::OnValidate(void) 
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    if (sFileScrap.getItemCount(sGroup->mGroupId) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.no_file"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FileScrap::Item *sItem;

    typedef std::deque<xpr_sint_t> IndexDeque;
    IndexDeque sIndexDeque;

    {
        CWaitCursor sWaitCursor;

        sCount = mListCtrl.GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            sItem = (fxb::FileScrap::Item *)mListCtrl.GetItemData(i);
            if (XPR_IS_NULL(sItem))
                continue;

            if (fxb::IsExistFile(sItem->mPath) == XPR_FALSE)
                sIndexDeque.push_front(i);
        }
    }

    if (sIndexDeque.empty() == true)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.validated"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
        return;
    }

    xpr_tchar_t sMsg[0xff] = {0};
    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.file_scrap.msg.some_invalidated"), XPR_STRING_LITERAL("%d")), sIndexDeque.size());
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
    {
        sIndexDeque.clear();
        return;
    }

    xpr_sint_t sIndex;
    IndexDeque::iterator sIterator;

    mListCtrl.SetRedraw(XPR_FALSE);

    sIterator = sIndexDeque.begin();
    for (; sIterator != sIndexDeque.end(); ++sIterator)
    {
        sIndex = *sIterator;

        mListCtrl.DeleteItem(sIndex);

        sFileScrap.removeItem(sGroup->mGroupId, sIndex);
    }

    mListCtrl.SetRedraw();

    updateStatus();

    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_scrap.msg.removed_invalidated_items"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
    }

    sIndexDeque.clear();
}
