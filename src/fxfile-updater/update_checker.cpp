//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "update_checker.h"

#include "conf_file_ex.h"
#include "crc32.h"
#include "md5.h"
#include "sha.h"

#include "curl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace updater
{
UpdateChecker::UpdateChecker(void)
    : mCheckMinorVer(XPR_TRUE)
    , mCheckInProgress(XPR_FALSE), mChecked(XPR_FALSE)
    , mNewVersion(XPR_FALSE)
    , mDownloadInProgress(XPR_FALSE), mDownloaded(XPR_FALSE)
{
}

UpdateChecker::~UpdateChecker(void)
{
}

void UpdateChecker::initStatic(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void UpdateChecker::finalizeStatic(void)
{
    curl_global_cleanup();
}

void UpdateChecker::setUrl(const xpr::tstring &aUrl)
{
    mUrl = aUrl;
}

void UpdateChecker::setDir(const xpr::tstring &aDir)
{
    mDir = aDir;
}

void UpdateChecker::setNowVersion(const xpr::tstring &aNowVersion, xpr_bool_t aCheckMinorVer)
{
    mNowVersion    = aNowVersion;
    mCheckMinorVer = aCheckMinorVer;
}

struct DownloadFileData
{
    xpr_bool_t   mFirst;
    xpr_sint_t   mSleepTime;
    xpr::tstring mFilePath;
};

xpr_size_t UpdateChecker::downloadFile(void *aSource, xpr_size_t aSize, xpr_size_t aMemoryBlock, void *aUserData)
{
    const xpr_size_t  sBufferSize        = aSize * aMemoryBlock;
    xpr_size_t        sWrittenBufferSize = 0;
    DownloadFileData *sDownloadFileData  = (DownloadFileData *)aUserData;

    XPR_ASSERT(sDownloadFileData != XPR_NULL);

    if (sBufferSize > 0)
    {
        xpr_rcode_t sRcode;

        if (XPR_IS_TRUE(sDownloadFileData->mFirst))
        {
            xpr::FileIo sFileIo;

            sRcode = sFileIo.open(sDownloadFileData->mFilePath.c_str(), xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                xpr_ssize_t sWrittenSize = 0;

                sRcode = sFileIo.write(aSource, sBufferSize, &sWrittenSize);
                if (XPR_RCODE_IS_SUCCESS(sRcode))
                {
                    sWrittenBufferSize = sWrittenSize;
                }
                else
                {
                    // error
                }

                sFileIo.close();
            }
            else
            {
                // error
            }

            sDownloadFileData->mFirst = XPR_FALSE;
        }
        else
        {
            xpr::FileIo sFileIo;
            xpr_ssize_t sWrittenSize = 0;

            sRcode = sFileIo.open(sDownloadFileData->mFilePath.c_str(), xpr::FileIo::OpenModeAppend);
            if (XPR_RCODE_IS_SUCCESS(sRcode))
            {
                sRcode = sFileIo.seekToEnd();
                if (XPR_RCODE_IS_SUCCESS(sRcode))
                {
                    sRcode = sFileIo.write(aSource, sBufferSize, &sWrittenSize);
                    if (XPR_RCODE_IS_SUCCESS(sRcode))
                    {
                        sWrittenBufferSize = sWrittenSize;
                    }
                    else
                    {
                        // error
                    }
                }
                else
                {
                    // error
                }

                sFileIo.close();
            }
            else
            {
                // error
            }
        }

        if (sDownloadFileData->mSleepTime > 0)
        {
            xpr::Thread::sleep(sDownloadFileData->mSleepTime);
        }
    }

    return sBufferSize;
}

/**
 * check new version
 */
xpr_bool_t UpdateChecker::check(void)
{
    CURL *sCtx = curl_easy_init();
    if (XPR_IS_NULL(sCtx))
    {
        return XPR_FALSE;
    }

    xpr_size_t   sSplit;
    xpr::tstring sMetaFileName;
    xpr::tstring sUrl = mUrl;

    sSplit = sUrl.find_last_of(XPR_STRING_LITERAL('/'));
    if (sSplit == xpr::tstring::npos)
    {
        curl_easy_cleanup(sCtx);
        return XPR_FALSE;
    }
    else
    {
        sMetaFileName = sUrl.substr(sSplit + 1);
    }

    mMetaFilePath  = mDir;
    mMetaFilePath += XPR_FILE_SEPARATOR_STRING;
    mMetaFilePath += sMetaFileName;

    DownloadFileData *sDownloadFileData = new DownloadFileData;
    sDownloadFileData->mFirst     = XPR_TRUE;
    sDownloadFileData->mSleepTime = 1;
    sDownloadFileData->mFilePath  = mMetaFilePath;

    curl_easy_setopt(sCtx, CURLOPT_NOPROGRESS,    XPR_TRUE);
    curl_easy_setopt(sCtx, CURLOPT_WRITEFUNCTION, downloadFile);
    curl_easy_setopt(sCtx, CURLOPT_URL,           sUrl.c_str());
    curl_easy_setopt(sCtx, CURLOPT_WRITEDATA,     sDownloadFileData);

    // start download
    mCheckInProgress = XPR_TRUE;

    const CURLcode sCURLcode = curl_easy_perform(sCtx);

    xpr_bool_t sResult = XPR_FALSE;
    if (sCURLcode == CURLE_OK)
    {
        // parse meta-data file
        sResult = MetaDataFile::parseMetaFile(mMetaFilePath, mMetaData);
        if (XPR_IS_TRUE(sResult))
        {
            // check version
            xpr_sint_t sMetaDataMajorVer = 0;
            xpr_sint_t sMetaDataMinorVer = 0;
            xpr_sint_t sMetaDataPatchVer = 0;
            xpr_sint_t sNowMajorVer      = 0;
            xpr_sint_t sNowMinorVer      = 0;
            xpr_sint_t sNowPatchVer      = 0;

            _stscanf(mMetaData.mVersion.c_str(),
                     XPR_STRING_LITERAL("%d.%d.%d"),
                     &sMetaDataMajorVer, &sMetaDataMinorVer, &sMetaDataPatchVer);

            _stscanf(mNowVersion.c_str(),
                     XPR_STRING_LITERAL("%d.%d.%d"),
                     &sNowMajorVer, &sNowMinorVer, &sNowPatchVer);

            if (sMetaDataMajorVer == sNowMajorVer)
            {
                if (XPR_IS_FALSE(mCheckMinorVer))
                {
                    mNewVersion = XPR_FALSE;
                }
                else
                {
                    if (sMetaDataMinorVer == sNowMinorVer)
                    {
                        if (sMetaDataPatchVer == sNowPatchVer)
                        {
                            mNewVersion = XPR_FALSE;
                        }
                        else
                        {
                            mNewVersion = (sMetaDataPatchVer > sNowPatchVer) ? XPR_TRUE : XPR_FALSE;
                        }
                    }
                    else
                    {
                        mNewVersion = (sMetaDataMinorVer > sNowMinorVer) ? XPR_TRUE : XPR_FALSE;
                    }
                }
            }
            else
            {
                mNewVersion = (sMetaDataMajorVer > sNowMajorVer) ? XPR_TRUE : XPR_FALSE;
            }
        }
    }

    XPR_SAFE_DELETE(sDownloadFileData);

    curl_easy_cleanup(sCtx);

    if (XPR_IS_TRUE(sResult))
    {
        mChecked = XPR_TRUE;
    }

    mCheckInProgress = XPR_FALSE;

    return sResult;
}

xpr_bool_t UpdateChecker::isChecked(void) const
{
    return mChecked;
}

void UpdateChecker::getCheckedVersion(xpr::tstring &aVersion)
{
    aVersion = mMetaData.mVersion;
}

xpr_bool_t UpdateChecker::existNewVersion(void) const
{
    return mNewVersion;
}

xpr_bool_t UpdateChecker::download(void)
{
    // [1] check to exist new version
    if (existNewVersion() == XPR_FALSE)
    {
        return XPR_FALSE;
    }

    // [2] download new version
    CURL *sCtx = curl_easy_init();
    if (XPR_IS_NULL(sCtx))
    {
        return XPR_FALSE;
    }

    xpr_size_t   sSplit;
    xpr::tstring sFileName;
    xpr::tstring sUrl = mMetaData.mFile;

    sSplit = sUrl.find_last_of(XPR_STRING_LITERAL('/'));
    if (sSplit == xpr::tstring::npos)
    {
        curl_easy_cleanup(sCtx);
        return XPR_FALSE;
    }
    else
    {
        sFileName = sUrl.substr(sSplit + 1);
    }

    mProgramFilePath  = mDir;
    mProgramFilePath += XPR_FILE_SEPARATOR_STRING;
    mProgramFilePath += sFileName;

    DownloadFileData *sDownloadFileData = new DownloadFileData;
    sDownloadFileData->mFirst     = XPR_TRUE;
    sDownloadFileData->mSleepTime = 1;
    sDownloadFileData->mFilePath  = mProgramFilePath;

    curl_easy_setopt(sCtx, CURLOPT_NOPROGRESS,    XPR_TRUE);
    curl_easy_setopt(sCtx, CURLOPT_WRITEFUNCTION, downloadFile);
    curl_easy_setopt(sCtx, CURLOPT_URL,           sUrl.c_str());
    curl_easy_setopt(sCtx, CURLOPT_WRITEDATA,     sDownloadFileData);

    // start download
    mDownloadInProgress = XPR_TRUE;

    const CURLcode sCURLcode = curl_easy_perform(sCtx);

    xpr_bool_t sResult = XPR_FALSE;
    if (sCURLcode == CURLE_OK)
    {
        // verify checksum (crc32, md5, sha-1) for the downloaded file
        sResult = MetaDataFile::verifyChecksum(mProgramFilePath, mMetaData);
        if (XPR_IS_TRUE(sResult))
        {
            sResult = XPR_TRUE;
        }
    }

    XPR_SAFE_DELETE(sDownloadFileData);

    curl_easy_cleanup(sCtx);

    if (XPR_IS_TRUE(sResult))
    {
        mDownloaded = XPR_TRUE;
    }

    mDownloadInProgress = XPR_FALSE;

    return sResult;
}

xpr_bool_t UpdateChecker::isDownloaded(void)
{
    return mDownloaded;
}

void UpdateChecker::getDownloadedFilePath(xpr::tstring &aFilePath)
{
    aFilePath = mProgramFilePath;
}
} // namespace fxfile
} // namespace updater
