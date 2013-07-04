//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_THUMB_IMG_LIST_H__
#define __FXFILE_THUMB_IMG_LIST_H__ 1
#pragma once

namespace fxfile
{
class Thumbnail;

typedef struct ThumbImage
{
    xpr::tstring  mPath;
    CBitmap      *mImage;
    xpr_sint_t    mWidth;
    xpr_sint_t    mHeight;
    xpr_sint_t    mDepth;
    FILETIME      mModifiedFileTime;
} ThumbImage;

class ThumbImgList
{
    friend class Thumbnail;

protected:
    typedef struct ThumbElement
    {
        xpr::tstring mPath;             // path name
        xpr_sint_t   mImageIndex;       // index of imagelist
        xpr_uint_t   mThumbImageId;     // thumbnail image id
        xpr_sint_t   mWidth;            // image width
        xpr_sint_t   mHeight;           // image height
        xpr_sint_t   mDepth;            // byte per pixel(color depth)
        FILETIME     mModifiedFileTime; // image last write time
    } ThumbElement;

public:
    ThumbImgList(void);
    virtual ~ThumbImgList(void);

public:
    xpr_bool_t create(xpr_sint_t cx, xpr_sint_t cy, xpr_uint_t aFlags);

    xpr_sint_t add(ThumbImage *aThumbImage, xpr_uint_t aThumbImageId);

    xpr_sint_t getImageCount(void) const;
    ThumbElement *getImage(xpr_sint_t aImageIndex);
    ThumbElement *getImage(const xpr_tchar_t *aPath);
    xpr_sint_t compareTime(xpr_sint_t aImageIndex, LPFILETIME aModifiedFileTime);

    CImageList *getImageList(void);
    xpr_bool_t draw(CDC *aDC, xpr_sint_t aImageIndex, POINT aPoint, xpr_uint_t aStyle);

    xpr_bool_t remove(xpr_sint_t aImageIndex);
    void removeAll(void);
    xpr_bool_t load(void);
    void save(void);

    void verify(void);
    xpr_bool_t clear(void);

protected:
    typedef std::deque<ThumbElement> ThumbDeque;
    ThumbDeque mThumbDeque;

    CImageList mImageList;
};
} // namespace fxfile

#endif // __FXFILE_THUMB_IMG_LIST_H__
