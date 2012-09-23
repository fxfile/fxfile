//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SEARCH_FILE_H__
#define __FXB_SEARCH_FILE_H__
#pragma once

namespace fxb
{
typedef void (WINAPI *SearchResultFunc)(const xpr_tchar_t *aFolder, WIN32_FIND_DATA *aWin32FindData, LPARAM aParam);

class SearchFile : public fxb::Thread
{
public:
    enum
    {
        FlagNone       = 0x00000000,
        FlagSubFolder  = 0x00000001,
        FlagCase       = 0x00000002,
        FlagSystem     = 0x00000004,
        FlagTypeFile   = 0x00010000,
        FlagTypeFolder = 0x00020000,
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

    xpr_bool_t isFlag(xpr_uint_t aFlag);
    xpr_uint_t getFlags(void);
    void setFlags(xpr_uint_t aFlags);

    void setName(const xpr_tchar_t *aName, xpr_bool_t aNoWildcard);
    void setText(const xpr_tchar_t *aText);
    void addIncludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder);
    void addExcludeDir(const xpr_tchar_t *aDir, xpr_bool_t aSubFolder);

    LPARAM getData(void);
    void setData(LPARAM aParam);
    void setResultFunc(SearchResultFunc aSearchResultFunc);

    Status getStatus(xpr_size_t *aSearchedCount = XPR_NULL, clock_t *aSearchTime = XPR_NULL);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

    void searchRecursive(xpr_sint_t aDepth, const xpr_tchar_t *aFolder, xpr_bool_t aSubFolder);
    xpr_bool_t searchText(const xpr_tchar_t *aFolder, const xpr_tchar_t *aFileName);

    inline xpr_bool_t isSearchText(void) const;

protected:
    HWND         mHwnd;
    xpr_uint_t   mMsg;

    xpr_uint_t   mFlags;
    xpr_char_t  *mTextA; // for ANSICODE text search
    xpr_wchar_t *mTextW; // for UNICODE text search

    typedef struct SchDir
    {
        std::tstring mPath;
        xpr_bool_t   mSubFolder;
    } SchDir;

    typedef std::deque<std::tstring> NameDeque;
    typedef std::deque<SchDir *> SchDirDeque;
    NameDeque   mNameDeque;
    SchDirDeque mSchIncDirDeque;
    SchDirDeque mSchExcDirDeque;

    // search cache data
    xpr_byte_t      *mBuffer; // buffer for text search
    WIN32_FIND_DATA  mWin32FindData;

    xpr_bool_t mSubFolder;
    xpr_bool_t mCase;
    xpr_sint_t mType;
    xpr_bool_t mSystem;
    xpr_sint_t mMatchFlags;

    clock_t    mSearchTime;
    xpr_size_t mSearchedCount;
    Status     mStatus;
    Cs         mCs;

    LPARAM           mParam;
    SearchResultFunc mSearchResultFunc;
};
} // namespace fxb

#endif // __FXB_SEARCH_FILE_H__
