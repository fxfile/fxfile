//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEARCH_RESULT_PANE_OBSERVER_H__
#define __FX_SEARCH_RESULT_PANE_OBSERVER_H__
#pragma once

class SearchResultPane;

class SearchResultPaneObserver
{
public:
    virtual void onSetFocus(SearchResultPane &aSearchResultPane) = 0;
};

#endif // __FX_SEARCH_RESULT_PANE_OBSERVER_H__
