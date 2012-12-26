//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_INIT_H__
#define __XPR_INIT_H__
#pragma once

#include "xpr_dlsym.h"

namespace xpr
{
XPR_DL_API void initialize(void);
XPR_DL_API void finalize(void);
} // namespace xpr

#endif // __XPR_INIT_H__
