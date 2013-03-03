//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SearchDlg.h"

#include <math.h>                    // for animation

#include "fxb/fxb_fnmatch.h"        // for pattern matching
#include "fxb/fxb_ini_file_ex.h"
#include "fxb/fxb_search_file.h"

#include "SearchDlgObserver.h"
#include "MainFrame.h"
#include "ExplorerView.h"
#include "SearchResultPane.h"
#include "SearchResultCtrl.h"
#include "DlgStateMgr.h"
#include "DlgState.h"
#include "ConfDir.h"

#include "cmd/SearchLocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum SearchLocType
{
    SearchLocTypeNone = 0,
    SearchLocTypeItem,
    SearchLocTypeAllLocalDrive,
    SearchLocTypeUserDefine,
    SearchLocTypeUser,
    SearchLocTypeCustomBrowse,
    SearchLocTypeCustom,
};

typedef struct SearchLoc
{
    SearchLocType mType;
    union
    {
        LPITEMIDLIST        mFullPidl;
        xpr_tchar_t        *mPaths;
        fxb::SearchUserLoc *mUserLoc;
    };
} SearchLoc;

// user defined timer
enum
{
    TM_ID_ANIMATION = 0,
};

// user defined message
enum
{
    WM_FINALIZE = WM_USER + 100,
};

static const xpr_size_t kPaneWidth[] = { 150, 150 };
static const CPoint     kAnimationOffset = CPoint(0, 40);

static const xpr_tchar_t kDirCountKey [] = XPR_STRING_LITERAL("search_dir.dir_count");
static const xpr_tchar_t kDirKey      [] = XPR_STRING_LITERAL("search_dir.dir%d");
static const xpr_tchar_t kIncludeKey  [] = XPR_STRING_LITERAL("search_dir.dir%d_include");
static const xpr_tchar_t kSubFolderKey[] = XPR_STRING_LITERAL("search_dir.dir%d_sub_folder");

SearchDlg::SearchDlg(void)
    : super(IDD_SEARCH, XPR_NULL)
    , mObserver(XPR_NULL)
    , mSearchFile(XPR_NULL)
    , mAnimation(XPR_FALSE)
    , mDegree(0.0)
    , mCurSel(-1)
    , mDlgState(XPR_NULL)
    , mSearchResultViewIndex(-1)
    , mSearchResultPaneId(0)
{
}

SearchDlg::~SearchDlg(void)
{
    XPR_SAFE_DELETE(mSearchFile);
}

void SearchDlg::setObserver(SearchDlgObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t SearchDlg::Create(CWnd *aParentWnd)
{
    return super::Create(IDD_SEARCH, aParentWnd);
}

void SearchDlg::DoDataExchange(CDataExchange *aDX)
{
    super::DoDataExchange(aDX);

    DDX_Control(aDX, IDC_SEARCH_LOC,        mLocComboBox);
    DDX_Control(aDX, IDC_SEARCH_CUR_LOC,    mCurLocButton);
    DDX_Control(aDX, IDC_SEARCH_STATUS_BAR, mStatusBar);
}

BEGIN_MESSAGE_MAP(SearchDlg, super)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_SEARCH_LOC, OnSelchangeLocation)
    ON_NOTIFY(CBEN_DELETEITEM, IDC_SEARCH_LOC, OnDeleteitemLocation)
    ON_BN_CLICKED(IDC_SEARCH_CUR_LOC, OnCurLocation)
    ON_BN_CLICKED(IDC_SEARCH_START,   OnStart)
    ON_COMMAND(ID_TOOL_FILE_SEARCH,   OnFileSearch)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t SearchDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_SEARCH_NAME,         sizeResize, sizeNone);
    AddControl(IDC_SEARCH_TEXT,         sizeResize, sizeNone);
    AddControl(IDC_SEARCH_LOC,          sizeResize, sizeNone);
    AddControl(IDC_SEARCH_CUR_LOC,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_SEARCH_START,        sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDCANCEL,                sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_SEARCH_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.search.type.folder_and_file")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.search.type.only_folder")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.search.type.only_file")));

    enableWindow(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_SEARCH_SUBFOLDER))->SetCheck(2);
    ((CComboBox *)GetDlgItem(IDC_SEARCH_TYPE))->SetCurSel(0);

    mLocImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    mLocComboBox.SetImageList(&mLocImageList);

    HICON sIcon;
    sIcon = theApp.LoadIcon(MAKEINTRESOURCE(IDI_SEARCH));
    mLocImageList.Add(sIcon);
    ::DestroyIcon(sIcon);

    CComboBoxEx *pComboBoxEx;
    pComboBoxEx = (CComboBoxEx *)GetDlgItem(IDC_SEARCH_LOC);
    pComboBoxEx->ModifyStyle(0, CBS_AUTOHSCROLL);
    pComboBoxEx->GetEditCtrl()->LimitText(XPR_MAX_PATH);

    ::SHAutoComplete(pComboBoxEx->GetEditCtrl()->m_hWnd, SHACF_FILESYSTEM);

    ((CComboBox *)GetDlgItem(IDC_SEARCH_NAME))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_SEARCH_TEXT))->LimitText(XPR_MAX_PATH);
    pComboBoxEx->LimitText(XPR_MAX_PATH);

    xpr_sint_t sImage;
    xpr_sint_t sIndex = 0;
    COMBOBOXEXITEM sComboBoxExItem;
    xpr_tchar_t sName[XPR_MAX_PATH + 1];
    LPITEMIDLIST sFullPidl = XPR_NULL;
    SearchLoc *sSearchLoc;
    SHFILEINFO sShFileInfo = {0};

    // 1. 'Desktop' folder
    ::SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_DESKTOP, &sFullPidl);
    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

        sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mLocImageList.Add(sIcon);
        ::DestroyIcon(sIcon);

        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeItem;
        sSearchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // 2. 'My Documents' folder
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    if (SUCCEEDED(::SHGetDesktopFolder(&sShellFolder)))
    {
        sShellFolder->ParseDisplayName(
            XPR_NULL,
            XPR_NULL,
            XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"),
            XPR_NULL,
            &sFullPidl,
            XPR_NULL);

        COM_RELEASE(sShellFolder);
    }

    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

        sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mLocImageList.Add(sIcon);
        ::DestroyIcon(sIcon);

        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeItem;
        sSearchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // 3. 'Computer' folder
    ::SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_DRIVES, &sFullPidl);
    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

        sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mLocImageList.Add(sIcon);
        ::DestroyIcon(sIcon);

        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeItem;
        sSearchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    xpr_tchar_t sDriveStrings[XPR_MAX_PATH + 1] = {0};
    fxb::GetDriveStrings(sDriveStrings);

    if (_tcslen(sDriveStrings) > 0)
    {
        // 4. All the local drives
        xpr_sint_t i;
        xpr_tchar_t *sDrive;
        xpr_tchar_t sPaths[XPR_MAX_PATH + 1] = {0};

        _stprintf(sName, XPR_STRING_LITERAL("%s ("), theApp.loadString(XPR_STRING_LITERAL("popup.search.location.local_hard_drive")));

        sDrive = sDriveStrings;
        for (i = 0; ; ++i)
        {
            if (*sDrive == 0)
                break;

            if (GetDriveType(sDrive) == DRIVE_FIXED)
            {
                _stprintf(sName + _tcslen(sName), XPR_STRING_LITERAL("%c:;"), *sDrive);
                _stprintf(sPaths + _tcslen(sPaths), XPR_STRING_LITERAL("%c:;"), *sDrive);
            }

            sDrive += _tcslen(sDrive) + 1;
        }

        if (_tcslen(sName) > 0)
        {
            sName[_tcslen(sName) - 1] = '\0';
            sPaths[_tcslen(sPaths) - 1] = '\0';
            _tcscat(sName, XPR_STRING_LITERAL(")"));

            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sPath, XPR_STRING_LITERAL("%c:\\"), sPaths[0]);
            SHGetFileInfo(sPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

            sImage = mLocImageList.Add(sShFileInfo.hIcon);
            ::DestroyIcon(sShFileInfo.hIcon);

            sSearchLoc = new SearchLoc;
            sSearchLoc->mType = SearchLocTypeAllLocalDrive;
            sSearchLoc->mPaths = new xpr_tchar_t[_tcslen(sPaths) + 1];
            _tcscpy(sSearchLoc->mPaths, sPaths);

            memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
            sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
            sComboBoxExItem.iItem          = sIndex++;
            sComboBoxExItem.iImage         = sImage;
            sComboBoxExItem.iSelectedImage = sImage;
            sComboBoxExItem.iIndent        = 1;
            sComboBoxExItem.pszText        = sName;
            sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
            mLocComboBox.InsertItem(&sComboBoxExItem);
        }

        // 5. Each hard drive
        sDrive = sDriveStrings;
        for (i = 0; ; ++i)
        {
            if (*sDrive == 0)
                break;

            fxb::SHGetPidlFromPath(sDrive, &sFullPidl);
            if (sFullPidl != XPR_NULL)
                fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

            SHGetFileInfo(sDrive, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

            sImage = mLocImageList.Add(sShFileInfo.hIcon);
            ::DestroyIcon(sShFileInfo.hIcon);

            sSearchLoc = new SearchLoc;
            sSearchLoc->mType = SearchLocTypeItem;
            sSearchLoc->mFullPidl = sFullPidl;

            memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
            sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
            sComboBoxExItem.iItem          = sIndex++;
            sComboBoxExItem.iImage         = sImage;
            sComboBoxExItem.iSelectedImage = sImage;
            sComboBoxExItem.iIndent        = 1;
            sComboBoxExItem.pszText        = sName;
            sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
            mLocComboBox.InsertItem(&sComboBoxExItem);

            sDrive += _tcslen(sDrive) + 1;
        }
    }

    // 6. User defined locations
    {
        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeUserDefine;
        sSearchLoc->mUserLoc = XPR_NULL;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask    = CBEIF_TEXT | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem   = sIndex++;
        sComboBoxExItem.iIndent = -2;
        sComboBoxExItem.pszText = (xpr_tchar_t *)theApp.loadString(XPR_STRING_LITERAL("popup.search.location.user_defined"));
        sComboBoxExItem.lParam  = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // 7. Browse
    {
        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeCustomBrowse;
        sSearchLoc->mPaths = XPR_NULL;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask    = CBEIF_TEXT | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem   = sIndex++;
        sComboBoxExItem.iIndent = -2;
        sComboBoxExItem.pszText = (xpr_tchar_t *)theApp.loadString(XPR_STRING_LITERAL("popup.search.location.browse"));
        sComboBoxExItem.lParam  = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // user-defined location
    fxb::SearchUserLocDeque sUserLocDeque;
    loadUserLoc(&sUserLocDeque);
    updateUserLoc(&sUserLocDeque);
    sUserLocDeque.clear();

    mLocComboBox.SetCurSel(3);

    // status
    mStatusBar.addPane(1, XPR_NULL);
    mStatusBar.addPane(2, XPR_NULL);
    mStatusBar.setPaneSize(0, kPaneWidth[0], kPaneWidth[0]);
    mStatusBar.setPaneSize(1, kPaneWidth[1], kPaneWidth[1]);

    xpr_tchar_t sStatusText[0xff] = {0};
    xpr_tchar_t sElapsedTimeText[0xff] = {0};
    getStatusText(sStatusText, sElapsedTimeText);

    mStatusBar.setPaneText(0, sStatusText);
    mStatusBar.setDynamicPaneText(1, sElapsedTimeText, 0);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.search.title")));
    SetDlgItemText(IDC_SEARCH_LABEL_NAME,   theApp.loadString(XPR_STRING_LITERAL("popup.search.label.name")));
    SetDlgItemText(IDC_SEARCH_LABEL_TEXT,   theApp.loadString(XPR_STRING_LITERAL("popup.search.label.text")));
    SetDlgItemText(IDC_SEARCH_LABEL_LOC,    theApp.loadString(XPR_STRING_LITERAL("popup.search.label.location")));
    SetDlgItemText(IDC_SEARCH_GROUP_OPTION, theApp.loadString(XPR_STRING_LITERAL("popup.search.group.option")));
    SetDlgItemText(IDC_SEARCH_SUBFOLDER,    theApp.loadString(XPR_STRING_LITERAL("popup.search.check.sub-folder")));
    SetDlgItemText(IDC_SEARCH_CASE,         theApp.loadString(XPR_STRING_LITERAL("popup.search.check.case")));
    SetDlgItemText(IDC_SEARCH_NO_WILDCARD,  theApp.loadString(XPR_STRING_LITERAL("popup.search.check.no_wildcard")));
    SetDlgItemText(IDC_SEARCH_SYSTEM,       theApp.loadString(XPR_STRING_LITERAL("popup.search.check.system")));
    SetDlgItemText(IDC_SEARCH_LABEL_TYPE,   theApp.loadString(XPR_STRING_LITERAL("popup.search.label.type")));
    SetDlgItemText(IDC_SEARCH_START,        theApp.loadString(XPR_STRING_LITERAL("popup.search.button.search")));
    SetDlgItemText(IDCANCEL,                theApp.loadString(XPR_STRING_LITERAL("popup.search.button.close")));

    CenterWindow();

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("Search"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Search"), IDC_SEARCH_NAME);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Text"),       IDC_SEARCH_TEXT);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Sub Folder"), IDC_SEARCH_SUBFOLDER);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Case"),       IDC_SEARCH_CASE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Only"),       IDC_SEARCH_NO_WILDCARD);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("System"),     IDC_SEARCH_SYSTEM);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"),       IDC_SEARCH_TYPE);
        mDlgState->load();
    }

    mCurLocButton.SetTooltipText(theApp.loadString(XPR_STRING_LITERAL("popup.search.location.current_folder")));
    mCurLocButton.DrawTransparent(XPR_TRUE);
    mCurLocButton.SetIcon(IDI_EXPAND, IDI_EXPAND);

    return XPR_TRUE;
}

void SearchDlg::OnDestroy(void) 
{
    CComboBox *sNameCtrl = (CComboBox *)GetDlgItem(IDC_SEARCH_NAME);

    // Save Command
    DlgState::insertComboEditString(sNameCtrl, XPR_FALSE);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        CRect sRect;
        GetWindowRect(sRect);

        xpr_tchar_t sPos[0xff] = {0};
        _stprintf(sPos, XPR_STRING_LITERAL("%d,%d,%d,%d"), sRect.left, sRect.top, sRect.right, sRect.bottom);

        mDlgState->reset();
        mDlgState->setStateS(XPR_STRING_LITERAL("Window"), sPos);
        mDlgState->save();
    }

    super::OnDestroy();

    if (mSearchFile != XPR_NULL)
        mSearchFile->Stop();
}

void SearchDlg::OnClose(void) 
{
    DestroyWindow();
}

void SearchDlg::PostNcDestroy(void) 
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onPostNcDestroy(*this);
    }

    delete this;
}

void SearchDlg::enableWindow(xpr_bool_t aEnable)
{
    GetDlgItem(IDC_SEARCH_START)->SetWindowText(
        (aEnable == XPR_TRUE) ?
        theApp.loadString(XPR_STRING_LITERAL("popup.search.button.search")) : theApp.loadString(XPR_STRING_LITERAL("popup.search.button.stop")));

    GetDlgItem(IDC_SEARCH_SUBFOLDER  )->EnableWindow(aEnable);
    GetDlgItem(IDC_SEARCH_CASE       )->EnableWindow(aEnable);
    GetDlgItem(IDC_SEARCH_NO_WILDCARD)->EnableWindow(aEnable);
    GetDlgItem(IDC_SEARCH_TYPE       )->EnableWindow(aEnable);
    GetDlgItem(IDC_SEARCH_SYSTEM     )->EnableWindow(aEnable);
    GetDlgItem(IDC_SEARCH_CUR_LOC    )->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL              )->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_NULL)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE) sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }
}

void SearchDlg::OnSelchangeLocation(void) 
{
    xpr_sint_t sIndex = mLocComboBox.GetCurSel();
    if (sIndex < 0)
        return;

    SearchLoc *sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(sIndex);
    if (sSearchLoc == XPR_NULL)
        return;

    switch (sSearchLoc->mType)
    {
    case SearchLocTypeUserDefine:
        {
            fxb::SearchUserLocDeque sUserLocDeque;
            getUserLoc(&sUserLocDeque);

            SearchLocDlg sDlg;
            sDlg.setSearchLoc(&sUserLocDeque);
            sUserLocDeque.clear();

            if (sDlg.DoModal() == IDOK)
            {
                fxb::SearchUserLocDeque *sUpdatedUserLocDeque;
                sUpdatedUserLocDeque = sDlg.getSearchLoc(XPR_TRUE);

                saveUserLoc(sUpdatedUserLocDeque);
                updateUserLoc(sUpdatedUserLocDeque);

                XPR_SAFE_DELETE(sUpdatedUserLocDeque);
            }

            break;
        }

    case SearchLocTypeCustomBrowse:
        {
            BROWSEINFO sBrowseInfo = {0};
            sBrowseInfo.hwndOwner = GetSafeHwnd();
            sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
            sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.search.location.browse.title"));

            LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
            if (sFullPidl != XPR_NULL)
            {
                if (insertLocation(sFullPidl) == XPR_FALSE)
                    COM_FREE(sFullPidl);
            }

            break;
        }
    }

    mLocComboBox.SetFocus();
}

void SearchDlg::loadUserLoc(fxb::SearchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeSearchDir, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return;

    xpr_sint_t              i;
    xpr_sint_t              sDirCount;
    xpr_tchar_t             sKey[XPR_MAX_PATH + 1];
    fxb::SearchUserLoc     *sUserLoc;
    fxb::SearchUserLocItem *sUserLocItem;
    const xpr_tchar_t      *sDir;
    fxb::IniFile::Section  *sSection;
    fxb::IniFile::SectionIterator sSectionIterator;

    sSectionIterator = sIniFile.beginSection();
    for (; sSectionIterator != sIniFile.endSection(); ++sSectionIterator)
    {
        sSection = *sSectionIterator;
        XPR_ASSERT(sSection != XPR_NULL);

        sUserLoc = new fxb::SearchUserLoc;
        sUserLoc->mName = sIniFile.getSectionName(sSection);

        sDirCount = sIniFile.getValueI(sSection, kDirCountKey, 0);
        sDirCount = min(sDirCount, MAX_SEARCH_LOC);

        for (i = 0; i < sDirCount; ++i)
        {
            _stprintf(sKey, kDirKey, i + 1);

            sDir = sIniFile.getValueS(sSection, sKey, XPR_NULL);
            if (sDir == XPR_NULL)
                continue;

            sUserLocItem = new fxb::SearchUserLocItem;
            sUserLocItem->mPath = sDir;

            if (sUserLocItem->mPath.length() == 2)
                sUserLocItem->mPath += XPR_STRING_LITERAL("\\");

            // include
            _stprintf(sKey, kIncludeKey, i + 1);
            sUserLocItem->mInclude = sIniFile.getValueB(sSection, sKey, XPR_TRUE);

            // sub folder
            _stprintf(sKey, kSubFolderKey, i + 1);
            sUserLocItem->mSubFolder = sIniFile.getValueB(sSection, sKey, XPR_FALSE);

            sUserLoc->mUserLocDeque.push_back(sUserLocItem);
        }

        aUserLocDeque->push_back(sUserLoc);
    }
}

void SearchDlg::saveUserLoc(fxb::SearchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeSearchDir, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer search directory file"));

    xpr_sint_t              i;
    xpr_tchar_t             sKey[XPR_MAX_PATH + 1];
    fxb::IniFile::Section  *sSection;
    fxb::SearchUserLoc     *sUserLoc;
    fxb::SearchUserLocItem *sUserLocItem;
    fxb::SearchUserLocDeque::iterator sIterator;
    fxb::UserLocDeque::iterator sLocIterator;

    sIterator = aUserLocDeque->begin();
    for (; sIterator != aUserLocDeque->end(); ++sIterator)
    {
        sUserLoc = *sIterator;
        XPR_ASSERT(sUserLoc != XPR_NULL);

        sSection = sIniFile.addSection(sUserLoc->mName.c_str());
        XPR_ASSERT(sSection != XPR_NULL);

        sIniFile.setValueI(sSection, kDirCountKey, (xpr_sint_t)sUserLoc->mUserLocDeque.size());

        sLocIterator = sUserLoc->mUserLocDeque.begin();
        for (i = 0; sLocIterator != sUserLoc->mUserLocDeque.end(); ++sLocIterator, ++i)
        {
            sUserLocItem = *sLocIterator;
            XPR_ASSERT(sUserLocItem != XPR_NULL);

            // directory
            _stprintf(sKey, kDirKey, i + 1);
            sIniFile.setValueS(sSection, sKey, sUserLocItem->mPath);

            // include
            if (sUserLocItem->mInclude == XPR_FALSE)
            {
                _stprintf(sKey, kIncludeKey, i + 1);
                sIniFile.setValueB(sSection, sKey, sUserLocItem->mInclude);
            }

            // sub folder
            if (sUserLocItem->mSubFolder == XPR_TRUE)
            {
                _stprintf(sKey, kSubFolderKey, i + 1);
                sIniFile.setValueB(sSection, sKey, sUserLocItem->mSubFolder);
            }
        }
    }

    sIniFile.writeFile(xpr::CharSetUtf16);
}

void SearchDlg::getUserLoc(fxb::SearchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    SearchLoc *sSearchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
        if (sSearchLoc == XPR_NULL)
            continue;

        if (sSearchLoc->mType == SearchLocTypeUser)
            aUserLocDeque->push_back(sSearchLoc->mUserLoc);
    }
}

void SearchDlg::updateUserLoc(fxb::SearchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    SearchLoc *sSearchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
        if (sSearchLoc == XPR_NULL)
            continue;

        if (sSearchLoc->mType == SearchLocTypeUser)
            mLocComboBox.DeleteItem(i);
    }

    xpr_sint_t sIndex;

    sIndex = sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
        if (sSearchLoc == XPR_NULL)
            continue;

        if (sSearchLoc->mType == SearchLocTypeUserDefine)
        {
            sIndex = i + 1;
            break;
        }
    }

    fxb::SearchUserLoc *sUserLoc;
    fxb::SearchUserLocDeque::iterator sIterator;

    sIterator = aUserLocDeque->begin();
    for (; sIterator != aUserLocDeque->end(); ++sIterator)
    {
        sUserLoc = *sIterator;
        if (sUserLoc == XPR_NULL)
            continue;

        sSearchLoc = new SearchLoc;
        sSearchLoc->mType = SearchLocTypeUser;
        sSearchLoc->mUserLoc = sUserLoc;

        COMBOBOXEXITEM sComboBoxExItem = {0};
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = 0;
        sComboBoxExItem.iSelectedImage = 0;
        sComboBoxExItem.iIndent        = 1;
        sComboBoxExItem.pszText        = (xpr_tchar_t *)sUserLoc->mName.c_str();
        sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }
}

xpr_bool_t SearchDlg::insertLocation(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(m_hWnd))
        return XPR_FALSE;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    HRESULT sHResult = ::SHGetPathFromIDList(aFullPidl, sPath);
    if (FAILED(sHResult))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    ::SHGetDesktopFolder(&sShellFolder);

    xpr_sint_t i, sCount;
    SearchLoc *sSearchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
        if (sSearchLoc == XPR_NULL)
            continue;

        if (sSearchLoc->mType != SearchLocTypeItem)
            continue;

        if (sSearchLoc->mFullPidl == XPR_NULL)
            continue;

        if (sShellFolder->CompareIDs(0, aFullPidl, sSearchLoc->mFullPidl) == 0)
        {
            mLocComboBox.SetCurSel(i);
            break;
        }
    }

    COM_RELEASE(sShellFolder);

    if (i != sCount)
        return XPR_FALSE;

    SHFILEINFO sShFileInfo = {0};
    SHGetFileInfo(sPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

    xpr_sint_t sImage;
    sImage = mLocImageList.Add(sShFileInfo.hIcon);
    ::DestroyIcon(sShFileInfo.hIcon);

    sSearchLoc = new SearchLoc;
    sSearchLoc->mType = SearchLocTypeItem;
    sSearchLoc->mFullPidl = aFullPidl;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = i;
    sComboBoxExItem.iImage         = sImage;
    sComboBoxExItem.iSelectedImage = sImage;
    sComboBoxExItem.iIndent        = 1;
    sComboBoxExItem.pszText        = sPath;
    sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;
    mLocComboBox.SetCurSel(mLocComboBox.InsertItem(&sComboBoxExItem));

    return XPR_TRUE;
}

xpr_sint_t SearchDlg::insertLocation(const xpr_tchar_t *aPaths)
{
    if (aPaths == XPR_NULL)
        return -1;

    if (XPR_IS_NULL(m_hWnd))
        return -1;

    xpr_sint_t i, sCount;
    SearchLoc *sSearchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
        if (sSearchLoc == XPR_NULL)
            continue;

        if (sSearchLoc->mType != SearchLocTypeCustom)
            continue;

        if (sSearchLoc->mPaths == XPR_NULL)
            continue;

        if (_tcsicmp(aPaths, sSearchLoc->mPaths) == 0)
        {
            mLocComboBox.SetCurSel(i);
            return i;
        }
    }

    sSearchLoc = new SearchLoc;
    sSearchLoc->mType = SearchLocTypeCustom;
    sSearchLoc->mPaths = new xpr_tchar_t[_tcslen(aPaths) + 1];
    _tcscpy(sSearchLoc->mPaths, aPaths);

    xpr_tchar_t sIconPath[XPR_MAX_PATH + 1];

    xpr_tchar_t *sSplit = _tcsrchr(sSearchLoc->mPaths, XPR_STRING_LITERAL(';'));
    if (sSplit != XPR_NULL)
    {
        ::SHGetSpecialFolderPath(XPR_NULL, sIconPath, CSIDL_WINDOWS, XPR_FALSE);
    }
    else
    {
        _tcscpy(sIconPath, sSearchLoc->mPaths);
        if (_tcslen(sIconPath) == 2)
            _tcscat(sIconPath, XPR_STRING_LITERAL("\\"));
    }

    SHFILEINFO sShFileInfo = {0};
    ::SHGetFileInfo(sIconPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

    xpr_sint_t sImage;
    sImage = mLocImageList.Add(sShFileInfo.hIcon);
    ::DestroyIcon(sShFileInfo.hIcon);

    if (sSplit != XPR_NULL)
        *sSplit = XPR_STRING_LITERAL(';');

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = i;
    sComboBoxExItem.iImage         = sImage;
    sComboBoxExItem.iSelectedImage = sImage;
    sComboBoxExItem.iIndent        = 1;
    sComboBoxExItem.pszText        = sSearchLoc->mPaths;
    sComboBoxExItem.lParam         = (LPARAM)sSearchLoc;

    xpr_sint_t sIndex;
    sIndex = mLocComboBox.InsertItem(&sComboBoxExItem);

    mLocComboBox.SetCurSel(sIndex);

    return sIndex;
}

static xpr_size_t MultiStr2List(xpr_tchar_t *aMultiStr, std::deque<std::tstring> &aStrDeque)
{
    xpr_sint_t i;
    std::tstring sStr;
    std::tstring::iterator sIterator;

    xpr_tchar_t *sMultiStr = aMultiStr;

    for (i = 0; aMultiStr[i]; ++i)
    {
        if (aMultiStr[i] != XPR_STRING_LITERAL(';'))
            continue;

        aMultiStr[i] = XPR_STRING_LITERAL('\0');

        sStr = sMultiStr;
        if (sStr.empty() == false)
            aStrDeque.push_back(sStr);

        aMultiStr[i] = XPR_STRING_LITERAL(';');

        sMultiStr = aMultiStr + i + 1;
    }

    sStr = sMultiStr;
    if (sStr.empty() == false)
        aStrDeque.push_back(sStr);

    return aStrDeque.size();
}

static xpr_size_t ValidateTarget(std::deque<std::tstring> &aTargetDeque)
{
    xpr_sint_t sLen;
    std::deque<std::tstring>::iterator sIterator;
    std::tstring::iterator sStrIterator;

    sIterator = aTargetDeque.begin();
    while (sIterator != aTargetDeque.end())
    {
        sLen = (xpr_sint_t)sIterator->length();
        if (sLen < 2)
        {
            sIterator = aTargetDeque.erase(sIterator);
            continue;
        }

        if (sLen == 2 || sLen == 3)
        {
            // drive (c:\\ -> c:)
            if (sLen == 3 && sIterator->at(1) == XPR_STRING_LITERAL(':') && sIterator->at(2) == XPR_STRING_LITERAL('\\'))
                sIterator->erase(--sIterator->end());
        }
        else
        {
            // remove last backsplash
            sStrIterator = --sIterator->end();
            if (*sStrIterator == XPR_STRING_LITERAL('\\'))
                sIterator->erase(sStrIterator);

            if (fxb::IsExistFile(sIterator->c_str()) == XPR_FALSE)
            {
                sIterator = aTargetDeque.erase(sIterator);
                continue;
            }
        }

        sIterator++;
    }

    return aTargetDeque.size();
}

void WINAPI OnSearchResult(const xpr_tchar_t *aFolder, WIN32_FIND_DATA *aWin32FindData, LPARAM lParam)
{
    SearchResultCtrl *sSearchResultCtrl = (SearchResultCtrl *)lParam;
    if (XPR_IS_NULL(sSearchResultCtrl))
        return;

    sSearchResultCtrl->onSearch(aFolder, aWin32FindData);
}

SearchResultCtrl *SearchDlg::getSearchResultCtrl(void) const
{
    ExplorerView *sExplorerView = gFrame->getExplorerView(mSearchResultViewIndex);
    if (XPR_IS_NULL(sExplorerView))
        return XPR_NULL;

    xpr_sint_t sTab = sExplorerView->findTab(mSearchResultPaneId);

    SearchResultPane *sSearchResultPane = dynamic_cast<SearchResultPane *>(sExplorerView->getTabPane(sTab));
    if (XPR_IS_NULL(sSearchResultPane))
        return XPR_NULL;

    return sSearchResultPane->getSearchResultCtrl();
}

void SearchDlg::OnStart(void) 
{
    if (mSearchFile != XPR_NULL)
    {
        if (mSearchFile->getStatus() == fxb::SearchFile::StatusSearching)
        {
            mSearchFile->Stop();
            return;
        }
    }

    ExplorerView *sExplorerView = gFrame->getExplorerView();
    if (XPR_IS_NOT_NULL(sExplorerView))
    {
        xpr_sint_t sTab = sExplorerView->newSearchResultTab();

        sExplorerView->setCurTab(sTab);

        mSearchResultViewIndex = sExplorerView->getViewIndex();
        mSearchResultPaneId = sExplorerView->getTabPaneId(sTab);
    }

    SearchResultCtrl *sSearchResultCtrl = getSearchResultCtrl();

    //----------------------------------------------------------------------
    // options
    //----------------------------------------------------------------------
    xpr_bool_t sSubFolder  = ((CButton *)GetDlgItem(IDC_SEARCH_SUBFOLDER))->GetCheck();
    xpr_bool_t sCase       = ((CButton *)GetDlgItem(IDC_SEARCH_CASE))->GetCheck();
    xpr_bool_t sNoWildcard = ((CButton *)GetDlgItem(IDC_SEARCH_NO_WILDCARD))->GetCheck();
    xpr_bool_t sSystem     = ((CButton *)GetDlgItem(IDC_SEARCH_SYSTEM))->GetCheck();
    xpr_sint_t sType       = ((CComboBox *)GetDlgItem(IDC_SEARCH_TYPE))->GetCurSel();

    xpr_uint_t sFlags = 0;
    if (sSubFolder   == XPR_TRUE) sFlags |= fxb::SearchFile::FlagSubFolder;
    if (sCase        == XPR_TRUE) sFlags |= fxb::SearchFile::FlagCase;
    if (sSystem      == XPR_TRUE) sFlags |= fxb::SearchFile::FlagSystem;
    if (sType == 0 || sType == 1) sFlags |= fxb::SearchFile::FlagTypeFolder;
    if (sType == 0 || sType == 2) sFlags |= fxb::SearchFile::FlagTypeFile;

    //----------------------------------------------------------------------
    // locations
    //----------------------------------------------------------------------

    SearchLoc *sSearchLoc = XPR_NULL;
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();

    xpr_bool_t sLocation = XPR_FALSE;
    if (sCurSel >= 0 && !mLocComboBox.HasEditChanged())
    {
        sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(sCurSel);
        if (sSearchLoc != XPR_NULL)
            sLocation = XPR_TRUE;
    }
    else
    {
        xpr_sint_t sLen = mLocComboBox.GetEditCtrl()->GetWindowTextLength();
        xpr_tchar_t *sPath = new xpr_tchar_t[sLen + 2];
        if (sPath == XPR_NULL)
            return;

        mLocComboBox.GetEditCtrl()->GetWindowText(sPath, sLen + 1);

        xpr_sint_t i;
        xpr_sint_t sCount;
        xpr_tchar_t *sText;

        sCount = mLocComboBox.GetCount();
        for (i = 0; i < sCount; ++i)
        {
            sLen = mLocComboBox.GetLBTextLen(i);
            sText = new xpr_tchar_t[sLen + 1];
            mLocComboBox.GetLBText(i, sText);

            if (_tcsicmp(sPath, sText) == 0)
            {
                mLocComboBox.SetCurSel(i);
                sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(i);
                sLocation = XPR_TRUE;
            }

            XPR_SAFE_DELETE_ARRAY(sText);

            if (sLocation == XPR_TRUE)
                break;
        }

        if (sSearchLoc == XPR_NULL)
        {
            std::deque<std::tstring> sPathDeque;

            MultiStr2List(sPath, sPathDeque);
            ValidateTarget(sPathDeque);

            if (sPathDeque.empty() == false)
            {
                std::tstring strPaths;
                std::deque<std::tstring>::iterator sIterator;

                sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    strPaths += *sIterator;
                    strPaths += XPR_STRING_LITERAL(";");
                }

                strPaths[strPaths.length()-1] = XPR_STRING_LITERAL('\0');

                xpr_sint_t sIndex = insertLocation(strPaths.c_str());
                if (sIndex != -1)
                {
                    mLocComboBox.SetCurSel(sIndex);
                    sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData(sIndex);
                    sLocation = XPR_TRUE;
                }
            }

            sPathDeque.clear();
        }

        XPR_SAFE_DELETE_ARRAY(sPath);
    }

    if (sLocation == XPR_FALSE)
    {
        xpr_sint_t sLen = mLocComboBox.GetEditCtrl()->GetWindowTextLength();
        xpr_tchar_t *sPath = new xpr_tchar_t[sLen + 2];

        mLocComboBox.GetEditCtrl()->GetWindowText(sPath, sLen + 1);

        xpr_tchar_t sMsg[0xff + XPR_MAX_PATH] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.msg.wrong_location"), XPR_STRING_LITERAL("%s")), sPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        mLocComboBox.SetFocus();

        XPR_SAFE_DELETE_ARRAY(sPath);

        return;
    }

    SearchDirDeque sIncDirDeque;
    SearchDirDeque sExcDirDeque;

    SearchDir *sSearchDir;
    SearchDirDeque::iterator sIterator;

    if (sSearchLoc != XPR_NULL)
    {
        switch (sSearchLoc->mType)
        {
        case SearchLocTypeItem:
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sSearchLoc->mFullPidl, SHGDN_FORPARSING, sPath);

                if (_tcsncmp(sPath, XPR_STRING_LITERAL("::{"), 3) == 0) // 'My Computer' folder
                {
                    xpr_tchar_t sDriveStrings[XPR_MAX_PATH + 1];
                    fxb::GetDriveStrings(sDriveStrings);

                    xpr_sint_t i;
                    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
                    const xpr_tchar_t *sDrive = sDriveStrings;

                    for (i = 0; sDrive[i]; ++i)
                    {
                        _stprintf(sPath, XPR_STRING_LITERAL("%c:"), *sDrive);

                        sSearchDir = new SearchDir;
                        sSearchDir->mPath = sPath;
                        sSearchDir->mSubFolder = sSubFolder;

                        sIncDirDeque.push_back(sSearchDir);

                        sDrive += _tcslen(sDriveStrings) + 1;
                    }
                }
                else
                {
                    sSearchDir = new SearchDir;
                    sSearchDir->mPath = sPath;
                    sSearchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSearchDir);
                }

                break;
            }

        case SearchLocTypeAllLocalDrive:
            {
                std::deque<std::tstring> sPathDeque;
                MultiStr2List(sSearchLoc->mPaths, sPathDeque);

                ValidateTarget(sPathDeque);

                std::deque<std::tstring>::iterator sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    sSearchDir = new SearchDir;
                    sSearchDir->mPath = *sIterator;
                    sSearchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSearchDir);
                }
                break;
            }

        case SearchLocTypeUser:
            {
                fxb::UserLocDeque::iterator sIterator;
                fxb::SearchUserLocItem *sUserLocItem;

                sIterator = sSearchLoc->mUserLoc->mUserLocDeque.begin();
                for (; sIterator != sSearchLoc->mUserLoc->mUserLocDeque.end(); ++sIterator)
                {
                    sUserLocItem = *sIterator;
                    if (sUserLocItem == XPR_NULL)
                        continue;

                    sSearchDir = new SearchDir;
                    sSearchDir->mPath = sUserLocItem->mPath;
                    sSearchDir->mSubFolder = sUserLocItem->mSubFolder;

                    if (sUserLocItem->mInclude == XPR_TRUE)
                        sIncDirDeque.push_back(sSearchDir);
                    else
                        sExcDirDeque.push_back(sSearchDir);
                }

                break;
            }

        case SearchLocTypeCustom:
            {
                std::deque<std::tstring> sPathDeque;
                MultiStr2List(sSearchLoc->mPaths, sPathDeque);

                ValidateTarget(sPathDeque);

                std::deque<std::tstring>::iterator sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    sSearchDir = new SearchDir;
                    sSearchDir->mPath = *sIterator;
                    sSearchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSearchDir);
                }
                break;
            }
        }
    }

    //----------------------------------------------------------------------
    // find what
    //----------------------------------------------------------------------
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    CComboBox *sNameCtrl = (CComboBox *)GetDlgItem(IDC_SEARCH_NAME);
    xpr_sint_t sNameCurSel = sNameCtrl->GetCurSel();
    if (sNameCurSel != CB_ERR)
        sNameCtrl->GetLBText(sNameCurSel, sName);
    else
        sNameCtrl->GetWindowText(sName, XPR_MAX_PATH);

    DlgState::insertComboEditString(sNameCtrl, XPR_FALSE);

    //----------------------------------------------------------------------
    // text in file
    //----------------------------------------------------------------------
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_SEARCH_TEXT, sText, XPR_MAX_PATH);

    //----------------------------------------------------------------------
    // start search
    //----------------------------------------------------------------------
    sSearchResultCtrl->onStartSearch();

    XPR_SAFE_DELETE(mSearchFile);

    mSearchFile = new fxb::SearchFile;
    mSearchFile->setOwner(m_hWnd, WM_FINALIZE);

    mSearchFile->setFlags(sFlags);
    mSearchFile->setName(sName, sNoWildcard);
    mSearchFile->setText(sText);
    mSearchFile->setResultFunc(OnSearchResult);
    mSearchFile->setData((LPARAM)sSearchResultCtrl);

    sIterator = sIncDirDeque.begin();
    for (; sIterator != sIncDirDeque.end(); ++sIterator)
    {
        sSearchDir = *sIterator;
        if (sSearchDir == XPR_NULL)
            continue;

        mSearchFile->addIncludeDir(sSearchDir->mPath.c_str(), sSearchDir->mSubFolder);

        XPR_SAFE_DELETE(sSearchDir);
    }

    sIterator = sExcDirDeque.begin();
    for (; sIterator != sExcDirDeque.end(); ++sIterator)
    {
        sSearchDir = *sIterator;
        if (sSearchDir == XPR_NULL)
            continue;

        mSearchFile->addExcludeDir(sSearchDir->mPath.c_str(), sSearchDir->mSubFolder);

        XPR_SAFE_DELETE(sSearchDir);
    }

    sIncDirDeque.clear();
    sExcDirDeque.clear();

    if (mSearchFile->Start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        startAnimation();
    }
}

void SearchDlg::OnCancel(void)
{
    if (mSearchFile != XPR_NULL)
    {
        if (mSearchFile->getStatus() == fxb::SearchFile::StatusSearching)
        {
            mSearchFile->Stop();
        }
    }

    DestroyWindow();
}

LRESULT SearchDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    stopAnimation();
    enableWindow(XPR_TRUE);

    SearchResultCtrl *sSearchResultCtrl = getSearchResultCtrl();
    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
    {
        sSearchResultCtrl->onEndSearch();
    }

    xpr_tchar_t sStatusText[0xff] = {0};
    xpr_tchar_t sStatusElapsedTimeText[0xff] = {0};
    getStatusText(sStatusText, sStatusElapsedTimeText);

    mStatusBar.setPaneText(0, sStatusText);
    mStatusBar.setDynamicPaneText(1, sStatusElapsedTimeText, 0);

    xpr_tchar_t sItemCountText[0xff] = {0};
    xpr_tchar_t sElapsedMinuteTimeText[0xff] = {0};
    xpr_tchar_t sElapsedSecondTimeText[0xff] = {0};

    getStatusNumberFormat(
        sItemCountText, XPR_COUNT_OF(sItemCountText) - 1,
        sElapsedMinuteTimeText, XPR_COUNT_OF(sElapsedMinuteTimeText) - 1,
        sElapsedSecondTimeText, XPR_COUNT_OF(sElapsedSecondTimeText) - 1);

    xpr::tstring sMsg;
    if (sElapsedMinuteTimeText[0] == 0)
    {
        sMsg.format(
            theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.msg.search_result"),
            XPR_STRING_LITERAL("%s,%s")),
            sItemCountText,
            sElapsedSecondTimeText);
    }
    else
    {
        sMsg.format(
            theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.msg.search_long_result"),
            XPR_STRING_LITERAL("%s,%s,%s")),
            sItemCountText,
            sElapsedMinuteTimeText,
            sElapsedSecondTimeText);
    }

    MessageBox(sMsg.c_str(), XPR_NULL, MB_OK | MB_ICONINFORMATION);

    return 0;
}

void SearchDlg::getStatusNumberFormat(xpr_tchar_t *aItemCountText,
                                      xpr_size_t   aMaxItemCountTextLen,
                                      xpr_tchar_t *aElapsedMinuteTimeText,
                                      xpr_size_t   aMaxElapsedMinuteTimeTextLen,
                                      xpr_tchar_t *aElapsedSecondTimeText,
                                      xpr_size_t   aMaxElapsedSecondTimeTextLen)
{
    clock_t sSearchTime = 0;
    if (mSearchFile != XPR_NULL)
        mSearchFile->getStatus(XPR_NULL, &sSearchTime);

    SearchResultCtrl *sSearchResultCtrl = getSearchResultCtrl();

    xpr_sint_t sCount = 0;
    if (XPR_IS_NOT_NULL(sSearchResultCtrl))
        sCount = sSearchResultCtrl->GetItemCount();

    if (XPR_IS_NOT_NULL(aItemCountText))
    {
        fxb::GetFormatedNumber(sCount, aItemCountText, aMaxItemCountTextLen);
    }

    if (XPR_IS_NOT_NULL(aElapsedMinuteTimeText) || XPR_IS_NOT_NULL(aElapsedSecondTimeText))
    {
        xpr_double_t sElapsedSeconds = ((xpr_double_t)sSearchTime) / CLOCKS_PER_SEC;
        if (sElapsedSeconds == 0.0)
        {
            if (XPR_IS_NOT_NULL(aElapsedMinuteTimeText))
                *aElapsedMinuteTimeText = 0;

            _stprintf(aElapsedSecondTimeText, XPR_STRING_LITERAL("%.0f"), sElapsedSeconds);
        }
        else if (sElapsedSeconds < 60.0 || XPR_IS_NULL(aElapsedMinuteTimeText))
        {
            if (XPR_IS_NOT_NULL(aElapsedMinuteTimeText))
                *aElapsedMinuteTimeText = 0;

            _stprintf(aElapsedSecondTimeText, XPR_STRING_LITERAL("%.2f"), sElapsedSeconds);
        }
        else
        {
            xpr_sint_t   sMinutes = (xpr_sint_t)(sElapsedSeconds / 60.0);
            xpr_double_t sSeconds = sElapsedSeconds - (60 * sMinutes);

            _stprintf(aElapsedSecondTimeText, XPR_STRING_LITERAL("%d"),   sMinutes);
            _stprintf(aElapsedSecondTimeText, XPR_STRING_LITERAL("%.2f"), sSeconds);
        }
    }
}

void SearchDlg::getStatusText(xpr_tchar_t *aStatusText, xpr_tchar_t *aElapsedTimeText)
{
    xpr_tchar_t sItemCountText[0xff] = {0};
    xpr_tchar_t sElapsedMinuteTimeText[0xff] = {0};
    xpr_tchar_t sElapsedSecondTimeText[0xff] = {0};

    getStatusNumberFormat(
        sItemCountText, XPR_COUNT_OF(sItemCountText) - 1,
        sElapsedMinuteTimeText, XPR_COUNT_OF(sElapsedMinuteTimeText) - 1,
        sElapsedSecondTimeText, XPR_COUNT_OF(sElapsedSecondTimeText) - 1);

    _stprintf(aStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.status.count"), XPR_STRING_LITERAL("%s")), sItemCountText);

    if (sElapsedMinuteTimeText[0] == 0)
    {
        _stprintf(aElapsedTimeText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.status.elapsed_time"), XPR_STRING_LITERAL("%s")), sElapsedSecondTimeText);
    }
    else
    {
        _stprintf(aElapsedTimeText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.search.status.elapsed_long_time"), XPR_STRING_LITERAL("%s,%s")), sElapsedMinuteTimeText, sElapsedSecondTimeText);
    }
}

xpr_bool_t SearchDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            if (pMsg->hwnd == mLocComboBox.GetEditCtrl()->m_hWnd)
            {
                if (mCurSel == -1)
                    mCurSel = mLocComboBox.GetCurSel();

                if (mCurSel != -1)
                    mLocComboBox.SetCurSel(mCurSel);

                return XPR_TRUE;
            }
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void SearchDlg::OnDeleteitemLocation(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMCOMBOBOXEX *sNmComboBoxEx = (NMCOMBOBOXEX *)pNMHDR;
    *pResult = 0;

    SearchLoc *sSearchLoc = (SearchLoc *)mLocComboBox.GetItemData((xpr_sint_t)sNmComboBoxEx->ceItem.iItem);
    if (sSearchLoc != XPR_NULL)
    {
        switch (sSearchLoc->mType)
        {
        case SearchLocTypeUser:
            {
                if (sSearchLoc->mUserLoc != XPR_NULL)
                    sSearchLoc->mUserLoc->clear();
                XPR_SAFE_DELETE(sSearchLoc->mUserLoc);
                break;
            }

        case SearchLocTypeAllLocalDrive:
        case SearchLocTypeCustom:
            {
                XPR_SAFE_DELETE(sSearchLoc->mPaths);
                break;
            }

        case SearchLocTypeItem:
            {
                COM_FREE(sSearchLoc->mFullPidl);
                break;
            }
        }

        XPR_SAFE_DELETE(sSearchLoc);
    }
}

void SearchDlg::OnPaint(void)
{
    CPaintDC sPaintDc(this);

    if (mAnimation == XPR_TRUE)
    {
        CRect sRect, sCloseButtonRect;
        GetDlgItem(IDCANCEL)->GetWindowRect(&sCloseButtonRect);
        ScreenToClient(&sCloseButtonRect);

        CBitmap sBitmap;
        sBitmap.LoadBitmap(IDB_SEARCH);
        BITMAP sBitmapHeader = {0};
        sBitmap.GetBitmap(&sBitmapHeader);

        GetClientRect(&sRect);
        sRect.top  = sCloseButtonRect.top  + kAnimationOffset.y + (sBitmapHeader.bmHeight + 6);
        sRect.left = sCloseButtonRect.left + kAnimationOffset.x + (sBitmapHeader.bmWidth  + 6);

        xpr_double_t sDegree = 360.f - mDegree;
        xpr_double_t sRadian = sDegree * 3.141592 / 180.f;

        CPoint sPoint;
        sPoint.x = (xpr_slong_t)(20.f * cos(sRadian) +  0.f * sin(sRadian) + .5f);
        sPoint.y = (xpr_slong_t)( 0.f * cos(sRadian) - 20.f * sin(sRadian) + .5f);

        sRect.left += sPoint.x;
        sRect.top  += sPoint.y;

        CDC sTmpDc;
        sTmpDc.CreateCompatibleDC(&sPaintDc);

        CBitmap *sOldBitmap = sTmpDc.SelectObject(&sBitmap);

        ::TransparentBlt(
            sPaintDc.m_hDC,
            sRect.left, sRect.top, sBitmapHeader.bmWidth, sBitmapHeader.bmHeight,
            sTmpDc,
            0, 0, sBitmapHeader.bmWidth, sBitmapHeader.bmHeight,
            RGB(255,0,255));

        sTmpDc.SelectObject(sOldBitmap);

        sBitmap.DeleteObject();
    }
}

void SearchDlg::startAnimation(void)
{
    mDegree = 0.f;
    mAnimation = XPR_TRUE;
    SetTimer(TM_ID_ANIMATION, 50, XPR_NULL);
}

void SearchDlg::stopAnimation(void)
{
    mDegree = 0.f;
    mAnimation = XPR_FALSE;
    KillTimer(TM_ID_ANIMATION);

    Invalidate();
    UpdateWindow();
}

void SearchDlg::OnTimer(xpr_uint_t aIdEvent) 
{
    if (aIdEvent == TM_ID_ANIMATION)
    {
        CRect sRect, sCloseButtonRect;
        GetDlgItem(IDCANCEL)->GetWindowRect(&sCloseButtonRect);
        ScreenToClient(&sCloseButtonRect);

        GetClientRect(&sRect);
        sRect.top    = sCloseButtonRect.top  + kAnimationOffset.y;
        sRect.left   = sCloseButtonRect.left + kAnimationOffset.x;
        sRect.bottom = sRect.top + 100;

        mDegree += 10.0;
        if (mDegree >= 360.0)
            mDegree = 0.0;

        InvalidateRect(sRect);
        UpdateWindow();
    }

    super::OnTimer(aIdEvent);
}

void SearchDlg::OnFileSearch(void) 
{
    gFrame->SendMessage(WM_COMMAND, ID_TOOL_FILE_SEARCH, (LPARAM)gFrame->m_hWnd);
}

void SearchDlg::OnCurLocation(void)
{
    gFrame->SendMessage(WM_COMMAND, ID_EDIT_SEARCH_LOCATION, (LPARAM)gFrame->m_hWnd);
}
