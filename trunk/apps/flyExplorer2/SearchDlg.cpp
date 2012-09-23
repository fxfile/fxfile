//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
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
#include "fxb/fxb_ini_file.h"
#include "fxb/fxb_search_file.h"

#include "MainFrame.h"
#include "DlgState.h"
#include "CfgPath.h"

#include "cmd/SchLocDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum SchLocType
{
    SchLocTypeNone = 0,
    SchLocTypeItem,
    SchLocTypeAllLocalDrive,
    SchLocTypeUserDefine,
    SchLocTypeUser,
    SchLocTypeCustomBrowse,
    SchLocTypeCustom,
};

typedef struct SchLoc
{
    SchLocType mType;
    union
    {
        LPITEMIDLIST     mFullPidl;
        xpr_tchar_t     *mPaths;
        fxb::SchUserLoc *mUserLoc;
    };
} SchLoc;

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

SearchDlg::SearchDlg(void)
    : super(IDD_SEARCH, XPR_NULL)
    , mSearchFile(XPR_NULL)
    , mResultCtrl(XPR_NULL)
    , mAnimation(XPR_FALSE)
    , mDegree(0.0)
    , mCurSel(-1)
    , mAccelTable(XPR_NULL)
{
}

SearchDlg::~SearchDlg()
{
    XPR_SAFE_DELETE(mSearchFile);
}

void SearchDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SEARCH_LOC,     mLocComboBox);
    DDX_Control(pDX, IDC_SEARCH_CUR_LOC, mCurLocButton);
}

BEGIN_MESSAGE_MAP(SearchDlg, super)
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_SEARCH_LOC, OnSelchangeLocation)
    ON_NOTIFY(CBEN_DELETEITEM, IDC_SEARCH_LOC, OnDeleteitemLocation)
    ON_BN_CLICKED(IDC_SEARCH_CUR_LOC, OnCurLocation)
    ON_BN_CLICKED(IDC_SEARCH_START,   OnStart)
    ON_BN_CLICKED(IDC_SEARCH_STOP,    OnStop)
    ON_COMMAND(ID_VIEW_BAR_SEARCH,    OnViewBarSearch)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t SearchDlg::Create(CWnd* pParentWnd)
{
    return super::Create(IDD_SEARCH, pParentWnd);
}

HBRUSH SearchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor) 
{
    HBRUSH hbr = super::OnCtlColor(pDC, pWnd, nCtlColor);

    pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
    pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));

    if (mBrush.m_hObject == XPR_NULL)
        mBrush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

    return mBrush;
}

xpr_bool_t SearchDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_SEARCH_NAME,    sizeResize, sizeNone);
    AddControl(IDC_SEARCH_TEXT,    sizeResize, sizeNone);
    AddControl(IDC_SEARCH_LOC,     sizeResize, sizeNone);
    AddControl(IDC_SEARCH_CUR_LOC, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_SEARCH_START,   sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_SEARCH_STOP,    sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_SEARCH_LINE,    sizeResize, sizeNone);
    AddControl(IDC_SEARCH_LINE2,   sizeResize, sizeNone);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_SEARCH_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("bar.search.type.folder_and_file")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("bar.search.type.only_folder")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("bar.search.type.only_file")));

    enableWindow(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_SEARCH_SUBFOLDER))->SetCheck(2);
    ((CComboBox *)GetDlgItem(IDC_SEARCH_TYPE))->SetCurSel(0);
    mResultCtrl = ((SearchBar *)GetParent())->getSearchResultCtrl();

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
    SchLoc *sSchLoc;
    SHFILEINFO sShFileInfo = {0};

    // 1. 'Desktop' folder
    ::SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_DESKTOP, &sFullPidl);
    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

        sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mLocImageList.Add(sIcon);
        ::DestroyIcon(sIcon);

        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeItem;
        sSchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // 2. 'My Documents' folder
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    ::SHGetDesktopFolder(&sShellFolder);
    sShellFolder->ParseDisplayName(XPR_NULL, XPR_NULL, XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"), XPR_NULL, &sFullPidl, XPR_NULL);
    COM_RELEASE(sShellFolder);
    if (sFullPidl != XPR_NULL)
    {
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName);

        sIcon = fxb::GetItemIcon(sFullPidl);
        sImage = mLocImageList.Add(sIcon);
        ::DestroyIcon(sIcon);

        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeItem;
        sSchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
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

        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeItem;
        sSchLoc->mFullPidl = sFullPidl;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = sImage;
        sComboBoxExItem.iSelectedImage = sImage;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
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

        _stprintf(sName, XPR_STRING_LITERAL("%s ("), theApp.loadString(XPR_STRING_LITERAL("bar.search.location.local_hard_drive")));

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

            sSchLoc = new SchLoc;
            sSchLoc->mType = SchLocTypeAllLocalDrive;
            sSchLoc->mPaths = new xpr_tchar_t[_tcslen(sPaths) + 1];
            _tcscpy(sSchLoc->mPaths, sPaths);

            memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
            sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
            sComboBoxExItem.iItem          = sIndex++;
            sComboBoxExItem.iImage         = sImage;
            sComboBoxExItem.iSelectedImage = sImage;
            sComboBoxExItem.iIndent        = 1;
            sComboBoxExItem.pszText        = sName;
            sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
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

            sSchLoc = new SchLoc;
            sSchLoc->mType = SchLocTypeItem;
            sSchLoc->mFullPidl = sFullPidl;

            memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
            sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
            sComboBoxExItem.iItem          = sIndex++;
            sComboBoxExItem.iImage         = sImage;
            sComboBoxExItem.iSelectedImage = sImage;
            sComboBoxExItem.iIndent        = 1;
            sComboBoxExItem.pszText        = sName;
            sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
            mLocComboBox.InsertItem(&sComboBoxExItem);

            sDrive += _tcslen(sDrive) + 1;
        }
    }

    // 6. User defined locations
    {
        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeUserDefine;
        sSchLoc->mUserLoc = XPR_NULL;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask    = CBEIF_TEXT | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem   = sIndex++;
        sComboBoxExItem.iIndent = -2;
        sComboBoxExItem.pszText = (xpr_tchar_t *)theApp.loadString(XPR_STRING_LITERAL("bar.search.location.user_defined"));
        sComboBoxExItem.lParam  = (LPARAM)sSchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // 7. Browse
    {
        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeCustomBrowse;
        sSchLoc->mPaths = XPR_NULL;

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask    = CBEIF_TEXT | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem   = sIndex++;
        sComboBoxExItem.iIndent = -2;
        sComboBoxExItem.pszText = (xpr_tchar_t *)theApp.loadString(XPR_STRING_LITERAL("bar.search.location.browse"));
        sComboBoxExItem.lParam  = (LPARAM)sSchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }

    // user-defined location
    fxb::SchUserLocDeque sUserLocDeque;
    loadUserLoc(&sUserLocDeque);
    updateUserLoc(&sUserLocDeque);
    sUserLocDeque.clear();

    mLocComboBox.SetCurSel(3);

    // update status
    {
        xpr_tchar_t sStatusText[0xff] = {0};
        xpr_tchar_t sElapsedTimeText[0xff] = {0};

        getStatusText(sStatusText, sElapsedTimeText);

        SetDlgItemText(IDC_SEARCH_STATUS, sStatusText);
        SetDlgItemText(IDC_SEARCH_TIME, sElapsedTimeText);
    }

    createAccelTable();

    SetDlgItemText(IDC_SEARCH_LABEL_NAME,   theApp.loadString(XPR_STRING_LITERAL("bar.search.label.name")));
    SetDlgItemText(IDC_SEARCH_LABEL_TEXT,   theApp.loadString(XPR_STRING_LITERAL("bar.search.label.text")));
    SetDlgItemText(IDC_SEARCH_LABEL_LOC,    theApp.loadString(XPR_STRING_LITERAL("bar.search.label.location")));
    SetDlgItemText(IDC_SEARCH_START,        theApp.loadString(XPR_STRING_LITERAL("bar.search.button.search")));
    SetDlgItemText(IDC_SEARCH_STOP,         theApp.loadString(XPR_STRING_LITERAL("bar.search.button.stop")));
    SetDlgItemText(IDC_SEARCH_LABEL_OPTION, theApp.loadString(XPR_STRING_LITERAL("bar.search.label.option")));
    SetDlgItemText(IDC_SEARCH_SUBFOLDER,    theApp.loadString(XPR_STRING_LITERAL("bar.search.check.sub-folder")));
    SetDlgItemText(IDC_SEARCH_CASE,         theApp.loadString(XPR_STRING_LITERAL("bar.search.check.case")));
    SetDlgItemText(IDC_SEARCH_NO_WILDCARD,  theApp.loadString(XPR_STRING_LITERAL("bar.search.check.no_wildcard")));
    SetDlgItemText(IDC_SEARCH_SYSTEM,       theApp.loadString(XPR_STRING_LITERAL("bar.search.check.system")));
    SetDlgItemText(IDC_SEARCH_LABEL_TYPE,   theApp.loadString(XPR_STRING_LITERAL("bar.search.label.type")));
    SetDlgItemText(IDC_SEARCH_LABEL_RESULT, theApp.loadString(XPR_STRING_LITERAL("bar.search.label.result")));
    SetDlgItemText(IDC_SEARCH_LABEL_TIME,   theApp.loadString(XPR_STRING_LITERAL("bar.search.label.elapsed_time")));

    mState.setDialog(this);
    mState.setSection(XPR_STRING_LITERAL("Search"));
    mState.setComboBoxList(XPR_STRING_LITERAL("Search"), IDC_SEARCH_NAME);
    mState.setEditCtrl(XPR_STRING_LITERAL("Text"),       IDC_SEARCH_TEXT);
    mState.setCheckBox(XPR_STRING_LITERAL("Sub Folder"), IDC_SEARCH_SUBFOLDER);
    mState.setCheckBox(XPR_STRING_LITERAL("Case"),       IDC_SEARCH_CASE);
    mState.setCheckBox(XPR_STRING_LITERAL("Only"),       IDC_SEARCH_NO_WILDCARD);
    mState.setCheckBox(XPR_STRING_LITERAL("System"),     IDC_SEARCH_SYSTEM);
    mState.setComboBox(XPR_STRING_LITERAL("Type"),       IDC_SEARCH_TYPE);
    mState.load();

    mCurLocButton.SetTooltipText(theApp.loadString(XPR_STRING_LITERAL("bar.search.location.current_folder")));
    mCurLocButton.DrawTransparent(XPR_TRUE);
    mCurLocButton.SetIcon(IDI_EXPAND, IDI_EXPAND);

    return XPR_TRUE;
}

void SearchDlg::createAccelTable(void)
{
    xpr_sint_t sIds[] = { ID_VIEW_BAR_SEARCH };
    xpr_sint_t sIdCount = XPR_COUNTOF(sIds);

    ACCEL sMainAccel[MAX_ACCEL] = {0};
    HACCEL sMainAccelHandle = gFrame->getAccelTable();
    xpr_sint_t nMainAccel = ::CopyAcceleratorTable(sMainAccelHandle, sMainAccel, MAX_ACCEL);

    ACCEL sAccel[MAX_ACCEL] = {0};
    xpr_sint_t i, j, sCount = 0;
    for (i = 0; i < sIdCount; ++i)
    {
        for (j = 0; j < nMainAccel; ++j)
        {
            if (sIds[i] == sMainAccel[j].cmd)
            {
                sAccel[sCount].cmd   = sMainAccel[j].cmd;
                sAccel[sCount].fVirt = sMainAccel[j].fVirt;
                sAccel[sCount].key   = sMainAccel[j].key;
                sCount++;
            }
        }
    }

    if (mAccelTable != XPR_NULL)
        ::DestroyAcceleratorTable(mAccelTable);

    mAccelTable = ::CreateAcceleratorTable(sAccel, sCount);
}

void SearchDlg::setNameFocus(void)
{
    GetDlgItem(IDC_SEARCH_NAME)->SetFocus();
}

xpr_bool_t SearchDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
    HCURSOR sCursor = ::LoadCursor(XPR_NULL, IDC_ARROW);

    cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, sCursor, 0, 0);

    if (sCursor != XPR_NULL)
        ::DestroyCursor(sCursor);

    return super::PreCreateWindow(cs);
}

xpr_bool_t SearchDlg::OnEraseBkgnd(CDC* pDC)
{
    return super::OnEraseBkgnd(pDC);
}

void SearchDlg::OnLButtonDown(xpr_uint_t nFlags, CPoint point) 
{
    GetDlgItem(IDC_SEARCH_NAME)->SetFocus();
    super::OnLButtonDown(nFlags, point);
}

void SearchDlg::enableWindow(xpr_bool_t bEnable)
{
    GetDlgItem(IDC_SEARCH_SUBFOLDER  )->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_CASE       )->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_NO_WILDCARD)->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_START      )->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_TYPE       )->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_SYSTEM     )->EnableWindow(bEnable);
    GetDlgItem(IDC_SEARCH_STOP       )->EnableWindow(!bEnable);
    GetDlgItem(IDC_SEARCH_CUR_LOC    )->EnableWindow(bEnable);
}

void SearchDlg::OnSelchangeLocation(void) 
{
    xpr_sint_t sIndex = mLocComboBox.GetCurSel();
    if (sIndex < 0)
        return;

    SchLoc *sSchLoc = (SchLoc *)mLocComboBox.GetItemData(sIndex);
    if (sSchLoc == XPR_NULL)
        return;

    switch (sSchLoc->mType)
    {
    case SchLocTypeUserDefine:
        {
            fxb::SchUserLocDeque sUserLocDeque;
            getUserLoc(&sUserLocDeque);

            SchLocDlg sDlg;
            sDlg.setSchLoc(&sUserLocDeque);
            sUserLocDeque.clear();

            if (sDlg.DoModal() == IDOK)
            {
                fxb::SchUserLocDeque *sUpdatedUserLocDeque;
                sUpdatedUserLocDeque = sDlg.getSchLoc(XPR_TRUE);

                saveUserLoc(sUpdatedUserLocDeque);
                updateUserLoc(sUpdatedUserLocDeque);

                XPR_SAFE_DELETE(sUpdatedUserLocDeque);
            }

            break;
        }

    case SchLocTypeCustomBrowse:
        {
            BROWSEINFO sBrowseInfo = {0};
            sBrowseInfo.hwndOwner = GetSafeHwnd();
            sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;// | BIF_USENEWUI;
            sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("bar.search.location.browse.title"));

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

void SearchDlg::loadUserLoc(fxb::SchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getLoadPath(CfgPath::TypeSearchDir, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);
    if (sIniFile.readFileW() == XPR_FALSE)
        return;

    xpr_size_t i, j;
    xpr_size_t sKeyCount;
    xpr_size_t sEntryCount;
    xpr_tchar_t sEntry[XPR_MAX_PATH + 1];
    fxb::SchUserLoc *sUserLoc;
    fxb::SchUserLocItem *sUserLocItem;
    const xpr_tchar_t *sKey;
    const xpr_tchar_t *sDir;

    sKeyCount = sIniFile.getKeyCount();
    for (i = 0; i < sKeyCount; ++i)
    {
        sKey = sIniFile.getKeyName(i);
        if (sKey == XPR_NULL)
            continue;

        sUserLoc = new fxb::SchUserLoc;
        if (sUserLoc == XPR_NULL)
            continue;

        sUserLoc->mName = sKey;

        sEntryCount = sIniFile.getEntryCount(sKey);
        sEntryCount = min(sEntryCount, MAX_SCH_LOC);
        for (j = 0; j < sEntryCount; ++j)
        {
            // directory
            _stprintf(sEntry, XPR_STRING_LITERAL("Dir%d"), j+1);

            sDir = sIniFile.getValueS(sKey, sEntry);
            if (sDir == XPR_NULL)
                continue;

            sUserLocItem = new fxb::SchUserLocItem;
            sUserLocItem->mPath = sDir;

            if (sUserLocItem->mPath.length() == 2)
                sUserLocItem->mPath += XPR_STRING_LITERAL("\\");

            // exclude
            _stprintf(sEntry, XPR_STRING_LITERAL("Include%d"), j+1);
            sUserLocItem->mInclude = sIniFile.getValueB(sKey, sEntry, XPR_TRUE);

            // sub folder
            _stprintf(sEntry, XPR_STRING_LITERAL("SubFolder%d"), j+1);
            sUserLocItem->mSubFolder = sIniFile.getValueB(sKey, sEntry, XPR_FALSE);

            sUserLoc->mUserLocDeque.push_back(sUserLocItem);
        }

        aUserLocDeque->push_back(sUserLoc);
    }
}

void SearchDlg::saveUserLoc(fxb::SchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeSearchDir, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Search Directory File"));

    xpr_sint_t i, j;
    const xpr_tchar_t *sKey;
    xpr_tchar_t sEntry[XPR_MAX_PATH + 1];
    fxb::SchUserLoc *sUserLoc;
    fxb::SchUserLocItem *sUserLocItem;
    fxb::SchUserLocDeque::iterator sIterator;
    fxb::UserLocDeque::iterator sLocIterator;

    sIterator = aUserLocDeque->begin();
    for (i = 0; sIterator != aUserLocDeque->end(); ++sIterator)
    {
        sUserLoc = *sIterator;
        if (sUserLoc == XPR_NULL)
            continue;

        sKey = sUserLoc->mName.c_str();

        sLocIterator = sUserLoc->mUserLocDeque.begin();
        for (j = 0; sLocIterator != sUserLoc->mUserLocDeque.end(); ++sLocIterator, ++j)
        {
            sUserLocItem = *sLocIterator;

            // directory
            _stprintf(sEntry, XPR_STRING_LITERAL("Dir%d"), j+1);
            sIniFile.setValueS(sKey, sEntry, sUserLocItem->mPath.c_str());

            // exclude
            if (sUserLocItem->mInclude == XPR_FALSE)
            {
                _stprintf(sEntry, XPR_STRING_LITERAL("Include%d"), j+1);
                sIniFile.setValueB(sKey, sEntry, sUserLocItem->mInclude);
            }

            // sub folder
            if (sUserLocItem->mSubFolder == XPR_TRUE)
            {
                _stprintf(sEntry, XPR_STRING_LITERAL("SubFolder%d"), j+1);
                sIniFile.setValueB(sKey, sEntry, sUserLocItem->mSubFolder);
            }
        }

        i++;
    }

    sIniFile.writeFileW();
}

void SearchDlg::getUserLoc(fxb::SchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    SchLoc *sSchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
        if (sSchLoc == XPR_NULL)
            continue;

        if (sSchLoc->mType == SchLocTypeUser)
            aUserLocDeque->push_back(sSchLoc->mUserLoc);
    }
}

void SearchDlg::updateUserLoc(fxb::SchUserLocDeque *aUserLocDeque)
{
    if (aUserLocDeque == XPR_NULL)
        return;

    xpr_sint_t i, sCount;
    SchLoc *sSchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
        if (sSchLoc == XPR_NULL)
            continue;

        if (sSchLoc->mType == SchLocTypeUser)
            mLocComboBox.DeleteItem(i);
    }

    xpr_sint_t sIndex;

    sIndex = sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
        if (sSchLoc == XPR_NULL)
            continue;

        if (sSchLoc->mType == SchLocTypeUserDefine)
        {
            sIndex = i + 1;
            break;
        }
    }

    fxb::SchUserLoc *sUserLoc;
    fxb::SchUserLocDeque::iterator sIterator;

    sIterator = aUserLocDeque->begin();
    for (; sIterator != aUserLocDeque->end(); ++sIterator)
    {
        sUserLoc = *sIterator;
        if (sUserLoc == XPR_NULL)
            continue;

        sSchLoc = new SchLoc;
        sSchLoc->mType = SchLocTypeUser;
        sSchLoc->mUserLoc = sUserLoc;

        COMBOBOXEXITEM sComboBoxExItem = {0};
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = sIndex++;
        sComboBoxExItem.iImage         = 0;
        sComboBoxExItem.iSelectedImage = 0;
        sComboBoxExItem.iIndent        = 1;
        sComboBoxExItem.pszText        = (xpr_tchar_t *)sUserLoc->mName.c_str();
        sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
        mLocComboBox.InsertItem(&sComboBoxExItem);
    }
}

xpr_bool_t SearchDlg::insertLocation(LPITEMIDLIST aFullPidl)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    HRESULT sHResult = ::SHGetPathFromIDList(aFullPidl, sPath);
    if (FAILED(sHResult))
        return XPR_FALSE;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    ::SHGetDesktopFolder(&sShellFolder);

    xpr_sint_t i, sCount;
    SchLoc *sSchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
        if (sSchLoc == XPR_NULL)
            continue;

        if (sSchLoc->mType != SchLocTypeItem)
            continue;

        if (sSchLoc->mFullPidl == XPR_NULL)
            continue;

        if (sShellFolder->CompareIDs(0, aFullPidl, sSchLoc->mFullPidl) == 0)
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

    sSchLoc = new SchLoc;
    sSchLoc->mType = SchLocTypeItem;
    sSchLoc->mFullPidl = aFullPidl;

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = i;
    sComboBoxExItem.iImage         = sImage;
    sComboBoxExItem.iSelectedImage = sImage;
    sComboBoxExItem.iIndent        = 1;
    sComboBoxExItem.pszText        = sPath;
    sComboBoxExItem.lParam         = (LPARAM)sSchLoc;
    mLocComboBox.SetCurSel(mLocComboBox.InsertItem(&sComboBoxExItem));

    return XPR_TRUE;
}

xpr_sint_t SearchDlg::insertLocation(const xpr_tchar_t *aPaths)
{
    if (aPaths == XPR_NULL)
        return -1;

    xpr_sint_t i, sCount;
    SchLoc *sSchLoc;

    sCount = mLocComboBox.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
        if (sSchLoc == XPR_NULL)
            continue;

        if (sSchLoc->mType != SchLocTypeCustom)
            continue;

        if (sSchLoc->mPaths == XPR_NULL)
            continue;

        if (_tcsicmp(aPaths, sSchLoc->mPaths) == 0)
        {
            mLocComboBox.SetCurSel(i);
            return i;
        }
    }

    sSchLoc = new SchLoc;
    sSchLoc->mType = SchLocTypeCustom;
    sSchLoc->mPaths = new xpr_tchar_t[_tcslen(aPaths) + 1];
    _tcscpy(sSchLoc->mPaths, aPaths);

    xpr_tchar_t sIconPath[XPR_MAX_PATH + 1];

    xpr_tchar_t *sSplit = _tcsrchr(sSchLoc->mPaths, XPR_STRING_LITERAL(';'));
    if (sSplit != XPR_NULL)
    {
        ::SHGetSpecialFolderPath(XPR_NULL, sIconPath, CSIDL_WINDOWS, XPR_FALSE);
    }
    else
    {
        _tcscpy(sIconPath, sSchLoc->mPaths);
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
    sComboBoxExItem.pszText        = sSchLoc->mPaths;
    sComboBoxExItem.lParam         = (LPARAM)sSchLoc;

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

void WINAPI OnSearchResult(const xpr_tchar_t *aFolder, WIN32_FIND_DATA *aWfd, LPARAM lParam)
{
    SearchResultCtrl *pResultCtrl = (SearchResultCtrl *)lParam;
    if (!pResultCtrl)
        return;

    pResultCtrl->onSearch(aFolder, aWfd);
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

    mResultCtrl = ((SearchBar *)GetParent())->getSearchResultCtrl();

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

    SchLoc *sSchLoc = XPR_NULL;
    xpr_sint_t sCurSel = mLocComboBox.GetCurSel();

    xpr_bool_t sLocation = XPR_FALSE;
    if (sCurSel >= 0 && !mLocComboBox.HasEditChanged())
    {
        sSchLoc = (SchLoc *)mLocComboBox.GetItemData(sCurSel);
        if (sSchLoc != XPR_NULL)
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
                sSchLoc = (SchLoc *)mLocComboBox.GetItemData(i);
                sLocation = XPR_TRUE;
            }

            XPR_SAFE_DELETE_ARRAY(sText);

            if (sLocation == XPR_TRUE)
                break;
        }

        if (sSchLoc == XPR_NULL)
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
                    sSchLoc = (SchLoc *)mLocComboBox.GetItemData(sIndex);
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
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("bar.search.msg.wrong_location"), XPR_STRING_LITERAL("%s")), sPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        mLocComboBox.SetFocus();

        XPR_SAFE_DELETE_ARRAY(sPath);

        return;
    }

    SchDirDeque sIncDirDeque;
    SchDirDeque sExcDirDeque;

    LPSCHDIR sSchDir;
    SchDirDeque::iterator sIterator;

    if (sSchLoc != XPR_NULL)
    {
        switch (sSchLoc->mType)
        {
        case SchLocTypeItem:
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sSchLoc->mFullPidl, SHGDN_FORPARSING, sPath);

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

                        sSchDir = new SchDir;
                        sSchDir->mPath = sPath;
                        sSchDir->mSubFolder = sSubFolder;

                        sIncDirDeque.push_back(sSchDir);

                        sDrive += _tcslen(sDriveStrings) + 1;
                    }
                }
                else
                {
                    sSchDir = new SchDir;
                    sSchDir->mPath = sPath;
                    sSchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSchDir);
                }

                break;
            }

        case SchLocTypeAllLocalDrive:
            {
                std::deque<std::tstring> sPathDeque;
                MultiStr2List(sSchLoc->mPaths, sPathDeque);

                ValidateTarget(sPathDeque);

                std::deque<std::tstring>::iterator sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    sSchDir = new SchDir;
                    sSchDir->mPath = *sIterator;
                    sSchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSchDir);
                }
                break;
            }

        case SchLocTypeUser:
            {
                fxb::UserLocDeque::iterator sIterator;
                fxb::SchUserLocItem *sUserLocItem;

                sIterator = sSchLoc->mUserLoc->mUserLocDeque.begin();
                for (; sIterator != sSchLoc->mUserLoc->mUserLocDeque.end(); ++sIterator)
                {
                    sUserLocItem = *sIterator;
                    if (sUserLocItem == XPR_NULL)
                        continue;

                    sSchDir = new SchDir;
                    sSchDir->mPath = sUserLocItem->mPath;
                    sSchDir->mSubFolder = sUserLocItem->mSubFolder;

                    if (sUserLocItem->mInclude == XPR_TRUE)
                        sIncDirDeque.push_back(sSchDir);
                    else
                        sExcDirDeque.push_back(sSchDir);
                }

                break;
            }

        case SchLocTypeCustom:
            {
                std::deque<std::tstring> sPathDeque;
                MultiStr2List(sSchLoc->mPaths, sPathDeque);

                ValidateTarget(sPathDeque);

                std::deque<std::tstring>::iterator sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    sSchDir = new SchDir;
                    sSchDir->mPath = *sIterator;
                    sSchDir->mSubFolder = sSubFolder;

                    sIncDirDeque.push_back(sSchDir);
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
    mResultCtrl->onStartSearch();

    if (gFrame->mWorkingBar.IsWindowVisible() == XPR_FALSE)
        gFrame->ShowControlBar(&gFrame->mWorkingBar, XPR_TRUE, XPR_FALSE);
    gFrame->mSearchBar.EnableTracking(XPR_FALSE);

    XPR_SAFE_DELETE(mSearchFile);

    mSearchFile = new fxb::SearchFile;
    mSearchFile->setOwner(m_hWnd, WM_FINALIZE);

    mSearchFile->setFlags(sFlags);
    mSearchFile->setName(sName, sNoWildcard);
    mSearchFile->setText(sText);
    mSearchFile->setResultFunc(OnSearchResult);
    mSearchFile->setData((LPARAM)mResultCtrl);

    sIterator = sIncDirDeque.begin();
    for (; sIterator != sIncDirDeque.end(); ++sIterator)
    {
        sSchDir = *sIterator;
        if (sSchDir == XPR_NULL)
            continue;

        mSearchFile->addIncludeDir(sSchDir->mPath.c_str(), sSchDir->mSubFolder);

        XPR_SAFE_DELETE(sSchDir);
    }

    sIterator = sExcDirDeque.begin();
    for (; sIterator != sExcDirDeque.end(); ++sIterator)
    {
        sSchDir = *sIterator;
        if (sSchDir == XPR_NULL)
            continue;

        mSearchFile->addExcludeDir(sSchDir->mPath.c_str(), sSchDir->mSubFolder);

        XPR_SAFE_DELETE(sSchDir);
    }

    sIncDirDeque.clear();
    sExcDirDeque.clear();

    if (mSearchFile->Start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        startAnimation();
        GetDlgItem(IDC_SEARCH_STOP)->SetFocus();
    }
}

LRESULT SearchDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    stopAnimation();
    enableWindow(XPR_TRUE);
    gFrame->mSearchBar.EnableTracking(XPR_TRUE);

    mResultCtrl->onEndSearch();

    xpr_tchar_t sStatusText[0xff] = {0};
    xpr_tchar_t sElapsedTimeText[0xff] = {0};

    getStatusText(sStatusText, sElapsedTimeText);

    SetDlgItemText(IDC_SEARCH_STATUS, sStatusText);
    SetDlgItemText(IDC_SEARCH_TIME, sElapsedTimeText);

    MessageBox(sStatusText, XPR_NULL, MB_OK | MB_ICONINFORMATION);

    return 0;
}

void SearchDlg::getStatusText(xpr_tchar_t *aStatusText, xpr_tchar_t *aElapsedTimeText)
{
    clock_t sSearchTime = 0;
    if (mSearchFile != XPR_NULL)
        mSearchFile->getStatus(XPR_NULL, &sSearchTime);

    xpr_sint_t sCount = 0;
    if (mResultCtrl != XPR_NULL)
        sCount = mResultCtrl->GetItemCount();

    if (aStatusText != XPR_NULL)
    {
        xpr_tchar_t sCountText[0xff] = {0};
        fxb::GetFormatedNumber(sCount, sCountText, 0xfe);

        _stprintf(aStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("bar.search.status.count"), XPR_STRING_LITERAL("%s")), sCountText);
    }

    if (aElapsedTimeText != XPR_NULL)
    {
        xpr_double_t sSeconds = ((xpr_double_t)sSearchTime) / CLOCKS_PER_SEC;
        if (sSeconds == 0.0)
        {
            _stprintf(aElapsedTimeText, theApp.loadFormatString(XPR_STRING_LITERAL("bar.search.status.elapsed_time"), XPR_STRING_LITERAL("%.0f")), sSeconds);
        }
        else if (sSeconds < 60.0)
        {
            _stprintf(aElapsedTimeText, theApp.loadFormatString(XPR_STRING_LITERAL("bar.search.status.elapsed_time"), XPR_STRING_LITERAL("%.2f")), sSeconds);
        }
        else
        {
            xpr_sint_t sMinutes = (xpr_sint_t)(sSeconds / 60.0);
            _stprintf(aElapsedTimeText, theApp.loadFormatString(XPR_STRING_LITERAL("bar.search.status.elapsed_long_time"), XPR_STRING_LITERAL("%d,%.2f")), sMinutes, sSeconds - (60 * sMinutes));
        }
    }
}

void SearchDlg::OnStop(void) 
{
    if (mSearchFile != XPR_NULL)
        mSearchFile->Stop();
}

void SearchDlg::OnDestroy(void) 
{
    CComboBox *sNameCtrl = (CComboBox *)GetDlgItem(IDC_SEARCH_NAME);

    // Save Command
    DlgState::insertComboEditString(sNameCtrl, XPR_FALSE);

    mState.reset();
    mState.save();

    super::OnDestroy();

    if (mSearchFile != XPR_NULL)
        mSearchFile->Stop();
}

xpr_bool_t SearchDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_SYSKEYDOWN)
    {
        if (pMsg->wParam == VK_F4)
        {
            if (GetAsyncKeyState(VK_MENU) < 0)
            {
                gFrame->PostMessage(WM_CLOSE);
                return XPR_TRUE;
            }
        }
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        //if ((GetKeyState(VK_CONTROL) < 0 && pMsg->wParam == 'E') ||
        //    (GetKeyState(VK_CONTROL) < 0 && pMsg->wParam == 'e') ||
        //    pMsg->wParam == VK_F3)
        //{
        //    gFrame->ShowControlBar(&gFrame->mSearchBar, XPR_FALSE, XPR_FALSE);
        //    return XPR_TRUE;
        //}

        if (pMsg->wParam == VK_ESCAPE)
        {
            if (pMsg->hwnd == mLocComboBox.GetEditCtrl()->m_hWnd)
            {
                if (mCurSel == -1)
                    mCurSel = mLocComboBox.GetCurSel();

                if (mCurSel != -1)
                    mLocComboBox.SetCurSel(mCurSel);
            }

            return XPR_TRUE;
        }

        if (pMsg->wParam == VK_RETURN)
        {
            OnStart();
            return XPR_TRUE;
        }
    }

    if (mAccelTable != XPR_NULL)
    {
        if (::TranslateAccelerator(m_hWnd, mAccelTable, pMsg))
            return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

void SearchDlg::OnDeleteitemLocation(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMCOMBOBOXEX *sNmComboBoxEx = (NMCOMBOBOXEX *)pNMHDR;
    *pResult = 0;

    SchLoc *sSchLoc = (SchLoc *)mLocComboBox.GetItemData((xpr_sint_t)sNmComboBoxEx->ceItem.iItem);
    if (sSchLoc != XPR_NULL)
    {
        switch (sSchLoc->mType)
        {
        case SchLocTypeUser:
            {
                if (sSchLoc->mUserLoc != XPR_NULL)
                    sSchLoc->mUserLoc->clear();
                XPR_SAFE_DELETE(sSchLoc->mUserLoc);
                break;
            }

        case SchLocTypeAllLocalDrive:
        case SchLocTypeCustom:
            {
                XPR_SAFE_DELETE(sSchLoc->mPaths);
                break;
            }

        case SchLocTypeItem:
            {
                COM_FREE(sSchLoc->mFullPidl);
                break;
            }
        }

        XPR_SAFE_DELETE(sSchLoc);
    }
}

void SearchDlg::OnPaint(void)
{
    CPaintDC sPaintDc(this);

    if (mAnimation == XPR_TRUE)
    {
        CRect sRect, sRect2;
        GetDlgItem(IDC_SEARCH_TIME)->GetWindowRect(&sRect2);
        ScreenToClient(&sRect2);

        CBitmap sBitmap;
        sBitmap.LoadBitmap(IDB_SEARCH);
        BITMAP sBitmapHeader = {0};
        sBitmap.GetBitmap(&sBitmapHeader);

        GetClientRect(&sRect);
        sRect.top  = sRect2.top  + (20 + sBitmapHeader.bmHeight + 6);
        sRect.left = sRect.right - (20 + sBitmapHeader.bmWidth  + 6);

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
        ::TransparentBlt(sPaintDc.m_hDC, sRect.left, sRect.top, sBitmapHeader.bmWidth, sBitmapHeader.bmHeight,
            sTmpDc, 0, 0, sBitmapHeader.bmWidth, sBitmapHeader.bmHeight, RGB(255,0,255));
        sTmpDc.SelectObject(sOldBitmap);
        sBitmap.DeleteObject();
    }
}

void SearchDlg::startAnimation(void)
{
    mDegree = 0.f;
    mAnimation = XPR_TRUE;
    SetTimer(TM_ID_ANIMATION, 100, XPR_NULL);
}

void SearchDlg::stopAnimation(void)
{
    mDegree = 0.f;
    mAnimation = XPR_FALSE;
    KillTimer(TM_ID_ANIMATION);

    Invalidate();
    UpdateWindow();
}

void SearchDlg::OnTimer(xpr_uint_t nIDEvent) 
{
    if (nIDEvent == TM_ID_ANIMATION)
    {
        CRect sRect, sRect2;
        GetDlgItem(IDC_SEARCH_TIME)->GetWindowRect(&sRect2);
        ScreenToClient(&sRect2);

        GetClientRect(&sRect);
        sRect.top = sRect2.top + 20;

        mDegree += 10.0;
        if (mDegree >= 360.0)
            mDegree = 0.0;

        InvalidateRect(sRect);
        UpdateWindow();
    }

    super::OnTimer(nIDEvent);
}

void SearchDlg::OnViewBarSearch(void) 
{
    gFrame->SendMessage(WM_COMMAND, ID_VIEW_BAR_SEARCH, (LPARAM)gFrame->m_hWnd);
}

void SearchDlg::OnCurLocation(void)
{
    gFrame->SendMessage(WM_COMMAND, ID_EDIT_SEARCH_LOCATION, (LPARAM)gFrame->m_hWnd);
}
