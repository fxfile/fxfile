//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_env.h"
#include "xpr_config.h"
#include "xpr_rcode.h"
#include "xpr_thread_sync.h"
#include "xpr_memory.h"

namespace xpr
{
static Mutex gEnvGetMutex; // TODO: Mutex should change to spin lock or rw lock later.
static Mutex gEnvSetMutex; // TODO: Mutex should change to spin lock or rw lock later.

XPR_DL_API xpr_rcode_t getEnv(const xpr_char_t  *aName, xpr_char_t *aValue, xpr_size_t aMaxLen)
{
    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

    xpr_char_t *sValue = getenv(aName);
    if (XPR_IS_NULL(sValue))
    {
        return XPR_RCODE_ENOENT;
    }

    xpr_size_t sLen = strlen(sValue);
    if (sLen > aMaxLen)
    {
        return XPR_RCODE_E2BIG;
    }

    strcpy(aValue, sValue);

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t getEnv(const xpr_wchar_t *aName, xpr_wchar_t *aValue, xpr_size_t aMaxLen)
{
    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

    xpr_wchar_t *sValue = _wgetenv(aName);
    if (XPR_IS_NULL(sValue))
    {
        return XPR_RCODE_ENOENT;
    }

    xpr_size_t sLen = wcslen(sValue);
    if (sLen > aMaxLen)
    {
        return XPR_RCODE_E2BIG;
    }

    wcscpy(aValue, sValue);

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t setEnv(const xpr_char_t *aName, const xpr_char_t *aValue)
{
    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);
    MutexGuard sEnvSetMutexGuard(gEnvSetMutex);

#if defined(XPR_CFG_OS_AIX)   || \
    defined(XPR_CFG_OS_LINUX) || \
    defined(XPR_CFG_OS_TRU64)

    xpr_sint_t sResult = setenv(aName, aValue, XPR_TRUE);
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#else

    xpr_size_t sNameLen  = strlen(aName);
    xpr_size_t sValueLen = strlen(aValue);

    if ((sNameLen + 1 + sValueLen) > XPR_MAX_ENV)
    {
        return XPR_RCODE_E2BIG;
    }

    xpr_char_t sEnvVar[XPR_MAX_ENV] = {0};

    sprintf(sEnvVar, XPR_MBCS_STRING_LITERAL("%s=%s"), aName, aValue);

    xpr_sint_t sResult = _putenv(sEnvVar);
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#endif

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t setEnv(const xpr_wchar_t *aName, const xpr_wchar_t *aValue)
{
    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);
    MutexGuard sEnvSetMutexGuard(gEnvSetMutex);

#if defined(XPR_CFG_OS_AIX)   || \
    defined(XPR_CFG_OS_LINUX) || \
    defined(XPR_CFG_OS_TRU64)

    xpr_sint_t sResult = _wsetenv(aName, aValue, XPR_TRUE);
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#else

    xpr_size_t sNameLen  = wcslen(aName);
    xpr_size_t sValueLen = wcslen(aValue);

    if ((sNameLen + 1 + sValueLen) > XPR_MAX_ENV)
    {
        return XPR_RCODE_E2BIG;
    }

    xpr_wchar_t sEnvVar[XPR_MAX_ENV] = {0};

    _swprintf(sEnvVar, XPR_WIDE_STRING_LITERAL("%s=%s"), aName, aValue);

    xpr_sint_t sResult = _wputenv(sEnvVar);
    if (sResult != 0)
    {
        return XPR_RCODE_GET_OS_ERROR();
    }

#endif

    return XPR_RCODE_SUCCESS;
}

XPR_DL_API xpr_rcode_t getEnvList(EnvA **aEnvList, xpr_size_t *aCount)
{
    EnvA       *sEnvListHead = XPR_NULL;
    EnvA       *sEnvList     = XPR_NULL;
    xpr_size_t  sCount       = 0;

    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_char_t *sEnvStrings = GetEnvironmentStrings();

    if (*sEnvStrings != 0)
    {
        xpr_size_t sLen = strlen(sEnvStrings);

        if (XPR_IS_NOT_NULL(aEnvList))
        {
            sEnvList = new EnvA;
            sEnvList->mEnvVar = new xpr_char_t[sLen + 1];
            strcpy(sEnvList->mEnvVar, sEnvStrings);
            sEnvList->mNext   = XPR_NULL;

            sEnvListHead = sEnvList;
        }

        sEnvStrings += sLen + 1;
        ++sCount;

        while (*sEnvStrings != 0)
        {
            sLen = strlen(sEnvStrings);

            if (XPR_IS_NOT_NULL(aEnvList))
            {
                sEnvList->mNext = new EnvA;
                sEnvList->mNext->mEnvVar = new xpr_char_t[sLen + 1];
                strcpy(sEnvList->mNext->mEnvVar, sEnvStrings);
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
            sEnvList->mEnvVar = new xpr_char_t[sLen + 1];
            strcpy(sEnvList->mEnvVar, *sEnvStrings);
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
                sEnvList->mNext->mEnvVar = new xpr_char_t[sLen + 1];
                strcpy(sEnvList->mNext->mEnvVar, *sEnvStrings);
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

XPR_DL_API xpr_rcode_t getEnvList(EnvW **aEnvList, xpr_size_t *aCount)
{
    EnvW       *sEnvListHead = XPR_NULL;
    EnvW       *sEnvList     = XPR_NULL;
    xpr_size_t  sCount       = 0;

    MutexGuard sEnvGetMutexGuard(gEnvGetMutex);

#if defined(XPR_CFG_OS_WINDOWS)

    xpr_wchar_t *sEnvStrings = GetEnvironmentStringsW();

    if (*sEnvStrings != 0)
    {
        xpr_size_t sLen = wcslen(sEnvStrings);

        if (XPR_IS_NOT_NULL(aEnvList))
        {
            sEnvList = new EnvW;
            sEnvList->mEnvVar = new xpr_wchar_t[sLen + 1];
            wcscpy(sEnvList->mEnvVar, sEnvStrings);
            sEnvList->mNext   = XPR_NULL;

            sEnvListHead = sEnvList;
        }

        sEnvStrings += sLen + 1;
        ++sCount;

        while (*sEnvStrings != 0)
        {
            sLen = wcslen(sEnvStrings);

            if (XPR_IS_NOT_NULL(aEnvList))
            {
                sEnvList->mNext = new EnvW;
                sEnvList->mNext->mEnvVar = new xpr_wchar_t[sLen + 1];
                wcscpy(sEnvList->mNext->mEnvVar, sEnvStrings);
                sEnvList->mNext->mNext   = XPR_NULL;

                sEnvList = sEnvList->mNext;
            }

            sEnvStrings += sLen + 1;
            ++sCount;
        }
    }

#else // not XPR_CFG_OS_WINDOWS

    extern xpr_wchar_t **_wenviron;

    xpr_wchar_t **sEnvStrings = _wenviron;

    if (*sEnvStrings != XPR_NULL)
    {
        xpr_size_t sLen = wcslen(*sEnvStrings);

        if (XPR_IS_NOT_NULL(aEnvList))
        {
            sEnvList = new EnvW;
            sEnvList->mEnvVar = new xpr_wchar_t[sLen + 1];
            wcscpy(sEnvList->mEnvVar, *sEnvStrings);
            sEnvList->mNext   = XPR_NULL;

            sEnvListHead = sEnvList;
        }

        ++sEnvStrings;
        ++sCount;

        while (*sEnvStrings != 0)
        {
            sLen = wcslen(*sEnvStrings);

            if (XPR_IS_NOT_NULL(aEnvList))
            {
                sEnvList->mNext = new EnvW;
                sEnvList->mNext->mEnvVar = new xpr_wchar_t[sLen + 1];
                wcscpy(sEnvList->mNext->mEnvVar, *sEnvStrings);
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

XPR_DL_API void freeEnvList(EnvA *aEnvList)
{
    if (XPR_IS_NULL(aEnvList))
    {
        return;
    }

    EnvA *sNextEnvList;

    do
    {
        sNextEnvList = aEnvList->mNext;

        XPR_SAFE_DELETE_ARRAY(aEnvList->mEnvVar);
        XPR_SAFE_DELETE(aEnvList);

        aEnvList = sNextEnvList;

    } while (XPR_IS_NOT_NULL(aEnvList));
}

XPR_DL_API void freeEnvList(EnvW *aEnvList)
{
    if (XPR_IS_NULL(aEnvList))
    {
        return;
    }

    EnvW *sNextEnvList;

    do
    {
        sNextEnvList = aEnvList->mNext;

        XPR_SAFE_DELETE_ARRAY(aEnvList->mEnvVar);
        XPR_SAFE_DELETE(aEnvList);

        aEnvList = sNextEnvList;

    } while (XPR_IS_NOT_NULL(aEnvList));
}
} // namespace xpr
