//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_SCRAP_H__
#define __FXB_FILE_SCRAP_H__
#pragma once

namespace fxb
{
const xpr_size_t MAX_FILE_SCRAP_GROUP      = 20;
const xpr_size_t MAX_FILE_SCRAP_GROUP_NAME = 50;

const xpr_tchar_t kFileScrapNoGroupName[] = XPR_STRING_LITERAL("{{No-Group}}");

class FileScrap : public xpr::Singleton<FileScrap>
{
    friend class xpr::Singleton<FileScrap>;

protected: FileScrap(void);
public:   ~FileScrap(void);

public:
    enum { InvalidGroupId = 0 };

    struct Item
    {
        xpr_uint_t   mGroupId;
        std::tstring mPath;
    };

    typedef std::deque<Item *> ItemDeque;

    struct Group
    {
        xpr_uint_t   mGroupId;
        std::tstring mGroupName;
        ItemDeque    mItemDeque;

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
    // group
    xpr_uint_t addGroup(const xpr_tchar_t *aGroupName);
    xpr_size_t getGroupCount(void);
    Group *getGroup(xpr_size_t aIndex);
    Group *findGroup(xpr_uint_t aGroupId);
    Group *findGroup(const xpr_tchar_t *aGroupName);
    xpr_uint_t findGroupId(const xpr_tchar_t *aGroupName);
    xpr_bool_t renameGroup(const xpr_tchar_t *aOldGroupName, const xpr_tchar_t *aNewGroupName);
    void removeGroup(xpr_uint_t aGroupId);
    void removeAllGroups(void);

    void setCurGourpId(xpr_uint_t aGroupId);
    xpr_uint_t getCurGroupId(void) const;
    xpr_uint_t getDefGroupId(void) const;

    // item
    void addItem(xpr_uint_t aGroupId, const xpr_tchar_t *aPath);
    void removeItem(xpr_uint_t aGroupId, xpr_size_t aIndex);
    void removeAllItems(xpr_uint_t aGroupId);
    void getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex, xpr_tchar_t *aPath);
    const xpr_tchar_t *getItemPath(xpr_uint_t aGroupId, xpr_size_t aIndex);
    Item *getItem(xpr_uint_t aGroupId, xpr_size_t aIndex);
    xpr_size_t getItemCount(xpr_uint_t aGroupId);
    xpr_bool_t isItemExist(xpr_uint_t aGroupId, const xpr_tchar_t *aPath);
    xpr_bool_t isItemFileAllExist(xpr_uint_t aGroupId);
    xpr_size_t getItemCountNotExist(xpr_uint_t aGroupId);
    void removeItemNotExist(xpr_uint_t aGroupId);
    xpr_tchar_t *getItemFileOpPath(xpr_uint_t aGroupId, xpr_size_t *aCount = XPR_NULL);

    // operation
    void copyOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget);
    void moveOperation(xpr_uint_t aGroupId, const xpr_tchar_t *aTarget);
    void deleteOperation(xpr_uint_t aGroupId);
    void recycleBinOperation(xpr_uint_t aGroupId);
    xpr_bool_t setClipboardCopy(xpr_uint_t aGroupId);
    xpr_bool_t setClipboardCut(xpr_uint_t aGroupId);

    xpr_bool_t loadFromFile(const xpr_tchar_t *aPath);
    xpr_bool_t saveToFile(const xpr_tchar_t *aPath);

protected:
    xpr_bool_t setClipboard(xpr_uint_t aGroupId, xpr_bool_t aCopy = XPR_TRUE);
    void doOperation(xpr_uint_t aGroupId, xpr_sint_t aMode, const xpr_tchar_t *aTarget);

    xpr_uint_t generateGroupId(void);

    xpr_uint_t setDefGroup(void);

protected:
    typedef std::deque<Group *> GroupDeque;
    GroupDeque   mGroupDeque;

    xpr_uint_t   mIdMgr;

    typedef std::map<std::tstring, Group *> GroupMap;
    GroupMap     mGroupMap;

    typedef std::map<xpr_uint_t, Group *> GroupIdMap;
    GroupIdMap   mGroupIdMap;

    xpr_uint_t   mCurGroupId;
    xpr_uint_t   mDefGroupId;

    xpr_tchar_t *mFileOpPath;
};
} // namespace fxb

#endif // __FXB_FILE_SCRAP_H__
