//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_split.h"

#include "crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
const xpr_size_t kDefaultBufferSize = 16 * 1024; // 16KB
} // namespace anonymous

FileSplit::FileSplit(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mSplitedCount(0)
    , mFlags(0)
    , mUnitSize(0i64)
    , mBufferSize(kDefaultBufferSize)
{
}

FileSplit::~FileSplit(void)
{
}

void FileSplit::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void FileSplit::setPath(const xpr_tchar_t *aPath, const xpr_tchar_t *aDestDir)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aDestDir))
        return;

    mPath = aPath;
    mDestDir = aDestDir;

    if (mDestDir[mDestDir.length()-1] == XPR_STRING_LITERAL('\\'))
        mDestDir.erase(mDestDir.length()-1);
}

xpr_uint_t FileSplit::getFlags(void)
{
    return mFlags;
}

void FileSplit::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

void FileSplit::setSplitSize(xpr_sint64_t aUnitSize)
{
    mUnitSize = aUnitSize;
}

void FileSplit::setBufferSize(xpr_size_t aBufferSize)
{
    mBufferSize = aBufferSize;
}

xpr_bool_t FileSplit::start(void)
{
    if (mUnitSize <= 10)
        mUnitSize = 10;

    if (IsExistFile(mPath) == XPR_FALSE)
    {
        setStatus(StatusFileNotExist);
        return XPR_FALSE;
    }

    if (IsExistFile(mDestDir) == XPR_FALSE)
    {
        setStatus(StatusDestDirNotExist);
        return XPR_FALSE;
    }

    mSplitedCount = 0;
    setStatus(StatusSplitting);

    xpr_rcode_t sRcode = mThread.start(dynamic_cast<xpr::Thread::Runnable *>(this));

    return XPR_RCODE_IS_SUCCESS(sRcode);
}

void FileSplit::stop(void)
{
    mThread.stop();
    mThread.join();
}

xpr_sint_t FileSplit::runThread(xpr::Thread &aThread)
{
    xpr_char_t *sCrcCode = XPR_NULL;
    if (XPR_TEST_BITS(mFlags, FlagCrcFile))
    {
        sCrcCode = new xpr_char_t[0xff];
        sCrcCode[0] = '\0';

        getFileCrcSfv(mPath.c_str(), sCrcCode);
    }

    Status sStatus = StatusNone;

    xpr_rcode_t sRcode;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(mPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        sStatus = StatusNotReadable;
    }
    else
    {
        xpr_uint64_t sFileSize = GetFileSize(mPath);

        xpr_ssize_t sRead;
        xpr_byte_t *sBuffer;
        xpr_sint_t sOpenMode;
        xpr::FileIo sTargetFileIo;
        xpr::string sFileName;

        xpr_sint_t sIndex;
        xpr_tchar_t sPath[XPR_MAX_PATH + 1];

        xpr_sint64_t i;
        xpr_sint64_t sCount;
        xpr_sint64_t sWritedFileSize;
        xpr_size_t sSize;
        xpr_size_t sWritedBuffer;
        xpr_ssize_t sWrite;

        sBuffer = new xpr_byte_t[mBufferSize];
        sFileName = mPath.substr(mPath.rfind(XPR_STRING_LITERAL('\\'))+1);
        sIndex = 1;

        while (mThread.isStop() == XPR_FALSE)
        {
            sRcode = sFileIo.read(sBuffer, mBufferSize, &sRead);
            if (XPR_RCODE_IS_ERROR(sRcode) || sRead == 0)
                break;

            sWritedBuffer = 0;

            do
            {
                if (sTargetFileIo.isOpened() == XPR_FALSE)
                    sWritedFileSize = 0;

                if (sTargetFileIo.isOpened() == XPR_FALSE)
                {
                    _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.%03d"), mDestDir.c_str(), sFileName.c_str(), sIndex++);

                    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
                    sRcode = sTargetFileIo.open(sPath, sOpenMode);

                    if (XPR_RCODE_IS_SUCCESS(sRcode))
                    {
                        xpr::MutexGuard sLockGuard(mMutex);
                        mSplitedCount++;
                    }
                }

                if (sTargetFileIo.isOpened() == XPR_FALSE)
                    break;

                sSize = sRead - sWritedBuffer;
                if ((xpr_sint64_t)sSize > mUnitSize)
                    sSize = (xpr_size_t)mUnitSize;

                sCount = (xpr_sint64_t)sSize / mUnitSize;
                if ((xpr_sint64_t)sSize % mUnitSize)
                    sCount++;

                for (i = 0; i < sCount; ++i)
                {
                    if ((mUnitSize - sWritedFileSize) < (xpr_sint64_t)sSize)
                        sSize = (xpr_size_t)(mUnitSize - sWritedFileSize);

                    sRcode = sTargetFileIo.write(sBuffer + sWritedBuffer, sSize, &sWrite);
                    if (XPR_RCODE_IS_ERROR(sRcode))
                        break;

                    sWritedBuffer += sWrite;
                    sWritedFileSize += sWrite;
                    if (sWritedFileSize == mUnitSize)
                        break;

                    if (sWritedBuffer == sRead)
                        break;
                }

                if (sWritedFileSize == mUnitSize)
                {
                    sTargetFileIo.close();
                }

                if (sWritedBuffer == sRead)
                    break;

            } while (mThread.isStop() == XPR_FALSE);

            if (XPR_RCODE_IS_ERROR(sRcode))
                break;
        }

        if (sTargetFileIo.isOpened() == XPR_TRUE)
        {
            sTargetFileIo.close();
        }

        sFileIo.close();

        if (mThread.isStop() == XPR_FALSE && XPR_IS_NOT_NULL(sCrcCode))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.crc"), mDestDir.c_str(), sFileName.c_str());

            xpr_tchar_t sCrcCodeString[0xff] = {0};
            xpr_size_t sInputBytes;
            xpr_size_t sOutputBytes;

            sInputBytes = strlen(sCrcCode) * sizeof(xpr_char_t);
            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sCrcCode, &sInputBytes, sCrcCodeString, &sOutputBytes);
            sCrcCodeString[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            xpr::FileIo sCrcFileIo;

            sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
            sRcode = sCrcFileIo.open(sPath, sOpenMode);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                xpr::TextFileWriter sTextFileWriter(sCrcFileIo);

                sTextFileWriter.setEncoding((sizeof(xpr_tchar_t) == 2) ? xpr::CharSetUtf16 : xpr::CharSetMultiBytes);
                sTextFileWriter.setEndOfLine(xpr::TextFileWriter::kUnixStyle);
                sTextFileWriter.writeBom();

                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("filename=%s"), sFileName.c_str());
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("size=%I64u"), sFileSize);
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("crc32=%s"), sCrcCodeString);

                sFileIo.close();
            }
        }

        if (mThread.isStop() == XPR_FALSE && XPR_TEST_BITS(mFlags, FlagBatFile))
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s.bat"), mDestDir.c_str(), sFileName.c_str());

            xpr::FileIo sBatFileIo;

            sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
            sRcode = sBatFileIo.open(sPath, sOpenMode);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                xpr::TextFileWriter sTextFileWriter(sBatFileIo);

                sTextFileWriter.setEncoding(xpr::CharSetMultiBytes);
                sTextFileWriter.setEndOfLine(xpr::TextFileWriter::kUnixStyle);
                sTextFileWriter.writeBom();

                const xpr_tchar_t *sTempFile = XPR_STRING_LITERAL("fxCombine.tmp");

                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("@echo off"));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo."));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo Created by fxfile (http://www.flychk.com)"));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo."));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo File Combine..."));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo."));
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("copy /b \"%s.001\" \"%s\""), sFileName.c_str(), sTempFile);

                xpr_size_t j;
                for (j = 2; j <= mSplitedCount; ++j)
                    sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("copy /b \"%s\" + \"%s.%03d\" \"%s\""), sTempFile, sFileName.c_str(), j, sTempFile);

                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("ren \"%s\" \"%s\""), sTempFile, sFileName.c_str());
                sTextFileWriter.writeFormatLine(XPR_STRING_LITERAL("echo Done..."));

                sBatFileIo.close();
            }
        }

        if (mThread.isStop() == XPR_FALSE && XPR_TEST_BITS(mFlags, FlagDelOrgFile))
        {
            ::DeleteFile(mPath.c_str());
        }

        XPR_SAFE_DELETE_ARRAY(sBuffer);
        XPR_SAFE_DELETE_ARRAY(sCrcCode);

        sStatus = mThread.isStop() ? StatusStopped : StatusSplitCompleted;
    }

    setStatus(sStatus);

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileSplit::Status FileSplit::getStatus(xpr_size_t *aSplitedCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aSplitedCount)) *aSplitedCount = mSplitedCount;

    return mStatus;
}

void FileSplit::setStatus(Status aStatus)
{
    xpr::MutexGuard sLockGuard(mMutex);
    mStatus = aStatus;
}
} // namespace cmd
} // namespace fxfile
