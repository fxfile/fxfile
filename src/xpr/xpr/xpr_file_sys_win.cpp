//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_file_sys.h"
#include "xpr_rcode.h"
#include "xpr_string.h"
#include "xpr_bit.h"

#include <direct.h>

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
xpr_bool_t FileSys::exist(const xpr::string &aFilePath)
{
    xpr_bool_t sResult = XPR_FALSE;

    HANDLE          sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};

    sFindFile = ::FindFirstFile(aFilePath.c_str(), &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        ::FindClose(sFindFile);
        sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t FileSys::mkdir(const xpr::string &aDir)
{
    xpr_bool_t sResult = ::CreateDirectory(aDir.c_str(), XPR_NULL);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::mkdir_recursive(const xpr::string &aDir)
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
            if (::CreateDirectory(sDir.c_str(), XPR_NULL) == XPR_FALSE)
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
            if (::CreateDirectory(sDir.c_str(), XPR_NULL) == XPR_FALSE)
            {
                sRcode = XPR_RCODE_GET_OS_ERROR();

                sResult = XPR_FALSE;
            }
        }
    }

    if (XPR_IS_TRUE(sResult))
    {
        sResult = XPR_TEST_BITS(::GetFileAttributes(sDir.c_str()), FILE_ATTRIBUTE_DIRECTORY);
        if (XPR_IS_FALSE(sResult))
        {
            sRcode = XPR_RCODE_ENOTDIR;
        }
    }

    return sRcode;
}

xpr_rcode_t FileSys::rmdir(const xpr::string &aDir)
{
    xpr_bool_t sResult = ::RemoveDirectory(aDir.c_str());
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::remove(const xpr::string &aFilePath)
{
    xpr_bool_t sResult = ::DeleteFile(aFilePath.c_str());
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::rename(const xpr::string &aOldFilePath, const xpr::string &aNewFilePath)
{
    xpr_bool_t sResult = ::MoveFile(aOldFilePath.c_str(), aNewFilePath.c_str());
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_uint64_t FileSys::getFileSize(const xpr::string &aFilePath)
{
    WIN32_FIND_DATA sWin32FindData = {0};

    HANDLE sFindFile = ::FindFirstFile(aFilePath.c_str(), &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return 0;

    ::FindClose(sFindFile);

    if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        return 0;

    xpr_uint64_t sFileSize = sWin32FindData.nFileSizeLow;
    sFileSize += (xpr_uint64_t)sWin32FindData.nFileSizeHigh * (xpr_uint64_t)kuint32max;

    return sFileSize;
}

xpr_rcode_t FileSys::chdir(const xpr::string &aCurDir)
{
    xpr_bool_t sResult = ::SetCurrentDirectory(aCurDir.c_str());
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getcwd(xpr::string &aCurDir)
{
    aCurDir.clear();
    aCurDir.reserve(XPR_MAX_PATH + 1);

    xpr_tchar_t *sCurDir = const_cast<xpr_tchar_t *>(aCurDir.data());

    DWORD sResult = ::GetCurrentDirectory(XPR_MAX_PATH, sCurDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    aCurDir.update();

    if (aCurDir.back() == XPR_FILE_SEPARATOR)
    {
        aCurDir.erase(aCurDir.length() - 1);
    }

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExePath(xpr::string &aExePath)
{
    aExePath.clear();
    aExePath.reserve(XPR_MAX_PATH + 1);

    xpr_tchar_t *sExePath = const_cast<xpr_tchar_t *>(aExePath.data());

    DWORD sResult = ::GetModuleFileName(XPR_NULL, sExePath, XPR_MAX_PATH);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    aExePath.update();

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getExeDir(xpr::string &aExeDir)
{
    xpr_rcode_t sRcode;
    xpr::string sExePath;

    sRcode = getExePath(sExePath);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        return sRcode;
    }

    xpr_size_t sOffset = sExePath.rfind(XPR_FILE_SEPARATOR);
    if (sOffset == xpr::string::npos)
    {
        return XPR_RCODE_ENOENT;
    }

    sExePath.erase(sOffset);

    aExeDir = sExePath;

    return XPR_RCODE_SUCCESS;
}

xpr_rcode_t FileSys::getTempDir(xpr::string &aTempDir)
{
    aTempDir.clear();
    aTempDir.reserve(XPR_MAX_PATH + 1);

    xpr_tchar_t *sTempDir = const_cast<xpr_tchar_t *>(aTempDir.data());

    DWORD sResult = ::GetTempPath(XPR_MAX_PATH, sTempDir);
    if (sResult == 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

    aTempDir.update();

    if (aTempDir.back() == XPR_FILE_SEPARATOR)
    {
        aTempDir.erase(aTempDir.length() - 1);
    }

    return XPR_RCODE_SUCCESS;
}
#endif // XPR_CFG_OS_WINDOWS
} // namespace xpr
