//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_debug.h"
#include "xpr_config.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
XPR_DL_API void traceOutA(xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sMsgLength = _vscprintf(aFormat, sArgs);
    xpr_char_t *sMsg = new xpr_char_t[sMsgLength + 1];

    vsprintf(sMsg, aFormat, sArgs);

    ::OutputDebugStringA(sMsg);

    va_end(sArgs);

    delete[] sMsg;
    sMsg = XPR_NULL;
}

XPR_DL_API void traceOutW(xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sMsgLength = _vscwprintf(aFormat, sArgs);
    xpr_wchar_t *sMsg = new xpr_wchar_t[sMsgLength + 1];

    _vswprintf(sMsg, aFormat, sArgs);

    ::OutputDebugStringW(sMsg);

    va_end(sArgs);

    delete[] sMsg;
    sMsg = XPR_NULL;
}
#endif
} // namespace xpr
