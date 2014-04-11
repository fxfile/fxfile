//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_DEFINE_H__
#define __XPR_DEFINE_H__ 1
#pragma once

#include "xpr_config.h"

namespace xpr
{
#if defined(XPR_CFG_COMPILER_CPP)
#define XPR_EXTERN_C extern "C"
#define XPR_EXTERN_C_BEGIN extern "C" {
#define XPR_EXTERN_C_END }
#else // not XPR_CFG_COMPILER_CPP
#define XPR_EXTERN_C extern
#define XPR_EXTERN_C_BEGIN
#define XPR_EXTERN_C_END
#endif

#define XPR_UNUSED(x) x

#define XPR_WIDE_DEFINE2(x) L##x
#define XPR_WIDE_DEFINE(x) XPR_WIDE_DEFINE2(x)

#define XPR_NUMBER_TO_MBCS_STRING_DEFINE(x) #x
#define XPR_NUMBER_TO_MBCS_STRING(x) XPR_NUMBER_TO_MBCS_STRING_DEFINE(x)

#define XPR_NUMBER_TO_WIDE_STRING_DEFINE(x) L#x
#define XPR_NUMBER_TO_WIDE_STRING(x) XPR_NUMBER_TO_WIDE_STRING_DEFINE(x)

#define XPR_MBCS_DATE __DATE__
#define XPR_WIDE_DATE XPR_WIDE_DEFINE(__DATE__)
#define XPR_MBCS_TIME __TIME__
#define XPR_WIDE_TIME XPR_WIDE_DEFINE(__TIME__)
#if defined(XPR_CFG_UNICODE)
#define XPR_DATE XPR_WIDE_DATE
#define XPR_TIME XPR_WIDE_TIME
#else
#define XPR_DATE XPR_MBCS_DATE
#define XPR_TIME XPR_MBCS_TIME
#endif

#if defined(XPR_CFG_COMPILTER_GCC)
#if defined(XPR_CFG_BUILD_DEBUG)
#define XPR_INLINE   static __inline__
#define XPR_STDCALL  __stdcall
#define XPR_FASTCALL __fastcall
#define XPR_CDECL    __cdecl
#elif ((XPR_CFG_COMPILER_MAJOR > 3) || ((XPR_CFG_COMPILER_MAJOR == 3) && (XPR_CFG_COMPILER_MINOR > 0)))
#define XPR_INLINE   static __inline__ __attribute__((always_inline))
#define XPR_STDCALL  __attribute__(stdcall)
#define XPR_FASTCALL __attribute__(fastcall)
#define XPR_CDECL    __attribute__(cdecl)
#else
#define XPR_INLINE static __inline__
#define XPR_STDCALL  __stdcall
#define XPR_FASTCALL __fastcall
#define XPR_CDECL    __cdecl
#endif
#elif defined(XPR_CFG_COMPILER_MSVC)
#define XPR_INLINE   static __forceinline
#define XPR_STDCALL  __stdcall
#define XPR_FASTCALL __fastcall
#define XPR_CDECL    __cdecl
#else
#define XPR_INLINE   static
#define XPR_STDCALL  __stdcall
#define XPR_FASTCALL __fastcall
#define XPR_CDECL    __cdecl
#endif

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&); \
    void operator=(const ClassName&)

#define XPR_FOR_EACH(i, aMax) \
    for ((i) = (0); (i) < aMax; ++(i))
#define XPR_FOR_EACH_REVERSE(i, aMin) \
    for (; (i) >= aMin; --(i))
} // namespace xpr

#endif // __XPR_DEFINE_H__
