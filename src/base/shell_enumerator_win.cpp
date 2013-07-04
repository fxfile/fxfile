//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "shell_enumerator.h"
#include "shell_item.h"
#include "pidl_win.h"

namespace fxfile
{
namespace base
{
ShellEnumerator::ShellEnumerator(ShellItem &aShellFolderItem)
    : mShellFolderItem(aShellFolderItem)
    , mEnumerator(XPR_NULL)
{
}

ShellEnumerator::~ShellEnumerator(void)
{
}

xpr_bool_t ShellEnumerator::enumerate(void)
{
    return XPR_TRUE;
}

xpr_bool_t ShellEnumerator::next(ShellItem *aShellItem)
{
    return XPR_TRUE;
}
} // namespace base
} // namespace fxfile
