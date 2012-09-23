//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_PIC_CNV_CLR_DLG_H__
#define __FX_PIC_CNV_CLR_DLG_H__
#pragma once

#include "PicCnvOptDlg.h"
#include "gfl/libgfl.h"

class PicCnvClrDlg : public CDialog
{
    typedef CDialog super;

public:
    PicCnvClrDlg(const xpr_tchar_t *aPath);

public:
    xpr_bool_t isAllApply(void) { return mAllApply; }
    void getColorMode(GFL_MODE &aGflMode, GFL_MODE_PARAMS &aGflModeParams);
    void preOK(void);
    void enableWindow(xpr_sint_t aIndex);

    static xpr_sint_t mIndex;
    static xpr_sint_t mCurSel;
    static void initStaticVar(void);

protected:
    xpr_bool_t      mAllApply;
    GFL_MODE        mGflMode;
    GFL_MODE_PARAMS mGflModeParams;

    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnApplyAll(void);
    virtual void OnOK(void);
    afx_msg void OnRadioBinary(void);
    afx_msg void OnRadioGrayscale(void);
    afx_msg void OnRadioColor(void);
    afx_msg void OnRadioTruecolor(void);
};

#endif // __FX_PIC_CNV_CLR_DLG_H__
