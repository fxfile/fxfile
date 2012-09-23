//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SCH_LOC_EDIT_DLG_H__
#define __FX_SCH_LOC_EDIT_DLG_H__
#pragma once

#include "fxb/fxb_search.h"
#include "resource.h"

class SchLocEditDlg : public CDialog
{
    typedef CDialog super;

public:
    SchLocEditDlg(void);
    virtual ~SchLocEditDlg(void);

public:
    const xpr_tchar_t *getPath(void) const;
    xpr_bool_t isInclude(void) const;
    xpr_bool_t isSubFolder(void) const;

    void setPath(const xpr_tchar_t *aPath);
    void setPath(LPITEMIDLIST aFullPidl);
    void setInclude(xpr_bool_t aInclude);
    void setSubFolder(xpr_bool_t aSubFolder);
    void setPathSet(fxb::SchUserLocPathSet *aSchUserLocPathSet);

protected:
    std::tstring            mPath;
    xpr_bool_t              mInclude;
    xpr_bool_t              mSubFolder;
    fxb::SchUserLocPathSet *mSchUserLocPathSet;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnIncludeExclude(void);
    afx_msg void OnPathBrowse(void);
};

#endif // __FX_SCH_LOC_EDIT_DLG_H__
