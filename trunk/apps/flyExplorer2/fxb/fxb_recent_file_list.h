//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_RECENT_FILE_LIST_H__
#define __FXB_RECENT_FILE_LIST_H__
#pragma once

namespace fxb
{
class IniFileEx;

class RecentFileList : public xpr::Singleton<RecentFileList>
{
    friend class xpr::Singleton<RecentFileList>;

public:
    RecentFileList(void);
    virtual ~RecentFileList(void);

public:
    void load(fxb::IniFileEx &aIniFile);
    void save(fxb::IniFileEx &aIniFile) const;

public:
    void               addFile(const xpr_tchar_t *aPath);
    xpr_size_t         getFileCount(void) const;
    const xpr_tchar_t *getFile(xpr_size_t aIndex) const;
    void               clear(void);

protected:
    typedef std::deque<std::tstring> FileDeque;
    FileDeque mFileDeque;
};
} // namespace fxb

#endif // __FXB_RECENT_FILE_LIST_H__
