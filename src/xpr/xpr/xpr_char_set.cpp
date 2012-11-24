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
static const xpr_char_t *kCharSets[] = {
    "",             // CharSetNone
    "char",         // CharSetMultiBytes
    "utf-8",        // CharSetUtf8
    "utf-16le",     // CharSetUtf16
    "utf-16be",     // CharSetUtf16be
    "utf-32le",     // CharSetUtf32
    "utf-32be",     // CharSetUtf32be
};

xpr_rcode_t convertCharSet(const void *aInput,  xpr_size_t *aInputBytes,  CharSet aInputCharSet,
                                 void *aOutput, xpr_size_t *aOutputBytes, CharSet aOutputCharSet)
{
    if (XPR_IS_NULL(aInput) || XPR_IS_NULL(aOutput) || XPR_IS_NULL(aInputBytes) || XPR_IS_NULL(aOutputBytes))
        return XPR_RCODE_EINVAL;

    if (*aInputBytes == 0)
    {
        *aOutputBytes = 0;
        return XPR_RCODE_SUCCESS;
    }

    static const xpr_size_t sCharSetCount = XPR_COUNT_OF(kCharSets);
    if (!XPR_IS_RANGE(0, (xpr_size_t)aInputCharSet,  sCharSetCount-1) ||
        !XPR_IS_RANGE(0, (xpr_size_t)aOutputCharSet, sCharSetCount-1))
    {
        return XPR_RCODE_EINVAL;
    }

    if (aInputCharSet == aOutputCharSet)
    {
        if (*aInputBytes > *aOutputBytes)
        {
            xpr_memcpy(aOutput, aInput, *aOutputBytes);
            *aInputBytes = *aOutputBytes;
        }
        else
        {
            xpr_memcpy(aOutput, aInput, *aInputBytes);
            *aOutputBytes = *aInputBytes;
        }

        return XPR_RCODE_SUCCESS;
    }

    const xpr_char_t *sFromEncoding = kCharSets[aInputCharSet];
    const xpr_char_t *sToEncoding = kCharSets[aOutputCharSet];

    iconv_t sDecoder = ::iconv_open(sToEncoding, sFromEncoding);
    if (sDecoder == (iconv_t)-1)
        return XPR_RCODE_ENOTSUP;

    xpr_size_t sInputBytesLeft = *aInputBytes;
    xpr_size_t sOutputBytesLeft = *aOutputBytes;

    xpr_size_t sResult = ::iconv(sDecoder, (const xpr_char_t **)&aInput, &sInputBytesLeft, (xpr_char_t **)&aOutput, &sOutputBytesLeft);

    ::iconv_close(sDecoder);

    xpr_rcode_t sRcode = XPR_RCODE_SUCCESS;
    if (sResult == (xpr_size_t)-1)
    {
        // When iconv function can not convert multibyte sequence,
        // errno may be one of following error value.

        // E2BIG  - There is not sufficient room at *outbuf.
        // EILSEQ - An invalid multibyte sequence has been encountered in the input.
        // EINVAL - An incomplete multibyte sequence has been encountered in the input.

        sRcode = XPR_RCODE_SUCCESS;
    }

    if (sRcode == XPR_RCODE_SUCCESS)
    {
        *aInputBytes -= sInputBytesLeft;
        *aOutputBytes -= sOutputBytesLeft;
    }

    return sRcode;
}
} // namespace xpr
