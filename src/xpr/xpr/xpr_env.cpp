//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_env.h"
#include "xpr_config.h"
#include "xpr_rcode.h"
#include "xpr_mutex.h"
#include "xpr_memory.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
Mutex gEnvGetMutex; // TODO: Mutex should change to spin lock or rw lock later.
Mutex gEnvSetMutex; // TODO: Mutex should change to spin lock or rw lock later.
} // namespace anonymous

XPR_DL_API xpr_rcode_t getEnv(const xpr_tchar_t *aName, xpr::string &aValue)
{
    XPR_ASSERT(aName != XPR_NULL);

    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_tchar_t *sValue = _tgetenv(aName);
    if (XPR_IS_NULL(sValue))
    {
        return XPR_RCODE_ENOENT;
    }

    aValue = sValue;

#else // not XPR_CFG_OS_WINDOWS

    xpr_char_t *sValue = getenv(aName);
    if (XPR_IS_NULL(sValue))
    {
        return XPR_RCODE_ENOENT;
    }

    aValue = sValue;

#endif // XPR_CFG_OS_WINDOWS

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t setEnv(const xpr_tchar_t *aName, const xpr::string &aValue)
{
    XPR_ASSERT(aName != XPR_NULL);

    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);
    MutexGuard sEnvSetMutexGuard(gEnvSetMutex);

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_size_t sNameLen  = _tcslen(aName);
    xpr_size_t sValueLen = aValue.length();

    if ((sNameLen + 1 + sValueLen) > XPR_MAX_ENV)
    {
        return XPR_RCODE_E2BIG;
    }

    xpr::string sEnvVar;
    sEnvVar.format(XPR_STRING_LITERAL("%s=%s"), aName, aValue.c_str());

    xpr_sint_t sResult = _tputenv(sEnvVar.c_str());
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#else // not XPR_CFG_OS_WINDOWS

    xpr_sint_t sResult = setenv(aName, aValue.c_str(), XPR_TRUE);
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#endif // XPR_CFG_OS_WINDOWS

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t getEnvList(Env **aEnvList, xpr_size_t *aCount)
{
    Env       *sEnvListHead = XPR_NULL;
    Env       *sEnvList     = XPR_NULL;
    xpr_size_t sCount       = 0;

    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_tchar_t *sEnvStrings = GetEnvironmentStrings();

    if (*sEnvStrings != 0)
    {
        xpr_size_t sLen = _tcslen(sEnvStrings);

        if (XPR_IS_NOT_NULL(aEnvList))
        {
            sEnvList = new Env;
            sEnvList->mEnvVar = sEnvStrings;
            sEnvList->mNext   = XPR_NULL;

            sEnvListHead = sEnvList;
        }

        sEnvStrings += sLen + 1;
        ++sCount;

        while (*sEnvStrings != 0)
        {
            sLen = _tcslen(sEnvStrings);

            if (XPR_IS_NOT_NULL(aEnvList))
            {
                sEnvList->mNext = new Env;
                sEnvList->mNext->mEnvVar = sEnvStrings;
                sEnvList->mNext->mNext   = XPR_NULL;

                sEnvList = sEnvList->mNext;
            }

            sEnvStrings += sLen + 1;
            ++sCount;
        }
    }

#else // not XPR_CFG_OS_WINDOWS

    extern xpr_char_t **environ;

    xpr_char_t **sEnvStrings = environ;

    if (*sEnvStrings != XPR_NULL)
    {
        xpr_size_t sLen = strlen(*sEnvStrings);

        if (XPR_IS_NOT_NULL(aEnvList))
        {
            sEnvList = new EnvA;
            sEnvList->mEnvVar = *sEnvStrings;
            sEnvList->mNext   = XPR_NULL;

            sEnvListHead = sEnvList;
        }

        ++sEnvStrings;
        ++sCount;

        while (*sEnvStrings != 0)
        {
            sLen = strlen(*sEnvStrings);

            if (XPR_IS_NOT_NULL(aEnvList))
            {
                sEnvList->mNext = new EnvA;
                sEnvList->mNext->mEnvVar = *sEnvStrings;
                sEnvList->mNext->mNext   = XPR_NULL;

                sEnvList = sEnvList->mNext;
            }

            ++sEnvStrings;
            ++sCount;
        }
    }

#endif

    if (XPR_IS_NOT_NULL(aEnvList))
    {
        *aEnvList = sEnvListHead;
    }

    if (XPR_IS_NOT_NULL(aCount))
    {
        *aCount = sCount;
    }

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API void freeEnvList(Env *aEnvList)
{
    if (XPR_IS_NULL(aEnvList))
    {
        return;
    }

    Env *sNextEnvList;

    do
    {
        sNextEnvList = aEnvList->mNext;

        XPR_SAFE_DELETE(aEnvList);

        aEnvList = sNextEnvList;

    } while (XPR_IS_NOT_NULL(aEnvList));
}
} // namespace xpr
