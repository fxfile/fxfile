//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_UPDATE_CHECKER_H__
#define __FXFILE_UPCHECKER_UPDATE_CHECKER_H__ 1
#pragma once

#include "meta_data_file.h"

namespace fxfile
{
namespace upchecker
{
class UpdateChecker
{
public:
    UpdateChecker(void);
    virtual ~UpdateChecker(void);

public:
    static void initStatic(void);
    static void finalizeStatic(void);

    void setUrl(const xpr::string &aUrl);
    void setDir(const xpr::string &aDir);

    xpr_bool_t check(void);
    xpr_bool_t isChecked(void) const;
    const MetaDataFile::MetaData &getMetaData(void) const;

private:
    static xpr_size_t downloadFile(void *aSource, xpr_size_t aSize, xpr_size_t aMemoryBlock, void *aUserData);

private:
    xpr::string mUrl;
    xpr::string mDir;
    xpr::string mMetaFilePath;

    MetaDataFile::MetaData mMetaData;

    xpr_bool_t mCheckInProgress;
    xpr_bool_t mChecked;
};
} // namespace fxfile
} // namespace upchecker

#endif // __FXFILE_UPCHECKER_UPDATE_CHECKER_H__
