//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "accel_table.h"

#include "Option.h"
#include "AppVer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

static const xpr_sint32_t kEndCode = 0xFFFFFFFF;

#pragma pack(pop)

AccelTable::AccelTable(void)
{
}

AccelTable::~AccelTable(void)
{
}

xpr_bool_t AccelTable::loadFromFile(const xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t *aCount, xpr_sint32_t aMaxCount)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aAccel) || aCount <= 0 || aMaxCount <= 0)
        return XPR_FALSE;

    FILE *sFile = _tfopen(aPath, XPR_STRING_LITERAL("rb"));
    if (XPR_IS_NULL(sFile))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    //----------------------------------------------------------------------
    // Check File Header - 100 Bytes
    //----------------------------------------------------------------------
    FileHeader sFileHeader = {0};
    fread(&sFileHeader, sizeof(FileHeader), 1, sFile);
    sResult = strncmp(sFileHeader.mProgram, "flyExplorer", 11) == 0 && sFileHeader.mFileType == 1;
    if (sResult == XPR_TRUE)
    {
        //----------------------------------------------------------------------
        // Check File Footer
        //----------------------------------------------------------------------
        FileFooter sFileFooter = {0};
        fseek(sFile, -(long)sizeof(DWORD), SEEK_END);
        fread(&sFileFooter, sizeof(FileFooter), 1, sFile);
        fseek(sFile, sizeof(FileHeader), SEEK_SET);
        sResult = (sFileFooter.mEndCode == kEndCode) ? XPR_TRUE : XPR_FALSE;
        if (sResult == XPR_TRUE)
        {
            //----------------------------------------------------------------------
            // Load File Body
            //----------------------------------------------------------------------
            xpr_sint_t sLoadedCount = 0;
            fread(&sLoadedCount, sizeof(xpr_sint_t), 1, sFile);

            if (sLoadedCount <= aMaxCount)
            {
                fread(aAccel, sizeof(ACCEL)*sLoadedCount, 1, sFile);
                *aCount = sLoadedCount;

                sResult = XPR_TRUE;
            }
        }
    }

    fclose(sFile);

    return sResult;
}

xpr_bool_t AccelTable::saveToFile(xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t aCount)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aAccel) || aCount < 0)
        return XPR_FALSE;

    FILE *sFile = _tfopen(aPath, XPR_STRING_LITERAL("wb"));
    if (XPR_IS_NULL(sFile))
        return XPR_FALSE;

    fseek(sFile, 0, SEEK_SET);

    //----------------------------------------------------------------------
    // File Header - 100 Bytes
    //----------------------------------------------------------------------
    FileHeader sFileHeader = {0};
    strcpy(sFileHeader.mProgram,     "flyExplorer");
    strcpy(sFileHeader.mDescription, "AccelKey");
    sFileHeader.mMajorVer     = MAJOR_VER;
    sFileHeader.mMinorVer     = MINOR_VER;
    sFileHeader.mCommpression = 0;
    sFileHeader.mStringMode   = 0;
    sFileHeader.mFileType     = 1;

    fwrite(&sFileHeader, sizeof(FileHeader), 1, sFile);

    //----------------------------------------------------------------------
    // File Body
    //----------------------------------------------------------------------
    fwrite(&aCount, sizeof(xpr_sint_t), 1, sFile);
    fwrite(aAccel, sizeof(ACCEL) * aCount, 1, sFile);

    //----------------------------------------------------------------------
    // File Tail - 4 Bytes
    //----------------------------------------------------------------------
    FileFooter sFileFooter = {0};
    sFileFooter.mEndCode = kEndCode;

    fwrite(&sFileFooter, sizeof(FileFooter), 1, sFile);
    fclose(sFile);

    return XPR_TRUE;
}
