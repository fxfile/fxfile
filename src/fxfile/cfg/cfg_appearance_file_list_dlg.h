//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_APPEARANCE_FILE_LIST_DLG_H__
#define __FXFILE_CFG_APPEARANCE_FILE_LIST_DLG_H__ 1
#pragma once

#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgAppearanceFileListDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceFileListDlg(void);

protected:
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    xpr_bool_t loadImage(xpr_sint_t aWidth, xpr_sint_t aHeight, xpr::string &aPath, xpr_bool_t &aSatistifed);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnCustomIcon16Browse(void);
    afx_msg void OnCustomIcon32Browse(void);
    afx_msg void OnCustomIcon48Browse(void);
    afx_msg void OnCustomIcon256Browse(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_APPEARANCE_FILE_LIST_DLG_H__
