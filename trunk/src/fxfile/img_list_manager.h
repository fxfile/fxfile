//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_IMG_LIST_MANAGER_H__
#define __FXFILE_IMG_LIST_MANAGER_H__ 1
#pragma once

#include "singleton_manager.h"
#include "sys_img_list.h"

namespace fxfile
{
class ImgListManager : public SingletonInstance
{
    friend class SingletonManager;

protected: ImgListManager(void);
public:   ~ImgListManager(void);

public:
    void loadSystemImgList(void);
    void loadCustomImgList(const xpr_tchar_t *aCustomImagePath16,
                           const xpr_tchar_t *aCustomImagePath32,
                           const xpr_tchar_t *aCustomImagePath48,
                           const xpr_tchar_t *aCustomImagePath256);

public:
    // system image list
    SysImgList mSysImgList16;
    SysImgList mSysImgList32;
    SysImgList mSysImgList48;
    SysImgList mSysImgList256;

    // custom image list
    SysImgList mCustomImgList16;
    SysImgList mCustomImgList32;
    SysImgList mCustomImgList48;
    SysImgList mCustomImgList256;
};
} // namespace fxfile

#endif // __FXFILE_IMG_LIST_MANAGER_H__
