//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_FILE_IO_H__
#define __XPR_FILE_IO_H__
#pragma once

#include "xpr_config.h"
#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_file_sys.h"
#include "xpr_char_set.h"

#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_EOL_MBCS XPR_MBCS_STRING_LITERAL("\r\n")
#define XPR_EOL_WIDE XPR_WIDE_STRING_LITERAL("\r\n")
#else
#define XPR_EOL_MBCS XPR_MBCS_STRING_LITERAL("\n")
#define XPR_EOL_WIDE XPR_WIDE_STRING_LITERAL("\n")
#endif

#if defined(XPR_CFG_UNICODE)
#define XPR_EOL XPR_EOL_WIDE
#else
#define XPR_EOL XPR_EOL_MBCS
#endif

namespace xpr
{
class XPR_DL_API FileIo
{
public:
    enum
    {
#if defined (XPR_CFG_OS_WINDOWS)
        OpenModeReadOnly  = 0x00000000,
        OpenModeWriteOnly = 0x00000001,
        OpenModeReadWrite = 0x00000002,
        OpenModeAppend    = 0x00000008,
        OpenModeCreate    = 0x00000010,
        OpenModeTruncate  = 0x00000020,
        OpenModeExclusive = 0x00000040,
        OpenModeSync      = 0x00000100,
#else
        OpenModeReadOnly  = O_RDONLY,
        OpenModeWriteOnly = O_WRONLY,
        OpenModeReadWrite = O_RDWR,
        OpenModeAppend    = O_APPEND,
        OpenModeCreate    = O_CREAT,
        OpenModeTruncate  = O_TRUNC,
        OpenModeExclusive = O_EXCL,
        OpenModeSync      = O_SYNC,
#endif
    };

    enum
    {
        SeekModeBegin,
        SeekModeCurrent,
        SeekModeEnd,
    };

    struct Handle
    {
#if defined(XPR_CFG_OS_WINDOWS)
        HANDLE     mFile;
#else
        xpr_sint_t mFile;
#endif
    };

public:
    FileIo(void);
    virtual ~FileIo(void);

public:
    virtual xpr_rcode_t open(const xpr_char_t *aPath, xpr_sint_t aOpenMode);
    virtual xpr_rcode_t open(const xpr_wchar_t *aPath, xpr_sint_t aOpenMode);
    virtual xpr_rcode_t open(const void *aPath, xpr_size_t aPathBytes, CharSet aCharSet, xpr_sint_t aOpenMode);
    virtual xpr_bool_t isOpened(void) const;
    virtual xpr_sint_t getOpenMode(void) const;
    virtual void close(void);

public:
    virtual xpr_bool_t getPath(xpr_char_t *aPath, xpr_size_t aMaxLen);
    virtual xpr_bool_t getPath(xpr_wchar_t *aPath, xpr_size_t aMaxLen);
    virtual CharSet getPathCharSet(void) const;
    virtual const xpr_byte_t *getPath(void) const;
    virtual xpr_size_t getPathBytes(void) const;

public:
    virtual xpr_rcode_t read(void *aData, xpr_size_t aSize, xpr_ssize_t *aReadSize);
    virtual xpr_rcode_t write(const void *aData, xpr_size_t aSize, xpr_ssize_t *aWrittenSize);
    virtual xpr_rcode_t sync(void);

public:
    virtual xpr_sint64_t tell(void) const;
    virtual xpr_rcode_t seekToBegin(void);
    virtual xpr_rcode_t seekFromBegin(xpr_sint64_t aOffset);
    virtual xpr_rcode_t seek(xpr_sint64_t aOffset, xpr_uint_t aSeekMode = SeekModeCurrent);
    virtual xpr_rcode_t seekToEnd(void);
    virtual xpr_rcode_t seekFromEnd(xpr_sint64_t aOffset);

public:
    virtual xpr_sint64_t getFileSize(void) const;
    virtual xpr_rcode_t truncate(void);
    virtual xpr_rcode_t truncate(xpr_sint64_t aOffset);

public:
    virtual xpr_rcode_t lock(void);
    virtual xpr_rcode_t tryLock(void);
    virtual xpr_rcode_t unlock(void);

protected:
    Handle      mHandle;
    xpr_byte_t *mPath;
    xpr_size_t  mPathBytes;
    CharSet     mPathCharSet;
    xpr_sint_t  mOpenMode;
};
} // namespace xpr

#endif // __XPR_FILE_IO_H__
