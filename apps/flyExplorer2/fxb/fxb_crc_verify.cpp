//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_crc_verify.h"

#include "fxb_crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
#define BUFFER_SIZE 260

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
    FILE *sFile;
    xpr_char_t *sCode;
    xpr_char_t *sFileNameA;
    xpr_tchar_t sFileName[XPR_MAX_PATH + 1];
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    std::tstring sCrcDir;
    std::tstring sCrcFileName;
    xpr_char_t sBuffer[BUFFER_SIZE + 1];
    xpr_sint_t sLen, sCount2 = 0;
    VerifyFile sVerifyFile;
    const xpr_char_t *sWhitespace;

    CrcFileDeque::iterator sIterator;

    sIterator = mCrcFileDeque.begin();
    for (; sIterator != mCrcFileDeque.end(); ++sIterator)
    {
        CrcFile &sCrcFile = *sIterator;

        xpr_size_t sFind = sCrcFile.mPath.rfind(XPR_STRING_LITERAL('\\'));

        sCrcDir      = sCrcFile.mPath.substr(0, sFind);
        sCrcFileName = sCrcFile.mPath.substr(sFind + 1);

        sFile = _tfopen(sCrcFile.mPath.c_str(), XPR_STRING_LITERAL("rt"));
        sVerifyFile.clear();

        // Get CRC CheckSum Method
        sCount2 = 0;
        while (fgets(sBuffer, BUFFER_SIZE, sFile) != XPR_NULL)
        {
            if (sCount2 > 20)
                break;

            if (sBuffer[0] == ';')
                continue;

            sLen = (xpr_sint_t)strlen(sBuffer);

            sWhitespace = strchr(sBuffer, ' ');
            if (XPR_IS_NOT_NULL(sWhitespace))
            {
                if ((sWhitespace - sBuffer) == 32)
                    sVerifyFile.mMethod = 1;
            }

            sCount2++;
        }

        // Get File List, CRC Code
        fseek(sFile, 0, SEEK_SET);
        while (fgets(sBuffer, BUFFER_SIZE, sFile) != XPR_NULL)
        {
            if (strlen(sBuffer) < 8)
                continue;

            if (sBuffer[0] == ';')
                continue;

            if (sVerifyFile.mMethod == 0)
            {
                sLen = (xpr_sint_t)strlen(sBuffer);
                sFileNameA = sBuffer;
                sFileNameA[sLen-10] = '\0';
                sCode = sBuffer + sLen - 9;
                sCode[8] = '\0';
            }
            else
            {
                sCode = sBuffer;
                sCode[32] = '\0';
                sFileNameA = sBuffer + 33;
                sLen = (xpr_sint_t)strlen(sFileNameA);
                sFileNameA[sLen - 1] = '\0';
            }

            sInputBytes = strlen(sFileNameA) * sizeof(xpr_char_t);
            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sFileNameA, &sInputBytes, sFileName, &sOutputBytes);
            sFileName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sVerifyFile.mPath  = sCrcDir;
            sVerifyFile.mPath += XPR_STRING_LITERAL('\\');
            sVerifyFile.mPath += sFileName;

            strcpy(sVerifyFile.mCrcCode, sCode);

            sCrcFile.mVerifyFileDeque.push_back(sVerifyFile);
        }

        fclose(sFile);
    }
}

xpr_size_t CrcVerify::getCrcCount(void)
{
    return mCrcFileDeque.size();
}

const xpr_tchar_t *CrcVerify::getCrcPath(xpr_size_t aCrc)
{
    if (!XPR_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    return mCrcFileDeque[aCrc].mPath.c_str();
}

xpr_size_t CrcVerify::getVerifyCount(xpr_size_t aCrc)
{
    if (!XPR_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    return mCrcFileDeque[aCrc].mVerifyFileDeque.size();
}

CrcVerify::VerifyFile *CrcVerify::getVerifyFile(xpr_size_t aCrc, xpr_size_t aVerify)
{
    if (!XPR_STL_IS_INDEXABLE(aCrc, mCrcFileDeque))
        return XPR_NULL;

    if (!XPR_STL_IS_INDEXABLE(aVerify, mCrcFileDeque[aCrc].mVerifyFileDeque))
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
    std::tstring sFile;
    xpr_char_t *sCode;
    xpr_sint_t sMethod;
    xpr_sint_t sCrcResult;
    VerifyResult sResult;

    xpr_ulong_t sCrcVal;
    xpr_char_t sCrcValHex[100];

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
                sCrcVal = 0;
                sCrcResult = getFileCrcSfv(sFile.c_str(), &sCrcVal);
                sprintf(sCrcValHex, "%08x", sCrcVal);
            }
            else
            {
                sCrcResult = getFileCrcMd5(sFile.c_str(), sCrcValHex);
            }

            if (sCrcResult == CRC_ERR_FILE_READ || sCrcResult == CRC_ERR_READ_LENGTH)
            {
                sResult = VerifyResultFailed;
            }
            else
            {
                sResult = VerifyResultEqualed;

                if (_stricmp(sCode, sCrcValHex))
                    sResult = VerifyResultNotEqualed;
            }

            sFileIterator->mResult = sResult;
        }

        if (IsStop() == XPR_TRUE)
            break;
    }

    {
        CsLocker sLocker(mCs);
        mStatus = IsStop() ? StatusStopped : StatusVerifyCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)mStatus, (LPARAM)XPR_NULL);

    return 0;
}

CrcVerify::Status CrcVerify::getStatus(void)
{
    CsLocker sLocker(mCs);

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
} // namespace fxb
