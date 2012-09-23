//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SHELL_FOLDER_H__
#define __FXB_SHELL_FOLDER_H__
#pragma once

namespace fxb
{
typedef const LPSHELLFOLDER LPCSHELLFOLDER;

class ShellFolder
{
public:
    ShellFolder(LPSHELLFOLDER aShellFolder = XPR_NULL);
    virtual ~ShellFolder(void);

public:
    LPSHELLFOLDER init(LPSHELLFOLDER aShellFolder);
    void release(void);

    xpr_bool_t isValid(void);

    LPSHELLFOLDER operator->() { return mShellFolder; }
    LPCSHELLFOLDER operator->() const { return mShellFolder; }
    operator LPSHELLFOLDER() { return mShellFolder; }
    //operator=(LPSHELLFOLDER lpsf) { mShellFolder = lpsf; };
    LPSHELLFOLDER* operator&() { return &mShellFolder; };

protected:
    LPSHELLFOLDER mShellFolder;
};
} // namespace fxb

#endif // __FXB_SHELL_FOLDER_H__
