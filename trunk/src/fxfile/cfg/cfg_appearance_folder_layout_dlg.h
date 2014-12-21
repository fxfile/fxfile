//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_APPEARANCE_FOLDER_LAYOUT_DLG_H__
#define __FXFILE_CFG_APPEARANCE_FOLDER_LAYOUT_DLG_H__ 1
#pragma once

#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgAppearanceFolderLayoutDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceFolderLayoutDlg(void);

protected:
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

private:
    static xpr_sint_t getCurSelFromViewStyle(xpr_sint_t aViewStyle);
    static xpr_sint_t getViewStyleFromCurSel(xpr_sint_t aCurSel);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnClean(void);
    afx_msg void OnInit(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_APPEARANCE_FOLDER_LAYOUT_DLG_H__
