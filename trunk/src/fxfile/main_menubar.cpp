//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "main_menubar.h"

#include "main_frame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxfile
{
MainMenuBar::MainMenuBar(void)
{
}

BEGIN_MESSAGE_MAP(MainMenuBar, CMenuBar)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void MainMenuBar::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_COOLBAR_MENUBAR) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
        }
    }
}
} // namespace fxfile
