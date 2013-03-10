//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_file_sys.h"
#include "xpr_rcode.h"
#include "xpr_string.h"
#include "xpr_wide_string.h"
#include "xpr_bit.h"

#include <direct.h>

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
xpr_bool_t FileSys::exist(const xpr_char_t *aFilePath)
{
    xpr_bool_t sResult = XPR_FALSE;

    HANDLE           sFindFile;
    WIN32_FIND_DATAA sWin32FindData = {0};

    sFindFile = ::FindFirstFileA(aFilePath, &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        ::FindClose(sFindFile);
        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FileSys::exist(const xpr_wchar_t *aFilePath)
{
    xpr_bool_t sResult = XPR_FALSE;

    HANDLE           sFindFile;
    WIN32_FIND_DATAW sWin32FindData = {0};

    sFindFile = ::FindFirstFileW(aFilePath, &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        ::FindClose(sFindFile);
        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_rcode_t FileSys::mkdir(const xpr_char_t *aDir)
{
    xpr_bool_t sResult = ::CreateDirectoryA(aDir, XPR_NULL);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_bool_t FileSys::mkdir(const xpr_wchar_t *aDir)
{
    xpr_bool_t sResult = ::CreateDirectoryW(aDir, XPR_NULL);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::mkdir_recursive(const xpr_char_t *aDir)
{
    if (mkdir(aDir) == XPR_RCODE_SUCCESS)
    {
        return XPR_RCODE_SUCCESS;
    }

    xpr_rcode_t sRcode  = XPR_RCODE_SUCCESS;
    xpr_bool_t  sResult = XPR_TRUE;
    xpr::string sDir(aDir);
    xpr_size_t  sSplit  = 0;

    if (sDir.length() >= 3 &&
        sDir[1] == XPR_MBCS_STRING_LITERAL(':') && sDir[2] ==XPR_FILE_SEPARATOR)
    {
        // drive
        sSplit = 3;
    }

    do
    {
        sSplit = sDir.find(XPR_FILE_SEPARATOR, sSplit);
        if (sSplit == xpr::string::npos)
        {
            break;
        }

        sDir[sSplit] = XPR_MBCS_STRING_LITERAL('\0');

        if (exist(sDir.c_str()) == XPR_FALSE)
        {
            if (::CreateDirectoryA(sDir.c_str(), XPR_NULL) == XPR_FALSE)
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();

                sResult = XPR_FALSE;
            }
        }

        sDir[sSplit] = XPR_FILE_SEPARATOR;
        ++sSplit;

    } while (XPR_IS_TRUE(sResult));

    if (XPR_IS_TRUE(sResult))
    {
        if (exist(sDir.c_str()) == XPR_FALSE)
        {
            if (::CreateDirectoryA(sDir.c_str(), XPR_NULL) == XPR_FALSE)
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();

                sResult = XPR_FALSE;
            }
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        sResult = XPR_TEST_BITS(GetFileAttributesA(sDir.c_str()), FILE_ATTRIBUTE_DIRECTORY);
        if (XPR_IS_FALSE(sResult))
        {
            sRcode = XPR_RCODE_ENOTDIR;
        }
    }

    return sRcode;
}

xpr_rcode_t FileSys::mkdir_recursive(const xpr_wchar_t *aDir)
{
    if (mkdir(aDir) == XPR_RCODE_SUCCESS)
    {
        return XPR_RCODE_SUCCESS;
    }

    xpr_rcode_t  sRcode  = XPR_RCODE_SUCCESS;
    xpr_bool_t   sResult = XPR_TRUE;
    xpr::wstring sDir(aDir);
    xpr_size_t   sSplit  = 0;

    if (sDir.length() >= 3 &&
        sDir[1] == XPR_WIDE_STRING_LITERAL(':') && sDir[2] == XPR_FILE_SEPARATOR)
    {
        // drive
        sSplit = 3;
    }

    do
    {
        sSplit = sDir.find(XPR_FILE_SEPARATOR, sSplit);
        if (sSplit == xpr::string::npos)
        {
            break;
        }

        sDir[sSplit] = XPR_WIDE_STRING_LITERAL('\0');

        if (exist(sDir.c_str()) == XPR_FALSE)
        {
            if (::CreateDirectoryW(sDir.c_str(), XPR_NULL) == XPR_FALSE)
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();

                sResult = XPR_FALSE;
            }
        }

        sDir[sSplit] = XPR_FILE_SEPARATOR;
        ++sSplit;

    } while (XPR_IS_TRUE(sResult));

    if (XPR_IS_TRUE(sResult))
    {
        if (exist(sDir.c_str()) == XPR_FALSE)
        {
            if (::CreateDirectoryW(sDir.c_str(), XPR_NULL) == XPR_FALSE)
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();

                sResult = XPR_FALSE;
            }
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        sResult = XPR_TEST_BITS(GetFileAttributesW(sDir.c_str()), FILE_ATTRIBUTE_DIRECTORY);
        if (XPR_IS_FALSE(sResult))
        {
            sRcode = XPR_RCODE_ENOTDIR;
        }
    }

    return sRcode;
}

xpr_rcode_t FileSys::rmdir(const xpr_char_t *aDir)
{
    xpr_bool_t sResult = RemoveDirectoryA(aDir);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::rmdir(const xpr_wchar_t *aDir)
{
    xpr_bool_t sResult = RemoveDirectoryW(aDir);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::remove(const xpr_char_t *aFilePath)
{
    xpr_bool_t sResult = ::DeleteFileA(aFilePath);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::remove(const xpr_wchar_t *aFilePath)
{
    xpr_bool_t sResult = ::DeleteFileW(aFilePath);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::rename(const xpr_char_t *aOldFilePath, const xpr_char_t *aNewFilePath)
{
    xpr_bool_t sResult = ::MoveFileA(aOldFilePath, aNewFilePath);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::rename(const xpr_wchar_t *aOldFilePath, const xpr_wchar_t *aNewFilePath)
{
    xpr_bool_t sResult = ::MoveFileW(aOldFilePath, aNewFilePath);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_uint64_t FileSys::getFileSize(const xpr_char_t *aFilePath)
{
    WIN32_FIND_DATAA sWin32FindData = {0};

    HANDLE sFindFile = ::FindFirstFileA(aFilePath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return 0;

    ::FindClose(sFindFile);

    if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return 0;

    xpr_uint64_t sFileSize = sWin32FindData.nFileSizeLow;
    sFileSize += (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;

    return sFileSize;
}

xpr_uint64_t FileSys::getFileSize(const xpr_wchar_t *aFilePath)
{
    WIN32_FIND_DATAW sWin32FindData = {0};

    HANDLE sFindFile = ::FindFirstFileW(aFilePath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return 0;

    ::FindClose(sFindFile);

    if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return 0;

    xpr_uint64_t sFileSize = sWin32FindData.nFileSizeLow;
    sFileSize += (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;

    return sFileSize;
}

xpr_rcode_t FileSys::chdir(const xpr_char_t *aCurDir)
{
    xpr_bool_t sResult = ::SetCurrentDirectoryA(aCurDir);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::chdir(const xpr_wchar_t *aCurDir)
{
    xpr_bool_t sResult = ::SetCurrentDirectoryW(aCurDir);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getcwd(xpr_char_t *aCurDir, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetCurrentDirectoryA(aMaxLen, aCurDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    xpr_size_t sLen = strlen(aCurDir);

    if (aCurDir[sLen - 1] == XPR_FILE_SEPARATOR)
    {
        aCurDir[sLen - 1] = XPR_MBCS_STRING_LITERAL('\0');
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getcwd(xpr_wchar_t *aCurDir, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetCurrentDirectoryW(aMaxLen, aCurDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    xpr_size_t sLen = wcslen(aCurDir);

    if (aCurDir[sLen - 1] == XPR_FILE_SEPARATOR)
    {
        aCurDir[sLen - 1] = XPR_WIDE_STRING_LITERAL('\0');
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExePath(xpr_char_t *aExePath, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetModuleFileNameA(XPR_NULL, aExePath, aMaxLen);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExePath(xpr_wchar_t *aExePath, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetModuleFileNameW(XPR_NULL, aExePath, aMaxLen);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExeDir(xpr_char_t *aExeDir, xpr_size_t aMaxLen)
{
    xpr_rcode_t sRcode;
    xpr_char_t  sExePath[XPR_MAX_PATH + 1] = {0};

    sRcode = getExePath(sExePath, XPR_MAX_PATH);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_char_t *sSplit = strrchr(sExePath, XPR_FILE_SEPARATOR);
    if (XPR_IS_NULL(sSplit))
    {
        return XPR_RCODE_ENOENT;
    }

    *sSplit = 0;

    xpr_size_t sLen = (xpr_size_t)(sSplit - sExePath);
    if (sLen > aMaxLen)
    {
        return XPR_RCODE_E2BIG;
    }

    strcpy(aExeDir, sExePath);

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExeDir(xpr_wchar_t *aExeDir, xpr_size_t aMaxLen)
{
    xpr_rcode_t sRcode;
    xpr_wchar_t sExePath[XPR_MAX_PATH + 1] = {0};

    sRcode = getExePath(sExePath, XPR_MAX_PATH);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_wchar_t *sSplit = wcsrchr(sExePath, XPR_FILE_SEPARATOR);
    if (XPR_IS_NULL(sSplit))
    {
        return XPR_RCODE_ENOENT;
    }

    *sSplit = 0;

    xpr_size_t sLen = wcslen(sExePath);
    if (sLen > aMaxLen)
    {
        return XPR_RCODE_E2BIG;
    }

    wcscpy(aExeDir, sExePath);

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getTempDir(xpr_char_t *aTempDir, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetTempPathA(aMaxLen, aTempDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    xpr_size_t sLen = strlen(aTempDir);

    if (aTempDir[sLen - 1] == XPR_FILE_SEPARATOR)
    {
        aTempDir[sLen - 1] = 0;
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getTempDir(xpr_wchar_t *aTempDir, xpr_size_t aMaxLen)
{
    DWORD sResult = ::GetTempPathW(aMaxLen, aTempDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    xpr_size_t sLen = wcslen(aTempDir);

    if (aTempDir[sLen - 1] == XPR_FILE_SEPARATOR)
    {
        aTempDir[sLen - 1] = 0;
    }

    return XPR_RCODE_SUCCESS;
}
#endif // XPR_CFG_OS_WINDOWS
} // namespace xpr
