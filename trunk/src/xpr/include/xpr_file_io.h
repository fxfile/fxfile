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

#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_EOLA XPR_MBCS_STRING_LITERAL("\r\n")
#define XPR_EOLW XPR_WIDE_STRING_LITERAL("\r\n")
#else
#define XPR_EOLA XPR_MBCS_STRING_LITERAL("\n")
#define XPR_EOLW XPR_WIDE_STRING_LITERAL("\n")
#endif

#if defined(XPR_CFG_UNICODE)
#define XPR_EOL XPR_EOLW
#else
#define XPR_EOL XPR_EOLA
#endif

namespace xpr
{
//class FileIo
//{
//public:
//    enum OpenMode
//    {
//#if defined (XPR_CFG_OS_WINDOWS)
//        OpenModeReadOnly  = 0x00000000,
//        OpenModeWriteOnly = 0x00000001,
//        OpenModeReadWrite = 0x00000002,
//        OpenModeAppend    = 0x00000008,
//        OpenModeCreate    = 0x00000010,
//        OpenModeTruncate  = 0x00000020,
//        OpenModeExclusive = 0x00000040,
//        OpenModeSync      = 0x00000100,
//#else
//        OpenModeReadOnly  = O_RDONLY,
//        OpenModeWriteOnly = O_WRONLY,
//        OpenModeReadWrite = O_RDWR,
//        OpenModeAppend    = O_APPEND,
//        OpenModeCreate    = O_CREAT,
//        OpenModeTruncate  = O_TRUNC,
//        OpenModeExclusive = O_EXCL,
//        OpenModeSync      = O_SYNC,
//#endif
//    };
//
//    enum SeekMode
//    {
//        SeekModeBegin,
//        SeekModeCurrent,
//        SeekModeEnd,
//    };
//
//    struct Handle
//    {
//#if defined(XPR_CFG_OS_WINDOWS)
//        HANDLE mFile;
//#else
//        sint_t mFile;
//#endif
//    };
//
//public:
//    virtual xpr_rcode_t open(const xpr_tchar_t *aPath, OpenMode aOpenMode);
//    virtual xpr_bool_t isOpened(void);
//    virtual void close(void);
//
//public:
//    virtual xpr_bool_t getPath(xpr_tchar_t *aPath, xpr_size_t aMaxLen);
//    virtual const xpr_tchar_t *getPath(void);
//
//public:
//    virtual xpr_rcode_t read(void *aData, xpr_sint_t aSize);
//    virtual xpr_rcode_t write(void *aData, xpr_sint_t aSize);
//    virtual xpr_bool_t flush(void);
//
//public:
//    virtual xpr_sint64_t tell(void);
//    virtual xpr_rcode_t seekToBegin(void);
//    virtual xpr_rcode_t seekFromBegin(xpr_sint64_t aOffset);
//    virtual xpr_rcode_t seek(sint64_t aOffset, SeekMode aSeekMode = SeekModeCurrent);
//    virtual xpr_rcode_t seekToEnd(void);
//    virtual xpr_rcode_t seekFromEnd(xpr_sint64_t aOffset);
//
//public:
//    virtual xpr_sint64_t getFileSize(void);
//    virtual xpr_rcode_t setFileSize(xpr_sint64_t aSize);
//    virtual xpr_bool_t setEndOfFile(void);
//
//protected:
//    Handle mHandle;
//    xpr_tchar_t mPath[XPR_MAX_PATH];
//};
} // namespace xpr

#endif // __XPR_FILE_IO_H__
