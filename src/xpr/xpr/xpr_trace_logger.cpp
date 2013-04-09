//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_trace_logger.h"
#include "xpr_file_sys.h"
#include "xpr_text_file_io.h"
#include "xpr_system.h"
#include "xpr_debug.h"
#include "xpr_string.h"
#include "xpr_wide_string.h"
#include "xpr_bit.h"
#include "xpr_time.h"
#include "xpr_process.h"
#include "xpr_thread.h"
#include "xpr_detail_cross_string.h"

namespace xpr
{
static const xpr_sint_t kDefLevel     = TraceLogger::kLogLevelInfo;
static const xpr_uint_t kDefFlags     = TraceLogger::kFlagTraceOut;
static const xpr_sint_t kDefFileSize  = 10 * 1024 * 1024; // 10MB
static const xpr_sint_t kDefFileCount = 10;               // 10 (10MB x 10 = 100MB)

static const xpr_char_t *kLogLevelText[] = {
    XPR_MBCS_STRING_LITERAL("NONE"),
    XPR_MBCS_STRING_LITERAL("FATAL"),
    XPR_MBCS_STRING_LITERAL("ERROR"),
    XPR_MBCS_STRING_LITERAL("WARN"),
    XPR_MBCS_STRING_LITERAL("INFO"),
    XPR_MBCS_STRING_LITERAL("DEBUG"),
    XPR_MBCS_STRING_LITERAL("TRACE"),
};

static const xpr_wchar_t *kLogLevelTextW[] = {
    XPR_WIDE_STRING_LITERAL("NONE"),
    XPR_WIDE_STRING_LITERAL("FATAL"),
    XPR_WIDE_STRING_LITERAL("ERROR"),
    XPR_WIDE_STRING_LITERAL("WARN"),
    XPR_WIDE_STRING_LITERAL("INFO"),
    XPR_WIDE_STRING_LITERAL("DEBUG"),
    XPR_WIDE_STRING_LITERAL("TRACE"),
};

struct TraceLogger::LogRecord
{
    const void *mMessage;
    xpr_size_t  mMessageBytes;
    xpr_bool_t  mWideChar;
};

TraceLogger::TraceLogger(void)
    : mLevel(kDefLevel)
    , mFlags(kDefFlags)
    , mFileSize(kDefFileSize), mFileCount(kDefFileCount), mFileIndex(0)
    , mFileDir(XPR_NULL), mFileName(XPR_NULL), mFilePath(XPR_NULL)
    , mTextFileWriter(XPR_NULL)
    , mCharSet(CharSetNone), mWriteBom(XPR_FALSE)
    , mStarted(XPR_FALSE)
{
}

TraceLogger::~TraceLogger(void)
{
    stop();

    XPR_SAFE_DELETE(mFileDir);
    XPR_SAFE_DELETE(mFileName);
    XPR_SAFE_DELETE(mFilePath);
}

xpr_sint_t TraceLogger::getLevel(void) const
{
    return mLevel;
}

xpr_uint_t TraceLogger::getFlags(void) const
{
    return mFlags;
}

xpr_bool_t TraceLogger::hasFlag(xpr_uint_t aFlag) const
{
    return XPR_TEST_BITS(mFlags, aFlag);
}

xpr_bool_t TraceLogger::getFileDir(xpr_char_t *aFileDir, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(mFileDir))
    {
        return XPR_FALSE;
    }

    return mFileDir->getString(aFileDir, aMaxLen);
}

xpr_bool_t TraceLogger::getFileDir(xpr_wchar_t *aFileDir, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(mFileDir))
    {
        return XPR_FALSE;
    }

    return mFileDir->getString(aFileDir, aMaxLen);
}

xpr_bool_t TraceLogger::getFileName(xpr_char_t *aFileName, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(mFileName))
    {
        return XPR_FALSE;
    }

    return mFileDir->getString(aFileName, aMaxLen);
}

xpr_bool_t TraceLogger::getFileName(xpr_wchar_t *aFileName, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(mFileName))
    {
        return XPR_FALSE;
    }

    return mFileDir->getString(aFileName, aMaxLen);
}

xpr_size_t TraceLogger::getFileSize(void) const
{
    return mFileSize;
}

xpr_size_t TraceLogger::getFileCount(void) const
{
    return mFileCount;
}

CharSet TraceLogger::getEncoding(void) const
{
    return mCharSet;
}

xpr_bool_t TraceLogger::isWriteBom(void) const
{
    return mWriteBom;
}

xpr_bool_t TraceLogger::isCircular(void) const
{
    if (XPR_IS_FALSE(hasFlag(kFlagCircular)))
    {
        return XPR_FALSE;
    }

    return (mFileCount >= 2) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t TraceLogger::setLevel(xpr_sint_t aLevel)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mLevel = aLevel;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFlags(xpr_uint_t aFlags)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mFlags = aFlags;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileDir(const xpr_char_t *aFileDir)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    detail::CrossString *sFileDir = new detail::CrossString;
    if (XPR_IS_NULL(sFileDir))
    {
        return XPR_FALSE;
    }

    xpr_rcode_t sRcode = sFileDir->setString(aFileDir);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    // remove, if last character is file separator
    xpr_char_t *sSplit = strrchr(sFileDir->mStringA, XPR_FILE_SEPARATOR);
    if (XPR_IS_NOT_NULL(sSplit))
    {
        *sSplit = 0;
    }

    XPR_SAFE_DELETE(mFileDir);

    mFileDir = sFileDir;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileDir(const xpr_wchar_t *aFileDir)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    detail::CrossString *sFileDir = new detail::CrossString;
    if (XPR_IS_NULL(sFileDir))
    {
        return XPR_FALSE;
    }

    xpr_rcode_t sRcode = sFileDir->setString(aFileDir);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    // remove, if last character is file separator
    xpr_wchar_t *sSplit = wcsrchr(sFileDir->mStringW, XPR_FILE_SEPARATOR);
    if (XPR_IS_NOT_NULL(sSplit))
    {
        *sSplit = 0;
    }

    XPR_SAFE_DELETE(mFileDir);

    mFileDir = sFileDir;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileName(const xpr_char_t *aFileName)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    detail::CrossString *sFileName = new detail::CrossString;
    if (XPR_IS_NULL(sFileName))
    {
        return XPR_FALSE;
    }

    xpr_rcode_t sRcode = sFileName->setString(aFileName);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    XPR_SAFE_DELETE(mFileName);

    mFileName = sFileName;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileName(const xpr_wchar_t *aFileName)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    detail::CrossString *sFileName = new detail::CrossString;
    if (XPR_IS_NULL(sFileName))
    {
        return XPR_FALSE;
    }

    xpr_rcode_t sRcode = sFileName->setString(aFileName);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return XPR_FALSE;
    }

    XPR_SAFE_DELETE(mFileName);

    mFileName = sFileName;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileSize(xpr_sint_t aFileSize)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mFileSize = aFileSize;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileCount(xpr_sint_t aFileCount)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mFileCount = aFileCount;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setEncoding(CharSet aCharSet, xpr_bool_t aWriteBom)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mCharSet = aCharSet;
    mWriteBom = aWriteBom;

    return XPR_TRUE;
}

xpr_rcode_t TraceLogger::start(void)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_RCODE_SUCCESS;
    }

    if (XPR_IS_NULL(mFileName))
    {
        return XPR_RCODE_ENOENT;
    }

    detail::CrossString *sFilePath = new detail::CrossString;
    if (XPR_IS_NULL(sFilePath))
    {
        return XPR_RCODE_ENOMEM;
    }

    if ((XPR_IS_NOT_NULL(mFileDir) && XPR_IS_TRUE(mFileDir->mWideChar)) || (XPR_IS_TRUE(mFileName->mWideChar)))
    {
        xpr_size_t  sLen = 0;
        xpr_wchar_t sString[XPR_MAX_PATH + 1] = {0};

        if (XPR_IS_NOT_NULL(mFileDir))
        {
            mFileDir->getString(sString, XPR_MAX_PATH);

            sLen = wcslen(sString);
            if (sLen > 0)
            {
                sString[sLen] = XPR_FILE_SEPARATOR;
                ++sLen;
            }
        }

        mFileName->getString(sString + sLen, XPR_MAX_PATH - sLen);

        sFilePath->setString(sString);
    }
    else
    {
        xpr_size_t sLen = 0;
        xpr_char_t sString[XPR_MAX_PATH + 1] = {0};

        if (XPR_IS_NOT_NULL(mFileDir))
        {
            mFileDir->getString(sString, XPR_MAX_PATH);

            sLen = strlen(sString);
            if (sLen > 0)
            {
                sString[sLen] = XPR_FILE_SEPARATOR;
                ++sLen;
            }
        }

        mFileName->getString(sString + sLen, XPR_MAX_PATH - sLen);

        sFilePath->setString(sString);
    }

    XPR_SAFE_DELETE(mFilePath);

    mFilePath = sFilePath;
    mFileIndex = 0;

    mStarted = XPR_TRUE;

    return XPR_RCODE_SUCCESS;
}

void TraceLogger::stop(void)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        XPR_SAFE_DELETE(mTextFileWriter);

        mFileIo.close();

        mStarted = XPR_FALSE;
    }
}

xpr_bool_t TraceLogger::isStarted(void) const
{
    return mStarted;
}

xpr_rcode_t TraceLogger::logFatal(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelFatal, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logFatal(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelFatal, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logError(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelError, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logError(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelError, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logWarning(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelWarning, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logWarning(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelWarning, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logInfo(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelInfo, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logInfo(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelInfo, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logDebug(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelDebug, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logDebug(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelDebug, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logTrace(const xpr_char_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelTrace, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logTrace(const xpr_wchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelTrace, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::log(xpr_sint_t aLevel, const xpr_char_t *aFormat, va_list aArgs)
{
    if (aLevel > mLevel)
    {
        return XPR_RCODE_SUCCESS;
    }

    xpr_pid_t         sProcessId = xpr::getPid();
    xpr::ThreadId     sThreadId  = Thread::getCurThreadId();
    const xpr_char_t *sLevelText = kLogLevelText[aLevel];
    xpr::string       sFormattedMessage;
    xpr::TimeExpr     sTimeExprNow;
    xpr_sint_t        sMillisecond;

    xpr::getLocalTimeNow(sTimeExprNow);
    sMillisecond = sTimeExprNow.getMillisecond();

    sFormattedMessage.format(
        XPR_MBCS_STRING_LITERAL("[%02d-%02d-%02d %02d:%02d:%02d.%03d PID-%lld TID-%lld %s] "),
        sTimeExprNow.mYear, sTimeExprNow.mMonth, sTimeExprNow.mDay,
        sTimeExprNow.mHour, sTimeExprNow.mMinute, sTimeExprNow.mSecond, sMillisecond,
        sProcessId, sThreadId, sLevelText);
    sFormattedMessage.append_format(aFormat, aArgs);
    sFormattedMessage.append(XPR_MBCS_EOL);

    LogRecord sLogRecord;
    sLogRecord.mMessage      = sFormattedMessage.c_str();
    sLogRecord.mMessageBytes = sFormattedMessage.bytes();
    sLogRecord.mWideChar     = XPR_FALSE;

    if (XPR_IS_TRUE(hasFlag(kFlagTraceOut)))
    {
        // TODO
    }

    if (XPR_IS_TRUE(hasFlag(kFlagThreadSafe)))
    {
        return logThreadSafe(&sLogRecord);
    }

    return log(&sLogRecord);
}

xpr_rcode_t TraceLogger::log(xpr_sint_t aLevel, const xpr_wchar_t *aFormat, va_list aArgs)
{
    if (aLevel > mLevel)
    {
        return XPR_RCODE_SUCCESS;
    }

    xpr_pid_t          sProcessId = xpr::getPid();
    xpr::ThreadId      sThreadId  = Thread::getCurThreadId();
    const xpr_wchar_t *sLevelText = kLogLevelTextW[aLevel];
    xpr::wstring       sFormattedMessage;
    xpr::TimeExpr      sTimeExprNow;
    xpr_sint_t         sMillisecond;

    xpr::getLocalTimeNow(sTimeExprNow);
    sMillisecond = sTimeExprNow.getMillisecond();

    sFormattedMessage.format(
        XPR_WIDE_STRING_LITERAL("[%02d-%02d-%02d %02d:%02d:%02d.%03d PID-%lld TID-%lld %s] "),
        sTimeExprNow.mYear, sTimeExprNow.mMonth, sTimeExprNow.mDay,
        sTimeExprNow.mHour, sTimeExprNow.mMinute, sTimeExprNow.mSecond, sMillisecond,
        sProcessId, sThreadId, sLevelText);
    sFormattedMessage.append_format(aFormat, aArgs);
    sFormattedMessage.append(XPR_WIDE_EOL);

    LogRecord sLogRecord;
    sLogRecord.mMessage      = sFormattedMessage.c_str();
    sLogRecord.mMessageBytes = sFormattedMessage.bytes();
    sLogRecord.mWideChar     = XPR_TRUE;

    if (XPR_IS_TRUE(hasFlag(kFlagTraceOut)))
    {
        // TODO
    }

    if (XPR_IS_TRUE(hasFlag(kFlagThreadSafe)))
    {
        return logThreadSafe(&sLogRecord);
    }

    return log(&sLogRecord);
}

xpr_rcode_t TraceLogger::log(LogRecord *aLogRecord)
{
    XPR_ASSERT(aLogRecord != XPR_NULL);

    xpr_uint64_t sFileSize = 0;

    if (XPR_IS_TRUE(mFilePath->mWideChar))
    {
        sFileSize = FileSys::getFileSize(mFilePath->mStringW);
    }
    else
    {
        sFileSize = FileSys::getFileSize(mFilePath->mStringA);
    }

    xpr_rcode_t sRcode;

    if (sFileSize > 0)
    {
        if ((sFileSize + aLogRecord->mMessageBytes) > mFileSize)
        {
            sRcode = moveLogFiles();
            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                return sRcode;
            }
        }
    }

    sRcode = openLogFile();
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    CharSet sCharSet = CharSetMultiBytes;
    if (XPR_IS_TRUE(aLogRecord->mWideChar))
    {
        if (sizeof(xpr_wchar_t) == 4)
        {
            sCharSet = CharSetUtf32;
        }
        else
        {
            sCharSet = CharSetUtf16;
        }
    }

    sRcode = mTextFileWriter->write(aLogRecord->mMessage, aLogRecord->mMessageBytes, sCharSet);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sRcode = XPR_RCODE_SUCCESS;

        if (XPR_IS_TRUE(hasFlag(kFlagFileSync)))
        {
            sRcode = mFileIo.sync();
        }
    }

    closeLogFile();

    return sRcode;
}

xpr_rcode_t TraceLogger::logThreadSafe(LogRecord *aLogRecord)
{
    XPR_ASSERT(aLogRecord != XPR_NULL);

    xpr_rcode_t sRcode;

    {
        MutexGuard sMutexGuard(mMutex);

        sRcode = log(aLogRecord);
    }

    return sRcode;
}

// If circular logging option is enabled and file count is 10 and file name is 'test.log',
//
//        +-- test.log
// rename +-> test.log-1 
//            test.log-2 --+
//            test.log-3 <-+   --+
//               ...     <-------+    --+ rename
// delete <-- test.log-9 <--------------+
//
xpr_rcode_t TraceLogger::moveLogFiles(void)
{
    xpr_rcode_t sRcode = XPR_RCODE_SUCCESS;

    if (XPR_IS_FALSE(mFilePath->mWideChar))
    {
        xpr_sint_t i;
        xpr_sint_t sOldLogNumber = mFileIndex + 1;
        xpr_char_t sFilePath1[XPR_MAX_PATH] = {0};
        xpr_char_t sFilePath2[XPR_MAX_PATH] = {0};

        if (XPR_IS_TRUE(isCircular()))
        {
            if (sOldLogNumber >= (mFileCount - 1))
            {
                sprintf(sFilePath2, XPR_MBCS_STRING_LITERAL("%s-%d"), mFilePath->mStringA, mFileCount - 1);

                if (XPR_IS_TRUE(FileSys::exist(sFilePath2)))
                {
                    sRcode = FileSys::remove(sFilePath2);
                    if (XPR_RCODE_IS_ERROR(sRcode))
                    {
                        return sRcode;
                    }
                }

                sOldLogNumber = mFileCount - 1;
            }
        }
        else
        {
            do
            {
                sprintf(sFilePath2, XPR_MBCS_STRING_LITERAL("%s-%d"), mFilePath->mStringA, sOldLogNumber);

                if (XPR_IS_FALSE(FileSys::exist(sFilePath2)))
                {
                    break;
                }

                ++sOldLogNumber;

            } while (true);
        }

        for (i = sOldLogNumber; i > 1; --i)
        {
            sprintf(sFilePath1, XPR_MBCS_STRING_LITERAL("%s-%d"), mFilePath->mStringA, i - 1);
            sprintf(sFilePath2, XPR_MBCS_STRING_LITERAL("%s-%d"), mFilePath->mStringA, i);

            sRcode = FileSys::rename(sFilePath1, sFilePath2);
            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                return sRcode;
            }
        }

        sprintf(sFilePath1, XPR_MBCS_STRING_LITERAL("%s"), mFilePath->mStringA);
        sprintf(sFilePath2, XPR_MBCS_STRING_LITERAL("%s-%d"), mFilePath->mStringA, 1);

        sRcode = FileSys::rename(sFilePath1, sFilePath2);
        if (XPR_RCODE_IS_ERROR(sRcode))
        {
            return sRcode;
        }

        ++mFileIndex;
    }
    else
    {
        xpr_sint_t  i;
        xpr_sint_t  sOldLogNumber = mFileIndex + 1;
        xpr_wchar_t sFilePath1[XPR_MAX_PATH] = {0};
        xpr_wchar_t sFilePath2[XPR_MAX_PATH] = {0};

        if (XPR_IS_TRUE(isCircular()))
        {
            if (sOldLogNumber >= (mFileCount - 1))
            {
                swprintf(sFilePath2, XPR_WIDE_STRING_LITERAL("%s-%d"), mFilePath->mStringA, mFileCount - 1);

                if (XPR_IS_TRUE(FileSys::exist(sFilePath2)))
                {
                    sRcode = FileSys::remove(sFilePath2);
                    if (XPR_RCODE_IS_ERROR(sRcode))
                    {
                        return sRcode;
                    }
                }

                sOldLogNumber = mFileCount - 1;
            }
        }
        else
        {
            do
            {
                swprintf(sFilePath2, XPR_WIDE_STRING_LITERAL("%s-%d"), mFilePath->mStringA, sOldLogNumber);

                if (XPR_IS_FALSE(FileSys::exist(sFilePath2)))
                {
                    break;
                }

                ++sOldLogNumber;

            } while (true);
        }

        for (i = sOldLogNumber; i > 1; --i)
        {
            swprintf(sFilePath1, XPR_WIDE_STRING_LITERAL("%s-%d"), mFilePath->mStringA, i - 1);
            swprintf(sFilePath2, XPR_WIDE_STRING_LITERAL("%s-%d"), mFilePath->mStringA, i);

            sRcode = FileSys::rename(sFilePath1, sFilePath2);
            if (XPR_RCODE_IS_ERROR(sRcode))
            {
                return sRcode;
            }
        }

        swprintf(sFilePath1, XPR_WIDE_STRING_LITERAL("%s"), mFilePath->mStringA);
        swprintf(sFilePath2, XPR_WIDE_STRING_LITERAL("%s-%d"), mFilePath->mStringA, 1);

        sRcode = FileSys::rename(sFilePath1, sFilePath2);
        if (XPR_RCODE_IS_ERROR(sRcode))
        {
            return sRcode;
        }

        ++mFileIndex;
    }

    return sRcode;
}

xpr_rcode_t TraceLogger::openLogFile(void)
{
    xpr_rcode_t sRcode;
    xpr_bool_t  sExist = XPR_FALSE;
    xpr_sint_t  sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeAppend;

    if (XPR_IS_TRUE(mFilePath->mWideChar))
    {
        sExist = FileSys::exist(mFilePath->mStringW);

        sRcode = mFileIo.open(mFilePath->mStringW, sOpenMode);
    }
    else
    {
        sExist = FileSys::exist(mFilePath->mStringA);

        sRcode = mFileIo.open(mFilePath->mStringA, sOpenMode);
    }

    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    mTextFileWriter = new TextFileWriter(mFileIo);
    if (XPR_IS_NULL(mTextFileWriter))
    {
        return XPR_RCODE_ENOMEM;
    }

    mTextFileWriter->setEncoding(mCharSet);
    mTextFileWriter->setEndOfLine(XPR_MBCS_EOL);

    if (XPR_IS_FALSE(sExist) && XPR_IS_TRUE(mWriteBom))
    {
        mTextFileWriter->writeBom();
    }

    return sRcode;
}

void TraceLogger::closeLogFile(void)
{
    mFileIo.close();
}
} // namespace xpr
