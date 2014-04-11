//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "explorer_status_bar.h"

#include "context_menu.h"
#include "size_format.h"
#include "option.h"

#include "gui/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
const xpr_sint_t kPaneWidth[] = { 150, 90, 70, 90 };

//
// control id
//
enum
{
    CTRL_ID_DRIVE_PROGRESS_CTRL = 50,
};
} // namespace anonymous

ExplorerStatusBar::ExplorerStatusBar(void)
    : mDriveIcon(XPR_NULL)
{
}

ExplorerStatusBar::~ExplorerStatusBar(void)
{
    DESTROY_ICON(mDriveIcon);
}

BEGIN_MESSAGE_MAP(ExplorerStatusBar, super)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_PROGRESS_DBLCK, OnProgressDblclk)
END_MESSAGE_MAP()

xpr_sint_t ExplorerStatusBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    addPane(kPaneIdNormal,        XPR_NULL);
    addPane(kPaneIdSize,          XPR_NULL);
    addPane(kPaneIdDrive,         XPR_NULL);
    addPane(kPaneIdDriveProgress, XPR_NULL);

    // First pane is variable. One's pane size is minimum size.
    setPaneSize(0, kPaneWidth[0], kPaneWidth[0]);
    setPaneSize(1, kPaneWidth[1], kPaneWidth[1]);
    setPaneSize(2, kPaneWidth[2], kPaneWidth[2]);
    setPaneSize(3, kPaneWidth[3], kPaneWidth[3]);

    mDriveProgressCtrl.Create(WS_VISIBLE | WS_CHILD | /*WS_BORDER |*/ PBS_SMOOTH, CRect(0,0,0,0), this, CTRL_ID_DRIVE_PROGRESS_CTRL);
    mDriveProgressCtrl.setRange(0, 100);
    mDriveProgressCtrl.setProgressColor(RGB(100,100,100));
    mDriveProgressCtrl.setBackColor(::GetSysColor(COLOR_3DFACE));
    mDriveProgressCtrl.setTextProgressColor(RGB(50,50,50));
    mDriveProgressCtrl.setTextBackColor(RGB(255,255,255));

    recalcLayout();

    return 0;
}

void ExplorerStatusBar::OnDestroy(void)
{
    super::OnDestroy();

    DESTROY_ICON(mDriveIcon);

    mDriveProgressCtrl.DestroyWindow();
}

void ExplorerStatusBar::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void ExplorerStatusBar::recalcLayout(void)
{
    CRect sRect;
    getPaneRect(3, &sRect);

    mDriveProgressCtrl.MoveWindow(sRect);
    mDriveProgressCtrl.Invalidate(XPR_FALSE);
}

void ExplorerStatusBar::setDiskFreeSpace(const xpr_tchar_t *aPath)
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
        LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sRootPath);
        if (XPR_IS_NULL(sFullPidl))
        {
            // default C drive
            _stprintf(sRootPath, XPR_STRING_LITERAL("C:\\"));
            sFullPidl = fxfile::base::Pidl::create(sRootPath);
        }

        if (XPR_IS_NOT_NULL(sFullPidl))
            sResult = GetName(sFullPidl, SHGDN_INFOLDER, sText);

        COM_FREE(sFullPidl);
    }

    if (XPR_IS_FALSE(sResult))
        _tcscpy(sText, sRootPath);

    // Get Root Free Size
    xpr_tchar_t sFreeSizeText[XPR_MAX_PATH + 1];
    xpr_uint64_t sTotalSize = 0, sUsedSize = 0, sFreeSize = 0;
    GetDiskSize(sRootPath, sTotalSize, sUsedSize, sFreeSize);
    SizeFormat::getSizeUnitFormat(sFreeSize, SIZE_UNIT_AUTO, sFreeSizeText, XPR_MAX_PATH);
    _stprintf(sText, XPR_STRING_LITERAL("%s %s"), sText, sFreeSizeText);

    // Get Root Icon
    SHFILEINFO sShFileInfo = {0};
    ::SHGetFileInfo(sRootPath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);

    DESTROY_ICON(mDriveIcon);
    mDriveIcon = sShFileInfo.hIcon;
    setPaneIcon(2, mDriveIcon);

    // Set Status Pane Size, Set Status Pane Text
    setDynamicPaneText(2, sText, 0);

    xpr_sint_t sPercent = sTotalSize > 0ui64 ? (xpr_sint_t)((xpr_double_t)sUsedSize / (xpr_double_t)sTotalSize * 100) : 0;
    mDriveProgressCtrl.setPos(sPercent);

    recalcLayout();
}

LRESULT ExplorerStatusBar::OnProgressDblclk(WPARAM wParam, LPARAM lParam)
{
    showDriveProperties();

    return 0;
}

void ExplorerStatusBar::showDriveProperties(void)
{
    xpr_tchar_t sText[0xff] = {0};
    if (getPaneText(2, sText, 0xfe) == XPR_FALSE)
        return;

    xpr_tchar_t *sColon = _tcsrchr(sText, ':');
    if (XPR_IS_NULL(sColon))
        return;

    --sColon;
    xpr_tchar_t sDriveChar = *sColon;

    xpr_tchar_t sDrive[XPR_MAX_PATH + 1] = {0};
    _stprintf(sDrive, XPR_STRING_LITERAL("%c:\\"), sDriveChar);

    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sDrive);
    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl        = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
        {
            ContextMenu::invokeCommand(sShellFolder, &sPidl, 1, CMID_VERB_PROPERTIES, AfxGetMainWnd()->GetSafeHwnd());
        }

        COM_RELEASE(sShellFolder);
    }

    COM_FREE(sFullPidl);
}
} // namespace fxfile
