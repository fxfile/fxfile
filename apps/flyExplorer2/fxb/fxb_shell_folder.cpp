//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_shell_folder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
ShellFolder::ShellFolder(LPSHELLFOLDER aShellFolder)
    : mShellFolder(aShellFolder)
{
    //ShellFolder p;
    //::SHGetDesktopFolder(&p);
}

ShellFolder::~ShellFolder(void)
{
}

LPSHELLFOLDER ShellFolder::init(LPSHELLFOLDER aShellFolder)
{
    LPSHELLFOLDER sOldShellFolder = mShellFolder;
    mShellFolder = aShellFolder;
    return sOldShellFolder;
}

void ShellFolder::release(void)
{
    COM_RELEASE(mShellFolder);
}

xpr_bool_t ShellFolder::isValid(void)
{
    return mShellFolder != XPR_NULL;
}
} // namespace fxb
