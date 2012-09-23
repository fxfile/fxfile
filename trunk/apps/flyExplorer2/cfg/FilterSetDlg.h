//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILTER_SET_DLG_H__
#define __FX_FILTER_SET_DLG_H__
#pragma once

#include "fxb/fxb_filter.h"
#include "rgc/ColourPickerXP.h"
#include "rgc/IconList.h"

class FilterSetDlg : public CDialog
{
    typedef CDialog super;

public:
    FilterSetDlg(fxb::FilterItem *aFilterItem = XPR_NULL);
    virtual ~FilterSetDlg(void);

public:
    fxb::FilterItem *getFilter(void);
    void setFilter(fxb::FilterItem *aFilterItem);

    void addName(const xpr_tchar_t *aName);

protected:
    fxb::FilterItem mFilterItem;

    typedef std::set<std::tstring> NameSet;
    NameSet mNameSet;

protected:
    CColourPickerXP mColorCtrl;
    IconList        mIconListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDestroy();
    afx_msg void OnApplyColorCode(void);
};

#endif // __FX_FILTER_SET_DLG_H__
