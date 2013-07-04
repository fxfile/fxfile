//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ICON_LIST_H__
#define __FXFILE_ICON_LIST_H__
#pragma once

class IconList : public CListBox
{
public:
    IconList(void);
    virtual ~IconList(void);

public:
    void setImageList(CImageList *aLargeImgList, CImageList *aSmallImgList);
    void setImageList(xpr_uint_t aLargeId, xpr_uint_t aSmallId);
    void setIconOffset(xpr_sint_t aOffset);

protected:
    void insertIcons(void);

protected:
    xpr_sint_t mOffset;

    CImageList *mLargeImgList;
    CImageList *mSmallImgList;

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT aDrawItemStruct);

protected:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);
    virtual void PreSubclassWindow(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDestroy(void);
};

#endif // __FXFILE_ICON_LIST_H__
