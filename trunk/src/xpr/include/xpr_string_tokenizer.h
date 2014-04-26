//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_STRING_TOKENIZER_H__
#define __XPR_STRING_TOKENIZER_H__ 1
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_string.h"

namespace xpr
{
class XPR_DL_API StringTokenizer
{
public:
    StringTokenizer(const xpr::string &aString, const xpr::string &aDelimiters = XPR_STRING_LITERAL(" "));
    virtual ~StringTokenizer(void);

public:
    xpr_bool_t next(void);
    xpr_bool_t next(const xpr::string &aDelimiters);
    void reset(void);

    const xpr::string &getToken(void) const;

private:
    const xpr::string mString;
    const xpr::string mDelimiters;
    xpr::string       mToken;
    xpr_size_t        mOffset;
};
} // namespace xpr

#endif // __XPR_STRING_TOKENIZER_H__
