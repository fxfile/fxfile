//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEARCH_BAR_H__
#define __FX_SEARCH_BAR_H__
#pragma once

#include "rgc/scbarcf.h"

#include "SearchDlg.h"

class SearchResultCtrl;

class SearchBar : public CSizingControlBarCF
{
    typedef CSizingControlBarCF super;

    DECLARE_DYNAMIC(SearchBar);

public:
    SearchBar(void);
    virtual ~SearchBar(void);

public:
    void createChild(void);
    void destroyChild(void);

    SearchDlg *getSearchDlg(void) const;
    SearchResultCtrl *getSearchResultCtrl(void) const;

protected:
    SearchDlg *mSearchDlg;

public:
    virtual xpr_bool_t DestroyWindow(void);
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_SEARCH_BAR_H__
