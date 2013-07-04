//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_DLG_OBSERVER_H__
#define __FXFILE_SEARCH_DLG_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class SearchDlg;

class SearchDlgObserver
{
public:
    virtual void onPostNcDestroy(SearchDlg &aSearchDlg) = 0;
};
} // namespace fxfile

#endif // __FXFILE_SEARCH_DLG_OBSERVER_H__
