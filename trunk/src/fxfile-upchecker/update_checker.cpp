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
namespace upchecker
{
UpdateChecker::UpdateChecker(void)
    : mCheckInProgress(XPR_FALSE), mChecked(XPR_FALSE)
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

void UpdateChecker::setUrl(const xpr::string &aUrl)
{
    mUrl = aUrl;
}

void UpdateChecker::setDir(const xpr::string &aDir)
{
    mDir = aDir;
}

struct DownloadFileData
{
    xpr_bool_t  mFirst;
    xpr_sint_t  mSleepTime;
    xpr::string mFilePath;
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

    xpr_size_t  sSplit;
    xpr::string sMetaFileName;
    xpr::string sUrl = mUrl;

    sSplit = sUrl.find_last_of(XPR_STRING_LITERAL('/'));
    if (sSplit == xpr::string::npos)
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

const MetaDataFile::MetaData &UpdateChecker::getMetaData(void) const
{
    return mMetaData;
}
} // namespace fxfile
} // namespace upchecker
