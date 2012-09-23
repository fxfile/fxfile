//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DRAG_DROP_TOOL_BAR_H__
#define __FX_DRAG_DROP_TOOL_BAR_H__
#pragma once

#include "ToolBarEx.h"
#include "DropTarget.h"

class DragDropToolBar : public CToolBarEx, public DropTargetObserver
{
    typedef CToolBarEx super;

    DECLARE_DYNAMIC(DragDropToolBar)

public:
    DragDropToolBar(void);
    virtual ~DragDropToolBar(void);

public:
    DropTarget *getDropTarget(void) { return &mDropTarget; }

protected:
    // from DropTargetObserver
    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

    DropTarget mDropTarget;
    xpr_uint_t mShellIDListClipFormat;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDestroy(void);
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
};

#endif // __FX_DRAG_DROP_TOOL_BAR_H__
