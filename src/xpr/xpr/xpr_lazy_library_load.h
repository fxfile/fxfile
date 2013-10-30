//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_LAZY_LIBRARY_LOAD_H__
#define __XPR_LAZY_LIBRARY_LOAD_H__ 1
#pragma once

#include "xpr_types.h"

#include <iconv.h>

typedef iconv_t (* LIBICON_OPEN_FUNC )(const char* tocode, const char* fromcode);
typedef size_t  (* LIBICON_FUNC      )(iconv_t cd,  char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);
typedef int     (* LIBICON_CLOSE_FUNC)(iconv_t cd);

namespace xpr
{
struct LazyIconvLibrary
{
    void               *mDlHandle;
    LIBICON_OPEN_FUNC   mIconOpenFunc;
    LIBICON_FUNC        mIconFunc;
    LIBICON_CLOSE_FUNC  mIconCloseFunc;
};

extern LazyIconvLibrary gLazyIconvLibrary;

xpr_bool_t loadIconvLibrary(void);
void       unloadIconvLibrary(void);
} // namespace xpr

#endif // __XPR_LAZY_LIBRARY_LOAD_H__
