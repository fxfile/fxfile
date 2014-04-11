//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PIC_CONV_DLG_H__
#define __FXFILE_PIC_CONV_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"
#include "gfl/libgfl.h"

#ifdef _DEBUG
#include "gfl/libgfle.h"
#endif

namespace fxfile
{
class DlgState;

namespace cmd
{
class PicConv;

class PicConvDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    PicConvDlg(void);
    virtual ~PicConvDlg(void);

public:
    xpr_bool_t addPath(const xpr::string &aPath);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void setStatus(const xpr_tchar_t *aStatusText);

protected:
    PicConv *mPicConv;

    struct PicItem
    {
        xpr::string mPath;
        xpr_sint_t  mFormatIndex;
    };

    typedef std::deque<PicItem *> PicDeque;
    PicDeque mPicDeque;

    GFL_SAVE_PARAMS *mGflSaveParams;
    xpr_bool_t       mLossless;
    xpr_sint_t       mPaletteTransparent;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CListCtrl      mListCtrl;
    CComboBoxEx    mFormatComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnSelchangeFormat(void);
    afx_msg void OnOption(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    virtual void OnOK(void);
    afx_msg void OnGetdispinfoPcList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPicColor(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_PIC_CONV_DLG_H__
