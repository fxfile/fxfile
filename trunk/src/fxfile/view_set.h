//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_VIEW_SET_H__
#define __FXFILE_VIEW_SET_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

#define MAX_COLUMN          (5)
#define DEF_COLUMN_SIZE     (200)  // pixel
#define MAX_COLUMN_SIZE     (1000) // pixel

#define MIN_COLUMN_ID       (0)
#define MAX_COLUMN_ID       (5)

typedef struct ColumnId
{
    GUID       mFormatId;
    xpr_sint_t mPropertyId;

    bool operator==(const ColumnId &aColumnId) const
    {
        if (mPropertyId != aColumnId.mPropertyId)
            return false;

        if (memcmp(&mFormatId, &aColumnId.mFormatId, sizeof(GUID)) != 0)
            return false;

        return true;
    }

    bool operator!=(const ColumnId &aColumnId) const
    {
        if (memcmp(&mFormatId, &aColumnId.mFormatId, sizeof(GUID)) == 0)
            return false;

        if (mPropertyId == aColumnId.mPropertyId)
            return false;

        return true;
    }

    bool operator<(const ColumnId &aColumnId) const
    {
        if (mPropertyId == aColumnId.mPropertyId)
            return memcmp(&mFormatId, &aColumnId.mFormatId, sizeof(GUID)) < 0;

        return (mPropertyId < aColumnId.mPropertyId);
    }
} ColumnId;

typedef struct ColumnSortInfo : public ColumnId
{
    xpr_bool_t     mAscending;
} ColumnSortInfo;

typedef struct ColumnSortData
{
    ColumnSortInfo mColumnSortInfo;
    xpr_sint_t     mColumn;
} ColumnSortData;

typedef struct ColumnItem : public ColumnId
{
    xpr_sint_t     mWidth;
} ColumnItem;

typedef struct ColumnInfo : public ColumnId
{
    xpr_sint_t     mColumn;
    xpr_sint_t     mWidth;
    xpr_sint_t     mAlign;
    xpr::tstring   mName;
} ColumnInfo;

typedef struct ColumnItemData : public ColumnInfo
{
    xpr_bool_t mUse;
} ColumnItemData;

typedef std::list<ColumnItemData *> ColumnDataList;

class ViewSetMgr;

class FolderViewSet
{
    friend class ViewSetMgr;

public:
    FolderViewSet(void);
    ~FolderViewSet(void);

protected:
    void clone(FolderViewSet &aFolderViewSet) const;
    void getHashValue(xpr_tchar_t *aHashValue) const;

public:
    xpr_uint_t      mViewStyle;
    ColumnSortInfo  mColumnSortInfo;
    xpr_bool_t      mAllSubApply;
    xpr_sint_t      mColumnCount;
    ColumnItem     *mColumnItem;
};

class ViewSetMgr : public fxfile::base::Singleton<ViewSetMgr>
{
    friend class fxfile::base::Singleton<ViewSetMgr>;

public:
    ViewSetMgr(void);
    virtual ~ViewSetMgr(void);

public:
    void load(void);
    xpr_bool_t save(void) const;

public:
    xpr_bool_t getViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet);
    xpr_bool_t setViewSet(const xpr_tchar_t *aPath, const FolderViewSet *aFolderViewSet);

    void setSaveLocation(xpr_bool_t aInstalledPath = XPR_TRUE);

    void verify(void);
    void clear(void);

protected:
    void loadIndex(fxfile::base::ConfFileEx &aConfFile);
    void saveIndex(fxfile::base::ConfFileEx &aConfFile) const;

    xpr_bool_t loadViewSet(fxfile::base::ConfFileEx &aConfFile, const xpr_tchar_t *aSection, FolderViewSet &aFolderViewSet);
    void       saveViewSet(fxfile::base::ConfFileEx &aConfFile, const xpr_tchar_t *aSection, const FolderViewSet &aFolderViewSet) const;

protected:
    typedef std::tr1::unordered_set<xpr::tstring> SubSet;                   // key = path
    typedef std::tr1::unordered_map<xpr::tstring, xpr::tstring> IndexMap;   // key = path
    typedef std::tr1::unordered_map<xpr::tstring, FolderViewSet *> HashMap; // key = hash

    SubSet   mSubSet;
    IndexMap mIndexMap;
    HashMap  mHashMap;

    xpr_bool_t mInstalledPath;                           // installed path
};

#endif // __FXFILE_VIEW_SET_H__
