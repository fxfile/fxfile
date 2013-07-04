//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_LAUNCHER_MANAGER_H__
#define __FXFILE_LAUNCHER_MANAGER_H__ 1
#pragma once

namespace fxfile
{
class LauncherManager
{
public:
    static void startupProcess(xpr_ushort_t aGlobalHotKey, xpr_bool_t aShowTray);
    static void shutdownProcess(xpr_ushort_t aGlobalHotKey, xpr_bool_t aShowTray);
    static void registerWinStartup(void);
    static void unregisterWinStartup(void);
};
} // namespace fxfile

#endif // __FXFILE_LAUNCHER_MANAGER_H__
