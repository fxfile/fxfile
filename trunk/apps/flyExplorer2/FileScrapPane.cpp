//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FileScrapPane.h"

#include "FileScrapPaneObserver.h"
#include "FileScrapCtrl.h"
#include "FileScrapToolBar.h"
#include "MainFrame.h"

#include "fxb/fxb_sys_img_list.h"
#include "fxb/fxb_size_format.h"

#include "cmd/InputDlg.h"
#include "command_string_table.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("FileScrapPane");
static const xpr_size_t  kPaneWidth[] = { 150, 90 };

//
// control id
//
enum
{
    CTRL_ID_FILE_SCRAP = 50,
    CTRL_ID_GROUP_COMBOBOX,
    CTRL_ID_TOOLBAR,
    CTRL_ID_STATUS_BAR,
};

//
// pane id
//
enum
{
    kPaneIdNormal = 10,
    kPaneIdSize,
};

FileScrapPane::FileScrapPane(void)
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mFileScrapCtrl(XPR_NULL), mGroupComboBox(XPR_NULL), mToolBar(XPR_NULL), mStatusBar(XPR_NULL)
    , mGroupLabelText(XPR_NULL), mGroupLabelRect(CRect(0,0,0,0))
{
    registerWindowClass();
}

FileScrapPane::~FileScrapPane(void)
{
}

xpr_bool_t FileScrapPane::registerWindowClass(void)
{
    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    //Check weather the class is registerd already
    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        //If not then we have to register the new class
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = kClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t FileScrapPane::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(FileScrapPane, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_WM_SETTINGCHANGE()
    ON_WM_INITMENUPOPUP()
    ON_WM_MENUCHAR()
    ON_WM_MEASUREITEM()
    ON_NOTIFY(TBN_DROPDOWN, CTRL_ID_TOOLBAR, OnToolbarDropDown)
    ON_CBN_SELCHANGE(CTRL_ID_GROUP_COMBOBOX, OnCbnSelchangeGroup)
END_MESSAGE_MAP()

xpr_sint_t FileScrapPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    createFont();

    mFileScrapCtrl = new FileScrapCtrl;
    if (XPR_IS_NOT_NULL(mFileScrapCtrl))
    {
        mFileScrapCtrl->setObserver(dynamic_cast<FileScrapCtrlObserver *>(this));

        if (mFileScrapCtrl->Create(this, CTRL_ID_FILE_SCRAP, CRect(0, 0, 0, 0)) == XPR_TRUE)
        {
            fxb::SysImgListMgr &sSysImgListMgr = fxb::SysImgListMgr::instance();
            mFileScrapCtrl->SetImageList(&sSysImgListMgr.mSysImgList16, LVSIL_SMALL);
            mFileScrapCtrl->SetImageList(&sSysImgListMgr.mSysImgList32, LVSIL_NORMAL);
        }
        else
        {
            XPR_SAFE_DELETE(mFileScrapCtrl);
        }
    }

    mGroupComboBox = new CComboBoxEx;
    if (XPR_IS_NOT_NULL(mGroupComboBox))
    {
        DWORD sStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBS_DROPDOWNLIST;
        if (mGroupComboBox->Create(sStyle, CRect(0,0,0,100), this, CTRL_ID_GROUP_COMBOBOX) == XPR_TRUE)
        {
            mGroupImgList.Create(16, 16, ILC_COLOR16 | ILC_MASK, -1, -1);
            HICON sIcon = theApp.LoadIcon(MAKEINTRESOURCE(IDI_FILE_SCRAP));
            mGroupImgList.Add(sIcon);
            ::DestroyIcon(sIcon);

            mGroupComboBox->SetImageList(&mGroupImgList);
        }
        else
        {
            XPR_SAFE_DELETE(mGroupComboBox);
        }
    }

    mToolBar = new FileScrapToolBar;
    if (XPR_IS_NOT_NULL(mToolBar))
    {
        DWORD sStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_FLYBY;
        DWORD sExStyle = TBSTYLE_EX_DRAWDDARROWS;
        if (mToolBar->Create(this, sStyle, CTRL_ID_TOOLBAR, sExStyle) == XPR_TRUE)
        {
            mToolBar->Init();
            mToolBar->UpdateToolbarSize();
            mToolBar->SetDefaultButtons();
        }
        else
        {
            XPR_SAFE_DELETE(mToolBar);
        }
    }

    mStatusBar = new StatusBar;
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        if (mStatusBar->Create(this, CTRL_ID_STATUS_BAR, CRect(0, 0, 0, 0)) == XPR_TRUE)
        {
            mStatusBar->addPane(kPaneIdNormal, XPR_NULL);
            mStatusBar->addPane(kPaneIdSize,   XPR_NULL);

            mStatusBar->setPaneSize(0, kPaneWidth[0], kPaneWidth[0]);
            mStatusBar->setPaneSize(1, kPaneWidth[1], kPaneWidth[1]);
        }
        else
        {
            XPR_SAFE_DELETE(mStatusBar);
        }
    }

    if (XPR_IS_NULL(mFileScrapCtrl) || XPR_IS_NULL(mGroupComboBox) || XPR_IS_NULL(mToolBar) || XPR_IS_NULL(mStatusBar))
    {
        DESTROY_DELETE(mFileScrapCtrl);
        DESTROY_DELETE(mGroupComboBox);
        DESTROY_DELETE(mToolBar);
        DESTROY_DELETE(mStatusBar);

        return -1;
    }

    mGroupLabelText = theApp.loadString(XPR_STRING_LITERAL("file_scrap.label.group"));

    updateGroup();

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sFileScrap.setObserver(dynamic_cast<FileScrapObserver *>(this));

    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onCreated(*this);

    return 0;
}

void FileScrapPane::OnDestroy(void)
{
    if (XPR_IS_NOT_NULL(mObserver))
        mObserver->onDestroyed(*this);

    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();
    sFileScrap.setObserver(XPR_NULL);

    DESTROY_DELETE(mFileScrapCtrl);
    DESTROY_DELETE(mGroupComboBox);
    DESTROY_DELETE(mToolBar);
    DESTROY_DELETE(mStatusBar);

    destroyFont();

    super::OnDestroy();
}

void FileScrapPane::setObserver(FileScrapPaneObserver *aObserver)
{
    mObserver = aObserver;
}

void FileScrapPane::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

xpr_sint_t FileScrapPane::getViewIndex(void) const
{
    return mViewIndex;
}

void FileScrapPane::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void FileScrapPane::recalcLayout(void)
{
    CRect sRect;
    GetClientRect(&sRect);

    HDWP sHdwp = ::BeginDeferWindowPos(4);

    xpr_sint_t sLeft = 0;
    xpr_sint_t sHeight = 0;
    xpr_sint_t sComboBoxHeight = 0;
    xpr_sint_t sToolBarWidth = 0;
    xpr_sint_t sToolBarHeight = 0;

    if (XPR_IS_NOT_NULL(mToolBar))
    {
        mToolBar->getToolBarSize(&sToolBarWidth, &sToolBarHeight);

        sHeight = max(sHeight, sToolBarHeight);
    }

    if (XPR_IS_NOT_NULL(mGroupComboBox))
    {
        CRect sGroupComboBoxRect(sRect);
        mGroupComboBox->GetWindowRect(&sGroupComboBoxRect);
        ScreenToClient(&sGroupComboBoxRect);

        sComboBoxHeight = sGroupComboBoxRect.Height();

        sHeight = max(sHeight, sComboBoxHeight);
    }

    mGroupLabelRect = sRect;
    mGroupLabelRect.left   = 5;
    mGroupLabelRect.bottom = mGroupLabelRect.top + sHeight;
    mGroupLabelRect.right  = mGroupLabelRect.left + 200;

    if (XPR_IS_NOT_NULL(mGroupLabelText))
    {
        CRect sGroupLabelCalcRect(mGroupLabelRect);

        CClientDC sClientDC(this);

        CFont *sOldFont = sClientDC.SelectObject(&mFont);

        sClientDC.DrawText(mGroupLabelText, &sGroupLabelCalcRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);

        sClientDC.SelectObject(sOldFont);

        mGroupLabelRect.right = sGroupLabelCalcRect.right + 5;
    }
    else
    {
        mGroupLabelRect.right = mGroupLabelRect.left + 70;
    }

    sLeft += mGroupLabelRect.right;

    if (XPR_IS_NOT_NULL(mGroupComboBox))
    {
        CRect sGroupComboBoxRect(sRect);
        sGroupComboBoxRect.left   = sLeft;
        sGroupComboBoxRect.right  = sGroupComboBoxRect.left + 200;
        sGroupComboBoxRect.top    = (sHeight - sComboBoxHeight) / 2;
        sGroupComboBoxRect.bottom = sGroupComboBoxRect.top + sComboBoxHeight;

        ::DeferWindowPos(sHdwp, *mGroupComboBox, XPR_NULL, sGroupComboBoxRect.left, sGroupComboBoxRect.top, sGroupComboBoxRect.Width(), sGroupComboBoxRect.Height(), SWP_NOZORDER);

        sLeft = sGroupComboBoxRect.right + 5;
    }

    if (XPR_IS_NOT_NULL(mToolBar))
    {
        CRect sToolBarRect(sRect);
        sToolBarRect.left   = sLeft;
        sToolBarRect.bottom = sToolBarRect.top + sHeight;

        ::DeferWindowPos(sHdwp, *mToolBar, XPR_NULL, sToolBarRect.left, sToolBarRect.top, sToolBarRect.Width(), sToolBarRect.Height(), SWP_NOZORDER);

        sRect.top = sToolBarRect.bottom;
    }

    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        CRect sStatusBarRect(sRect);
        sStatusBarRect.top = sStatusBarRect.bottom - mStatusBar->getDefaultHeight();

        ::DeferWindowPos(sHdwp, *mStatusBar, XPR_NULL, sStatusBarRect.left, sStatusBarRect.top, sStatusBarRect.Width(), sStatusBarRect.Height(), SWP_NOZORDER);

        sRect.bottom -= sStatusBarRect.Height();
    }

    if (XPR_IS_NOT_NULL(mFileScrapCtrl))
    {
        ::DeferWindowPos(sHdwp, *mFileScrapCtrl, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
    }

    ::EndDeferWindowPos(sHdwp);
}

void FileScrapPane::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mFileScrapCtrl))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->onSetFocus(*this);

        mFileScrapCtrl->SetFocus();
    }
}

LRESULT FileScrapPane::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu)
{
    LRESULT sResult;

    if (BCMenu::IsMenu(aMenu) == XPR_TRUE)
        sResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sResult = super::OnMenuChar(aChar, aFlags, aMenu);

    return sResult;
}

void FileScrapPane::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    //super::OnInitMenuPopup(aPopupMenu, aIndex, aSysMenu);
    ASSERT(aPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = aPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = sBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if sId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if sId(xpr_uint_t) is -1, it's sub-menu.
                    sBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

                        sBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = aPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == aPopupMenu->m_hMenu)
        sState.m_pParentMenu = aPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL && (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t sIndex;
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (sIndex = 0; sIndex < sIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == aPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = aPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = aPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = aPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL || (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 || sState.m_nID == (xpr_uint_t)-1)
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
        xpr_uint_t sCount = aPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount && aPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void FileScrapPane::OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct)
{
    xpr_bool_t sSetFlag = XPR_FALSE;

    if (aMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)aMeasureItemStruct->itemID) == XPR_TRUE)
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)aMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu) == XPR_TRUE)
            {
                mMenu.MeasureItem(aMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sSetFlag))
        super::OnMeasureItem(aIdCtl, aMeasureItemStruct);
}

xpr_bool_t FileScrapPane::OnCmdMsg(xpr_uint_t aId, xpr_sint_t aCode, void *aExtra, AFX_CMDHANDLERINFO *aHandlerInfo)
{
    if (aHandlerInfo == XPR_NULL)
    {
        xpr_sint_t sMsg = 0;
        xpr_sint_t sCode = aCode;

        if (sCode != CN_UPDATE_COMMAND_UI)
        {
            sMsg  = HIWORD(aCode);
            sCode = LOWORD(aCode);
        }

        if (sMsg == 0)
            sMsg = WM_COMMAND;

        if (sCode == CN_UPDATE_COMMAND_UI)
        {
            CCmdUI *sCmdUI = (CCmdUI *)aExtra;

            if (gFrame->canExecute(aId, sCmdUI, this) == XPR_TRUE)
            {
                return XPR_TRUE;
            }
        }
        else if (sCode == CN_COMMAND)
        {
            if (gFrame->executeCommand(aId, this) == XPR_TRUE)
            {
                return XPR_TRUE;
            }
        }
    }

    return super::OnCmdMsg(aId, aCode, aExtra, aHandlerInfo);
}

void FileScrapPane::OnToolbarDropDown(NMHDR *aNmHdr, LRESULT *aLResult)
{
    NMTOOLBAR *sNmToolBar = (NMTOOLBAR *)aNmHdr;
    CWnd *sWnd = CWnd::FromHandle(sNmToolBar->hdr.hwndFrom);

    xpr_uint_t sMenuId = 0;
    switch (sNmToolBar->iItem)
    {
    case ID_EDIT_FILE_SCRAP_GROUP_ADD: sMenuId = IDR_FILE_SCRAP; break;

    default:
        return;
    }

    CRect sRect;
    sWnd->SendMessage(TB_GETRECT, sNmToolBar->iItem, (LPARAM)&sRect);
    sWnd->ClientToScreen(&sRect);

    BCMenu sMenu;
    if (sMenu.LoadMenu(sMenuId) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        ASSERT(sPopupMenu);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sRect.left, sRect.bottom, this, &sRect);
        }
    }
}

void FileScrapPane::createFont(void)
{
    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(sNonClientMetrics), &sNonClientMetrics, 0);

    LOGFONT sLogFont = sNonClientMetrics.lfMenuFont;

    mFont.CreateFontIndirect(&sLogFont);
    SetFont(&mFont, XPR_TRUE);
}

void FileScrapPane::recreateFont(void)
{
    destroyFont();
    createFont();
}

void FileScrapPane::destroyFont(void)
{
    if (XPR_IS_NOT_NULL(mFont.m_hObject))
        mFont.DeleteObject();
}

void FileScrapPane::OnPaint(void)
{
    CPaintDC sPaintDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    COLORREF sTextColor = ::GetSysColor(COLOR_BTNTEXT);
    COLORREF sBkgndColor = ::GetSysColor(COLOR_BTNFACE);

    CDC sMemDC;
    sMemDC.CreateCompatibleDC(&sPaintDC);
    sMemDC.SetBkMode(TRANSPARENT);
    sMemDC.SetTextColor(sTextColor);

    CBitmap sBitmap;
    sBitmap.CreateCompatibleBitmap(&sPaintDC, sClientRect.Width(), sClientRect.Height());

    CBitmap *sOldBitmap = sMemDC.SelectObject(&sBitmap);

    sMemDC.FillSolidRect(&sClientRect, sBkgndColor);

    CFont *sOldFont = sMemDC.SelectObject(&mFont);

    if (XPR_IS_NOT_NULL(mGroupLabelText))
    {
        sMemDC.DrawText(mGroupLabelText, &mGroupLabelRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    }

    sMemDC.SelectObject(sOldFont);

    sPaintDC.BitBlt(0, 0, sClientRect.Width(), sClientRect.Height(), &sMemDC, 0, 0, SRCCOPY);

    sMemDC.SelectObject(sOldBitmap);
}

void FileScrapPane::OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection)
{
    recreateFont();

    Invalidate();

    super::OnSettingChange(aFlags, aSection);
}

FileScrapCtrl *FileScrapPane::getFileScrapCtrl(void) const
{
    return mFileScrapCtrl;
}

void FileScrapPane::onSetFocus(FileScrapCtrl &aFileScrapCtrl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSetFocus(*this);
    }
}

xpr_bool_t FileScrapPane::onExplore(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onExplore(*this, aDir, aSelPath);
    }

    return XPR_FALSE;
}

xpr_bool_t FileScrapPane::onExplore(FileScrapCtrl &aFileScrapCtrl, LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        return mObserver->onExplore(*this, aFullPidl);
    }

    return XPR_FALSE;
}

void FileScrapPane::onUpdateStatus(FileScrapCtrl &aFileScrapCtrl)
{
    updateStatus();
}

void FileScrapPane::updateStatus(void)
{
    if (XPR_IS_NOT_NULL(mStatusBar))
    {
        xpr_sint_t sFileCount  = 0;
        xpr_sint_t sDirCount   = 0;
        xpr_sint_t sTotalCount = 0;
        mFileScrapCtrl->getStatus(sFileCount, sDirCount);
        sTotalCount = sFileCount + sDirCount;

        xpr_tchar_t sFileCountText[0xff] = {0};
        xpr_tchar_t sDirCountText [0xff] = {0};
        xpr_tchar_t sTotalCountText[0xff] = {0};
        fxb::GetFormatedNumber(sFileCount,  sFileCountText,  XPR_COUNT_OF(sFileCountText)  - 1);
        fxb::GetFormatedNumber(sDirCount,   sDirCountText,   XPR_COUNT_OF(sDirCountText)   - 1);
        fxb::GetFormatedNumber(sTotalCount, sTotalCountText, XPR_COUNT_OF(sTotalCountText) - 1);

        xpr_tchar_t sStatusText[0xff] = {0};

        _stprintf(
            sStatusText,
            theApp.loadFormatString(XPR_STRING_LITERAL("file_scrap.status.count"),
            XPR_STRING_LITERAL("%s,%s,%s")),
            sTotalCountText,
            sDirCountText,
            sFileCountText);

        mStatusBar->setPaneText(0, sStatusText);
    }
}

void FileScrapPane::updateGroup(void)
{
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    xpr_sint_t sCurSel = 0;
    fxb::FileScrap::Group *sGroup;

    sCount = sFileScrap.getGroupCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = sFileScrap.getGroup(i);
        if (XPR_IS_NULL(sGroup))
            continue;

        addGroup(sGroup);

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
            sCurSel = (xpr_sint_t)i;
    }

    updateDefGroup();

    mGroupComboBox->SetCurSel(sCurSel);
    OnCbnSelchangeGroup();
}

xpr_sint_t FileScrapPane::addGroup(fxb::FileScrap::Group *aGroup, xpr_sint_t aIndex)
{
    if (XPR_IS_NULL(aGroup))
        return -1;

    std::tstring sGroupName(aGroup->mGroupName);
    if (sGroupName == fxb::kFileScrapNoGroupName)
        sGroupName = theApp.loadString(XPR_STRING_LITERAL("file_scrap.group.no_group"));

    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask           = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_LPARAM;
    sComboBoxExItem.iItem          = (aIndex == -1) ? mGroupComboBox->GetCount() : aIndex;
    sComboBoxExItem.iImage         = 0;
    sComboBoxExItem.iSelectedImage = 0;
    sComboBoxExItem.pszText        = (xpr_tchar_t *)sGroupName.c_str();
    sComboBoxExItem.lParam         = (LPARAM)aGroup;

    return mGroupComboBox->InsertItem(&sComboBoxExItem);
}

void FileScrapPane::OnCbnSelchangeGroup(void)
{
    fxb::FileScrap::Group *sGroup = getCurGroup();
    if (XPR_IS_NULL(sGroup))
        return;

    mFileScrapCtrl->clearList();
    mFileScrapCtrl->initList(sGroup);
}

void FileScrapPane::refresh(void)
{
    mFileScrapCtrl->refresh();
}

void FileScrapPane::onAddGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    xpr_sint_t sIndex = addGroup(aGroup);
    if (sIndex != CB_ERR)
    {
        mGroupComboBox->SetCurSel(sIndex);
        OnCbnSelchangeGroup();
    }
}

void FileScrapPane::onRenameGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask    = CBEIF_TEXT;
    sComboBoxExItem.iItem   = mGroupComboBox->GetCurSel();
    sComboBoxExItem.pszText = (xpr_tchar_t *)aGroup->mGroupName.c_str();
    mGroupComboBox->SetItem(&sComboBoxExItem);

    mGroupComboBox->RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);

    updateDefGroup();
}

void FileScrapPane::onRemoveGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    xpr_sint_t sDeleteItem = -1;

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FileScrap::Group *sGroup;

    sCount = mGroupComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = (fxb::FileScrap::Group *)mGroupComboBox->GetItemData(i);
        if (XPR_IS_NULL(sGroup))
            continue;

        if (aGroup->mGroupId == sGroup->mGroupId)
        {
            sDeleteItem = i;
            break;
        }
    }

    if (sDeleteItem < 0)
        return;

    xpr_bool_t sIsCurSel = (sDeleteItem == mGroupComboBox->GetCurSel()) ? XPR_TRUE : XPR_FALSE;

    mGroupComboBox->DeleteItem(sDeleteItem);

    sCount = mGroupComboBox->GetCount();
    if (sCount == 0)
    {
        mFileScrapCtrl->DeleteAllItems();
        mGroupComboBox->SetCurSel(-1);
        updateStatus();
        return;
    }

    if (XPR_IS_TRUE(sIsCurSel))
    {
        xpr_sint_t sCurSel = -1;

        for (i = 0; i < sCount; ++i)
        {
            sGroup = (fxb::FileScrap::Group *)mGroupComboBox->GetItemData(i);
            if (XPR_IS_NULL(sGroup))
                continue;

            if (sGroup->mGroupId == aFileScrap.getCurGroupId())
            {
                sCurSel = i;
                break;
            }
        }

        if (sCurSel >= sCount)
            sCurSel--;

        mGroupComboBox->SetCurSel(sCurSel);
        OnCbnSelchangeGroup();

        updateDefGroup();
    }
}

void FileScrapPane::onRemoveAllGroups(fxb::FileScrap &aFileScrap)
{
}

fxb::FileScrap::Group *FileScrapPane::getCurGroup(void) const
{
    xpr_sint_t sIndex = mGroupComboBox->GetCurSel();
    if (sIndex == CB_ERR)
        return XPR_NULL;

    return (fxb::FileScrap::Group *)mGroupComboBox->GetItemData(sIndex);
}

xpr_bool_t FileScrapPane::isCurGroup(fxb::FileScrap::Group *aGroup)
{
    if (XPR_IS_NULL(aGroup))
        return XPR_FALSE;

    fxb::FileScrap::Group *sCurGroup = getCurGroup();
    if (XPR_IS_NULL(sCurGroup))
        return XPR_FALSE;

    if (sCurGroup->mGroupId == aGroup->mGroupId)
        return XPR_TRUE;

    return XPR_FALSE;
}

void FileScrapPane::updateDefGroup(void)
{
    fxb::FileScrap &sFileScrap = fxb::FileScrap::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    std::tstring sGroupName;
    fxb::FileScrap::Group *sGroup;

    sCount = mGroupComboBox->GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sGroup = (fxb::FileScrap::Group *)mGroupComboBox->GetItemData((xpr_sint_t)i);
        if (XPR_IS_NULL(sGroup))
            continue;

        sGroupName = sGroup->mGroupName;
        if (sGroupName == fxb::kFileScrapNoGroupName)
            sGroupName = theApp.loadString(XPR_STRING_LITERAL("file_scrap.group.no_group"));

        if (sGroup->mGroupId == sFileScrap.getCurGroupId())
            sGroupName += theApp.loadString(XPR_STRING_LITERAL("file_scrap.group.default"));

        COMBOBOXEXITEM sComboBoxExItem = {0};
        sComboBoxExItem.mask    = CBEIF_TEXT;
        sComboBoxExItem.iItem   = i;
        sComboBoxExItem.pszText = (xpr_tchar_t *)sGroupName.c_str();
        mGroupComboBox->SetItem(&sComboBoxExItem);
    }

    mGroupComboBox->RedrawWindow(XPR_NULL, XPR_NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}

void FileScrapPane::onSetCurGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    updateDefGroup();
}

void FileScrapPane::onUpdateList(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    xpr_bool_t sIsCurGroup = isCurGroup(aGroup);
    if (XPR_IS_TRUE(sIsCurGroup))
    {
        mFileScrapCtrl->clearList();
        mFileScrapCtrl->initList(aGroup);
    }
}

void FileScrapPane::onRemovedItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup, const fxb::FileScrap::IdSet &aIdSet)
{
    if (isCurGroup(aGroup) == XPR_FALSE)
        return;

    mFileScrapCtrl->removeItems(aIdSet);
}

xpr_sint_t FileScrapPane::getItemCount(void) const
{
    return mFileScrapCtrl->GetItemCount();
}

xpr_sint_t FileScrapPane::getSelectedItemCount(void) const
{
    return mFileScrapCtrl->GetSelectedCount();
}

void FileScrapPane::getSelFileScrapItemIdSet(fxb::FileScrap::IdSet &sIdSet)
{
    mFileScrapCtrl->getSelFileScrapItemIdSet(sIdSet);
}

void FileScrapPane::onRemovedAllItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup)
{
    if (isCurGroup(aGroup) == XPR_FALSE)
        return;

    mFileScrapCtrl->DeleteAllItems();
}
