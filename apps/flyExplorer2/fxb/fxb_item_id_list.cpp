//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_item_id_list.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxb
{
ItemIDList::ItemIDList(LPITEMIDLIST aPidl)
    : mPidl(aPidl)
{
}

ItemIDList::ItemIDList(xpr_tchar_t *aPath)
    : mPidl(XPR_NULL)
{
    SHGetPidlFromPath(aPath, &mPidl);
}

ItemIDList::ItemIDList(ItemIDList &aItemIDList)
    : mPidl(XPR_NULL)
{
    if (aItemIDList.isValid() == XPR_FALSE)
    {
        mPidl = CopyItemIDList(aItemIDList);
    }
}

ItemIDList::~ItemIDList(void)
{
}

LPITEMIDLIST ItemIDList::init(LPITEMIDLIST aPidl)
{
    LPITEMIDLIST sPrevPidl = mPidl;
    mPidl = aPidl;
    return sPrevPidl;
}

void ItemIDList::free(void)
{
    COM_FREE(mPidl);
}

xpr_bool_t ItemIDList::isValid(void)
{
    return mPidl != XPR_NULL;
}
} // namespace fxb
