//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_OPTION_MANAGER_H__
#define __FXFILE_OPTION_MANAGER_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

namespace fxfile
{
class Option;
class ExplorerView;
class ExplorerCtrl;
class FolderCtrl;

class OptionManager : public fxfile::base::Singleton<OptionManager>
{
    friend class fxfile::base::Singleton<OptionManager>;

protected: OptionManager(void);
public:   ~OptionManager(void);

public:
    void initDefault(void);
    void initDefaultConfigOption(void);

    void load(xpr_bool_t &aInitCfg);
    xpr_bool_t save(void);

    xpr_bool_t loadMainOption(void);
    xpr_bool_t saveMainOption(void);

    xpr_bool_t loadConfigOption(void);
    xpr_bool_t saveConfigOption(void);

    Option *getOption(void) const { return mOption; }

protected:
    void loadFilter(fxfile::base::ConfFileEx &aConfFile);
    void saveFilter(fxfile::base::ConfFileEx &aConfFile);

    void loadProgramAss(fxfile::base::ConfFileEx &aConfFile);
    void saveProgramAss(fxfile::base::ConfFileEx &aConfFile);

    void loadSizeFormat(fxfile::base::ConfFileEx &aConfFile);
    void saveSizeFormat(fxfile::base::ConfFileEx &aConfFile);

    void loadRecentFileList(fxfile::base::ConfFileEx &aConfFile);
    void saveRecentFileList(fxfile::base::ConfFileEx &aConfFile);

private:
    Option *mOption;
};
} // namespace fxfile

#endif // __FXFILE_OPTION_MANAGER_H__
