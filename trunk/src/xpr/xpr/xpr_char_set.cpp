//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_char_set.h"
#include "xpr_rcode.h"
#include "xpr_math.h"
#include "xpr_memory.h"

#include <iconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
static const xpr_char_t *kCodePage[] = {
    "",             // CodePageNone
    "char",         // CodePageMultibytes
    "utf-8",        // CodePageUtf8
    "utf-16le",     // CodePageUtf16
    "utf-32le",     // CodePageUtf32
};

xpr_rcode_t convertCharSet(const void *aInput,  xpr_size_t  aInputBytes,  CodePage aInputCodePage,
                                 void *aOutput, xpr_size_t *aOutputBytes, CodePage aOutputCodePage)
{
    if (aInput == XPR_NULL || aOutput == XPR_NULL || aOutputBytes == XPR_NULL)
        return XPR_RCODE_EINVAL;

    if (aInputBytes == 0)
    {
        *aOutputBytes = 0;
        return XPR_RCODE_SUCCESS;
    }

    static const xpr_size_t sCodePageCount = XPR_COUNTOF(kCodePage);
    if (!XPR_IS_RANGE(0, (xpr_size_t)aInputCodePage,  sCodePageCount-1) ||
        !XPR_IS_RANGE(0, (xpr_size_t)aOutputCodePage, sCodePageCount-1))
    {
        return XPR_RCODE_EINVAL;
    }

    if (aInputCodePage == aOutputCodePage)
    {
        if (aInputBytes > *aOutputBytes)
            return XPR_RCODE_EINVAL;

        MemCpy(aOutput, aInput, aInputBytes);
        *aOutputBytes = aInputBytes;

        return XPR_RCODE_SUCCESS;
    }

    const xpr_char_t *sFromEncoding = kCodePage[aInputCodePage];
    const xpr_char_t *sToEncoding = kCodePage[aOutputCodePage];

    iconv_t sDecoder = ::iconv_open(sToEncoding, sFromEncoding);
    if (sDecoder == (iconv_t)-1)
        return XPR_RCODE_ENOTSUP;

    xpr_size_t sInputBytesLeft = aInputBytes;
    xpr_size_t sOutputBytesLeft = *aOutputBytes;

    xpr_size_t sResult = ::iconv(sDecoder, (const xpr_char_t **)&aInput, &sInputBytesLeft, (xpr_char_t **)&aOutput, &sOutputBytesLeft);

    ::iconv_close(sDecoder);

    xpr_rcode_t sRcode = XPR_RCODE_SUCCESS;
    if (sResult == (xpr_size_t)-1)
    {
        sRcode = errno;
        if (sRcode == 0)
            sRcode = XPR_RCODE_ENOTSUP;
    }

    if (sRcode == XPR_RCODE_SUCCESS)
    {
        *aOutputBytes -= sOutputBytesLeft;
    }

    return sRcode;
}
} // namespace xpr
