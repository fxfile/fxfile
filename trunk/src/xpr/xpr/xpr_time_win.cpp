//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
const xpr_time_t k1601to1970Offset = XPR_SINT64_LITERAL(11644473600000000); // offset from Jan 1, 1601 (UTC) to Jan 1, 1970

class HighResTimer
{
public:
    HighResTimer(void)
        : mFreq(0)
        , mEnable(XPR_FALSE)
    {
        mEnable = ::QueryPerformanceFrequency((LARGE_INTEGER *)&mFreq);
    }

public:
    xpr_sint64_t mFreq;
    xpr_bool_t   mEnable;
};

static HighResTimer gHighResTimer;

#define XPR_HIGH_REST_TIMER_CALC(cur,freq,unit) (((cur / freq) * unit) + ((cur % freq) * unit / freq))

XPR_DL_API xpr_time_t timer_ms(void)
{
    static const xpr_sint64_t ms = 1000;

    xpr_sint64_t t = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&t);
    return XPR_HIGH_REST_TIMER_CALC(t, gHighResTimer.mFreq, ms);
}

XPR_DL_API xpr_time_t timer_us(void)
{
    static const xpr_sint64_t us = 1000 * 1000;

    xpr_sint64_t t = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&t);
    return XPR_HIGH_REST_TIMER_CALC(t, gHighResTimer.mFreq, us);
}

XPR_DL_API xpr_time_t timer_ns(void)
{
    static const xpr_sint64_t ns = 1000 * 1000 * 1000;

    xpr_sint64_t t = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&t);
    return XPR_HIGH_REST_TIMER_CALC(t, gHighResTimer.mFreq, ns);
}
#endif
} // namespace xpr
