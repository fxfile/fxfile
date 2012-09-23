//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_MAIN_MENU_BAR_H__
#define __FX_MAIN_MENU_BAR_H__
#pragma once

#include "rgc/MenuBar.h"

class MainMenuBar : public CMenuBar
{
public:
    MainMenuBar(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
};

#endif // __FX_MAIN_MENU_BAR_H__
