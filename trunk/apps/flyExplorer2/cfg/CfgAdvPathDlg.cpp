//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvPathDlg.h"

#include "../resource.h"
#include "../CfgPath.h"

#include "SelFolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAdvPathDlg::CfgAdvPathDlg(void)
    : super(IDD_CFG_ADV_PATH, XPR_NULL)
{
}

void CfgAdvPathDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_CFG_PATH_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(CfgAdvPathDlg, super)
    ON_NOTIFY(LVN_GETDISPINFO,  IDC_CFG_CFG_PATH_LIST, OnLvnGetdispinfoList)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CFG_CFG_PATH_LIST, OnLvnItemActivate)
    ON_NOTIFY(LVN_DELETEITEM,   IDC_CFG_CFG_PATH_LIST, OnLvnDeleteitemList)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE, OnCustomPathBrowse)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_SET_PATH,           OnSetPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_PROGRAM,            OnCfgPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_APPDATA,            OnCfgPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_CUSTOM,             OnCfgPath)
    ON_BN_CLICKED(IDC_CFG_CFG_PATH_EACH_CUSTOM,        OnCfgPath)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvPathDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE);
    addIgnoreApply(IDC_CFG_CFG_PATH_SET_PATH);

    ((CEdit *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH))->LimitText(XPR_MAX_PATH);

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_CONFIG_TREE);
    mImageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 18, 18);
    mImageList.Add(&sBitmap, RGB(255,0,255));
    sBitmap.DeleteObject();

    mListCtrl.SetImageList(&mImageList, LVSIL_SMALL);

    DWORD sExStyle = mListCtrl.GetExtendedStyle();
    sExStyle |= LVS_EX_FULLROWSELECT;
    mListCtrl.SetExtendedStyle(sExStyle);

    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.list.column.item")),     LVCFMT_LEFT,  110, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.list.column.filename")), LVCFMT_LEFT,  100, -1);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.list.column.path")),     LVCFMT_LEFT,  200, -1);

    SetDlgItemText(IDC_CFG_CFG_PATH_PROGRAM,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.program")));
    SetDlgItemText(IDC_CFG_CFG_PATH_APPDATA,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.appdata")));
    SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.custom")));
    SetDlgItemText(IDC_CFG_CFG_PATH_EACH_CUSTOM, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.radio.each_custom")));
    SetDlgItemText(IDC_CFG_CFG_PATH_SET_PATH,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.button.set_path")));

    return XPR_TRUE;
}

void CfgAdvPathDlg::onInit(Option::Config &aConfig)
{
    CfgPath &sCfgPath = CfgPath::instance();

    if (sCfgPath.isTypeAll() == XPR_TRUE)
    {
        std::tstring sPath;
        sCfgPath.getCfgPath(CfgPath::TypeAll, sPath);

        if (_tcsicmp(sPath.c_str(), XPR_STRING_LITERAL("%flyExplorer%")) == 0)
            ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM))->SetCheck(XPR_TRUE);
        else if (_tcsicmp(sPath.c_str(), XPR_STRING_LITERAL("%AppData%")) == 0)
            ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA))->SetCheck(XPR_TRUE);
        else
        {
            ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM))->SetCheck(XPR_TRUE);
            SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sPath.c_str());
        }
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_EACH_CUSTOM))->SetCheck(XPR_TRUE);
    }

    xpr_sint_t sType;
    xpr_sint_t sIndex = 0;
    LVITEM sLvItem = {0};
    Item *sItem;
    std::tstring sPath;

    for (sType = CfgPath::TypeBegin; sType < CfgPath::TypeEnd; ++sType)
    {
        if (sCfgPath.getCfgPath(sType, sPath) == XPR_FALSE)
            continue;

        sItem = new Item;
        sItem->mType = sType;
        sItem->mPath = sPath;

        sLvItem.mask       = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        sLvItem.iItem      = sIndex++;
        sLvItem.iSubItem   = 0;
        sLvItem.iImage     = I_IMAGECALLBACK;
        sLvItem.pszText    = LPSTR_TEXTCALLBACK;
        sLvItem.cchTextMax = XPR_MAX_PATH;
        sLvItem.lParam     = (LPARAM)sItem;
        mListCtrl.InsertItem(&sLvItem);
    }

    OnCfgPath();
}

void CfgAdvPathDlg::onApply(Option::Config &aConfig)
{
    CfgPath &sCfgPath = CfgPath::instance();

    sCfgPath.setBackup(XPR_TRUE);

    if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM))->GetCheck())
    {
        sCfgPath.clear();
        sCfgPath.setCfgPath(CfgPath::TypeAll, XPR_STRING_LITERAL("%flyExplorer%"));
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA))->GetCheck())
    {
        sCfgPath.clear();
        sCfgPath.setCfgPath(CfgPath::TypeAll, XPR_STRING_LITERAL("%AppData%\\flyExplorer"));
    }
    else if (((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM))->GetCheck())
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        GetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sPath, XPR_MAX_PATH);

        sCfgPath.clear();
        sCfgPath.setCfgPath(CfgPath::TypeAll, sPath);
    }
    else
    {
        xpr_sint_t i;
        xpr_sint_t sCount;
        Item *sItem;

        sCount = mListCtrl.GetItemCount();
        for (i = 0; i < sCount; ++i)
        {
            sItem = (Item *)mListCtrl.GetItemData(i);
            if (sItem == XPR_NULL)
                continue;

            sCfgPath.setCfgPath(sItem->mType, sItem->mPath.c_str());
        }
    }

    sCfgPath.setBackup(XPR_FALSE);

    if (sCfgPath.isChangedCfgPath() == XPR_TRUE)
    {
        sCfgPath.moveToNewCfgPath();
    }
}

void CfgAdvPathDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    Item *sItem = (Item *)pDispInfo->item.lParam;
    if (sItem == XPR_NULL)
        return;

    LVITEM &sLvItem = pDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        sLvItem.pszText[0] = XPR_STRING_LITERAL('\0');

        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                const xpr_tchar_t *sDispName = CfgPath::getDispName(sItem->mType);
                if (sDispName != XPR_NULL)
                    _tcscpy(sLvItem.pszText, sDispName);
                break;
            }

        case 1:
            {
                const xpr_tchar_t *sFileName = CfgPath::getFileName(sItem->mType);
                if (sFileName != XPR_NULL)
                    _tcscpy(sLvItem.pszText, sFileName);
                break;
            }

        case 2:
            {
                _tcscpy(sLvItem.pszText, sItem->mPath.c_str());
                break;
            }
        }
    }

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_IMAGE))
    {
        sLvItem.iImage = 23;
    }
}

void CfgAdvPathDlg::OnLvnItemActivate(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)pNMHDR;
    *pResult = 0;

    OnSetPath();
}

void CfgAdvPathDlg::OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    Item *sItem = (Item *)pNMListView->lParam;
    XPR_SAFE_DELETE(sItem);
}

xpr_bool_t CfgAdvPathDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        if (GetKeyState(VK_CONTROL) < 0 || pMsg->hwnd == mListCtrl.m_hWnd)
        {
            OnSetPath();
            return XPR_TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void CfgAdvPathDlg::OnCustomPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = fxb::Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
        {
            SetDlgItemText(IDC_CFG_CFG_PATH_CUSTOM_PATH, sNewPath);

            setModified();
        }
    }
}

void CfgAdvPathDlg::OnSetPath(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    Item *sItem = (Item *)mListCtrl.GetItemData(sIndex);
    if (sItem == XPR_NULL)
        return;

    const xpr_tchar_t *sDispName = CfgPath::getDispName(sItem->mType);
    const xpr_tchar_t *sFileName = CfgPath::getFileName(sItem->mType);

    SelFolderDlg sDlg;
    sDlg.setFolder(sItem->mPath.c_str());
    if (sDlg.DoModal() != IDOK)
        return;

    const xpr_tchar_t *sSelFolder = sDlg.getSelFolder();
    if (sSelFolder == XPR_NULL)
        return;

    if (_tcsicmp(sItem->mPath.c_str(), sSelFolder) == 0)
        return;

    sItem->mPath = sSelFolder;

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM    ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA    ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM     ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_EACH_CUSTOM))->SetCheck(XPR_TRUE);

    setModified();
}

void CfgAdvPathDlg::OnCfgPath(void)
{
    xpr_bool_t sProgram    = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_PROGRAM    ))->GetCheck();
    xpr_bool_t sAppData    = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_APPDATA    ))->GetCheck();
    xpr_bool_t sCustom     = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM     ))->GetCheck();
    xpr_bool_t sEachCustom = ((CButton *)GetDlgItem(IDC_CFG_CFG_PATH_EACH_CUSTOM))->GetCheck();

    GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH       )->EnableWindow(sCustom);
    GetDlgItem(IDC_CFG_CFG_PATH_CUSTOM_PATH_BROWSE)->EnableWindow(sCustom);
    GetDlgItem(IDC_CFG_CFG_PATH_LIST              )->EnableWindow(sEachCustom);
    GetDlgItem(IDC_CFG_CFG_PATH_SET_PATH          )->EnableWindow(sEachCustom);
}
