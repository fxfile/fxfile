//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_list.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
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

    xpr_size_t sOutputBytes;
    xpr_char_t sAnsiSeparator[0xff] = {0};

    sOutputBytes = sizeof(0xfe) * sizeof(xpr_tchar_t);
    XPR_TCS_TO_MBS(mSplitChar.c_str(), mSplitChar.length() * sizeof(xpr_tchar_t), sAnsiSeparator, &sOutputBytes);
    sAnsiSeparator[sOutputBytes / sizeof(xpr_char_t)] = 0;

    FILE *sFile = _tfopen(mTextFilePath.c_str(), XPR_STRING_LITERAL("wt"));
    if (XPR_IS_NOT_NULL(sFile))
    {
        mPathMaxLen += 6;
        mFileMaxLen += 6;
        xpr_char_t sFormat[50];
        sprintf(sFormat, "%%-%ds", XPR_IS_TRUE(sFrontPath) ? mPathMaxLen : mFileMaxLen);

        xpr_tchar_t sName[XPR_MAX_PATH + 1];
        xpr_char_t sAnsiName[XPR_MAX_PATH + 1];
        DWORD sFileAttributes;

        PathDeque::iterator sIterator;
        std::tstring sPath;

        sIterator = mPathDeque.begin();
        for (; sIterator != mPathDeque.end(); ++sIterator)
        {
            if (IsStop() == XPR_TRUE)
                break;

            sPath = *sIterator;

            sFileAttributes = ::GetFileAttributes(sPath.c_str());
            if (XPR_IS_TRUE(sOnlyFile) && XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                continue;

            if (XPR_IS_FALSE(sFrontPath))
                sPath = sPath.substr(sPath.rfind(XPR_STRING_LITERAL('\\'))+1);

            if (XPR_IS_FALSE(sExtension) && !XPR_TEST_BITS(sFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
            {
                const xpr_tchar_t *sExt = GetFileExt(sPath.c_str());
                if (XPR_IS_NOT_NULL(sExt))
                    sPath.erase(sExt - sPath.c_str());
            }

            _tcscpy(sName, sPath.c_str());

            sOutputBytes = XPR_MAX_PATH * sizeof(xpr_tchar_t);
            XPR_TCS_TO_MBS(sPath.c_str(), sPath.length() * sizeof(xpr_tchar_t), sAnsiName, &sOutputBytes);
            sAnsiName[sOutputBytes / sizeof(xpr_char_t)] = 0;

            if (XPR_IS_TRUE(sAttribute))
            {
                if (XPR_IS_TRUE(sSplitChar)) fprintf(sFile, "%s", sAnsiName);
                else                         fprintf(sFile, sFormat, sAnsiName);

                GetFileSize(sPath.c_str(), sName, XPR_MAX_PATH);
                if (XPR_IS_TRUE(sSplitChar)) fprintf(sFile, "%s%s", sAnsiSeparator, sAnsiName);
                else                         fprintf(sFile, " %20s", sAnsiName);

                GetFileType(sPath.c_str(), sName);
                if (XPR_IS_TRUE(sSplitChar)) fprintf(sFile, "%s%s", sAnsiSeparator, sAnsiName);
                else                         fprintf(sFile, " %-20s", sAnsiName);

                GetFileTime(sPath.c_str(), sName);
                if (XPR_IS_TRUE(sSplitChar)) fprintf(sFile, "%s%s", sAnsiSeparator, sAnsiName);
                else                         fprintf(sFile, " %-20s", sAnsiName);
            }
            else
            {
                fprintf(sFile, "%s", sAnsiName);
            }

            if (XPR_IS_TRUE(sByLine)) fprintf(sFile, "\n");
            else                      fprintf(sFile, " ");
        }

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

FileList::Status FileList::getStatus(void)
{
    CsLocker sLocker(mCs);

    return mStatus;
}
} // namespace fxb
