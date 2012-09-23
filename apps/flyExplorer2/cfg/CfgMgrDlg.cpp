//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgMgrDlg.h"

#include "../resource.h"

#include "CfgGeneralDlg.h"
#include "CfgContentsDlg.h"
#include "CfgFldDlg.h"
#include "CfgFldWndDlg.h"
#include "CfgExpDlg.h"
#include "CfgExpViewSetDlg.h"
#include "CfgExpWndDlg.h"
#include "CfgExpDispDlg.h"
#include "CfgExpFilterDlg.h"
#include "CfgExpThumbDlg.h"
#include "CfgDispDlg.h"
#include "CfgDispSizeDlg.h"
#include "CfgFuncDlg.h"
#include "CfgFuncBookmarkDlg.h"
#include "CfgFuncRenameDlg.h"
#include "CfgFuncFileAssDlg.h"
#include "CfgFuncRefreshDlg.h"
#include "CfgFuncDragDlg.h"
#include "CfgFuncSplitDlg.h"
#include "CfgFuncFileScrapDlg.h"
#include "CfgFuncHistoryDlg.h"
#include "CfgFuncFileOpDlg.h"
#include "CfgAdvDlg.h"
#include "CfgAdvPathDlg.h"
#include "CfgAdvStateDlg.h"
#include "CfgAdvHotKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

xpr_sint_t CfgMgrDlg::mLastShowCfg = 0;

CfgMgrDlg::CfgMgrDlg(xpr_sint_t aInitShowCfg)
    : super(IDD_CFG_MGR, XPR_NULL)
    , mInitShowCfg(aInitShowCfg)
    , mShowCfg(-1)
{
    if (mInitShowCfg == -1)
        mInitShowCfg = mLastShowCfg;
}

void CfgMgrDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(CfgMgrDlg, super)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_NOTIFY(TVN_SELCHANGED, IDC_CFG_TREE, OnSelchangedTree)
    ON_BN_CLICKED(IDC_CFG_DEFAULT, OnDefault)
    ON_BN_CLICKED(IDC_CFG_APPLY,   OnApply)
    ON_MESSAGE(WM_SETMODIFIED, OnSetModified)
    ON_MESSAGE(WM_GETMODIFIED, OnGetModified)
END_MESSAGE_MAP()

xpr_bool_t CfgMgrDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDarkGrayBrush.CreateSolidBrush(RGB(127,127,127));

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_CONFIG_TREE);
    mImgList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 18, 18);
    mImgList.Add(&sBitmap, RGB(255,0,255));
    sBitmap.DeleteObject();

    mTreeCtrl.SetImageList(&mImgList, TVSIL_NORMAL);
    mTreeCtrl.SetItemHeight(mTreeCtrl.GetItemHeight() + 2);

    // Init Configuration Dialog
    xpr_sint_t sIndex;
    sIndex = addCfgItem( 0,     -1, new CfgGeneralDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.general")));
    sIndex = addCfgItem( 4,     -1, new CfgFldDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(0),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view1")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(1),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view2")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(2),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view3")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(3),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view4")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(4),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view5")));
    /*    */ addCfgItem( 4, sIndex, new CfgFldWndDlg(5),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.folder_pane.view6")));
    sIndex = addCfgItem( 1,     -1, new CfgContentsDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.contents")));
    sIndex = addCfgItem(12,     -1, new CfgExpDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window")));
    /*    */ addCfgItem(20, sIndex, new CfgExpViewSetDlg,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view_set")));
    /*    */ addCfgItem( 9, sIndex, new CfgExpWndDlg(0),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view1")));
    /*    */ addCfgItem(10, sIndex, new CfgExpWndDlg(1),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view2")));
    /*    */ addCfgItem(10, sIndex, new CfgExpWndDlg(2),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view3")));
    /*    */ addCfgItem(10, sIndex, new CfgExpWndDlg(3),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view4")));
    /*    */ addCfgItem(10, sIndex, new CfgExpWndDlg(4),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view5")));
    /*    */ addCfgItem(10, sIndex, new CfgExpWndDlg(5),     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.view6")));
    /*    */ addCfgItem(19, sIndex, new CfgExpDispDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.display")));
    /*    */ addCfgItem(11, sIndex, new CfgExpFilterDlg,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.filtering")));
    /*    */ addCfgItem( 5, sIndex, new CfgExpThumbDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.explorer_window.thumbnail")));
    sIndex = addCfgItem(18,     -1, new CfgDispDlg,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.display")));
    /*    */ addCfgItem(22, sIndex, new CfgDispSizeDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.display.size")));
    sIndex = addCfgItem(17,     -1, new CfgFuncDlg,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function")));
    /*    */ addCfgItem(13, sIndex, new CfgFuncBookmarkDlg,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.bookmark")));
    /*    */ addCfgItem( 7, sIndex, new CfgFuncRenameDlg,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.rename")));
    /*    */ addCfgItem(21, sIndex, new CfgFuncFileAssDlg,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.view_edit")));
    /*    */ addCfgItem(14, sIndex, new CfgFuncRefreshDlg,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.refresh")));
    /*    */ addCfgItem( 8, sIndex, new CfgFuncDragDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.drag_drop")));
    /*    */ addCfgItem(15, sIndex, new CfgFuncSplitDlg,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.split_view")));
    /*    */ addCfgItem(16, sIndex, new CfgFuncFileScrapDlg, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.scrap")));
    /*    */ addCfgItem(25, sIndex, new CfgFuncHistoryDlg,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.history")));
    /*    */ addCfgItem(26, sIndex, new CfgFuncFileOpDlg,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.operation")));
    sIndex = addCfgItem( 6,     -1, new CfgAdvDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced")));
    /*    */ addCfgItem(23, sIndex, new CfgAdvPathDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced.cfg_path")));
    /*    */ addCfgItem(23, sIndex, new CfgAdvStateDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced.dialog_state")));
    /*    */ addCfgItem(24, sIndex, new CfgAdvHotKeyDlg,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced.global_hotkey")));

    if (!XPR_STL_IS_INDEXABLE(mInitShowCfg, mCfgDeque))
        mInitShowCfg = 0;

    // Show Init Dialog
    //showCfg(mShowCfg);

    // expand all
    HTREEITEM sTreeItem = mTreeCtrl.GetRootItem();
    while (sTreeItem != XPR_NULL)
    {
        if (mTreeCtrl.ItemHasChildren(sTreeItem))
            mTreeCtrl.Expand(sTreeItem, TVE_EXPAND);

        sTreeItem = mTreeCtrl.GetNextSiblingItem(sTreeItem);
    }

    mTreeCtrl.SelectItem(mCfgDeque[mInitShowCfg]->mTreeItem);
    mTreeCtrl.EnsureVisible(mCfgDeque[mInitShowCfg]->mTreeItem);

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);
    mCfgDeque[mInitShowCfg]->mModified = XPR_FALSE;

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.title")));
    SetDlgItemText(IDC_CFG_DEFAULT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.button.default")));
    SetDlgItemText(IDC_CFG_APPLY,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.button.apply")));
    SetDlgItemText(IDOK,            theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,        theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

xpr_sint_t CfgMgrDlg::addCfgItem(xpr_sint_t aImage, xpr_sint_t aParent, CfgDlg *aCfgDlg, const xpr_tchar_t *aText)
{
    CfgItem *sCfgItem = new CfgItem;
    if (sCfgItem == XPR_NULL)
        return -1;

    if (!XPR_STL_IS_INDEXABLE(aParent, mCfgDeque))
        aParent = -1;

    HTREEITEM sParentTreeItem = TVI_ROOT;
    if (aParent != -1)
        sParentTreeItem = mCfgDeque[aParent]->mTreeItem;

    sCfgItem->mTreeItem = mTreeCtrl.InsertItem(aText, aImage, aImage, sParentTreeItem);
    sCfgItem->mText     = aText;
    sCfgItem->mImage    = aImage;
    sCfgItem->mParent   = aParent;
    sCfgItem->mCfgDlg   = aCfgDlg;
    sCfgItem->mModified = XPR_FALSE;

    mCfgDeque.push_back(sCfgItem);

    return (xpr_sint_t)mCfgDeque.size() - 1;
}

xpr_bool_t CfgMgrDlg::showCfg(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mCfgDeque))
        return XPR_FALSE;

    CfgItem *sCfgItem = mCfgDeque[aIndex];

    // Hide
    if (mShowCfg != -1)
    {
        if (XPR_STL_IS_INDEXABLE(mShowCfg, mCfgDeque))
        {
            CfgItem *sOldCfgItem = mCfgDeque[mShowCfg];

            if (sOldCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
                sOldCfgItem->mCfgDlg->ShowWindow(SW_HIDE);
        }
    }

    // Show
    if (sCfgItem->mCfgDlg->m_hWnd == XPR_NULL)
    {
        sCfgItem->mCfgDlg->Create(aIndex, this);

        CRect sDescRect;
        GetDlgItem(IDC_CFG_DESC)->GetWindowRect(&sDescRect);
        ScreenToClient(&sDescRect);

        HDC sDC = ::GetDC(XPR_NULL);
        xpr_sint_t y = MulDiv(44, GetDeviceCaps(sDC , LOGPIXELSY), 96);
        ::ReleaseDC(XPR_NULL, sDC);

        sCfgItem->mCfgDlg->SetWindowPos(XPR_NULL, sDescRect.left, y, 0, 0, SWP_NOSIZE);
        //sCfgItem->mCfgDlg->SetWindowPos(XPR_NULL, 190, 44, 0, 0, SWP_NOSIZE);
    }

    sCfgItem->mCfgDlg->ShowWindow(SW_SHOW);

    // description (e.g. Configuration > Function > Bookmark)
    std::tstring sDesc(XPR_STRING_LITERAL("  "));
    xpr_sint_t sParent = sCfgItem->mParent;

    sDesc += theApp.loadString(XPR_STRING_LITERAL("popup.cfg.title"));
    sDesc += XPR_STRING_LITERAL(" > ");
    sDesc += sCfgItem->mText;

    while (sParent != -1)
    {
        if (!XPR_STL_IS_INDEXABLE(sParent, mCfgDeque))
            break;

        sDesc += XPR_STRING_LITERAL(" > ");
        sDesc += mCfgDeque[sParent]->mText;

        sParent = mCfgDeque[sParent]->mParent;
    }

    SetDlgItemText(IDC_CFG_DESC, sDesc.c_str());

    UpdateWindow();

    return XPR_TRUE;
}

void CfgMgrDlg::OnDestroy(void)
{
    super::OnDestroy();

    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (; sIterator != mCfgDeque.end(); ++sIterator)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
            sCfgItem->mCfgDlg->DestroyWindow();

        XPR_SAFE_DELETE(sCfgItem->mCfgDlg);
        XPR_SAFE_DELETE(sCfgItem);
    }

    mCfgDeque.clear();
}

void CfgMgrDlg::showCfgByTree(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mCfgDeque))
        return;

    //xpr_bool_t sModified = mCfgDeque[aIndex]->mModified;
    mChanging = XPR_TRUE;

    showCfg(aIndex);

    mChanging = XPR_FALSE;
    //::SendMessage(m_hWnd, WM_SETMODIFIED, aIndex, sModified);
}

void CfgMgrDlg::OnOK(void) 
{
    OnApply();

    super::OnOK();
}

void CfgMgrDlg::OnCancel(void) 
{
    super::OnCancel();
}

void CfgMgrDlg::OnApply(void)
{
    CWaitCursor sWaitCursor;

    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (; sIterator != mCfgDeque.end(); ++sIterator)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mModified == XPR_TRUE && sCfgItem->mCfgDlg != XPR_NULL && sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
        {
            ::SendMessage(sCfgItem->mCfgDlg->GetSafeHwnd(), WM_APPLY, 0, 0);
            sCfgItem->mModified = XPR_FALSE;
        }
    }

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);

    OptionMgr::instance().save();
}

LRESULT CfgMgrDlg::OnSetModified(WPARAM wParam, LPARAM lParam)
{
    if (mChanging == XPR_TRUE)
        return 0;

    xpr_size_t sIndex = (xpr_size_t)wParam;
    xpr_bool_t sModified = (xpr_bool_t)lParam;

    xpr_size_t sCount = mCfgDeque.size();
    if (XPR_STL_IS_INDEXABLE(sIndex, mCfgDeque))
    {
        mCfgDeque[sIndex]->mModified = sModified;

        xpr_size_t i;
        CfgItem *sCfgItem;
        CfgDeque::iterator sIterator;
        xpr_bool_t sEnable = XPR_FALSE;

        sIterator = mCfgDeque.begin();
        for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
        {
            sCfgItem = *sIterator;
            if (sCfgItem == XPR_NULL)
                continue;

            if (sCfgItem->mModified == XPR_TRUE)
            {
                sEnable = XPR_TRUE;
                break;
            }
        }

        GetDlgItem(IDC_CFG_APPLY)->EnableWindow(sEnable);
    }

    return XPR_TRUE;
}

LRESULT CfgMgrDlg::OnGetModified(WPARAM wParam, LPARAM lParam)
{
    xpr_sint_t sIndex = (xpr_sint_t)wParam;

    if (!XPR_STL_IS_INDEXABLE(sIndex, mCfgDeque))
        return 0;

    return mCfgDeque[sIndex]->mModified;
}

void CfgMgrDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    *pResult = 0;

    HTREEITEM sTreeItem = pNMTreeView->itemNew.hItem;

    xpr_sint_t i;
    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sTreeItem == sCfgItem->mTreeItem)
        {
            showCfgByTree(i);
            break;
        }
    }

    mShowCfg = i;
    mLastShowCfg = mShowCfg;
}

void CfgMgrDlg::OnDefault(void) 
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.confirm_default"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING | MB_YESNO);
    if (sMsgId != IDYES)
        return;

    OptionMgr &sOptionMgr = OptionMgr::instance();

    sOptionMgr.initDefault();

    sOptionMgr.applyFolderCtrl(XPR_FALSE);
    sOptionMgr.applyExplorerView(XPR_FALSE);
    sOptionMgr.applyEtc();

    xpr_sint_t i;
    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mCfgDlg != XPR_NULL && sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
            sCfgItem->mCfgDlg->DestroyWindow();

        SendMessage(WM_SETMODIFIED, i, XPR_TRUE);
    }

    mTreeCtrl.SelectItem(mCfgDeque[0]->mTreeItem);
    showCfg(0);

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.default_ok"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

HBRUSH CfgMgrDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor)
{
    HBRUSH sBrush = super::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd != XPR_NULL)
    {
        switch (pWnd->GetDlgCtrlID())
        {
        case IDC_CFG_DESC:
            {
                pDC->SetTextColor(RGB(255,255,255));
                pDC->SetBkMode(TRANSPARENT);
                return mDarkGrayBrush;
            }
        }
    }

    return sBrush;
}
