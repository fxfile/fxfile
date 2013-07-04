//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SHELL_REGISTRY_H__
#define __FXFILE_SHELL_REGISTRY_H__ 1
#pragma once

namespace fxfile
{
class ShellRegistry
{
public:
    static void registerShell(void);
    static void unregisterShell(void);
};
} // namespace fxfile

#endif // __FXFILE_SHELL_REGISTRY_H__
