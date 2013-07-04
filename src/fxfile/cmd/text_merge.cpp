//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "text_merge.h"

#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
static const xpr_size_t kTextBufferSize = 16 * 1024; // 16KB

TextMerge::TextMerge(void)
    : mHwnd(NULL), mMsg(NULL)
    , mStatus(StatusNone)
    , mEncoding(XPR_FALSE)
{
}

TextMerge::~TextMerge(void)
{
    Stop();

    mPathDeque.clear();
}

void TextMerge::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void TextMerge::setEncoding(xpr_bool_t bEncoding)
{
    mEncoding = bEncoding;
}

void TextMerge::setTextFile(const xpr_tchar_t *aTextFile)
{
    if (XPR_IS_NOT_NULL(aTextFile))
       mTextFile = aTextFile;
}

void TextMerge::addPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPathDeque.push_back(aPath);
}

xpr_bool_t TextMerge::OnPreEntry(void)
{
    mSucceededCount = 0;
    mStatus = StatusMerging;

    return XPR_TRUE;
}

unsigned TextMerge::OnEntryProc(void)
{
    xpr_rcode_t sRcode;
    xpr_sint_t sOpenMode;
    xpr::FileIo sMergeFileIo;

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sMergeFileIo.open(mTextFile.c_str(), sOpenMode);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        xpr::TextFileWriter sTextFileWriter(sMergeFileIo);

        sTextFileWriter.setEncoding(XPR_IS_TRUE(mEncoding) ? xpr::CharSetUtf16 : xpr::CharSetMultiBytes);
        sTextFileWriter.setEndOfLine(xpr::TextFileWriter::kUnixStyle);
        sTextFileWriter.writeBom();

        PathDeque::iterator sIterator;
        xpr::tstring sPath;
        xpr_tchar_t *sText = new xpr_tchar_t[kTextBufferSize + 1];
        xpr_ssize_t sTextReadLen;

        sIterator = mPathDeque.begin();
        for (; sIterator != mPathDeque.end(); ++sIterator)
        {
            if (IsStop() == XPR_TRUE)
                break;

            sPath = *sIterator;

            xpr::FileIo sFileIo;

            sRcode = sFileIo.open(sPath.c_str(), xpr::FileIo::OpenModeReadOnly);
            if (XPR_RCODE_IS_ERROR(sRcode))
                continue;

            xpr::TextFileReader sTextFileReader(sFileIo);

            if (sTextFileReader.detectEncoding() == xpr::CharSetNone)
                sTextFileReader.setEncoding(xpr::CharSetMultiBytes);

            do
            {
                sRcode = sTextFileReader.read(sText, kTextBufferSize, &sTextReadLen);
                if (XPR_RCODE_IS_ERROR(sRcode) || sTextReadLen == 0)
                    break;

                sText[sTextReadLen] = 0;

                sTextFileWriter.write(sText,
                                      sTextReadLen * sizeof(xpr_tchar_t),
                                      (sizeof(xpr_tchar_t) == 2) ? xpr::CharSetUtf16 : xpr::CharSetMultiBytes);

            } while (IsStop() == XPR_FALSE);

            sFileIo.close();

            {
                xpr::MutexGuard sLockGuard(mMutex);
                mSucceededCount++;
            }
        }

        sMergeFileIo.close();

        XPR_SAFE_DELETE_ARRAY(sText);
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = (IsStop() == XPR_TRUE) ? StatusStopped : StatusMergeCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)0, (LPARAM)0);

    return 0;
}

TextMerge::Status TextMerge::getStatus(xpr_size_t *aSucceededCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aSucceededCount)) *aSucceededCount = mSucceededCount;

    return mStatus;
}
} // namespace cmd
} // namespace fxfile
