//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DLG_TOOL_BAR_H__
#define __FXFILE_DLG_TOOL_BAR_H__
#pragma once

class DlgToolBar : public CToolBar
{
    typedef CToolBar super;

public:
    DlgToolBar(void);
    virtual ~DlgToolBar(void);

public:
    void setImageListId(xpr_uint_t aId, CSize aBitmapSize);          // 1
    void enableToolTips(xpr_bool_t aEnable);                         // 2

    xpr_bool_t createToolBar(CDialog *aDialog);                      // 3
    void setToolBarButtons(TBBUTTON *aTbButtons, xpr_sint_t aCount); // 4
    void reposition(CDialog *aDialog);                               // 5

protected:
    void enableButton(xpr_sint_t aId, xpr_bool_t aEnable);

protected:
    CImageList mImageList;
    xpr_uint_t mImageListId;
    CSize      mBitmapSize;
    xpr_bool_t mEnableToolTips;

protected:
    typedef CMap<xpr_sint_t, xpr_sint_t &, xpr_bool_t, xpr_bool_t &> ButtonStateMap;
    ButtonStateMap mButtonStateMap;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDestroy(void);
    afx_msg void OnTimer(UINT_PTR aIdEvent);
    afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
};

#endif // __FXFILE_DLG_TOOL_BAR_H__
