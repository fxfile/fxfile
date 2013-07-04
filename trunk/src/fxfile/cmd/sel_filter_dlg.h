//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEL_FILTER_DLG_H__
#define __FXFILE_SEL_FILTER_DLG_H__ 1
#pragma once

#include "filter.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class SelFilterDlg : public CDialog
{
    typedef CDialog super;

public:
    SelFilterDlg(xpr_bool_t aSelect);

public:
    FilterItem *getSelFilterItem(void) { return mFilterItem; }
    xpr_bool_t isOnlySel(void) { return mOnlySel; }

protected:
    xpr_bool_t  mSelect;
    xpr_bool_t  mOnlySel;
    FilterItem *mFilterItem;

protected:
    DlgState  *mDlgState;
    CComboBox  mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelchangeFilter(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_SEL_FILTER_DLG_H__
