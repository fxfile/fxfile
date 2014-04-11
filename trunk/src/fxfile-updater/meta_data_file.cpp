//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "meta_data_file.h"
#include "conf_file_ex.h"
#include "crc32.h"
#include "md5.h"
#include "sha.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace updater
{
static const xpr_tchar_t kProgramKey  [] = XPR_STRING_LITERAL("program");
static const xpr_tchar_t kVersionKey  [] = XPR_STRING_LITERAL("version");
static const xpr_tchar_t kFileKey     [] = XPR_STRING_LITERAL("file");
static const xpr_tchar_t kHashCrc32Key[] = XPR_STRING_LITERAL("hash-crc32");
static const xpr_tchar_t kHashMd5Key  [] = XPR_STRING_LITERAL("hash-md5");
static const xpr_tchar_t kHashSha1Key [] = XPR_STRING_LITERAL("hash-sha1");

static const xpr_sint_t  kHashBufferSize = 16 * 1024; // 16KB

/**
 * parse meta-data file
 */
xpr_bool_t MetaDataFile::parseMetaFile(const xpr::string &aMetaFilePath, MetaData &aMetaData)
{
    fxfile::base::ConfFileEx sConfFile(aMetaFilePath.c_str());

    sConfFile.setFlags(fxfile::base::ConfFile::kFlagLock);

    if (sConfFile.load() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    fxfile::base::ConfFile::Section *sSection = sConfFile.findSection(XPR_STRING_LITERAL(""));
    XPR_ASSERT(sSection != XPR_NULL);

    aMetaData.mProgram   = sConfFile.getValueS(sSection, kProgramKey,   XPR_STRING_LITERAL(""));
    aMetaData.mVersion   = sConfFile.getValueS(sSection, kVersionKey,   XPR_STRING_LITERAL(""));
    aMetaData.mFile      = sConfFile.getValueS(sSection, kFileKey,      XPR_STRING_LITERAL(""));
    aMetaData.mHashCrc32 = sConfFile.getValueS(sSection, kHashCrc32Key, XPR_STRING_LITERAL(""));
    aMetaData.mHashMd5   = sConfFile.getValueS(sSection, kHashMd5Key,   XPR_STRING_LITERAL(""));
    aMetaData.mHashSha1  = sConfFile.getValueS(sSection, kHashSha1Key,  XPR_STRING_LITERAL(""));

    return XPR_TRUE;
}

xpr_bool_t MetaDataFile::verifyChecksum(const xpr::string &aProgramFilePath, MetaData &aMetaData)
{
    xpr_rcode_t sRcode;
    xpr::FileIo sFileIo;
    xpr_ssize_t sRead;

    sRcode = sFileIo.open(aProgramFilePath.c_str(), xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    CRC32_CTX sCrc32Ctx;
    MD5_CTX   sMd5Ctx;
    MD5_CTX   sSha1Ctx;

    CRC32_init(&sCrc32Ctx);
    MD5_init(&sMd5Ctx);
    SHA_init(&sSha1Ctx);

    xpr_uchar_t *sBuffer = new xpr_uchar_t[kHashBufferSize];

    do
    {
        sRcode = sFileIo.read(sBuffer, kHashBufferSize, &sRead);
        if (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0)
        {
            HASH_update(&sCrc32Ctx, sBuffer, (xpr_sint_t)sRead);
            HASH_update(&sMd5Ctx,   sBuffer, (xpr_sint_t)sRead);
            HASH_update(&sSha1Ctx,  sBuffer, (xpr_sint_t)sRead);
        }
    } while (XPR_RCODE_IS_SUCCESS(sRcode) && sRead > 0);

    XPR_SAFE_DELETE_ARRAY(sBuffer);

    sFileIo.close();

    const xpr_uint8_t *sHashCrc32Value = (const xpr_uint8_t *)HASH_final(&sCrc32Ctx);
    const xpr_uint8_t *sHashMd5Value   = (const xpr_uint8_t *)HASH_final(&sMd5Ctx);
    const xpr_uint8_t *sHashSha1Value  = (const xpr_uint8_t *)HASH_final(&sSha1Ctx);

    // binary to ascii
    xpr_sint_t i;
    xpr_tchar_t sHashCrc32[CRC32_DIGEST_SIZE * 2 + 1] = {0};
    xpr_tchar_t sHashMd5  [MD5_DIGEST_SIZE   * 2 + 1] = {0};
    xpr_tchar_t sHashSha1 [SHA_DIGEST_SIZE   * 2 + 1] = {0};

    for (i = 0; i < CRC32_DIGEST_SIZE; ++i)
    {
        _stprintf(sHashCrc32 + _tcslen(sHashCrc32), XPR_STRING_LITERAL("%02x"), (xpr_sint_t)sHashCrc32Value[i]);
    }

    for (i = 0; i < MD5_DIGEST_SIZE; ++i)
    {
        _stprintf(sHashMd5 + _tcslen(sHashMd5), XPR_STRING_LITERAL("%02x"), (xpr_sint_t)sHashMd5Value[i]);
    }

    for (i = 0; i < SHA_DIGEST_SIZE; ++i)
    {
        _stprintf(sHashSha1 + _tcslen(sHashSha1), XPR_STRING_LITERAL("%02x"), (xpr_sint_t)sHashSha1Value[i]);
    }

    // verify the calculated checksum between checksum of meta file
    if (aMetaData.mHashCrc32.compare_case(sHashCrc32) != 0)
    {
        return XPR_FALSE;
    }

    if (aMetaData.mHashMd5.compare_case(sHashMd5) != 0)
    {
        return XPR_FALSE;
    }

    if (aMetaData.mHashSha1.compare_case(sHashSha1) != 0)
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}
} // namespace fxfile
} // namespace updater
