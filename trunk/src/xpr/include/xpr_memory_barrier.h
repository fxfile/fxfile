//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_MEMORY_BARRIER_H__
#define __XPR_MEMORY_BARRIER_H__ 1
#pragma once

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
    #if defined(XPR_CFG_CPU_ARCH_IA64)
#define XPR_MEMORY_BARRIER() __mf()
    #elif defined(XPR_CFG_CPU_ARCH_X86) || defined(XPR_CFG_CPU_ARCH_X86_64)
        #if defined(XPR_CFG_COMPILER_64BIT)
#define XPR_MEMORY_BARRIER() __faststorefence()
        #else
#define XPR_MEMORY_BARRIER() __asm mfence
        #endif
    #endif
#else
#error Please, support for your OS and compiler. <xpr_memory_barrier.h>
#endif
} // namespace xpr

#endif // __XPR_MEMORY_BARRIER_H__
