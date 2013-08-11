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
class ShellEnumerator
{
public:
    enum
    {
        ListTypeAll = 0,
        ListTypeOnlyFile,
        ListTypeOnlyFolder,
    };

    enum
    {
        AttributeHidden = 0x01,
        AttributeSystem = 0x02,
    };

public:
    ShellEnumerator(void);
    virtual ~ShellEnumerator(void);

public:
    xpr_bool_t enumerate(HWND aHwnd, LPSHELLFOLDER aShellFolder, xpr_sint_t aListType, xpr_sint_t aAttributes);
    xpr_bool_t next(LPITEMIDLIST *aPidl);

protected:
    LPENUMIDLIST mEnumIdList;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_SHELL_ENUMERATOR_H__
