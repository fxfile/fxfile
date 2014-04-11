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
#include "xpr_bit.h"
#include "xpr_time.h"
#include "xpr_process.h"
#include "xpr_thread.h"
#include "xpr_memory.h"
#include "xpr_rcode.h"

namespace xpr
{
namespace
{
const xpr_sint_t kDefLevel     = TraceLogger::kLogLevelInfo;
const xpr_uint_t kDefFlags     = TraceLogger::kFlagTraceOut;
const xpr_sint_t kDefFileSize  = 10 * 1024 * 1024; // 10MB
const xpr_sint_t kDefFileCount = 10;               // 10 (10MB x 10 = 100MB)

const xpr_char_t *kLogLevelText[] = {
    XPR_MBCS_STRING_LITERAL("NONE"),
    XPR_MBCS_STRING_LITERAL("FATAL"),
    XPR_MBCS_STRING_LITERAL("ERROR"),
    XPR_MBCS_STRING_LITERAL("WARN"),
    XPR_MBCS_STRING_LITERAL("INFO"),
    XPR_MBCS_STRING_LITERAL("DEBUG"),
    XPR_MBCS_STRING_LITERAL("TRACE"),
};

const xpr_wchar_t *kLogLevelTextW[] = {
    XPR_WIDE_STRING_LITERAL("NONE"),
    XPR_WIDE_STRING_LITERAL("FATAL"),
    XPR_WIDE_STRING_LITERAL("ERROR"),
    XPR_WIDE_STRING_LITERAL("WARN"),
    XPR_WIDE_STRING_LITERAL("INFO"),
    XPR_WIDE_STRING_LITERAL("DEBUG"),
    XPR_WIDE_STRING_LITERAL("TRACE"),
};
} // namespace anonymous

struct TraceLogger::LogRecord
{
    const xpr_tchar_t *mMessage;
    xpr_size_t         mMessageBytes;
    CharSet            mMessageCharSet;
};

TraceLogger::TraceLogger(void)
    : mLevel(kDefLevel)
    , mFlags(kDefFlags)
    , mFileSize(kDefFileSize), mFileCount(kDefFileCount), mFileIndex(0)
    , mTextFileWriter(XPR_NULL)
    , mCharSet(CharSetNone), mWriteBom(XPR_FALSE)
    , mStarted(XPR_FALSE)
{
}

TraceLogger::~TraceLogger(void)
{
    stop();
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

void TraceLogger::getFileDir(xpr::string &aFileDir) const
{
    aFileDir = mFileDir;
}

void TraceLogger::getFileName(xpr::string &aFileName) const
{
    aFileName = mFileName;
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

xpr_bool_t TraceLogger::setFileDir(const xpr::string &aFileDir)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mFileDir = aFileDir;

    return XPR_TRUE;
}

xpr_bool_t TraceLogger::setFileName(const xpr::string &aFileName)
{
    if (XPR_IS_TRUE(isStarted()))
    {
        return XPR_FALSE;
    }

    mFileName = aFileName;

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

    mFilePath.clear();

    if (mFileDir.empty() == XPR_FALSE)
    {
        mFilePath  = mFileDir;
        mFilePath += XPR_FILE_SEPARATOR;
    }

    mFilePath += mFileName;

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
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelFatal, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logFatal(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelFatal, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logError(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelError, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logError(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelError, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logWarning(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelWarning, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logWarning(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelWarning, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logInfo(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelInfo, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logInfo(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelInfo, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logDebug(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelDebug, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logDebug(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelDebug, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logTrace(const xpr_char_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelTrace, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::logTrace(const xpr_wchar_t *aFormat, ...)
{
    XPR_ASSERT(aFormat != XPR_NULL);

    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_rcode_t sRcode = log(kLogLevelTrace, aFormat, sArgs);

    va_end(sArgs);

    return sRcode;
}

xpr_rcode_t TraceLogger::log(xpr_sint_t aLevel, const xpr_char_t *aFormat, va_list aArgs)
{
    XPR_ASSERT(aFormat != XPR_NULL);

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
    sLogRecord.mMessage        = sFormattedMessage.c_str();
    sLogRecord.mMessageBytes   = sFormattedMessage.bytes();
    sLogRecord.mMessageCharSet = sFormattedMessage.get_char_set();

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
    XPR_ASSERT(aFormat != XPR_NULL);

    if (aLevel > mLevel)
    {
        return XPR_RCODE_SUCCESS;
    }

    xpr_pid_t          sProcessId = xpr::getPid();
    xpr::ThreadId      sThreadId  = Thread::getCurThreadId();
    const xpr_wchar_t *sLevelText = kLogLevelTextW[aLevel];
    xpr::string        sFormattedMessage;
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
    sLogRecord.mMessage        = sFormattedMessage.c_str();
    sLogRecord.mMessageBytes   = sFormattedMessage.bytes();
    sLogRecord.mMessageCharSet = sFormattedMessage.get_char_set();

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

    xpr_rcode_t  sRcode;
    xpr_uint64_t sFileSize = FileSys::getFileSize(mFilePath.c_str());

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

    sRcode = mTextFileWriter->write(aLogRecord->mMessage, aLogRecord->mMessageBytes, aLogRecord->mMessageCharSet);
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
    xpr_sint_t  i;
    xpr_sint_t  sOldLogNumber = mFileIndex + 1;
    xpr::string sFilePath1;
    xpr::string sFilePath2;

    if (XPR_IS_TRUE(isCircular()))
    {
        if (sOldLogNumber >= (mFileCount - 1))
        {
            sFilePath2.format(XPR_STRING_LITERAL("%s-%d"), mFilePath.c_str(), mFileCount - 1);

            if (XPR_IS_TRUE(FileSys::exist(sFilePath2.c_str())))
            {
                sRcode = FileSys::remove(sFilePath2.c_str());
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
            sFilePath2.format(XPR_STRING_LITERAL("%s-%d"), mFilePath.c_str(), sOldLogNumber);

            if (XPR_IS_FALSE(FileSys::exist(sFilePath2.c_str())))
            {
                break;
            }

            ++sOldLogNumber;

        } while (true);
    }

    for (i = sOldLogNumber; i > 1; --i)
    {
        sFilePath1.format(XPR_STRING_LITERAL("%s-%d"), mFilePath.c_str(), i - 1);
        sFilePath2.format(XPR_STRING_LITERAL("%s-%d"), mFilePath.c_str(), i);

        sRcode = FileSys::rename(sFilePath1.c_str(), sFilePath2.c_str());
        if (XPR_RCODE_IS_ERROR(sRcode))
        {
            return sRcode;
        }
    }

    sFilePath1.format(XPR_STRING_LITERAL("%s"), mFilePath.c_str());
    sFilePath2.format(XPR_STRING_LITERAL("%s-%d"), mFilePath.c_str(), 1);

    sRcode = FileSys::rename(sFilePath1.c_str(), sFilePath2.c_str());
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    ++mFileIndex;

    return sRcode;
}

xpr_rcode_t TraceLogger::openLogFile(void)
{
    xpr_rcode_t sRcode;
    xpr_bool_t  sExist = XPR_FALSE;
    xpr_sint_t  sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeAppend;

    sExist = FileSys::exist(mFilePath.c_str());

    sRcode = mFileIo.open(mFilePath, sOpenMode);

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
