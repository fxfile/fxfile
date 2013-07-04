//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_combine.h"

#include "conf_file_ex.h"
#include "crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
static const xpr_size_t kDefaultBufferSize = 16 * 1024; // 16KB

static const xpr_tchar_t kFileNameKey[] = XPR_STRING_LITERAL("filename");
static const xpr_tchar_t kSizeKey    [] = XPR_STRING_LITERAL("size");
static const xpr_tchar_t kCrc32Key   [] = XPR_STRING_LITERAL("crc32");

FileCombine::FileCombine(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mBufferSize(kDefaultBufferSize)
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
        xpr::tstring sBasePath = mPath;
        sBasePath.erase(sBasePath.length()-_tcslen(sExt));

        xpr::tstring sCrcFile = sBasePath;
        sCrcFile += XPR_STRING_LITERAL(".crc");

        xpr::tstring sFileName = sBasePath;
        sFileName = sFileName.substr(sFileName.rfind(XPR_STRING_LITERAL('\\'))+1);

        xpr::tstring sDestPath;
        sDestPath = mDestDir + XPR_STRING_LITERAL('\\') + sFileName;

        xpr_rcode_t sRcode;
        xpr::FileIo sFileIo;
        xpr::FileIo sDestFileIo;
        xpr_sint_t sOpenMode;
        xpr_char_t *sBuffer;
        xpr_ssize_t sRead;
        xpr_ssize_t sWritten;
        xpr_tchar_t sIndexText[0xff] = {0};
        xpr::tstring sPath;
        xpr_size_t sInputBytes;
        xpr_size_t sOutputBytes;

        sBuffer = new xpr_char_t[mBufferSize];

        sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        sRcode = sDestFileIo.open(sDestPath.c_str(), sOpenMode);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            for (; IsStop() == XPR_FALSE; ++sIndex)
            {
                _stprintf(sIndexText, XPR_STRING_LITERAL(".%03d"), sIndex);
                sPath = sBasePath + sIndexText;

                sRcode = sFileIo.open(sPath.c_str(), xpr::FileIo::OpenModeReadOnly);
                if (XPR_RCODE_IS_ERROR(sRcode))
                    break;

                {
                    xpr::MutexGuard sLockGuard(mMutex);
                    mCombinedCount++;
                }

                while (IsStop() == XPR_FALSE)
                {
                    sRcode = sFileIo.read(sBuffer, mBufferSize, &sRead);
                    if (XPR_RCODE_IS_ERROR(sRcode) || sRead == 0)
                        break;

                    sRcode = sDestFileIo.write(sBuffer, sRead, &sWritten);
                    if (XPR_RCODE_IS_ERROR(sRcode))
                        break;
                }

                sFileIo.close();
            }

            sDestFileIo.close();

            sStatus = StatusCombineCompleted;

            // crc checking
            if (IsStop() == XPR_FALSE && IsExistFile(sCrcFile.c_str()) == XPR_TRUE)
            {
                fxfile::base::ConfFileEx sConfFile(sCrcFile.c_str());
                sConfFile.load();

                const xpr_tchar_t *sTargetFileName = sConfFile.getValueS(XPR_STRING_LITERAL(""), kFileNameKey, XPR_NULL);
                const xpr_tchar_t *sFileSize       = sConfFile.getValueS(XPR_STRING_LITERAL(""), kSizeKey,     XPR_NULL);
                const xpr_tchar_t *sCrcCode        = sConfFile.getValueS(XPR_STRING_LITERAL(""), kCrc32Key,    XPR_NULL);

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

                        sInputBytes = _tcslen(sCrcCode) * sizeof(xpr_tchar_t);
                        sOutputBytes = 0xff * sizeof(xpr_char_t);
                        XPR_TCS_TO_MBS(sCrcCode, &sInputBytes, sCrcCode1, &sOutputBytes);
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
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = sStatus;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileCombine::Status FileCombine::GetStatus(xpr_size_t *aCombinedCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aCombinedCount)) *aCombinedCount = mCombinedCount;

    return mStatus;
}
} // namespace cmd
} // namespace fxfile
