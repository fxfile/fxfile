//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_H__
#define __FXFILE_FILE_SCRAP_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
const xpr_size_t MAX_FILE_SCRAP_GROUP      = 20;
const xpr_size_t MAX_FILE_SCRAP_GROUP_NAME = 50;

const xpr_tchar_t kFileScrapNoGroupName[] = XPR_STRING_LITERAL("{{No-Group}}");

class FileScrapObserver;

class FileScrap : public fxfile::base::Singleton<FileScrap>
{
    friend class fxfile::base::Singleton<FileScrap>;

protected: FileScrap(void);
public:   ~FileScrap(void);

public:
    enum { InvalidGroupId = 0 };

    struct Item
    {
        xpr_uint_t   mGroupId;
        xpr_uint_t   mId;
        xpr::tstring mPath;
    };

    typedef std::deque<Item *> ItemDeque;
    typedef std::set<xpr_uint_t> IdSet;

    struct Group
    {
        xpr_uint_t   mGroupId;
        xpr::tstring mGroupName;
        ItemDeque    mItemDeque;
        xpr_uint_t   mIdMgr;

        ~Group(void)
        {
            Item *sItem;
            ItemDeque::iterator sIterator;

            sIterator = mItemDeque.begin();
            for (; sIterator != mItemDeque.end(); ++sIterator)
            {
                sItem = *sIterator;
                XPR_SAFE_DELETE(sItem);
            }

            mItemDeque.clear();
        }
    };

public:
    void setObserver(FileScrapObserver *aObserver);

    // group
    xpr_uint_t   addGroup(const xpr_tchar_t *aGroupName);
    xpr_size_t   getGroupCount(void) const;
    Group *      getGroup(xpr_size_t aIndex) const;
    Group *      findGroup(xpr_uint_t aGroupId) const;
    Group *      findGroup(const xpr_tchar_t *aGroupName) const;
    xpr_uint_t   findGroupId(const xpr_tchar_t *aGroupName) const;
    xpr_bool_t   renameGroup(const xpr_tchar_t *aOldGroupName, const xpr_tchar_t *aNewGroupName);
    void         removeGroup(xpr_uint_t aGroupId);
    void         removeAllGroups(void);

    void         setCurGroupId(xpr_uint_t aGroupId);
    xpr_uint_t   getCurGroupId(void) const;
    xpr_uint_t   getDefGroupId(void) const;

    // item
    xpr_bool_t   addItem(xpr_uint_t aGroupId, LPSHELLFOLDER aShellFolder, LPCITEMIDLIST *aPidls, xpr_sint_t aCount);
    xpr_bool_t   addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath, xpr_sint_t aCount);
    xpr_bool_t   addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath);
    xpr_bool_t   regenerate(xpr_uint_t aGroupId);
    void         removeItem(xpr_uint_t aGroupId, xpr_size_t aIndex);
    void         removeItems(xpr_uint_t aGroupId, const IdSet &aIdSet);
    void         removeAllItems(xpr_uint_t aGroupId);
    void         getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex, xpr_tchar_t *aPath) const;
    const xpr_tchar_t *getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex) const;
    Item *       getItem(xpr_uint_t aGroupId, xpr_size_t aIndex) const;
    xpr_size_t   getItemCount(xpr_uint_t aGroupId) const;
    xpr_bool_t   isItemEmpty(xpr_uint_t aGroupId) const;
    xpr_bool_t   isItemExist(xpr_uint_t aGroupId, const xpr_tchar_t *aPath) const;
    xpr_bool_t   isItemExist(Group *aGroup, const xpr_tchar_t *aPath) const;
    xpr_size_t   getItemCountNotExist(xpr_uint_t aGroupId) const;
    xpr_size_t   removeItemNotExist(xpr_uint_t aGroupId);
    xpr_tchar_t *getItemFileOpPath(xpr_uint_t aGroupId, xpr_size_t *aCount = XPR_NULL);

    // operation
    void         copyOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget);
    void         moveOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget);
    void         deleteOperation(xpr_uint_t aGroupId);
    void         trashOperation(xpr_uint_t aGroupId);
    xpr_bool_t   setClipboardCopy(xpr_uint_t aGroupId);
    xpr_bool_t   setClipboardCut(xpr_uint_t aGroupId);

    xpr_bool_t   load(const xpr_tchar_t *aPath);
    xpr_bool_t   save(const xpr_tchar_t *aPath) const;

protected:
    void addItem(Group *aGroup, const xpr_tchar_t *aPath);

    xpr_bool_t setClipboard(xpr_uint_t aGroupId, xpr_bool_t aCopy = XPR_TRUE);
    void doOperation(xpr_uint_t aGroupId, xpr_sint_t aMode, const xpr_tchar_t *aTarget);

    xpr_uint_t generateGroupId(void);

    xpr_uint_t setDefGroup(void);

protected:
    FileScrapObserver *mObserver;

    typedef std::deque<Group *> GroupDeque;
    GroupDeque   mGroupDeque;

    xpr_uint_t   mGroupIdMgr;

    typedef std::map<xpr::tstring, Group *> GroupMap;
    GroupMap     mGroupMap;

    typedef std::map<xpr_uint_t, Group *> GroupIdMap;
    GroupIdMap   mGroupIdMap;

    xpr_uint_t   mCurGroupId;
    xpr_uint_t   mDefGroupId;

    xpr_tchar_t *mFileOpPath;
};
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_H__
