//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_RECENT_FILE_LIST_H__
#define __FXFILE_RECENT_FILE_LIST_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base
} // namespace fxfile

namespace fxfile
{
class RecentFileList : public fxfile::base::Singleton<RecentFileList>
{
    friend class fxfile::base::Singleton<RecentFileList>;

public:
    RecentFileList(void);
    virtual ~RecentFileList(void);

public:
    void load(fxfile::base::ConfFileEx &aConfFile);
    void save(fxfile::base::ConfFileEx &aConfFile) const;

public:
    void               addFile(const xpr_tchar_t *aPath);
    xpr_size_t         getFileCount(void) const;
    const xpr_tchar_t *getFile(xpr_size_t aIndex) const;
    void               clear(void);

protected:
    typedef std::deque<xpr::tstring> FileDeque;
    FileDeque mFileDeque;
};
} // namespace fxfile

#endif // __FXFILE_RECENT_FILE_LIST_H__
