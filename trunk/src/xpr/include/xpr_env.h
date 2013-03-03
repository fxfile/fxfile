//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_ENV_H__
#define __XPR_ENV_H__
#pragma once

#include "xpr_types.h"
#include "xpr_rcode.h"
#include "xpr_config.h"

// XPR_MAX_ENV is maximum length of 'name=value' format string.
#if defined(_MAX_ENV)
#define XPR_MAX_ENV _MAX_ENV
#else
#define XPR_MAX_ENV 32767
#endif

namespace xpr
{
XPR_DL_API xpr_rcode_t getEnv(const xpr_char_t  *aName, xpr_char_t  *aValue, xpr_size_t aMaxLen);
XPR_DL_API xpr_rcode_t getEnv(const xpr_wchar_t *aName, xpr_wchar_t *aValue, xpr_size_t aMaxLen);

XPR_DL_API xpr_rcode_t setEnv(const xpr_char_t  *aName, const xpr_char_t  *aValue);
XPR_DL_API xpr_rcode_t setEnv(const xpr_wchar_t *aName, const xpr_wchar_t *aValue);

typedef struct EnvA
{
    xpr_char_t *mEnvVar;
    EnvA       *mNext;
} EnvA;

typedef struct EnvW
{
    xpr_wchar_t *mEnvVar;
    EnvW        *mNext;
} EnvW;

#if defined(XPR_CFG_UNICODE)
typedef EnvA Env;
#else // not XPR_CFG_UNICODE
typedef EnvW Env;
#endif // XPR_CFG_UNICODE

XPR_DL_API xpr_rcode_t getEnvList(EnvA **aEnvList, xpr_size_t *aCount);
XPR_DL_API xpr_rcode_t getEnvList(EnvW **aEnvList, xpr_size_t *aCount);
XPR_DL_API void        freeEnvList(EnvA *aEnvList);
XPR_DL_API void        freeEnvList(EnvW *aEnvList);
} // namespace xpr

#endif // __XPR_ENV_H__
