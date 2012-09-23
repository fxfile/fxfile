//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "StatusBarEx.h"

#include "fxb/fxb_context_menu.h"
#include "fxb/fxb_size_format.h"
#include "rgc/MemDC.h"

#include "Option.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const xpr_sint_t STATUS_PANE_WIDTH[5] = { -1, 90, 70, 70, 120 };

static xpr_uint_t indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
    ID_SEPARATOR,
};

StatusBarEx::StatusBarEx()
{
    mDriveIcon = XPR_NULL;
    mGroupIcon = XPR_NULL;
}

StatusBarEx::~StatusBarEx()
{
    DESTROY_ICON(mDriveIcon);
    DESTROY_ICON(mGroupIcon);
}

BEGIN_MESSAGE_MAP(StatusBarEx, CStatusBar)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_PROGRESS_DBLCK, OnProgressDblclk)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
END_MESSAGE_MAP()

xpr_sint_t StatusBarEx::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CStatusBar::OnCreate(aCreateStruct) == -1)
        return -1;

    mDriveProgressCtrl.Create(WS_VISIBLE | WS_CHILD | /*WS_BORDER |*/ PBS_SMOOTH, CRect(0,0,0,0), this, 200);
    mDriveProgressCtrl.setRange(0, 100);
    mDriveProgressCtrl.setProgressColor(RGB(100,100,100));
    mDriveProgressCtrl.setBackColor(::GetSysColor(COLOR_3DFACE));
    mDriveProgressCtrl.setTextProgressColor(RGB(50,50,50));
    mDriveProgressCtrl.setTextBackColor(RGB(255,255,255));
    recalcLayout();

    if (fxb::UserEnv::instance().mWinVer >= fxb::UserEnv::WinXP)
    {
        CClientDC sClientDC(this);
        CSize sText = sClientDC.GetTextExtent(XPR_STRING_LITERAL("flychk"));
        xpr_sint_t sCyEdge = GetSystemMetrics(SM_CYEDGE);
        GetStatusBarCtrl().SetMinHeight(sText.cy + sCyEdge * 2);
    }

    return 0;
}

xpr_bool_t StatusBarEx::init(void)
{
    if (SetIndicators(indicators, sizeof(indicators)/sizeof(xpr_uint_t)) == XPR_FALSE)
        return XPR_FALSE;

    SetPaneStyle(0, SBPS_NORMAL | SBPS_STRETCH);
    SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL,    STATUS_PANE_WIDTH[1]);
    SetPaneInfo(2, ID_SEPARATOR, SBPS_NOBORDERS, STATUS_PANE_WIDTH[2]);
    SetPaneInfo(3, ID_SEPARATOR, SBPS_NOBORDERS, STATUS_PANE_WIDTH[3]);
    SetPaneInfo(4, ID_SEPARATOR, SBPS_NOBORDERS, STATUS_PANE_WIDTH[4]);

    return XPR_TRUE;
}

void StatusBarEx::OnDestroy(void)
{
    CStatusBar::OnDestroy();

    DESTROY_ICON(mDriveIcon);
    DESTROY_ICON(mGroupIcon);

    mDriveProgressCtrl.DestroyWindow();
}

void StatusBarEx::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    CStatusBar::OnSize(aType, cx, cy);

    recalcLayout();
}

void StatusBarEx::recalcLayout(void)
{
    CRect sRect;
    GetItemRect(3, sRect);
    mDriveProgressCtrl.MoveWindow(sRect);
}

void StatusBarEx::setDiskFreeSpace(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    // Get Root Path
    xpr_tchar_t sRootPath[XPR_MAX_PATH + 1] = {0};
    if (aPath[1] == ':' && aPath[2] == '\\') // Drive
        _stprintf(sRootPath, XPR_STRING_LITERAL("%c:\\"), aPath[0]);
    else if (aPath[0] == '\\' && aPath[1] == '\\' && _tcschr(aPath+2, '\\')) // UNC
    {
        _tcscpy(sRootPath, aPath);

        xpr_tchar_t *sSplit = _tcschr(sRootPath+2, '\\');
        if (XPR_IS_NOT_NULL(sSplit) && (sSplit[1] != ':' || sSplit[2] != ':'))
        {
            sSplit++;
            sSplit = _tcschr(sSplit, '\\');
            if (XPR_IS_NOT_NULL(sSplit))
                *sSplit = '\0';
        }
        else
        {
            *sRootPath = '\0';
        }
    }

    // default C drive
    if (sRootPath[0] == '\0')
        _stprintf(sRootPath, XPR_STRING_LITERAL("C:\\"));

    xpr_bool_t sResult = XPR_FALSE;

    // Get Root Name
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    if (_tcslen(sRootPath) <= 3)
    {
        LPITEMIDLIST sFullPidl = fxb::IL_CreateFromPath(sRootPath);
        if (XPR_IS_NULL(sFullPidl))
        {
            // default C drive
            _stprintf(sRootPath, XPR_STRING_LITERAL("C:\\"));
            sFullPidl = fxb::IL_CreateFromPath(sRootPath);
        }

        if (XPR_IS_NOT_NULL(sFullPidl))
            sResult = fxb::GetName(sFullPidl, SHGDN_INFOLDER, sText);

        COM_FREE(sFullPidl);
    }

    if (XPR_IS_FALSE(sResult))
        _tcscpy(sText, sRootPath);

    // Get Root Free Size
    xpr_tchar_t sFreeSizeText[XPR_MAX_PATH + 1];
    xpr_uint64_t sTotalSize = 0, sUsedSize = 0, sFreeSize = 0;
    fxb::GetDiskSize(sRootPath, sTotalSize, sUsedSize, sFreeSize);
    fxb::SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, sFreeSizeText, XPR_MAX_PATH);
    _stprintf(sText, XPR_STRING_LITERAL("%s %s"), sText, sFreeSizeText);

    // Get Root Icon
    SHFILEINFO sShFileInfo = {0};
    ::SHGetFileInfo(sRootPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

    DESTROY_ICON(mDriveIcon);
    mDriveIcon = sShFileInfo.hIcon;
    GetStatusBarCtrl().SetIcon(2, mDriveIcon);

    // Set Status Pane Size, Set Status Pane Text
    setDynamicPaneText(2, sText, 25);

    xpr_sint_t sPercent = sTotalSize > 0ui64 ? (xpr_sint_t)((xpr_double_t)sUsedSize / (xpr_double_t)sTotalSize * 100) : 0;
    mDriveProgressCtrl.setPos(sPercent);
    recalcLayout();
}

void StatusBarEx::setGroup(HICON aIcon, const xpr_tchar_t *aGroup)
{
    DESTROY_ICON(mGroupIcon);
    mGroupIcon = ::DuplicateIcon(XPR_NULL, aIcon);

    GetStatusBarCtrl().SetIcon(4, mGroupIcon);
    SetPaneText(4, aGroup);
}

void StatusBarEx::OnDblclk(NMHDR *aNmHdr, LRESULT *aResult)
{
    LPNMMOUSE sNmMouse = (LPNMMOUSE)aNmHdr;
    *aResult = 0;

    xpr_sint_t sIndex = (xpr_sint_t)sNmMouse->dwItemSpec;
    if (sIndex == 2 || sIndex == 3)
        showDriveProperty();
}

LRESULT StatusBarEx::OnProgressDblclk(WPARAM wParam, LPARAM lParam)
{
    showDriveProperty();
    return 0;
}

void StatusBarEx::showDriveProperty(void)
{
    xpr_sint_t sIndex = 2;
    CString sText = GetPaneText(sIndex);

    xpr_sint_t sFind = sText.ReverseFind(':');
    if (sFind < 0)
        return;

    xpr_tchar_t sDrive[XPR_MAX_PATH + 1];
    _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), sText[sFind - 1]);

    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPITEMIDLIST sPidl = XPR_NULL;
    LPITEMIDLIST sFullPidl = fxb::IL_CreateFromPath(sDrive);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        HRESULT sHResult = fxb::SH_BindToParent(sFullPidl, IID_IShellFolder, (LPVOID *)&sShellFolder, (LPCITEMIDLIST *)&sPidl);
        if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
            fxb::ContextMenu::invokeCommand(sShellFolder, &sPidl, 1, CMID_VERB_PROPERTIES, AfxGetMainWnd()->GetSafeHwnd());
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);
}

xpr_bool_t StatusBarEx::OnEraseBkgnd(CDC *aDC)
{
    return CStatusBar::OnEraseBkgnd(aDC);
}

void StatusBarEx::setDynamicPaneText(xpr_sint_t aPane, const xpr_tchar_t *aText, xpr_sint_t aOffset)
{
    if (!XPR_IS_RANGE(0, aPane, GetCount()-1) || XPR_IS_NULL(aText))
        return;

    xpr_sint_t sWidth = 0;
    xpr_sint_t sDefWidth = STATUS_PANE_WIDTH[aPane];

    if (aText[0] != XPR_STRING_LITERAL('\0'))
    {
        NONCLIENTMETRICS sNonClientMetrics;
        sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(sNonClientMetrics), &sNonClientMetrics, 0);

        CFont sTextFont;
        sTextFont.CreateFontIndirect(&sNonClientMetrics.lfStatusFont);

        CClientDC sClientDC(this);
        CFont *sOldFont = sClientDC.SelectObject(&sTextFont);

        SIZE sSize = {0};
        ::GetTextExtentPoint32(sClientDC.m_hDC, aText, (xpr_sint_t)_tcslen(aText), &sSize);

        sClientDC.SelectObject(sOldFont);
        sTextFont.DeleteObject();

        sWidth = sSize.cx + aOffset;
    }

    sWidth = max(sWidth, sDefWidth);

    xpr_sint_t sOldWidth;
    xpr_uint_t sId, sStyle;

    GetPaneInfo(aPane, sId, sStyle, sOldWidth);
    SetPaneInfo(aPane, sId, sStyle, sWidth);
    SetPaneText(aPane, aText);
}
