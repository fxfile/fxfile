//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TRACE_LOGGER_H__
#define __XPR_TRACE_LOGGER_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_char_set.h"
#include "xpr_file_io.h"
#include "xpr_thread_sync.h"

namespace xpr
{
namespace detail
{
class CrossString;
} // namespace detail

class TextFileWriter;

class XPR_DL_API TraceLogger
{
    DISALLOW_COPY_AND_ASSIGN(TraceLogger);

public:
    enum
    {
        kLogLevelNone  = 0,
        kLogLevelFatal,
        kLogLevelError,
        kLogLevelWarning,
        kLogLevelInfo,
        kLogLevelDebug,
        kLogLevelTrace,
    };

    enum
    {
        kFlagNone       = 0x0000,
        kFlagThreadSafe = 0x0001,
        kFlagTraceOut   = 0x0002, // TODO
        kFlagCircular   = 0x0004,
        kFlagFileSync   = 0x0008,
        kFlagBulkSync   = 0x0010, // TODO
        kFlagThread     = 0x0020, // TODO async trace logging
    };

public:
    TraceLogger(void);
    virtual ~TraceLogger(void);

public:
    xpr_sint_t getLevel(void) const;
    xpr_uint_t getFlags(void) const;
    xpr_bool_t hasFlag(xpr_uint_t aFlag) const;
    xpr_bool_t getFileDir(xpr_char_t *aFileDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getFileDir(xpr_wchar_t *aFileDir, xpr_size_t aMaxLen) const;
    xpr_bool_t getFileName(xpr_char_t *aFileName, xpr_size_t aMaxLen) const;
    xpr_bool_t getFileName(xpr_wchar_t *aFileName, xpr_size_t aMaxLen) const;
    xpr_size_t getFileSize(void) const;
    xpr_size_t getFileCount(void) const;
    CharSet    getEncoding(void) const;
    xpr_bool_t isWriteBom(void) const;
    xpr_bool_t isCircular(void) const;

    xpr_bool_t setLevel(xpr_sint_t aLevel);
    xpr_bool_t setFlags(xpr_uint_t aFlags);
    xpr_bool_t setFileDir(const xpr_char_t *aFileDir);
    xpr_bool_t setFileDir(const xpr_wchar_t *aFileDir);
    xpr_bool_t setFileName(const xpr_char_t *aFileName);
    xpr_bool_t setFileName(const xpr_wchar_t *aFileName);
    xpr_bool_t setFileSize(xpr_sint_t aFileSize);
    xpr_bool_t setFileCount(xpr_sint_t aFileCount);
    xpr_bool_t setEncoding(CharSet aCharSet, xpr_bool_t aWriteBom);

public:
    virtual xpr_rcode_t start(void);
    virtual void stop(void);

    virtual xpr_bool_t isStarted(void) const;

public:
    xpr_rcode_t logFatal(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logFatal(const xpr_wchar_t *aFormat, ...);
    xpr_rcode_t logError(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logError(const xpr_wchar_t *aFormat, ...);
    xpr_rcode_t logWarning(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logWarning(const xpr_wchar_t *aFormat, ...);
    xpr_rcode_t logInfo(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logInfo(const xpr_wchar_t *aFormat, ...);
    xpr_rcode_t logDebug(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logDebug(const xpr_wchar_t *aFormat, ...);
    xpr_rcode_t logTrace(const xpr_char_t *aFormat, ...);
    xpr_rcode_t logTrace(const xpr_wchar_t *aFormat, ...);

protected:
    virtual xpr_rcode_t log(xpr_sint_t aLevel, const xpr_char_t *aFormat, va_list aArgs);
    virtual xpr_rcode_t log(xpr_sint_t aLevel, const xpr_wchar_t *aFormat, va_list aArgs);

private:
    struct LogRecord;
    virtual xpr_rcode_t log(LogRecord *aLogRecord);
    virtual xpr_rcode_t logThreadSafe(LogRecord *aLogRecord);

    xpr_rcode_t moveLogFiles(void);
    xpr_rcode_t openLogFile(void);
    void closeLogFile(void);

protected:
    xpr_sint_t           mLevel;
    xpr_uint_t           mFlags;
    detail::CrossString *mFileDir;
    detail::CrossString *mFileName;
    detail::CrossString *mFilePath;
    xpr_sint_t           mFileSize;
    xpr_sint_t           mFileCount;
    xpr_sint_t           mFileIndex;
    FileIo               mFileIo;
    TextFileWriter      *mTextFileWriter;
    CharSet              mCharSet;
    xpr_bool_t           mWriteBom;
    xpr_bool_t           mStarted;
    Mutex                mMutex;
};
} // namespace xpr

#endif // __XPR_TRACE_LOGGER_H__
