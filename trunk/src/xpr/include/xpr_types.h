//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TYPES_H__
#define __XPR_TYPES_H__
#pragma once

#include "xpr_define.h"

typedef int                xpr_bool_t;
typedef char               xpr_char_t;
typedef unsigned char      xpr_uchar_t;
typedef wchar_t            xpr_wchar_t;
#if defined(XPR_CFG_UNICODE)
typedef xpr_wchar_t        xpr_tchar_t;
#else // not XPR_CFG_UNICODE
typedef xpr_char_t         xpr_tchar_t;
#endif
typedef short              xpr_sshort_t;
typedef unsigned short     xpr_ushort_t;
typedef int                xpr_sint_t;
typedef unsigned int       xpr_uint_t;
typedef long               xpr_slong_t;
typedef unsigned long      xpr_ulong_t;
typedef long long          xpr_slonglong_t;
typedef unsigned long long xpr_ulonglong_t;
typedef float              xpr_float_t;
typedef double             xpr_double_t;

typedef signed __int8      xpr_sint8_t;
typedef unsigned __int8    xpr_uint8_t;
typedef signed __int16     xpr_sint16_t;
typedef unsigned __int16   xpr_uint16_t;
typedef signed __int32     xpr_sint32_t;
typedef unsigned __int32   xpr_uint32_t;
typedef signed __int64     xpr_sint64_t;
typedef unsigned __int64   xpr_uint64_t;

typedef char               xpr_sbyte_t;
typedef unsigned char      xpr_byte_t;
typedef int                xpr_rcode_t;

typedef ptrdiff_t          xpr_ptrdiff_t;
typedef intptr_t           xpr_sintptr_t;
typedef uintptr_t          xpr_uintptr_t;

typedef xpr_uint16_t       xpr_char16_t;
typedef xpr_uint32_t       xpr_char32_t;

#if defined(XPR_CFG_OS_WINDOWS)
#if defined(XPR_CFG_CPU_ARCH_32BIT)
typedef _W64 xpr_uint_t    xpr_size_t;
typedef _W64 xpr_sint_t    xpr_ssize_t;
#else // not XPR_CFG_CPU_ARCH_32BIT
typedef xpr_uint64_t       xpr_size_t;
typedef xpr_sint64_t       xpr_ssize_t;
#endif
typedef off_t              xpr_offset_t;
typedef xpr_sint32_t       xpr_key_t;
typedef xpr_sint32_t       xpr_mode_t;
#else // not XPR_CFG_OS_WINDOWS
typedef size_t             xpr_size_t;
typedef ssize_t            xpr_ssize_t;
typedef off_t              xpr_offset_t;
typedef key_t              xpr_key_t;
typedef mode_t             xpr_mode_t;
#endif

#if defined(XPR_CFG_COMPILER_MSVC)
#define XPR_SINT8_LITERAL(x)     (x##i8)
#define XPR_UINT8_LITERAL(x)     (x##ui8)
#define XPR_SINT16_LITERAL(x)    (x##i16)
#define XPR_UINT16_LITERAL(x)    (x##ui16)
#define XPR_SINT32_LITERAL(x)    (x##i32)
#define XPR_UINT32_LITERAL(x)    (x##ui32)
#define XPR_SINT64_LITERAL(x)    (x##i64)
#define XPR_UINT64_LITERAL(x)    (x##ui64)
#define XPR_SLONG_LITERAL(x)     (x##l)
#define XPR_ULONG_LITERAL(x)     (x##ul)
#define XPR_SLONGLONG_LITERAL(x) (x##ll)
#define XPR_ULONGLONG_LITERAL(x) (x##ull)
#else // not XPR_CFG_COMPILER_MSVC
#define XPR_SINT8_LITERAL(x)     (x)
#define XPR_UINT8_LITERAL(x)     (x)
#define XPR_SINT16_LITERAL(x)    (x)
#define XPR_UINT16_LITERAL(x)    (x)
#define XPR_SINT32_LITERAL(x)    (x)
#define XPR_UINT32_LITERAL(x)    (x)
#define XPR_SINT64_LITERAL(x)    (x)
#define XPR_UINT64_LITERAL(x)    (x)
#define XPR_SLONG_LITERAL(x)     (x)
#define XPR_ULONG_LITERAL(x)     (x)
#define XPR_SLONGLONG_LITERAL(x) (x#ll)
#define XPR_ULONGLONG_LITERAL(x) (x#ull)
#endif

#define XPR_MBCS_STRING_LITERAL(x) (x)
#define XPR_WIDE_STRING_LITERAL(x) L##x

#if defined(XPR_CFG_UNICODE)
#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_STRING_LITERAL(x)      XPR_WIDE_STRING_LITERAL(x)
#else // not XPR_CFG_OS_WINDOWS
#define XPR_STRING_LITERAL(x)      XPR_MBCS_STRING_LITERAL(x)
#endif
#else // not XPR_CFG_UNICODE
#define XPR_STRING_LITERAL(x)      XPR_MBCS_STRING_LITERAL(x)
#endif

const xpr_sint8_t  ksint8min  = XPR_SINT8_LITERAL (0x80);
const xpr_sint8_t  ksint8max  = XPR_SINT8_LITERAL (0x7f);
const xpr_uint8_t  kuint8min  = XPR_UINT8_LITERAL (0x0);
const xpr_uint8_t  kuint8max  = XPR_UINT8_LITERAL (0xff);
const xpr_sint16_t ksint16min = XPR_SINT16_LITERAL(0x8000);
const xpr_sint16_t ksint16max = XPR_SINT16_LITERAL(0x7fff);
const xpr_uint16_t kuint16min = XPR_UINT16_LITERAL(0x0);
const xpr_uint16_t kuint16max = XPR_UINT16_LITERAL(0xffff);
const xpr_sint32_t ksint32min = XPR_SINT32_LITERAL(0x80000000);
const xpr_sint32_t ksint32max = XPR_SINT32_LITERAL(0x7fffffff);
const xpr_uint32_t kuint32min = XPR_UINT32_LITERAL(0x0);
const xpr_uint32_t kuint32max = XPR_UINT32_LITERAL(0xffffffff);
const xpr_sint64_t ksint64min = XPR_SINT64_LITERAL(0x8000000000000000);
const xpr_sint64_t ksint64max = XPR_SINT64_LITERAL(0x7fffffffffffffff);
const xpr_uint64_t kuint64min = XPR_UINT64_LITERAL(0x0);
const xpr_uint64_t kuint64max = XPR_UINT64_LITERAL(0xffffffffffffffff);

#if defined(XPR_CFG_OS_WINDOWS)
const xpr_sint_t   ksintmax   = ksint32max;
const xpr_uint_t   kuintmax   = kuint32max;
const xpr_slong_t  kslongmax  = ksint32max;
const xpr_ulong_t  kulongmax  = kuint32max;
#else // not XPR_CFG_OS_WINDOWS
#error Please, support for your types. <xpr_types.h>
#endif

const xpr_sbyte_t  ksbyte8min = XPR_SINT8_LITERAL (0x80);
const xpr_sbyte_t  ksbyte8max = XPR_SINT8_LITERAL (0x7f);
const xpr_byte_t   kbyte8min  = XPR_UINT8_LITERAL (0x0);
const xpr_byte_t   kbyte8max  = XPR_UINT8_LITERAL (0xff);

#define XPR_TRUE 1
#define XPR_FALSE 0
#define XPR_IS_TRUE(a) ((a) != XPR_FALSE)
#define XPR_IS_FALSE(a) ((a) == XPR_FALSE)

#define XPR_NULL NULL
#define XPR_IS_NULL(a) ((a) == XPR_NULL)
#define XPR_IS_NOT_NULL(a) ((a) != XPR_NULL)

#define XPR_SIZE_OF sizeof
#define XPR_OFFSET_OF offsetof

#ifndef countof
#define XPR_COUNT_OF(a) (sizeof(a) / sizeof(a[0]))
#else // countof
#define XPR_COUNT_OF countof
#endif

#endif // __XPR_TYPES_H__
