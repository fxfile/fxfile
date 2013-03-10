//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_file_io.h"
#include "xpr_rcode.h"
#include "xpr_bit.h"
#include "xpr_system.h"
#include "xpr_memory.h"
#include "xpr_char_set.h"
#include "xpr_detail_cross_string.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
FileIo::FileIo(void)
    : mOpenMode(0)
    , mFilePath(XPR_NULL)
{
    mHandle.mFile = XPR_NULL;
}

FileIo::~FileIo(void)
{
    close();
}

xpr_rcode_t FileIo::open(const xpr_char_t *aPath, xpr_sint_t aOpenMode)
{
    xpr_size_t sPathLen = strlen(aPath);

    return open(aPath, sPathLen * sizeof(xpr_char_t), XPR_FALSE, aOpenMode);
}

xpr_rcode_t FileIo::open(const xpr_wchar_t *aPath, xpr_sint_t aOpenMode)
{
    xpr_size_t sPathLen = wcslen(aPath);

    return open(aPath, sPathLen * sizeof(xpr_wchar_t), XPR_TRUE, aOpenMode);
}

xpr_rcode_t FileIo::open(const void *aPath, xpr_size_t aPathBytes, xpr_bool_t aWideChar, xpr_sint_t aOpenMode)
{
    if (XPR_IS_NULL(aPath) || aPathBytes == 0)
        return XPR_RCODE_EINVAL;

    DWORD sAccessFlags    = 0;
    DWORD sSharedMode     = 0;
    DWORD sCreateFlags    = 0;
    DWORD sAttributeFlags = 0;

    if (XPR_TEST_BITS(aOpenMode, OpenModeWriteOnly))
        sAccessFlags = GENERIC_WRITE;
    else if (XPR_TEST_BITS(aOpenMode, OpenModeReadWrite))
        sAccessFlags = GENERIC_READ | GENERIC_WRITE;
    else if (XPR_TEST_BITS(aOpenMode, OpenModeAppend))
        sAccessFlags = FILE_APPEND_DATA;
    else
        sAccessFlags = GENERIC_READ;

    sSharedMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if (getOsVer() >= xpr::kOsVerWinNT)
        sSharedMode |= FILE_SHARE_DELETE;

    if (XPR_TEST_BITS(aOpenMode, OpenModeCreate))
    {
        if (XPR_TEST_BITS(aOpenMode, OpenModeExclusive))
            sCreateFlags = CREATE_NEW;
        else if (XPR_TEST_BITS(aOpenMode, OpenModeTruncate))
            sCreateFlags = CREATE_ALWAYS;
        else
            sCreateFlags = OPEN_ALWAYS;
    }
    else
    {
        if (XPR_TEST_BITS(aOpenMode, OpenModeExclusive))
            return XPR_RCODE_EINVAL;
        else if (XPR_TEST_BITS(aOpenMode, OpenModeTruncate))
            sCreateFlags = TRUNCATE_EXISTING;
        else
            sCreateFlags = OPEN_EXISTING;
    }

    if (XPR_TEST_BITS(aOpenMode, OpenModeSync))
        sAttributeFlags = FILE_FLAG_WRITE_THROUGH;

    detail::CrossString *sFilePath = new detail::CrossString;
    if (XPR_IS_NULL(sFilePath))
    {
        return XPR_RCODE_ENOMEM;
    }

    xpr_rcode_t sRcode = sFilePath->setString(aPath, aPathBytes, aWideChar);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        XPR_SAFE_DELETE(sFilePath);

        return sRcode;
    }

    HANDLE sFile;

    if (XPR_IS_TRUE(aWideChar))
    {
        sFile = ::CreateFileW(sFilePath->mStringW,
                              sAccessFlags,
                              sSharedMode,
                              XPR_NULL,
                              sCreateFlags,
                              sAttributeFlags,
                              XPR_NULL);
    }
    else
    {
        sFile = ::CreateFileA(sFilePath->mStringA,
                              sAccessFlags,
                              sSharedMode,
                              XPR_NULL,
                              sCreateFlags,
                              sAttributeFlags,
                              XPR_NULL);
    }

    if (sFile == INVALID_HANDLE_VALUE)
    {
        XPR_SAFE_DELETE(sFilePath);

        return XPR_RCODE_GET_OS_ERROR();
    }

    mHandle.mFile = sFile;

    XPR_SAFE_DELETE(mFilePath);
    mFilePath = sFilePath;

    mOpenMode = aOpenMode;

    return XPR_RCODE_SUCCESS;
}

xpr_bool_t FileIo::isOpened(void) const
{
    if (XPR_IS_NULL(mHandle.mFile) || mHandle.mFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_sint_t FileIo::getOpenMode(void) const
{
    return mOpenMode;
}

void FileIo::close(void)
{
    if (XPR_IS_TRUE(isOpened()))
    {
        ::CloseHandle(mHandle.mFile);
        mHandle.mFile = XPR_NULL;

        XPR_SAFE_DELETE(mFilePath);

        mOpenMode = 0;
    }
}

xpr_bool_t FileIo::getPath(xpr_char_t *aPath, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    if (XPR_IS_NULL(mFilePath))
        return XPR_FALSE;

    return mFilePath->getString(aPath, aMaxLen);
}

xpr_bool_t FileIo::getPath(xpr_wchar_t *aPath, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    if (XPR_IS_NULL(mFilePath))
        return XPR_FALSE;

    return mFilePath->getString(aPath, aMaxLen);
}

xpr_bool_t FileIo::isWideChar(void) const
{
    if (XPR_IS_NULL(mFilePath))
        return XPR_FALSE;

    return mFilePath->mWideChar;
}

const xpr_byte_t *FileIo::getPath(void) const
{
    if (XPR_IS_NULL(mFilePath))
        return XPR_NULL;

    return mFilePath->mString;
}

xpr_size_t FileIo::getPathBytes(void) const
{
    if (XPR_IS_NULL(mFilePath))
        return 0;

    return mFilePath->mBytes;
}

xpr_size_t FileIo::getPathLen(void) const
{
    if (XPR_IS_NULL(mFilePath))
        return 0;

    return mFilePath->getLen();
}

xpr_rcode_t FileIo::read(void        *aData,
                         xpr_size_t   aSize,
                         xpr_ssize_t *aReadSize)
{
    if (XPR_IS_NULL(aData) || XPR_IS_NULL(aReadSize))
        return XPR_RCODE_EINVAL;

    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    if (aSize == 0)
    {
        *aReadSize = aSize;
        return XPR_RCODE_SUCCESS;
    }

    DWORD sReadSize = 0;

    xpr_bool_t sResult;
    sResult = ::ReadFile(mHandle.mFile,
                         aData,
                         (DWORD)aSize,
                         &sReadSize,
                         XPR_NULL);

    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    *aReadSize = (xpr_ssize_t)sReadSize;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::write(const void  *aData,
                          xpr_size_t   aSize,
                          xpr_ssize_t *aWrittenSize)
{
    if (XPR_IS_NULL(aData) || XPR_IS_NULL(aWrittenSize))
        return XPR_RCODE_EINVAL;

    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    DWORD sWrittenSize = 0;

    xpr_bool_t sResult;
    sResult = ::WriteFile(mHandle.mFile,
                          aData,
                          (DWORD)aSize,
                          &sWrittenSize,
                          XPR_NULL);

    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    *aWrittenSize = (xpr_ssize_t)sWrittenSize;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::sync(void)
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    xpr_bool_t sResult = ::FlushFileBuffers(mHandle.mFile);
    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_sint64_t FileIo::tell(void) const
{
    if (XPR_IS_FALSE(isOpened()))
        return 0;

    xpr_sint64_t sOffsetFilePointer = 0;
    xpr_sint64_t sFilePointer = 0;

    xpr_bool_t sResult;
    sResult = ::SetFilePointerEx(
        mHandle.mFile,
        *reinterpret_cast<PLARGE_INTEGER>(&sOffsetFilePointer),
        reinterpret_cast<PLARGE_INTEGER>(&sFilePointer),
        FILE_CURRENT);

    if (XPR_IS_FALSE(sResult))
        return 0;

    return sFilePointer;
}

xpr_rcode_t FileIo::seekToBegin(void)
{
    return seek(0, SeekModeBegin);
}

xpr_rcode_t FileIo::seekFromBegin(xpr_sint64_t aOffset)
{
    return seek(aOffset, SeekModeBegin);
}

xpr_rcode_t FileIo::seek(xpr_sint64_t aOffset, xpr_uint_t aSeekMode)
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    xpr_sint64_t sNewFilePointer = 0;
    DWORD sMoveMethod;

    switch (aSeekMode)
    {
    case SeekModeBegin:   sMoveMethod = FILE_BEGIN;   break;
    case SeekModeCurrent: sMoveMethod = FILE_CURRENT; break;
    case SeekModeEnd:     sMoveMethod = FILE_END;     break;

    default:
        return XPR_RCODE_EINVAL;
    }

    xpr_bool_t sResult;
    sResult = ::SetFilePointerEx(
        mHandle.mFile,
        *reinterpret_cast<PLARGE_INTEGER>(&aOffset),
        reinterpret_cast<PLARGE_INTEGER>(&sNewFilePointer),
        sMoveMethod);

    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::seekToEnd(void)
{
    return seek(0, SeekModeEnd);
}

xpr_rcode_t FileIo::seekFromEnd(xpr_sint64_t aOffset)
{
    return seek(aOffset, SeekModeEnd);
}

xpr_sint64_t FileIo::getFileSize(void) const
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    xpr_sint64_t sFileSize = 0;

    xpr_bool_t sResult;
    sResult = ::GetFileSizeEx(mHandle.mFile, (PLARGE_INTEGER)&sFileSize);
    if (XPR_IS_FALSE(sResult))
        return 0;

    return sFileSize;
}

xpr_rcode_t FileIo::truncate(void)
{
    xpr_sint64_t sOffset = tell();
    return truncate(sOffset);
}

xpr_rcode_t FileIo::truncate(xpr_sint64_t aOffset)
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    xpr_rcode_t sRcode = seekFromBegin(aOffset);
    if (sRcode != XPR_RCODE_SUCCESS)
        return sRcode;

    xpr_bool_t sResult = ::SetEndOfFile(mHandle.mFile);
    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::lock(void)
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    DWORD sLen = kuint32max;

    if (getOsVer() < xpr::kOsVerWinNT)
    {
        xpr_rcode_t sRcode;

        while (::LockFile(mHandle.mFile, 0, 0, sLen, 0) == XPR_FALSE)
        {
            sRcode = XPR_RCODE_GET_OS_ERROR();
            if (XPR_RCODE_IS_EAGAIN(sRcode)) // ERROR_LOCK_VIOLATION
            {
                Sleep(200);
            }
            else
            {
                return sRcode;
            }
        }
    }
    else
    {
        DWORD sFlags = LOCKFILE_EXCLUSIVE_LOCK;
        OVERLAPPED sOverlapped = {0,};

        xpr_bool_t sResult;
        sResult = ::LockFileEx(mHandle.mFile,
                               sFlags,
                               0,
                               sLen,
                               sLen,
                               &sOverlapped);

        if (XPR_IS_FALSE(sResult))
            return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::tryLock(void)
{
    if (XPR_IS_FALSE(isOpened()))
        return XPR_RCODE_EBADF;

    DWORD sLen = kuint32max;
    xpr_bool_t sResult;

    if (getOsVer() < xpr::kOsVerWinNT)
    {
        sResult = ::LockFile(mHandle.mFile, 0, 0, sLen, 0);
    }
    else
    {
        DWORD sFlags = LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY;
        OVERLAPPED sOverlapped = {0,};

        sResult = ::LockFileEx(mHandle.mFile,
                               sFlags,
                               0,
                               sLen,
                               sLen,
                               &sOverlapped);
    }

    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileIo::unlock(void)
{
    DWORD sLen = kuint32max;
    xpr_bool_t sResult;

    if (getOsVer() < xpr::kOsVerWinNT)
    {
        sResult = ::UnlockFile(mHandle.mFile, 0, 0, sLen, 0);
    }
    else
    {
        sResult = ::UnlockFile(mHandle.mFile, 0, 0, sLen, sLen);
    }

    if (XPR_IS_FALSE(sResult))
        return XPR_RCODE_GET_OS_ERROR();

    return XPR_RCODE_SUCCESS;
}

#endif // XPR_CFG_OS_WINDOWS
} // namespace xpr
