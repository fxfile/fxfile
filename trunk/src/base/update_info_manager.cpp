//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "update_info_manager.h"

namespace fxfile
{
namespace base
{
static const xpr_tchar_t kEnvAppDataName[] = XPR_STRING_LITERAL("APPDATA");

static const xpr_tchar_t kProgramDir    [] = XPR_STRING_LITERAL("fxfile");
static const xpr_tchar_t kUpdateDir     [] = XPR_STRING_LITERAL("update");

UpdateInfoManager::UpdateInfoManager(void)
    : mInitialized(XPR_FALSE)
    , mProcessMutexKey(0)
    , mShmKey(0), mShmAttachedAddr(XPR_NULL)
{
}

UpdateInfoManager::~UpdateInfoManager(void)
{
    close();
}

void UpdateInfoManager::close(void)
{
    mProcessMutex.close();

    if (XPR_IS_TRUE(mShm.isOpened()))
    {
        mShm.detach(mShmAttachedAddr);
        mShm.close();
    }

    mShmAttachedAddr = XPR_NULL;
}

xpr_bool_t UpdateInfoManager::getDefaultUpdateHomeDir(xpr::tstring &aDir)
{
    XPR_ASSERT(aDir != XPR_NULL);

    xpr_bool_t  sResult = XPR_FALSE;
    xpr_tchar_t sUpdateHomeDir[XPR_MAX_PATH + 1];
    xpr_rcode_t sRcode;

    sRcode = xpr::getEnv(kEnvAppDataName, sUpdateHomeDir, XPR_MAX_PATH);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        _tcscat(sUpdateHomeDir, XPR_FILE_SEPARATOR_STRING);
        _tcscat(sUpdateHomeDir, kProgramDir);
        _tcscat(sUpdateHomeDir, XPR_FILE_SEPARATOR_STRING);
        _tcscat(sUpdateHomeDir, kUpdateDir);

        sRcode = xpr::FileSys::mkdir_recursive(sUpdateHomeDir);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            aDir = sUpdateHomeDir;

            return XPR_TRUE;
        }
    }

    sRcode = xpr::FileSys::getTempDir(sUpdateHomeDir, XPR_MAX_PATH);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        _tcscat(sUpdateHomeDir, XPR_FILE_SEPARATOR_STRING);
        _tcscat(sUpdateHomeDir, kProgramDir);
        _tcscat(sUpdateHomeDir, XPR_FILE_SEPARATOR_STRING);
        _tcscat(sUpdateHomeDir, kUpdateDir);

        sRcode = xpr::FileSys::mkdir_recursive(sUpdateHomeDir);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            aDir = sUpdateHomeDir;

            return XPR_TRUE;
        }
    }

    return XPR_FALSE;
}

void UpdateInfoManager::setUpdateHomeDir(const xpr::tstring &aDir)
{
    mUpdateHomeDir = aDir;
}

/**
 * get shared memory key and semaphore key from '.key' file
 */
xpr_rcode_t UpdateInfoManager::openUpdateInfo(void)
{
    xpr_rcode_t  sRcode;
    xpr::tstring sKeyFilePath;
    xpr::FileIo  sFileIo;
    xpr_sint_t   sOpenMode;
    xpr_ssize_t  sReadSize;

    getKeyFilePath(sKeyFilePath);

    sOpenMode = xpr::FileIo::OpenModeReadOnly;

    sRcode = sFileIo.open(sKeyFilePath.c_str(), sOpenMode);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sRcode = sFileIo.read(&mProcessMutexKey, sizeof(xpr_key_t), &sReadSize);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            sRcode = sFileIo.read(&mShmKey, sizeof(xpr_key_t), &sReadSize);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                // nothing to do
            }
        }
    }

    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        close();

        return sRcode;
    }

    sRcode = mProcessMutex.open(mProcessMutexKey);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sRcode = mShm.open(mShmKey);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            sRcode = mShm.attach(&mShmAttachedAddr);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                // nothing to do
            }
        }
    }

    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        mInitialized = XPR_TRUE;
    }
    else
    {
        close();
    }

    return sRcode;
}

/**
 * read update information from shared memory
 */
xpr_rcode_t UpdateInfoManager::readUpdateInfo(UpdateInfo &aUpdateInfo)
{
    if (XPR_IS_FALSE(mInitialized))
    {
        return XPR_RCODE_EFAULT;
    }

    {
        xpr::ProcessMutexGuard sLockGuard(mProcessMutex);

        memcpy(&aUpdateInfo, mShmAttachedAddr, sizeof(UpdateInfo));
    }

    return XPR_RCODE_SUCCESS;
}

void UpdateInfoManager::getCheckedVersion(const UpdateInfo &aUpdateInfo, xpr_tchar_t *aCheckedVersion, xpr_size_t aMaxLen)
{
    XPR_ASSERT(aCheckedVersion != XPR_NULL);

    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sInputBytes = wcslen(aUpdateInfo.mCheckedVersion) * sizeof(xpr_wchar_t);
    sOutputBytes = aMaxLen * sizeof(xpr_tchar_t);
    XPR_UTF16_TO_TCS(aUpdateInfo.mCheckedVersion, &sInputBytes, aCheckedVersion, &sOutputBytes);
    aCheckedVersion[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
}

void UpdateInfoManager::getDownloadedFilePath(const UpdateInfo &aUpdateInfo, xpr_tchar_t *aDownloadedFilePath, xpr_size_t aMaxLen)
{
    XPR_ASSERT(aDownloadedFilePath != XPR_NULL);

    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sInputBytes = wcslen(aUpdateInfo.mDownloadedFilePath) * sizeof(xpr_wchar_t);
    sOutputBytes = aMaxLen * sizeof(xpr_tchar_t);
    XPR_UTF16_TO_TCS(aUpdateInfo.mDownloadedFilePath, &sInputBytes, aDownloadedFilePath, &sOutputBytes);
    aDownloadedFilePath[sOutputBytes / sizeof(xpr_tchar_t)] = 0;
}

/**
 * generate shared memory key or process mutex key
 */
xpr_key_t UpdateInfoManager::generateKey(xpr_key_t aOldKey)
{
    xpr_key_t sKey;

#if defined(XPR_CFG_OS_WINDOWS)

    sKey = aOldKey;

    if (sKey == 0)
    {
        xpr_pid_t sPid = xpr::getPid();

        sKey = ((xpr_key_t)sPid) << 16;
    }
    else
    {
        xpr_key_t sPid   = sKey >> 16;
        xpr_key_t sIndex = sKey & 0xffff;

        ++sIndex;

        if (sIndex > 0xffff)
        {
            ++sPid;
            sIndex = 0xff;
        }

        sKey = (sPid << 16) | sIndex;
    }

#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <update_info_manager.cpp>
#endif

    return sKey;
}

void UpdateInfoManager::getKeyFilePath(xpr::tstring &aFilePath)
{
    aFilePath  = mUpdateHomeDir;
    aFilePath += XPR_FILE_SEPARATOR_STRING;
    aFilePath += kUpdateKeyFileName;
}

void UpdateInfoManager::initUpdateInfo(UpdateInfo &aUpdateInfo)
{
    memset(&aUpdateInfo, 0, sizeof(UpdateInfo));
}

/**
 * initialize update information on shared memory
 *
 *   1) generate shared memory key and semaphore key
 *   2) write shared memory key and semaphore key to '.key' file
 *   3) write initialized update information on shared memory
 */
xpr_rcode_t UpdateInfoManager::createUpdateInfo(void)
{
    xpr_rcode_t sRcode;

    // create process mutex object
    do
    {
        mProcessMutexKey = generateKey(mProcessMutexKey);

        sRcode = mProcessMutex.create(mProcessMutexKey);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            break;
        }
    } while (true);

    // create shared memory object
    do
    {
        mShmKey = generateKey(mShmKey);

        sRcode = mShm.create(mShmKey, sizeof(UpdateInfo), 755);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            break;
        }
    } while (true);

    // attach shared memory
    sRcode = mShm.attach(&mShmAttachedAddr);
    if (XPR_RCODE_IS_ERROR(sRcode))
    {
        close();

        return sRcode;
    }

    // initialized update info on shared memory
    {
        xpr::ProcessMutexGuard sLockGuard(mProcessMutex);

        UpdateInfo sUpdateInfo = {0};
        initUpdateInfo(sUpdateInfo);

        memcpy(mShmAttachedAddr, &sUpdateInfo, sizeof(UpdateInfo));
    }

    // write shared memory key and process mutex key to '.key' file
    xpr::tstring sKeyFilePath;
    xpr::FileIo  sFileIo;
    xpr_sint_t   sOpenMode;
    xpr_ssize_t  sWrittenSize;

    getKeyFilePath(sKeyFilePath);

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;

    sRcode = sFileIo.open(sKeyFilePath.c_str(), sOpenMode);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        sRcode = sFileIo.write(&mProcessMutexKey, sizeof(xpr_key_t), &sWrittenSize);
        if (XPR_RCODE_IS_SUCCESS(sRcode))
        {
            sRcode = sFileIo.write(&mShmKey, sizeof(xpr_key_t), &sWrittenSize);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                // nothing to do
            }
        }
    }

    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        mInitialized = XPR_TRUE;
    }
    else
    {
        close();

        xpr::FileSys::remove(sKeyFilePath.c_str());
    }

    return sRcode;
}

/**
 * write update information on shared memory
 */
xpr_rcode_t UpdateInfoManager::writeUpdateInfo(const UpdateInfo &aUpdateInfo)
{
    if (XPR_IS_FALSE(mInitialized))
    {
        return XPR_RCODE_EFAULT;
    }

    {
        xpr::ProcessMutexGuard sLockGuard(mProcessMutex);

        memcpy(mShmAttachedAddr, &aUpdateInfo, sizeof(UpdateInfo));
    }

    return XPR_RCODE_SUCCESS;
}
} // namespace base
} // namespace fxfile
