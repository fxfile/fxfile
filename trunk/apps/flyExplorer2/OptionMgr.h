//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_OPTION_MGR_H__
#define __FX_OPTION_MGR_H__
#pragma once

#include "xpr_pattern.h"

class Option;
class ExplorerView;
class ExplorerCtrl;
class FolderCtrl;

class OptionMgr : public xpr::Singleton<OptionMgr>
{
    friend class xpr::Singleton<OptionMgr>;

protected: OptionMgr(void);
public:   ~OptionMgr(void);

public:
    void initDefault(void);

    void load(xpr_bool_t &aInitCfg);
    xpr_bool_t save(void);

    xpr_bool_t loadOption(void);
    xpr_bool_t saveOption(void);
    xpr_bool_t saveMainOption(void);

    xpr_bool_t loadFilter(void);
    xpr_bool_t saveFilter(void);

    xpr_bool_t loadFileAss(void);
    xpr_bool_t saveFileAss(void);

    xpr_bool_t loadSizeFormat(void);
    xpr_bool_t saveSizeFormat(void);

    void setModifiedHidden(xpr_bool_t aModifiedHidden) { mModifiedHidden = aModifiedHidden; }
    void setModifiedSystem(xpr_bool_t aModifiedSystem) { mModifiedSystem = aModifiedSystem; }
    xpr_bool_t isModifiedHidden(void) { return mModifiedHidden; }
    xpr_bool_t isModifiedSystem(void) { return mModifiedSystem; }

    // Apply to FolderCtrl
    void applyFolderCtrl(xpr_bool_t aLoading);
    void applyFolderCtrl(xpr_sint_t aIndex, xpr_bool_t aLoading);
    void applyFolderCtrl(FolderCtrl *aFolderCtrl, xpr_sint_t aIndex, xpr_bool_t aLoading);
    void applyFolderCtrl(FolderCtrl *aFolderCtrl, xpr_bool_t aLoading);

    // Apply to ExplorerView
    void setOnlyExplorerViewApply(xpr_bool_t aOnlyExplorerView) { mOnlyExplorerView = aOnlyExplorerView; }
    void applyExplorerView(xpr_bool_t aLoading);
    void applyExplorerView(xpr_sint_t aIndex, xpr_bool_t aLoading);
    void applyExplorerView(ExplorerView *aExplorerView, xpr_sint_t aIndex, xpr_bool_t aLoading);
    void applyExplorerView(ExplorerView *aExplorerView, xpr_bool_t aLoading);
    void applyExplorerCtrl(ExplorerCtrl *aExplorerCtrl, xpr_bool_t aLoading);
    void applyExplorerCtrl(ExplorerCtrl *aExplorerCtrl, xpr_sint_t aIndex, xpr_bool_t aLoading);

    // Apply to Etc Control
    void applyEtc(void);
    void applyEtc(xpr_sint_t aIndex);

    Option *getOption(void) { return mOption; }

    xpr_bool_t FontDlgToString(CFontDialog &aDlg, xpr_tchar_t *aFontText);
    xpr_bool_t StringToLogFont(const xpr_tchar_t *aFontText, LOGFONT &aLogFont);

private:
    Option *mOption;

    xpr_bool_t mModifiedHidden;
    xpr_bool_t mModifiedSystem;

    xpr_bool_t mOnlyExplorerView;
};

#endif // __FX_OPTION_MGR_H__
