//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_CLIP_FORMAT_H__
#define __FXB_CLIP_FORMAT_H__
#pragma once

#include "xpr_pattern.h"

namespace fxb
{
class ClipFormat : public xpr::Singleton<ClipFormat>
{
    friend class xpr::Singleton<ClipFormat>;

protected: ClipFormat(void);
public:   ~ClipFormat(void);

public:
    void registerClipFormat(void);

public:
    xpr_uint_t mShellIDList;
    xpr_uint_t mDropEffect;
    xpr_uint_t mShellIDListOffset;
    xpr_uint_t mFileName;
    xpr_uint_t mFileNameW;
    xpr_uint_t mFileNameMap;
    xpr_uint_t mFileNameMapW;
    xpr_uint_t mFileContents;
    xpr_uint_t mFileDescriptorA;
    xpr_uint_t mFileDescriptorW;
    xpr_uint_t mInetUrl;
};
} // namespace fxb

#endif // __FXB_CLIP_FORMAT_H__
