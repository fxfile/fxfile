//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEARCH_RESULT_CTRL_OBSERVER_H__
#define __FX_SEARCH_RESULT_CTRL_OBSERVER_H__
#pragma once

class SearchResultCtrl;

class SearchResultCtrlObserver
{
public:
    virtual void onSetFocus(SearchResultCtrl &aSearchResultCtrl) = 0;
};

#endif // __FX_SEARCH_RESULT_CTRL_OBSERVER_H__
