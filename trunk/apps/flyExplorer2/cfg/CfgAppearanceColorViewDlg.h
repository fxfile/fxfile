//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_APPEARANCE_COLOR_VIEW_DLG_H__
#define __FX_CFG_APPEARANCE_COLOR_VIEW_DLG_H__
#pragma once

#include "../rgc/ColourPickerXP.h"
#include "CfgDlg.h"

class CfgAppearanceColorViewDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceColorViewDlg(xpr_sint_t aViewIndex);

public:
    // from CfgDlg
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    xpr_sint_t mViewIndex;
    CColourPickerXP mFileListTextCustomColorCtrl;
    CColourPickerXP mFileListBkgndCustomColorCtrl;
    CColourPickerXP mFolderTreeTextCustomColorCtrl;
    CColourPickerXP mFolderTreeBkgndCustomColorCtrl;
    CColourPickerXP mFolderTreeInactiveHighlightColorCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg LRESULT OnSelEndOK(WPARAM aWParam, LPARAM aLParam);
    afx_msg void OnFileListBkgndImageBrowse(void);
};

#endif // __FX_CFG_APPEARANCE_COLOR_VIEW_DLG_H__
