//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SYNC_DIRS_H__
#define __FXFILE_SYNC_DIRS_H__ 1
#pragma once

#include "xpr_thread_sync.h"

namespace fxfile
{
enum CompareDiff
{
    CompareDiffNone            = 0x00000000,
    CompareDiffFailed          = 0xffffffff,     //  !
    CompareDiffToLeft          = 0x01000000,     //  <-
    CompareDiffToRight         = 0x02000000,     // ->
    CompareDiffEqualed         = 0x10000000,     // ===
    CompareDiffNotEqualed      = 0x20000000,     // =/=
    CompareDiffSize            = 0x00000001,
    CompareDiffTime            = 0x00000002,
    CompareDiffAttributes      = 0x00000004,
    CompareDiffContentsBytes   = 0x00000100,
    CompareDiffContentsHashSFV = 0x00000200,
    CompareDiffContentsHashMD5 = 0x00000400,
};

enum CompareContents
{
    CompareContentsNone = 0,
    CompareContentsHashSFV,
    CompareContentsHashMD5,
    CompareContentsBytes,
};

enum CompareExist
{
    CompareExistNone  = 0x0000,
    CompareExistLeft  = 0x0001,
    CompareExistRight = 0x0002,
    CompareExistEqual = CompareExistLeft + CompareExistRight,
    CompareExistOther = 0x0004,
};

enum SyncDirection
{
    SyncDirectionNone = 0,
    SyncDirectionToLeft,
    SyncDirectionToRight,
    SyncDirectionBidirection,
};

enum Sync
{
    SyncNone = 0,
    SyncFailed,
    SyncEqualed,
    SyncNotEqualed,
    SyncToLeft,
    SyncToRight,
};

class SyncItem
{
public:
    SyncItem(void);
    ~SyncItem(void);

public:
    void clear(void);

public:
    const xpr_tchar_t *getSubPath(void);
    void getSubPath(xpr::tstring &aSubPath);
    xpr_bool_t getPath(const xpr_tchar_t *aDir, xpr_tchar_t *aPath);
    xpr_bool_t getPath(const xpr_tchar_t *aDir, xpr::tstring &aPath);
    xpr_bool_t getPath(const xpr::tstring &aDir, xpr::tstring &aPath);

    LPFILETIME getTime(xpr_sint_t aIndex);

    xpr_uint_t getDiff(void);
    xpr_uint_t getSync(void);
    void setSync(xpr_uint_t aSync);
    void setOrgSync(void);

    xpr_bool_t isDirectory(xpr_sint_t aIndex);
    xpr_bool_t isExistLeft(void);
    xpr_bool_t isExistRight(void);
    xpr_bool_t isExistBoth(void);

protected:
    void setDiff(xpr_uint_t aDiff);

public:
    class CompareFlags
    {
    public:
        CompareFlags(void)
        {
            mBuffer[0] = mBuffer[1] = XPR_NULL;
        }

        ~CompareFlags(void)
        {
            XPR_SAFE_DELETE_ARRAY(mBuffer[0]);
            XPR_SAFE_DELETE_ARRAY(mBuffer[1]);
        }

    public:
        xpr_bool_t    mEqualDir;
        xpr::tstring  mDir[2];
        xpr::tstring  mPath[2];
        xpr_bool_t    mDateTime;
        xpr_bool_t    mAttributes;
        xpr_bool_t    mSize;
        xpr_uint_t    mContents;
        xpr_char_t   *mBuffer[2];
        xpr_size_t    mBufferSize;
        xpr_char_t    mCrc[2][100];
        HANDLE        mStopEvent;
    };

    class SyncFlags
    {
    public:
        SyncFlags(void)
        {
            mBuffer = XPR_NULL;
        }

        ~SyncFlags(void)
        {
            XPR_SAFE_DELETE_ARRAY(mBuffer);
        }

    public:
        xpr_uint_t    mDirection;
        xpr::tstring  mDir[2];
        xpr::tstring  mPath[2];
        HANDLE        mStopEvent;
        xpr_char_t   *mBuffer;
        xpr_size_t    mBufferSize;
    };

    enum Result
    {
        ResultFailed,
        ResultCompared,
        ResultStopEvent,
        ResultSkiped,
        ResultSynchronized,
    };

    inline Result compare(CompareFlags &aCompareFlags);
    inline Result synchronize(SyncFlags &aSyncFlags, CompareFlags &aCompareFlags);

public:
    xpr_sint_t   mSubLevel;
    xpr::tstring mSubPath;

public:
    xpr_sint64_t mFileSize[2];
    FILETIME     mCreatedFileTime[2];
    FILETIME     mLastAccessFileTime[2];
    FILETIME     mModifiedFileTime[2];
    DWORD        mFileAttributes[2];
    xpr_char_t  *mHash[2];

public:
    xpr_uint_t mExist;
    xpr_uint_t mDiff;
    xpr_uint_t mSync;
    xpr_uint_t mOrgSync;
};

typedef std::tr1::unordered_multimap<xpr::tstring, SyncItem *> SyncMap;
typedef std::pair<SyncMap::iterator, SyncMap::iterator> SyncMapPairIterator;
typedef std::deque<SyncItem *> SyncDeque;
typedef std::deque<xpr::tstring> FilterDeque;

class SyncDirs
{
public:
    enum Status
    {
        StatusNone = 0,
        StatusScaning,
        StatusScanCompleted,
        StatusComparing,
        StatusCompareCompleted,
        StatusSynchronizing,
        StatusSynchronizeCompleted,
    };

public:
    SyncDirs(const xpr_tchar_t *aDir1 = XPR_NULL, const xpr_tchar_t *aDir2 = XPR_NULL);
    virtual ~SyncDirs(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

public:
    void clear(void);

public:
    void getDir(xpr_tchar_t *aDir1, xpr_tchar_t *aDir2);
    void getDir(xpr::tstring &aDir1, xpr::tstring &aDir2);
    void setDir(const xpr_tchar_t *aDir1, const xpr_tchar_t *aDir2);

    void setDirection(xpr_uint_t aDirection);
    void setSubLevel(xpr_uint_t aSubLevel);

    void setExcludeExist(xpr_uint_t aExcludeExist);
    void setIncludeFilter(const xpr_tchar_t *aIncludeFilter);
    void setExcludeFilter(const xpr_tchar_t *aExcludeFilter);
    void setExcludeAttributes(DWORD aExcludeAttribs);

    void setDateTime(xpr_bool_t aDateTime);
    void setSize(xpr_bool_t aSize);
    void setAttributes(xpr_bool_t aAttributes);
    void setContents(xpr_uint_t aContents);
    void setBufferSize(xpr_size_t aBufferSize);

    xpr_bool_t scan(void);
    xpr_bool_t compare(void);
    xpr_bool_t scanCompare(void);
    xpr_bool_t synchronize(void);
    void stop(DWORD aStopMillseconds = INFINITE);

    void addItem(xpr_sint_t aIndex, const xpr_tchar_t *aPath);
    SyncItem *getSyncItem(xpr_sint_t aIndex);

    xpr_size_t getCount(xpr_size_t *aDirCount = XPR_NULL, xpr_size_t *aFileCount = XPR_NULL);
    xpr_size_t getDiffCount(xpr_size_t *aEqualCount = XPR_NULL, xpr_size_t *aNotEqualCount = XPR_NULL, xpr_size_t *aLeftExistCount = XPR_NULL, xpr_size_t *aRightExistCount = XPR_NULL, xpr_size_t *aFailCount = XPR_NULL);
    xpr_size_t getSyncFiles(xpr_uint_t aDirection, xpr_sint64_t *aSize = XPR_NULL);

public:
    xpr_uint_t getStatus(Status &aSatus);

protected:
    void scanRecursiveDir(SyncMap &aSyncMap, const xpr_sint_t aIndex, xpr_uint_t aLevel, const xpr::tstring &aDir, const xpr::tstring &aBaseDir);
    static bool sortDir(SyncItem *&aSyncItem1, SyncItem *&aSyncItem2);

    void getCompareFlags(SyncItem::CompareFlags &aCompareFlags);

protected:
    xpr::tstring mDir[2];

    xpr_uint_t   mDirection; // [0] <->, (1) ->, (2) <-
    xpr_uint_t   mSubLevel;

    xpr_uint_t   mExcludeExist;
    FilterDeque  mIncludeFilterDeque;
    FilterDeque  mExcludeFilterDeque;
    DWORD        mExcludeAttributes;

    xpr_bool_t   mDateTime;
    xpr_bool_t   mSize;
    xpr_bool_t   mAttributes;
    xpr_uint_t   mContents;
    xpr_size_t   mBufferSize;

    SyncMap      mSyncMap;
    SyncDeque    mSyncDeque;

protected:
    Status       mStatus;
    xpr_uint_t   mScanDirCount;
    xpr_uint_t   mCompareFileCount;
    xpr_uint_t   mSyncFileCount;
    xpr::Mutex   mMutex;

protected:
    static unsigned __stdcall ScanProc(void *aParam);
    static unsigned __stdcall CompareProc(void *aParam);
    static unsigned __stdcall ScanCompareProc(void *aParam);
    static unsigned __stdcall SynchronizeProc(void *aParam);
    DWORD OnScan(void);
    DWORD OnCompare(void);
    DWORD OnScanCompare(void);
    DWORD OnSynchronize(void);
    inline xpr_bool_t isStopThread(void);

protected:
    HANDLE   mThread;
    unsigned mThreadId;
    HANDLE   mStopEvent;
    HANDLE   mStoppedEvent;

    HWND       mHwnd;
    xpr_uint_t mMsg;
};
} // namespace fxfile

#endif // __FXFILE_SYNC_DIRS_H__
