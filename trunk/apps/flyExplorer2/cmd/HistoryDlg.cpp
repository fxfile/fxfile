//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "HistoryDlg.h"

#include "fxb/fxb_context_menu.h"

#include "DlgState.h"
#include "DlgStateMgr.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_HISTORY_REFRESH 100

HistoryDlg::HistoryDlg(xpr_size_t aHistory)
    : super(IDD_HISTORY)
    , mCurHistoryDisp(aHistory)
    , mIndex(0xffffffff)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

HistoryDlg::~HistoryDlg(void)
{
    DESTROY_ICON(mIcon);

    HistoryDispDeque::iterator sIterator;

    sIterator = mHistoryDispDeque.begin();
    for (; sIterator != mHistoryDispDeque.end(); ++sIterator)
    {
        HistoryDisp *sHistoryDisp = *sIterator;
        XPR_SAFE_DELETE(sHistoryDisp);
    }

    mHistoryDispDeque.clear();
}

void HistoryDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HISTORY_TREE, mTreeCtrl);
    DDX_Control(pDX, IDC_HISTORY_TAB,  mTabCtrl);
}

BEGIN_MESSAGE_MAP(HistoryDlg, super)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
    ON_WM_SIZE()
    ON_NOTIFY(NM_RCLICK,      IDC_HISTORY_TREE, OnRclickTree)
    ON_NOTIFY(NM_DBLCLK,      IDC_HISTORY_TREE, OnNMDblclkTree)
    ON_NOTIFY(TVN_SELCHANGED, IDC_HISTORY_TREE, OnTvnSelchangedTree)
    ON_NOTIFY(TVN_GETINFOTIP, IDC_HISTORY_TREE, OnTvnGetInfoTipTree)
    ON_NOTIFY(TCN_SELCHANGE,  IDC_HISTORY_TAB,  OnSelchangeTab)
END_MESSAGE_MAP()

xpr_bool_t HistoryDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    HICON sIconHandle;
    sIconHandle = theApp.LoadIcon(MAKEINTRESOURCE(IDI_NOT));
    mTreeImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    mTreeImgList.Add(sIconHandle);
    ::DestroyIcon(sIconHandle);

    mTreeCtrl.SetImageList(&mTreeImgList, TVSIL_NORMAL);

    //-----------------------------------------------------------------------------

    if (mToolBar.Create(this) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create toolbar\n"));
        return -1;
    }

    mToolBarImgList.Create(IDB_TB_DRIVEDLG, 16, 1, RGB(255,0,255));
    mToolBar.GetToolBarCtrl().SetImageList(&mToolBarImgList);
    mToolBar.GetToolBarCtrl().SetBitmapSize(CSize(16,16));

    mToolBar.ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
    mToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

    mToolBar.SetBorders(0, 0, 0, 0);

    DWORD sStyle = mToolBar.GetBarStyle();
    sStyle &= ~CBRS_BORDER_TOP;
    sStyle |= CBRS_FLYBY;
    mToolBar.SetBarStyle(sStyle);

    //-----------------------------------------------------------------------------

    TBBUTTON sTbButton = {0};
    sTbButton.idCommand = ID_HISTORY_REFRESH;
    sTbButton.iBitmap = 0;
    sTbButton.fsState = TBSTATE_ENABLED;
    sTbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
    sTbButton.iString = mToolBar.GetToolBarCtrl().AddStrings(theApp.loadString(XPR_STRING_LITERAL("popup.history.toolbar.refresh")));

    mToolBar.GetToolBarCtrl().AddButtons(1, &sTbButton);

    //-----------------------------------------------------------------------------
    // ControlBar Reposition

    CRect sClientStartRect;
    CRect sClientNowRect;
    GetClientRect(sClientStartRect);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, sClientNowRect);

    CPoint sOffsetPoint(sClientNowRect.left - sClientStartRect.left, sClientNowRect.top - sClientStartRect.top); 
    CRect sChildRect;
    CWnd *sChildWnd = GetWindow(GW_CHILD);
    while (XPR_IS_NOT_NULL(sChildWnd))
    {
        sChildWnd->GetWindowRect(sChildRect);
        ScreenToClient(sChildRect);
        sChildRect.OffsetRect(sOffsetPoint);
        sChildWnd->MoveWindow(sChildRect, XPR_FALSE);
        sChildWnd = sChildWnd->GetNextWindow();
    }
    CRect sWindowRect;
    GetWindowRect(sWindowRect);
    sWindowRect.right += sClientStartRect.Width() - sClientNowRect.Width();
    sWindowRect.bottom += sClientStartRect.Height() - sClientNowRect.Height();
    MoveWindow(sWindowRect, XPR_FALSE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

    //----------------------------------------------------------------------

    if (mHistoryDispDeque.size() == 1)
    {
        mTabCtrl.ShowWindow(SW_HIDE);

        CRect sTabRect;
        mTabCtrl.GetWindowRect(&sTabRect);
        ScreenToClient(&sTabRect);

        CRect sTreeRect;
        mTreeCtrl.GetWindowRect(&sTreeRect);
        ScreenToClient(&sTreeRect);

        sTreeRect.top = sTabRect.top;
        mTreeCtrl.MoveWindow(&sTreeRect);
    }

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_HISTORY_TREE,   sizeResize, sizeResize);
    AddControl(IDC_HISTORY_STATUS, sizeResize, sizeRepos);
    //------------------------------------------------------------

    HistoryDisp *sHistoryDisp;
    HistoryDispDeque::iterator sIterator;

    sIterator = mHistoryDispDeque.begin();
    for (; sIterator != mHistoryDispDeque.end(); ++sIterator)
    {
        sHistoryDisp = *sIterator;
        if (sHistoryDisp == XPR_NULL)
            continue;

        mTabCtrl.InsertItem(mTabCtrl.GetItemCount(), sHistoryDisp->mTabText);
    }

    mTabCtrl.SetCurSel((xpr_sint_t)mCurHistoryDisp);
    showTab(mCurHistoryDisp);

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("History"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void HistoryDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(nType, cx, cy);

    if (mToolBar.m_hWnd != XPR_NULL)
    {
        CRect rcToolBar;
        mToolBar.GetWindowRect(&rcToolBar);
        mToolBar.MoveWindow(0, 0, cx, rcToolBar.Height());
    }

    if (mTabCtrl.m_hWnd != XPR_NULL)
    {
        CRect sRect;
        mTabCtrl.GetWindowRect(&sRect);
        ScreenToClient(&sRect);

        sRect.right = cx - sRect.left;

        mTabCtrl.MoveWindow(&sRect, XPR_FALSE);
        mTabCtrl.Invalidate(XPR_FALSE);
    }

    CWnd *sWnd = GetDlgItem(IDC_HISTORY_STATUS);
    if (sWnd != XPR_NULL && sWnd->m_hWnd != XPR_NULL)
        sWnd->Invalidate(XPR_FALSE);
}

void HistoryDlg::OnDestroy(void)
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void HistoryDlg::getSelHistory(xpr_size_t &aHistory, xpr_uint_t &aIndex)
{
    aHistory = mCurHistoryDisp;
    aIndex   = mIndex;
}

xpr_sint_t HistoryDlg::addHistory(const xpr_tchar_t *aTitleText, const xpr_tchar_t *aTabText, fxb::HistoryDeque *aHistoryDeque)
{
    if (aTitleText == XPR_NULL || aTabText == XPR_NULL || aHistoryDeque == XPR_NULL)
        return -1;

    HistoryDisp *sHistoryDisp   = new HistoryDisp;
    sHistoryDisp->mTitleText    = aTitleText;
    sHistoryDisp->mTabText      = aTabText;
    sHistoryDisp->mHistoryDeque = aHistoryDeque;
    mHistoryDispDeque.push_back(sHistoryDisp);

    return (xpr_sint_t)mHistoryDispDeque.size() - 1;
}

xpr_uint_t HistoryDlg::getItemIndex(HTREEITEM aTreeItem)
{
    if (aTreeItem == XPR_NULL)
        return 0xffffffff;

    return (xpr_uint_t)mTreeCtrl.GetItemData(aTreeItem);
}

void HistoryDlg::updateStatus(void)
{
    HistoryDisp *sHistoryDisp = getCurHistoryDisp();
    if (sHistoryDisp == XPR_NULL)
        return;

    xpr_size_t sCount = 0;
    if (sHistoryDisp != XPR_NULL)
        sCount = sHistoryDisp->mHistoryDeque->size();

    xpr_tchar_t sWindowText[0xff] = {0};
    _stprintf(sWindowText, XPR_STRING_LITERAL("%s (%d)"), sHistoryDisp->mTitleText, sCount);
    SetWindowText(sWindowText);
}

void HistoryDlg::OnOK(void)
{
    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        mIndex = getItemIndex(sTreeItem);
    }

    super::OnOK();
}

void HistoryDlg::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    OnOK();
}

void HistoryDlg::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    if (aWnd == XPR_NULL)
        return;

    if (aWnd->m_hWnd == mTreeCtrl.m_hWnd)
    {
        CRect sWindowRect;
        GetWindowRect(&sWindowRect);

        if (sWindowRect.PtInRect(aPoint) == XPR_FALSE)
        {
            getContextMenu(XPR_FALSE);
        }
    }
}

void HistoryDlg::OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    getContextMenu(XPR_TRUE);

    *pResult = 0;
}

void HistoryDlg::getContextMenu(xpr_bool_t sRightClick)
{
    HistoryDisp *sHistoryDisp = getCurHistoryDisp();
    if (sHistoryDisp == XPR_NULL)
        return;

    CPoint sCursorPoint;
    GetCursorPos(&sCursorPoint);

    HTREEITEM sTreeItem = XPR_NULL;

    if (sRightClick == XPR_TRUE)
    {
        CPoint sHitTestPoint(sCursorPoint);
        mTreeCtrl.ScreenToClient(&sHitTestPoint);

        sTreeItem = mTreeCtrl.HitTest(sHitTestPoint);
    }
    else
    {
        sTreeItem = mTreeCtrl.GetSelectedItem();

        CRect sRect;
        mTreeCtrl.GetItemRect(sTreeItem, sRect, XPR_TRUE);
        mTreeCtrl.ClientToScreen(&sRect);
        if (!(sRect.left < sCursorPoint.x && sCursorPoint.x < sRect.right) || !(sRect.top < sCursorPoint.y && sRect.bottom < sCursorPoint.y))
        {
            sCursorPoint.x = sRect.left + (sRect.right-sRect.left)/2;
            sCursorPoint.y = sRect.bottom;
        }
    }

    if (sTreeItem == XPR_NULL)
        return;

    xpr_uint_t sIndex = getItemIndex(sTreeItem);
    if (XPR_STL_IS_INDEXABLE(sIndex, *sHistoryDisp->mHistoryDeque) == XPR_FALSE)
        return;

    fxb::HistoryDeque::reverse_iterator sReverseIterator = sHistoryDisp->mHistoryDeque->rbegin() + sIndex;
    if (sReverseIterator == sHistoryDisp->mHistoryDeque->rend())
        return;

    LPITEMIDLIST sFullPidl = *sReverseIterator;
    if (sFullPidl == XPR_NULL)
        return;

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;

    HRESULT hr = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);
    if (SUCCEEDED(hr) && sShellFolder != XPR_NULL && sPidl != XPR_NULL)
    {
        xpr_uint_t nFlags = TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON;
        fxb::ContextMenu::trackItemMenu(sShellFolder, &sPidl, 1, &sCursorPoint, nFlags, m_hWnd, CMF_EXPLORE);
    }

    COM_RELEASE(sShellFolder);
}

void HistoryDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;

    HistoryDisp *sHistoryDisp = getCurHistoryDisp();
    if (sHistoryDisp == XPR_NULL)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH * 2] = {0};

    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        xpr_uint_t sIndex = getItemIndex(sTreeItem);
        if (XPR_STL_IS_INDEXABLE(sIndex, *sHistoryDisp->mHistoryDeque))
        {
            fxb::HistoryDeque::reverse_iterator sReverseIterator = sHistoryDisp->mHistoryDeque->rbegin() + sIndex;
            if (sReverseIterator != sHistoryDisp->mHistoryDeque->rend())
            {
                LPITEMIDLIST sFullPidl = *sReverseIterator;
                if (sFullPidl != XPR_NULL)
                {
                    if (fxb::IsFileSystem(sFullPidl))
                        fxb::GetName(sFullPidl, SHGDN_FORPARSING, sPath);
                    else
                        fxb::GetFullPath(sFullPidl, sPath);
                }
            }
        }
    }

    SetDlgItemText(IDC_HISTORY_STATUS, sPath);
}

void HistoryDlg::OnTvnGetInfoTipTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
    *pResult = 0;

    HistoryDisp *sHistoryDisp = getCurHistoryDisp();
    if (sHistoryDisp == XPR_NULL)
        return;

    xpr_sint_t sIndex = (xpr_sint_t)pGetInfoTip->lParam;
    fxb::HistoryDeque::reverse_iterator sReverseIterator = sHistoryDisp->mHistoryDeque->rbegin() + sIndex;
    if (sReverseIterator != sHistoryDisp->mHistoryDeque->rend())
    {
        LPITEMIDLIST sFullPidl = *sReverseIterator;
        if (sFullPidl != XPR_NULL)
        {
            if (fxb::IsFileSystem(sFullPidl))
                fxb::GetName(sFullPidl, SHGDN_FORPARSING, pGetInfoTip->pszText);
            else
                fxb::GetFullPath(sFullPidl, pGetInfoTip->pszText);
        }
    }
}

xpr_bool_t HistoryDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->hwnd == mTreeCtrl.m_hWnd && pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_F5)
        {
            showTab(mTabCtrl.GetCurSel());
            return XPR_TRUE;
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            OnOK();
            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void HistoryDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    xpr_sint_t nIndex = mTabCtrl.GetCurSel();
    showTab(nIndex);
}

void HistoryDlg::showTab(xpr_size_t aIndex)
{
    if (XPR_STL_IS_INDEXABLE(aIndex, mHistoryDispDeque) == XPR_FALSE)
        return;

    mTreeCtrl.DeleteAllItems();

    mCurHistoryDisp = aIndex;

    HistoryDisp *sHistoryDisp = getCurHistoryDisp();
    if (sHistoryDisp == XPR_NULL)
        return;

    xpr_sint_t i;
    xpr_sint_t sImage;
    HICON sIconHandle;
    xpr_tchar_t sName[XPR_MAX_PATH * 2];
    HTREEITEM sTreeItem;

    fxb::HistoryDeque::reverse_iterator sReverseIterator;
    LPITEMIDLIST sFullPidl;

    sReverseIterator = sHistoryDisp->mHistoryDeque->rbegin();
    for (i = 0; sReverseIterator != sHistoryDisp->mHistoryDeque->rend(); ++sReverseIterator, ++i)
    {
        sFullPidl = *sReverseIterator;
        if (sFullPidl == XPR_NULL)
            continue;

        _stprintf(sName, XPR_STRING_LITERAL("[%d] "), i+1);
        fxb::GetName(sFullPidl, SHGDN_INFOLDER, sName+_tcslen(sName));

        sImage = 0;
        sIconHandle = fxb::GetItemIcon(sFullPidl);
        if (sIconHandle != XPR_NULL)
        {
            sImage = mTreeImgList.Add(sIconHandle);
            ::DestroyIcon(sIconHandle);
        }

        sTreeItem = mTreeCtrl.InsertItem(sName, sImage, sImage);
        mTreeCtrl.SetItemData(sTreeItem, i);
    }

    updateStatus();
}

HistoryDlg::HistoryDisp *HistoryDlg::getCurHistoryDisp(void)
{
    if (XPR_STL_IS_INDEXABLE(mCurHistoryDisp, mHistoryDispDeque) == XPR_FALSE)
        return XPR_NULL;

    return mHistoryDispDeque[mCurHistoryDisp];
}
