//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "crc_checksum.h"

#include "crc32.h"
#include "md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
const xpr_size_t kCrcBufferSize = 16 * 1024; // 16KB
} // namespace anonymous

xpr_rcode_t getFileCrcSfv(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal)
{
    XPR_ASSERT(aPath != XPR_NULL);
    XPR_ASSERT(aCrcVal != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sRead;
    xpr::FileIo sFileIo;
    const xpr_sint_t sBufLen = sizeof(xpr_uchar_t) * kCrcBufferSize;

    sRcode = sFileIo.open(aPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_uchar_t *sBuffer = new xpr_uchar_t[kCrcBufferSize];
    if (XPR_IS_NULL(sBuffer))
    {
        sFileIo.close();

        return XPR_RCODE_ENOMEM;
    }

    CRC32_CTX sHashCtx;
    CRC32_init(&sHashCtx);

    do
    {
        sRcode = sFileIo.read(sBuffer, sBufLen, &sRead);
        if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0)
        {
            HASH_update(&sHashCtx, sBuffer, (xpr_sint_t)sRead);
        }
    } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0);

    XPR_SAFE_DELETE_ARRAY(sBuffer);

    sFileIo.close();

    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_sint_t i;
    const xpr_sint_t sSize = HASH_size(&sHashCtx);
    const xpr_uint8_t *sHashValue = (const xpr_uint8_t *)HASH_final(&sHashCtx);

    aCrcVal[0] = '\0';

    for (i = 0; i < sSize; ++i)
    {
        sprintf(aCrcVal + strlen(aCrcVal), XPR_MBCS_STRING_LITERAL("%02x"), (xpr_sint_t)sHashValue[i]);
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t getFileCrcMd5(const xpr_tchar_t *aPath, xpr_char_t *aCrcVal)
{
    XPR_ASSERT(aPath != XPR_NULL);
    XPR_ASSERT(aCrcVal != XPR_NULL);

    xpr_rcode_t sRcode;
    xpr_ssize_t sRead;
    xpr::FileIo sFileIo;
    const xpr_sint_t sBufLen = sizeof(xpr_uchar_t) * kCrcBufferSize;

    sRcode = sFileIo.open(aPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_uchar_t *sBuffer = new xpr_uchar_t[kCrcBufferSize];
    if (XPR_IS_NULL(sBuffer))
    {
        sFileIo.close();

        return XPR_RCODE_ENOMEM;
    }

    MD5_CTX sHashCtx;
    MD5_init(&sHashCtx);

    do
    {
        sRcode = sFileIo.read(sBuffer, sBufLen, &sRead);
        if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0)
        {
            HASH_update(&sHashCtx, sBuffer, (xpr_sint_t)sRead);
        }
    } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0);

    XPR_SAFE_DELETE_ARRAY(sBuffer);

    sFileIo.close();

    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_sint_t i;
    const xpr_sint_t sSize = HASH_size(&sHashCtx);
    const xpr_uint8_t *sHashValue = (const xpr_uint8_t *)HASH_final(&sHashCtx);

    aCrcVal[0] = '\0';

    for (i = 0; i < sSize; ++i)
    {
        sprintf(aCrcVal + strlen(aCrcVal), XPR_MBCS_STRING_LITERAL("%02x"), (xpr_sint_t)sHashValue[i]);
    }

    return XPR_RCODE_SUCCESS;
}
} // namespace fxfile
