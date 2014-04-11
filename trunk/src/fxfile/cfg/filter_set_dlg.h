//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILTER_SET_DLG_H__
#define __FXFILE_FILTER_SET_DLG_H__ 1
#pragma once

#include "../filter.h"
#include "gui/ColourPickerXP.h"
#include "gui/IconList.h"

namespace fxfile
{
namespace cfg
{
class FilterSetDlg : public CDialog
{
    typedef CDialog super;

public:
    FilterSetDlg(FilterItem *aFilterItem = XPR_NULL);
    virtual ~FilterSetDlg(void);

public:
    FilterItem *getFilter(void);
    void setFilter(FilterItem *aFilterItem);

    void addName(const xpr_tchar_t *aName);

protected:
    FilterItem mFilterItem;

    typedef std::set<xpr::string> NameSet;
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
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_FILTER_SET_DLG_H__
