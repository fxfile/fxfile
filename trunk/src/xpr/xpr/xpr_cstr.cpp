//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_cstr.h"
#include "xpr_rcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
XPR_DL_API xpr_char_t *xpr_strcpy(xpr_char_t *aDest, xpr_size_t aDestSize, const xpr_char_t *aSrc, xpr_size_t aCount)
{
#if defined(XPR_CFG_OS_WINDOWS)
    strncpy_s(aDest, aDestSize, aSrc, aCount);
    return aDest;
#else // not, XPR_CFG_OS_WINDOWS
#endif // XPR_CFG_OS_WINDOWS
}

XPR_DL_API xpr_sint_t xpr_strcmp(const xpr_char_t *aStr1, const xpr_char_t *aStr2, xpr_size_t aLength)
{
    return strcmp(aStr1, aStr2);
}

// reference: FreeBSD
XPR_DL_API xpr_char_t *xpr_strnstr(const xpr_char_t *aStr, const xpr_char_t *aSubStr, xpr_size_t aLength)
{
    xpr_char_t sSubStrChar, sChar;
    xpr_size_t sSubStrLength;

    if ((sSubStrChar = *aSubStr++) != '\0')
    {
        sSubStrLength = strlen(aSubStr);

        do
        {
            do
            {
                if ((sChar = *aStr++) == '\0' || aLength-- < 1)
                    return XPR_NULL;
            } while (sChar != sSubStrChar);

            if (sSubStrLength > aLength)
                return XPR_NULL;

        } while (strncmp(aStr, aSubStr, sSubStrLength) != 0);

        aStr--;
    }

    return ((xpr_char_t *)aStr);
}

XPR_DL_API xpr_char_t *xpr_strrstr(const xpr_char_t *aStr, const xpr_char_t *aSubStr)
{
    xpr_size_t sLength1 = strlen(aStr);
    xpr_size_t sLength2 = strlen(aSubStr);
    xpr_sint_t sPos = (xpr_sint_t)sLength1 - (xpr_sint_t)sLength2;
    xpr_char_t *sResult = XPR_NULL;

    if (sLength1 != 0 && sLength2 == 0)
        return (xpr_char_t *)aStr;

    if (sLength1 == 0 || sLength2 == 0)
        return (xpr_char_t *)aStr;

    if (sLength1 < sLength2)
        return XPR_NULL;

    while (sPos >= 0)
    {
        if (strncmp(aStr + sPos, aSubStr, sLength2) == 0)
        {
            sResult = (xpr_char_t *)aStr + sPos;
            break;
        }

        sPos--;
    }

    return sResult;
}

XPR_DL_API xpr_rcode_t xpr_printf(const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vprintf(aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

XPR_DL_API xpr_rcode_t xpr_vprintf(const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = vprintf(aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t xpr_fprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vfprintf(aFile, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

XPR_DL_API xpr_rcode_t xpr_vfprintf(xpr_file_t *aFile, const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = vfprintf(aFile, aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t xpr_snprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, ...)
{
    xpr_rcode_t sRcode;

    va_list sArgs;
    va_start(sArgs, aFormat);

    sRcode = xpr_vsnprintf(aBuffer, aSize, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

XPR_DL_API xpr_rcode_t xpr_vsnprintf(xpr_char_t *aBuffer, xpr_size_t aSize, const xpr_char_t *aFormat, va_list aArgs)
{
    xpr_sint_t r = _vsnprintf(aBuffer, aSize, aFormat, aArgs);
    if (r < 0)
        return errno;

    return XPR_RCODE_SUCCESS;
}
} // namespace xpr
