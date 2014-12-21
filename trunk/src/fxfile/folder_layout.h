//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FOLDER_LAYOUT_H__
#define __FXFILE_FOLDER_LAYOUT_H__ 1
#pragma once

#include "xpr_guid.h"
#include "conf_file_ex.h"

namespace fxfile
{
#define DEF_COLUMN_SIZE     (200)  // pixel
#define MAX_COLUMN_SIZE     (1000) // pixel

typedef struct ColumnId
{
    xpr::Guid  mFormatId;
    xpr_sint_t mPropertyId;
} ColumnId;

XPR_INLINE bool operator== (const ColumnId &aColumnId1, const ColumnId &aColumnId2)
{
    if (aColumnId1.mPropertyId != aColumnId2.mPropertyId)
        return false;

    if (aColumnId1.mFormatId != aColumnId2.mFormatId)
        return false;

    return true;
}

XPR_INLINE bool operator!= (const ColumnId &aColumnId1, const ColumnId &aColumnId2)
{
    if (aColumnId1.mFormatId == aColumnId2.mFormatId)
        return false;

    if (aColumnId1.mPropertyId == aColumnId2.mPropertyId)
        return false;

    return true;
}

XPR_INLINE bool operator< (const ColumnId &aColumnId1, const ColumnId &aColumnId2)
{
    if (aColumnId1.mPropertyId == aColumnId2.mPropertyId)
        return (aColumnId1.mFormatId < aColumnId2.mFormatId);

    return (aColumnId1.mPropertyId < aColumnId2.mPropertyId);
}

XPR_INLINE bool operator> (const ColumnId &aColumnId1, const ColumnId &aColumnId2)
{
    if (aColumnId1.mPropertyId == aColumnId2.mPropertyId)
        return (aColumnId1.mFormatId > aColumnId2.mFormatId);

    return (aColumnId1.mPropertyId > aColumnId2.mPropertyId);
}

typedef struct ColumnInfo : public ColumnId
{
    xpr_sint_t  mColumn;
    xpr_sint_t  mWidth;
    xpr_sint_t  mAlign;
    xpr::string mName;
} ColumnInfo;

typedef struct ColumnItemData : public ColumnInfo
{
    xpr_bool_t mUse;
} ColumnItemData;

typedef std::list<ColumnItemData *> ColumnDataList;

class FolderLayout
{
public:
    FolderLayout(void);
    ~FolderLayout(void);

public:
    void clear(void);
    void clone(FolderLayout &aFolderLayout) const;
    xpr_sint_t compare(const FolderLayout &aFolderLayout) const;
    xpr_bool_t test(const FolderLayout &aFolderLayout) const;
    xpr_uint32_t getHashValue(void) const;

public:
    struct ColumnItem : public ColumnId
    {
        xpr_sint_t mWidth;
    };

    xpr_uint_t  mViewStyle;
    ColumnId    mSortColumnId;
    xpr_bool_t  mSortAscending;
    xpr_bool_t  mAllSubApply;
    xpr_sint_t  mColumnCount;
    ColumnItem *mColumnItem;
};

XPR_INLINE bool operator== (const FolderLayout &aFolderLayout1, const FolderLayout &aFolderLayout2)
{
    return (aFolderLayout1.test(aFolderLayout2) == XPR_TRUE);
}

XPR_INLINE bool operator!= (const FolderLayout &aFolderLayout1, const FolderLayout &aFolderLayout2)
{
    return (aFolderLayout1.test(aFolderLayout2) != XPR_TRUE);
}

XPR_INLINE bool operator< (const FolderLayout &aFolderLayout1, const FolderLayout &aFolderLayout2)
{
    return (aFolderLayout1.compare(aFolderLayout2) < 0) ? true : false;
}

XPR_INLINE bool operator> (const FolderLayout &aFolderLayout1, const FolderLayout &aFolderLayout2)
{
    return (aFolderLayout1.compare(aFolderLayout2) > 0) ? true : false;
}

typedef struct FolderLayoutChange
{
    enum ChangeMode
    {
        ChangeModeViewStyle,
        ChangeModeColumnUse,
        ChangeModeColumnSize,
        ChangeModeSortItems,
    };

    ChangeMode  mChangeMode;
    xpr_sint_t  mViewStyle;
    xpr_bool_t  mRefresh;
    ColumnId   *mColumnId;
    xpr_bool_t  mSortAscending;
    xpr_sint_t  mColumnIndex;
    xpr_sint_t  mColumnWidth;
} FolderLayoutChange;
} // namespace fxfile

#endif // __FXFILE_FOLDER_LAYOUT_H__
