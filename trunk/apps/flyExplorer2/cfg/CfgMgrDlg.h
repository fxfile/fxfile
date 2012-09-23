//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_MGR_DLG_H__
#define __FX_CFG_MGR_DLG_H__
#pragma once

class CfgDlg;

class CfgMgrDlg : public CDialog
{
    typedef CDialog super;

public:
    CfgMgrDlg(xpr_sint_t aInitShowCfg = -1);

protected:
    xpr_bool_t showCfg(xpr_sint_t aIndex);
    void showCfgByTree(xpr_sint_t aIndex);
    xpr_sint_t addCfgItem(xpr_sint_t aImage, xpr_sint_t aParent, CfgDlg *aCfgDlg, const xpr_tchar_t *aText);

protected:
    typedef struct CfgItem
    {
        HTREEITEM     mTreeItem;
        std::tstring  mText;
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

    CImageList mImgList;

protected:
    CTreeCtrl mTreeCtrl;
    CBrush    mDarkGrayBrush;

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
    afx_msg LRESULT OnSetModified(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetModified(WPARAM wParam, LPARAM lParam);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor);
};

#endif // __FX_CFG_MGR_DLG_H__
