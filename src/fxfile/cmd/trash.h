//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TRASH_H__
#define __FXFILE_TRASH_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class Trash
{
public:
    Trash(void);
    virtual ~Trash(void);

public:
    static void getQueryInfo(xpr_sint64_t *aCount = XPR_NULL, xpr_sint64_t *aSize = XPR_NULL);
    static HRESULT empty(HWND aHwnd, const xpr_tchar_t *aRootPath, DWORD aFlags);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TRASH_H__
