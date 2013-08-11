//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DragDropToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(DragDropToolBar, CToolBarEx)

DragDropToolBar::DragDropToolBar(void)
{
}

DragDropToolBar::~DragDropToolBar(void)
{

}

BEGIN_MESSAGE_MAP(DragDropToolBar, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_sint_t DragDropToolBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    mShellIDListClipFormat = ::RegisterClipboardFormat(CFSTR_SHELLIDLIST);

    mDropTarget.registerObserver(this);
    mDropTarget.Register(this);

    return 0;
}

void DragDropToolBar::OnDestroy(void)
{
    mDropTarget.Revoke();

    super::OnDestroy();
}

DROPEFFECT DragDropToolBar::OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    return DROPEFFECT_NONE;
}

void DragDropToolBar::OnDragLeave(void)
{
}

DROPEFFECT DragDropToolBar::OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    return DROPEFFECT_NONE;
}

void DragDropToolBar::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
}

void DragDropToolBar::OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
}
