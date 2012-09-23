//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_RENAME_HELPER_H__
#define __FXB_RENAME_HELPER_H__
#pragma once

namespace fxb
{
class RenameHelper
{
public:
    enum RenameStyle
    {
        RenameStyleNone,
        RenameStyleKeepExt,
        RenameStyleSelExceptForExt
    };

    enum Result
    {
        ResultSucceeded,
        ResultEmptiedName,
        ResultInvalidParameter,
        ResultInvalidName,
        ResultUserCancel,
        ResultUnknownError,
    };

public:
    RenameHelper(RenameStyle aRenameStyle = RenameStyleNone);
    virtual ~RenameHelper(void);

public:
    void setRenameStyle(RenameStyle aRenameStyle);
    void setItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aShellAttributes);

    const xpr_tchar_t *getEditName(void);
    void getEditSel(xpr_sint_t *aSelItem0, xpr_sint_t *aSelItem1);
    xpr_sint_t getIconIndex(void);

    Result rename(HWND aHwnd, const xpr_tchar_t *aNewName);

    LPITEMIDLIST getNewItemIDList(xpr_bool_t aNull = XPR_TRUE);
    const xpr_tchar_t *getNewName(void);

protected:
    RenameStyle   mRenameStyle;

    LPSHELLFOLDER mShellFolder;
    LPITEMIDLIST  mPidl;
    LPITEMIDLIST  mFullPidl;
    xpr_ulong_t   mShellAttributes;

    std::tstring  mEditName;

    LPITEMIDLIST  mNewPidl;
    LPITEMIDLIST  mNewFullPidl;
    std::tstring  mNewName;

    xpr_sint_t    mSelItem0;
    xpr_sint_t    mSelItem1;
    std::tstring  mKeepExt;
    std::tstring  mParsingExt;
};
} // namespace fxb

#endif // __FXB_RENAME_HELPER_H__
