//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRAG_IMAGE_H__
#define __FXFILE_DRAG_IMAGE_H__
#pragma once

#include "pattern.h"

class DragImage : public fxfile::base::Singleton<DragImage>
{
    friend class fxfile::base::Singleton<DragImage>;

protected: DragImage(void);
public:   ~DragImage(void);

public:
    xpr_bool_t beginDrag(HWND aHwnd, CImageList *aMyDragImage, LPDATAOBJECT aDataObject);
    void endDrag(void);

    void showMyDragNoLock(xpr_bool_t aNoLock);

    xpr_bool_t isMyDragging(void) const;
    CImageList *getMyDragImage(void) const;

protected:
    IDragSourceHelper *mDragSourceHelper;

    xpr_bool_t  mMyDragging;
    CImageList *mMyDragImage;
};

#endif // __FXFILE_DRAG_IMAGE_H__
