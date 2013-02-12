//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_DLG_H__
#define __FX_CFG_DLG_H__
#pragma once

#include "../Option.h"

class CfgDlgObserver;

class CfgDlg : public CDialog
{
    typedef CDialog super;

public:
    CfgDlg(xpr_uint_t aResourceId = 0, CWnd *sParentWnd = XPR_NULL);

public:
    CfgDlgObserver *getObserver(void) const;
    void setObserver(CfgDlgObserver *aObserver);

public:
    virtual xpr_bool_t Create(xpr_size_t aCfgIndex, CWnd *aParentWnd);

public:
    xpr_size_t getCfgIndex(void) const;
    xpr_bool_t isModified(void);
    void setModified(xpr_bool_t aModified = XPR_TRUE);

    virtual void onInit(Option::Config &aConfig) = 0;
    virtual void onApply(Option::Config &aConfig) = 0;

protected:
    void addIgnoreApply(xpr_uint_t aId);

protected:
    CfgDlgObserver *mObserver;

    xpr_uint_t mResourceId;
    xpr_size_t mCfgIndex;

    typedef std::set<xpr_uint_t> IgnoreApplySet;
    IgnoreApplySet mIgnoreApplySet;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnMsgApply(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_CFG_DLG_H__
