//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_SHM_H__
#define __XPR_SHM_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
class XPR_DL_API Shm
{
    DISALLOW_COPY_AND_ASSIGN(Shm);

public:
    Shm(void);
    ~Shm(void);

public:
    xpr_rcode_t create(xpr_key_t aKey, xpr_size_t aReqSize, xpr_mode_t aPermission);
    xpr_rcode_t open(xpr_key_t aKey);
    xpr_bool_t isOpened(void);
    void close(void);

public:
    xpr_rcode_t attach(void **aAttachedAddr);
    xpr_rcode_t detach(void *aAttachedAddr);

protected:
    struct NativeHandle
    {
#if defined(XPR_CFG_OS_WINDOWS)
        HANDLE     mHandle;
#else
        sint32_t   aShmId;
#endif
        void      *mAddr;
        xpr_size_t mSize;
    };

    NativeHandle mHandle;
};
} // namespace xpr

#endif // __XPR_SHM_H__
