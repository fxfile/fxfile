//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_list.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
FileList::FileList(void)
    : mHwnd(XPR_NULL), mMsg(XPR_NULL)
    , mPathMaxLen(-1)
    , mFileMaxLen(-1)
    , mFlags(0)
    , mStatus(StatusNone)
{
}

FileList::~FileList(void)
{
    Stop();

    mPathDeque.clear();
}

void FileList::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

xpr_uint_t FileList::getFlags(void)
{
    return mFlags;
}

void FileList::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

void FileList::setSplitChar(const xpr_tchar_t *aSplitChar)
{
    if (XPR_IS_NOT_NULL(aSplitChar))
        mSplitChar = aSplitChar;
}

void FileList::setTextFile(const xpr_tchar_t *aTextFile)
{
    if (XPR_IS_NOT_NULL(aTextFile))
        mTextFilePath = aTextFile;
}

void FileList::addPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(aPath);
    if (sLen > mPathMaxLen)
        mPathMaxLen = sLen;

    xpr_tchar_t *sSplit = (xpr_tchar_t *)_tcsrchr(aPath, '\\');
    if (XPR_IS_NOT_NULL(sSplit))
    {
        xpr_sint_t sFileLen = (xpr_sint_t)_tcslen(sSplit+1);
        if (sFileLen > mFileMaxLen)
            mFileMaxLen = sFileLen;
    }

    mPathDeque.push_back(aPath);
}

xpr_bool_t FileList::OnPreEntry(void)
{
    mStatus = StatusCreating;

    return XPR_TRUE;
}

unsigned FileList::OnEntryProc(void)
{
    xpr_bool_t sByLine    = XPR_TEST_BITS(mFlags, FlagsByLine);
    xpr_bool_t sOnlyFile  = XPR_TEST_BITS(mFlags, FlagsOnlyFile);
    xpr_bool_t sFrontPath = XPR_TEST_BITS(mFlags, FlagsWithDir);
    xpr_bool_t sExtension = XPR_TEST_BITS(mFlags, FlagsExtension);
    xpr_bool_t sAttribute = XPR_TEST_BITS(mFlags, FlagsAttribute);
    xpr_bool_t sSplitChar = XPR_TEST_BITS(mFlags, FlagsSplitChar);

    xpr_rcode_t sRcode;
    xpr_sint_t sOpenMode;
    xpr::FileIo sFileIo;
    const xpr_tchar_t *sSeparator = mSplitChar.c_str();

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sFileIo.open(mTextFilePath, sOpenMode);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        xpr::TextFileWriter sTextFileWriter(sFileIo);

        sTextFileWriter.setEncoding((sizeof(xpr_tchar_t) == 2) ? xpr::CharSetUtf16 : xpr::CharSetMultiBytes);
        sTextFileWriter.setEndOfLine(xpr::TextFileWriter::kUnixStyle);
        sTextFileWriter.writeBom();

        mPathMaxLen += 6;
        mFileMaxLen += 6;
        xpr_tchar_t sFormat[50];
        _stprintf(sFormat, XPR_STRING_LITERAL("%%-%ds"), XPR_IS_TRUE(sFrontPath) ? mPathMaxLen : mFileMaxLen);

        xpr_tchar_t sName[XPR_MAX_PATH + 1];
        DWORD sFileAttributes;

        PathDeque::iterator sIterator;
        xpr::string sPath;

        sIterator = mPathDeque.begin();
        for (; sIterator != mPathDeque.end(); ++sIterator)
        {
            if (IsStop() == XPR_TRUE)
                break;

            sPath = *sIterator;

            sFileAttributes = ::GetFileAttributes(sPath.c_str());
            if (XPR_IS_TRUE(sOnlyFile) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                continue;

            // file name
            _tcscpy(sName, sPath.c_str());

            if (XPR_IS_FALSE(sFrontPath))
                _tcscpy(sName, sPath.substr(sPath.rfind(XPR_STRING_LITERAL('\\'))+1).c_str());

            if (XPR_IS_FALSE(sExtension) && !XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sName);
                if (XPR_IS_NOT_NULL(sExt))
                    *sExt = 0;
            }

            if (XPR_IS_TRUE(sAttribute))
            {
                // file name
                if (XPR_IS_TRUE(sSplitChar))
                {
                    sTextFileWriter.write(sName);
                }
                else
                {
                    sTextFileWriter.writeFormat(sFormat, sName);
                }

                // file size
                if (XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                    sName[0] = 0;
                else
                    GetFileSize(sPath.c_str(), sName, XPR_MAX_PATH);

                if (XPR_IS_TRUE(sSplitChar))
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL("%s%s"), sSeparator, sName);
                }
                else
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL(" %20s"), sName);
                }

                // file type
                GetFileType(sPath.c_str(), sName);

                if (XPR_IS_TRUE(sSplitChar))
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL("%s%s"), sSeparator, sName);
                }
                else
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL(" %-20s"), sName);
                }

                // file time
                GetFileTime(sPath.c_str(), sName);

                if (XPR_IS_TRUE(sSplitChar))
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL("%s%s"), sSeparator, sName);
                }
                else
                {
                    sTextFileWriter.writeFormat(XPR_STRING_LITERAL(" %-20s"), sName);
                }
            }
            else
            {
                sTextFileWriter.writeFormat(XPR_STRING_LITERAL("%s"), sName);
            }

            if (XPR_IS_TRUE(sByLine))
            {
                sTextFileWriter.write(XPR_EOL);
            }
            else
            {
                sTextFileWriter.write(XPR_STRING_LITERAL(" "));
            }
        }

        sFileIo.close();

        {
            xpr::MutexGuard sLockGuard(mMutex);
            mStatus = IsStop() ? StatusStopped : StatusCreateCompleted;
        }
    }
    else
    {
        xpr::MutexGuard sLockGuard(mMutex);
        mStatus = StatusFailed;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)XPR_NULL, (LPARAM)XPR_NULL);

    return 0;
}

FileList::Status FileList::getStatus(void)
{
    xpr::MutexGuard sLockGuard(mMutex);

    return mStatus;
}
} // namespace cmd
} // namespace fxfile
