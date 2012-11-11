//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DropTarget.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DropTarget::DropTarget(void)
    : mDropHelper(XPR_NULL), mUseDropHelper(XPR_FALSE), mDragShowContent(XPR_FALSE)
    , mObserver(XPR_NULL)
{
    HRESULT sHResult;
    sHResult = CoCreateInstance(CLSID_DragDropHelper, XPR_NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&mDropHelper);
    if (SUCCEEDED(sHResult))
    {
        mUseDropHelper = XPR_TRUE;
    }
}

DropTarget::~DropTarget(void)
{
    COM_RELEASE(mDropHelper);
}

void DropTarget::registerObserver(DropTargetObserver *aObserver)
{
    mObserver = dynamic_cast<DropTargetObserver *>(aObserver);
}

void DropTarget::unregisterObserver(void)
{
    mObserver = XPR_NULL;
}

void DropTarget::setDragContents(xpr_bool_t aDragContents)
{
    mUseDropHelper = XPR_FALSE;

    if (XPR_IS_TRUE(aDragContents))
    {
        if (XPR_IS_FALSE(mUseDropHelper))
        {
            HRESULT sHResult;

            sHResult = ::CoCreateInstance(
                CLSID_DragDropHelper,
                XPR_NULL,
                CLSCTX_INPROC_SERVER,
                IID_IDropTargetHelper,
                (LPVOID *)&mDropHelper);

            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(mDropHelper))
            {
                mUseDropHelper = XPR_TRUE;
            }
            else
            {
                COM_RELEASE(mDropHelper);
            }
        }
    }
    else
    {
        COM_RELEASE(mDropHelper);
    }
}

DROPEFFECT DropTarget::OnDragEnter(CWnd *aWnd, COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    mKeyState = aKeyState;
    mRightDrag = XPR_FALSE;

    // bug: expand function on folder tree (hide drag contents for dragging)
    mDragShowContent = XPR_FALSE;
    if (xpr::getOsVer() >= xpr::kOsVerWin2000)
    {
        ::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &mDragShowContent, 0);
        if (XPR_IS_FALSE(mDragShowContent))
        {
            if (XPR_IS_TRUE(mUseDropHelper) && XPR_IS_NOT_NULL(mDropHelper))
                mDropHelper->Show(XPR_FALSE);
        }
    }

    if (XPR_IS_NOT_NULL(aWnd) && XPR_IS_NOT_NULL(aOleDataObject))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            return mObserver->OnDragEnter(aOleDataObject, aKeyState, aPoint);
    }

    // Default
    if (XPR_IS_TRUE(mUseDropHelper))
    {
        IDataObject* pIDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
        mDropHelper->DragEnter(aWnd->GetSafeHwnd(), pIDataObject, &aPoint, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK);
    }

    return DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;
}

DROPEFFECT DropTarget::OnDragOver(CWnd *aWnd, COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    mKeyState = aKeyState;
    if (aKeyState & MK_RBUTTON) mRightDrag = XPR_TRUE;

    if (XPR_IS_FALSE(mDragShowContent))
    {
        if (XPR_IS_TRUE(mUseDropHelper) && XPR_IS_NOT_NULL(mDropHelper))
            mDropHelper->Show(XPR_FALSE);
    }

    if (XPR_IS_NOT_NULL(aWnd) && XPR_IS_NOT_NULL(aOleDataObject))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            return mObserver->OnDragOver(aOleDataObject, aKeyState, aPoint);
    }

    // Default
    if (XPR_IS_NOT_NULL(mDropHelper))
    {
        CPoint ptDrag(aPoint);
        //aWnd->ClientToScreen(&ptDrag);
        mDropHelper->DragOver(&ptDrag, DROPEFFECT_NONE);
    }

    return DROPEFFECT_NONE;
}

void DropTarget::OnDragLeave(CWnd *aWnd)
{
    mRightDrag = XPR_FALSE;

    if (XPR_IS_NOT_NULL(aWnd))
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->OnDragLeave();
            return;
        }
    }

    // Default
    if (XPR_IS_TRUE(mUseDropHelper))
        mDropHelper->DragLeave();
}

DROPEFFECT DropTarget::OnDragScroll(CWnd *aWnd, DWORD aKeyState, CPoint aPoint)
{
    mKeyState = aKeyState;

    if (XPR_IS_NOT_NULL(aWnd))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->OnDragScroll(aKeyState, aPoint);
    }

    return COleDropTarget::OnDragScroll(aWnd, aKeyState, aPoint);
}

xpr_bool_t DropTarget::OnDrop(CWnd *aWnd, COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
    if (XPR_IS_NOT_NULL(aWnd) && XPR_IS_NOT_NULL(aOleDataObject))
    {
        if (XPR_IS_NOT_NULL(mObserver))
            mObserver->OnDrop(aOleDataObject, aDropEffect, aPoint);
    }

    mRightDrag = XPR_FALSE;

    return COleDropTarget::OnDrop(aWnd, aOleDataObject, aDropEffect, aPoint);
}
