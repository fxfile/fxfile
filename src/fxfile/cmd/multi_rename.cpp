//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "multi_rename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
MultiRename::MultiRename(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mStatus(StatusNone)
    , mPreparedCount(0), mValidatedCount(0), mRenamedCount(0)
    , mInvalidItem(-1)
    , mFlags(0)
{
}

MultiRename::~MultiRename(void)
{
    Stop();

    RenDeque::iterator sIterator;
    RenItem *sRenItem;

    sIterator = mRenDeque.begin();
    for (; sIterator != mRenDeque.end(); ++sIterator)
    {
        sRenItem = *sIterator;
        XPR_SAFE_DELETE(sRenItem);
    }

    mRenDeque.clear();
}

void MultiRename::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void MultiRename::setBackupName(const xpr_tchar_t *aBackup)
{
    if (XPR_IS_NOT_NULL(aBackup))
        mBackup = aBackup;
}

void MultiRename::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

xpr_uint_t MultiRename::getFlags(void)
{
    return mFlags;
}

xpr_bool_t MultiRename::isFlag(xpr_uint_t aFlag)
{
    return XPR_TEST_BITS(mFlags, aFlag);
}

void MultiRename::addPath(const xpr_tchar_t *aDir, const xpr_tchar_t *aOld, const xpr_tchar_t *aNew)
{
    if (XPR_IS_NULL(aDir) || XPR_IS_NULL(aOld) || XPR_IS_NULL(aNew))
        return;

    RenItem *sRenItem = new RenItem;
    if (XPR_IS_NULL(sRenItem))
        return;

    sRenItem->mDir    = aDir;
    sRenItem->mOld    = aOld;
    sRenItem->mNew    = aNew;
    sRenItem->mResult = ResultNone;

    mRenDeque.push_back(sRenItem);
}

void MultiRename::addPath(const xpr::tstring &aDir, const xpr::tstring &aOld, const xpr::tstring &aNew)
{
    addPath(aDir.c_str(), aOld.c_str(), aNew.c_str());
}

xpr_bool_t MultiRename::OnPreEntry(void)
{
    mInvalidItem = -1;
    mValidatedCount = 0;
    mRenamedCount = 0;
    mStatus = StatusPreparing;

    return XPR_TRUE;
}

unsigned MultiRename::OnEntryProc(void)
{
    xpr_bool_t sReadOnlyRename = isFlag(FlagReadOnlyRename);

    xpr::tstring sSrc;
    xpr::tstring sDst;
    xpr::tstring sTemp;
    xpr::tstring sTempFileName;
    DWORD sAttributes;

    xpr_sint_t sInvalidItem = -1;
    xpr_bool_t sInvalid = XPR_FALSE;
    Status sStatus = StatusNone;

    xpr_size_t sLen;
    RenItem *sRenItem;
    RenItem *sRenItem2;
    RenDeque::iterator sIterator;
    RenDeque::iterator sIterator2;

    typedef std::tr1::unordered_multimap<xpr::tstring, RenItem *> HashPathMap;
    typedef std::pair<HashPathMap::iterator, HashPathMap::iterator> HashPathPairIterator;
    HashPathMap sHashOldPathMap;
    HashPathMap sHashNewPathMap;
    HashPathMap::iterator sHashPathIterator;
    HashPathPairIterator sPairRangeIterator;

    sIterator = mRenDeque.begin();
    for (; sIterator != mRenDeque.end(); ++sIterator)
    {
        sRenItem = *sIterator;
        if (XPR_IS_NULL(sRenItem))
            continue;

        sSrc = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mOld;
        sDst = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mNew;

        sSrc.upper_case();
        sDst.upper_case();

        sHashOldPathMap.insert(HashPathMap::value_type(sSrc, sRenItem));
        sHashNewPathMap.insert(HashPathMap::value_type(sDst, sRenItem));

        {
            xpr::MutexGuard sLockGuard(mMutex);
            mPreparedCount++;
        }
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = StatusValidating;
    }

    sIterator = mRenDeque.begin();
    for (; sIterator != mRenDeque.end(); ++sIterator)
    {
        sRenItem = *sIterator;
        if (XPR_IS_NULL(sRenItem))
            continue;

        if (IsStop())
            break;

        sSrc = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mOld;
        sDst = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mNew;

        sLen = sDst.length();

        if (sLen == 0 || sLen >= XPR_MAX_PATH)
        {
            sRenItem->mResult = (sLen == 0) ? ResultEmptiedName : ResultExcessPathLength;
            sInvalidItem = (xpr_sint_t)std::distance(mRenDeque.begin(), sIterator);
            sInvalid = XPR_TRUE;
            break;
        }

        if (VerifyFileName(sRenItem->mNew) == XPR_FALSE)
        {
            sRenItem->mResult = ResultInvalidName;
            sInvalidItem = (xpr_sint_t)std::distance(mRenDeque.begin(), sIterator);
            sInvalid = XPR_TRUE;
            break;
        }

        sDst.upper_case();
        sPairRangeIterator = sHashNewPathMap.equal_range(sDst);

        sHashPathIterator = sPairRangeIterator.first;
        for (; sHashPathIterator != sPairRangeIterator.second; ++sHashPathIterator)
        {
            sRenItem2 = sHashPathIterator->second;
            if (XPR_IS_NULL(sRenItem2))
                continue;

            if (sRenItem != sRenItem2)
                break;
        }

        if (sHashPathIterator != sPairRangeIterator.second)
        {
            sRenItem->mResult = ResultEqualedName;
            sInvalidItem = (xpr_sint_t)std::distance(mRenDeque.begin(), sIterator);
            sInvalid = XPR_TRUE;
            break;
        }

        {
            xpr::MutexGuard sLockGuard(mMutex);
            mValidatedCount++;
        }
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = StatusRenaming;
    }

    if (IsStop() == XPR_FALSE && XPR_IS_FALSE(sInvalid))
    {
        sIterator = mRenDeque.begin();
        for (; sIterator != mRenDeque.end(); ++sIterator)
        {
            sRenItem = *sIterator;
            if (XPR_IS_NULL(sRenItem))
                continue;

            if (IsStop())
                break;

            sRenItem->mResult = ResultNone;

            sSrc = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mOld;
            sDst = sRenItem->mDir + XPR_STRING_LITERAL('\\') + sRenItem->mNew;

            // notice : The file name must compare by case.
            if (_tcscmp(sSrc.c_str(), sDst.c_str()) != 0)
            {
                // "c:\\0.txt" -> "c:\\1.txt"
                if (::MoveFile(sSrc.c_str(), sDst.c_str()) == XPR_FALSE)
                {
                    // "c:\\1.txt"
                    if (IsExistFile(sDst) == XPR_FALSE)
                    {
                        sRenItem->mResult = ResultPathNotExist;
                    }
                    else
                    {
                        sAttributes = ::GetFileAttributes(sSrc.c_str());

                        if (XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_READONLY))
                        {
                            if (XPR_IS_FALSE(sReadOnlyRename))
                                sRenItem->mResult = ResultReadOnly;
                            else
                                ::SetFileAttributes(sSrc.c_str(), sAttributes & ~FILE_ATTRIBUTE_READONLY);
                        }

                        if (sRenItem->mResult == ResultNone)
                        {
                            // "c:\\1.txt" -> "c:\\[org] 1.txt"
                            // "c:\\[org] 1.txt" -> "c:\\[org] [org] 1.txt"
                            sTempFileName = sRenItem->mNew;

                            do
                            {
                                sTempFileName.insert(0, mBackup);

                                sTemp  = sRenItem->mDir;
                                sTemp += XPR_STRING_LITERAL('\\');
                                sTemp += sTempFileName;
                            }
                            while (::MoveFile(sDst.c_str(), sTemp.c_str()) == XPR_FALSE);

                            // Old Name : "1.txt" -> "[org] 1.txt"
                            sTemp = sDst;
                            sTemp.upper_case();
                            sPairRangeIterator = sHashOldPathMap.equal_range(sTemp);

                            sHashPathIterator = sPairRangeIterator.first;
                            for (; sHashPathIterator != sPairRangeIterator.second; ++sHashPathIterator)
                            {
                                sRenItem2 = sHashPathIterator->second;
                                if (XPR_IS_NULL(sRenItem2))
                                    continue;

                                if (sRenItem != sRenItem2)
                                {
                                    sRenItem2->mOld = sTempFileName;
                                }
                            }

                            if (::MoveFile(sSrc.c_str(), sDst.c_str()) == XPR_TRUE)
                            {
                                if (XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_READONLY))
                                    ::SetFileAttributes(sDst.c_str(), sAttributes);
                            }
                        }
                    }
                }
            }

            if (sRenItem->mResult == ResultNone)
                sRenItem->mResult = ResultSucceeded;

            {
                xpr::MutexGuard sLockGuard(mMutex);
                mRenamedCount++;
            }
        }
    }

    sHashOldPathMap.clear();
    sHashNewPathMap.clear();

    if (IsStop() == XPR_TRUE)
    {
        sStatus = StatusStopped;
    }
    else
    {
        if (XPR_IS_TRUE(sInvalid))
            sStatus = StatusInvalid;
        else
            sStatus = StatusRenameCompleted;
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mInvalidItem = sInvalidItem;
        mStatus = sStatus;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

MultiRename::Status MultiRename::getStatus(xpr_size_t *aPreparedCount, xpr_size_t *aValidatedCount, xpr_size_t *aRenamedCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (aPreparedCount)  *aPreparedCount  = mPreparedCount;
    if (aValidatedCount) *aValidatedCount = mValidatedCount;
    if (aRenamedCount)   *aRenamedCount   = mRenamedCount;

    return mStatus;
}

MultiRename::Result MultiRename::getItemResult(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mRenDeque))
        return ResultNone;

    return mRenDeque[aIndex]->mResult;
}

xpr_sint_t MultiRename::getInvalidItem(void)
{
    xpr::MutexGuard sLockGuard(mMutex);

    return mInvalidItem;
}
} // namespace cmd
} // namespace fxfile
