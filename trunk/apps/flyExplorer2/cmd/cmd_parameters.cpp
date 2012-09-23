//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cmd_parameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace cmd
{
CommandParameters::CommandParameters(void)
{
}

CommandParameters::~CommandParameters(void)
{
    mParameters.clear();
}

void *CommandParameters::get(xpr_sint_t aId)
{
    ParameterMap::iterator sIterator = mParameters.find(aId);
    if (sIterator == mParameters.end())
        return XPR_NULL;

    return sIterator->second;
}

void *CommandParameters::set(xpr_sint_t aId, void *aValue)
{
    void *sOldValue = XPR_NULL;

    ParameterMap::iterator sIterator = mParameters.find(aId);
    if (sIterator != mParameters.end())
    {
        sOldValue = sIterator->second;
        mParameters.erase(sIterator);
    }

    mParameters[aId] = aValue;

    return sOldValue;
}
} // namespace cmd
