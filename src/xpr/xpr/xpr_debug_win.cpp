//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_debug.h"
#include "xpr_config.h"
#include "xpr_memory.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
XPR_DL_API void traceOut(const xpr_tchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sMsgLength = _vsctprintf(aFormat, sArgs);
    xpr_tchar_t *sMsg = new xpr_tchar_t[sMsgLength + 1];

    _vstprintf(sMsg, aFormat, sArgs);

    ::OutputDebugString(sMsg);

    va_end(sArgs);

    XPR_SAFE_DELETE_ARRAY(sMsg);
}
#endif
} // namespace xpr
