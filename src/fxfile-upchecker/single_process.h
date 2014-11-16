//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_SINGLE_PROCESS_H__
#define __FXFILE_UPCHECKER_SINGLE_PROCESS_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
namespace upchecker
{
class SingleProcess
{
public:
    static xpr_bool_t check(void);
    static void       lock(void);
    static void       unlock(void);
    static xpr_uint_t getMsg(void);
    static void       postMsg(void);

protected:
    static HANDLE mSingleProcessMutex;
};
} // namespace upchecker
} // namespace fxfile

#endif // __FXFILE_UPCHECKER_SINGLE_PROCESS_H__
