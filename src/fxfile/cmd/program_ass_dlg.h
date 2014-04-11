//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PROGRAM_ASS_DLG_H__
#define __FXFILE_PROGRAM_ASS_DLG_H__ 1
#pragma once

#include "program_ass.h"
#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class ProgramAssDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    ProgramAssDlg(void);
    virtual ~ProgramAssDlg(void);

public:
    void setPath(const xpr::string &aPath);
    void setList(const ProgramAssDeque &aList);
    ProgramAssItem *getSelProgramAssItem(void);

protected:
    void getContextMenu(xpr_bool_t aRightClick);

protected:
    xpr::string      mPath;
    ProgramAssDeque  mList;
    ProgramAssItem  *mProgramAssItem;

protected:
    CImageList  mImgList;
    DlgState   *mDlgState;
    CTreeCtrl   mTreeCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    virtual void OnOK(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnGetInfoTipTree(NMHDR *pNMHDR, LRESULT *pResult);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PROGRAM_ASS_DLG_H__
