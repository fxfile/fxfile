//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_ENV_H__
#define __XPR_ENV_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_rcode.h"
#include "xpr_config.h"
#include "xpr_string.h"

// XPR_MAX_ENV is maximum length of 'name=value' format string.
#if defined(_MAX_ENV)
#define XPR_MAX_ENV _MAX_ENV
#else
#define XPR_MAX_ENV 32767
#endif

namespace xpr
{
XPR_DL_API xpr_rcode_t getEnv(const xpr_tchar_t *aName, xpr::string &aValue);
XPR_DL_API xpr_rcode_t setEnv(const xpr_tchar_t *aName, const xpr::string &aValue);

typedef struct Env
{
    xpr::string  mEnvVar;
    Env         *mNext;
} Env;

XPR_DL_API xpr_rcode_t getEnvList(Env **aEnvList, xpr_size_t *aCount);
XPR_DL_API void        freeEnvList(Env *aEnvList);
} // namespace xpr

#endif // __XPR_ENV_H__
