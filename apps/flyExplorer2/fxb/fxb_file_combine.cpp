//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_combine.h"

#include "fxb_ini_file.h"
#include "fxb_crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
#define DEFAULT_BUFFER_SIZE (16*1024)

FileCombine::FileCombine(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mBufferSize(DEFAULT_BUFFER_SIZE)
    , mCombinedCount(0)
{
}

FileCombine::~FileCombine(void)
{
}

void FileCombine::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void FileCombine::setPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
       mPath = aPath;
}

void FileCombine::setDestDir(const xpr_tchar_t *aDestDir)
{
    if (XPR_IS_NULL(aDestDir))
        return;

    mDestDir = aDestDir;

    if (mDestDir[mDestDir.length()-1] == XPR_STRING_LITERAL('\\'))
        mDestDir.erase(mDestDir.length()-1);
}

void FileCombine::setBufferSize(DWORD aBufferSize)
{
    mBufferSize = aBufferSize;
}

xpr_bool_t FileCombine::OnPreEntry(void)
{
    if (IsExistFile(mPath) == XPR_FALSE)
    {
        mStatus = StatusFileNotExist;
        return XPR_FALSE;
    }

    if (IsExistFile(mDestDir) == XPR_FALSE)
    {
        mStatus = StatusDestDirNotExist;
        return XPR_FALSE;
    }

    mStatus = StatusCombining;
    mCombinedCount = 0;

    return XPR_TRUE;
}

unsigned FileCombine::OnEntryProc(void)
{
    Status sStatus = StatusNone;

    xpr_sint_t sIndex = -1;

    const xpr_tchar_t *sExt = GetFileExt(mPath);
    if (XPR_IS_NOT_NULL(sExt))
        _stscanf(sExt+1, XPR_STRING_LITERAL("%d"), &sIndex);

    if (sIndex >= 0)
    {
        std::tstring sBasePath = mPath;
        sBasePath.erase(sBasePath.length()-_tcslen(sExt));

        std::tstring sCrcFile = sBasePath;
        sCrcFile += XPR_STRING_LITERAL(".crc");

        std::tstring sFileName = sBasePath;
        sFileName = sFileName.substr(sFileName.rfind(XPR_STRING_LITERAL('\\'))+1);

        std::tstring sDestPath;
        sDestPath = mDestDir + XPR_STRING_LITERAL('\\') + sFileName;

        FILE *sFile, *sDestFile;
        xpr_char_t *sBuffer;
        xpr_size_t sRead;
        xpr_tchar_t sIndexText[0xff] = {0};
        std::tstring sPath;
        xpr_size_t sOutputBytes;

        sBuffer = new xpr_char_t[mBufferSize];

        sDestFile = _tfopen(sDestPath.c_str(), XPR_STRING_LITERAL("wb"));
        if (XPR_IS_NOT_NULL(sDestFile))
        {
            for (; IsStop() == XPR_FALSE; ++sIndex)
            {
                _stprintf(sIndexText, XPR_STRING_LITERAL(".%03d"), sIndex);
                sPath = sBasePath + sIndexText;

                sFile = _tfopen(sPath.c_str(), XPR_STRING_LITERAL("rb"));
                if (XPR_IS_NULL(sFile))
                    break;

                {
                    CsLocker sLocker(mCs);
                    mCombinedCount++;
                }

                while (IsStop() == XPR_FALSE)
                {
                    sRead = fread(sBuffer, 1, mBufferSize, sFile);
                    if (sRead == 0)
                        break;

                    fwrite(sBuffer, sRead, 1, sDestFile);
                }

                fclose(sFile);
            }

            fclose(sDestFile);

            sStatus = StatusCombineCompleted;

            // crc checking
            if (IsStop() == XPR_FALSE && IsExistFile(sCrcFile.c_str()) == XPR_TRUE)
            {
                IniFile sIniFile(sCrcFile.c_str());
                sIniFile.readFileA();

                const xpr_tchar_t *sTargetFileName = sIniFile.getValueS(XPR_STRING_LITERAL(""), XPR_STRING_LITERAL("filename"), XPR_NULL);
                const xpr_tchar_t *sFileSize       = sIniFile.getValueS(XPR_STRING_LITERAL(""), XPR_STRING_LITERAL("size"),     XPR_NULL);
                const xpr_tchar_t *sCrcCode        = sIniFile.getValueS(XPR_STRING_LITERAL(""), XPR_STRING_LITERAL("crc32"),    XPR_NULL);

                xpr_bool_t sCorrectCrcCode = XPR_FALSE;

                if (XPR_IS_NOT_NULL(sTargetFileName) && XPR_IS_NOT_NULL(sFileSize) && XPR_IS_NOT_NULL(sCrcCode))
                {
                    const xpr_tchar_t *sFileName1 = sTargetFileName;
                    const xpr_tchar_t *sFileName2 = sFileName.c_str();

                    xpr_uint64_t sFileSize1 = 0ui64;
                    xpr_uint64_t sFileSize2 = 0ui64;

                    _stscanf(sFileSize, XPR_STRING_LITERAL("%I64u"), &sFileSize1);
                    sFileSize2 = GetFileSize(sDestPath);

                    if (_tcsicmp(sFileName1, sFileName2) == 0 && sFileSize1 == sFileSize2)
                    {
                        xpr_char_t sCrcCode1[0xff] = {0};
                        xpr_char_t sCrcCode2[0xff] = {0};

                        sOutputBytes = 0xff * sizeof(xpr_char_t);
                        XPR_TCS_TO_MBS(sCrcCode, _tcslen(sCrcCode) * sizeof(xpr_tchar_t), sCrcCode1, &sOutputBytes);
                        sCrcCode1[sOutputBytes / sizeof(xpr_char_t)] = 0;

                        sCrcCode2[0] = XPR_STRING_LITERAL('\0');
                        getFileCrcSfv(sDestPath.c_str(), sCrcCode2);

                        if (_stricmp(sCrcCode1, sCrcCode2) == 0)
                            sCorrectCrcCode = XPR_TRUE;
                    }
                }

                if (XPR_IS_FALSE(sCorrectCrcCode))
                    sStatus = StatusInvalidCrcCode;
            }
        }
        else
        {
            sStatus = StatusNotWritable;
        }

        XPR_SAFE_DELETE_ARRAY(sBuffer);
    }
    else
    {
        sStatus = StatusNotCombine;
    }

    if (IsStop() == XPR_TRUE)
        sStatus = StatusStopped;

    {
        CsLocker sLocker(mCs);
        mStatus = sStatus;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileCombine::Status FileCombine::GetStatus(xpr_size_t *aCombinedCount)
{
    CsLocker sLocker(mCs);

    if (XPR_IS_NOT_NULL(aCombinedCount)) *aCombinedCount = mCombinedCount;

    return mStatus;
}
} // namespace fxb
