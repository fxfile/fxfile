//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_REG_EXPR_H__
#define __XPR_REG_EXPR_H__ 1
#pragma once

#include <regex>

namespace std
{
namespace tr1
{
#if defined(XPR_CFG_UNICODE)
typedef wregex tregex;
typedef wcmatch tcmatch;
typedef wsmatch tsmatch;
#else
typedef regex tregex;
typedef cmatch tcmatch;
typedef smatch tsmatch;
#endif
} // namespace tr1
} // namespace std

#endif // __XPR_REG_EXPR_H__
