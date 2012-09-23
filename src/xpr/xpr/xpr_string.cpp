//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_string.h"
#include "xpr_rcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
xpr_char_t *xpr_strcpy(xpr_char_t *aDest, xpr_size_t aDestSize, const xpr_char_t *aSrc, xpr_size_t aCount)
{
#if defined(XPR_CFG_OS_WINDOWS)
    strncpy_s(aDest, aDestSize, aSrc, aCount);
    return aDest;
#else // not, XPR_CFG_OS_WINDOWS
#endif // XPR_CFG_OS_WINDOWS
}

xpr_sint_t xpr_strcmp(const xpr_char_t *aStr1, const xpr_char_t *aStr2, xpr_size_t aLength)
{
    return strcmp(aStr1, aStr2);
}

xpr_rcode_t xpr_printf(const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vprintf(aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t xpr_vprintf(const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = vprintf(aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t xpr_fprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vfprintf(aFile, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t xpr_vfprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = vfprintf(aFile, aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t xpr_snprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vsnprintf(aBuffer, aSize, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t xpr_vsnprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = _vsnprintf(aBuffer, aSize, aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}
} // namespace xpr
