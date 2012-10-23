//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "BookmarkItemEditDlg.h"

#include "rgc/FileDialogST.h"

#include "Option.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BookmarkItemEditDlg::BookmarkItemEditDlg(void)
    : super(IDD_BOOKMARK_ITEM_EDIT, XPR_NULL)
    , mFullPidl(XPR_NULL)
    , mNotCustomIcon(XPR_NULL)
    , mCustomSmallIcon(XPR_NULL)
    , mSeparator(XPR_FALSE)
{
    mOldName[0] = 0;
    mOldPath[0] = 0;
}

BookmarkItemEditDlg::~BookmarkItemEditDlg(void)
{
    DESTROY_ICON(mNotCustomIcon);
    DESTROY_ICON(mCustomSmallIcon);
}

void BookmarkItemEditDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BMK_ITEM_EDIT_SMALL_ICON, mSmallIconWnd);
    DDX_Control(pDX, IDC_BMK_ITEM_EDIT_HOTKEY,     mHotKeyCtrl);
}

BEGIN_MESSAGE_MAP(BookmarkItemEditDlg, super)
    ON_BN_CLICKED(IDC_BMK_ITEM_EDIT_PATH_BROWSE,    OnPathBrowse)
    ON_BN_CLICKED(IDC_BMK_ITEM_EDIT_STARTUP_BROWSE, OnStartupBrowse)
    ON_BN_CLICKED(IDC_BMK_ITEM_EDIT_CUSTOM_ICON,    OnCustomIcon)
    ON_BN_CLICKED(IDC_BMK_ITEM_EDIT_ICON_BROWSE,    OnIconBrowse)
    ON_BN_CLICKED(IDC_BMK_ITEM_EDIT_SEPARATOR,      OnSeparator)
END_MESSAGE_MAP()

xpr_bool_t BookmarkItemEditDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_BMK_ITEM_EDIT_SHOW);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.show.default")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.show.minimize")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.show.maximize")));

    WORD sInvalidComb = HKCOMB_A | HKCOMB_CA | HKCOMB_NONE | HKCOMB_S | HKCOMB_SA | HKCOMB_SC | HKCOMB_SCA;
    mHotKeyCtrl.SetRules(sInvalidComb, HOTKEYF_CONTROL);

    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_NAME      ))->LimitText(MAX_BOOKMARK_NAME);
    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_PATH      ))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_INDEX))->LimitText(10);
    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_PATH ))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_PARAM     ))->LimitText(MAX_BOOKMARK_PARAM);
    ((CEdit *)GetDlgItem(IDC_BMK_ITEM_EDIT_STARTUP   ))->LimitText(XPR_MAX_PATH);

    ::SHAutoComplete(GetDlgItem(IDC_BMK_ITEM_EDIT_PATH     )->m_hWnd, SHACF_FILESYSTEM);
    ::SHAutoComplete(GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_PATH)->m_hWnd, SHACF_FILESYSTEM);
    ::SHAutoComplete(GetDlgItem(IDC_BMK_ITEM_EDIT_STARTUP  )->m_hWnd, SHACF_FILESYSTEM);

    mNotCustomIcon = fxb::BookmarkMgr::instance().getTypeIcon(fxb::BookmarkMgr::IconTypeNot, XPR_TRUE);
    mSmallIconWnd.SetIcon(mNotCustomIcon);

    fillItem();

    if (_tcscmp(mBookmark.mPath.c_str(), fxb::BOOKMARK_SEPARATOR) == 0)
    {
        ((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_SEPARATOR))->SetCheck(1);
        OnSeparator();
    }

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.title")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_NAME,       theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.name")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_SEPARATOR,        theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.check.seperator")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_PATH,       theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.path")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_OPTION,     theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.option")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_ICON_INDEX, theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.icon_index")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_CUSTOM_ICON,      theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.check.custom_icon")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_ICON,       theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.icon")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_PARAM,      theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.parameter")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_STARTUP,    theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.startup")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_SHOW,       theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.show")));
    SetDlgItemText(IDC_BMK_ITEM_EDIT_LABEL_HOTKEY,     theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.label.hotkey")));

    return XPR_TRUE;
}

//----------------------------------------------------------------------
// bookmark item type    example
//----------------------------------------------------------------------
// 1. file               C:\\flyExplorer.exe
// 2. folder             C:\\WinNT
// 3. tree item          Desktop\\Documents\\My Music
// 4. virtual item       Desktop\\Computer
// 5. control item       Desktop\\Control Panels\\Date/Time
// 6. remote computer    \\flychk
//----------------------------------------------------------------------

void BookmarkItemEditDlg::add(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return;

    mFullPidl = aFullPidl;

    fxb::GetName(aFullPidl, SHGDN_INFOLDER, mBookmark.mName);
    getPathFromPidl(aFullPidl, mBookmark.mPath);

    // remove file extension, if new bookmark item is file.
    if (fxb::IsFileSystem(aFullPidl) == XPR_TRUE)
    {
        if (fxb::IsFileSystemFolder(aFullPidl) == XPR_FALSE)
        {
            xpr_tchar_t *sFileExt = (xpr_tchar_t *)fxb::GetFileExt(mBookmark.mName);
            if (XPR_IS_NOT_NULL(sFileExt))
                *sFileExt = 0;
        }
    }

    mBookmark.mIconPath.clear();
    mBookmark.destroyIcon();
}

void BookmarkItemEditDlg::getPathFromPidl(LPITEMIDLIST aFullPidl, std::tstring &aPath)
{
    xpr_bool_t sOnlyFileSystemPath = XPR_FALSE;

    if (gOpt->mContentsBookmarkRealPath == XPR_TRUE)
        sOnlyFileSystemPath = XPR_TRUE;
    else
    {
        sOnlyFileSystemPath = XPR_TRUE;

        xpr_bool_t sParentVirtualItem = fxb::IsParentVirtualItem(aFullPidl);
        if (sParentVirtualItem == XPR_TRUE && fxb::IsFileSystem(aFullPidl) == XPR_TRUE)
        {
            std::tstring sPath;
            std::tstring sFullPath;

            fxb::GetName(aFullPidl, SHGDN_FORPARSING, sPath);
            fxb::GetFullPath(aFullPidl, sFullPath);

            xpr_tchar_t sMsg[XPR_MAX_PATH * 3] = {0};
            _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.bookmark_item_edit.msg.question_real_path"), XPR_STRING_LITERAL("%s,%s")), sPath.c_str(), sFullPath.c_str());
            xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
            sOnlyFileSystemPath = (sMsgId == IDYES) ? XPR_TRUE : XPR_FALSE;
        }
    }

    fxb::Pidl2Path(aFullPidl, aPath, sOnlyFileSystemPath);
}

void BookmarkItemEditDlg::add(fxb::BookmarkItem *aBookmark)
{
    if (aBookmark == XPR_NULL)
        return;

    mBookmark = *aBookmark;
}

void BookmarkItemEditDlg::copyBookmark(fxb::BookmarkItem *aBookmark)
{
    if (aBookmark == XPR_NULL)
        return;

    *aBookmark = mBookmark;
}

void BookmarkItemEditDlg::fillItem(void)
{
    WORD sVirtualKeyCode = LOWORD(mBookmark.mHotKey);
    WORD sModifiers      = HIWORD(mBookmark.mHotKey);

    SetDlgItemText(IDC_BMK_ITEM_EDIT_NAME,    mBookmark.mName.c_str());
    SetDlgItemText(IDC_BMK_ITEM_EDIT_PATH,    mBookmark.mPath.c_str());
    SetDlgItemText(IDC_BMK_ITEM_EDIT_PARAM,   mBookmark.mParam.c_str());
    SetDlgItemText(IDC_BMK_ITEM_EDIT_STARTUP, mBookmark.mStartup.c_str());

    ((CComboBox *)GetDlgItem(IDC_BMK_ITEM_EDIT_SHOW))->SetCurSel(mBookmark.mShowCmd);

    mHotKeyCtrl.SetHotKey(sVirtualKeyCode, sModifiers);

    xpr_bool_t sCustomIcon = mBookmark.mIconPath.empty() ? XPR_FALSE : XPR_TRUE;
    if (sCustomIcon == XPR_TRUE)
    {
        ((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_CUSTOM_ICON))->SetCheck(sCustomIcon);
        enableWindow(sCustomIcon);

        DESTROY_ICON(mCustomSmallIcon);

        xpr_bool_t sLarge = XPR_FALSE;

        ::ExtractIconEx(
            mBookmark.mIconPath.c_str(),
            mBookmark.mIconIndex,
            (sLarge == XPR_TRUE) ? &mCustomSmallIcon : XPR_NULL,
            (sLarge == XPR_TRUE) ? XPR_NULL : &mCustomSmallIcon,
            1);

        if (mCustomSmallIcon == XPR_NULL)
        {
            if (mBookmark.mIconPath[0] == XPR_STRING_LITERAL('%'))
            {
                std::tstring sRealIcon;
                fxb::GetEnvRealPath(mBookmark.mIconPath, sRealIcon);

                ::ExtractIconEx(
                    sRealIcon.c_str(),
                    mBookmark.mIconIndex,
                    (sLarge == XPR_TRUE) ? &mCustomSmallIcon : XPR_NULL,
                    (sLarge == XPR_TRUE) ? XPR_NULL : &mCustomSmallIcon,
                    1);
            }
        }

        mSmallIconWnd.SetIcon(mCustomSmallIcon);

        SetDlgItemText(IDC_BMK_ITEM_EDIT_ICON_PATH, mBookmark.mIconPath.c_str());
        SetDlgItemInt(IDC_BMK_ITEM_EDIT_ICON_INDEX, mBookmark.mIconIndex);
    }
    else
        enableWindow(XPR_FALSE);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void BookmarkItemEditDlg::OnPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_BMK_ITEM_EDIT_PATH, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_BROWSEINCLUDEFILES;// | BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.path.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;

    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
        {
            mBookmark.mName.clear();
            mBookmark.mPath.clear();

            // modify bookmark
            fxb::GetName(sFullPidl, SHGDN_INFOLDER, mBookmark.mName);
            getPathFromPidl(sFullPidl, mBookmark.mPath);

            // remove file extension, if new bookmark item is file.
            if (fxb::IsFileSystem(sFullPidl) == XPR_TRUE)
            {
                if (fxb::IsFileSystemFolder(sFullPidl) == XPR_FALSE)
                {
                    xpr_tchar_t *sFileExt = (xpr_tchar_t *)fxb::GetFileExt(mBookmark.mName);
                    if (XPR_IS_NOT_NULL(sFileExt))
                        *sFileExt = 0;
                }
            }

            fillItem();
        }

        COM_FREE(sFullPidl);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void BookmarkItemEditDlg::OnStartupBrowse(void)
{
    xpr_tchar_t sStartup[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_BMK_ITEM_EDIT_STARTUP, sStartup, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::SHGetPidlFromPath(sStartup);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.startup.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;

    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sStartup);
        SetDlgItemText(IDC_BMK_ITEM_EDIT_STARTUP, sStartup);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void BookmarkItemEditDlg::OnOK(void) 
{
    WORD sVirtualKeyCode, sModifiers;
    mHotKeyCtrl.GetHotKey(sVirtualKeyCode, sModifiers);

    // Virtual Key Code
    // 0 - 48
    // ...
    // 9 - 57
    if (sVirtualKeyCode != 0 && !XPR_IS_RANGE(48, sVirtualKeyCode, 57))
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.msg.wrong_hotkey"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        mHotKeyCtrl.SetFocus();
        return;
    }

    xpr_tchar_t sName[MAX_BOOKMARK_NAME + 1]   = {0};
    xpr_tchar_t sPath[XPR_MAX_PATH + 1]    = {0};
    xpr_tchar_t sParam[MAX_BOOKMARK_PARAM + 1] = {0};
    xpr_tchar_t sStartup[XPR_MAX_PATH + 1] = {0};

    GetDlgItemText(IDC_BMK_ITEM_EDIT_NAME,    sName,    MAX_BOOKMARK_NAME);
    GetDlgItemText(IDC_BMK_ITEM_EDIT_PATH,    sPath,    XPR_MAX_PATH);
    GetDlgItemText(IDC_BMK_ITEM_EDIT_PARAM,   sParam,   MAX_BOOKMARK_PARAM);
    GetDlgItemText(IDC_BMK_ITEM_EDIT_STARTUP, sStartup, XPR_MAX_PATH);

    if (_tcslen(sPath) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.bookmark_item_edit.msg.input_path"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_BMK_ITEM_EDIT_PATH)->SetFocus();
        return;
    }

    mBookmark.mName    = sName;
    mBookmark.mPath    = sPath;
    mBookmark.mParam   = sParam;
    mBookmark.mStartup = sStartup;

    xpr_sint_t sShowCmd = ((CComboBox *)GetDlgItem(IDC_BMK_ITEM_EDIT_SHOW))->GetCurSel();
    if (sShowCmd < 0) sShowCmd = 0;
    if (sShowCmd > 2) sShowCmd = 0;
    mBookmark.mShowCmd = sShowCmd;

    mBookmark.mHotKey = MAKELONG(sVirtualKeyCode, sModifiers);

    xpr_bool_t sCustomIcon = ((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_CUSTOM_ICON))->GetCheck();
    if (sCustomIcon == XPR_FALSE)
    {
        mBookmark.mIconPath.clear();
        mBookmark.mIconIndex = 0;
    }
    else
    {
        xpr_tchar_t sIcon[XPR_MAX_PATH + 1] = {0};
        GetDlgItemText(IDC_BMK_ITEM_EDIT_ICON_PATH, sIcon, XPR_MAX_PATH);
        mBookmark.mIconPath = sIcon;
        mBookmark.mIconIndex = GetDlgItemInt(IDC_BMK_ITEM_EDIT_ICON_INDEX, XPR_NULL, XPR_TRUE);
    }

    if (isSeparator() == XPR_TRUE)
    {
        mBookmark.clear();
        mBookmark.mName = fxb::BOOKMARK_SEPARATOR;
        mBookmark.mPath = fxb::BOOKMARK_SEPARATOR;
    }

    super::OnOK();
}

void BookmarkItemEditDlg::OnCustomIcon(void) 
{
    xpr_bool_t sEnable = (((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_CUSTOM_ICON))->GetCheck() > 0) ? XPR_TRUE : XPR_FALSE;
    enableWindow(sEnable);
}

void BookmarkItemEditDlg::OnIconBrowse(void) 
{
    xpr_tchar_t sIcon[XPR_MAX_PATH + 1];
    GetDlgItemText(IDC_BMK_ITEM_EDIT_ICON_PATH, sIcon, XPR_MAX_PATH);
    xpr_sint_t sIconIndex = GetDlgItemInt(IDC_BMK_ITEM_EDIT_ICON_INDEX, XPR_NULL, XPR_TRUE);

    std::tstring sRealIcon = sIcon;
    if (sIcon[0] == XPR_STRING_LITERAL('%'))
    {
        std::tstring strIcon = sIcon;
        fxb::GetEnvRealPath(strIcon, sRealIcon);
        _tcscpy(sIcon, sRealIcon.c_str());
    }

    if (PickIconDlg(m_hWnd, (xpr_wchar_t *)sIcon, XPR_MAX_PATH, &sIconIndex))
    {
        DESTROY_ICON(mCustomSmallIcon);
        ::ExtractIconEx(sIcon, sIconIndex, XPR_NULL, &mCustomSmallIcon, 1);
        mSmallIconWnd.SetIcon(mCustomSmallIcon);

        if (_tcsicmp(sRealIcon.c_str(), sIcon))
            SetDlgItemText(IDC_BMK_ITEM_EDIT_ICON_PATH, sIcon);

        SetDlgItemInt(IDC_BMK_ITEM_EDIT_ICON_INDEX, sIconIndex);
    }
}

void BookmarkItemEditDlg::enableWindow(xpr_bool_t aCustomIcon, xpr_bool_t aSeparator)
{
    xpr_bool_t sEnable = aCustomIcon;
    if (aSeparator == XPR_TRUE)
        sEnable = XPR_FALSE;

    GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_INDEX)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_PATH)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_SMALL_ICON)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_ICON_BROWSE)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_LABEL_ICON)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_LABEL_ICON_INDEX)->EnableWindow(sEnable);

    sEnable = !aSeparator;

    GetDlgItem(IDC_BMK_ITEM_EDIT_NAME)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_PATH)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_PATH_BROWSE)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_CUSTOM_ICON)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_PARAM)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_STARTUP)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_STARTUP_BROWSE)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_SHOW)->EnableWindow(sEnable);
    GetDlgItem(IDC_BMK_ITEM_EDIT_HOTKEY)->EnableWindow(sEnable);
}

void BookmarkItemEditDlg::OnSeparator(void)
{
    xpr_bool_t sCustomIcon = ((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_CUSTOM_ICON))->GetCheck();
    xpr_bool_t sSeparator  = ((CButton *)GetDlgItem(IDC_BMK_ITEM_EDIT_SEPARATOR))->GetCheck();
    enableWindow(sCustomIcon, sSeparator);

    if (sSeparator == XPR_TRUE)
    {
        GetDlgItemText(IDC_BMK_ITEM_EDIT_NAME, mOldName, MAX_BOOKMARK_NAME);
        GetDlgItemText(IDC_BMK_ITEM_EDIT_PATH, mOldPath, XPR_MAX_PATH);
        SetDlgItemText(IDC_BMK_ITEM_EDIT_NAME, fxb::BOOKMARK_SEPARATOR);
        SetDlgItemText(IDC_BMK_ITEM_EDIT_PATH, fxb::BOOKMARK_SEPARATOR);
    }
    else
    {
        SetDlgItemText(IDC_BMK_ITEM_EDIT_NAME, mOldName);
        SetDlgItemText(IDC_BMK_ITEM_EDIT_PATH, mOldPath);
    }

    mSeparator = sSeparator;
}
