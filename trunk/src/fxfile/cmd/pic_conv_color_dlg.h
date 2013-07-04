//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PIC_CONV_COLOR_DLG_H__
#define __FXFILE_PIC_CONV_COLOR_DLG_H__ 1
#pragma once

#include "pic_conv_opt_dlg.h"
#include "gfl/libgfl.h"

namespace fxfile
{
namespace cmd
{
class PicConvColorDlg : public CDialog
{
    typedef CDialog super;

public:
    PicConvColorDlg(const xpr_tchar_t *aPath);

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
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PIC_CONV_COLOR_DLG_H__
