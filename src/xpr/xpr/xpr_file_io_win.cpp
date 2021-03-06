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
#include "xpr_debug.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
FileIo::FileIo(void)
    : mOpenMode(0)
{
    mHandle.mFile = XPR_NULL;
}

FileIo::~FileIo(void)
{
    close();
}

xpr_rcode_t FileIo::open(const xpr::string &aFilePath, xpr_sint_t aOpenMode)
{
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

    HANDLE sFile;
    sFile = ::CreateFile(aFilePath.c_str(),
                         sAccessFlags,
                         sSharedMode,
                         XPR_NULL,
                         sCreateFlags,
                         sAttributeFlags,
                         XPR_NULL);

    if (sFile == INVALID_HANDLE_VALUE)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    mHandle.mFile = sFile;
    mFilePath     = aFilePath;
    mOpenMode     = aOpenMode;

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

        mFilePath.clear();

        mOpenMode = 0;
    }
}

void FileIo::getFilePath(xpr::string &aFilePath) const
{
    aFilePath = mFilePath;
}

const xpr::string &FileIo::getFilePath(void) const
{
    return mFilePath;
}

xpr_rcode_t FileIo::read(void        *aData,
                         xpr_size_t   aSize,
                         xpr_ssize_t *aReadSize)
{
    XPR_ASSERT(aData != XPR_NULL);
    XPR_ASSERT(aReadSize != XPR_NULL);

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
    XPR_ASSERT(aData != XPR_NULL);
    XPR_ASSERT(aWrittenSize != XPR_NULL);

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
