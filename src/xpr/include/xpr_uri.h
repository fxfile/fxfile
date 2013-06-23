//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_URI_H__
#define __XPR_URI_H__ 1
#pragma once

namespace xpr
{
#define XPR_URI_SEPARATOR               XPR_STRING_LITERAL('/')
#define XPR_URI_SCHEME_SEPARATOR        XPR_STRING_LITERAL("://")
#define XPR_URI_USERINFO_SEPARATOR      XPR_STRING_LITERAL('@')
#define XPR_URI_PORT_SEPARATOR          XPR_STRING_LITERAL(':')
#define XPR_URI_USER_PASSWORD_SEPARATOR XPR_STRING_LITERAL(':')
#define XPR_URI_QUERY_SEPARATOR         XPR_STRING_LITERAL('?')
#define XPR_URI_QUERY_KEY_SEPARATOR     XPR_STRING_LITERAL('&')
#define XPR_URI_FRAGMENT_SEPARATOR      XPR_STRING_LITERAL('#')
} // namespace xpr

#endif // __XPR_URI_H__
