//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_MAIN_DLG_H__
#define __FXFILE_CFG_MAIN_DLG_H__ 1
#pragma once

#include "gui/TreeCtrlEx.h"

#include "../option.h"
#include "cfg_dlg_observer.h"

namespace fxfile
{
namespace cfg
{
class CfgDlg;

class CfgMainDlg : public CDialog, public CfgDlgObserver
{
    typedef CDialog super;

public:
    CfgMainDlg(xpr_sint_t aInitShowCfg = -1);

protected:
    xpr_bool_t showCfg(xpr_sint_t aCfgIndex);
    void showCfgByTree(xpr_sint_t aCfgIndex);
    xpr_sint_t addCfgItem(xpr_sint_t aImage, xpr_sint_t aParent, CfgDlg *aCfgDlg, const xpr_tchar_t *aText);

protected:
    typedef struct CfgItem
    {
        HTREEITEM     mTreeItem;
        xpr::string   mText;
        xpr_sint_t    mImage;
        xpr_sint_t    mParent;

        CfgDlg       *mCfgDlg;
        xpr_bool_t    mModified;
    } CfgItem;

    typedef std::deque<CfgItem *> CfgDeque;
    CfgDeque   mCfgDeque;
    xpr_sint_t mInitShowCfg;
    xpr_sint_t mShowCfg;
    static xpr_sint_t mLastShowCfg;

    xpr_bool_t mChanging;

    Option::Config mNewConfig;

    CImageList mImgList;

protected:
    TreeCtrlEx mTreeCtrl;
    CBrush     mDarkGrayBrush;

protected:
    // from CfgDlgObserver
    virtual xpr_bool_t onIsModified(CfgDlg &aCfgDlg);
    virtual void onSetModified(CfgDlg &aCfgDlg, xpr_bool_t aModified);
    void onSetModified(xpr_size_t aCfgIndex, xpr_bool_t aModified);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDestroy(void);
    virtual void OnOK(void);
    virtual void OnCancel(void);
    afx_msg void OnApply(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDefault(void);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_MAIN_DLG_H__
