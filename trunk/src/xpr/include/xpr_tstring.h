//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TSTRING_H__
#define __XPR_TSTRING_H__
#pragma once

#include "xpr_string.h"
#include "xpr_wide_string.h"

namespace xpr
{
#if defined(XPR_CFG_UNICODE)
typedef string tstring;
#else
typedef wstring tstring;
#endif
} // namespace xpr

#endif // __XPR_TSTRING_H__
