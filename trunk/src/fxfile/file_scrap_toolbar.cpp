//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_scrap_toolbar.h"

#include "resource.h"
#include "command_string_table.h"

#include "gui/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
const COLORREF IL_COLOR_MASK = RGB(255,0,255);

const xpr_byte_t TBSTYLE_1 = TBSTYLE_BUTTON;
const xpr_byte_t TBSTYLE_2 = TBSTYLE_SEP;
const xpr_byte_t TBSTYLE_3 = TBSTYLE_BUTTON | TBSTYLE_DROPDOWN;
const xpr_byte_t TBSTYLE_4 = TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN;

TBBUTTONEX kTbButtons[] = {
    { {  30, ID_EDIT_FILE_SCRAP_GROUP_ADD,       TBSTATE_ENABLED, TBSTYLE_3, 0, 0, 0 }, 1 },
    { {   0, 0,                                  TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { {  35, ID_EDIT_FILE_SCRAP_COPY_TO,         TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  34, ID_EDIT_FILE_SCRAP_MOVE_TO,         TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  19, ID_EDIT_FILE_SCRAP_DELETE,          TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {   0, 0,                                  TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { {   5, ID_EDIT_FILE_SCRAP_REFRESH,         TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {   0, 0,                                  TBSTATE_ENABLED, TBSTYLE_2, 0, 0, 0 }, 1 },
    { {  50, ID_EDIT_FILE_SCRAP_VALIDATE,        TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  49, ID_EDIT_FILE_SCRAP_REMOVE_LIST,     TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
    { {  49, ID_EDIT_FILE_SCRAP_REMOVE_ALL_LIST, TBSTATE_ENABLED, TBSTYLE_1, 0, 0, 0 }, 1 },
};
} // namespace anonymous

FileScrapToolBar::FileScrapToolBar(void)
{
}

FileScrapToolBar::~FileScrapToolBar(void)
{
}

BEGIN_MESSAGE_MAP(FileScrapToolBar, super)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

xpr_sint_t FileScrapToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(TBSTYLE_TRANSPARENT, TBSTYLE_TOOLTIPS);

    return 0;
}

void FileScrapToolBar::Init(void)
{
    SetIconSize(CSize(22, 22), CSize(16, 16));

    SetImageList(
        IDB_TB_MAIN_COLD_SMALL, IDB_TB_MAIN_HOT_SMALL, IDB_TB_MAIN_HOT_SMALL,
        IDB_TB_MAIN_COLD_LARGE, IDB_TB_MAIN_HOT_LARGE, IDB_TB_MAIN_HOT_LARGE,
        TBI_SMALL, IL_COLOR_MASK);

    xpr_sint_t sCount = sizeof(kTbButtons) / sizeof(kTbButtons[0]);
    SetButtons(sCount, kTbButtons);
}

xpr_bool_t FileScrapToolBar::HasButtonText(xpr_uint_t aId)
{
    if (GetTextType() == TBT_RIGHT)
    {
        xpr_bool_t sResult = XPR_FALSE;
        switch (aId)
        {
        case ID_EDIT_FILE_SCRAP_GROUP_ADD:
        case ID_EDIT_FILE_SCRAP_REFRESH:
            sResult = XPR_TRUE;
            break;
        }

        return sResult;
    }

    return XPR_TRUE;
}

xpr_bool_t FileScrapToolBar::HasButtonTip(xpr_uint_t aId)
{
    xpr_bool_t sResult = XPR_TRUE;

    if (GetTextType() == TBT_RIGHT)
    {
        switch (aId)
        {
        case ID_EDIT_FILE_SCRAP_GROUP_ADD:
        case ID_EDIT_FILE_SCRAP_REFRESH:
            sResult = XPR_FALSE;
            break;
        }
    }
    else if (GetTextType() == TBT_TEXT)
        sResult = XPR_FALSE;

    return sResult;
}

void FileScrapToolBar::GetButtonTextByCommand(xpr_uint_t aId, CString &aText)
{
    CommandStringTable &sCommandStringTable = CommandStringTable::instance();

    const xpr_tchar_t *sStringId = sCommandStringTable.loadButtonString(aId);
    ASSERT(sStringId != NULL);

    if (XPR_IS_NULL(sStringId))
        return;

    aText = gApp.loadString(sStringId);
}

void FileScrapToolBar::getToolBarSize(xpr_sint_t *aWidth, xpr_sint_t *aHeight)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();
    sToolBarCtrl.AutoSize();

    CRect sItemRect;
    xpr_sint_t i;
    xpr_sint_t cyChild = 0;
    xpr_sint_t cxIdeal = 0;
    xpr_sint_t sCount = sToolBarCtrl.GetButtonCount();
    for (i = 0; i < sCount; ++i)
    {
        if (sToolBarCtrl.GetItemRect(i, sItemRect) == XPR_TRUE)
        {
            cxIdeal += sItemRect.Width();
            cyChild = max(cyChild, sItemRect.Height());
        }
    }

    if (XPR_IS_NOT_NULL(aWidth )) *aWidth  = cxIdeal;
    if (XPR_IS_NOT_NULL(aHeight)) *aHeight = cyChild;
}

xpr_bool_t FileScrapToolBar::OnEraseBkgnd(CDC* pDC)
{
    return CToolBar::OnEraseBkgnd(pDC);
}

void FileScrapToolBar::OnPaint()
{
    CToolBar::Default();
}
} // namespace fxfile
