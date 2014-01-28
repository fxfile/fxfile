//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_APPEARANCE_COLOR_DLG_H__
#define __FXFILE_CFG_APPEARANCE_COLOR_DLG_H__ 1
#pragma once

#include "gui/ColourPickerXP.h"
#include "../option.h"
#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgAppearanceColorDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceColorDlg(void);

protected:
    void showTabDlg(xpr_sint_t aViewIndex);

protected:
    // from CfgDlg
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

private:
    struct ViewColor
    {
        xpr_sint_t   mExplorerTextColorType;
        xpr_uint_t   mExplorerTextColor;
        xpr_sint_t   mExplorerBkgndColorType;
        xpr_uint_t   mExplorerBkgndColor;
        xpr_sint_t   mFolderTreeTextColorType;
        xpr_uint_t   mFolderTreeTextColor;
        xpr_sint_t   mFolderTreeBkgndColorType;
        xpr_uint_t   mFolderTreeBkgndColor;
        xpr_sint_t   mExplorerBkgndImage;
        xpr::tstring mExplorerBkgndImagePath;
        xpr_sint_t   mFolderTreeHighlightColor;
        xpr_sint_t   mFolderTreeHighlight;
    };

    void loadViewColor(const ViewColor &aViewColor);
    void saveViewColor(ViewColor &aViewColor);
    void saveViewColor(void);

private:
    xpr_sint_t      mOldViewIndex;
    CColourPickerXP mFileListTextCustomColorCtrl;
    CColourPickerXP mFileListBkgndCustomColorCtrl;
    CColourPickerXP mFolderTreeTextCustomColorCtrl;
    CColourPickerXP mFolderTreeBkgndCustomColorCtrl;
    CColourPickerXP mFolderTreeInactiveHighlightColorCtrl;
    CColourPickerXP mActivedViewColorCtrl;
    CColourPickerXP mInfoBarBookmarkColorCtrl;
    CColourPickerXP mPathBarHighlightColorCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnApplyAll(void);
    afx_msg void OnSelChangeView(void);
    afx_msg void OnFileListBkgndImageBrowse(void);
    afx_msg LRESULT OnSelEndOK(WPARAM aWParam, LPARAM aLParam);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_APPEARANCE_COLOR_DLG_H__
