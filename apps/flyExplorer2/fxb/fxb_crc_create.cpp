//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_crc_create.h"

#include "fxb_crc_checksum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
CrcCreate::CrcCreate(void)
    : mHwnd(XPR_NULL), mMsg(XPR_NULL)
    , mProcessedCount(0), mSucceededCount(0)
    , mMethod(0), mEach(XPR_FALSE)
    , mStatus(StatusNone)
{
}

CrcCreate::~CrcCreate(void)
{
    Stop();

    mPathDeque.clear();
}

void CrcCreate::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void CrcCreate::setMethod(xpr_sint_t aMethod)
{
    mMethod = aMethod;
}

void CrcCreate::setEach(xpr_bool_t aEach)
{
    mEach = aEach;
}

void CrcCreate::setPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPath = aPath;
}

void CrcCreate::addPath(const std::tstring &aPath)
{
    mPathDeque.push_back(aPath);
}

xpr_size_t CrcCreate::getCount(void)
{
    return mPathDeque.size();
}

const xpr_tchar_t *CrcCreate::getPath(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mPathDeque))
        return XPR_NULL;

    return mPathDeque[aIndex].c_str();
}

xpr_bool_t CrcCreate::OnPreEntry(void)
{
    mProcessedCount = 0;
    mSucceededCount = 0;
    mStatus = StatusCreating;

    return XPR_TRUE;
}

unsigned CrcCreate::OnEntryProc(void)
{
    FILE *sFile = XPR_NULL;
    if (XPR_IS_FALSE(mEach))
    {
        sFile = _tfopen(mPath.c_str(), XPR_STRING_LITERAL("wt"));
        if (XPR_IS_NOT_NULL(sFile))
        {
            if (mMethod == 0)
            {
                fprintf(sFile, "; Generated by WIN-SFV32 v1.0");
                fprintf(sFile, " (Compatible: flyExplorer)\n");
            }
        }
    }

    if (XPR_IS_TRUE(mEach) || (XPR_IS_FALSE(mEach) && XPR_IS_NOT_NULL(sFile)))
    {
        xpr_ulong_t sCrcSfv = 0; // sfv
        xpr_char_t sCrcMd5[100]; // md5

        xpr_sint_t i;
        xpr_bool_t sResult = XPR_TRUE;
        const xpr_tchar_t *sExt;
        xpr_char_t sFileNameA[XPR_MAX_PATH + 1];
        xpr_size_t sOutputBytes;
        std::tstring sPath;
        std::tstring sFileName;
        std::tstring sCrcPath;
        PathDeque::iterator sIterator;

        sIterator = mPathDeque.begin();
        for (i = 0; sIterator != mPathDeque.end(); ++sIterator, ++i)
        {
            if (IsStop() == XPR_TRUE)
                break;

            sPath = *sIterator;

            if (XPR_IS_TRUE(mEach))
            {
                sCrcPath = sPath;

                sExt = GetFileExt(sCrcPath.c_str());
                if (XPR_IS_NOT_NULL(sExt))
                    sCrcPath.erase(sCrcPath.length()-_tcslen(sExt), _tcslen(sExt));

                sCrcPath += (mMethod == 0) ? XPR_STRING_LITERAL(".sfv") : XPR_STRING_LITERAL(".md5");

                sFile = _tfopen(sCrcPath.c_str(), XPR_STRING_LITERAL("wt"));
                if (XPR_IS_NULL(sFile))
                    continue;

                if (mMethod == 0)
                {
                    fprintf(sFile, "; Generated by WIN-SFV32 v1.0");
                    fprintf(sFile, " (Compatible: flyExplorer)\n");
                }
            }

            sCrcSfv = 0;
            sCrcMd5[0] = '\0';
            if (mMethod == 0) getFileCrcSfv(sPath.c_str(), &sCrcSfv);
            else              getFileCrcMd5(sPath.c_str(), sCrcMd5);

            sFileName = sPath.substr(sPath.rfind(XPR_STRING_LITERAL('\\'))+1);

            if (mMethod == 0)
            {
                sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
                XPR_TCS_TO_MBS(sFileName.c_str(), sFileName.length() * sizeof(xpr_tchar_t), sFileNameA, &sOutputBytes);
                sFileNameA[sOutputBytes / sizeof(xpr_char_t)] = 0;

                fprintf(sFile, "%s %08X\n", sFileNameA, sCrcSfv);
            }
            else
            {
                if (sCrcMd5[0] != '\0')
                {
                    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
                    XPR_TCS_TO_MBS(sFileName.c_str(), sFileName.length() * sizeof(xpr_tchar_t), sFileNameA, &sOutputBytes);
                    sFileNameA[sOutputBytes / sizeof(xpr_char_t)] = 0;

                    fprintf(sFile, "%s %s\n", _strupr(sCrcMd5), sFileNameA);
                }
            }

            if (XPR_IS_TRUE(mEach))
                fclose(sFile);

            {
                CsLocker sLocker(mCs);

                mProcessedCount++;
                if (XPR_IS_TRUE(sResult))
                    mSucceededCount++;
            }
        }

        if (XPR_IS_FALSE(mEach))
            fclose(sFile);

        {
            CsLocker sLocker(mCs);
            mStatus = IsStop() ? StatusStopped : StatusCreateCompleted;
        }
    }
    else
    {
        CsLocker sLocker(mCs);
        mStatus = StatusFailed;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

CrcCreate::Status CrcCreate::getStatus(xpr_size_t *aProcessedCount, xpr_size_t *aSucceededCount)
{
    CsLocker sLocker(mCs);

    if (XPR_IS_NOT_NULL(aProcessedCount)) *aProcessedCount = mProcessedCount;
    if (XPR_IS_NOT_NULL(aSucceededCount)) *aSucceededCount = mSucceededCount;

    return mStatus;
}
} // namespace fxb