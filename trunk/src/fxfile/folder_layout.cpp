//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_layout.h"

#include "xpr_hash.h"

#include "conf_file_ex.h"
#include "md5.h"
#include "conf_dir.h"
#include "fxfile_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
FolderLayout::FolderLayout(void)
    : mColumnItem(XPR_NULL)
{
    clear();
}

FolderLayout::~FolderLayout(void)
{
    XPR_SAFE_DELETE_ARRAY(mColumnItem);
}

void FolderLayout::clear(void)
{
    mViewStyle                = VIEW_STYLE_DETAILS;
    mSortColumnId.mFormatId.clear();
    mSortColumnId.mPropertyId = 0;
    mSortAscending            = XPR_TRUE;
    mAllSubApply              = XPR_FALSE;
    mColumnCount              = 0;
    XPR_SAFE_DELETE_ARRAY(mColumnItem);
}

void FolderLayout::clone(FolderLayout &aFolderLayout) const
{
    XPR_SAFE_DELETE_ARRAY(aFolderLayout.mColumnItem);

    aFolderLayout = *this;

    if (mColumnCount > 0)
    {
        aFolderLayout.mColumnItem = new ColumnItem[mColumnCount];
        memcpy(aFolderLayout.mColumnItem, mColumnItem, sizeof(ColumnItem) * mColumnCount);
    }
}

xpr_sint_t FolderLayout::compare(const FolderLayout &aFolderLayout) const
{
    xpr_sint_t sDiff;

    sDiff = mViewStyle - aFolderLayout.mViewStyle;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = mSortColumnId.mFormatId.compare(aFolderLayout.mSortColumnId.mFormatId);
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = mSortColumnId.mPropertyId - aFolderLayout.mSortColumnId.mPropertyId;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = mSortAscending - aFolderLayout.mSortAscending;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = mAllSubApply - aFolderLayout.mAllSubApply;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    sDiff = mColumnCount - aFolderLayout.mColumnCount;
    if (sDiff != 0)
        return (sDiff > 0) ? 1 : -1;

    xpr_sint_t i;
    for (i = 0; i < mColumnCount; ++i)
    {
        sDiff = mColumnItem[i].mFormatId.compare(aFolderLayout.mColumnItem[i].mFormatId);
        if (sDiff != 0)
            return (sDiff > 0) ? 1 : -1;

        sDiff = mColumnItem[i].mPropertyId - aFolderLayout.mColumnItem[i].mPropertyId;
        if (sDiff != 0)
            return (sDiff > 0) ? 1 : -1;

        sDiff = mColumnItem[i].mWidth - aFolderLayout.mColumnItem[i].mWidth;
        if (sDiff != 0)
            return (sDiff > 0) ? 1 : -1;
    }

    return 0;
}

xpr_bool_t FolderLayout::test(const FolderLayout &aFolderLayout) const
{
    return (compare(aFolderLayout) == 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_uint32_t FolderLayout::getHashValue(void) const
{
    xpr::Hash sHash;
    sHash.reset(xpr::Hash::Default);

    sHash.update((xpr_byte_t *)&mViewStyle,     sizeof(xpr_uint_t));
    sHash.update((xpr_byte_t *)&mSortColumnId,  sizeof(ColumnId));
    sHash.update((xpr_byte_t *)&mSortAscending, sizeof(xpr_bool_t));
    sHash.update((xpr_byte_t *)&mAllSubApply,   sizeof(xpr_bool_t));
    sHash.update((xpr_byte_t *)&mColumnCount,   sizeof(xpr_sint_t));
    sHash.update((xpr_byte_t *)mColumnItem,     sizeof(ColumnItem) * mColumnCount);

    xpr_uint32_t sHashValue = *((const xpr_uint32_t *)sHash.getHashValue());

    return sHashValue;
}
} // namespace fxfile
