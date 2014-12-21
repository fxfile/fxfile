//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FOLDER_LAYOUT_MANAGER_H__
#define __FXFILE_FOLDER_LAYOUT_MANAGER_H__ 1
#pragma once

#include "folder_layout.h"
#include "singleton_manager.h"

namespace fxfile
{
namespace base
{
class ConfFileEx;
} // namespace base

class FolderLayoutManager : public SingletonInstance
{
    friend class SingletonManager;

protected: FolderLayoutManager(void);
public:   ~FolderLayoutManager(void);

public:
    void load(void);
    xpr_bool_t save(void) const;

public:
    xpr_bool_t load(const xpr::string &aKey, FolderLayout &aFolderLayout);
    xpr_bool_t save(const xpr::string &aKey, const FolderLayout &aFolderLayout);

    void setSaveLocation(xpr_bool_t aInstalledPath = XPR_TRUE);

    void verify(void);
    void clear(void);

public:
    static xpr_bool_t loadFromConfFile(FolderLayout &aFolderLayout, fxfile::base::ConfFileEx &aConfFile, base::ConfFile::Section *aSection, const xpr_tchar_t *aBaseKey);
    static void saveToConfFile(const FolderLayout &aFolderLayout, fxfile::base::ConfFileEx &aConfFile, base::ConfFile::Section *aSection, const xpr_tchar_t *aBaseKey);

private:
    void loadIndex(fxfile::base::ConfFileEx &aConfFile);
    void saveIndex(fxfile::base::ConfFileEx &aConfFile) const;

    xpr_uint64_t generateHashValue(const FolderLayout &aFolderLayout) const;

private:
    typedef std::tr1::unordered_set<xpr::string> SubSet;                   // <path>
    typedef std::tr1::unordered_map<xpr::string, xpr_uint64_t> IndexMap;   // <path, hash>
    typedef std::tr1::unordered_map<xpr_uint64_t, FolderLayout *> HashMap; // <hash, value>

    SubSet   mSubSet;
    IndexMap mIndexMap;
    HashMap  mHashMap;

    xpr_bool_t mInstalledPath;
};
} // namespace fxfile

#endif // __FXFILE_FOLDER_LAYOUT_MANAGER_H__
