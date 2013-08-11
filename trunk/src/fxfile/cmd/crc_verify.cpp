//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "crc_verify.h"

#include "crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
CrcVerify::CrcVerify(void)
    : mHwnd(XPR_NULL), mMsg(XPR_NULL)
    , mStatus(StatusNone)
{
}

CrcVerify::~CrcVerify(void)
{
    Stop();

    mCrcFileDeque.clear();
}

void CrcVerify::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void CrcVerify::addCrcPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    CrcFile sCrcFile;
    sCrcFile.mPath = aPath;

    mCrcFileDeque.push_back(sCrcFile);
}

void CrcVerify::init(void)
{
    xpr_rcode_t sRcode;
    xpr::FileIo sFileIo;
    xpr_tchar_t *sCode;
    xpr_tchar_t *sFileName;
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    xpr::tstring sCrcDir;
    xpr::tstring sCrcFileName;
    const xpr_size_t sMaxTextLineLen = 1024;
    xpr_tchar_t sTextLine[sMaxTextLineLen + 1];
    xpr_ssize_t sTextReadLen = 0;
    xpr_sint_t sCount2 = 0;
    VerifyFile sVerifyFile;
    const xpr_tchar_t *sWhitespace;

    CrcFileDeque::iterator sIterator;

    sIterator = mCrcFileDeque.begin();
    for (; sIterator != mCrcFileDeque.end(); ++sIterator)
    {
        CrcFile &sCrcFile = *sIterator;

        xpr_size_t sFind = sCrcFile.mPath.rfind(XPR_STRING_LITERAL('\\'));

        sCrcDir      = sCrcFile.mPath.substr(0, sFind);
        sCrcFileName = sCrcFile.mPath.substr(sFind + 1);

        sRcode = sFileIo.open(sCrcFile.mPath.c_str(), xpr::FileIo::OpenModeReadOnly);
        if (XPR_RCODE_IS_ERROR(sRcode))
            continue;

        sVerifyFile.clear();

        // Get CRC CheckSum Method
        {
            xpr::TextFileReader sTextFileReader(sFileIo);

            if (sTextFileReader.detectEncoding() == xpr::CharSetNone)
                sTextFileReader.setEndOfLine(xpr::CharSetMultiBytes);

            sCount2 = 0;

            do
            {
                sRcode = sTextFileReader.readLine(sTextLine, sMaxTextLineLen, &sTextReadLen);
                if (XPR_RCODE_IS_ERROR(sRcode) || sTextReadLen < 0)
                    break;

                if (sCount2 > 20)
                    break;

                if (sTextLine[0] == ';')
                    continue;

                sWhitespace = _tcschr(sTextLine, ' ');
                if (XPR_IS_NOT_NULL(sWhitespace))
                {
                    if ((sWhitespace - sTextLine) == 32)
                        sVerifyFile.mMethod = 1;
                }

                sCount2++;
            } while (true);
        }

        // Get File List, CRC Code
        sFileIo.seekToBegin();

        xpr::TextFileReader sTextFileReader(sFileIo);

        if (sTextFileReader.detectEncoding() == xpr::CharSetMultiBytes)
            sTextFileReader.setEncoding(xpr::CharSetMultiBytes);

        do
        {
            sRcode = sTextFileReader.readLine(sTextLine, sMaxTextLineLen, &sTextReadLen);
            if (XPR_RCODE_IS_ERROR(sRcode) || sTextReadLen < 0)
                break;
            
            if (sTextReadLen < 8)
                continue;

            if (sTextLine[0] == ';')
                continue;

            if (sVerifyFile.mMethod == 0)
            {
                sFileName = sTextLine;
                sFileName[sTextReadLen - 9] = '\0';
                sCode = sTextLine + sTextReadLen - 8;
                sCode[8] = '\0';
            }
            else
            {
                sCode = sTextLine;
                sCode[32] = '\0';
                sFileName = sTextLine + 33;
            }

            sVerifyFile.mPath  = sCrcDir;
            sVerifyFile.mPath += XPR_STRING_LITERAL('\\');
            sVerifyFile.mPath += sFileName;

            sInputBytes = _tcslen(sCode) * sizeof(xpr_tchar_t);
            sOutputBytes = sizeof(CrcVal) - sizeof(xpr_char_t);
            XPR_TCS_TO_MBS(sCode, &sInputBytes, sVerifyFile.mCrcCode, &sOutputBytes);
            sVerifyFile.mCrcCode[sOutputBytes / sizeof(xpr_char_t)] = 0;

            sCrcFile.mVerifyFileDeque.push_back(sVerifyFile);
        } while (true);

        sFileIo.close();
    }
}

xpr_size_t CrcVerify::getCrcCount(void)
{
    return mCrcFileDeque.size();
}

const xpr_tchar_t *CrcVerify::getCrcPath(xpr_size_t aCrc)
{
    if (!FXFILE_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    return mCrcFileDeque[aCrc].mPath.c_str();
}

xpr_size_t CrcVerify::getVerifyCount(xpr_size_t aCrc)
{
    if (!FXFILE_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    return mCrcFileDeque[aCrc].mVerifyFileDeque.size();
}

CrcVerify::VerifyFile *CrcVerify::getVerifyFile(xpr_size_t aCrc, xpr_size_t aVerify)
{
    if (!FXFILE_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    if (!FXFILE_STL_IS_INDEXABLE(aVerify, mCrcFileDeque[aCrc].mVerifyFileDeque))
        return XPR_NULL;

    return &mCrcFileDeque[aCrc].mVerifyFileDeque[aVerify];
}

void CrcVerify::resetVerifyResult(void)
{
    CrcFileDeque::iterator sIterator;
    VerifyFileDeque::iterator sVerifyFileIterator;

    sIterator = mCrcFileDeque.begin();
    for (; sIterator != mCrcFileDeque.end(); ++sIterator)
    {
        CrcFile &sCrcFile = *sIterator;

        sVerifyFileIterator = sCrcFile.mVerifyFileDeque.begin();
        for (; sVerifyFileIterator != sCrcFile.mVerifyFileDeque.end(); ++sVerifyFileIterator)
            sVerifyFileIterator->mResult = VerifyResultNone;
    }
}

xpr_bool_t CrcVerify::OnPreEntry(void) 
{
    resetVerifyResult();

    mStatus = StatusVerifying;

    return XPR_TRUE;
}

unsigned CrcVerify::OnEntryProc(void)
{
    xpr::tstring sFile;
    xpr_char_t *sCode;
    xpr_sint_t sMethod;
    xpr_rcode_t sRcode;
    VerifyResult sResult;

    xpr_char_t sCrcValue[100];

    CrcFileDeque::iterator sCrcIterator;
    VerifyFileDeque::iterator sFileIterator;

    sCrcIterator = mCrcFileDeque.begin();
    for (; sCrcIterator != mCrcFileDeque.end(); ++sCrcIterator)
    {
        CrcFile &sCrcFile = *sCrcIterator;

        sFileIterator = sCrcFile.mVerifyFileDeque.begin();
        for (; sFileIterator != sCrcFile.mVerifyFileDeque.end(); ++sFileIterator)
        {
            if (IsStop() == XPR_TRUE)
                break;

            sFile   = sFileIterator->mPath;
            sCode   = sFileIterator->mCrcCode;
            sMethod = sFileIterator->mMethod;
            sResult = VerifyResultNone;

            if (sMethod == 0)
            {
                sRcode = getFileCrcSfv(sFile.c_str(), sCrcValue);
            }
            else
            {
                sRcode = getFileCrcMd5(sFile.c_str(), sCrcValue);
            }

            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                sResult = VerifyResultFailed;
            }
            else
            {
                sResult = VerifyResultEqualed;

                if (_stricmp(sCode, sCrcValue))
                {
                    sResult = VerifyResultNotEqualed;
                }
            }

            sFileIterator->mResult = sResult;
        }

        if (IsStop() == XPR_TRUE)
            break;
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = IsStop() ? StatusStopped : StatusVerifyCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)mStatus, (LPARAM)XPR_NULL);

    return 0;
}

CrcVerify::Status CrcVerify::getStatus(void)
{
    xpr::MutexGuard sLockGuard(mMutex);

    return mStatus;
}

xpr_sint_t CrcVerify::getResult(xpr_size_t *aProcessedCount, xpr_size_t *aNotEqualedCount, xpr_size_t *aFailedCount)
{
    xpr_sint_t sCount = 0;
    xpr_sint_t sProcessedCount = 0;
    xpr_sint_t sNotEqualedCount = 0;
    xpr_sint_t sFailedCount = 0;

    CrcFileDeque::iterator sIterator;
    VerifyFileDeque::iterator sVerifyFileIterator;

    sIterator = mCrcFileDeque.begin();
    for (; sIterator != mCrcFileDeque.end(); ++sIterator)
    {
        CrcFile &sCrcFile = *sIterator;

        sVerifyFileIterator = sCrcFile.mVerifyFileDeque.begin();
        for (; sVerifyFileIterator != sCrcFile.mVerifyFileDeque.end(); ++sVerifyFileIterator)
        {
            switch (sVerifyFileIterator->mResult)
            {
            case VerifyResultFailed:
            case VerifyResultNotEqualed:
            case VerifyResultEqualed:
                {
                    switch (sVerifyFileIterator->mResult)
                    {
                    case VerifyResultFailed:     sFailedCount++;     break;
                    case VerifyResultNotEqualed: sNotEqualedCount++; break;
                    }

                    sProcessedCount++;
                }
            }

            sCount++;
        }
    }

    if (XPR_IS_NOT_NULL(aProcessedCount )) *aProcessedCount  = sProcessedCount;
    if (XPR_IS_NOT_NULL(aNotEqualedCount)) *aNotEqualedCount = sNotEqualedCount;
    if (XPR_IS_NOT_NULL(aFailedCount    )) *aFailedCount     = sFailedCount;

    return sCount;
}
} // namespace cmd
} // namespace fxfile
