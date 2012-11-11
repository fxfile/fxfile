//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_sync_dirs.h"

#include "fxb_crc_checksum.h"
#include "fxb_fnmatch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
SyncItem::SyncItem(void)
{
    clear();
}

SyncItem::~SyncItem(void)
{
    clear();
}

void SyncItem::clear(void)
{
    mSubLevel = 0;
    mSubPath.clear();

    memset(mFileSize,           0, sizeof(mFileSize));
    memset(mCreatedFileTime,    0, sizeof(mCreatedFileTime));
    memset(mLastAccessFileTime, 0, sizeof(mLastAccessFileTime));
    memset(mModifiedFileTime,   0, sizeof(mModifiedFileTime));
    memset(mFileAttributes,     0, sizeof(mFileAttributes));
    memset(mHash,               0, sizeof(mHash));

    if (XPR_IS_NOT_NULL(mHash))
    {
        XPR_SAFE_DELETE_ARRAY(mHash[0]);
        XPR_SAFE_DELETE_ARRAY(mHash[1]);
    }

    mExist   = CompareExistNone;
    mDiff    = CompareDiffNone;
    mSync    = SyncNone;
    mOrgSync = SyncNone;
}

const xpr_tchar_t *SyncItem::getSubPath(void)
{
    return mSubPath.c_str();
}

void SyncItem::getSubPath(std::tstring &aSubPath)
{
    aSubPath = mSubPath;
}

xpr_bool_t SyncItem::getPath(const std::tstring &aDir, std::tstring &aPath)
{
    if (aDir.empty() == true)
        return XPR_FALSE;

    aPath = aDir;
    if (aPath[aPath.length()-1] != XPR_STRING_LITERAL('\\'))
        aPath += XPR_STRING_LITERAL('\\');
    aPath += mSubPath;

    return XPR_TRUE;
}

xpr_bool_t SyncItem::getPath(const xpr_tchar_t *aDir, std::tstring &aPath)
{
    if (XPR_IS_NULL(aDir))
        return XPR_FALSE;

    std::tstring sDir = aDir;
    return getPath(sDir, aPath);
}

xpr_bool_t SyncItem::getPath(const xpr_tchar_t *aDir, xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aDir) || XPR_IS_NULL(aPath))
        return XPR_FALSE;

    std::tstring sDir = aDir;
    std::tstring sPath;

    if (getPath(sDir, sPath) == XPR_FALSE)
        return XPR_FALSE;

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

LPFILETIME SyncItem::getTime(xpr_sint_t aIndex)
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return XPR_NULL;

    if (isDirectory(aIndex) == XPR_TRUE)
        return &mCreatedFileTime[aIndex];

    return &mModifiedFileTime[aIndex];
}

xpr_uint_t SyncItem::getDiff(void) { return mDiff; }
xpr_uint_t SyncItem::getSync(void) { return mSync; }

void SyncItem::setDiff(xpr_uint_t aDiff)
{
    mDiff = aDiff;
    mSync = SyncNone;

    if (mSync == SyncNone && XPR_TEST_BITS(mDiff, CompareDiffFailed))     mSync = SyncFailed;
    if (mSync == SyncNone && XPR_TEST_BITS(mDiff, CompareDiffEqualed))    mSync = SyncEqualed;
    if (mSync == SyncNone && XPR_TEST_BITS(mDiff, CompareDiffToLeft))     mSync = SyncToLeft;
    if (mSync == SyncNone && XPR_TEST_BITS(mDiff, CompareDiffToRight))    mSync = SyncToRight;
    if (mSync == SyncNone && XPR_TEST_BITS(mDiff, CompareDiffNotEqualed)) mSync = SyncNotEqualed;

    mOrgSync = mSync;
}

void SyncItem::setOrgSync(void)
{
    mSync = mOrgSync;
}

void SyncItem::setSync(xpr_uint_t aSync)
{
    if (mSync == SyncEqualed)
        return;

    if (!XPR_TEST_BITS(mExist, CompareExistEqual))
    {
        if (XPR_TEST_BITS(mExist, CompareExistLeft) && aSync == SyncToLeft)
            return;

        if (XPR_TEST_BITS(mExist, CompareExistRight) && aSync == SyncToRight)
            return;
    }

    mSync = aSync;
}

xpr_bool_t SyncItem::isDirectory(xpr_sint_t aIndex)
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return XPR_FALSE;

    return XPR_TEST_BITS(mFileAttributes[aIndex], FILE_ATTRIBUTE_DIRECTORY);
}

xpr_bool_t SyncItem::isExistLeft (void) { return XPR_TEST_BITS(mExist, CompareExistLeft);  }
xpr_bool_t SyncItem::isExistRight(void) { return XPR_TEST_BITS(mExist, CompareExistRight); }
xpr_bool_t SyncItem::isExistBoth (void) { return XPR_TEST_BITS(mExist, CompareExistEqual);  }

SyncItem::Result SyncItem::compare(CompareFlags &aCompareFlags)
{
    register xpr_sint_t i;
    xpr_uint_t sDiff;

    Result sResult = ResultCompared;

    if (XPR_IS_TRUE(aCompareFlags.mEqualDir))
    {
        setDiff(CompareDiffEqualed);
        return sResult;
    }

    setDiff(CompareDiffNone);

    // file existense
    if (!XPR_TEST_BITS(mExist, CompareExistEqual))
    {
        xpr_bool_t sExistLeft = XPR_TEST_BITS(mExist, CompareExistLeft);
        setDiff(XPR_IS_TRUE(sExistLeft) ? CompareDiffToRight : CompareDiffToLeft);
        return sResult;
    }

    xpr_bool_t sIsDir = XPR_TEST_BITS(mFileAttributes[0], FILE_ATTRIBUTE_DIRECTORY);

    // file time
    if (XPR_IS_TRUE(aCompareFlags.mDateTime))
    {
        xpr_sint_t sCompare = ::CompareFileTime(getTime(0), getTime(1));
        if (sCompare != 0)
        {
            sDiff  = CompareDiffNotEqualed;
            sDiff |= CompareDiffTime;
            sDiff |= (sCompare > 0) ? CompareDiffToRight : CompareDiffToLeft;
            setDiff(sDiff);
            return sResult;
        }
    }

    // file attributes
    if (XPR_IS_TRUE(aCompareFlags.mAttributes))
    {
        if (mFileAttributes[0] != mFileAttributes[1])
        {
            sDiff  = CompareDiffNotEqualed;
            sDiff |= CompareDiffAttributes;
            setDiff(sDiff);
            return sResult;
        }
    }

    // file size, only files
    if (XPR_IS_TRUE(aCompareFlags.mSize) && XPR_IS_FALSE(sIsDir))
    {
        if (mFileSize[0] != mFileSize[1])
        {
            sDiff  = CompareDiffNotEqualed;
            sDiff |= CompareDiffSize;
            setDiff(sDiff);
            return sResult;
        }
    }

    // file contents, only files
    if (XPR_IS_TRUE(aCompareFlags.mContents) != CompareContentsNone && XPR_IS_FALSE(sIsDir))
    {
        aCompareFlags.mPath[0] = aCompareFlags.mDir[0] + XPR_STRING_LITERAL('\\') + mSubPath;
        aCompareFlags.mPath[1] = aCompareFlags.mDir[1] + XPR_STRING_LITERAL('\\') + mSubPath;

        // file hash
        if (aCompareFlags.mContents == CompareContentsHashSFV ||
            aCompareFlags.mContents == CompareContentsHashMD5)
        {
            aCompareFlags.mCrc[0][0] = '\0';
            aCompareFlags.mCrc[1][0] = '\0';

            if (aCompareFlags.mContents == CompareContentsHashSFV)
            {
                getFileCrcSfv(aCompareFlags.mPath[0].c_str(), aCompareFlags.mCrc[0]);
                getFileCrcSfv(aCompareFlags.mPath[1].c_str(), aCompareFlags.mCrc[1]);

                if (strlen(aCompareFlags.mCrc[0]) <= 0 || strlen(aCompareFlags.mCrc[1]) <= 0)
                {
                    setDiff(CompareDiffFailed);
                }
                else
                {
                    if (strcmp(aCompareFlags.mCrc[0], aCompareFlags.mCrc[1]))
                    {
                        sDiff  = CompareDiffNotEqualed;
                        sDiff |= CompareDiffContentsHashSFV;
                        setDiff(sDiff);
                    }
                }
            }

            if (aCompareFlags.mContents == CompareContentsHashMD5)
            {
                getFileCrcMd5(aCompareFlags.mPath[0].c_str(), aCompareFlags.mCrc[0]);
                getFileCrcMd5(aCompareFlags.mPath[1].c_str(), aCompareFlags.mCrc[1]);

                if (strlen(aCompareFlags.mCrc[0]) <= 0 || strlen(aCompareFlags.mCrc[1]) <= 0)
                {
                    setDiff(CompareDiffFailed);
                }
                else
                {
                    if (strcmp(aCompareFlags.mCrc[0], aCompareFlags.mCrc[1]))
                    {
                        sDiff  = CompareDiffNotEqualed;
                        sDiff |= CompareDiffContentsHashMD5;
                        setDiff(sDiff);
                    }
                }
            }

            for (i = 0; i < 2; ++i)
            {
                if (strlen(aCompareFlags.mCrc[i]) > 0)
                {
                    mHash[i] = new xpr_char_t[strlen(aCompareFlags.mCrc[i])+1];
                    strcpy(mHash[i], aCompareFlags.mCrc[i]);
                }
            }

            if (getDiff() != CompareDiffNone)
                return sResult;
        }
        else
        {
            xpr_rcode_t sRcode1, sRcode2;
            xpr::FileIo sFileIo1;
            xpr::FileIo sFileIo2;

            sRcode1 = sFileIo1.open(aCompareFlags.mPath[0].c_str(), xpr::FileIo::OpenModeReadOnly);
            sRcode2 = sFileIo2.open(aCompareFlags.mPath[1].c_str(), xpr::FileIo::OpenModeReadOnly);

            if (XPR_RCODE_IS_SUCCESS(sRcode1) && XPR_RCODE_IS_SUCCESS(sRcode2))
            {
                xpr_ssize_t sRead1, sRead2;
                xpr_size_t sLen;

                do
                {
                    if (aCompareFlags.mStopEvent)
                    {
                        if (::WaitForSingleObject(aCompareFlags.mStopEvent, 0) == WAIT_OBJECT_0)
                        {
                            sResult = ResultStopEvent;
                            break;
                        }
                    }

                    sRcode1 = sFileIo1.read(aCompareFlags.mBuffer[0], aCompareFlags.mBufferSize, &sRead1);
                    sRcode2 = sFileIo2.read(aCompareFlags.mBuffer[1], aCompareFlags.mBufferSize, &sRead2);

                    if (XPR_RCODE_IS_SUCCESS(sRcode1) && XPR_RCODE_IS_SUCCESS(sRcode2) && sRead1 > 0 && sRead2 > 0)
                    {
                        sLen = min(sRead1, sRead2);
                        if (memcmp(aCompareFlags.mBuffer[0], aCompareFlags.mBuffer[1], sLen))
                        {
                            sDiff  = CompareDiffNotEqualed;
                            sDiff |= CompareDiffContentsBytes;
                            setDiff(sDiff);
                            break;
                        }
                    }
                } while (XPR_RCODE_IS_SUCCESS(sRcode1) && XPR_RCODE_IS_SUCCESS(sRcode2) && sRead1 > 0 && sRead2 > 0);
            }
            else
            {
                setDiff(CompareDiffFailed);
            }

            sFileIo1.close();
            sFileIo2.close();

            if (getDiff() != CompareDiffNone)
                return sResult;
        }
    }

    setDiff(CompareDiffEqualed);

    return sResult;
}

SyncItem::Result SyncItem::synchronize(SyncFlags &aSyncFlags, CompareFlags &aCompareFlags)
{
    xpr_sint_t sIndex1 = -1;
    xpr_sint_t sIndex2 = -1;

    if (XPR_TEST_BITS(aSyncFlags.mDirection, SyncDirectionToLeft))
    {
        xpr_uint_t sSync = getSync();
        if (XPR_TEST_BITS(sSync, SyncToLeft))
        {
            sIndex1 = 1;
            sIndex2 = 0;
        }
    }

    if (XPR_TEST_BITS(aSyncFlags.mDirection, SyncDirectionToRight))
    {
        xpr_uint_t sSync = getSync();
        if (XPR_TEST_BITS(sSync, SyncToRight))
        {
            sIndex1 = 0;
            sIndex2 = 1;
        }
    }

    if (sIndex1 == -1 || sIndex2 == -1)
        return ResultSkiped;

    Result sResult = ResultSynchronized;

    aSyncFlags.mPath[0] = aSyncFlags.mDir[sIndex1] + XPR_STRING_LITERAL('\\') + mSubPath;
    aSyncFlags.mPath[1] = aSyncFlags.mDir[sIndex2] + XPR_STRING_LITERAL('\\') + mSubPath;

    if (XPR_TEST_BITS(mFileAttributes[sIndex1], FILE_ATTRIBUTE_DIRECTORY))
    {
        if (CreateDirectoryLevel(aSyncFlags.mPath[1].c_str(), aSyncFlags.mDir[sIndex2].length() + 1) == XPR_TRUE)
        {
            mFileSize[sIndex2] = mFileSize[sIndex1];

            if (::SetFileAttributes(aSyncFlags.mPath[1].c_str(), mFileAttributes[sIndex1]))
            {
                mFileAttributes[sIndex2] = mFileAttributes[sIndex1];
            }

            if (SetFileTime(aSyncFlags.mPath[1].c_str(), &mCreatedFileTime[sIndex1], XPR_NULL, XPR_NULL) == XPR_TRUE)
            {
                mCreatedFileTime[sIndex2]    = mCreatedFileTime[sIndex1];
                mLastAccessFileTime[sIndex2] = mLastAccessFileTime[sIndex1];
                mModifiedFileTime[sIndex2]   = mModifiedFileTime[sIndex1];
            }

            mExist |= (sIndex2 == 1) ? CompareExistRight : CompareExistLeft;

            compare(aCompareFlags);
        }
    }
    else
    {
        xpr_rcode_t sRcode1, sRcode2;
        xpr_sint_t sOpenMode;
        xpr::FileIo sFileIo1;
        xpr::FileIo sFileIo2;

        sRcode1 = sFileIo1.open(aSyncFlags.mPath[0].c_str(), xpr::FileIo::OpenModeReadOnly);

        sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
        sRcode2 = sFileIo2.open(aSyncFlags.mPath[1].c_str(), sOpenMode);

        if (XPR_RCODE_IS_SUCCESS(sRcode1) && XPR_RCODE_IS_SUCCESS(sRcode2))
        {
            xpr_sint64_t sTotalWritten = 0;
            xpr_ssize_t sWritten;
            xpr_ssize_t sRead;

            do
            {
                if (XPR_IS_NOT_NULL(aSyncFlags.mStopEvent))
                {
                    if (::WaitForSingleObject(aSyncFlags.mStopEvent, 0) == WAIT_OBJECT_0)
                    {
                        sResult = ResultStopEvent;
                        break;
                    }
                }

                sRcode1 = sFileIo1.read(aSyncFlags.mBuffer, aSyncFlags.mBufferSize, &sRead);
                if (XPR_RCODE_IS_SUCCESS(sRcode1) && sRead > 0)
                {
                    sRcode2 = sFileIo2.write(aSyncFlags.mBuffer, sRead, &sWritten);
                    if (XPR_RCODE_IS_SUCCESS(sRcode2))
                        sTotalWritten += sWritten;
                }
            } while (XPR_RCODE_IS_SUCCESS(sRcode1) && sRead > 0);

            if (sTotalWritten == mFileSize[sIndex1])
            {
                mFileSize[sIndex2] = mFileSize[sIndex1];
                mExist |= (sIndex2 == 1) ? CompareExistRight : CompareExistLeft;
            }
        }

        sFileIo1.close();
        sFileIo2.close();

        if (SetFileTime(aSyncFlags.mPath[1].c_str(), &mCreatedFileTime[sIndex1], &mLastAccessFileTime[sIndex1], &mModifiedFileTime[sIndex1]) == XPR_TRUE)
        {
            mCreatedFileTime[sIndex2]    = mCreatedFileTime[sIndex1];
            mLastAccessFileTime[sIndex2] = mLastAccessFileTime[sIndex1];
            mModifiedFileTime[sIndex2]   = mModifiedFileTime[sIndex1];
        }

        if (::SetFileAttributes(aSyncFlags.mPath[1].c_str(), mFileAttributes[sIndex1]) == XPR_TRUE)
        {
            mFileAttributes[sIndex2] = mFileAttributes[sIndex1];
        }

        compare(aCompareFlags);
    }

    return sResult;
}

static const xpr_size_t kDefaultBufferSize = 16 * 1024; // 16KB

SyncDirs::SyncDirs(const xpr_tchar_t *aDir1, const xpr_tchar_t *aDir2)
    : mThread(XPR_NULL), mThreadId(0)
    , mStopEvent(XPR_NULL), mStoppedEvent(XPR_NULL)
    , mHwnd(XPR_NULL), mMsg(0)
{
    clear();
    setDir(aDir1, aDir2);
}

SyncDirs::~SyncDirs(void)
{
    clear();
}

void SyncDirs::clear(void)
{
    stop();

    mDir[0].clear();
    mDir[1].clear();

    mDirection      = SyncDirectionNone;
    mSubLevel       = 0;

    mExcludeExist   = CompareExistNone;
    mIncludeFilterDeque.clear();
    mExcludeFilterDeque.clear();
    mExcludeAttributes  = 0;

    mDateTime       = XPR_FALSE;
    mSize           = XPR_FALSE;
    mAttributes     = XPR_FALSE;
    mContents       = CompareContentsNone;
    mBufferSize     = kDefaultBufferSize;

    SyncItem *sSyncItem;
    SyncDeque::iterator sIterator;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        sSyncItem = *sIterator;
        XPR_SAFE_DELETE(sSyncItem);
    }

    mSyncMap.clear();
    mSyncDeque.clear();

    mStatus = StatusNone;
    mScanDirCount = 0;
    mCompareFileCount = 0;
}

void SyncDirs::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void SyncDirs::scanRecursiveDir(SyncMap            &aSyncMap,
                                const xpr_sint_t    aIndex,
                                xpr_uint_t          aLevel,
                                const std::tstring &aDir,
                                const std::tstring &aBaseDir)
{
    if (isStopThread() == XPR_TRUE)
        return;

    if (aLevel > mSubLevel)
        return;

    {
        CsLocker sLocker(mCs);
        mScanDirCount++;
    }

    std::tstring sPath = aDir + XPR_STRING_LITERAL("\\*.*");

    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};
    std::tstring sSubPath;

    SyncItem *sSyncItem;
    SyncMapPairIterator sPairRangeIterator;
    SyncMap::iterator sIterator;

    xpr_sint_t sMatch;
    FilterDeque::iterator sFilterIterator;

    sFindFile = ::FindFirstFile(sPath.c_str(), &sWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (isStopThread() == XPR_TRUE)
                break;

            if (_tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
                continue;

            if (aLevel >= mSubLevel && XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                continue;

            if (XPR_ANY_BITS(mExcludeAttributes, sWin32FindData.dwFileAttributes))
                continue;

            if (mIncludeFilterDeque.empty() == false)
            {
                sMatch = FNM_NOMATCH;

                sFilterIterator = mIncludeFilterDeque.begin();
                for (; sFilterIterator != mIncludeFilterDeque.end(); ++sFilterIterator)
                {
                    sMatch = fnmatch(sFilterIterator->c_str(), sWin32FindData.cFileName, FNM_CASEFOLD);
                    if (sMatch == FNM_MATCH)
                        break;
                }

                if (sMatch != FNM_MATCH)
                    continue;
            }

            if (mExcludeFilterDeque.empty() == false)
            {
                sMatch = FNM_NOMATCH;

                sFilterIterator = mExcludeFilterDeque.begin();
                for (; sFilterIterator != mExcludeFilterDeque.end(); ++sFilterIterator)
                {
                    sMatch = fnmatch(sFilterIterator->c_str(), sWin32FindData.cFileName, FNM_CASEFOLD);
                    if (sMatch == FNM_MATCH)
                        break;
                }

                if (sMatch == FNM_MATCH)
                    continue;
            }

            sPath = aDir + XPR_STRING_LITERAL('\\') + sWin32FindData.cFileName;
            sSubPath = sPath.substr(aBaseDir.length()+1);

            sPairRangeIterator = aSyncMap.equal_range(sSubPath);

            sIterator = sPairRangeIterator.first;
            for (; sIterator != sPairRangeIterator.second; ++sIterator)
            {
                sSyncItem = sIterator->second;
                if (XPR_IS_NULL(sSyncItem))
                    continue;

                if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY) == 
                    XPR_TEST_BITS(sSyncItem->mFileAttributes[(aIndex == 0) ? 1 : 0], FILE_ATTRIBUTE_DIRECTORY))
                {
                    break;
                }
            }

            if (sIterator == sPairRangeIterator.second)
            {
                sSyncItem = new SyncItem;
                aSyncMap.insert(SyncMap::value_type(sSubPath, sSyncItem));
            }
            else
            {
                sSyncItem = sIterator->second;
            }

            sSyncItem->mSubLevel = aLevel;
            sSyncItem->mSubPath  = sSubPath;

            sSyncItem->mFileSize[aIndex]           = ((xpr_sint64_t)sWin32FindData.nFileSizeHigh * (xpr_sint64_t)kuint32max) + sWin32FindData.nFileSizeLow;
            sSyncItem->mCreatedFileTime[aIndex]    = sWin32FindData.ftCreationTime;
            sSyncItem->mLastAccessFileTime[aIndex] = sWin32FindData.ftLastAccessTime;
            sSyncItem->mModifiedFileTime[aIndex]   = sWin32FindData.ftLastWriteTime;
            sSyncItem->mFileAttributes[aIndex]     = sWin32FindData.dwFileAttributes;

            sSyncItem->mExist |= (aIndex == 0) ? CompareExistLeft : CompareExistRight;

            if (aLevel < mSubLevel && XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                sPath = aDir + XPR_STRING_LITERAL('\\') + sWin32FindData.cFileName;

                aLevel++;
                scanRecursiveDir(aSyncMap, aIndex, aLevel, sPath, aBaseDir);
                aLevel--;

                if (isStopThread() == XPR_TRUE)
                    break;
            }
        }
        while (::FindNextFile(sFindFile, &sWin32FindData) == XPR_TRUE);

        ::FindClose(sFindFile);
    }
}

bool SyncDirs::sortDir(SyncItem *&aSyncItem1, SyncItem *&aSyncItem2)
{
    xpr_size_t sOffset = aSyncItem1->mSubLevel - aSyncItem2->mSubLevel;

    static std::tstring sDir1;
    static std::tstring sDir2;

    sDir1.clear();
    sDir2.clear();

    DWORD sFileAttributes1 = aSyncItem1->mFileAttributes[0];
    DWORD sFileAttributes2 = aSyncItem2->mFileAttributes[0];

    if (!XPR_TEST_BITS(aSyncItem1->mExist, CompareExistLeft)) sFileAttributes1 = aSyncItem1->mFileAttributes[1];
    if (!XPR_TEST_BITS(aSyncItem2->mExist, CompareExistLeft)) sFileAttributes2 = aSyncItem2->mFileAttributes[1];

    bool sIsDir1 = XPR_TEST_BITS(sFileAttributes1, FILE_ATTRIBUTE_DIRECTORY);
    bool sIsDir2 = XPR_TEST_BITS(sFileAttributes2, FILE_ATTRIBUTE_DIRECTORY);

    if (XPR_IS_TRUE(sIsDir1))
        sDir1 = aSyncItem1->mSubPath;
    else
    {
        if (aSyncItem1->mSubLevel > 0)
        {
            xpr_size_t sFind = aSyncItem1->mSubPath.rfind(XPR_STRING_LITERAL('\\'));
            if (sFind != std::tstring::npos)
                sDir1 = aSyncItem1->mSubPath.substr(0, sFind);
        }
    }

    if (sIsDir2)
        sDir2 = aSyncItem2->mSubPath;
    else
    {
        if (aSyncItem2->mSubLevel > 0)
        {
            xpr_size_t sFind = aSyncItem2->mSubPath.rfind(XPR_STRING_LITERAL('\\'));
            if (sFind != std::tstring::npos)
                sDir2 = aSyncItem2->mSubPath.substr(0, sFind);
        }
    }

    if (sDir1.empty() == false && sDir2.empty() == false)
    {
        xpr_sint_t sCompare = (lstrcmpi(sDir1.c_str(), sDir2.c_str()) != 0) ? XPR_TRUE : XPR_FALSE;
        if (sCompare != 0)
            return (bool)(sCompare < 0);

        return sIsDir1;
    }

    if (aSyncItem1->mSubLevel != aSyncItem2->mSubLevel)
        return (bool)(aSyncItem1->mSubLevel < aSyncItem2->mSubLevel);

    if (sIsDir1 ^ sIsDir2)
        return (bool)(sIsDir2);

    return (bool)(lstrcmpi(aSyncItem1->mSubPath.c_str(), aSyncItem2->mSubPath.c_str()) < 0);
}

xpr_bool_t SyncDirs::scan(void)
{
    if (mDir[0].empty() == true || mDir[1].empty() == true)
        return XPR_FALSE;

    if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
        return XPR_FALSE;

    stop();

    SyncItem *sSyncItem;
    SyncDeque::iterator sIterator;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        sSyncItem = *sIterator;
        XPR_SAFE_DELETE(sSyncItem);
    }

    mSyncMap.clear();
    mSyncDeque.clear();

    mStatus = StatusScaning;
    mScanDirCount = 0;

    mStopEvent    = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mStoppedEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread       = (HANDLE)::_beginthreadex(XPR_NULL, 0, ScanProc, this, 0, &mThreadId);

    return XPR_TRUE;
}

xpr_bool_t SyncDirs::compare(void)
{
    if (mDir[0].empty() == true || mDir[1].empty() == true)
        return XPR_FALSE;

    if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
        return XPR_FALSE;

    stop();

    mStatus = StatusComparing;
    mCompareFileCount = 0;

    mStopEvent    = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mStoppedEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread       = (HANDLE)::_beginthreadex(XPR_NULL, 0, CompareProc, this, 0, &mThreadId);

    return XPR_TRUE;
}

xpr_bool_t SyncDirs::scanCompare(void)
{
    if (mDir[0].empty() == true || mDir[1].empty() == true)
        return XPR_FALSE;

    if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
        return XPR_FALSE;

    stop();

    mStatus = StatusComparing;
    mCompareFileCount = 0;

    mStopEvent    = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mStoppedEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread       = (HANDLE)::_beginthreadex(XPR_NULL, 0, ScanCompareProc, this, 0, &mThreadId);

    return XPR_TRUE;
}

xpr_bool_t SyncDirs::synchronize(void)
{
    if (mDir[0].empty() == true || mDir[1].empty() == true)
        return XPR_FALSE;

    if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
        return XPR_FALSE;

    stop();

    mStatus = StatusSynchronizing;
    mSyncFileCount = 0;

    mStopEvent    = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mStoppedEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread       = (HANDLE)::_beginthreadex(XPR_NULL, 0, SynchronizeProc, this, 0, &mThreadId);

    return XPR_TRUE;
}

void SyncDirs::stop(DWORD aStopMillseconds)
{
    if (XPR_IS_NOT_NULL(mThread))
    {
        ::SetEvent(mStopEvent);
        if (::WaitForSingleObject(mThread, aStopMillseconds) == WAIT_TIMEOUT)
            ::TerminateThread(mThread, 0);

        CLOSE_HANDLE(mStopEvent);
        CLOSE_HANDLE(mStoppedEvent);
        CLOSE_HANDLE(mThread);
        mThreadId = 0;
    }
}

xpr_bool_t SyncDirs::isStopThread(void)
{
    if (XPR_IS_NULL(mStopEvent))
        return XPR_FALSE;

    return (::WaitForSingleObject(mStopEvent, 0) == WAIT_OBJECT_0) ? XPR_TRUE : XPR_FALSE;
}

unsigned __stdcall SyncDirs::ScanProc(void *aParam)
{
    DWORD sExitCode = 0;

    SyncDirs *sSyncDirs = (SyncDirs *)aParam;
    if (XPR_IS_NOT_NULL(sSyncDirs))
        sExitCode = sSyncDirs->OnScan();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD SyncDirs::OnScan(void)
{
    //if (mDir[0].empty() == true || mDir[1].empty() == true)
    //    return 0;

    //if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
    //    return 0;

    {
        SyncItem *sSyncItem;
        SyncMap::iterator sIterator;

        sIterator = mSyncMap.begin();
        for (; sIterator != mSyncMap.end(); ++sIterator)
        {
            sSyncItem = sIterator->second;
            XPR_SAFE_DELETE(sSyncItem);
        }

        mSyncMap.clear();
    }

    xpr_sint_t i;
    for (i = 0; i < 2; ++i)
    {
        scanRecursiveDir(mSyncMap, i, 0, mDir[i], mDir[i]);
    }

    {
        SyncItem *sSyncItem;
        SyncMap::iterator sIterator;

        sIterator = mSyncMap.begin();
        for (; sIterator != mSyncMap.end(); ++sIterator)
        {
            sSyncItem = sIterator->second;
            if (XPR_IS_NULL(sSyncItem))
                continue;

            switch (mExcludeExist)
            {
            case CompareExistOther:
                {
                    if (!XPR_TEST_BITS(sSyncItem->mExist, CompareExistEqual))
                    {
                        XPR_SAFE_DELETE(sSyncItem);
                    }
                    break;
                }

            case CompareExistEqual:
                {
                    if (XPR_TEST_BITS(sSyncItem->mExist, CompareExistEqual))
                    {
                        XPR_SAFE_DELETE(sSyncItem);
                    }
                    break;
                }
            }

            if (XPR_IS_NOT_NULL(sSyncItem))
                mSyncDeque.push_back(sSyncItem);
        }

        mSyncMap.clear();
    }

    sort(mSyncDeque.begin(), mSyncDeque.end(), sortDir);

    {
        CsLocker sLocker(mCs);
        mStatus = StatusScanCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)mStatus, (LPARAM)mSyncDeque.size());

    return 0;
}

unsigned __stdcall SyncDirs::CompareProc(void *aParam)
{
    DWORD sExitCode = 0;

    SyncDirs *sSyncDirs = (SyncDirs *)aParam;
    if (XPR_IS_NOT_NULL(sSyncDirs))
        sExitCode = sSyncDirs->OnCompare();

    ::_endthreadex(sExitCode);
    return 0;
}

void SyncDirs::getCompareFlags(SyncItem::CompareFlags &aCompareFlags)
{
    xpr_bool_t sEqualDir = (_tcsicmp(mDir[0].c_str(), mDir[1].c_str()) == 0) ? XPR_TRUE : XPR_FALSE;

    aCompareFlags.mEqualDir   = sEqualDir;
    aCompareFlags.mDir[0]     = mDir[0];
    aCompareFlags.mDir[1]     = mDir[1];
    aCompareFlags.mDateTime   = mDateTime;
    aCompareFlags.mAttributes = mAttributes;
    aCompareFlags.mSize       = mSize;
    aCompareFlags.mContents   = mContents;
    aCompareFlags.mBuffer[0]  = new xpr_char_t[mBufferSize];
    aCompareFlags.mBuffer[1]  = new xpr_char_t[mBufferSize];
    aCompareFlags.mBufferSize = mBufferSize;
    aCompareFlags.mStopEvent  = mStopEvent;
}

DWORD SyncDirs::OnCompare(void)
{
    //if (mDir[0].empty() == true || mDir[1].empty() == true)
    //    return 0;

    //if (IsExistFile(mDir[0]) == XPR_FALSE || IsExistFile(mDir[1]) == XPR_FALSE)
    //    return 0;

    SyncItem::CompareFlags sCompareFlags;
    getCompareFlags(sCompareFlags);

    SyncDeque::iterator sIterator;
    SyncItem *sSyncItem;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        {
            CsLocker sLocker(mCs);
            mCompareFileCount = (xpr_uint_t)std::distance(mSyncDeque.begin(), sIterator) + 1;
        }

        if (isStopThread() == XPR_TRUE)
            break;

        sSyncItem = *sIterator;
        if (XPR_IS_NULL(sSyncItem))
            continue;

        if (sSyncItem->compare(sCompareFlags) == SyncItem::ResultStopEvent)
            break;
    }

    {
        CsLocker sLocker(mCs);
        mStatus = StatusCompareCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)mStatus, (LPARAM)mSyncDeque.size());

    return 0;
}

unsigned __stdcall SyncDirs::ScanCompareProc(void *aParam)
{
    DWORD sExitCode = 0;

    SyncDirs *sSyncDirs = (SyncDirs *)aParam;
    if (XPR_IS_NOT_NULL(sSyncDirs))
        sExitCode = sSyncDirs->OnScanCompare();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD SyncDirs::OnScanCompare()
{
    OnScan();
    OnCompare();
    return 0;
}

unsigned __stdcall SyncDirs::SynchronizeProc(void *aParam)
{
    DWORD sExitCode = 0;

    SyncDirs *sSyncDirs = (SyncDirs *)aParam;
    if (XPR_IS_NOT_NULL(sSyncDirs))
        sExitCode = sSyncDirs->OnSynchronize();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD SyncDirs::OnSynchronize(void)
{
    SyncItem *sSyncItem;
    SyncDeque::iterator sIterator;

    SyncItem::SyncFlags sSyncFlags;
    sSyncFlags.mDirection  = mDirection;
    sSyncFlags.mDir[0]     = mDir[0];
    sSyncFlags.mDir[1]     = mDir[1];
    sSyncFlags.mStopEvent  = mStopEvent;
    sSyncFlags.mBuffer     = new xpr_char_t[mBufferSize];
    sSyncFlags.mBufferSize = mBufferSize;

    SyncItem::CompareFlags sCompareFlags;
    getCompareFlags(sCompareFlags);

    SyncItem::Result sResult;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        if (isStopThread() == XPR_TRUE)
            break;

        sSyncItem = *sIterator;
        if (XPR_IS_NULL(sSyncItem))
            continue;

        sResult = sSyncItem->synchronize(sSyncFlags, sCompareFlags);

        if (sResult == SyncItem::ResultSynchronized)
        {
            CsLocker sLocker(mCs);
            mSyncFileCount++;
        }

        if (sResult == SyncItem::ResultStopEvent)
            break;
    }

    {
        CsLocker sLocker(mCs);
        mStatus = StatusSynchronizeCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)mStatus, (LPARAM)mSyncDeque.size());

    return 0;
}

xpr_size_t SyncDirs::getCount(xpr_size_t *aDirCount, xpr_size_t *aFileCount)
{
    if (XPR_IS_NOT_NULL(aDirCount) || XPR_IS_NOT_NULL(aFileCount))
    {
        xpr_size_t sDirCount  = 0;
        xpr_size_t sFileCount = 0;

        SyncItem *sSyncItem;
        SyncDeque::iterator sIterator;

        sIterator = mSyncDeque.begin();
        for (; sIterator != mSyncDeque.end(); ++sIterator)
        {
            sSyncItem = *sIterator;
            if (XPR_IS_NULL(sSyncItem))
                continue;

            if (sSyncItem->isDirectory(0) || sSyncItem->isDirectory(1))
                sDirCount++;
            else
                sFileCount++;
        }

        if (XPR_IS_NOT_NULL(aDirCount))  *aDirCount  = sDirCount;
        if (XPR_IS_NOT_NULL(aFileCount)) *aFileCount = sFileCount;
    }

    return mSyncDeque.size();
}

xpr_size_t SyncDirs::getDiffCount(xpr_size_t *aEqualCount,
                                  xpr_size_t *aNotEqualCount,
                                  xpr_size_t *aLeftExistCount,
                                  xpr_size_t *aRightExistCount,
                                  xpr_size_t *aFailCount)
{
    xpr_size_t sTotalDiff = 0;

    xpr_size_t sEqualCount = 0;
    xpr_size_t sNotEqualCount = 0;
    xpr_size_t sLeftExistCount = 0;
    xpr_size_t sRightExistCount = 0;
    xpr_size_t sFailCount = 0;

    xpr_uint_t sExist, sDiff;
    SyncItem *sSyncItem;
    SyncDeque::iterator sIterator;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        sSyncItem = *sIterator;
        if (XPR_IS_NULL(sSyncItem))
            continue;

        sExist = sSyncItem->mExist;
        sDiff  = sSyncItem->getDiff();

        if (!XPR_TEST_BITS(sExist, CompareExistEqual))
        {
            if (XPR_TEST_BITS(sExist, CompareExistLeft))
                sLeftExistCount++;
            else
                sRightExistCount++;

            sTotalDiff++;
        }
        else
        {
            if (XPR_TEST_BITS(sDiff, CompareDiffNotEqualed))
            {
                sNotEqualCount++;
                sTotalDiff++;
            }

            if (XPR_TEST_BITS(sDiff, CompareDiffEqualed))
                sEqualCount++;

            if (XPR_TEST_BITS(sDiff, CompareDiffFailed))
                sFailCount++;
        }
    }

    if (XPR_IS_NOT_NULL(aEqualCount))      *aEqualCount      = sEqualCount;
    if (XPR_IS_NOT_NULL(aNotEqualCount))   *aNotEqualCount   = sNotEqualCount;
    if (XPR_IS_NOT_NULL(aLeftExistCount))  *aLeftExistCount  = sLeftExistCount;
    if (XPR_IS_NOT_NULL(aRightExistCount)) *aRightExistCount = sRightExistCount;
    if (XPR_IS_NOT_NULL(aFailCount))       *aFailCount       = sFailCount;

    return sTotalDiff;
}

xpr_size_t SyncDirs::getSyncFiles(xpr_uint_t aDirection, xpr_sint64_t *aSize)
{
    if (aDirection != SyncDirectionToRight && aDirection != SyncDirectionToLeft)
    {
        if (XPR_IS_NOT_NULL(aSize))
            *aSize = 0;

        return 0;
    }

    xpr_uint_t sSync = SyncToRight;
    if (aDirection == SyncDirectionToLeft)
        sSync = SyncToLeft;

    xpr_size_t sFiles = 0;
    xpr_sint64_t sSize = 0i64;

    SyncItem *sSyncItem;
    SyncDeque::iterator sIterator;

    sIterator = mSyncDeque.begin();
    for (; sIterator != mSyncDeque.end(); ++sIterator)
    {
        sSyncItem = *sIterator;
        if (XPR_IS_NULL(sSyncItem))
            continue;

        if (XPR_TEST_BITS(sSyncItem->getSync(), sSync))
        {
            sFiles++;

            if (sSync == SyncToRight)
                sSize += sSyncItem->mFileSize[0];
            else
                sSize += sSyncItem->mFileSize[1];
        }
    }

    if (XPR_IS_NOT_NULL(aSize))
        *aSize = sSize;

    return sFiles;
}

void SyncDirs::getDir(xpr_tchar_t *aDir1, xpr_tchar_t *aDir2)
{
    if (XPR_IS_NOT_NULL(aDir1)) _tcscpy(aDir1, mDir[0].c_str());
    if (XPR_IS_NOT_NULL(aDir2)) _tcscpy(aDir2, mDir[1].c_str());
}

void SyncDirs::getDir(std::tstring &aDir1, std::tstring &aDir2)
{
    aDir1 = mDir[0];
    aDir2 = mDir[1];
}

void SyncDirs::setDir(const xpr_tchar_t *aDir1, const xpr_tchar_t *aDir2)
{
    if (XPR_IS_NOT_NULL(aDir1)) mDir[0] = aDir1;
    if (XPR_IS_NOT_NULL(aDir2)) mDir[1] = aDir2;

    xpr_sint_t i;
    for (i = 0; i < 2; ++i)
    {
        std::tstring &sDir = mDir[i];
        if (sDir.length() > 2)
        {
            if (sDir[sDir.length()-1] == XPR_STRING_LITERAL('\\'))
                sDir.erase(sDir.length()-1, 1);
        }
    }
}

void SyncDirs::setDirection(xpr_uint_t aDirection)
{
    mDirection = aDirection;
}

void SyncDirs::setSubLevel(xpr_uint_t aSubLevel)
{
    mSubLevel = aSubLevel;
}

void SyncDirs::setExcludeExist(xpr_uint_t aExcludeExist)
{
    mExcludeExist = aExcludeExist;
}

static void getFilterList(const xpr_tchar_t *aFilter, FilterDeque &aFilterDeque)
{
    if (XPR_IS_NULL(aFilter))
        return;

    aFilterDeque.clear();

    xpr_size_t sOffset = 0;
    xpr_size_t sFind = std::tstring::npos;
    std::tstring sFilter = aFilter;
    std::tstring sString;

    while (true)
    {
        sFind = sFilter.find(XPR_STRING_LITERAL(';'), sOffset);
        if (sFind == std::tstring::npos)
            break;

        sString = sFilter.substr(sOffset, sFind-sOffset);
        if (sString.empty() == false)
            aFilterDeque.push_back(sString);
        sOffset = sFind + 1;
    }

    sString = sFilter.substr(sOffset);
    if (sString.empty() == false)
        aFilterDeque.push_back(sString);
}

void SyncDirs::setIncludeFilter(const xpr_tchar_t *aIncludeFilter)
{
    if (XPR_IS_NOT_NULL(aIncludeFilter))
        getFilterList(aIncludeFilter, mIncludeFilterDeque);
}

void SyncDirs::setExcludeFilter(const xpr_tchar_t *aExcludeFilter)
{
    if (XPR_IS_NOT_NULL(aExcludeFilter))
        getFilterList(aExcludeFilter, mExcludeFilterDeque);
}

void SyncDirs::setExcludeAttributes(DWORD aExcludeAttributes)
{
    mExcludeAttributes = aExcludeAttributes;
}

void SyncDirs::setDateTime(xpr_bool_t aDateTime)
{
    mDateTime = aDateTime;
}

void SyncDirs::setSize(xpr_bool_t aSize)
{
    mSize = aSize;
}

void SyncDirs::setAttributes(xpr_bool_t aAttributes)
{
    mAttributes = aAttributes;
}

void SyncDirs::setContents(xpr_uint_t aContents)
{
    mContents = aContents;
}

void SyncDirs::setBufferSize(xpr_size_t aBufferSize)
{
    mBufferSize = aBufferSize;
}

void SyncDirs::addItem(xpr_sint_t aIndex, const xpr_tchar_t *aPath)
{
    if (!XPR_IS_RANGE(0, aIndex, 1))
        return;

    if (_tcsnicmp(mDir[aIndex].c_str(), aPath, mDir[aIndex].length()) != 0)
        return;

    std::tstring sSubPath;
    sSubPath = aPath + mDir[aIndex].length();

    SyncMap::iterator sIterator;
    SyncItem *sSyncItem;

    sIterator = mSyncMap.find(sSubPath);
    if (sIterator == mSyncMap.end())
    {
        sSyncItem = new SyncItem;
        if (XPR_IS_NULL(sSyncItem))
            return;

        mSyncMap.insert(SyncMap::value_type(sSubPath, sSyncItem));

        sSyncItem->mSubPath = sSubPath;
    }
    else
    {
        sSyncItem = sIterator->second;
        if (XPR_IS_NULL(sSyncItem))
            return;
    }

    sSyncItem->mExist = (aIndex == 0) ? CompareExistLeft : CompareExistRight;
}

SyncItem *SyncDirs::getSyncItem(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mSyncDeque))
        return XPR_NULL;

    return mSyncDeque[aIndex];
}

xpr_uint_t SyncDirs::getStatus(Status &aStatus)
{
    CsLocker sLocker(mCs);

    aStatus = mStatus;

    switch (aStatus)
    {
    case StatusScaning:       return mScanDirCount;
    case StatusComparing:     return mCompareFileCount;
    case StatusSynchronizing: return mSyncFileCount;
    }

    return 0;
}
} // namespace fxb
