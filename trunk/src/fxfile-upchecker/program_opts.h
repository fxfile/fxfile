//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_PROGRAM_OPTS_H__
#define __FXFILE_UPCHECKER_PROGRAM_OPTS_H__ 1
#pragma once

namespace fxfile
{
namespace upchecker
{
class ProgramOpts
{
public:
    ProgramOpts(void);
    ~ProgramOpts(void);

public:
    void parse(void);

public:
    xpr::string getConfDir(void) { return mConfDir; }

private:
    xpr::string mConfDir;
};
} // namespace upchecker
} // namespace fxfile

#endif // __FXFILE_UPCHECKER_PROGRAM_OPTS_H__
