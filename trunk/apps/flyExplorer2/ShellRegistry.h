//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SHELL_REGISTRY_H__
#define __FX_SHELL_REGISTRY_H__
#pragma once

class ShellRegistry
{
public:
    static void registerShell(void);
    static void unregisterShell(void);
};

#endif // __FX_SHELL_REGISTRY_H__
