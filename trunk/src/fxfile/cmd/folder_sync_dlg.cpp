//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_sync_dlg.h"

#include "sync_dirs.h"
#include "sys_img_list.h"
#include "option.h"
#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "folder_sync_opt_dlg.h"
#include "size_format.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
// user defined timer
enum
{
    WM_SYNC_STATUS = WM_USER + 100,
};

// user defined message
enum
{
    TM_SYNC_STATUS = 100,
};
} // namespace anonymous

FolderSyncDlg::FolderSyncDlg(void)
    : super(IDD_FOLDER_SYNC)
    , mSyncDirs(XPR_NULL)
    , mStop(XPR_FALSE)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FolderSyncDlg::~FolderSyncDlg(void)
{
    DESTROY_ICON(mIcon);
    XPR_SAFE_DELETE(mSyncDirs);
}

void FolderSyncDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FOLDER_SYNC_PATH1, mPathComboBox[0]);
    DDX_Control(pDX, IDC_FOLDER_SYNC_PATH2, mPathComboBox[1]);
    DDX_Control(pDX, IDC_FOLDER_SYNC_LIST,  mListCtrl);
}

BEGIN_MESSAGE_MAP(FolderSyncDlg, super)
    ON_WM_TIMER()
    ON_WM_CONTEXTMENU()
    ON_WM_MENUCHAR()
    ON_WM_INITMENUPOPUP()
    ON_WM_MEASUREITEM()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_NOTIFY(LVN_GETDISPINFO, IDC_FOLDER_SYNC_LIST, OnLvnGetdispinfoList)
    ON_NOTIFY(NM_CUSTOMDRAW,   IDC_FOLDER_SYNC_LIST, OnNMCustomdrawList)
    ON_COMMAND(ID_SYNC_LEFT,                    OnSyncLeft)
    ON_COMMAND(ID_SYNC_RIGHT,                   OnSyncRight)
    ON_COMMAND(ID_SYNC_NONE,                    OnSyncNone)
    ON_COMMAND(ID_SYNC_ORIGINAL,                OnSyncOriginal)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_PATH1_BROWSE, OnPath1Browse)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_PATH2_BROWSE, OnPath2Browse)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_COMPARE,      OnCompare)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_SYNCHRONIZE,  OnSynchronize)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_PREV_DIFF,    OnPrevDiff)
    ON_BN_CLICKED(IDC_FOLDER_SYNC_NEXT_DIFF,    OnNextDiff)
    ON_MESSAGE(WM_SYNC_STATUS, OnSyncStatus)
END_MESSAGE_MAP()

xpr_bool_t FolderSyncDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_FOLDER_SYNC_LIST,        sizeResize, sizeResize);
    AddControl(IDC_FOLDER_SYNC_STATUS,      sizeResize, sizeRepos);

    AddControl(IDC_FOLDER_SYNC_COMPARE,     sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_FOLDER_SYNC_SYNCHRONIZE, sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDC_FOLDER_SYNC_PREV_DIFF,   sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_FOLDER_SYNC_NEXT_DIFF,   sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                    sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    mPathComboBox[0].LimitText(XPR_MAX_PATH);
    mPathComboBox[1].LimitText(XPR_MAX_PATH);

    ((CEdit *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_INCLUDE_FILTER))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_EXCLUDE_FILTER))->LimitText(XPR_MAX_PATH);

    DWORD sExtendedStyle = mListCtrl.GetExtendedStyle();
    sExtendedStyle |= LVS_EX_INFOTIP;
    sExtendedStyle |= LVS_EX_FULLROWSELECT;
    mListCtrl.SetExtendedStyle(sExtendedStyle);

    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.name")),    LVCFMT_LEFT,   100);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.size")),    LVCFMT_RIGHT,   65);
    mListCtrl.InsertColumn(2, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.date")),    LVCFMT_LEFT,   100);
    mListCtrl.InsertColumn(3, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.compare")), LVCFMT_CENTER,  40);
    mListCtrl.InsertColumn(4, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.date")),    LVCFMT_LEFT,   100);
    mListCtrl.InsertColumn(5, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.size")),    LVCFMT_RIGHT,   65);
    mListCtrl.InsertColumn(6, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.list.column.name")),    LVCFMT_LEFT,   100);

    mListCtrl.SetImageList(&SysImgListMgr::instance().mSysImgList16, LVSIL_SMALL);

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_SYNC_IMAGES);
    mSyncImgList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 5, 1);
    mSyncImgList.Add(&sBitmap, RGB(255,0,255));

    {
        CWnd *sPath1BrowseCtrl = GetDlgItem(IDC_FOLDER_SYNC_PATH1_BROWSE);
        CWnd *sPath2BrowseCtrl = GetDlgItem(IDC_FOLDER_SYNC_PATH2_BROWSE);

        CRect sPath1Rect, sPath2Rect;
        CRect sPathBrowseRect;

        mPathComboBox[0].GetWindowRect(&sPath1Rect);
        ScreenToClient(&sPath1Rect);

        mPathComboBox[1].GetWindowRect(&sPath2Rect);
        ScreenToClient(&sPath2Rect);

        sPath1BrowseCtrl->GetWindowRect(&sPathBrowseRect);
        ScreenToClient(&sPathBrowseRect);

        mPathOffset[0] = sPath1Rect.left;
        mPathOffset[1] = sPathBrowseRect.left - sPath1Rect.right;
        mPathOffset[2] = sPath2Rect.left - sPathBrowseRect.right;
        mPathBrowseWidth = sPathBrowseRect.Width();
    }

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.title")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_OPTION,                 gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.group.scan_option")));
    SetDlgItemText(IDC_FOLDER_SYNC_LABEL_SCAN_INCLUDE_FILTER,   gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.label.scan_include_filter")));
    SetDlgItemText(IDC_FOLDER_SYNC_LABEL_SCAN_EXCLUDE_FILTER,   gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.label.scan_exclude_filter")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_EXCLUDE_EQUAL_FILENAME, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.scan_exclude_equal_filename")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_EXCLUDE_OTHER_FILENAME, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.scan_exclude_other_filename")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_SUBFOLDER,              gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.scan_subfolder")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_SYSTEM,                 gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.scan_system")));
    SetDlgItemText(IDC_FOLDER_SYNC_SCAN_HIDDEN,                 gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.scan_hidden")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE_OPTION,              gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.group.compare_option")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE_BY_DATETIME,         gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.compare_by_date_time")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE_BY_ATTRIBUTES,       gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.compare_by_attributes")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE_BY_SIZE,             gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.compare_by_size")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE_BY_CONTENTS,         gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.check.compare_by_contents")));
    SetDlgItemText(IDC_FOLDER_SYNC_COMPARE,                     gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.compare")));
    SetDlgItemText(IDC_FOLDER_SYNC_PREV_DIFF,                   gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.previous_diff")));
    SetDlgItemText(IDC_FOLDER_SYNC_NEXT_DIFF,                   gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.next_diff")));
    SetDlgItemText(IDC_FOLDER_SYNC_SYNCHRONIZE,                 gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.synchronize")));
    SetDlgItemText(IDCANCEL,                                    gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("FolderSync"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"),               mListCtrl.GetDlgCtrlID());
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Path1"),          IDC_FOLDER_SYNC_PATH1);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Path2"),          IDC_FOLDER_SYNC_PATH2);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Include Filter"),     IDC_FOLDER_SYNC_SCAN_INCLUDE_FILTER);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Exclude Filter"),     IDC_FOLDER_SYNC_SCAN_EXCLUDE_FILTER);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Exclude Equal"),      IDC_FOLDER_SYNC_SCAN_EXCLUDE_EQUAL_FILENAME);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Exclude Other"),      IDC_FOLDER_SYNC_SCAN_EXCLUDE_OTHER_FILENAME);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("System File"),        IDC_FOLDER_SYNC_SCAN_SYSTEM);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Hidden File"),        IDC_FOLDER_SYNC_SCAN_HIDDEN);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Subfolder"),          IDC_FOLDER_SYNC_SCAN_SUBFOLDER);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Compare Date Time"),  IDC_FOLDER_SYNC_COMPARE_BY_DATETIME);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Compare Attributes"), IDC_FOLDER_SYNC_COMPARE_BY_ATTRIBUTES);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Compare Size"),       IDC_FOLDER_SYNC_COMPARE_BY_SIZE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Compare Contents"),   IDC_FOLDER_SYNC_COMPARE_BY_CONTENTS);
        mDlgState->load();
    }

    SetDlgItemText(IDC_FOLDER_SYNC_PATH1, mDir[0].c_str());
    SetDlgItemText(IDC_FOLDER_SYNC_PATH2, mDir[1].c_str());

    return XPR_TRUE;
}

void FolderSyncDlg::OnDestroy(void)
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void FolderSyncDlg::setDir(const xpr_tchar_t *aDir1, const xpr_tchar_t *aDir2)
{
    if (aDir1 != XPR_NULL) mDir[0] = aDir1;
    if (aDir2 != XPR_NULL) mDir[1] = aDir2;
}

xpr_bool_t FolderSyncDlg::getPath(xpr_sint_t aIndex, xpr_tchar_t *aPath, xpr_sint_t aMaxPathLen)
{
    if (!XPR_IS_RANGE(0, aIndex, 1) || aPath == XPR_NULL || aMaxPathLen <= 0)
        return XPR_FALSE;

    xpr_sint_t sCurSel = mPathComboBox[aIndex].GetCurSel();
    if (sCurSel == CB_ERR)
        mPathComboBox[aIndex].GetWindowText(aPath, aMaxPathLen);
    else
        mPathComboBox[aIndex].GetLBText(sCurSel, aPath);

    return XPR_TRUE;
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void FolderSyncDlg::OnPath1Browse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    getPath(0, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = XPR_NULL;
    sOldFullPidl = fxfile::base::Pidl::create(sPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        GetName(sFullPidl, SHGDN_FORPARSING, sPath);

        xpr_sint_t sCurSel = -1;

        xpr_sint_t i;
        xpr_sint_t sCount;
        xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};

        sCount = mPathComboBox[0].GetCount();
        for (i = 0; i < sCount; ++i)
        {
            mPathComboBox[0].GetLBText(i, sText);
            if (_tcsicmp(sText, sPath) == 0)
            {
                sCurSel = i;
                break;
            }
        }

        mPathComboBox[0].SetCurSel(sCurSel);

        if (sCurSel == -1)
            SetDlgItemText(IDC_FOLDER_SYNC_PATH1, sPath);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void FolderSyncDlg::OnPath2Browse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    getPath(1, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = XPR_NULL;
    sOldFullPidl = fxfile::base::Pidl::create(sPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = 0;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        GetName(sFullPidl, SHGDN_FORPARSING, sPath);

        xpr_sint_t sCurSel = -1;

        xpr_sint_t i;
        xpr_sint_t sCount;
        xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};

        sCount = mPathComboBox[1].GetCount();
        for (i = 0; i < sCount; ++i)
        {
            mPathComboBox[1].GetLBText(i, sText);
            if (_tcsicmp(sText, sPath) == 0)
            {
                sCurSel = i;
                break;
            }
        }

        mPathComboBox[1].SetCurSel(sCurSel);

        if (sCurSel == -1)
            SetDlgItemText(IDC_FOLDER_SYNC_PATH2, sPath);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void FolderSyncDlg::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    *pResult = 0;

    xpr_sint_t sIndex = (xpr_sint_t)pLVCD->nmcd.dwItemSpec;
    SyncItem *sSyncItem = (SyncItem *)pLVCD->nmcd.lItemlParam;

    xpr_uint_t sSync = SyncNone;
    if (XPR_TEST_BITS(pLVCD->nmcd.dwDrawStage, CDDS_ITEM))
    {
        if (sSyncItem != XPR_NULL)
            sSync = sSyncItem->getSync();
    }

    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREERASE)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        switch (sSync)
        {
        case SyncToRight:    pLVCD->clrText = RGB(0,128,0); break;
        case SyncToLeft:     pLVCD->clrText = RGB(0,0,255); break;
        case SyncNotEqualed: pLVCD->clrText = RGB(255,0,0); break;
        }

        if (sSyncItem != XPR_NULL)
        {
            if (sSyncItem->isDirectory(0) || sSyncItem->isDirectory(1))
                pLVCD->clrTextBk = RGB(230,230,230);
        }

        *pResult = CDRF_NOTIFYPOSTPAINT;
    }
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
    {
        CDC *sDc = CDC::FromHandle(pLVCD->nmcd.hdc);
        if (sDc != XPR_NULL)
        {
            CHeaderCtrl *sHeaderCtrl = mListCtrl.GetHeaderCtrl();
            if (sHeaderCtrl != XPR_NULL)
            {
                CRect sHeaderRect;
                sHeaderCtrl->GetItemRect(3, &sHeaderRect);
                sHeaderCtrl->ClientToScreen(&sHeaderRect);
                mListCtrl.ScreenToClient(&sHeaderRect);

                CRect sItemRect;
                mListCtrl.GetItemRect(sIndex, &sItemRect, LVIR_BOUNDS);

                CPoint sPoint;
                sPoint.x = sHeaderRect.left + (sHeaderRect.Width() - 16) / 2;
                sPoint.y = sItemRect.top    + (sItemRect.Height()  - 16) / 2;

                xpr_sint_t sImage = -1;
                switch (sSync)
                {
                case SyncFailed:     sImage = 0; break;
                case SyncEqualed:    sImage = 1; break;
                case SyncNotEqualed: sImage = 2; break;
                case SyncToRight:    sImage = 3; break;
                case SyncToLeft:     sImage = 4; break;
                }

                if (sImage != -1)
                    mSyncImgList.Draw(sDc, sImage, sPoint, ILD_TRANSPARENT);
            }
        }

        *pResult = CDRF_DODEFAULT;
    }
}

void FolderSyncDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    if (mSyncDirs == XPR_NULL)
        return;

    SyncItem *sSyncItem = (SyncItem *)pDispInfo->item.lParam;
    if (sSyncItem == XPR_NULL)
        return;

    LVITEM &sLvItem = pDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_IMAGE))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        if (sSyncItem->isExistLeft())
            mSyncDirs->getDir(sPath, XPR_NULL);
        else
            mSyncDirs->getDir(XPR_NULL, sPath);
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));
        _tcscat(sPath, sSyncItem->mSubPath.c_str());

        sLvItem.iImage = GetItemIconIndex(sPath);
    }

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        // file name
        if ((sLvItem.iSubItem == 0 && sSyncItem->isExistLeft()  == XPR_TRUE) ||
            (sLvItem.iSubItem == 6 && sSyncItem->isExistRight() == XPR_TRUE))
        {
            xpr_sint_t sIndex = (sLvItem.iSubItem == 0) ? 0 : 1;

            if (sSyncItem->isDirectory(sIndex))
                _tcscpy(sLvItem.pszText, sSyncItem->mSubPath.c_str());
            else
            {
                xpr_size_t sFind = xpr::string::npos;

                if (sSyncItem->mSubLevel > 0)
                    sFind = sSyncItem->mSubPath.rfind(XPR_STRING_LITERAL('\\'));

                if (sFind == xpr::string::npos)
                    sFind = 0;
                else
                    sFind++;

                _tcscpy(sLvItem.pszText, sSyncItem->mSubPath.substr(sFind).c_str());
            }
        }

        // file size
        if ((sLvItem.iSubItem == 1 && sSyncItem->isExistLeft()  == XPR_TRUE) ||
            (sLvItem.iSubItem == 5 && sSyncItem->isExistRight() == XPR_TRUE))
        {
            xpr_sint_t sIndex = (sLvItem.iSubItem == 1) ? 0 : 1;

            if (sSyncItem->isDirectory(sIndex) == XPR_FALSE)
            {
                SizeFormat::getFormatedNumber(sSyncItem->mFileSize[sIndex], sLvItem.pszText, 200);
            }
        }

        if ((sLvItem.iSubItem == 2 && sSyncItem->isExistLeft()  == XPR_TRUE) ||
            (sLvItem.iSubItem == 4 && sSyncItem->isExistRight() == XPR_TRUE))
        {
            xpr_sint_t sIndex = (sLvItem.iSubItem == 2) ? 0 : 1;

            FILETIME sLocalFileTime;
            SYSTEMTIME sLocalSystemTime;

            if (FileTimeToLocalFileTime(sSyncItem->getTime(sIndex), &sLocalFileTime) &&
                FileTimeToSystemTime(&sLocalFileTime, &sLocalSystemTime))
            {
                xpr_tchar_t sDate[0xff] = {0};
                xpr_tchar_t sTime[0xff] = {0};

                const xpr_tchar_t *sDateFormat = gOpt->mConfig.mFileList2YearDate  ? XPR_STRING_LITERAL("yy-MM-dd") : XPR_STRING_LITERAL("yyyy-MM-dd");
                const xpr_tchar_t *sTimeFormat = gOpt->mConfig.mFileList24HourTime ? XPR_STRING_LITERAL("HH:mm")    : XPR_STRING_LITERAL("tt hh:mm");

                GetDateFormat(XPR_NULL, 0, &sLocalSystemTime, sDateFormat, sDate, 0xfe);
                GetTimeFormat(XPR_NULL, 0, &sLocalSystemTime, sTimeFormat, sTime, 0xfe);
                _stprintf(sLvItem.pszText, XPR_STRING_LITERAL("%s %s"), sDate, sTime);
            }
        }
    }

    sLvItem.mask |= LVIF_DI_SETITEM;
}

void FolderSyncDlg::view(void)
{
    if (mSyncDirs == XPR_NULL)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    xpr_sint_t sIndex;
    xpr_size_t i, sCount;
    SyncItem *sSyncItem;
    LVITEM sLvItem = {0};

    sIndex = 0;
    sCount = mSyncDirs->getCount();
    for (i = 0; i < sCount; ++i)
    {
        sSyncItem = mSyncDirs->getSyncItem((xpr_sint_t)i);
        if (sSyncItem == XPR_NULL)
            continue;

        if (sSyncItem->mExist == CompareExistNone)
            continue;

        sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;// | LVIF_INDENT;
        sLvItem.iItem      = sIndex;
        sLvItem.iSubItem   = 0;
        sLvItem.iIndent    = 0;//sSyncItem->m_nSubLevel;
        sLvItem.iImage     = I_IMAGECALLBACK;
        sLvItem.pszText    = LPSTR_TEXTCALLBACK;
        sLvItem.cchTextMax = XPR_MAX_PATH;
        sLvItem.lParam     = (LPARAM)sSyncItem;
        sIndex = mListCtrl.InsertItem(&sLvItem);

        sIndex++;
    }

    mListCtrl.SetRedraw();
    mListCtrl.Invalidate();
}

void FolderSyncDlg::scan(void)
{
    xpr_tchar_t sPath[2][XPR_MAX_PATH + 1];
    getPath(0, sPath[0], XPR_MAX_PATH);
    getPath(1, sPath[1], XPR_MAX_PATH);

    xpr_sint_t sLen[2];
    sLen[0] = (xpr_sint_t)_tcslen(sPath[0]);
    sLen[1] = (xpr_sint_t)_tcslen(sPath[1]);

    xpr_bool_t sIncorrectedPath[2];
    sIncorrectedPath[0] = sLen[0] <= 0 || !IsExistFile(sPath[0]);
    sIncorrectedPath[1] = sLen[1] <= 0 || !IsExistFile(sPath[1]);

    if (sIncorrectedPath[0] == XPR_TRUE || sIncorrectedPath[1] == XPR_TRUE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH * 3] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.msg.wrong_path"), XPR_STRING_LITERAL("%s,%s")), sPath[0], sPath[1]);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem((sIncorrectedPath[0] == XPR_TRUE) ? IDC_FOLDER_SYNC_PATH1 : IDC_FOLDER_SYNC_PATH2)->SetFocus();
        return;
    }

    if (sPath[0][sLen[0]-1] == XPR_STRING_LITERAL('\\')) sPath[0][sLen[0]-1] = XPR_STRING_LITERAL('\0');
    if (sPath[1][sLen[1]-1] == XPR_STRING_LITERAL('\\')) sPath[1][sLen[1]-1] = XPR_STRING_LITERAL('\0');

    if (_tcsicmp(sPath[0], sPath[1]) == 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.msg.same_path"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FOLDER_SYNC_PATH1)->SetFocus();
        return;
    }

    // scan filtering
    xpr_tchar_t sTempIncFilter[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sTempExcFilter[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FOLDER_SYNC_SCAN_INCLUDE_FILTER, sTempIncFilter, XPR_MAX_PATH);
    GetDlgItemText(IDC_FOLDER_SYNC_SCAN_EXCLUDE_FILTER, sTempExcFilter, XPR_MAX_PATH);

    xpr::string sIncFilter = sTempIncFilter;
    xpr::string sExcFilter = sTempExcFilter;
    sIncFilter.trim(); // TODO remove all white space
    sExcFilter.trim(); // TODO remove all white space

    if (sIncFilter.empty())
        sIncFilter = XPR_STRING_LITERAL("*");

    xpr_bool_t sExcludeEqual = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_EXCLUDE_EQUAL_FILENAME))->GetCheck();
    xpr_bool_t sExcludeOther = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_EXCLUDE_OTHER_FILENAME))->GetCheck();
    xpr_bool_t sSystemFile   = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_SYSTEM))->GetCheck();
    xpr_bool_t sHiddenFile   = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_HIDDEN))->GetCheck();
    xpr_bool_t sSubFolder    = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_SCAN_SUBFOLDER))->GetCheck();

    if (sExcludeEqual == XPR_TRUE && sExcludeOther == XPR_TRUE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.msg.exclusive_option"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FOLDER_SYNC_SCAN_EXCLUDE_EQUAL_FILENAME)->SetFocus();
        return;
    }

    DlgState::insertComboEditString(&mPathComboBox[0]);
    DlgState::insertComboEditString(&mPathComboBox[1]);

    mListCtrl.SetRedraw(XPR_FALSE);

    mListCtrl.DeleteAllItems();

    mListCtrl.SetRedraw();
    mListCtrl.Invalidate();

    XPR_SAFE_DELETE_ARRAY(mSyncDirs);

    xpr_uint_t sExcExist = CompareExistNone;
    if (sExcludeEqual == XPR_TRUE) sExcExist = CompareExistEqual;
    if (sExcludeOther == XPR_TRUE) sExcExist = CompareExistOther;

    DWORD sExcAttributes = 0;
    if (sSystemFile == XPR_FALSE) sExcAttributes |= FILE_ATTRIBUTE_SYSTEM;
    if (sHiddenFile == XPR_FALSE) sExcAttributes |= FILE_ATTRIBUTE_HIDDEN;

    DWORD sSubLevel = 0;
    if (sSubFolder == XPR_TRUE)
        sSubLevel = 0xffffffff;

    mSyncDirs = new SyncDirs;
    mSyncDirs->setOwner(*this, WM_SYNC_STATUS);
    mSyncDirs->setDir(sPath[0], sPath[1]);
    mSyncDirs->setSubLevel(sSubLevel);

    mSyncDirs->setExcludeExist(sExcExist);
    mSyncDirs->setIncludeFilter(sIncFilter.c_str());
    mSyncDirs->setExcludeFilter(sExcFilter.c_str());
    mSyncDirs->setExcludeAttributes(sExcAttributes);

    if (mSyncDirs->scan())
    {
        setControlState(StateCompare);
        SetTimer(TM_SYNC_STATUS, 200, XPR_NULL);
    }
}

void FolderSyncDlg::compare(void)
{
    if (mSyncDirs == XPR_NULL)
        return;

    // compare condition
    xpr_bool_t sDateTime   = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_COMPARE_BY_DATETIME))->GetCheck();
    xpr_bool_t sAttributes = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_COMPARE_BY_ATTRIBUTES))->GetCheck();
    xpr_bool_t sSize       = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_COMPARE_BY_SIZE))->GetCheck();
    xpr_bool_t sContents   = ((CButton *)GetDlgItem(IDC_FOLDER_SYNC_COMPARE_BY_CONTENTS))->GetCheck();

    mSyncDirs->setDateTime(sDateTime);
    mSyncDirs->setSize(sSize);
    mSyncDirs->setAttributes(sAttributes);
    mSyncDirs->setContents((sContents == XPR_TRUE) ? CompareContentsBytes : CompareContentsNone);

    mSyncDirs->compare();
}

void FolderSyncDlg::synchronize(void)
{
    if (mSyncDirs == XPR_NULL)
        return;

    xpr_tchar_t sPath[2][XPR_MAX_PATH + 1];
    xpr_sint_t sFiles[2];
    xpr_sint64_t sSize[2];

    mSyncDirs->getDir(sPath[0], sPath[1]);
    sFiles[0] = (xpr_sint_t)mSyncDirs->getSyncFiles(SyncDirectionToRight, &sSize[0]);
    sFiles[1] = (xpr_sint_t)mSyncDirs->getSyncFiles(SyncDirectionToLeft,  &sSize[1]);

    FolderSyncOptDlg sDlg;
    sDlg.setPath(0, sPath[0]);
    sDlg.setPath(1, sPath[1]);
    sDlg.setFiles(0, sFiles[0], sSize[0]);
    sDlg.setFiles(1, sFiles[1], sSize[1]);
    if (sDlg.DoModal() != IDOK)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    mListCtrl.DeleteAllItems();

    mListCtrl.SetRedraw();
    mListCtrl.Invalidate();

    xpr_uint_t sDirection = sDlg.getDirection();

    mSyncDirs->setDirection(sDirection);
    if (mSyncDirs->synchronize())
    {
        setControlState(StateSynchronize);
        SetTimer(TM_SYNC_STATUS, 200, XPR_NULL);
    }
}

void FolderSyncDlg::OnCompare(void)
{
    mStop = XPR_FALSE;

    if (mSyncDirs != XPR_NULL)
    {
        SyncDirs::Status sStatus = SyncDirs::StatusNone;
        mSyncDirs->getStatus(sStatus);

        if (sStatus == SyncDirs::StatusScaning || sStatus == SyncDirs::StatusComparing)
        {
            mSyncDirs->stop();
            mStop = XPR_TRUE;
            return;
        }

        XPR_SAFE_DELETE(mSyncDirs);
    }

    scan();
}

void FolderSyncDlg::OnSynchronize(void)
{
    mStop = XPR_FALSE;

    if (mSyncDirs != XPR_NULL)
    {
        SyncDirs::Status sStatus = SyncDirs::StatusNone;
        mSyncDirs->getStatus(sStatus);

        if (sStatus == SyncDirs::StatusSynchronizing)
        {
            mSyncDirs->stop();
            mStop = XPR_TRUE;
            return;
        }
    }

    synchronize();
}

LRESULT FolderSyncDlg::OnSyncStatus(WPARAM wParam, LPARAM lParam)
{
    SyncDirs::Status sStatus = (SyncDirs::Status)wParam;

    xpr_tchar_t sStatusText[0xff] = {0};

    switch (sStatus)
    {
    case SyncDirs::StatusScanCompleted:
        {
            xpr_size_t sFolderCount = (xpr_size_t)lParam;

            xpr_tchar_t sFolderCountText[0xff] = {0};
            SizeFormat::getFormatedNumber(sFolderCount, sFolderCountText, 0xfe);

            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.scan_completed"), XPR_STRING_LITERAL("%s")), sFolderCountText);
            setStatus(sStatusText);

            if (mStop == XPR_TRUE)
            {
                view();
                setControlState(StateNone);
            }
            else
            {
                compare();
            }

            break;
        }

    case SyncDirs::StatusCompareCompleted:
        {
            KillTimer(TM_SYNC_STATUS);

            xpr_size_t sTotalCount = (xpr_size_t)lParam;

            xpr_size_t sDirCount = 0;
            xpr_size_t sFileCount = 0;
            mSyncDirs->getCount(&sDirCount, &sFileCount);

            xpr_size_t sEqualCount = 0;
            xpr_size_t sNotEqualCount = 0;
            xpr_size_t sLeftExistCount = 0;
            xpr_size_t sRightExistCount = 0;
            xpr_size_t sFailCount = 0;
            mSyncDirs->getDiffCount(&sEqualCount, &sNotEqualCount, &sLeftExistCount, &sRightExistCount, &sFailCount);

            xpr_tchar_t sTotalCountText[0xff] = {0};
            xpr_tchar_t sDirCountText[0xff] = {0};
            xpr_tchar_t sFileCountText[0xff] = {0};
            xpr_tchar_t sEqualCountText[0xff] = {0};
            xpr_tchar_t sNotEqualCountText[0xff] = {0};
            xpr_tchar_t sLeftExistCountText[0xff] = {0};
            xpr_tchar_t sRightExistCountText[0xff] = {0};
            xpr_tchar_t sFailCountText[0xff] = {0};

            SizeFormat::getFormatedNumber(sTotalCount,      sTotalCountText,      0xfe);
            SizeFormat::getFormatedNumber(sDirCount,        sDirCountText,        0xfe);
            SizeFormat::getFormatedNumber(sFileCount,       sFileCountText,       0xfe);
            SizeFormat::getFormatedNumber(sEqualCount,      sEqualCountText,      0xfe);
            SizeFormat::getFormatedNumber(sNotEqualCount,   sNotEqualCountText,   0xfe);
            SizeFormat::getFormatedNumber(sLeftExistCount,  sLeftExistCountText,  0xfe);
            SizeFormat::getFormatedNumber(sRightExistCount, sRightExistCountText, 0xfe);
            SizeFormat::getFormatedNumber(sFailCount,       sFailCountText,       0xfe);

            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.compare_completed"), XPR_STRING_LITERAL("%s,%s,%s,%s,%s,%s,%s,%s")), sTotalCountText, sDirCountText, sFileCountText, sEqualCountText, sNotEqualCountText, sLeftExistCountText, sRightExistCountText, sFailCountText);
            setStatus(sStatusText);

            view();
            setControlState(StateNone);
            break;
        }

    case SyncDirs::StatusSynchronizeCompleted:
        {
            KillTimer(TM_SYNC_STATUS);

            xpr_size_t sSyncCount = (xpr_size_t)lParam;

            xpr_tchar_t sSyncCountText[0xff] = {0};
            SizeFormat::getFormatedNumber(sSyncCount, sSyncCountText, 0xfe);

            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.synchronize_completed"), XPR_STRING_LITERAL("%s")), sSyncCountText);
            setStatus(sStatusText);

            view();
            setControlState(StateNone);
            break;
        }
    }

    return 0;
}

void FolderSyncDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_SYNC_STATUS)
    {
        SyncDirs::Status sStatus = SyncDirs::StatusNone;
        xpr_uint_t sCount = mSyncDirs->getStatus(sStatus);

        xpr_tchar_t sCountText[0xff] = {0};
        xpr_tchar_t sStatusText[0xff] = {0};

        switch (sStatus)
        {
        case SyncDirs::StatusScaning:
            {
                SizeFormat::getFormatedNumber(sCount, sCountText, 0xfe);
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.scanning"), XPR_STRING_LITERAL("%s")), sCountText);
                break;
            }

        case SyncDirs::StatusComparing:
            {
                SizeFormat::getFormatedNumber(sCount, sCountText, 0xfe);
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.comparing"), XPR_STRING_LITERAL("%s")), sCountText);
                break;
            }

        case SyncDirs::StatusSynchronizing:
            {
                SizeFormat::getFormatedNumber(sCount, sCountText, 0xfe);
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.folder_sync.status.synchronizing"), XPR_STRING_LITERAL("%s")), sCountText);
                break;
            }
        }

        setStatus(sStatusText);
    }

    super::OnTimer(nIDEvent);
}

void FolderSyncDlg::setControlState(State uState)
{
    xpr_uint_t sIds[] = {
        IDC_FOLDER_SYNC_PATH1,
        IDC_FOLDER_SYNC_PATH1_BROWSE,
        IDC_FOLDER_SYNC_PATH2,
        IDC_FOLDER_SYNC_PATH2_BROWSE,
        IDC_FOLDER_SYNC_SCAN_INCLUDE_FILTER,
        IDC_FOLDER_SYNC_SCAN_EXCLUDE_FILTER,
        IDC_FOLDER_SYNC_SCAN_EXCLUDE_EQUAL_FILENAME,
        IDC_FOLDER_SYNC_SCAN_EXCLUDE_OTHER_FILENAME,
        IDC_FOLDER_SYNC_SCAN_SUBFOLDER,
        IDC_FOLDER_SYNC_SCAN_SYSTEM,
        IDC_FOLDER_SYNC_SCAN_HIDDEN,
        IDC_FOLDER_SYNC_COMPARE_BY_DATETIME,
        IDC_FOLDER_SYNC_COMPARE_BY_ATTRIBUTES,
        IDC_FOLDER_SYNC_COMPARE_BY_SIZE,
        IDC_FOLDER_SYNC_COMPARE_BY_CONTENTS,
        IDC_FOLDER_SYNC_COMPARE,
        IDC_FOLDER_SYNC_SYNCHRONIZE,
        IDCANCEL,
        IDC_FOLDER_SYNC_PREV_DIFF,
        IDC_FOLDER_SYNC_NEXT_DIFF,
        0};

        xpr_sint_t i;
        CWnd *sWnd;
        xpr_bool_t sEnable;

        for (i = 0; sIds[i]; ++i)
        {
            sWnd = GetDlgItem(sIds[i]);
            if (sWnd == XPR_NULL || sWnd->m_hWnd == XPR_NULL)
                continue;

            sEnable = (uState == StateNone);

            if (uState == StateCompare)
            {
                switch (sIds[i])
                {
                case IDC_FOLDER_SYNC_COMPARE:
                    sEnable = XPR_TRUE;
                    break;
                }
            }

            if (uState == StateSynchronize)
            {
                switch (sIds[i])
                {
                case IDC_FOLDER_SYNC_SYNCHRONIZE:
                    sEnable = XPR_TRUE;
                    break;
                }
            }

            sWnd->EnableWindow(sEnable);
        }

        switch (uState)
        {
        case StateCompare:
            SetDlgItemText(IDC_FOLDER_SYNC_COMPARE, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.stop")));
            break;

        case StateSynchronize:
            SetDlgItemText(IDC_FOLDER_SYNC_SYNCHRONIZE, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.stop")));
            break;

        default:
            SetDlgItemText(IDC_FOLDER_SYNC_COMPARE, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.compare")));
            SetDlgItemText(IDC_FOLDER_SYNC_SYNCHRONIZE, gApp.loadString(XPR_STRING_LITERAL("popup.folder_sync.button.synchronize")));
            break;
        }
}

xpr_bool_t FolderSyncDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
        {
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnSynchronize();
                return XPR_TRUE;
            }
        }

        if (pMsg->hwnd == mListCtrl.m_hWnd)
        {
            if (GetKeyState(VK_CONTROL) < 0)
            {
                switch (pMsg->wParam)
                {
                case VK_LEFT:  OnPrevDiff(); return XPR_TRUE;
                case VK_RIGHT: OnNextDiff(); return XPR_TRUE;
                case 'a':
                case 'A': selectListAllItem(XPR_TRUE); return XPR_TRUE;
                }
            }
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void FolderSyncDlg::OnContextMenu(CWnd *pWnd, CPoint pt)
{
    if (!pWnd)
        return;

    if (!pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
        return;

    if (pWnd->GetDlgCtrlID() != mListCtrl.GetDlgCtrlID())
        return;

    xpr_sint_t sIndex;
    SyncItem *sSyncItem;

    CPoint sClientPoint(pt);
    mListCtrl.ScreenToClient(&sClientPoint);
    sIndex = mListCtrl.HitTest(sClientPoint);
    if (sIndex < 0)
        return;

    sSyncItem = (SyncItem *)mListCtrl.GetItemData(sIndex);
    if (sSyncItem == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    if (sSyncItem->isExistLeft() == XPR_TRUE)
        mSyncDirs->getDir(sPath, XPR_NULL);
    else
        mSyncDirs->getDir(XPR_NULL, sPath);
    _tcscat(sPath, XPR_STRING_LITERAL("\\"));
    _tcscat(sPath, sSyncItem->mSubPath.c_str());

    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_SYNC) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt, this);
    }
}

LRESULT FolderSyncDlg::OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(pMenu))
        sResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
        sResult = CDialog::OnMenuChar(nChar, nFlags, pMenu);

    return sResult;
}

void FolderSyncDlg::OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu) 
{
    //CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    ASSERT(pPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *pBCPopupMenu = dynamic_cast<BCMenu *>(pPopupMenu);
    if (pBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = pBCPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        xpr::string sMenuText;
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

                    sString = gApp.loadString(sMenuText);
                    pBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = gApp.loadString(sStringId);

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
            xpr_sint_t sIndex;
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (sIndex = 0; sIndex < sIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == pPopupMenu->m_hMenu)
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
            while (sState.m_nIndex < sCount &&
                pPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void FolderSyncDlg::OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
    xpr_bool_t sSetFlag = XPR_FALSE;
    if (lpMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)lpMeasureItemStruct->itemID))
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)lpMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu))
            {
                mMenu.MeasureItem(lpMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (sSetFlag == XPR_FALSE)
        CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void FolderSyncDlg::setItemSync(xpr_uint_t aSync)
{
    xpr_sint_t sIndex;
    SyncItem *sSyncItem;

    mListCtrl.SetRedraw(XPR_FALSE);

    POSITION sPosition = mListCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        sIndex = mListCtrl.GetNextSelectedItem(sPosition);

        sSyncItem = (SyncItem *)mListCtrl.GetItemData(sIndex);
        if (sSyncItem != XPR_NULL)
        {
            sSyncItem->setSync(aSync);
            mListCtrl.RedrawItems(sIndex, sIndex);
        }
    }

    mListCtrl.SetRedraw();
}

void FolderSyncDlg::setItemOriginalSync(void)
{
    xpr_sint_t sIndex;
    POSITION sPos;
    SyncItem *sSyncItem;

    mListCtrl.SetRedraw(XPR_FALSE);

    sPos = mListCtrl.GetFirstSelectedItemPosition();
    while (sPos)
    {
        sIndex = mListCtrl.GetNextSelectedItem(sPos);

        sSyncItem = (SyncItem *)mListCtrl.GetItemData(sIndex);
        if (sSyncItem != XPR_NULL)
        {
            sSyncItem->setOrgSync();
            mListCtrl.RedrawItems(sIndex, sIndex);
        }
    }

    mListCtrl.SetRedraw();
}

void FolderSyncDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_FOLDER_SYNC_STATUS, aStatusText);
}

void FolderSyncDlg::OnSyncLeft(void)
{
    setItemSync(SyncToLeft);
}

void FolderSyncDlg::OnSyncRight(void)
{
    setItemSync(SyncToRight);
}

void FolderSyncDlg::OnSyncNone(void)
{
    setItemSync(SyncNone);
}

void FolderSyncDlg::OnSyncOriginal(void)
{
    setItemOriginalSync();
}

void FolderSyncDlg::selectListItem(xpr_sint_t aIndex, xpr_bool_t aUnselAll, xpr_bool_t aFocus)
{
    if (aUnselAll == XPR_TRUE)
    {
        xpr_sint_t sIndex;

        POSITION sPosition = mListCtrl.GetFirstSelectedItemPosition();
        while (XPR_IS_NOT_NULL(sPosition))
        {
            sIndex = mListCtrl.GetNextSelectedItem(sPosition);

            mListCtrl.SetItemState(sIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
        }
    }

    mListCtrl.EnsureVisible(aIndex, XPR_FALSE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

    if (aFocus == XPR_TRUE)
        mListCtrl.SetFocus();
}

void FolderSyncDlg::selectListAllItem(xpr_bool_t aFocus)
{
    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
        mListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

    if (aFocus == XPR_TRUE)
        mListCtrl.SetFocus();
}

void FolderSyncDlg::OnPrevDiff(void)
{
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    SyncItem *sSyncItem;
    xpr_uint_t sSync;

    sIndex = mListCtrl.GetSelectionMark() - 1;
    sCount = mListCtrl.GetItemCount();
    for (; sIndex >= 0; --sIndex)
    {
        sSyncItem = (SyncItem *)mListCtrl.GetItemData(sIndex);
        if (sSyncItem == XPR_NULL)
            continue;

        sSync = sSyncItem->getSync();
        if (sSync != SyncNone && sSync != SyncEqualed)
            break;
    }

    if (XPR_IS_RANGE(0, sIndex, sCount-1))
        selectListItem(sIndex, XPR_TRUE, XPR_TRUE);
}

void FolderSyncDlg::OnNextDiff(void)
{
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    SyncItem *sSyncItem;
    xpr_uint_t sSync;

    sIndex = mListCtrl.GetSelectionMark() + 1;
    sCount = mListCtrl.GetItemCount();
    for (; sIndex < sCount; ++sIndex)
    {
        sSyncItem = (SyncItem *)mListCtrl.GetItemData(sIndex);
        if (sSyncItem == XPR_NULL)
            continue;

        sSync = sSyncItem->getSync();
        if (sSync != SyncNone && sSync != SyncEqualed)
            break;
    }

    if (XPR_IS_RANGE(0, sIndex, sCount-1))
        selectListItem(sIndex, XPR_TRUE, XPR_TRUE);
}

void FolderSyncDlg::OnSizeItem(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy, HDWP hdwp)
{
    CWnd *sPath1BrowseCtrl = GetDlgItem(IDC_FOLDER_SYNC_PATH1_BROWSE);
    CWnd *sPath2BrowseCtrl = GetDlgItem(IDC_FOLDER_SYNC_PATH2_BROWSE);

    if (sPath1BrowseCtrl == XPR_NULL || sPath2BrowseCtrl == XPR_NULL)
        return;

    CWnd *sWnds[4] =
    {
        &mPathComboBox[0],
        sPath1BrowseCtrl,
        &mPathComboBox[1],
        sPath2BrowseCtrl
    };

    xpr_sint_t sPathWidth = (cx - (mPathOffset[0]*2 + mPathOffset[1]*2 + mPathBrowseWidth*2 + mPathOffset[2])) / 2;

    CRect sRect;
    mPathComboBox[0].GetWindowRect(&sRect);
    ScreenToClient(&sRect);

    CRect sWndRect[4];

    sRect.right = sRect.left + sPathWidth;
    sWndRect[0] = sRect;

    sRect.left = sRect.right + mPathOffset[1];
    sRect.right = sRect.left + mPathBrowseWidth;
    sWndRect[1] = sRect;

    sRect.left = sRect.right + mPathOffset[2];
    sRect.right = sRect.left + sPathWidth;
    sWndRect[2] = sRect;

    sRect.left = sRect.right + mPathOffset[1];
    sRect.right = sRect.left + mPathBrowseWidth;
    sWndRect[3] = sRect;

    xpr_sint_t i;
    for (i = 0; i < 4; ++i)
    {
        ::DeferWindowPos(hdwp, *sWnds[i], XPR_NULL, sWndRect[i].left, sWndRect[i].top, sWndRect[i].Width(), sWndRect[i].Height(), SWP_NOZORDER);
    }
}

xpr_bool_t FolderSyncDlg::OnEraseBkgnd(CDC* pDC)
{
    xpr_uint_t sIds[] = 
    {
        IDC_FOLDER_SYNC_PATH1,
        IDC_FOLDER_SYNC_PATH2,
        0
    };

    xpr_sint_t i;
    CRect sRect;
    CWnd *sWnd;

    for (i = 0; sIds[i]; ++i)
    {
        sWnd = GetDlgItem(sIds[i]);
        if (sWnd != XPR_NULL)
        {
            sWnd->GetWindowRect(&sRect);
            ScreenToClient(&sRect);

            pDC->ExcludeClipRect(sRect);
        }
    }

    return super::OnEraseBkgnd(pDC);
}
} // namespace cmd
} // namespace fxfile
