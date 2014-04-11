//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPDATER_UPDATE_CHECKER_H__
#define __FXFILE_UPDATER_UPDATE_CHECKER_H__ 1
#pragma once

#include "meta_data_file.h"

namespace fxfile
{
namespace updater
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
    void setNowVersion(const xpr::string &aNowVersion, xpr_bool_t aCheckMinorVer);

    xpr_bool_t check(void);
    xpr_bool_t isChecked(void) const;
    void getCheckedVersion(xpr::string &aVersion);

    xpr_bool_t existNewVersion(void) const;

    xpr_bool_t download(void);
    xpr_bool_t isDownloaded(void);
    void getDownloadedFilePath(xpr::string &aFilePath);

protected:
    static xpr_size_t downloadFile(void *aSource, xpr_size_t aSize, xpr_size_t aMemoryBlock, void *aUserData);

protected:
    xpr::string mUrl;
    xpr::string mDir;
    xpr::string mMetaFilePath;
    xpr::string mNowVersion;
    xpr_bool_t  mCheckMinorVer;

    MetaDataFile::MetaData mMetaData;

    xpr_bool_t  mCheckInProgress;
    xpr_bool_t  mChecked;
    xpr_bool_t  mNewVersion;

    xpr::string mProgramFilePath;
    xpr_bool_t  mDownloadInProgress;
    xpr_bool_t  mDownloaded;
};
} // namespace fxfile
} // namespace updater

#endif // __FXFILE_UPDATER_UPDATE_CHECKER_H__
