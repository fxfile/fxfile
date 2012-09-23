//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_PIDL_H__
#define __FXB_PIDL_H__
#pragma once

namespace fxb
{
LPITEMIDLIST ConcatPidls(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
LPITEMIDLIST CopyItemIDList(LPITEMIDLIST aPidl);
LPITEMIDLIST CreatePidl(xpr_uint_t aSize);
xpr_uint_t   GetSize(LPCITEMIDLIST aPidl);
LPITEMIDLIST Next(LPCITEMIDLIST aPidl);
xpr_sint_t   GetItemIDCount(LPITEMIDLIST aPidl);
xpr_sint_t   CompareItemID(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
xpr_sint_t   CompareItemID(LPCITEMIDLIST aPidl1, xpr_sint_t nSpecialFolder);
xpr_bool_t   IL_IsParent(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);
xpr_bool_t   IL_IsEqual(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
xpr_bool_t   IL_RemoveLastID(LPITEMIDLIST aPidl);
LPITEMIDLIST IL_FindLastID(LPCITEMIDLIST aPidl);
LPITEMIDLIST IL_Clone(LPCITEMIDLIST aPidl);
LPITEMIDLIST IL_FindChild(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);
xpr_bool_t   IsPidlDesktop(LPCITEMIDLIST aPidl);
xpr_bool_t   IsPidlSimple(LPCITEMIDLIST aPidl);
} // namespace fxb

#endif // __FXB_PIDL_H__
