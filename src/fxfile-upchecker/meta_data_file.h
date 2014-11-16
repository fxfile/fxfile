//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_META_DATA_FILE_H__
#define __FXFILE_UPCHECKER_META_DATA_FILE_H__ 1
#pragma once

namespace fxfile
{
namespace upchecker
{
class MetaDataFile
{
public:
    struct MetaData
    {
        xpr::string mProgram;
        xpr::string mVersion;
        xpr::string mNewUrl;
    };

public:
    static xpr_bool_t parseMetaFile(const xpr::string &aMetaFilePath, MetaData &aMetaData);
};
} // namespace fxfile
} // namespace upchecker

#endif // __FXFILE_UPCHECKER_META_DATA_FILE_H__
