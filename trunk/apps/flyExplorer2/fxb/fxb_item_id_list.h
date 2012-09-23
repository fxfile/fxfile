//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_ITEM_ID_LIST_H__
#define __FXB_ITEM_ID_LIST_H__
#pragma once

namespace fxb
{
class ItemIDList
{
public:
    ItemIDList(LPITEMIDLIST aPidl = XPR_NULL);
    ItemIDList(xpr_tchar_t *aPath);
    ItemIDList(ItemIDList &aItemIDList);
    virtual ~ItemIDList(void);

public:
    LPITEMIDLIST init(LPITEMIDLIST aPidl);
    void free(void);

    xpr_bool_t isValid(void);

    LPITEMIDLIST operator->() { return mPidl ; }
    LPCITEMIDLIST operator->() const { return mPidl; }
    operator LPITEMIDLIST() const { return mPidl; }
    //operator=(LPITEMIDLIST aPidl) { mPidl = aPidl; };
    LPITEMIDLIST* operator&() { return &mPidl; }

protected:
    LPITEMIDLIST mPidl;
};
} // namespace fxb

#endif // __FXB_ITEM_ID_LIST_H__
