//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILTER_H__
#define __FXFILE_FILTER_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

namespace fxfile
{
#define MAX_FILTER      20

#define MAX_FILTER_NAME 56
#define MAX_FILTER_EXTS 260

class FilterItem
{
public:
    FilterItem(void)
        : mColor(RGB(0,0,0))
        , mIconIndex(-1)
    {
    }

    void clear()
    {
        mName.clear();
        mExts.clear();
        mColor = RGB(0,0,0);
        mIconIndex = -1;
    }

    FilterItem& operator=(const FilterItem &aSrc)
    {
        mName      = aSrc.mName;
        mExts      = aSrc.mExts;
        mColor     = aSrc.mColor;
        mIconIndex = aSrc.mIconIndex;
        return *this;
    }

public:
    xpr::tstring mName;
    xpr::tstring mExts;
    COLORREF     mColor;
    xpr_sint_t   mIconIndex;
};

class Filter
{
public:
    Filter(void);
    virtual ~Filter(void);

public:
    void addFilter(FilterItem *aFilterItem);
    xpr_sint_t getCount(void);
    FilterItem *getFilter(xpr_sint_t aIndex = -1);
    FilterItem *getFilterFromName(const xpr_tchar_t *aName);
    FilterItem *getFilterFromName(const xpr::tstring &aName);

    xpr_sint_t getIndex(const xpr_tchar_t *aPath, xpr_bool_t aFolder = XPR_FALSE);
    COLORREF   getColor(const xpr_tchar_t *aPath, xpr_bool_t aFolder = XPR_FALSE);
    xpr_sint_t getIconIndex(const xpr_tchar_t *aPath, xpr_bool_t aFolder = XPR_FALSE);

    xpr_bool_t initDefault(void);
    void clear(void);

    xpr_bool_t load(fxfile::base::ConfFileEx &aConfFile);
    void save(fxfile::base::ConfFileEx &aConfFile) const;

    static void setString(
        const xpr_tchar_t *aFolderString,
        const xpr_tchar_t *aGeneralFileString,
        const xpr_tchar_t *aExecutableFileString,
        const xpr_tchar_t *aCompressedFileString,
        const xpr_tchar_t *aDocumentFileString,
        const xpr_tchar_t *aImageFileString,
        const xpr_tchar_t *aSoundFileString,
        const xpr_tchar_t *aMovieFileString,
        const xpr_tchar_t *aWebFileString,
        const xpr_tchar_t *aProgrammingFileString,
        const xpr_tchar_t *aTemporaryFileString);

protected:
    typedef std::deque<FilterItem *> FilterDeque;
    FilterDeque mFilterDeque;

    static const xpr_tchar_t *mFolderString;
    static const xpr_tchar_t *mGeneralFileString;
    static const xpr_tchar_t *mExecutableFileString;
    static const xpr_tchar_t *mCompressedFileString;
    static const xpr_tchar_t *mDocumentFileString;
    static const xpr_tchar_t *mImageFileString;
    static const xpr_tchar_t *mSoundFileString;
    static const xpr_tchar_t *mMovieFileString;
    static const xpr_tchar_t *mWebFileString;
    static const xpr_tchar_t *mProgrammingFileString;
    static const xpr_tchar_t *mTemporaryFileString;
};

class FilterMgr : public Filter, public fxfile::base::Singleton<FilterMgr>
{
    friend class fxfile::base::Singleton<FilterMgr>;

protected: FilterMgr(void);
public:   ~FilterMgr(void);
};
} // namespace fxfile

#endif // __FXFILE_FILTER_H__
