//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_H__
#define __FXFILE_SEARCH_H__ 1
#pragma once

namespace fxfile
{
#define MAX_SEARCH_LOC 100

typedef struct SearchUserLocItem
{
    xpr::tstring mPath;
    xpr_bool_t   mInclude;
    xpr_bool_t   mSubFolder;
} SearchUserLocItem;

typedef std::deque<SearchUserLocItem *> UserLocDeque;

typedef struct SearchUserLoc
{
    xpr::tstring mName;
    UserLocDeque mUserLocDeque;

    void clear(void)
    {
        SearchUserLocItem *sItem;
        UserLocDeque::iterator sIterator;

        sIterator = mUserLocDeque.begin();
        for (; sIterator != mUserLocDeque.end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_SAFE_DELETE(sItem);
        }

        mUserLocDeque.clear();
    }

} SearchUserLoc;

typedef std::deque<SearchUserLoc *> SearchUserLocDeque;

typedef std::set<xpr::tstring> SearchUserLocNameSet;
typedef std::set<xpr::tstring> SearchUserLocPathSet;
} // namespace fxfile

#endif // __FXFILE_SEARCH_H__
