//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_create.h"
#include "format.h"
#include "format_short_parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
void getLocalTime(SYSTEMTIME &aTimeNow)
{
    xpr::TimeExpr sNowTimeExpr;
    xpr::getLocalTimeNow(sNowTimeExpr);

    memset(&aTimeNow, 0, sizeof(aTimeNow));
    aTimeNow.wYear         = sNowTimeExpr.mYear;
    aTimeNow.wMonth        = sNowTimeExpr.mMonth;
    aTimeNow.wDay          = sNowTimeExpr.mDay;
    aTimeNow.wHour         = sNowTimeExpr.mHour;
    aTimeNow.wMinute       = sNowTimeExpr.mMinute;
    aTimeNow.wSecond       = sNowTimeExpr.mSecond;
    aTimeNow.wMilliseconds = sNowTimeExpr.getMillisecond();
}
} // namespace anonymous

BatchCreate::BatchCreate(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mCreateType(CreateTypeNone)
    , mStatus(StatusNone)
    , mPreparedCount(0), mValidatedCount(0), mCreatedCount(0)
    , mInvalidItem(-1)
{
}

BatchCreate::~BatchCreate(void)
{
    Stop();

    clear();
}

void BatchCreate::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void BatchCreate::setCreateType(CreateType aCreateType)
{
    mCreateType = aCreateType;
}

BatchCreate::CreateType BatchCreate::getCreateType(void) const
{
    return mCreateType;
}

BatchCreate::Result BatchCreate::addPath(const xpr::string &aPath)
{
    if (aPath.length() > XPR_MAX_PATH)
        return ResultExcessPathLength;

    Item *sItem    = new Item;
    sItem->mPath   = aPath;
    sItem->mResult = ResultNone;

    mNewDeque.push_back(sItem);

    return ResultSucceeded;
}

BatchCreate::Result BatchCreate::addFormat(const xpr_tchar_t *aDir,
                                           const xpr_tchar_t *aFormat,
                                           xpr_size_t         aCount)
{
    XPR_ASSERT(aDir != XPR_NULL);
    XPR_ASSERT(aFormat != XPR_NULL);
    XPR_ASSERT(aCount > 0);

    xpr::string sDir = aDir;
    xpr::string sFormat = aFormat;

    if (sDir[sDir.length()-1] != XPR_STRING_LITERAL('\\'))
    {
        sDir += XPR_STRING_LITERAL('\\');
    }

    if (mCreateType == CreateTypeTextFile)
    {
        sFormat += XPR_STRING_LITERAL(".txt");
    }

    Result sResult = ResultSucceeded;

    xpr_size_t  i = 0;
    xpr_size_t  sMaxNewLen;
    xpr_bool_t  sFolder = (mCreateType == CreateTypeFolder) ? XPR_TRUE : XPR_FALSE;
    xpr_bool_t  sAtLeastOneError = XPR_FALSE;
    xpr::string sOrgFilePath;
    xpr::string sOrgFileName;
    xpr::string sOldFileName;
    xpr::string sPath;
    SYSTEMTIME  sNowTime = {0,};

    FormatSequence *sFormatSequence;
    FileNameFormat  sFileNameFormat;
    ShortFormatParser &sShortFormatParser = SingletonManager::get<ShortFormatParser>();

    sFormatSequence = new FormatSequence;

    if (sShortFormatParser.parse(sFormat, *sFormatSequence) == XPR_FALSE)
    {
        XPR_SAFE_DELETE(sFormatSequence);

        return ResultWrongFormat;
    }

    sFileNameFormat.add(sFormatSequence);

    getLocalTime(sNowTime);

    for (i = 0; i < aCount; ++i)
    {
        sMaxNewLen = XPR_MAX_PATH - sDir.length() - 1;

        RenameContext sContext(sFolder,
                               sOrgFilePath,
                               sOrgFileName,
                               sOldFileName,
                               (xpr_sint_t)i,
                               sMaxNewLen,
                               sNowTime);

        if (XPR_IS_FALSE(sFileNameFormat.rename(sContext)))
        {
            sAtLeastOneError = XPR_TRUE;
        }

        sPath = sDir + sContext.mNewFileName;

        sResult = addPath(sPath);
        if (sResult == ResultExcessPathLength)
        {
            break;
        }
    }

    return sResult;
}

xpr_size_t BatchCreate::getCount(void) const
{
    return mNewDeque.size();
}

void BatchCreate::clear(void)
{
    NewDeque::iterator sIterator;
    Item *sItem;

    sIterator = mNewDeque.begin();
    for (; sIterator != mNewDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        XPR_SAFE_DELETE(sItem);
    }

    mNewDeque.clear();
}

xpr_bool_t BatchCreate::OnPreEntry(void)
{
    mInvalidItem = -1;
    mValidatedCount = 0;
    mCreatedCount = 0;
    mStatus = StatusPreparing;

    return XPR_TRUE;
}

unsigned BatchCreate::OnEntryProc(void)
{
    CreateType sCreateType = getCreateType();

    xpr::string sPath;
    xpr::string sFileName;

    xpr_sint_t sInvalidItem = -1;
    xpr_bool_t sInvalid = XPR_FALSE;
    Status sStatus = StatusNone;

    xpr_size_t sFind;
    Item *sItem;
    Item *sItem2;
    NewDeque::iterator sIterator;

    typedef std::tr1::unordered_multimap<xpr::string, Item *> HashPathMap;
    typedef std::pair<HashPathMap::iterator, HashPathMap::iterator> HashPathPairIterator;
    HashPathMap sHashPathMap;
    HashPathMap::iterator sHashPathIterator;
    HashPathPairIterator sPairRangeIterator;

    sIterator = mNewDeque.begin();
    for (; sIterator != mNewDeque.end(); ++sIterator)
    {
        sItem = *sIterator;
        if (XPR_IS_NULL(sItem))
            continue;

        sPath = sItem->mPath;

        sPath.upper_case();

        sHashPathMap.insert(HashPathMap::value_type(sPath, sItem));

        {
            xpr::MutexGuard sLockGuard(mMutex);
            mPreparedCount++;
        }
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = StatusVerifying;
    }

    sIterator = mNewDeque.begin();
    for (; sIterator != mNewDeque.end(); ++sIterator)
    {
        sItem = *sIterator;

        XPR_ASSERT(sItem != XPR_NULL);

        if (IsStop() == XPR_TRUE)
            break;

        sPath = sItem->mPath;
        if (sPath.empty() == XPR_TRUE)
            continue;

        sFileName.clear();

        sFind = sPath.rfind(XPR_STRING_LITERAL('\\'));
        if (sFind != xpr::string::npos)
            sFileName = sPath.substr(sFind+1);

        if (VerifyFileName(sFileName) == XPR_FALSE)
        {
            sItem->mResult = ResultInvalidName;
            sInvalidItem = (xpr_sint_t)std::distance(mNewDeque.begin(), sIterator);
            sInvalid = XPR_TRUE;
            break;
        }

        sPath.upper_case();
        sPairRangeIterator = sHashPathMap.equal_range(sPath);

        sHashPathIterator = sPairRangeIterator.first;
        for (; sHashPathIterator != sPairRangeIterator.second; ++sHashPathIterator)
        {
            sItem2 = sHashPathIterator->second;

            XPR_ASSERT(sItem2 != XPR_NULL);

            if (sItem != sItem2)
                break;
        }

        if (sHashPathIterator != sPairRangeIterator.second)
        {
            sItem->mResult = ResultEqualedName;
            sInvalidItem = (xpr_sint_t)std::distance(mNewDeque.begin(), sIterator);
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
        mStatus = StatusCreating;
    }

    if (IsStop() == XPR_FALSE && XPR_IS_FALSE(sInvalid))
    {
        xpr_rcode_t sRcode;
        xpr_sint_t sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        xpr::FileIo sFileIo;

        sIterator = mNewDeque.begin();
        for (; sIterator != mNewDeque.end(); ++sIterator)
        {
            sItem = *sIterator;

            XPR_ASSERT(sItem != XPR_NULL);

            if (IsStop() == XPR_TRUE)
                break;

            sItem->mResult = ResultNone;

            if (IsExistFile(sItem->mPath) == XPR_FALSE)
            {
                switch (sCreateType)
                {
                case CreateTypeFolder:
                    {
                        ::CreateDirectory(sItem->mPath.c_str(), XPR_NULL);
                        break;
                    }

                case CreateTypeFile:
                case CreateTypeTextFile:
                    {
                        sRcode = sFileIo.open(sItem->mPath, sOpenMode);
                        if (XPR_RCODE_IS_SUCCESS(sRcode))
                            sFileIo.close();
                        break;
                    }
                }
            }

            if (sItem->mResult == ResultNone)
                sItem->mResult = ResultSucceeded;

            {
                xpr::MutexGuard sLockGuard(mMutex);
                mCreatedCount++;
            }
        }
    }

    sHashPathMap.clear();

    if (IsStop() == XPR_TRUE)
    {
        sStatus = StatusStopped;
    }
    else
    {
        if (XPR_IS_TRUE(sInvalid))
            sStatus = StatusInvalid;
        else
            sStatus = StatusCreateCompleted;
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mInvalidItem = sInvalidItem;
        mStatus = sStatus;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

BatchCreate::Status BatchCreate::getStatus(xpr_size_t *aPreparedCount, xpr_size_t *aValidatedCount, xpr_size_t *aCreatedCount) const
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aPreparedCount))  *aPreparedCount  = mPreparedCount;
    if (XPR_IS_NOT_NULL(aValidatedCount)) *aValidatedCount = mValidatedCount;
    if (XPR_IS_NOT_NULL(aCreatedCount))   *aCreatedCount   = mCreatedCount;

    return mStatus;
}

BatchCreate::Result BatchCreate::getItemResult(xpr_size_t aIndex) const
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mNewDeque))
        return ResultNone;

    return mNewDeque[aIndex]->mResult;
}

xpr_sint_t BatchCreate::getInvalidItem(void) const
{
    xpr::MutexGuard sLockGuard(mMutex);

    return mInvalidItem;
}
} // namespace cmd
} // namespace fxfile
