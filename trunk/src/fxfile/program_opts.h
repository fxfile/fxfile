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

public:
    inline xpr_bool_t   isShowUsage(void) const { return mShowUsage; }
    inline xpr_bool_t   isShowVersion(void) const { return mShowVersion; }
    inline xpr_bool_t   isSelect(void) const { return mSelect; }
    inline xpr_bool_t   isResetConf(void) const { return mResetConf; }
    inline void         getWindowSplit(xpr_sint_t &aSplitRowCount, xpr_sint_t &aSplitColumnCount) const { aSplitRowCount = mSplitRowCount; aSplitColumnCount = mSplitColumnCount; }
    inline xpr::tstring getDir(xpr_size_t aIndex) { XPR_ASSERT(aIndex < MAX_VIEW_SPLIT); return mDir[aIndex]; }
    inline xpr::tstring getConfDir(void) { return mConfDir; }

private:
    xpr_bool_t   mShowUsage;
    xpr_bool_t   mShowVersion;
    xpr_bool_t   mSelect;
    xpr_sint_t   mSplitRowCount;
    xpr_sint_t   mSplitColumnCount;
    xpr::tstring mDir[MAX_VIEW_SPLIT];
    xpr::tstring mConfDir;
    xpr_bool_t   mResetConf;
};
} // namespace fxfile

#endif // __FXFILE_PROGRAM_OPTS_H__
