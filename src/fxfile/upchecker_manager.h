//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_MANAGER_H__
#define __FXFILE_UPCHECKER_MANAGER_H__ 1
#pragma once

#include "option.h"
#include "update_info_manager.h"

namespace fxfile
{
class UpcheckerManager
{
public:
    static void writeConfFile(Option::Config &aConfig);
    static void startupProcess(void);
    static void shutdownProcess(void);
    static void checkNow(void);
    static void registerWinStartup(void);
    static void unregisterWinStartup(void);
    static xpr_bool_t getUpdateInfo(fxfile::base::UpdateInfo &aUpdateInfo);
};
} // namespace fxfile

#endif // __FXFILE_UPCHECKER_MANAGER_H__
