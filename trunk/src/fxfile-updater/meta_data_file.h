//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_META_DATA_FILE_H__
#define __FXFILE_BASE_META_DATA_FILE_H__ 1
#pragma once

namespace fxfile
{
namespace updater
{
class MetaDataFile
{
public:
    struct MetaData
    {
        xpr::string mProgram;
        xpr::string mVersion;
        xpr::string mFile;
        xpr::string mHashCrc32;
        xpr::string mHashMd5;
        xpr::string mHashSha1;
    };

public:
    static xpr_bool_t parseMetaFile(const xpr::string &aMetaFilePath, MetaData &aMetaData);
    static xpr_bool_t verifyChecksum(const xpr::string &aProgramFilePath, MetaData &aMetaData);
};
} // namespace fxfile
} // namespace updater

#endif // __FXFILE_BASE_META_DATA_FILE_H__
