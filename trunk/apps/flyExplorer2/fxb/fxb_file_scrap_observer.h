//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FILE_SCRAP_OBSERVER_H__
#define __FXB_FILE_SCRAP_OBSERVER_H__
#pragma once

#include "fxb_file_scrap.h"

namespace fxb
{
class FileScrapObserver
{
public:
    virtual void onAddGroup(FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
    virtual void onSetCurGroup(FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
    virtual void onRenameGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
    virtual void onRemoveGroup(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
    virtual void onRemoveAllGroups(fxb::FileScrap &aFileScrap) = 0;

    virtual void onUpdateList(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
    virtual void onRemovedItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup, const fxb::FileScrap::IdSet &aIdSet) = 0;
    virtual void onRemovedAllItems(fxb::FileScrap &aFileScrap, fxb::FileScrap::Group *aGroup) = 0;
};
} // namespace fxb

#endif // __FXB_FILE_SCRAP_OBSERVER_H__
