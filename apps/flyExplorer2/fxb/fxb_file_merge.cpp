//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
#define DEFAULT_BUFFER_SIZE (16*1024)

FileMerge::FileMerge(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mBufferSize(DEFAULT_BUFFER_SIZE)
    , mMergedCount(0)
{
}

FileMerge::~FileMerge(void)
{
}

void FileMerge::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void FileMerge::addPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPathDeque.push_back(aPath);
}

void FileMerge::setDestPath(const xpr_tchar_t *aDestPath)
{
    if (XPR_IS_NOT_NULL(aDestPath))
        mDestPath = aDestPath;
}

void FileMerge::setBufferSize(DWORD aBufferSize)
{
    mBufferSize = aBufferSize;
}

xpr_bool_t FileMerge::OnPreEntry(void)
{
    std::tstring sDestDir = mDestPath;
    xpr_size_t sFind = sDestDir.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != std::tstring::npos)
        sDestDir = sDestDir.substr(0, sFind);

    if (IsExistFile(sDestDir) == XPR_FALSE)
    {
        mStatus = StatusDestDirNotExist;
        return XPR_FALSE;
    }

    mStatus = StatusMerging;
    mMergedCount = 0;

    return XPR_TRUE;
}

unsigned FileMerge::OnEntryProc(void)
{
    Status sStatus = StatusNone;

    FILE *sDestFile = _tfopen(mDestPath.c_str(), XPR_STRING_LITERAL("wb"));
    if (XPR_IS_NOT_NULL(sDestFile))
    {
        FILE *sFile;
        xpr_size_t sRead;
        xpr_char_t *sBuffer;

        PathDeque::iterator sIterator;
        std::tstring sPath;

        sBuffer = new xpr_char_t[mBufferSize];

        sIterator = mPathDeque.begin();
        for (; sIterator != mPathDeque.end(); ++sIterator)
        {
            sPath = *sIterator;

            sFile = _tfopen(sPath.c_str(), XPR_STRING_LITERAL("rb"));
            if (XPR_IS_NOT_NULL(sFile))
            {
                {
                    CsLocker sLocker(mCs);
                    mMergedCount++;
                }

                while (IsStop() == XPR_FALSE)
                {
                    sRead = fread(sBuffer, 1, mBufferSize, sFile);
                    if (sRead == 0)
                        break;

                    fwrite(sBuffer, sRead, 1, sDestFile);
                }

                fclose(sFile);
            }

            if (IsStop() == XPR_TRUE)
                break;
        }

        fclose(sDestFile);
        XPR_SAFE_DELETE_ARRAY(sBuffer);

        sStatus = StatusMergeCompleted;
    }
    else
    {
        sStatus = StatusNotWritable;
    }

    if (IsStop())
        sStatus = StatusStopped;

    {
        CsLocker sLocker(mCs);
        mStatus = sStatus;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileMerge::Status FileMerge::getStatus(xpr_size_t *sMergedCount)
{
    CsLocker sLocker(mCs);

    if (XPR_IS_NOT_NULL(sMergedCount)) *sMergedCount = mMergedCount;

    return mStatus;
}
} // namespace fxb
