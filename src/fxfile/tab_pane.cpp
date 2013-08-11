//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "tab_pane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
TabPane::TabPane(TabType aTabType)
    : mObserver(XPR_NULL)
    , mViewIndex(-1)
    , mTabType(aTabType)
{
}

TabPane::~TabPane(void)
{
}

xpr_bool_t TabPane::registerWindowClass(const xpr_tchar_t *aClassName)
{
    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    //Check weather the class is registerd already
    if (::GetClassInfo(sInstance, aClassName, &sWndClass) == XPR_FALSE)
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
        sWndClass.lpszClassName = aClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

xpr_sint_t TabPane::getViewIndex(void) const
{
    return mViewIndex;
}

void TabPane::setViewIndex(xpr_sint_t aViewIndex)
{
    mViewIndex = aViewIndex;
}

void TabPane::setObserver(TabPaneObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_bool_t TabPane::isSharedPane(void) const
{
    return XPR_FALSE;
}

xpr_bool_t TabPane::canDuplicatePane(void) const
{
    return XPR_FALSE;
}

TabType TabPane::getTabType(void) const
{
    return mTabType;
}

xpr_bool_t TabPane::isTabType(TabType aTabType) const
{
    return (aTabType == mTabType) ? XPR_TRUE : XPR_FALSE;
}
} // namespace fxfile
