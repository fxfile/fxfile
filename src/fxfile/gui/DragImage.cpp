//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DragImage.h"

#include "../user_env.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DragImage::DragImage(void)
    : mDragSourceHelper(XPR_NULL)
    , mMyDragging(XPR_FALSE), mMyDragImage(XPR_NULL)
{
}

DragImage::~DragImage(void)
{
    COM_RELEASE(mDragSourceHelper);
    XPR_SAFE_DELETE(mMyDragImage);
}

xpr_bool_t DragImage::beginDrag(HWND aHwnd, CImageList *aMyDragImage, LPDATAOBJECT aDataObject)
{
    if (XPR_IS_NULL(aMyDragImage))
    {
        mMyDragging = XPR_FALSE;

        if (xpr::getOsVer() >= xpr::kOsVerWin98 && fxfile::UserEnv::instance().mInetExpVer == fxfile::UserEnv::InetExp50More)
        {
            ::CoCreateInstance(
                CLSID_DragDropHelper,
                XPR_NULL,
                CLSCTX_INPROC_SERVER,
                IID_IDragSourceHelper,
                (LPVOID *)&mDragSourceHelper);

            if (XPR_IS_NOT_NULL(mDragSourceHelper))
            {
                CPoint sPoint(-20,0);
                GetCursorPos(&sPoint);
                ScreenToClient(aHwnd, &sPoint);

                HRESULT sHResult;
                sHResult = mDragSourceHelper->InitializeFromWindow(aHwnd, &sPoint, aDataObject);
                if (FAILED(sHResult))
                {
                    COM_RELEASE(mDragSourceHelper);
                }
            }
        }

        if (XPR_IS_NULL(mDragSourceHelper))
            return XPR_FALSE;
    }
    else
    {
        mMyDragging = XPR_TRUE;

        //CPoint sPoint(8,8);
        mMyDragImage = aMyDragImage;
        //mMyDragImage->BeginDrag(0, sPoint);
        mMyDragImage->BeginDrag(0, CPoint(-20, 0));
    }

    return XPR_TRUE;
}

void DragImage::showMyDragNoLock(xpr_bool_t aNoLock)
{
    if (XPR_IS_NOT_NULL(mMyDragImage))
    {
        mMyDragImage->DragShowNolock(aNoLock);
    }
}

void DragImage::endDrag(void)
{
    COM_RELEASE(mDragSourceHelper);

    mMyDragging = XPR_FALSE;

    if (XPR_IS_NOT_NULL(mMyDragImage))
    {
        mMyDragImage->EndDrag();

        XPR_SAFE_DELETE(mMyDragImage);
    }
}

xpr_bool_t DragImage::isMyDragging(void) const
{
    return mMyDragging;
}

CImageList *DragImage::getMyDragImage(void) const
{
    return mMyDragImage;
}
