//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_MAIN_COOL_BAR_H__
#define __FXFILE_MAIN_COOL_BAR_H__ 1
#pragma once

#include "gui/rebar/SizableReBar.h"
#include "main_toolBar.h"
#include "drive_toolBar.h"
#include "bookmark_toolBar.h"

namespace fxfile
{
class MainCoolBar : public CSizableReBar, protected CToolBarEx::Observer
{
public:
    MainCoolBar();
    ~MainCoolBar();

public:
    xpr_bool_t createBands(void);

    void setToolbarText(xpr_sint_t aText);
    xpr_sint_t getToolbarText(void) const;
    void setToobarIcon(xpr_sint_t aIcon);
    xpr_sint_t getToolbarIcon(void) const;

    void setBandVisible(xpr_uint_t aBandId, xpr_bool_t aVisible);
    xpr_bool_t isBandVislble(xpr_uint_t aBandId);

    xpr_bool_t isWrapable(xpr_uint_t aId);
    void setWrapable(xpr_uint_t aId, xpr_bool_t aWrapable);

    void saveStateFile(void);

public:
    MainToolBar     mMainToolBar;
    DriveToolBar    mDriveToolBar;
    BookmarkToolBar mBookmarkToolBar;

protected:
    // from CToolBarEx
    virtual void onUpdatedToolbarSize(CToolBarEx &theToolBar);

protected:
    void loadState(void);
    void saveState(void);

    void loadStateFile(void);

    xpr_bool_t loadStateFile(const xpr_tchar_t *aPath);
    void saveStateFile(const xpr_tchar_t *aPath);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSysColorChange(void);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnDestroy(void);
};
} // namespace fxfile

#endif // __FXFILE_MAIN_COOL_BAR_H__
