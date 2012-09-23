//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SYS_IMG_LIST_H__
#define __FXB_SYS_IMG_LIST_H__
#pragma once

#include "xpr_pattern.h"

namespace fxb
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
    xpr_bool_t SHGetImageLists(HIMAGELIST *aLargeImageList, HIMAGELIST *aSmallImageList);
};

class SysImgListMgr : public xpr::Singleton<SysImgListMgr>
{
    friend class xpr::Singleton<SysImgListMgr>;

protected: SysImgListMgr(void);
public:   ~SysImgListMgr(void);

public:
    void getSystemImgList(void);
    void getCustomImgList(void);

public:
    // System Image List
    SysImgList mSysImgList32;
    SysImgList mSysImgList16;

    // Custom Image List
    SysImgList mCusImgList32;
    SysImgList mCusImgList16;
};
} // namespace fxb

#endif // __FXB_SYS_IMG_LIST_H__
