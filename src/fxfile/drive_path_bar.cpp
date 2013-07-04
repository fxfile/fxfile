//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "drive_path_bar.h"

#include "gui/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
DrivePathBar::DrivePathBar(void)
{
}

DrivePathBar::~DrivePathBar(void)
{
}

BEGIN_MESSAGE_MAP(DrivePathBar, DriveToolBar)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

xpr_sint_t DrivePathBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (DriveToolBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyle(TBSTYLE_TRANSPARENT, TBSTYLE_TOOLTIPS);

    return 0;
}

void DrivePathBar::getToolBarSize(xpr_sint_t *aWidth, xpr_sint_t *aHeight)
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
        if (sToolBarCtrl.GetItemRect(i, sItemRect))
        {
            cxIdeal += sItemRect.Width();
            cyChild = max(cyChild, sItemRect.Height());
        }
    }

    if (XPR_IS_NOT_NULL(aWidth )) *aWidth  = cxIdeal;
    if (XPR_IS_NOT_NULL(aHeight)) *aHeight = cyChild;
}

xpr_bool_t DrivePathBar::OnEraseBkgnd(CDC* pDC)
{
    return CToolBar::OnEraseBkgnd(pDC);
}

void DrivePathBar::OnPaint()
{
    CToolBar::Default();
}
} // namespace fxfile
