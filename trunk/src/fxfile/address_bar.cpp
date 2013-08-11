//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "address_bar.h"
#include "address_bar_observer.h"

#include "main_frame.h"
#include "option.h"
#include "shell_enumerator.h"

#include "gui/gdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
// item type
enum
{
    ABIT_DESKTOP     = 0x00000001,
    ABIT_IN_DESKTOP  = 0x00000010,
    ABIT_IN_COMPUTER = 0x00000020,
    ABIT_TEMP        = 0x00010000,
};

// user defined messages
enum
{
    WM_SHCN_DESKTOP  = WM_USER + 100,
    WM_SHCN_COMPUTER = WM_USER + 101,
};

AddressBar::AddressBar(void)
{
    mObserver       = XPR_NULL;

    mCurSel         = -1;
    mOldSelFullPidl = XPR_NULL;

    mAutoComplete   = 0;

    mSmallImgList   = XPR_NULL;

    mCustomFont     = XPR_NULL;

    mShcnDesktopId  = 0;
    mShcnComputerId = 0;
}

AddressBar::~AddressBar(void)
{
    COM_FREE(mOldSelFullPidl);
}

BEGIN_MESSAGE_MAP(AddressBar, CComboBoxEx)
    ON_WM_CREATE()
    ON_MESSAGE(WM_SHCN_DESKTOP,  OnShcnDesktop)
    ON_MESSAGE(WM_SHCN_COMPUTER, OnShcnComputer)
    ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelEndOK)
    ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
    ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
    ON_NOTIFY_REFLECT(CBEN_DELETEITEM, OnDeleteItem)
END_MESSAGE_MAP()

void AddressBar::setObserver(AddressBarObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t AddressBar::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;
    sStyle |= CBS_DROPDOWN;
    sStyle |= CBS_AUTOHSCROLL;
    sStyle |= CBS_OWNERDRAWFIXED;

    return CComboBoxEx::Create(sStyle, aRect, aParentWnd, aId);
}

xpr_sint_t AddressBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CComboBoxEx::OnCreate(aCreateStruct) == -1)
        return -1;

    mInit = XPR_FALSE;

    return 0;
}

xpr_bool_t AddressBar::DestroyWindow(void)
{
    if (XPR_IS_NOT_NULL(mCustomFont))
    {
        DELETE_OBJECT(*mCustomFont);
        XPR_SAFE_DELETE(mCustomFont);
    }

    return CComboBoxEx::DestroyWindow();
}

void AddressBar::setSystemImageList(CImageList *aSmallImgList)
{
    mSmallImgList = aSmallImgList;
    SetImageList(mSmallImgList);
}

void AddressBar::setAutoComplete(void)
{
    if (mAutoComplete == 0 || mAutoComplete != (gOpt->mConfig.mAddressBarUrl+1))
    {
        DWORD sFlags = SHACF_FILESYSTEM;
        if (XPR_IS_TRUE(gOpt->mConfig.mAddressBarUrl))
            sFlags |= SHACF_URLALL;

        HRESULT sHResult = ::SHAutoComplete(GetEditCtrl()->m_hWnd, sFlags);
        if (SUCCEEDED(sHResult))
            mAutoComplete = gOpt->mConfig.mAddressBarUrl + 1;
    }
}

void AddressBar::doBaseItems(void)
{
    LPABITEMDATA  sCbItemData = XPR_NULL;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST  sFullPidl = XPR_NULL;
    HRESULT       sHResult;

    //
    // 1. Desktop
    // 
    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DESKTOP, &sFullPidl);
    if (FAILED(sHResult) || XPR_IS_NULL(sFullPidl))
        return;

    sCbItemData = new ABITEMDATA;
    sCbItemData->mShellFolder = sShellFolder;
    sCbItemData->mPidl        = sFullPidl;
    sCbItemData->mFullPidl    = fxfile::base::Pidl::concat(sFullPidl, sFullPidl);
    sCbItemData->mLevel       = 0;
    sCbItemData->mType        = ABIT_DESKTOP;
    sShellFolder->AddRef();

    CbItemDataDeque sCbItemDataDeque;
    sCbItemDataDeque.push_back(sCbItemData);

    sortItems(sCbItemDataDeque);
    insertItems(sCbItemDataDeque, 0);

    sCbItemDataDeque.clear();
    COM_RELEASE(sShellFolder);

    doBaseItemsInDesktop();

    doBaseItemsInComputer();
}

void AddressBar::doBaseItems(xpr_uint_t aType)
{
    switch (aType)
    {
    case ABIT_IN_DESKTOP:  doBaseItemsInDesktop();  break;
    case ABIT_IN_COMPUTER: doBaseItemsInComputer(); break;
    }
}

void AddressBar::doBaseItemsInDesktop(void)
{
    DWORD sEventMask = 0L;
    sEventMask |= SHCNE_RENAMEITEM;
    sEventMask |= SHCNE_CREATE;
    sEventMask |= SHCNE_DELETE;
    sEventMask |= SHCNE_MKDIR;
    sEventMask |= SHCNE_RMDIR;
    //sEventMask |= SHCNE_NETSHARE;
    //sEventMask |= SHCNE_NETUNSHARE;
    sEventMask |= SHCNE_ATTRIBUTES;
    sEventMask |= SHCNE_UPDATEDIR;
    sEventMask |= SHCNE_UPDATEITEM;
    //sEventMask |= SHCNE_SERVERDISCONNECT;
    sEventMask |= SHCNE_UPDATEIMAGE;
    sEventMask |= SHCNE_RENAMEFOLDER;

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder  = XPR_NULL;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DESKTOP, &sFullPidl);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sFullPidl))
    {
        if (mShcnDesktopId == 0)
            mShcnDesktopId = ShellChangeNotify::instance().registerWatch(this, WM_SHCN_DESKTOP, sFullPidl, sEventMask, XPR_FALSE);

        //
        // 2. Items in my computer
        //
        fillItem(sShellFolder, XPR_NULL, 1, 1, XPR_NULL, ABIT_IN_DESKTOP);
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);
}

void AddressBar::doBaseItemsInComputer(void)
{
    DWORD sEventMask = 0L;
    sEventMask |= SHCNE_RENAMEITEM;
    sEventMask |= SHCNE_CREATE;
    sEventMask |= SHCNE_DELETE;
    sEventMask |= SHCNE_MKDIR;
    sEventMask |= SHCNE_RMDIR;
    sEventMask |= SHCNE_MEDIAINSERTED;
    sEventMask |= SHCNE_MEDIAREMOVED;
    sEventMask |= SHCNE_DRIVEREMOVED;
    sEventMask |= SHCNE_DRIVEADD;
    //sEventMask |= SHCNE_NETSHARE;
    //sEventMask |= SHCNE_NETUNSHARE;
    sEventMask |= SHCNE_ATTRIBUTES;
    sEventMask |= SHCNE_UPDATEDIR;
    sEventMask |= SHCNE_UPDATEITEM;
    //sEventMask |= SHCNE_SERVERDISCONNECT;
    sEventMask |= SHCNE_UPDATEIMAGE;
    sEventMask |= SHCNE_DRIVEADDGUI;
    sEventMask |= SHCNE_RENAMEFOLDER;

    HRESULT sHResult;
    LPSHELLFOLDER sShellFolder  = XPR_NULL;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sFullPidl);
    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_sint_t sIndex = 0;
        LPABITEMDATA sCbItemData = XPR_NULL;
        sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sFullPidl);

        xpr_sint_t i;
        for (i = GetCount() - 1; i > 0; --i)
        {
            sCbItemData = (LPABITEMDATA)GetItemData(i);
            if (XPR_IS_NOT_NULL(sCbItemData))
            {
                if (sShellFolder->CompareIDs(0, sFullPidl, sCbItemData->mFullPidl) == 0)
                {
                    sIndex = i;
                    break;
                }
            }
        }

        LPSHELLFOLDER sShellFolder2 = XPR_NULL;

        sCbItemData->mShellFolder->BindToObject(
            sCbItemData->mPidl,
            0,
            IID_IShellFolder,
            reinterpret_cast<LPVOID *>(&sShellFolder2));

        if (mShcnComputerId == 0)
            mShcnComputerId = ShellChangeNotify::instance().registerWatch(this, WM_SHCN_COMPUTER, sFullPidl, sEventMask, XPR_FALSE);

        fillItem(sShellFolder2, sCbItemData->mFullPidl, 2, ++sIndex, XPR_NULL, ABIT_IN_COMPUTER);
    }
}

xpr_bool_t AddressBar::fillItem(LPSHELLFOLDER  aShellFolder,
                                LPITEMIDLIST   aFullPidl,
                                xpr_uint_t     aLevel,
                                xpr_uint_t     aInsert,
                                xpr_tchar_t   *aName,
                                xpr_uint_t     aType)
{
    // SHCONTF_INCLUDEHIDDEN flag must be set.
    // Then, hidden folder can explore regardless of 'Display hidden file' option.

    xpr_bool_t   sResult         = XPR_FALSE;
    LPITEMIDLIST sEnumPidl       = XPR_NULL;
    xpr_sint_t   sEnumListType   = 0;
    xpr_sint_t   sEnumAttributes = 0;

    sEnumListType = base::ShellEnumerator::ListTypeOnlyFolder;

    sEnumAttributes = base::ShellEnumerator::AttributeHidden;
    if (XPR_IS_TRUE(gOpt->mConfig.mShowSystemAttribute))
    {
        sEnumAttributes |= base::ShellEnumerator::AttributeSystem;
    }

    base::ShellEnumerator sShellEnumerator;

    if (sShellEnumerator.enumerate(m_hWnd, aShellFolder, sEnumListType, sEnumAttributes) == XPR_TRUE)
    {
        xpr::tstring    sName;
        LPITEMIDLIST    sEnumPidl        = XPR_NULL;
        LPABITEMDATA    sCbItemData      = XPR_NULL;
        xpr_ulong_t     sShellAttributes;
        DWORD           sFileAttributes;
        xpr_bool_t      sInserted        = (aName == XPR_NULL) ? XPR_TRUE : XPR_FALSE;
        xpr_bool_t      sMatched         = XPR_FALSE;
        CbItemDataDeque sCbItemDataDeque;

        while (sShellEnumerator.next(&sEnumPidl) == XPR_TRUE)
        {
            if (XPR_IS_NOT_NULL(aName))
            {
                GetName(aShellFolder, sEnumPidl, SHGDN_INFOLDER, sName);
                if (_tcsicmp(aName, sName.c_str()) == 0)
                {
                    sMatched = XPR_TRUE;
                }
                else
                {
                    COM_FREE(sEnumPidl);
                    continue;
                }
            }

            sShellAttributes = 0;
            sFileAttributes  = 0;

            if (getItemAttributes(aShellFolder, sEnumPidl, sShellAttributes, sFileAttributes) == XPR_FALSE)
            {
                COM_FREE(sEnumPidl);
                continue;
            }

            sCbItemData                   = new ABITEMDATA;
            sCbItemData->mShellFolder     = aShellFolder;
            sCbItemData->mPidl            = sEnumPidl;
            sCbItemData->mFullPidl        = fxfile::base::Pidl::concat(aFullPidl, sEnumPidl);
            sCbItemData->mShellAttributes = sShellAttributes;
            sCbItemData->mLevel           = aLevel;
            sCbItemData->mType            = aType;
            aShellFolder->AddRef();

            sCbItemDataDeque.push_back(sCbItemData);

            if (XPR_IS_NOT_NULL(aName))
            {
                sCbItemDataDeque.clear();
                sCbItemDataDeque.push_back(sCbItemData);

                sortItems(sCbItemDataDeque);
                insertItems(sCbItemDataDeque, aInsert, gOpt->mConfig.mAddressFullPath);
                sInserted = XPR_FALSE;
                sResult = XPR_TRUE;
                break;
            }
        }

        if (XPR_IS_TRUE(sInserted))
        {
            sortItems(sCbItemDataDeque);
            insertItems(sCbItemDataDeque, aInsert);
            sResult = XPR_TRUE;
        }

        sCbItemDataDeque.clear();
    }
    else
    {
        // failed to enumerate
        return XPR_FALSE;
    }

    return sResult;
}

xpr_bool_t AddressBar::getItemAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aShellAttributes, DWORD &aFileAttributes)
{
    aShellAttributes = SFGAO_FILESYSTEM | SFGAO_FOLDER | //SFGAO_HASSUBFOLDER | SFGAO_CONTENTSMASK | 
        SFGAO_CANRENAME | SFGAO_CANCOPY | SFGAO_CANMOVE | SFGAO_CANDELETE | SFGAO_LINK |
        SFGAO_SHARE | SFGAO_GHOSTED;

    aShellFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&aPidl, &aShellAttributes);

    if (!XPR_TEST_BITS(aShellAttributes, SFGAO_FOLDER))
    {
        return XPR_FALSE;
    }

    static HRESULT sComResult;
    static WIN32_FIND_DATA sWin32FindData;
    DWORD sFileAttributes = 0;

    if (XPR_TEST_BITS(aShellAttributes, SFGAO_FILESYSTEM))
    {
        sComResult = ::SHGetDataFromIDList(aShellFolder, aPidl, SHGDFIL_FINDDATA, &sWin32FindData, sizeof(WIN32_FIND_DATA));
        if (SUCCEEDED(sComResult))
        {
            if (sWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            {
                aShellAttributes |= SFGAO_GHOSTED;
            }

            if (sWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                aShellAttributes |= SFGAO_FOLDER;
            }
            else
            {
                aShellAttributes &= ~SFGAO_FOLDER;
            }

            sFileAttributes = sWin32FindData.dwFileAttributes;
        }
    }

    if (!XPR_TEST_BITS(aShellAttributes, SFGAO_FOLDER))
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

static bool sortCbItemData(LPABITEMDATA aCbItemData1, LPABITEMDATA aCbItemData2)
{
    HRESULT sHResult;

    sHResult = aCbItemData1->mShellFolder->CompareIDs(0, (LPCITEMIDLIST)aCbItemData1->mPidl, (LPCITEMIDLIST)aCbItemData2->mPidl);
    if (FAILED(sHResult))
        return false;

    return ((xpr_sshort_t)HRESULT_CODE(sHResult) < 0) ? true : false;
}

static bool sortTreeCbItemData(LPABITEMDATA aCbItemData1, LPABITEMDATA aCbItemData2)
{
    if (aCbItemData1->mLevel == 0)
        return true;

    if (aCbItemData2->mLevel == 0)
        return false;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
    if (FAILED(sHResult))
        return false;

    sHResult = sShellFolder->CompareIDs(0, (LPCITEMIDLIST)aCbItemData1->mFullPidl, (LPCITEMIDLIST)aCbItemData2->mFullPidl);

    COM_RELEASE(sShellFolder);

    if (FAILED(sHResult))
        return false;

    return ((xpr_sshort_t)HRESULT_CODE(sHResult) < 0) ? true : false;
}

void AddressBar::sortItems(CbItemDataDeque &aCbItemDataDeque)
{
    std::sort(aCbItemDataDeque.begin(), aCbItemDataDeque.end(), sortCbItemData);
}

void AddressBar::sortTreeItems(CbItemDataDeque &aCbItemDataDeque)
{
    std::sort(aCbItemDataDeque.begin(), aCbItemDataDeque.end(), sortTreeCbItemData);
}

void AddressBar::insertItems(CbItemDataDeque &aCbItemDataDeque, xpr_uint_t aInsert, xpr_bool_t aForParsing)
{
    xpr_uint_t i;
    COMBOBOXEXITEM sComboBoxExItem;
    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};

    LPABITEMDATA sCbItemData;
    CbItemDataDeque::iterator sIterator;

    i = 0;

    sIterator = aCbItemDataDeque.begin();
    for (; sIterator != aCbItemDataDeque.end(); ++sIterator)
    {
        sCbItemData = *sIterator;
        if (XPR_IS_NULL(sCbItemData))
            continue;

        sName[0] = XPR_STRING_LITERAL('\0');
        if (XPR_IS_TRUE(aForParsing) && (sIterator+1) == aCbItemDataDeque.end())
            GetName(sCbItemData->mShellFolder, sCbItemData->mPidl, SHGDN_FORPARSING, sName);

        if (sName[0] == XPR_STRING_LITERAL('\0'))
            GetName(sCbItemData->mShellFolder, sCbItemData->mPidl, SHGDN_INFOLDER, sName);

        memset(&sComboBoxExItem, 0, sizeof(COMBOBOXEXITEM));
        sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT | CBEIF_LPARAM;
        sComboBoxExItem.iItem          = aInsert + i;
        sComboBoxExItem.iImage         = GetItemIconIndex(sCbItemData->mFullPidl, XPR_FALSE);
        sComboBoxExItem.iSelectedImage = GetItemIconIndex(sCbItemData->mFullPidl, XPR_TRUE);
        sComboBoxExItem.iIndent        = sCbItemData->mLevel;
        sComboBoxExItem.pszText        = sName;
        sComboBoxExItem.cchTextMax     = XPR_MAX_PATH;
        sComboBoxExItem.lParam         = (LPARAM)sCbItemData;

        InsertItem(&sComboBoxExItem);
        i++;
    }
}

void AddressBar::OnDeleteItem(NMHDR *aNmHdr, LRESULT *aResult)
{
    NMCOMBOBOXEX* pNMComboBoxEx = (NMCOMBOBOXEX*)aNmHdr;
    *aResult = 0;

    LPABITEMDATA sCbItemData = (LPABITEMDATA)pNMComboBoxEx->ceItem.lParam;
    if (XPR_IS_NOT_NULL(sCbItemData))
    {
        COM_RELEASE(sCbItemData->mShellFolder);
        COM_FREE(sCbItemData->mPidl);
        COM_FREE(sCbItemData->mFullPidl);
        XPR_SAFE_DELETE(sCbItemData);
    }
}

void AddressBar::OnSelChange(void)
{
}

void AddressBar::OnSelEndOK(void)
{
    xpr_sint_t sIndex = GetCurSel();
    if (sIndex < 0)
        return;

    LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(sIndex);
    if (XPR_IS_NULL(sCbItemData))
        return;

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::clone(sCbItemData->mFullPidl);
    if (XPR_IS_NULL(sFullPidl))
        return;

    explore(sFullPidl, XPR_TRUE);

    COM_FREE(sFullPidl);
}

xpr_bool_t AddressBar::explore(LPITEMIDLIST aFullPidl, xpr_bool_t aNotifyBuddy)
{
    xpr_bool_t sResult = exploreItem(aFullPidl, aNotifyBuddy);
    if (XPR_IS_FALSE(sResult))
    {
        if (XPR_IS_NOT_NULL(mOldSelFullPidl))
        {
            sResult = exploreItem(mOldSelFullPidl);
            if (XPR_IS_TRUE(sResult))
                aFullPidl = fxfile::base::Pidl::clone(mOldSelFullPidl);
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        COM_FREE(mOldSelFullPidl);
        mOldSelFullPidl = fxfile::base::Pidl::clone(aFullPidl);
    }

    return sResult;
}

xpr_bool_t AddressBar::exploreItem(LPITEMIDLIST aFullPidl, xpr_bool_t aNotifyBuddy)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    if (XPR_IS_FALSE(mInit))
    {
        doBaseItems();
        mInit = XPR_TRUE;
    }

    setAutoComplete();

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPABITEMDATA sCbItemData;

    sCount = GetCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sCbItemData = (LPABITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sCbItemData))
            continue;

        if (XPR_TEST_BITS(sCbItemData->mType, ABIT_TEMP))
            DeleteItem(i);
    }

    xpr_tchar_t sFullPath[XPR_MAX_PATH * 2 + 1] = {0};
    GetDispFullPath(aFullPidl, sFullPath);

    xpr_sint_t sIndex;
    xpr_sint_t sCurSel;

    sIndex = sCurSel = searchFullPath(sFullPath);
    if (sIndex < 0)
    {
        xpr_sint_t sFind = -1;
        xpr_sint_t sItemIDList = -1;
        xpr_tchar_t *sSplit = sFullPath;

        sCount = fxfile::base::Pidl::getItemCount(aFullPidl);
        for (i = 0; i < sCount; ++i)
        {
            sSplit = _tcschr(sSplit, XPR_STRING_LITERAL('\\'));
            if (XPR_IS_NULL(sSplit))
                break;

            sSplit[0] = XPR_STRING_LITERAL('\0');
            sFind = searchFullPath(sFullPath);
            sSplit[0] = XPR_STRING_LITERAL('\\');

            if (sFind == -1)
                break;

            sItemIDList = i;
            sIndex = sFind;

            sSplit++;
        }

        if (sIndex < 0 || sItemIDList < 0)
            return XPR_FALSE;

        xpr_uint_t sType  = 0;
        xpr_uint_t sLevel = 0;

        LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sCbItemData))
        {
            sType  = sCbItemData->mType | ABIT_TEMP;
            sLevel = sCbItemData->mLevel;
        }

        LPSHELLFOLDER   sShellFolder;
        LPCITEMIDLIST   sPidl;
        xpr_ulong_t     sShellAttributes;
        DWORD           sFileAttributes;
        CbItemDataDeque sCbItemDataDeque;

        aFullPidl = fxfile::base::Pidl::clone(aFullPidl);
        sCount = fxfile::base::Pidl::getItemCount(aFullPidl);

        for (i = sCount - 1; i >= sItemIDList; --i)
        {
            sShellFolder = XPR_NULL;
            sPidl        = XPR_NULL;

            if (fxfile::base::Pidl::getSimplePidl(aFullPidl, sShellFolder, sPidl) == XPR_FALSE)
                continue;

            sShellAttributes = 0;
            sFileAttributes  = 0;

            getItemAttributes(sShellFolder, sPidl, sShellAttributes, sFileAttributes);

            sCbItemData = new ABITEMDATA;
            sCbItemData->mShellFolder     = sShellFolder;
            sCbItemData->mPidl            = fxfile::base::Pidl::clone(sPidl);
            sCbItemData->mFullPidl        = fxfile::base::Pidl::clone(aFullPidl);
            sCbItemData->mShellAttributes = sShellAttributes;
            sCbItemData->mLevel           = sLevel + (i - sItemIDList) + 1;
            sCbItemData->mType            = sType;

            sCbItemDataDeque.push_front(sCbItemData);

            fxfile::base::Pidl::removeLastItem(aFullPidl);
        }

        if (sCbItemDataDeque.empty() == false)
        {
            insertItems(sCbItemDataDeque, sIndex+1, gOpt->mConfig.mAddressFullPath);

            sCurSel = sIndex + (xpr_sint_t)sCbItemDataDeque.size();

            sCbItemDataDeque.clear();
        }
    }

    sCbItemData = (LPABITEMDATA)GetItemData(sCurSel);

    xpr_sint_t sResult = XPR_FALSE;
    if (sCbItemData->mFullPidl != XPR_NULL)
    {
        sResult = XPR_TRUE;

        if (mObserver != XPR_NULL)
        {
            sResult = mObserver->onExplore(*this, sCbItemData->mFullPidl, aNotifyBuddy);
        }
    }

    if (sResult == XPR_TRUE)
        SetCurSel(sCurSel);

    return sResult;
}

xpr_bool_t AddressBar::explore(const xpr_tchar_t *aFullPath, xpr_bool_t aNotifyBuddy)
{
    if (XPR_IS_NULL(aFullPath))
        return XPR_FALSE;

    LPITEMIDLIST aFullPidl = XPR_NULL;
    if (GetDispFullPidl(aFullPath, &aFullPidl) == XPR_FALSE)
        return XPR_FALSE;

    xpr_bool_t sResult;
    sResult = explore(aFullPidl, aNotifyBuddy);

    COM_FREE(aFullPidl);

    return sResult;
}

CString AddressBar::getFullPath(xpr_sint_t aIndex) const
{
    CString sFullPath;

    LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(aIndex);
    if (XPR_IS_NOT_NULL(sCbItemData))
    {
        xpr_sint_t sLevel = sCbItemData->mLevel;
        GetLBText(aIndex, sFullPath);

        xpr_sint_t i;
        CString strName;
        for (i = aIndex - 1; i >= 0; --i)
        {
            sCbItemData = (LPABITEMDATA)GetItemData(i);
            if (sCbItemData->mLevel < (xpr_uint_t)sLevel)
            {
                GetLBText(i, strName);
                sFullPath.Insert(0, XPR_STRING_LITERAL("\\"));
                sFullPath.Insert(0, strName);
                sLevel--;
            }
        }
    }

    return sFullPath;
}

void AddressBar::getFullPath(xpr_sint_t aIndex, xpr_tchar_t *aFullPath) const
{
    if (XPR_IS_NOT_NULL(aFullPath))
        _tcscpy(aFullPath, getFullPath(aIndex));
}

xpr_bool_t AddressBar::execute(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sCurSel = GetCurSel();

    xpr_sint_t sIndex = 0;
    LPITEMIDLIST sFullPidl = XPR_NULL;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, aPath);

    // 1. C drive
    if (_tcscmp(sPath, XPR_STRING_LITERAL("\\")) == 0)
        _tcscpy(sPath, XPR_STRING_LITERAL("C:\\"));

    // 2. system enviroment folder
    if (sPath[0] == '%')
    {
        sResult = GetEnvRealPidl(sPath, &sFullPidl);
    }

    // 3. file system path (with URL path)
    if (IsExistFile(sPath) == XPR_TRUE)
    {
        if (_tcslen(sPath) == 2)
            _tcscat(sPath, XPR_STRING_LITERAL("\\"));

        sResult = SUCCEEDED(fxfile::base::Pidl::create(sPath, sFullPidl));
    }

    // 4. network path
    if (XPR_IS_FALSE(sResult) && _tcsncmp(sPath, XPR_STRING_LITERAL("\\\\"), 2) == 0)
    {
        sResult = SUCCEEDED(fxfile::base::Pidl::create(sPath, sFullPidl));
    }

    // 5. in ComboBox
    if (XPR_IS_FALSE(sResult))
    {
        xpr_sint_t sIndex = searchName(sPath);
        if (sIndex >= 0)
        {
            LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(sIndex);
            if (XPR_IS_NOT_NULL(sCbItemData))
            {
                sFullPidl = fxfile::base::Pidl::clone(sCbItemData->mFullPidl);
                sResult = (sFullPidl != XPR_NULL);
            }
        }
    }

    // 6. full path
    if (XPR_IS_FALSE(sResult))
        sResult = GetDispFullPidl(sPath, &sFullPidl);

    // 7. name in ExplorerCtrl
    if (XPR_IS_FALSE(sResult))
    {
        if (mObserver != XPR_NULL)
        {
            sFullPidl = mObserver->onFailExec(*this, sPath);
        }

        sResult = (sFullPidl != XPR_NULL);
    }

    // Failed
    sResult = (sFullPidl != XPR_NULL);

    xpr_bool_t sExecFile = XPR_FALSE;
    xpr_bool_t sNavigateUrl = XPR_FALSE;

    // 8. execution file or update explorer
    if (XPR_IS_TRUE(sResult))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];
        GetName(sFullPidl, SHGDN_FORPARSING, sPath);

        DWORD sFileAttributes = GetFileAttributes(sPath);
        if (!XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            sResult = sExecFile = ExecFile(sPath, XPR_NULL);
        }
        else
        {
            sResult = explore(sFullPidl, XPR_TRUE);
        }
    }

    // 9. navigate URL
    if (XPR_IS_FALSE(sResult))
    {
        if (gOpt->mConfig.mAddressBarUrl == XPR_TRUE)
        {
            NavigateURL(sPath);
            sResult = sNavigateUrl = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sResult) || XPR_IS_TRUE(sExecFile) || XPR_IS_TRUE(sNavigateUrl))
    {
        if (XPR_IS_RANGE(0, sCurSel, GetCount()-1))
            SetCurSel(sCurSel);
    }

    if (XPR_IS_FALSE(sResult))
    {
        xpr_tchar_t sMsg[0xff + XPR_MAX_PATH + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("bar.address.msg.wrong_path"), XPR_STRING_LITERAL("%s")), aPath);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
    }

    COM_FREE(sFullPidl);

    return sResult;
}

xpr_bool_t AddressBar::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_SYSKEYDOWN)
    {
        if (pMsg->hwnd == m_hWnd || pMsg->hwnd == GetEditCtrl()->m_hWnd)
        {
            if (pMsg->wParam == VK_F4)
            {
                if (GetAsyncKeyState(VK_MENU) < 0)
                {
                    if (GetComboBoxCtrl()->GetDroppedState())
                        GetComboBoxCtrl()->ShowDropDown(XPR_FALSE);

                    AfxGetMainWnd()->PostMessage(WM_CLOSE);
                    return XPR_TRUE;
                }
            }
        }
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            if (GetEditCtrl()->m_hWnd == pMsg->hwnd)
            {
                if (mCurSel == -1)
                    mCurSel = GetCurSel();

                if (mCurSel != -1)
                    SetCurSel(mCurSel);

                return XPR_TRUE; // ShowDropDown(XPR_FALSE)
            }
        }

        if (((xpr_sint_t)pMsg->wParam) == VK_RETURN)
        {
            xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
            GetEditCtrl()->GetWindowText(sPath, XPR_MAX_PATH * 2);

            // if it's network path, then it checks the server.
            //if (_tcsncmp(sPath, XPR_STRING_LITERAL("\\\\"), 2) == 0)
            //{
            //    LPCHECKSERVERDATA sCheckServerData = new CheckServerData;
            //    sCheckServerData->hWnd = m_hWnd;
            //    sCheckServerData->lpszServer = new xpr_tchar_t[_tcslen(sPath)+1];
            //    _tcscpy(sCheckServerData->lpszServer, sPath);

            //    mCheckServer = sPath;
            //    mThread = (HANDLE)::_beginthreadex(XPR_NULL, 0, CheckServerProc, sCheckServerData, 0, &mThreadId);

            //    return XPR_TRUE;
            //}

            execute(sPath);
        }
        else if (((xpr_sint_t)pMsg->wParam) == VK_TAB)
        {
            if (XPR_IS_NOT_NULL(mObserver))
                mObserver->onMoveFocus(*this);

            return XPR_TRUE;
        }
    }

    return CComboBoxEx::PreTranslateMessage(pMsg);
}

void AddressBar::setCustomFont(xpr_tchar_t *aFontText)
{
    if (gOpt->mConfig.mCustomFont == XPR_FALSE)
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

void AddressBar::setCustomFont(CFont *aFont)
{
    if (gOpt->mConfig.mCustomFont == XPR_FALSE)
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

xpr_sint_t AddressBar::searchName(const xpr_tchar_t *aName)
{
    if (XPR_IS_NULL(aName))
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPABITEMDATA sCbItemData;
    COMBOBOXEXITEM sComboBoxExItem = {0};
    xpr_tchar_t sName[XPR_MAX_PATH + 1];

    sCount = GetCount();
    for (i =0 ; i < sCount; ++i)
    {
        sName[0] = XPR_STRING_LITERAL('\0');

        sComboBoxExItem.mask       = CBEIF_TEXT | CBEIF_LPARAM;
        sComboBoxExItem.iItem      = i;
        sComboBoxExItem.pszText    = sName;
        sComboBoxExItem.cchTextMax = XPR_MAX_PATH;
        if (GetItem(&sComboBoxExItem) == XPR_FALSE)
            continue;

        sCbItemData = (LPABITEMDATA)sComboBoxExItem.lParam;
        if (XPR_IS_NULL(sCbItemData))
            continue;

        if (_tcsicmp(sComboBoxExItem.pszText, aName) == 0)
            return i;
    }

    return -1;
}

xpr_sint_t AddressBar::searchFullPath(const xpr_tchar_t *aFullPath)
{
    if (XPR_IS_NULL(aFullPath))
        return -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPABITEMDATA sCbItemData;
    xpr_tchar_t sFullPath[XPR_MAX_PATH * 2 + 1];

    sCount = GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sCbItemData = (LPABITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sCbItemData))
            continue;

        GetDispFullPath(sCbItemData->mFullPidl, sFullPath);

        if (_tcsicmp(sFullPath, aFullPath) == 0)
            return i;
    }

    return -1;
}

void AddressBar::OnDropdown(void)
{
}

LRESULT AddressBar::OnShcnDesktop(WPARAM wParam, LPARAM lParam)
{
    Shcn *sShcn = (Shcn *)wParam;
    DWORD sEventId = (DWORD)lParam;

    xpr_uint_t sType = ABIT_IN_DESKTOP;

    switch (sEventId)
    {
    case SHCNE_RENAMEITEM:
    case SHCNE_RENAMEFOLDER:
        {
            OnShcnRenameItem(sType, sShcn);
            break;
        }

    case SHCNE_DELETE:
    case SHCNE_RMDIR:
        {
            OnShcnDeleteItem(sType, sShcn->mPidl1);
            break;
        }

    case SHCNE_ATTRIBUTES:
    case SHCNE_CREATE:
    case SHCNE_MKDIR:
    case SHCNE_UPDATEITEM:
    case SHCNE_UPDATEDIR:
    case SHCNE_UPDATEIMAGE:
        {
            OnShcnUpdateDir(sType, sShcn->mPidl1);
            break;
        }
    }

    sShcn->Free();
    XPR_SAFE_DELETE(sShcn);

    return 0;
}

LRESULT AddressBar::OnShcnComputer(WPARAM wParam, LPARAM lParam)
{
    Shcn *sShcn = (Shcn *)wParam;
    DWORD sEventId = (DWORD)lParam;

    xpr_uint_t sType = ABIT_IN_COMPUTER;

    switch (sEventId)
    {
    case SHCNE_RENAMEITEM:
    case SHCNE_RENAMEFOLDER:
        {
            OnShcnRenameItem(sType, sShcn);
            break;
        }

    case SHCNE_DELETE:
    case SHCNE_RMDIR:
        {
            OnShcnDeleteItem(sType, sShcn->mPidl1);
            break;
        }

    case SHCNE_ATTRIBUTES:
    case SHCNE_MEDIAINSERTED:
    case SHCNE_MEDIAREMOVED:
    case SHCNE_DRIVEREMOVED:
    case SHCNE_DRIVEADD:
    case SHCNE_DRIVEADDGUI:
    case SHCNE_CREATE:
    case SHCNE_MKDIR:
    case SHCNE_UPDATEITEM:
    case SHCNE_UPDATEDIR:
        {
            OnShcnUpdateDir(sType, sShcn->mPidl1);
            break;
        }
    }

    sShcn->Free();
    XPR_SAFE_DELETE(sShcn);

    return 0;
}

void AddressBar::OnShcnRenameItem(xpr_uint_t aType, Shcn *aShcn)
{
    xpr_tchar_t sFullPath[XPR_MAX_PATH * 2 + 1] = {0};
    GetDispFullPath(aShcn->mPidl1, sFullPath);

    xpr_sint_t sIndex = searchFullPath(sFullPath);
    if (sIndex < 0)
        return;

    LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(sIndex);
    if (XPR_IS_NULL(sCbItemData))
        return;

    xpr_bool_t    sResult;
    xpr_ulong_t   sShellAttributes;
    DWORD         sFileAttributes;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(aShcn->mPidl2, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
    {
        sShellAttributes = 0;
        sFileAttributes  = 0;

        getItemAttributes(sShellFolder, sPidl, sShellAttributes, sFileAttributes);

        COM_RELEASE(sCbItemData->mShellFolder);
        COM_FREE(sCbItemData->mFullPidl);
        COM_FREE(sCbItemData->mPidl);

        sCbItemData->mShellFolder     = sShellFolder;
        sCbItemData->mPidl            = fxfile::base::Pidl::clone(sPidl);
        sCbItemData->mFullPidl        = fxfile::base::Pidl::clone(aShcn->mPidl2);
        sCbItemData->mShellAttributes = sShellAttributes;

        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
        GetName(sShellFolder, sPidl, SHGDN_INFOLDER, sName);

        COMBOBOXEXITEM sComboBoxExItem = {0};
        sComboBoxExItem.mask    = CBEIF_TEXT;
        sComboBoxExItem.iItem   = sIndex;
        sComboBoxExItem.pszText = sName;
        SetItem(&sComboBoxExItem);
    }
}

void AddressBar::OnShcnDeleteItem(xpr_uint_t aType, LPITEMIDLIST aFullPidl)
{
    xpr_tchar_t szPath1[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t szPath2[XPR_MAX_PATH + 1] = {0};
    GetName(aFullPidl, SHGDN_FORPARSING, szPath1);

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPABITEMDATA sCbItemData;

    sCount = GetCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sCbItemData = (LPABITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sCbItemData))
            continue;

        if (!XPR_TEST_BITS(sCbItemData->mType, aType))
            continue;

        GetName(sCbItemData->mShellFolder, sCbItemData->mPidl, SHGDN_FORPARSING, szPath2);
        if (_tcsicmp(szPath1, szPath2) == 0)
        {
            DeleteItem(i);
        }
    }
}

void AddressBar::OnShcnUpdateDir(xpr_uint_t aType, LPITEMIDLIST aFullPidl)
{
    LPITEMIDLIST sCurSelFullPidl = XPR_NULL;

    xpr_sint_t sIndex = GetCurSel();
    if (sIndex >= 0)
    {
        LPABITEMDATA sCbItemData = (LPABITEMDATA)GetItemData(sIndex);
        if (XPR_IS_NOT_NULL(sCbItemData))
            sCurSelFullPidl = fxfile::base::Pidl::clone(sCbItemData->mFullPidl);
    }

    xpr_sint_t i;
    xpr_sint_t sCount;
    LPABITEMDATA sCbItemData;

    SetRedraw(XPR_FALSE);

    sCount = GetCount();
    for (i = sCount - 1; i >= 1; --i)
    {
        sCbItemData = (LPABITEMDATA)GetItemData(i);
        if (XPR_IS_NULL(sCbItemData))
            continue;

        if (XPR_TEST_BITS(sCbItemData->mType, aType))
            DeleteItem(i);
    }

    doBaseItems(aType);

    CbItemDataDeque sCbItemDataDeque;

    sCount = GetCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        sCbItemData = (LPABITEMDATA)GetItemData(i);
        sCbItemDataDeque.push_back(sCbItemData);
        SetItemData(i, XPR_NULL);
        DeleteItem(i);
    }

    sortTreeItems(sCbItemDataDeque);
    insertItems(sCbItemDataDeque, 0, XPR_FALSE);

    SetRedraw();
    Invalidate(XPR_TRUE);

    explore(sCurSelFullPidl);
    COM_FREE(sCurSelFullPidl);
}
} // namespace fxfile
