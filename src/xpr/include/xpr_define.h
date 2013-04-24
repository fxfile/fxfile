//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_DEFINE_H__
#define __XPR_DEFINE_H__
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

#define XPR_WIDEN2(x) L##x
#define XPR_WIDEN(x) XPR_WIDEN2(x)
#define __WFILE__ XPR_WIDEN(__FILE__)

#define XPR_FILE_PATH __FILE__
#define XPR_WFILE_PATH __WFILE__
#define XPR_FILE_NAME  (strrchr(XPR_FILE_PATH, '\\')+1)
#define XPR_WFILE_NAME (wcsrchr(XPR_WFILE_PATH, L'\\')+1)
#if defined(XPR_CFG_UNICODE)
#define XPR_TFILE_PATH XPR_WFILE_PATH
#define XPR_TFILE_NAME XPR_WFILE_NAME
#else // not XPR_CFG_UNICODE
#define XPR_TFILE_PATH XPR_FILE_PATH
#define XPR_TFILE_NAME XPR_FILE_NAME
#endif
#define XPR_DATE __DATE__
#define XPR_TIME __TIME__

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

#define XPR_STL_FOR_EACH(aIterator, aContainer) \
    for ((aIterator) = (aContainer).begin(); (aIterator) != (aContainer).end(); ++(aIterator))
#define XPR_STL_FOR_EACH_REVERSE(aIterator, aContainer) \
    for ((aIterator) = (aContainer).rbegin(); (aIterator) != (aContainer).rend(); ++(aIterator))
#define XPR_STL_IS_INDEXABLE(aIndex, aContainer) \
    ((0 <= ((xpr_sint_t)(aIndex))) && (((xpr_sint_t)(aIndex)) < ((xpr_sint_t)(aContainer).size())))
} // namespace xpr

#endif // __XPR_DEFINE_H__
