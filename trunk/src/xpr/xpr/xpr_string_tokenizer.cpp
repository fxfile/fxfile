//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_string_tokenizer.h"

namespace xpr
{
StringTokenizer::StringTokenizer(const xpr::string &aString, const xpr::string &aDelimiters)
    : mString(aString), mDelimiters(aDelimiters)
    , mOffset(0)
{
}

StringTokenizer::~StringTokenizer(void)
{
}

xpr_bool_t StringTokenizer::next(void)
{
    return next(mDelimiters);
}

xpr_bool_t StringTokenizer::next(const xpr::string &aDelimiters)
{
    xpr_size_t sTokenBegin = mString.find_first_not_of(aDelimiters, mOffset);
    if (sTokenBegin == xpr::string::npos)
    {
        mOffset = mString.length();
        return XPR_FALSE;
    }

    xpr_size_t sTokenEnd = mString.find_first_of(aDelimiters, sTokenBegin);
    if (sTokenEnd == xpr::string::npos)
    {
        mToken  = mString.substr(sTokenBegin);
        mOffset = mString.length();
        return XPR_TRUE;
    }

    mToken  = mString.substr(sTokenBegin, sTokenEnd - sTokenBegin);
    mOffset = sTokenEnd;

    return XPR_TRUE;
}

const xpr::string &StringTokenizer::getToken(void) const
{
    return mToken;
}

void StringTokenizer::reset(void)
{
    mToken.clear();
    mOffset = 0;
}
} // namespace xpr
