//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_FILE_H__
#define __FXFILE_SEARCH_FILE_H__ 1
#pragma once

#include "xpr_mutex.h"
#include "xpr_reg_expr.h"
#include "thread.h"

namespace fxfile
{
typedef void (WINAPI *SearchResultFunc)(const xpr_tchar_t *aFolder, WIN32_FIND_DATA *aWin32FindData, LPARAM aParam);

class SearchFile : protected xpr::Thread::Runnable
{
public:
    enum
    {
        FlagNone       = 0x00000000,
        FlagSubFolder  = 0x00000001,
        FlagCase       = 0x00000002,
        FlagRegExpr    = 0x00000004,
        FlagSystem     = 0x00000008,
        FlagNoWildcard = 0x00000010,
        FlagTypeFile   = 0x00010000,
        FlagTypeFolder = 0x00020000,
    };

    struct SearchParam
    {
        xpr_uint_t   mFlags;
        xpr_tchar_t *mName;
        xpr_tchar_t *mText;
    };

    enum Status
    {
        StatusNone,
        StatusFailed,
        StatusSearching,
        StatusSearchCompleted,
        StatusStopped,
    };

public:
    SearchFile(void);
    virtual ~SearchFile(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);
    void setSearchParam(const SearchParam &aParam);
    void setResultFunc(SearchResultFunc aSearchResultFunc, LPARAM aCallbackParam);
    void addIncludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder);
    void addExcludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder);

    xpr_bool_t start();
    void stop();

    Status getStatus(xpr_size_t *aSearchedCount = XPR_NULL, clock_t *aSearchTime = XPR_NULL);

private:
    // from xpr::Thread::Runnable
    xpr_sint_t runThread(xpr::Thread &aThread);

    void searchRecursive(xpr_sint_t aDepth, const xpr_tchar_t *aFolder, xpr_bool_t aSubFolder);
    xpr_bool_t searchText(const xpr_tchar_t *aFolder, const xpr_tchar_t *aFileName);

    xpr_bool_t isFlag(xpr_uint_t aFlag) const { return XPR_TEST_BITS(mFlags, aFlag); }
    xpr_uint_t getFlags(void) const { return mFlags; }

    inline xpr_bool_t isSearchText(void) const;

private:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    xpr_uint_t   mFlags;
    xpr_char_t  *mTextA; // for MULTIBYTE text search
    xpr_wchar_t *mTextW; // for UNICODE text search

    typedef struct SearchDir
    {
        xpr::string mPath;
        xpr_bool_t  mSubFolder;
    } SearchDir;

    typedef std::deque<xpr::string> NameDeque;
    typedef std::deque<SearchDir *> SearchDirDeque;
    NameDeque      mNameDeque;
    std::tr1::tregex mRegExpr;
    SearchDirDeque mSearchIncDirDeque;
    SearchDirDeque mSearchExcDirDeque;

    // search cache data
    xpr_byte_t      *mBuffer; // buffer for text search
    WIN32_FIND_DATA  mWin32FindData;

    xpr_bool_t mSubFolder;
    xpr_bool_t mCase;
    xpr_sint_t mType;
    xpr_bool_t mSystem;
    xpr_sint_t mMatchFlags;

    Thread     mThread;
    clock_t    mSearchTime;
    xpr_size_t mSearchedCount;
    Status     mStatus;
    xpr::Mutex mMutex;

    LPARAM           mCallbackParam;
    SearchResultFunc mSearchResultFunc;
};
} // namespace fxfile

#endif // __FXFILE_SEARCH_FILE_H__
