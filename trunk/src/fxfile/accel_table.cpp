//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "accel_table.h"

#include "option.h"
#include "app_ver.h"
#include "xpr_file_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace
{
#pragma pack(push, 1)

// 100 bytes
typedef struct FileHeader
{
    xpr_char_t   mProgram[12];
    xpr_char_t   mDescription[24];
    xpr_uint16_t mMajorVer;
    xpr_uint16_t mMinorVer;
    xpr_sint32_t mCommpression;
    xpr_uint32_t mStringMode;
    xpr_uint32_t mFileType;
    xpr_char_t   mDummy[44];
} FileHeader;

// 4 bytes
typedef struct FileFooter
{
    xpr_sint32_t mEndCode;
} FileFooter;

const xpr_sint32_t kEndCode = 0xFFFFFFFF;

#pragma pack(pop)
} // namespace anonymous

AccelTable::AccelTable(void)
{
}

AccelTable::~AccelTable(void)
{
}

xpr_bool_t AccelTable::load(const xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t *aCount, xpr_sint32_t aMaxCount)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aAccel) || aCount <= 0 || aMaxCount <= 0)
        return XPR_FALSE;

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadSize;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    //----------------------------------------------------------------------
    // Check File Header - 100 Bytes
    //----------------------------------------------------------------------
    FileHeader sFileHeader = {0};
    sRcode = sFileIo.read(&sFileHeader, sizeof(FileHeader), &sReadSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sResult = strncmp(sFileHeader.mProgram, FXFILE_PROGRAM_NAME_MBCS, 11) == 0 && sFileHeader.mFileType == 1;
    if (sResult == XPR_TRUE)
    {
        //----------------------------------------------------------------------
        // Check File Footer
        //----------------------------------------------------------------------
        sFileIo.seekFromEnd(-(xpr_sint64_t)sizeof(DWORD));

        FileFooter sFileFooter = {0};
        sRcode = sFileIo.read(&sFileFooter, sizeof(FileFooter), &sReadSize);
        if (XPR_RCODE_IS_ERROR(sRcode))
            return XPR_FALSE;

        sFileIo.seekFromBegin(sizeof(FileHeader));

        sResult = (sFileFooter.mEndCode == kEndCode) ? XPR_TRUE : XPR_FALSE;
        if (sResult == XPR_TRUE)
        {
            //----------------------------------------------------------------------
            // Load File Body
            //----------------------------------------------------------------------
            xpr_sint_t sLoadedCount = 0;
            sRcode = sFileIo.read(&sLoadedCount, sizeof(xpr_sint_t), &sReadSize);
            if (XPR_RCODE_IS_ERROR(sRcode))
                return XPR_FALSE;

            if (sLoadedCount <= aMaxCount)
            {
                sRcode = sFileIo.read(aAccel, sizeof(ACCEL) * sLoadedCount, &sReadSize);
                if (XPR_RCODE_IS_ERROR(sRcode))
                    return XPR_FALSE;

                *aCount = sLoadedCount;

                sResult = XPR_TRUE;
            }
        }
    }

    sFileIo.close();

    return sResult;
}

xpr_bool_t AccelTable::save(xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t aCount) const
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aAccel) || aCount < 0)
        return XPR_FALSE;

    xpr_rcode_t sRcode;
    xpr_ssize_t sWrittenSize;
    xpr_sint_t  sOpenMode;
    xpr::FileIo sFileIo;

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sFileIo.open(aPath, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    //----------------------------------------------------------------------
    // File Header - 100 Bytes
    //----------------------------------------------------------------------
    FileHeader sFileHeader = {0};
    strcpy(sFileHeader.mProgram,     FXFILE_PROGRAM_NAME_MBCS);
    strcpy(sFileHeader.mDescription, "AccelKey");
    sFileHeader.mMajorVer     = FXFILE_MAJOR_VER;
    sFileHeader.mMinorVer     = FXFILE_MINOR_VER;
    sFileHeader.mCommpression = 0;
    sFileHeader.mStringMode   = 0;
    sFileHeader.mFileType     = 1;

    sRcode = sFileIo.write(&sFileHeader, sizeof(FileHeader), &sWrittenSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    //----------------------------------------------------------------------
    // File Body
    //----------------------------------------------------------------------
    sRcode = sFileIo.write(&aCount, sizeof(xpr_sint32_t), &sWrittenSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sRcode = sFileIo.write(aAccel, sizeof(ACCEL) * aCount, &sWrittenSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    //----------------------------------------------------------------------
    // File Tail - 4 Bytes
    //----------------------------------------------------------------------
    FileFooter sFileFooter = {0};
    sFileFooter.mEndCode = kEndCode;

    sRcode = sFileIo.write(&sFileFooter, sizeof(FileFooter), &sWrittenSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    sFileIo.close();

    return XPR_TRUE;
}
} // namespace fxfile
