//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_VIEW_SET_H__
#define __FX_VIEW_SET_H__
#pragma once

#define MAX_COLUMN          5
#define DEF_COLUMN_SIZE     200  // pixel
#define MAX_COLUMN_SIZE     1000 // pixel

#define MIN_COLUMN_ID       0
#define MAX_COLUMN_ID       5

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
    std::tstring   mName;
} ColumnInfo;

typedef struct ColumnItemData : public ColumnInfo
{
    xpr_bool_t mUse;
} ColumnItemData;

typedef std::list<ColumnItemData *> ColumnDataList;

class FolderViewSet
{
public:
    FolderViewSet(void)
        : mViewStyle(LVS_REPORT)
        , mAllSubApply(XPR_FALSE)
        , mColumnCount(0)
        , mColumnItem(XPR_NULL)
    {
        mColumnSortInfo.mFormatId   = GUID_NULL;
        mColumnSortInfo.mPropertyId = 0;
        mColumnSortInfo.mAscending  = XPR_TRUE;
    }

    ~FolderViewSet(void)
    {
        XPR_SAFE_DELETE_ARRAY(mColumnItem);
    }

public:
    DWORD           mViewStyle;
    ColumnSortInfo  mColumnSortInfo;
    xpr_bool_t      mAllSubApply;
    xpr_sint_t      mColumnCount;
    ColumnItem     *mColumnItem;
};

class ViewSet
{
public:
    ViewSet(void);
    virtual ~ViewSet(void);

public:
    xpr_bool_t getViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet);
    xpr_bool_t setViewSet(const xpr_tchar_t *aPath, FolderViewSet *aFolderViewSet);

    void setCfgViewSet(xpr_bool_t aInstPath = XPR_TRUE);

    void verify(void);
    void clear(void);

protected:
    void readIndex(void);
    void saveIndex(void);

protected:
    typedef std::set<std::tstring> SubSet;               // path
    typedef std::map<std::tstring, std::tstring> KeyMap; // path, md5 hash

    SubSet mSubSet;
    KeyMap mKeyMap;

    xpr_bool_t mInstPath;                                // installed path
};

class ViewSetMgr : public ViewSet, public xpr::Singleton<ViewSetMgr>
{
    friend class xpr::Singleton<ViewSetMgr>;

protected: ViewSetMgr(void);
public:   ~ViewSetMgr(void);
};

#endif // __FX_VIEW_SET_H__
