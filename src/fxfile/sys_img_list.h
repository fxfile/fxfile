//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SYS_IMG_LIST_H__
#define __FXFILE_SYS_IMG_LIST_H__ 1
#pragma once

namespace fxfile
{
class SysImgList : public CImageList
{
public:
    enum ImgSize
    {
        ImgSizeSmall = 0, // 16x16
        ImgSizeLarge,     // 32x32
        ImgSizeExtra,     // 48x48, xp more
        ImgSizeJumbo,     // 256x256, vista more
    };

public:
    SysImgList(void);
    virtual ~SysImgList(void);

public:
    void init(ImgSize aImgSize);
    void release(void);

protected:
    HIMAGELIST SHGetImageList(xpr_sint_t aImageSize);
};
} // namespace fxfile

#endif // __FXFILE_SYS_IMG_LIST_H__
