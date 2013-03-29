//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_process.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
XPR_DL_API xpr_pid_t getPid(void)
{
    return (xpr_pid_t)GetCurrentProcessId();
}
#endif // XPR_CFG_OS_WINDOWS
} // namespace xpr
