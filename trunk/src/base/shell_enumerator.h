//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_SHELL_ENUMERATOR_H__
#define __FXFILE_BASE_SHELL_ENUMERATOR_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
class ShellItem;

class ShellEnumerator
{
public:
    ShellEnumerator(ShellItem &aShellFolderItem);
    virtual ~ShellEnumerator(void);

public:
    xpr_bool_t enumerate(void);
    xpr_bool_t next(ShellItem *aShellItem);

protected:
    ShellItem &mShellFolderItem;
    void      *mEnumerator;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_SHELL_ENUMERATOR_H__
