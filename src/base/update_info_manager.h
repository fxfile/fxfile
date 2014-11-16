//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_UPDATE_INFO_MGR_H__
#define __FXFILE_BASE_UPDATE_INFO_MGR_H__ 1
#pragma once

#include "xpr_shm.h"
#include "xpr_process_sync.h"
#include "upchecker_def.h"

namespace fxfile
{
namespace base
{
class UpdateInfoManager
{
public:
    UpdateInfoManager(void);
    ~UpdateInfoManager(void);

public:
    static xpr_bool_t getDefaultUpdateHomeDir(xpr::string &aDir);
    void setUpdateHomeDir(const xpr::string &aDir);

public:
    void close(void);

public:
    // for reader
    xpr_rcode_t openUpdateInfo(void);
    xpr_rcode_t readUpdateInfo(UpdateInfo &aUpdateInfo);

public:
    // for writer
    xpr_rcode_t createUpdateInfo(void);
    xpr_rcode_t writeUpdateInfo(const UpdateInfo &aUpdateInfo);

protected:
    xpr_key_t generateKey(xpr_key_t aOldKey);

    void getKeyFilePath(xpr::string &aFilePath);

    void initUpdateInfo(UpdateInfo &aUpdateInfo);

protected:
    xpr::string        mUpdateHomeDir;
    xpr_bool_t         mInitialized;

    xpr_key_t          mProcessMutexKey;
    xpr_key_t          mShmKey;
    xpr::ProcessMutex  mProcessMutex;
    xpr::Shm           mShm;
    void              *mShmAttachedAddr;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_UPDATE_INFO_MGR_H__
