//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "img_list_manager.h"

#include "gui/gdi.h"

namespace fxfile
{
ImgListManager::ImgListManager(void)
{
}

ImgListManager::~ImgListManager(void)
{
}

void ImgListManager::loadSystemImgList(void)
{
    mSysImgList256.init(SysImgList::ImgSizeJumbo);
    mSysImgList48.init(SysImgList::ImgSizeExtra);
    mSysImgList32.init(SysImgList::ImgSizeLarge);
    mSysImgList16.init(SysImgList::ImgSizeSmall);
}

void ImgListManager::loadCustomImgList(const xpr_tchar_t *aCustomImagePath16,
                                       const xpr_tchar_t *aCustomImagePath32,
                                       const xpr_tchar_t *aCustomImagePath48,
                                       const xpr_tchar_t *aCustomImagePath256)
{
    struct
    {
        CImageList        *mImgList;
        const xpr_tchar_t *mImagePath;
        CSize              mIconSize;
    } sImageList[] = {
        { &mCustomImgList16,  aCustomImagePath16,  CSize( 16, 16) },
        { &mCustomImgList32,  aCustomImagePath32,  CSize( 32, 32) },
        { &mCustomImgList48,  aCustomImagePath48,  CSize( 48, 48) },
        { &mCustomImgList256, aCustomImagePath256, CSize(256,246) }
    };

    xpr_sint_t i;
    xpr_sint_t sCount = XPR_COUNT_OF(sImageList);
    xpr::string sPath;

    for (i = 0; i < sCount; ++i)
    {
        sPath = sImageList[i].mImagePath;

        if (sPath.empty() == XPR_FALSE && sPath[0] == XPR_STRING_LITERAL('%'))
        {
            sPath.clear();
            GetEnvRealPath(sImageList[i].mImagePath, sPath);
        }

        LoadImgList(sImageList[i].mImgList, sPath.c_str(), sImageList[i].mIconSize, 3);
    }
}
} // namespace fxfile
