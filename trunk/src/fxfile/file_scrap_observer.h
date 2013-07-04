//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_OBSERVER_H__
#define __FXFILE_FILE_SCRAP_OBSERVER_H__ 1
#pragma once

#include "file_scrap.h"

namespace fxfile
{
class FileScrapObserver
{
public:
    virtual void onAddGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
    virtual void onSetCurGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
    virtual void onRenameGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
    virtual void onRemoveGroup(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
    virtual void onRemoveAllGroups(FileScrap &aFileScrap) = 0;

    virtual void onUpdateList(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
    virtual void onRemovedItems(FileScrap &aFileScrap, FileScrap::Group *aGroup, const FileScrap::IdSet &aIdSet) = 0;
    virtual void onRemovedAllItems(FileScrap &aFileScrap, FileScrap::Group *aGroup) = 0;
};
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_OBSERVER_H__
