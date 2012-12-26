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
    "utf-16le",     // CharSetUtf16le
    "utf-16be",     // CharSetUtf16be
    "utf-32le",     // CharSetUtf32le
    "utf-32be",     // CharSetUtf32be
};

CharSetConverter::CharSetConverter(void)
    : mHandle(XPR_NULL)
    , mInputCharSet(CharSetNone), mOutputCharSet(CharSetNone)
{
}

CharSetConverter::~CharSetConverter(void)
{
    close();
}

xpr_rcode_t CharSetConverter::open(CharSet aInputCharSet, CharSet aOutputCharSet)
{
    static const xpr_size_t sCharSetCount = XPR_COUNT_OF(kCharSets);
    if (!XPR_IS_RANGE(0, (xpr_size_t)aInputCharSet,  sCharSetCount-1) ||
        !XPR_IS_RANGE(0, (xpr_size_t)aOutputCharSet, sCharSetCount-1))
    {
        return XPR_RCODE_EINVAL;
    }

    const xpr_char_t *sInputEncoding = kCharSets[aInputCharSet];
    const xpr_char_t *sOutputEncoding = kCharSets[aOutputCharSet];

    iconv_t sHandle = ::iconv_open(sOutputEncoding, sInputEncoding);
    if (sHandle == (iconv_t)-1)
        return XPR_RCODE_ENOTSUP;

    mHandle        = sHandle;
    mInputCharSet  = aInputCharSet;
    mOutputCharSet = aOutputCharSet;

    return XPR_RCODE_SUCCESS;
}

xpr_bool_t CharSetConverter::isOpened(void) const
{
    return XPR_IS_NOT_NULL(mHandle) ? XPR_TRUE : XPR_FALSE;
}

void CharSetConverter::getCharSet(CharSet &aInputCharSet, CharSet &aOutputCharSet) const
{
    aInputCharSet = mInputCharSet;
    aOutputCharSet = mOutputCharSet;
}

xpr_rcode_t CharSetConverter::convert(const void *aInput,  xpr_size_t *aInputBytes,
                                            void *aOutput, xpr_size_t *aOutputBytes)
{
    if (XPR_IS_NULL(aInput) || XPR_IS_NULL(aOutput) || XPR_IS_NULL(aInputBytes) || XPR_IS_NULL(aOutputBytes))
        return XPR_RCODE_EINVAL;

    if (XPR_IS_NULL(mHandle))
        return XPR_RCODE_EBADF;

    if (*aInputBytes == 0)
    {
        *aOutputBytes = 0;
        return XPR_RCODE_SUCCESS;
    }

    if (mInputCharSet == mOutputCharSet)
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

    iconv_t sIconv = (iconv_t)mHandle;

    xpr_size_t sInputBytesLeft = *aInputBytes;
    xpr_size_t sOutputBytesLeft = *aOutputBytes;

    xpr_size_t sResult = ::iconv(sIconv,
                                 (const xpr_char_t **)&aInput,
                                 &sInputBytesLeft,
                                 (xpr_char_t **)&aOutput,
                                 &sOutputBytesLeft);

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

void CharSetConverter::close(void)
{
    if (XPR_IS_NOT_NULL(mHandle))
    {
        iconv_t sIconv = (iconv_t)mHandle;

        ::iconv_close(sIconv);

        mHandle = XPR_NULL;
    }
}

XPR_DL_API xpr_rcode_t convertCharSet(const void *aInput,  xpr_size_t *aInputBytes,  CharSet aInputCharSet,
                                            void *aOutput, xpr_size_t *aOutputBytes, CharSet aOutputCharSet)
{
    if (XPR_IS_NULL(aInput) || XPR_IS_NULL(aOutput) || XPR_IS_NULL(aInputBytes) || XPR_IS_NULL(aOutputBytes))
        return XPR_RCODE_EINVAL;

    xpr_rcode_t sRcode;

    CharSetConverter sCharSetConverter;
    sRcode = sCharSetConverter.open(aInputCharSet, aOutputCharSet);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return sRcode;

    sRcode = sCharSetConverter.convert(aInput, aInputBytes, aOutput, aOutputBytes);

    sCharSetConverter.close();

    return sRcode;
}
} // namespace xpr
