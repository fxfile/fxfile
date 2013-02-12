//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
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
    xpr_bool_t save(xpr_bool_t aOnlyConfig = XPR_FALSE);

    xpr_bool_t loadMainOption(void);
    xpr_bool_t saveMainOption(void);

    xpr_bool_t loadConfigOption(void);
    xpr_bool_t saveConfigOption(void);

    xpr_bool_t loadFilter(void);
    xpr_bool_t saveFilter(void);

    xpr_bool_t loadProgramAss(void);
    xpr_bool_t saveProgramAss(void);

    xpr_bool_t loadSizeFormat(void);
    xpr_bool_t saveSizeFormat(void);

    Option *getOption(void) { return mOption; }

private:
    Option *mOption;
};

#endif // __FX_OPTION_MGR_H__
