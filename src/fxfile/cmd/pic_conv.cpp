//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "pic_conv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
PicConv::PicConv(void)
    : mHwnd(XPR_NULL)
    , mMsg(0)
    , mMsgClr(0)
    , mEvent(XPR_NULL)
    , mStatus(StatusNone)
    , mProcessedCount(0), mSucceededCount(0)
    , mKeepOrg(XPR_TRUE)
    , mGflSaveParams(XPR_NULL)
    , mAllApply(XPR_FALSE)
    , mGflMode(-1)
    , mGflModeParams(-1)
{
}

PicConv::~PicConv(void)
{
    stop();

    mPathDeque.clear();

    CLOSE_HANDLE(mEvent);
}

void PicConv::setOwner(HWND aHwnd, xpr_uint_t aMsg, xpr_uint_t aMsgClr)
{
    mHwnd   = aHwnd;
    mMsg    = aMsg;
    mMsgClr = aMsgClr;
}

void PicConv::addPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPathDeque.push_back(aPath);
}

void PicConv::setSavParams(GFL_SAVE_PARAMS *aGflSaveParams)
{
    mGflSaveParams = aGflSaveParams;
}

void PicConv::setKeepOrg(xpr_bool_t aKeepOrg)
{
    mKeepOrg = aKeepOrg;
}

void PicConv::setColorMode(xpr_bool_t aAllApply, GFL_MODE aGflMode, GFL_MODE_PARAMS aGflModeParams)
{
    mAllApply      = aAllApply;
    mGflMode       = aGflMode;
    mGflModeParams = aGflModeParams;

    if (XPR_IS_NOT_NULL(mEvent))
        ::SetEvent(mEvent);
}

xpr_bool_t PicConv::start(void)
{
    mStatus = StatusConverting;
    mProcessedCount = 0;
    mSucceededCount = 0;

    CLOSE_HANDLE(mEvent);
    mEvent = ::CreateEvent(XPR_NULL, XPR_FALSE, XPR_FALSE, XPR_NULL);

    xpr_rcode_t sRcode = mThread.start(dynamic_cast<xpr::Thread::Runnable *>(this));

    return XPR_RCODE_IS_SUCCESS(sRcode);
}

void PicConv::stop(void)
{
    mThread.stop();
    mThread.join();
}

xpr_sint_t PicConv::runThread(xpr::Thread &aThread)
{
    mAllApply = XPR_FALSE;

    GFL_ERROR sGflError;
    GFL_BITMAP *sGflBitmap = XPR_NULL;
    GFL_LOAD_PARAMS sGflLoadParams;
    gflEnableLZW(GFL_TRUE);

    xpr::string sPath;
    xpr_char_t sPathA[XPR_MAX_PATH + 1];
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;
    PathDeque::iterator sIterator;

    sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
    {
        if (mThread.isStop() == XPR_TRUE)
            break;

        sPath = *sIterator;

        sInputBytes = sPath.length() * sizeof(xpr_tchar_t);
        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
        XPR_TCS_TO_MBS(sPath.c_str(), &sInputBytes, sPathA, &sOutputBytes);
        sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

        gflGetDefaultLoadParams(&sGflLoadParams);
        if (!gflLoadBitmap(sPathA, &sGflBitmap, &sGflLoadParams, XPR_NULL))
        {
            // keep original file
            mGflSaveParams->Flags = XPR_IS_TRUE(mKeepOrg) ? GFL_SAVE_REPLACE_EXTENSION : 0;

            // save image through format
            //if (sIndex == JPEG && mLossless)
            //    gflJpegLosslessTransform(sPathA, GFL_LOSSLESS_TRANSFORM_NONE);
            //else
            sGflError = gflSaveBitmap(sPathA, sGflBitmap, mGflSaveParams);

            if (sGflError == GFL_ERROR_BAD_BITMAP)
            {
                // color depth change if bitmap is not recognizated
                if (XPR_IS_FALSE(mAllApply))
                {
                    xpr_tchar_t *sPath2 = new xpr_tchar_t[sPath.length()+1];
                    _tcscpy(sPath2, sPath.c_str());

                    if (::PostMessage(mHwnd, mMsgClr, (WPARAM)sPath2, (LPARAM)XPR_NULL) == XPR_TRUE)
                    {
                        ::WaitForSingleObject(mEvent, INFINITE);

                        if (mThread.isStop() == XPR_TRUE)
                            break;
                    }
                    else
                    {
                        XPR_SAFE_DELETE_ARRAY(sPath2);
                    }
                }

                GFL_BITMAP *sDestGflBitmap = XPR_NULL;
                gflChangeColorDepth(sGflBitmap, &sDestGflBitmap, mGflMode, mGflModeParams);
                if (XPR_IS_NOT_NULL(sDestGflBitmap))
                {
                    if (XPR_IS_NOT_NULL(sGflBitmap))
                        gflFreeBitmap(sGflBitmap);

                    sGflBitmap = sDestGflBitmap;
                }

                sGflError = gflSaveBitmap(sPathA, sGflBitmap, mGflSaveParams);
            }

            if (sGflError == GFL_NO_ERROR)
            {
                xpr::MutexGuard sLockGuard(mMutex);
                mSucceededCount++;
            }

            if (XPR_IS_NOT_NULL(sGflBitmap))
            {
                gflFreeBitmap(sGflBitmap);
                sGflBitmap = XPR_NULL;
            }
        }

        {
            xpr::MutexGuard sLockGuard(mMutex);
            mProcessedCount++;
        }
    }

    if (XPR_IS_NOT_NULL(sGflBitmap))
    {
        gflFreeBitmap(sGflBitmap);
        sGflBitmap = XPR_NULL;
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = mThread.isStop() ? StatusStopped : StatusConvertCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

xpr_size_t PicConv::getCount(void)
{
    return mPathDeque.size();
}

PicConv::Status PicConv::getStatus(xpr_size_t *aProcessedCount, xpr_size_t *aSucceededCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aProcessedCount)) *aProcessedCount = mProcessedCount;
    if (XPR_IS_NOT_NULL(aSucceededCount)) *aSucceededCount = mSucceededCount;

    return mStatus;
}
} // namespace cmd
} // namespace fxfile
