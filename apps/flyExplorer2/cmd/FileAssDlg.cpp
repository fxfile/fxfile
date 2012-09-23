//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FileAssDlg.h"

#include "fxb/fxb_context_menu.h"
#include "DlgState.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FileAssDlg::FileAssDlg(void)
    : super(IDD_FILE_ASS)
    , mFileAssItem(XPR_NULL)
{
}

FileAssDlg::~FileAssDlg(void)
{
    mList.clear();
}

void FileAssDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILEASS_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(FileAssDlg, super)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
    ON_NOTIFY(NM_RCLICK,      IDC_FILEASS_TREE, OnRclickTree)
    ON_NOTIFY(TVN_SELCHANGED, IDC_FILEASS_TREE, OnTvnSelchangedTree)
    ON_NOTIFY(TVN_GETINFOTIP, IDC_FILEASS_TREE, OnTvnGetInfoTipTree)
    ON_NOTIFY(NM_DBLCLK,      IDC_FILEASS_TREE, OnNMDblclkTree)
END_MESSAGE_MAP()

xpr_bool_t FileAssDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    HICON sIconHandle;
    sIconHandle = theApp.LoadIcon(MAKEINTRESOURCE(IDI_NOT));
    mImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    mImgList.Add(sIconHandle);
    ::DestroyIcon(sIconHandle);

    mTreeCtrl.SetImageList(&mImgList, TVSIL_NORMAL);

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_FILEASS_TREE,   sizeResize, sizeResize);
    AddControl(IDC_FILEASS_STATUS, sizeResize, sizeRepos);
    //------------------------------------------------------------

    std::tstring sPath;

    sPath = mPath;
    xpr_size_t sFind = mPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != std::tstring::npos)
        sPath = sPath.substr(sFind+1);

    SetDlgItemText(IDC_FILEASS_FILE, sPath.c_str());

    xpr_sint_t sImageIndex;
    std::tstring sText;
    std::tstring sFileName;
    LPITEMIDLIST sFullPidl;
    TV_INSERTSTRUCT sTvInsertStruct = {0};
    fxb::FileAssItem *sFileAssItem;
    fxb::FileAssDeque::iterator sIterator;

    sIterator = mList.begin();
    for (; sIterator != mList.end(); ++sIterator)
    {
        sFileAssItem = *sIterator;
        if (sFileAssItem == XPR_NULL)
            continue;

        sFileName = sFileAssItem->mPath;
        xpr_size_t sFind = sFileName.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != std::tstring::npos)
            sFileName = sFileName.substr(sFind+1);

        sText  = sFileAssItem->mName;
        sText += XPR_STRING_LITERAL(" (") + sFileName + XPR_STRING_LITERAL(")");

        sPath = sFileAssItem->mPath;
        fxb::GetEnvRealPath(sFileAssItem->mPath, sPath);

        sImageIndex = -1;

        sFullPidl = fxb::SHGetPidlFromPath(sPath.c_str());
        if (sFullPidl != XPR_NULL)
        {
            sIconHandle = fxb::GetItemIcon(sFullPidl);
            if (sIconHandle != XPR_NULL)
            {
                sImageIndex = mImgList.Add(sIconHandle);
                DESTROY_ICON(sIconHandle);
            }

            COM_FREE(sFullPidl);
        }

        if (sImageIndex == -1)
            sImageIndex = 0;

        sTvInsertStruct.item.mask           = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        sTvInsertStruct.item.iSelectedImage = sImageIndex;
        sTvInsertStruct.item.iImage         = sImageIndex;
        sTvInsertStruct.item.pszText        = (xpr_tchar_t *)sText.c_str();
        sTvInsertStruct.item.lParam         = (LPARAM)sFileAssItem;

        sTvInsertStruct.hInsertAfter = TVI_LAST;
        sTvInsertStruct.hParent      = TVI_ROOT;

        mTreeCtrl.InsertItem(&sTvInsertStruct);
    }

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.file_ass.title")));
    SetDlgItemText(IDC_FILEASS_LABEL_FILE, theApp.loadString(XPR_STRING_LITERAL("popup.file_ass.label.file")));

    // Load Dialog State
    mState.setSection(XPR_STRING_LITERAL("FileAss"));
    mState.setDialog(this, XPR_TRUE);
    mState.load();

    return XPR_TRUE;
}

void FileAssDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(nType, cx, cy);

    CWnd *sWnd = GetDlgItem(IDC_FILEASS_STATUS);
    if (sWnd != XPR_NULL && sWnd->m_hWnd != XPR_NULL)
        sWnd->Invalidate(XPR_FALSE);
}

void FileAssDlg::OnDestroy(void)
{
    super::OnDestroy();

    // Save Dialog State
    mState.reset();
    mState.save();
}

void FileAssDlg::setPath(const std::tstring &aPath)
{
    mPath = aPath;
}

void FileAssDlg::setList(const fxb::FileAssDeque &aList)
{
    mList.insert(mList.begin(), aList.begin(), aList.end());
}

fxb::FileAssItem *FileAssDlg::getSelFileAssItem(void)
{
    return mFileAssItem;
}

void FileAssDlg::OnOK(void)
{
    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        mFileAssItem = (fxb::FileAssItem *)mTreeCtrl.GetItemData(sTreeItem);
    }

    super::OnOK();
}

void FileAssDlg::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    OnOK();
}

void FileAssDlg::OnContextMenu(CWnd* pWnd, CPoint pt)
{
    if (pWnd == XPR_NULL)
        return;

    if (pWnd->m_hWnd == mTreeCtrl.m_hWnd)
    {
        CRect sWindowRect;
        GetWindowRect(&sWindowRect);

        if (sWindowRect.PtInRect(pt) == XPR_FALSE)
        {
            getContextMenu(XPR_FALSE);
        }
    }
}

void FileAssDlg::OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    getContextMenu(XPR_TRUE);

    *pResult = 0;
}

void FileAssDlg::getContextMenu(xpr_bool_t sRightClick)
{
    CPoint sCursorPos;
    GetCursorPos(&sCursorPos);

    HTREEITEM sTreeItem = XPR_NULL;

    if (sRightClick == XPR_TRUE)
    {
        CPoint sHitTest(sCursorPos);
        mTreeCtrl.ScreenToClient(&sHitTest);

        sTreeItem = mTreeCtrl.HitTest(sHitTest);
    }
    else
    {
        sTreeItem = mTreeCtrl.GetSelectedItem();

        CRect sRect;
        mTreeCtrl.GetItemRect(sTreeItem, sRect, XPR_TRUE);
        mTreeCtrl.ClientToScreen(&sRect);
        if (!(sRect.left < sCursorPos.x && sCursorPos.x < sRect.right) || !(sRect.top < sCursorPos.y && sRect.bottom < sCursorPos.y))
        {
            sCursorPos.x = sRect.left + (sRect.right-sRect.left)/2;
            sCursorPos.y = sRect.bottom;
        }
    }

    if (sTreeItem == XPR_NULL)
        return;

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)mTreeCtrl.GetItemData(sTreeItem);
    if (sFileAssItem == XPR_NULL)
        return;

    std::tstring sPath;
    fxb::GetEnvRealPath(sFileAssItem->mPath, sPath);

    LPITEMIDLIST sFullPidl = fxb::SHGetPidlFromPath(sPath.c_str());
    if (sFullPidl == XPR_NULL)
        return;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;

    HRESULT sResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);
    if (SUCCEEDED(sResult) && sShellFolder != XPR_NULL && sPidl != XPR_NULL)
    {
        xpr_uint_t sFlags = TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON;
        fxb::ContextMenu::trackItemMenu(sShellFolder, &sPidl, 1, &sCursorPos, sFlags, m_hWnd, CMF_EXPLORE);
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);
}

void FileAssDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;

    std::tstring sPath;

    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)mTreeCtrl.GetItemData(sTreeItem);
        if (sFileAssItem != XPR_NULL)
            sPath = sFileAssItem->mPath;
    }

    SetDlgItemText(IDC_FILEASS_STATUS, sPath.c_str());
}

void FileAssDlg::OnTvnGetInfoTipTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
    *pResult = 0;

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)pGetInfoTip->lParam;
    if (sFileAssItem == XPR_NULL)
        return;

    _stprintf(pGetInfoTip->pszText, XPR_STRING_LITERAL("%s\n%s"), sFileAssItem->mName.c_str(), sFileAssItem->mPath.c_str());
}

xpr_bool_t FileAssDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->hwnd == mTreeCtrl.m_hWnd && pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
        {
            OnOK();
            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(pMsg);
}
