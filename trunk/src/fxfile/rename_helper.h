//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_RENAME_HELPER_H__
#define __FXFILE_RENAME_HELPER_H__ 1
#pragma once

namespace fxfile
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
    void setItem(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aShellAttributes);

    const xpr_tchar_t *getEditName(void);
    void getEditSel(xpr_sint_t *aSelItem0, xpr_sint_t *aSelItem1);
    xpr_sint_t getIconIndex(void);

    Result rename(HWND aHwnd, const xpr_tchar_t *aNewName);

    LPITEMIDLIST getNewItemIDList(xpr_bool_t aNull = XPR_TRUE);
    const xpr_tchar_t *getNewName(void);

protected:
    RenameStyle   mRenameStyle;

    LPSHELLFOLDER mShellFolder;
    LPCITEMIDLIST mPidl;
    LPITEMIDLIST  mFullPidl;
    xpr_ulong_t   mShellAttributes;

    xpr::string   mEditName;

    LPITEMIDLIST  mNewPidl;
    LPITEMIDLIST  mNewFullPidl;
    xpr::string   mNewName;

    xpr_sint_t    mSelItem0;
    xpr_sint_t    mSelItem1;
    xpr::string   mKeepExt;
    xpr::string   mParsingExt;
};
} // namespace fxfile

#endif // __FXFILE_RENAME_HELPER_H__
