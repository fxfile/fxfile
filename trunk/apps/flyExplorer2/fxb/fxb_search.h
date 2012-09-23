//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SEARCH_H__
#define __FXB_SEARCH_H__
#pragma once

namespace fxb
{
#define MAX_SCH_LOC 100

typedef struct SchUserLocItem
{
    std::tstring mPath;
    xpr_bool_t   mInclude;
    xpr_bool_t   mSubFolder;
} SchUserLocItem;

typedef std::deque<SchUserLocItem *> UserLocDeque;

typedef struct SchUserLoc
{
    std::tstring mName;
    UserLocDeque mUserLocDeque;

    void clear(void)
    {
        SchUserLocItem *sItem;
        UserLocDeque::iterator sIterator;

        sIterator = mUserLocDeque.begin();
        for (; sIterator != mUserLocDeque.end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_SAFE_DELETE(sItem);
        }

        mUserLocDeque.clear();
    }

} SchUserLoc;

typedef std::deque<SchUserLoc *> SchUserLocDeque;

typedef std::set<std::tstring> SchUserLocNameSet;
typedef std::set<std::tstring> SchUserLocPathSet;
} // namespace fxb

#endif // __FXB_SEARCH_H__
