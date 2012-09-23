//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_search_file.h"

#include "fxb_fnmatch.h"              // for pattern matching

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
#define MAX_SEARCH_TEXT_BUF_SIZE 10240

SearchFile::SearchFile(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mTextA(XPR_NULL), mTextW(XPR_NULL)
    , mSearchedCount(0), mSearchTime(0)
    , mStatus(StatusNone)
    , mBuffer(XPR_NULL)
    , mFlags(0)
    , mParam(XPR_NULL)
    , mSearchResultFunc(XPR_NULL)
{
    memset(&mWin32FindData, 0, sizeof(mWin32FindData));
}

SearchFile::~SearchFile(void)
{
    Stop();

    SchDir *sSchDir;
    SchDirDeque::iterator sIterator;

    sIterator = mSchIncDirDeque.begin();
    for (; sIterator != mSchIncDirDeque.end(); ++sIterator)
    {
        sSchDir = *sIterator;
        XPR_SAFE_DELETE(sSchDir);
    }

    sIterator = mSchExcDirDeque.begin();
    for (; sIterator != mSchExcDirDeque.end(); ++sIterator)
    {
        sSchDir = *sIterator;
        XPR_SAFE_DELETE(sSchDir);
    }

    mSchIncDirDeque.clear();
    mSchExcDirDeque.clear();

    XPR_SAFE_DELETE_ARRAY(mTextA);
    XPR_SAFE_DELETE_ARRAY(mTextW);
    XPR_SAFE_DELETE_ARRAY(mBuffer);
}

void SearchFile::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

xpr_bool_t SearchFile::isFlag(xpr_uint_t aFlag)
{
    return XPR_TEST_BITS(mFlags, aFlag);
}

xpr_uint_t SearchFile::getFlags(void)
{
    return mFlags;
}

void SearchFile::setFlags(xpr_uint_t aFlags)
{
    mFlags = aFlags;
}

static xpr_size_t MultiStr2List(xpr_tchar_t *aMultiStr, std::deque<std::tstring> &aStringDeque)
{
    if (XPR_IS_NULL(aMultiStr))
        return 0;

    xpr_sint_t i;
    std::tstring sString;
    std::tstring::iterator sIterator;

    xpr_tchar_t *sStringEnum = aMultiStr;

    for (i = 0; aMultiStr[i]; ++i)
    {
        if (aMultiStr[i] != XPR_STRING_LITERAL(';'))
            continue;

        aMultiStr[i] = XPR_STRING_LITERAL('\0');

        sString = sStringEnum;
        if (sString.empty() == false)
            aStringDeque.push_back(sString);

        aMultiStr[i] = XPR_STRING_LITERAL(';');

        sStringEnum = aMultiStr + i + 1;
    }

    sString = sStringEnum;
    if (sString.empty() == false)
        aStringDeque.push_back(sString);

    return aStringDeque.size();
}

void SearchFile::setName(const xpr_tchar_t *aName, xpr_bool_t aNoWildcard)
{
    if (XPR_IS_NULL(aName))
        return;

    xpr_tchar_t *sName = new xpr_tchar_t[_tcslen(aName) + 1];
    _tcscpy(sName, aName);

    MultiStr2List(sName, mNameDeque);

    if (aNoWildcard == XPR_FALSE)
    {
        if (mNameDeque.empty() == false)
        {
            NameDeque::iterator sIterator = mNameDeque.begin();
            for (; sIterator != mNameDeque.end(); ++sIterator)
            {
                std::tstring &sName = *sIterator;

                fxb::trim_left_right(sName);

                if (sName.find(XPR_STRING_LITERAL('*')) == std::tstring::npos && sName.find(XPR_STRING_LITERAL('?')) == std::tstring::npos)
                {
                    sName.insert(0, XPR_STRING_LITERAL("*"));
                    sName.append(XPR_STRING_LITERAL("*"));
                }
            }
        }
        else
        {
            mNameDeque.push_back(XPR_STRING_LITERAL("*"));
        }
    }

    XPR_SAFE_DELETE_ARRAY(sName);
}

void SearchFile::setText(const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(aText))
        return;

    xpr_size_t sLen = _tcslen(aText);
    xpr_size_t sOutputBytes;

    XPR_SAFE_DELETE_ARRAY(mTextA);
    XPR_SAFE_DELETE_ARRAY(mTextW);

    mTextA = new xpr_char_t[sLen + 1];
    mTextW = new xpr_wchar_t[sLen + 1];

    sOutputBytes = sLen * sizeof(xpr_char_t);
    XPR_TCS_TO_MBS(aText, sLen * sizeof(xpr_tchar_t), mTextA, &sOutputBytes);
    mTextA[sOutputBytes / sizeof(xpr_char_t)] = 0;

    sOutputBytes = sLen * sizeof(xpr_wchar_t);
    XPR_TCS_TO_UTF16(aText, sLen * sizeof(xpr_tchar_t), mTextW, &sOutputBytes);
    mTextW[sOutputBytes / sizeof(xpr_wchar_t)] = 0;

    _strlwr(mTextA);
    _wcslwr(mTextW);
}

void SearchFile::addIncludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder)
{
    if (XPR_IS_NULL(aDir))
        return;

    SchDir *sSchDir = new SchDir;
    if (XPR_IS_NULL(sSchDir))
        return;

    sSchDir->mPath      = aDir;
    sSchDir->mSubFolder = aSubFolder;

    mSchIncDirDeque.push_back(sSchDir);
}

void SearchFile::addExcludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder)
{
    if (XPR_IS_NULL(aDir))
        return;

    SchDir *sSchDir = new SchDir;
    if (XPR_IS_NULL(sSchDir))
        return;

    sSchDir->mPath      = aDir;
    sSchDir->mSubFolder = aSubFolder;

    mSchExcDirDeque.push_back(sSchDir);
}

xpr_bool_t SearchFile::OnPreEntry(void)
{
    mSearchedCount = 0;
    mStatus = StatusSearching;

    mSubFolder = isFlag(FlagSubFolder);
    mCase      = isFlag(FlagCase);
    mSystem    = isFlag(FlagSystem);

    mType = 0;
    if (isFlag(FlagTypeFile | FlagTypeFolder))
        mType = 0;
    else if (isFlag(FlagTypeFolder))
        mType = 1;
    else
        mType = 2;

    mFlags = 0;
    if (XPR_IS_FALSE(mCase))
        mMatchFlags |= FNM_CASEFOLD;

    if (XPR_IS_NULL(mBuffer))
        mBuffer = new xpr_byte_t[MAX_SEARCH_TEXT_BUF_SIZE + 1];

    return XPR_TRUE;
}

unsigned SearchFile::OnEntryProc(void)
{
    clock_t sStartClock, sStopClock;
    sStartClock = clock();

    SchDir *sSchDir;
    SchDirDeque::iterator sIterator;

    sIterator = mSchIncDirDeque.begin();
    for (; sIterator != mSchIncDirDeque.end(); ++sIterator)
    {
        sSchDir = *sIterator;
        if (XPR_IS_NULL(sSchDir))
            continue;

        searchRecursive(0, sSchDir->mPath.c_str(), sSchDir->mSubFolder);
    }

    sStopClock = clock();

    {
        CsLocker aLocker(mCs);
        mStatus = IsStop() ? StatusStopped : StatusSearchCompleted;
        mSearchTime = sStopClock - sStartClock;
    }

    ::PostMessage(mHwnd, mMsg, (WPARAM)0, (LPARAM)0);

    return 0;
}

void SearchFile::searchRecursive(xpr_sint_t aDepth, const xpr_tchar_t *aFolder, xpr_bool_t aSubFolder)
{
    if (IsStop())
        return;

    if (mSchExcDirDeque.empty() == false)
    {
        SchDir *sSchDir;
        SchDirDeque::iterator sIterator;

        sIterator = mSchExcDirDeque.begin();
        for (; sIterator != mSchExcDirDeque.end(); ++sIterator)
        {
            sSchDir = *sIterator;
            if (XPR_IS_NULL(sSchDir))
                continue;

            if (_tcsicmp(aFolder, sSchDir->mPath.c_str()) == 0)
                return;
        }
    }

    NameDeque::iterator sIterator;
    xpr_bool_t sMatched;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    _stprintf(sPath, XPR_STRING_LITERAL("%s\\*.*"), aFolder);

    HANDLE sFindFile = ::FindFirstFile(sPath, &mWin32FindData);
    if (sFindFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!_tcscmp(mWin32FindData.cFileName, XPR_STRING_LITERAL(".")) || !_tcscmp(mWin32FindData.cFileName, XPR_STRING_LITERAL("..")))
                continue;

            // Advanced options
            // ------------------------
            // 1. sub-folder
            // 2. case sensitivity
            // 3. no wildcard
            // 4. type (all, only folder, only file)
            // 5. system attribute

            if (XPR_IS_FALSE(mSystem) && XPR_TEST_BITS(mWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_SYSTEM))
                continue;

            if ((mType == 0) ||
                (mType == 1 && (mWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
                (mType == 2 && (mWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY))
            {
                sMatched = XPR_FALSE;

                sIterator = mNameDeque.begin();
                for (; sIterator != mNameDeque.end(); ++sIterator)
                {
                    std::tstring &sName = *sIterator;

                    if (fnmatch(sName.c_str(), mWin32FindData.cFileName, mMatchFlags) != FNM_NOMATCH)
                    {
                        sMatched = XPR_TRUE;
                        if (isSearchText() == XPR_TRUE)
                            sMatched = searchText(aFolder, mWin32FindData.cFileName);
                        break;
                    }
                }

                if (XPR_IS_TRUE(sMatched))
                {
                    if (XPR_IS_NOT_NULL(mSearchResultFunc))
                        mSearchResultFunc(aFolder, &mWin32FindData, mParam);
                }
            }

            if (XPR_IS_TRUE(aSubFolder))
            {
                if (XPR_IS_TRUE(mSubFolder) && XPR_TEST_BITS(mWin32FindData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    _stprintf(sPath, XPR_STRING_LITERAL("%s%s%s"),
                        aFolder,
                        (aFolder[_tcslen(aFolder)-1] == XPR_STRING_LITERAL('\\')) ? XPR_STRING_LITERAL("") : XPR_STRING_LITERAL("\\"),
                        mWin32FindData.cFileName);

                    searchRecursive(aDepth + 1, sPath, XPR_TRUE);
                }
            }

            if (IsStop())
                break;
        }
        while (::FindNextFile(sFindFile, &mWin32FindData));
    }

    ::FindClose(sFindFile);
}

xpr_bool_t SearchFile::searchText(const xpr_tchar_t *aFolder, const xpr_tchar_t *aFileName)
{
    static xpr_tchar_t sPath[XPR_MAX_PATH + 1];
    static xpr_size_t i, j, sLen, sTextLen;
    static xpr_char_t  *sBufferA, *sBufferA2;
    static xpr_wchar_t *sBufferW, *sBufferW2;
    static FILE *sFile;

    _stprintf(sPath, XPR_STRING_LITERAL("%s\\%s"), aFolder, aFileName);
    sFile = _tfopen(sPath, XPR_STRING_LITERAL("rb"));
    if (XPR_IS_NULL(sFile))
        return XPR_FALSE;

    // Is Ansi or Unciode text file?
    xpr_bool_t sUnicode = XPR_FALSE;
    xpr_ushort_t sCode = 0;
    fread(&sCode, 2, 1, sFile);
    if (sCode == 0xFEFF)
        sUnicode = XPR_TRUE;
    else
        fseek(sFile, 0, SEEK_SET);

    // search text in file
    xpr_bool_t sMatched = XPR_FALSE;
    if (XPR_IS_TRUE(sUnicode))
    {
        if (XPR_IS_NULL(mTextW))
            return XPR_FALSE;

        sLen = 0;
        sTextLen = (xpr_sint_t)wcslen(mTextW);
        sBufferW  = (xpr_wchar_t *)mBuffer;
        sBufferW2 = mTextW;

        while (true)
        {
            if (feof(sFile))
                break;

            sLen = fread(sLen == 0 ? mBuffer : mBuffer+sTextLen*sizeof(xpr_wchar_t), 1,
                MAX_SEARCH_TEXT_BUF_SIZE-sTextLen*sizeof(xpr_wchar_t)-1, sFile) + ((sLen == 0) ? 0 : sTextLen);

            sLen /= sizeof(xpr_wchar_t);
            for (i = 0; i < sLen; ++i)
            {
                if ('A' <= sBufferW[i] && sBufferW[i] <= 'Z')
                    sBufferW[i] += 32;

                if (sBufferW[i] == sBufferW2[0])
                {
                    for (j = 1; j < sTextLen; ++j)
                    {
                        if ('A' <= sBufferW[i] && sBufferW[i] <= 'Z')
                            sBufferW[i] += 32;

                        if (sBufferW[i+j] != sBufferW2[j])
                            break;
                    }

                    if (j == sTextLen)
                    {
                        sMatched = XPR_TRUE;
                        break;
                    }
                }
            }

            if (XPR_IS_TRUE(sMatched))
                break;

            memmove(sBufferW, sBufferW+sLen-sTextLen, sTextLen*sizeof(xpr_wchar_t));
        }
    }
    else
    {
        if (XPR_IS_NULL(mTextA))
            return XPR_FALSE;

        sLen = 0;
        sTextLen = strlen(mTextA);
        sBufferA  = (xpr_char_t *)mBuffer;
        sBufferA2 = mTextA;

        while (true)
        {
            if (feof(sFile))
                break;

            sLen = fread(sLen == 0 ? mBuffer : mBuffer+sTextLen, 1,
                MAX_SEARCH_TEXT_BUF_SIZE-sTextLen-1, sFile) + ((sLen == 0) ? 0 : sTextLen);

            for (i = 0; i < sLen; ++i)
            {
                if ('A' <= sBufferA[i] && sBufferA[i] <= 'Z')
                    sBufferA[i] += 32;

                if (sBufferA[i] == sBufferA2[0])
                {
                    for (j = 1; j < sTextLen; ++j)
                    {
                        if ('A' <= sBufferA[i+j] && sBufferA[i+j] <= 'Z')
                            sBufferA[i+j] += 32;

                        if (sBufferA[i+j] != sBufferA2[j])
                            break;
                    }

                    if (j == sTextLen)
                    {
                        sMatched = XPR_TRUE;
                        break;
                    }
                }
            }

            if (XPR_IS_TRUE(sMatched))
                break;

            memmove(sBufferA, sBufferA+sLen-sTextLen, sTextLen);
        }
    }

    fclose(sFile);
    sFile = XPR_NULL;

    return sMatched;
}

xpr_bool_t SearchFile::isSearchText(void) const
{
    if (XPR_IS_NULL(mTextA) || XPR_IS_NULL(mTextW))
        return XPR_FALSE;

    if (mTextA[0] == 0 || mTextW[0] == 0)
        return XPR_FALSE;

    return XPR_TRUE;
}

SearchFile::Status SearchFile::getStatus(xpr_size_t *aSearchedCount, clock_t *aSearchTime)
{
    CsLocker aLocker(mCs);

    if (XPR_IS_NOT_NULL(aSearchedCount)) *aSearchedCount = mSearchedCount;
    if (XPR_IS_NOT_NULL(aSearchTime))    *aSearchTime    = mSearchTime;

    return mStatus;
}

LPARAM SearchFile::getData(void)
{
    return mParam;
}

void SearchFile::setData(LPARAM lParam)
{
    mParam = lParam;
}

void SearchFile::setResultFunc(SearchResultFunc aSearchResultFunc)
{
    mSearchResultFunc = aSearchResultFunc;
}
} // namespace fxb
