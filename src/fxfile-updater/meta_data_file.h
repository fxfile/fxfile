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
        xpr::tstring mProgram;
        xpr::tstring mVersion;
        xpr::tstring mFile;
        xpr::tstring mHashCrc32;
        xpr::tstring mHashMd5;
        xpr::tstring mHashSha1;
    };

public:
    static xpr_bool_t parseMetaFile(const xpr::tstring &aMetaFilePath, MetaData &aMetaData);
    static xpr_bool_t verifyChecksum(const xpr::tstring &aProgramFilePath, MetaData &aMetaData);
};
} // namespace fxfile
} // namespace updater

#endif // __FXFILE_BASE_META_DATA_FILE_H__
