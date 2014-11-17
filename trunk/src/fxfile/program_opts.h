//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PROGRAM_OPTS_H__
#define __FXFILE_PROGRAM_OPTS_H__ 1
#pragma once

#include "singleton_manager.h"
#include "fxfile_def.h"

namespace fxfile
{
class ProgramOpts : public SingletonInstance
{
    friend class SingletonManager;

protected: ProgramOpts(void);
public:   ~ProgramOpts(void);

public:
    void parse(void);
    static void showUsage(void);
    static void showVersion(void);
    static void getUsage(xpr::string &aUsage);

public:
    xpr_bool_t  isShowUsage(void) const { return mShowUsage; }
    xpr_bool_t  isShowVersion(void) const { return mShowVersion; }
    xpr_bool_t  isSelect(void) const { return mSelect; }
    xpr_bool_t  isResetConf(void) const { return mResetConf; }
    void        getWindowSplit(xpr_sint_t &aSplitRowCount, xpr_sint_t &aSplitColumnCount) const { aSplitRowCount = mSplitRowCount; aSplitColumnCount = mSplitColumnCount; }
    xpr::string getDir(xpr_size_t aIndex) const { XPR_ASSERT(aIndex < MAX_VIEW_SPLIT); return mDir[aIndex]; }
    xpr::string getConfDir(void) const { return mConfDir; }

private:
    xpr_bool_t  mShowUsage;
    xpr_bool_t  mShowVersion;
    xpr_bool_t  mSelect;
    xpr_sint_t  mSplitRowCount;
    xpr_sint_t  mSplitColumnCount;
    xpr::string mDir[MAX_VIEW_SPLIT];
    xpr::string mConfDir;
    xpr_bool_t  mResetConf;
};
} // namespace fxfile

#endif // __FXFILE_PROGRAM_OPTS_H__
