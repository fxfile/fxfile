//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "attr_time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
AttrTime::AttrTime(void)
    : mHwnd(XPR_NULL), mMsg(XPR_NULL)
    , mProcessedCount(0), mSucceededCount(0)
    , mCreatedFileTime(XPR_NULL), mModifiedFileTime(XPR_NULL), mAccessFileTime(XPR_NULL)
    , mType(TypeFile), mDepth(-1)
    , mStatus(StatusNone)
{
}

AttrTime::~AttrTime(void)
{
    Stop();

    mPathDeque.clear();

    XPR_SAFE_DELETE(mCreatedFileTime);
    XPR_SAFE_DELETE(mModifiedFileTime);
    XPR_SAFE_DELETE(mAccessFileTime);
}

void AttrTime::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

void AttrTime::setSubFolder(xpr_bool_t aSubFolder, xpr_sint_t aDepth)
{
    mSubFolder = aSubFolder;
    mDepth = aDepth;
}

void AttrTime::setType(xpr_uint_t aType)
{
    mType = aType;
}

void AttrTime::setAttributes(xpr_bool_t aAttribute, xpr_sint_t aArchive, xpr_sint_t aReadOnly, xpr_sint_t aHidden, xpr_sint_t aSystem)
{
    mAttribute = aAttribute;
    mArchive   = aArchive;
    mReadOnly  = aReadOnly;
    mHidden    = aHidden;
    mSystem    = aSystem;
}

void AttrTime::setTime(LPSYSTEMTIME aCreatedSystemTime, LPSYSTEMTIME aModifiedSystemTime, LPSYSTEMTIME aAccessSystemTime)
{
    XPR_SAFE_DELETE(mCreatedFileTime);
    XPR_SAFE_DELETE(mModifiedFileTime);
    XPR_SAFE_DELETE(mAccessFileTime);

    if (XPR_IS_NOT_NULL(aCreatedSystemTime))
    {
        FILETIME sLocalCreatedFileTime = {0};
        ::SystemTimeToFileTime(aCreatedSystemTime, &sLocalCreatedFileTime);

        mCreatedFileTime = new FILETIME;
        ::LocalFileTimeToFileTime(&sLocalCreatedFileTime, mCreatedFileTime);
    }

    if (XPR_IS_NOT_NULL(aModifiedSystemTime))
    {
        FILETIME sLocalModifiedFileTime = {0};
        ::SystemTimeToFileTime(aModifiedSystemTime, &sLocalModifiedFileTime);

        mModifiedFileTime = new FILETIME;
        ::LocalFileTimeToFileTime(&sLocalModifiedFileTime, mModifiedFileTime);
    }

    if (XPR_IS_NOT_NULL(aCreatedSystemTime))
    {
        FILETIME sLocalAccessFileTime = {0};
        ::SystemTimeToFileTime(aAccessSystemTime, &sLocalAccessFileTime);

        mAccessFileTime = new FILETIME;
        ::LocalFileTimeToFileTime(&sLocalAccessFileTime, mAccessFileTime);
    }
}

void AttrTime::addPath(const xpr::tstring &aPath)
{
    mPathDeque.push_back(aPath);
}

xpr_size_t AttrTime::getCount(void)
{
    return mPathDeque.size();
}

const xpr_tchar_t *AttrTime::getPath(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mPathDeque))
        return XPR_NULL;

    return mPathDeque[aIndex].c_str();
}

xpr_bool_t AttrTime::OnPreEntry(void)
{
    mProcessedCount = 0;
    mSucceededCount = 0;
    mStatus = StatusChanging;

    return XPR_TRUE;
}

unsigned AttrTime::OnEntryProc(void)
{
    xpr_sint_t sDepth = 0;
    DWORD sAttributes;

    xpr_bool_t sResult = XPR_TRUE;

    xpr::tstring sPath;
    PathDeque::iterator sIterator;

    sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
    {
        if (IsStop() == XPR_TRUE)
            break;

        sPath = *sIterator;

        sAttributes = GetFileAttributes(sPath.c_str());

        if ((XPR_IS_TRUE(mAttribute)) || (XPR_IS_NOT_NULL(mCreatedFileTime) || XPR_IS_NOT_NULL(mModifiedFileTime) || XPR_IS_NOT_NULL(mAccessFileTime)))
        {
            if ((XPR_TEST_BITS(mType, TypeFile)   && !XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY)) ||
                (XPR_TEST_BITS(mType, TypeFolder) &&  XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY)))
            {
                sResult = OnAttrTimeProc(sPath, sAttributes);
            }
        }

        if (XPR_IS_TRUE(mSubFolder) && XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            if (sDepth < mDepth)
            {
                sDepth++;
                OnRcsvDirAttrTimeProc(sPath, sAttributes, sDepth);
                sDepth--;
            }
        }

        {
            xpr::MutexGuard sLockGuard(mMutex);

            mProcessedCount++;
            if (XPR_IS_TRUE(sResult))
                mSucceededCount++;
        }
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = (IsStop() == XPR_TRUE) ? StatusStopped : StatusChangeCompleted;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

xpr_bool_t AttrTime::OnRcsvDirAttrTimeProc(const xpr::tstring &strDir, DWORD sAttributes, xpr_sint_t sDepth)
{
    if (IsStop() == XPR_TRUE)
        return XPR_TRUE;

    xpr::tstring sPath = strDir + XPR_STRING_LITERAL('\\');
    sPath += XPR_STRING_LITERAL("*.*");

    HANDLE sFile;
    WIN32_FIND_DATA sWin32FindData = {0};

    sFile = ::FindFirstFile(sPath.c_str(), &sWin32FindData);
    if (sFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    do
    {
        if (_tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
            continue;

        sPath = strDir + XPR_STRING_LITERAL('\\') + sWin32FindData.cFileName;

        if ((XPR_IS_TRUE(mAttribute)) || (XPR_IS_NOT_NULL(mCreatedFileTime) || XPR_IS_NOT_NULL(mModifiedFileTime) || XPR_IS_NOT_NULL(mAccessFileTime)))
        {
            if ((XPR_TEST_BITS(mType, TypeFile)   && !XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY)) ||
                (XPR_TEST_BITS(mType, TypeFolder) &&  XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY)))
            {
                OnAttrTimeProc(sPath, sAttributes);
            }
        }

        if (XPR_TEST_BITS(sWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
        {
            if (sDepth < mDepth)
            {
                sDepth++;
                OnRcsvDirAttrTimeProc(sPath, sAttributes, sDepth);
                sDepth--;
            }
        }

        if (IsStop() == XPR_TRUE)
            break;
    }
    while (::FindNextFile(sFile, &sWin32FindData));

    ::FindClose(sFile);

    return XPR_TRUE;
}

xpr_bool_t AttrTime::OnAttrTimeProc(const xpr::tstring &aPath, DWORD aAttributes)
{
    xpr_bool_t sResult = XPR_FALSE;

    DWORD sAttributes2 = GetFileAttributes(aPath.c_str());
    if (XPR_IS_TRUE(mAttribute))
    {
        if (mArchive  == 0) sAttributes2 &= ~FILE_ATTRIBUTE_ARCHIVE;
        if (mReadOnly == 0) sAttributes2 &= ~FILE_ATTRIBUTE_READONLY;
        if (mHidden   == 0) sAttributes2 &= ~FILE_ATTRIBUTE_HIDDEN;
        if (mSystem   == 0) sAttributes2 &= ~FILE_ATTRIBUTE_SYSTEM;

        if (mArchive  == 1) sAttributes2 |= FILE_ATTRIBUTE_ARCHIVE;
        if (mReadOnly == 1) sAttributes2 |= FILE_ATTRIBUTE_READONLY;
        if (mHidden   == 1) sAttributes2 |= FILE_ATTRIBUTE_HIDDEN;
        if (mSystem   == 1) sAttributes2 |= FILE_ATTRIBUTE_SYSTEM;

        sResult = SetFileAttributes(aPath.c_str(), sAttributes2);
    }

    if (XPR_IS_NOT_NULL(mCreatedFileTime) || XPR_IS_NOT_NULL(mModifiedFileTime) || XPR_IS_NOT_NULL(mAccessFileTime))
    {
        SetFileTime(aPath.c_str(), mCreatedFileTime, mAccessFileTime, mModifiedFileTime);
    }

    return sResult;
}

AttrTime::Status AttrTime::getStatus(xpr_size_t *aProcessedCount, xpr_size_t *aSucceededCount)
{
    xpr::MutexGuard sLockGuard(mMutex);

    if (XPR_IS_NOT_NULL(aProcessedCount)) *aProcessedCount = mProcessedCount;
    if (XPR_IS_NOT_NULL(aSucceededCount)) *aSucceededCount = mSucceededCount;

    return mStatus;
}
} // namespace cmd
} // namespace fxfile
