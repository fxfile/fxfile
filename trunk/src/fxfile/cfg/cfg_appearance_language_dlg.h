//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_APPEARANCE_LANGUAGE_DLG_H__
#define __FXFILE_CFG_APPEARANCE_LANGUAGE_DLG_H__ 1
#pragma once

#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgAppearanceLanguageDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceLanguageDlg(void);

protected:
    // from CfgDlg
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    void chooseLanguagePack(xpr_sint_t aIndex);
    void chooseLanguagePack(const xpr_tchar_t *aLanguage);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelChangeLanguagePack(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_APPEARANCE_LANGUAGE_DLG_H__
