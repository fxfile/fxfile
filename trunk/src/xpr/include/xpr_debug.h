//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_DEBUG_H__
#define __XPR_DEBUG_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
    #if defined(XPR_CFG_BUILD_DEBUG)

XPR_DL_API void traceOut(const xpr_tchar_t *aFormat, ...);
#define XPR_TRACE              xpr::traceOut
#define XPR_ASSERT(exp)        assert(exp)
#define XPR_DEBUG_ASSERT(exp)  assert(exp)
#define XPR_VERIFY(exp)        assert(exp)
#define XPR_WARNING(cond, msg) ((cond) ? __noop : XPR_TRACE(msg))

    #else // not, XPR_CFG_BUILD_DEBUG

inline void traceOut(...) {}
#define XPR_TRACE              __noop
#define XPR_ASSERT(exp)        assert(exp)
#define XPR_DEBUG_ASSERT(exp)  __noop
#define XPR_VERIFY(exp)        exp
#define XPR_WARNING(cond, msg) __noop

    #endif // XPR_CFG_BUILD_DEBUG
#else // not XPR_CFG_OS_WINDOWS
    #if defined(XPR_CFG_BUILD_DEBUG)

#define XPR_TRACE              printf
#define XPR_ASSERT(exp)        assert(exp)
#define XPR_VERIFY(exp)        assert(exp)
#define XPR_WARNING(cond, msg) ((cond) ? __noop : XPR_TRACE(msg))

    #else // not XPR_CFG_BUILD_DEBUG

inline void traceOut(...) {}
#define XPR_TRACE
#define XPR_ASSERT(exp)
#define XPR_VERIFY(exp)        exp
#define XPR_WARNING(cond, msg)

    #endif // XPR_CFG_BUILD_DEBUG
#endif // XPR_CFG_OS_WINDOWS
} // namespace xpr

#endif // __XPR_DEBUG_H__
