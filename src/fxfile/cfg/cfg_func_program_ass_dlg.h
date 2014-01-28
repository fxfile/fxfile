//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_FUNC_PROGRAM_ASS_H__
#define __FXFILE_CFG_FUNC_PROGRAM_ASS_H__ 1
#pragma once

#include "../program_ass.h"

#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgFuncProgramAssDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgFuncProgramAssDlg(void);

protected:
    xpr_sint_t insertProgramAssItem(xpr_sint_t aIndex, ProgramAssItem *aProgramAssItem);
    xpr_sint_t addProgramAssItem(ProgramAssItem *aProgramAssItem);
    void selectItem(xpr_sint_t aIndex);

protected:
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    CListCtrl mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnItemAdd(void);
    afx_msg void OnItemModify(void);
    afx_msg void OnItemDelete(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDefault(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_FUNC_PROGRAM_ASS_H__
