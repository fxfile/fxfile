//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "drive_dlg.h"

#include "context_menu.h"
#include "img_list_manager.h"
#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#include <Imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
#define ID_DRIVE_REFRESH 100

DriveDlg::DriveDlg(void)
    : super(IDD_DRIVE, XPR_NULL)
    , mDrive(0)
    , mDlgState(XPR_NULL)
{
}

void DriveDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DRIVE_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(DriveDlg, super)
    ON_WM_SIZE()
    ON_COMMAND(ID_DRIVE_REFRESH, OnRefresh)
    ON_NOTIFY(NM_DBLCLK, IDC_DRIVE_TREE, OnDblclkDrvTree)
    ON_NOTIFY(NM_RCLICK, IDC_DRIVE_TREE, OnRclickDrvTree)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t DriveDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -----------------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_DRIVE_TREE, sizeResize, sizeResize);
    //----------------------------------------------------------------------

    if (mToolBar.Create(this) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create toolbar\n"));
        return -1;
    }

    mImgList.Create(IDB_TB_DRIVEDLG, 16, 1, RGB(255,0,255));
    mToolBar.GetToolBarCtrl().SetImageList(&mImgList);
    mToolBar.GetToolBarCtrl().SetBitmapSize(CSize(16,16));

    mToolBar.ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
    mToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

    mToolBar.SetBorders(0, 0, 0, 0);

    DWORD sStyle = mToolBar.GetBarStyle();
    sStyle &= ~CBRS_BORDER_TOP;
    sStyle |= CBRS_FLYBY;
    mToolBar.SetBarStyle(sStyle);

    //----------------------------------------------------------------------

    TBBUTTON sTbButton = {0};
    sTbButton.idCommand = ID_DRIVE_REFRESH;
    sTbButton.iBitmap = 0;
    sTbButton.fsState = TBSTATE_ENABLED;
    sTbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
    sTbButton.iString = mToolBar.GetToolBarCtrl().AddStrings(gApp.loadString(XPR_STRING_LITERAL("popup.drive.toolbar.refresh")));

    mToolBar.GetToolBarCtrl().AddButtons(1, &sTbButton);

    //----------------------------------------------------------------------
    // ControlBar Reposition

    CRect sClientStartRect;
    CRect sClientNowRect;
    GetClientRect(sClientStartRect);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, sClientNowRect);

    CPoint sOffset(sClientNowRect.left - sClientStartRect.left, sClientNowRect.top - sClientStartRect.top); 
    CRect sChildRect;
    CWnd *sChildWnd = GetWindow(GW_CHILD);
    while (sChildWnd != XPR_NULL)
    {
        sChildWnd->GetWindowRect(sChildRect);
        ScreenToClient(sChildRect);
        sChildRect.OffsetRect(sOffset);
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

    CRect sToolBarRect, sTreeRect;
    mToolBar.GetWindowRect(&sToolBarRect);
    mTreeCtrl.GetWindowRect(&sTreeRect);
    ScreenToClient(&sTreeRect);
    sTreeRect.top = sToolBarRect.Height();
    mTreeCtrl.MoveWindow(sTreeRect);

    mTreeCtrl.SetImageList(&SingletonManager::get<ImgListManager>().mSysImgList16, TVSIL_NORMAL);

    OnRefresh();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.drive.title")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("Drive"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void DriveDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy) 
{
    super::OnSize(nType, cx, cy);

    if (mToolBar.m_hWnd != XPR_NULL)
    {
        CRect sToolBarRect;
        mToolBar.GetWindowRect(&sToolBarRect);
        mToolBar.MoveWindow(0, 0, cx, sToolBarRect.Height());
    }
}

void DriveDlg::OnRefresh(void)
{
    xpr_tchar_t sDrive = 0;
    HTREEITEM sSelTreeItem = mTreeCtrl.GetSelectedItem();
    if (sSelTreeItem != XPR_NULL)
        sDrive = getDrive(sSelTreeItem);

    mTreeCtrl.SetRedraw(XPR_FALSE);

    mTreeCtrl.DeleteAllItems();

    xpr_tchar_t sDrivePath[XPR_MAX_PATH + 1] = {0};
    GetDriveStrings(sDrivePath);

    xpr_tchar_t *sCurDrive = sDrivePath;
    while (true)
    {
        if (*sCurDrive == 0)
            break;

        _tcsupr(sCurDrive);
        addDrive(sCurDrive);

        sCurDrive += _tcslen(sCurDrive) + 1;
    }

    mTreeCtrl.SetRedraw();

    sSelTreeItem = getDriveItem(sDrive);
    if (sSelTreeItem != XPR_NULL)
    {
        mTreeCtrl.SelectItem(sSelTreeItem);
    }
}

void DriveDlg::addDrive(xpr_tchar_t *aDrivePath)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;
    HRESULT sResult = fxfile::base::Pidl::create(aDrivePath, sFullPidl);
    if (FAILED(sResult))
        return;

    xpr_sint_t sImage = GetItemIconIndex(sFullPidl, XPR_FALSE);

    xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
    _stprintf(sName, XPR_STRING_LITERAL("[-%c-] "), aDrivePath[0]);
    GetName(sFullPidl, SHGDN_INFOLDER, sName+_tcslen(sName));

    HTREEITEM sTreeItem = mTreeCtrl.InsertItem(sName, sImage, sImage);

    mTreeCtrl.SetItemData(sTreeItem, (DWORD_PTR)(*aDrivePath));

    COM_FREE(sFullPidl);
}

HTREEITEM DriveDlg::getDriveItem(xpr_tchar_t aDrive)
{
    HTREEITEM sTreeItem = mTreeCtrl.GetRootItem();
    while (sTreeItem != XPR_NULL)
    {
        if (getDrive(sTreeItem) == aDrive)
            return sTreeItem;

        sTreeItem = mTreeCtrl.GetNextSiblingItem(sTreeItem);
    }

    return XPR_NULL;
}

xpr_tchar_t DriveDlg::getDrive(HTREEITEM aTreeItem)
{
    if (aTreeItem == XPR_NULL)
        return 0;

    return (xpr_tchar_t)mTreeCtrl.GetItemData(aTreeItem);
}

xpr_bool_t DriveDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->hwnd == mTreeCtrl.m_hWnd && pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_F5)
        {
            OnRefresh();
            return XPR_TRUE;
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            OnOK();
            return XPR_TRUE;
        }

        xpr_tchar_t sDrive = (xpr_tchar_t)pMsg->wParam;
        if (pMsg->wParam == VK_PROCESSKEY)
            sDrive = ::ImmGetVirtualKey(m_hWnd);

        xpr_tchar_t sDrivePath[2] = {0};
        sDrivePath[0] = sDrive;
        sDrivePath[1] = '\0';
        _tcsupr(sDrivePath);

        if (XPR_IS_RANGE('A', sDrivePath[0], 'Z'))
        {
            if (sDrivePath[0] < 'A')
                sDrivePath[0] = 'A' + sDrive - 1;

            xpr::string sText;
            HTREEITEM sTreeItem = mTreeCtrl.GetRootItem();
            while (sTreeItem != XPR_NULL)
            {
                sText = mTreeCtrl.GetItemText(sTreeItem);
                if (sText[2] == sDrivePath[0])
                {
                    mTreeCtrl.SelectItem(sTreeItem);
                    OnOK();
                    break;
                }

                sTreeItem = mTreeCtrl.GetNextSiblingItem(sTreeItem);
            }

            return XPR_TRUE;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void DriveDlg::OnRclickDrvTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    CPoint sCursorPos;
    GetCursorPos(&sCursorPos);

    CPoint sHitTest(sCursorPos);
    mTreeCtrl.ScreenToClient(&sHitTest);

    HTREEITEM sTreeItem;
    sTreeItem = mTreeCtrl.HitTest(sHitTest);

    xpr_tchar_t sDrive = getDrive(sTreeItem);

    xpr_tchar_t sDrivePath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sDrivePath, XPR_STRING_LITERAL("%c:\\"), sDrive);

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrivePath);
    if (sFullPidl != XPR_NULL)
    {
        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult) && sShellFolder != XPR_NULL && sPidl != XPR_NULL)
        {
            xpr_uint_t sFlags = TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON;
            ContextMenu::trackItemMenu(sShellFolder, &sPidl, 1, &sCursorPos, sFlags, m_hWnd, CMF_EXPLORE);
        }

        COM_RELEASE(sShellFolder);
        COM_FREE(sFullPidl);
    }
}

void DriveDlg::OnDblclkDrvTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnOK();

    *pResult = 0;
}

void DriveDlg::OnOK(void)
{
    HTREEITEM sTreeItem = mTreeCtrl.GetSelectedItem();
    if (sTreeItem != XPR_NULL)
    {
        mDrive = getDrive(sTreeItem);

        CDialog::OnOK();
    }
}

void DriveDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}
} // namespace cmd
} // namespace fxfile
