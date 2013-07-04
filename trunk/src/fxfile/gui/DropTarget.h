//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DROP_TARGET_H__
#define __FXFILE_DROP_TARGET_H__
#pragma once

#include <afxole.h>

class DropTarget;

class DropTargetObserver
{
    friend class DropTarget;

protected:
    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint) = 0;
    virtual void       OnDragLeave(void) = 0;
    virtual DROPEFFECT OnDragOver(COleDataObject *aDataObject, DWORD aKeyState, CPoint aPoint) = 0;
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint) = 0;
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint) = 0;
};

class DropTarget : public COleDropTarget
{
public:
    DropTarget(void);
    virtual ~DropTarget(void);

public:
    void registerObserver(DropTargetObserver *aObserver);
    void unregisterObserver(void);

    void setDragContents(xpr_bool_t aDragContents = XPR_TRUE);

public:
    IDropTargetHelper *getDropHelper(void) { return mDropHelper; }
    xpr_bool_t isUseDropHelper(void) { return mUseDropHelper; }
    xpr_bool_t isRightDrag(void) { return mRightDrag; }
    DWORD getKeyState(void) { return mKeyState; }

protected:
    // from COleDropTarget
    virtual DROPEFFECT OnDragEnter(CWnd *aWnd, COleDataObject *aDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(CWnd *aWnd);
    virtual DROPEFFECT OnDragOver(CWnd *aWnd, COleDataObject *aDataObject, DWORD aKeyState, CPoint aPoint);
    virtual DROPEFFECT OnDragScroll(CWnd *aWnd, DWORD aKeyState, CPoint aPoint);
    virtual xpr_bool_t OnDrop(CWnd *aWnd, COleDataObject *aDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

protected:
    DropTargetObserver *mObserver;

    xpr_bool_t mUseDropHelper;
    IDropTargetHelper *mDropHelper;

    xpr_bool_t mDragShowContent;
    xpr_bool_t mRightDrag;
    DWORD mKeyState;
};

#endif // __FXFILE_DROP_TARGET_H__
