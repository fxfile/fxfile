//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BOOKMARK_TOOL_BAR_OBSERVER_H__
#define __FXFILE_BOOKMARK_TOOL_BAR_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class BookmarkToolBar;

class BookmarkToolBarObserver
{
public:
    virtual void onBookmarkToolBarStatus(BookmarkToolBar &aBookmarkToolBar, xpr_sint_t aBookmarkIndex, xpr_sint_t aInsert, DROPEFFECT aDropEffect) = 0;
};
} // namespace fxfile

#endif // __FXFILE_BOOKMARK_TOOL_BAR_OBSERVER_H__
