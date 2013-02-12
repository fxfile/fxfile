//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ProgramAssDlg.h"

#include "fxb/fxb_context_menu.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ProgramAssDlg::ProgramAssDlg(void)
    : super(IDD_PROGRAM_ASS)
    , mProgramAssItem(XPR_NULL)
    , mDlgState(XPR_NULL)
{
}

ProgramAssDlg::~ProgramAssDlg(void)
{
    mList.clear();
}

void ProgramAssDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRAM_ASS_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(ProgramAssDlg, super)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
    ON_NOTIFY(NM_RCLICK,      IDC_PROGRAM_ASS_TREE, OnRclickTree)
    ON_NOTIFY(TVN_SELCHANGED, IDC_PROGRAM_ASS_TREE, OnTvnSelchangedTree)
    ON_NOTIFY(TVN_GETINFOTIP, IDC_PROGRAM_ASS_TREE, OnTvnGetInfoTipTree)
    ON_NOTIFY(NM_DBLCLK,      IDC_PROGRAM_ASS_TREE, OnNMDblclkTree)
END_MESSAGE_MAP()

xpr_bool_t ProgramAssDlg::OnInitDialog(void) 
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
    AddControl(IDC_PROGRAM_ASS_TREE,   sizeResize, sizeResize);
    AddControl(IDC_PROGRAM_ASS_STATUS, sizeResize, sizeRepos);
    //------------------------------------------------------------

    std::tstring sPath;

    sPath = mPath;
    xpr_size_t sFind = mPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != std::tstring::npos)
        sPath = sPath.substr(sFind+1);

    SetDlgItemText(IDC_PROGRAM_ASS_FILE, sPath.c_str());

    xpr_sint_t sImageIndex;
    std::tstring sText;
    std::tstring sFileName;
    LPITEMIDLIST sFullPidl;
    TV_INSERTSTRUCT sTvInsertStruct = {0};
    fxb::ProgramAssItem *sProgramAssItem;
    fxb::ProgramAssDeque::iterator sIterator;

    sIterator = mList.begin();
    for (; sIterator != mList.end(); ++sIterator)
    {
        sProgramAssItem = *sIterator;
        if (sProgramAssItem == XPR_NULL)
            continue;

        sFileName = sProgramAssItem->mPath;
        xpr_size_t sFind = sFileName.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != std::tstring::npos)
            sFileName = sFileName.substr(sFind+1);

        sText  = sProgramAssItem->mName;
        sText += XPR_STRING_LITERAL(" (") + sFileName + XPR_STRING_LITERAL(")");

        sPath = sProgramAssItem->mPath;
        fxb::GetEnvRealPath(sProgramAssItem->mPath, sPath);

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
        sTvInsertStruct.item.lParam         = (LPARAM)sProgramAssItem;

        sTvInsertStruct.hInsertAfter = TVI_LAST;
        sTvInsertStruct.hParent      = TVI_ROOT;

        mTreeCtrl.InsertItem(&sTvInsertStruct);
    }

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.program_ass.title")));
    SetDlgItemText(IDC_PROGRAM_ASS_LABEL_FILE, theApp.loadString(XPR_STRING_LITERAL("popup.program_ass.label.file")));

    // Load Dialog State
    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("ProgramAss"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void ProgramAssDlg::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    CWnd *sWnd = GetDlgItem(IDC_PROGRAM_ASS_STATUS);
    if (sWnd != XPR_NULL && sWnd->m_hWnd != XPR_NULL)
        sWnd->Invalidate(XPR_FALSE);
}

void ProgramAssDlg::OnDestroy(void)
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void ProgramAssDlg::setPath(const std::tstring &aPath)
{
    mPath = aPath;
}

void ProgramAssDlg::setList(const fxb::ProgramAssDeque &aList)
{
    mList.insert(mList.begin(), aList.begin(), aList.end());
}

fxb::ProgramAssItem *ProgramAssDlg::getSelProgramAssItem(void)
{
    return mProgramAssItem;
}

void ProgramAssDlg::OnOK(void)
{
    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        mProgramAssItem = (fxb::ProgramAssItem *)mTreeCtrl.GetItemData(sTreeItem);
    }

    super::OnOK();
}

void ProgramAssDlg::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    OnOK();
}

void ProgramAssDlg::OnContextMenu(CWnd* pWnd, CPoint pt)
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

void ProgramAssDlg::OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    getContextMenu(XPR_TRUE);

    *pResult = 0;
}

void ProgramAssDlg::getContextMenu(xpr_bool_t sRightClick)
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

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)mTreeCtrl.GetItemData(sTreeItem);
    if (sProgramAssItem == XPR_NULL)
        return;

    std::tstring sPath;
    fxb::GetEnvRealPath(sProgramAssItem->mPath, sPath);

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

void ProgramAssDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;

    std::tstring sPath;

    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)mTreeCtrl.GetItemData(sTreeItem);
        if (sProgramAssItem != XPR_NULL)
            sPath = sProgramAssItem->mPath;
    }

    SetDlgItemText(IDC_PROGRAM_ASS_STATUS, sPath.c_str());
}

void ProgramAssDlg::OnTvnGetInfoTipTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
    *pResult = 0;

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)pGetInfoTip->lParam;
    if (sProgramAssItem == XPR_NULL)
        return;

    _stprintf(pGetInfoTip->pszText, XPR_STRING_LITERAL("%s\n%s"), sProgramAssItem->mName.c_str(), sProgramAssItem->mPath.c_str());
}

xpr_bool_t ProgramAssDlg::PreTranslateMessage(MSG* pMsg)
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
