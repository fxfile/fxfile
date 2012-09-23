//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_CONFIG_H__
#define __XPR_CONFIG_H__
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>
#include <assert.h>

namespace xpr
{
//
// CPU architecture
//
#if defined(_M_X64) || defined(__x86_64__)
#define XPR_CFG_CPU_ARCH "x86-64"
#define XPR_CFG_CPU_ARCH_X86_FAMILY    1
#define XPR_CFG_CPU_ARCH_X86_64        1
#define XPR_CFG_CPU_ARCH_64BIT         1
#define XPR_CFG_CPU_ARCH_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define XPR_CFG_CPU_ARCH "x86"
#define XPR_CFG_CPU_ARCH_X86_FAMILY    1
#define XPR_CFG_CPU_ARCH_X86           1
#define XPR_CFG_CPU_ARCH_32BIT         1
#define XPR_CFG_CPU_ARCH_LITTLE_ENDIAN 1
#elif defined(__arm__) || defined(__thumb__)
#define XPR_CFG_CPU_ARCH "ARM"
#define XPR_CFG_CPU_ARCH_ARM_FAMILY    1
#define XPR_CFG_CPU_ARCH_ARMEL         1
#define XPR_CFG_CPU_ARCH_32BIT         1
#define XPR_CFG_CPU_ARCH_LITTLE_ENDIAN 1
#else
#error Please, support for your CPU architecture. <xpr_config.h>
#endif

//
// OS
//
#if defined(__APPLE__)
#define XPR_CFG_OS "MacOSX"
#define XPR_CFG_OS_MACOSX 1
#elif defined(ANDROID)
#define XPR_CFG_OS "Android"
#define XPR_CFG_OS_ANDROID 1
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define XPR_CFG_OS "Linux"
#define XPR_CFG_OS_LINUX 1
#elif defined(_WIN32)
#define XPR_CFG_OS "Windows"
#define XPR_CFG_OS_WINDOWS 1
#elif defined(__FreeBSD__)
#define XPR_CFG_OS "FreeBSD"
#define XPR_CFG_OS_FREEBSD 1
#elif defined(__OpenBSD__)
#define XPR_CFG_OS "OpenBSD"
#define XPR_CFG_OS_OPENBSD 1
#elif defined(__sun) || defined(__SVR4)
#define XPR_CFG_OS "Solaris"
#define XPR_CFG_OS_SOLARIS 1
#elif defined(hpux) || defined(__hpux) || defined(__hpux__)
#define XPR_CFG_OS "HP-UX"
#define XPR_CFG_OS_HPUX 1
#elif defined(_AIX)
#define XPR_CFG_OS "AIX"
#define XPR_CFG_OS_AIX 1
#elif defined(_osf) || defined(__osf)
#define XPR_CFG_OS "TRU64"
#define XPR_CFG_OS_TRU64 1
#elif defined(sgi) || defined(__sgi)
#define XPR_CFG_OS "SGI"
#define XPR_CFG_OS_SGI 1
#else
#error Please, support for your OS. <xpr_config.h>
#endif

#if defined(XPR_CFG_OS_MACOSX)  || defined(XPR_CFG_OS_LINUX)   || \
    defined(XPR_CFG_OS_FREEBSD) || defined(XPR_CFG_OS_OPENBSD) || \
    defined(XPR_CFG_OS_SOLARIS) || defined(XPR_CFG_OS_ANDROID)
#define XPR_CFG_OS_POSIX 1
#endif

//
// Compiler
//
#if defined(__GNUC__)
#define XPR_CFG_COMPILER_GCC 1
#define XPR_CFG_COMPILER_MAJOR __GNUC__
#define XPR_CFG_COMPILER_MINOR __GNUC__MINOR__
#elif defined(_MSC_VER)
#define XPR_CFG_COMPILER_MSVC 1
#define XPR_CFG_COMPILER_MAJOR _MSC_VER
#elif defined(__WATCOMC__)
#define XPR_CFG_COMPILER_WATCOMC 1
#elif defined(__BORLANDC__)
#define XPR_CFG_COMPILER_BORLANDC 1
#else
#error Please, support for your compiler. <xpr_config.h>
#endif

#if defined(XPR_CFG_CPU_ARCH_64BIT)
#define XPR_CFG_COMPILER_64BIT 1
#else
#define XPR_CFG_COMPILER_32BIT 1
#endif

#if defined(__STDC__)
#define XPR_CFG_COMPILER_C 1
#define XPR_CFG_COMPILER_C_VER __STDC_VERSION__
    #if (__STDC_VERSION__ >= 201112L)
#define XPR_CFG_COMPILER_C11 1
    #elif (__STDC_VERSION__ >= 199901L)
#define XPR_CFG_COMPILER_C99 1
    #elif (__STDC_VERSION__ >= 199409L)
#define XPR_CFG_COMPILER_C94 1
    #else
#define XPR_CFG_COMPILER_C90 1
    #endif
#endif

#if defined(__cplusplus)
#define XPR_CFG_COMPILER_CPP 1
#define XPR_CFG_COMPILER_CPP_VER __cplusplus
    #if (__cplusplus >= 201103L)
#define XPR_CFG_COMPILER_CPP11 1
    #elif (__cplusplus >= 199707L)
#define XPR_CFG_COMPILER_CPP98 1
    #else
#error Please, support for your compiler. <xpr_config.h>
    #endif
#endif

//
// Endian
//
//#define XPR_CFG_BIG_ENDIAN 1
#define XPR_CFG_LITTLE_ENDIAN 1

//
// Build mode
//
#if defined(_DEBUG)
#define XPR_CFG_BUILD_DEBUG 1
#else
#define XPR_CFG_BUILD_RELEASE 1
#endif

//
// Unicode
//
#if defined(_UNICODE)
#define XPR_CFG_UNICODE 1
#else
#define XPR_CFG_MBCS 1
#endif
} // namespace xpr

//
// Dedicated OS header file
//
#if defined(XPR_CFG_OS_WINDOWS)
#include "xpr_config_win.h"
#endif // XPR_CFG_OS_WINDOWS

#endif // __XPR_CONFIG_H__
