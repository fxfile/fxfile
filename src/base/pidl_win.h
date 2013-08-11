//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_PIDL_WIN_H__
#define __FXFILE_BASE_PIDL_WIN_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
/**
 * PIDL (Pointer to Item Identifier List) for Microsoft Windows Shell
 */
class Pidl
{
public:
    static LPITEMIDLIST alloc(xpr_size_t aSize);
    static void         free(LPITEMIDLIST aPidl);
    static LPITEMIDLIST clone(LPCITEMIDLIST aPidl);
    static LPITEMIDLIST concat(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
    static xpr_size_t   getSize(LPCITEMIDLIST aPidl);
    static LPITEMIDLIST next(LPCITEMIDLIST aPidl);
    static xpr_sint_t   getItemCount(LPCITEMIDLIST aPidl);
    static xpr_sint_t   compare(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
    static xpr_sint_t   compare(LPCITEMIDLIST aPidl1, xpr_sint_t aSpecialFolder);
    static xpr_bool_t   isParent(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);
    static xpr_bool_t   isEqual(LPCITEMIDLIST aPidl1, LPCITEMIDLIST aPidl2);
    static xpr_bool_t   removeLastItem(LPITEMIDLIST aPidl);
    static LPITEMIDLIST findLastItem(LPCITEMIDLIST aPidl);
    static LPITEMIDLIST findChildItem(LPCITEMIDLIST aParentPidl, LPCITEMIDLIST aChildPidl);
    static xpr_bool_t   isDesktopFolder(LPCITEMIDLIST aPidl);
    static xpr_bool_t   isSimplePidl(LPCITEMIDLIST aPidl);
    static LPITEMIDLIST create(const xpr_tchar_t *aPath);
    static LPITEMIDLIST create(const xpr::tstring &aPath);
    static LPITEMIDLIST create(xpr_sint_t aSpecialFolder);
    static LPITEMIDLIST create(const KNOWNFOLDERID &aKnownFolderId);
    static HRESULT      create(const xpr_tchar_t *aPath, LPITEMIDLIST &aFullPidl);
    static HRESULT      create(const xpr::tstring &aPath, LPITEMIDLIST &aFullPidl);
    static HRESULT      create(xpr_sint_t aSpecialFolder, LPITEMIDLIST &aFullPidl);
    static HRESULT      create(const KNOWNFOLDERID &aKnownFolderId, LPITEMIDLIST &aFullPidl);
    static LPITEMIDLIST getFullPidl(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aSimplePidl);
    static xpr_bool_t   getSimplePidl(LPCITEMIDLIST aFullPidl, LPSHELLFOLDER &aShellFolder, LPCITEMIDLIST &aSimplePidl);
    static xpr_bool_t   getShellFolder(LPSHELLFOLDER aParentShellFolder, LPCITEMIDLIST aPidl, LPSHELLFOLDER &aShellFolder);
    static xpr_bool_t   getShellFolder(LPCITEMIDLIST aFullPidl, LPSHELLFOLDER &aShellFolder);

    static xpr_bool_t   getName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr_tchar_t *aName, xpr_size_t aMaxLen);
    static xpr_bool_t   getName(LPCITEMIDLIST aFullPidl, DWORD aFlags, xpr::tstring &aName);
    static xpr_bool_t   getName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr_tchar_t *aName, xpr_size_t aMaxLen);
    static xpr_bool_t   getName(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, DWORD aFlags, xpr::tstring &aName);
    static xpr_ulong_t  getAttributes(LPCITEMIDLIST aFullPidl);
    static xpr_bool_t   getAttributes(LPCITEMIDLIST aFullPidl, xpr_ulong_t &aAttributes);
    static xpr_ulong_t  getAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl);
    static xpr_bool_t   getAttributes(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_ulong_t &aAttributes);
    static xpr_bool_t   getInfotip(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, xpr_tchar_t *aInfotip, xpr_size_t aMaxLen);
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_PIDL_WIN_H__
