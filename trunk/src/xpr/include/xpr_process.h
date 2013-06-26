//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_PROCESS_H__
#define __XPR_PROCESS_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

typedef xpr_uint64_t xpr_pid_t;

namespace xpr
{
XPR_DL_API xpr_pid_t getPid(void);
} // namespace xpr

#endif // __XPR_PROCESS_H__
