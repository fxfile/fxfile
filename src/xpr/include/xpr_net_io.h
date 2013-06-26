//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_NET_IO_H__
#define __XPR_NET_IO_H__ 1
#pragma once

#include "xpr_types.h"

namespace xpr
{
#define XPR_INVALID_PORT            0
#define XPR_MAX_PORT                65535
#define XPR_MAX_PORT_LENGTH         5

#define XPR_DEFAULT_FTP_PORT        21
#define XPR_DEFAULT_GOPHER_PORT     70
#define XPR_DEFAULT_HTTP_PORT       80
#define XPR_DEFAULT_HTTPS_PORT      443

#define XPR_MAX_HOST_NAME_LENGTH    256
#define XPR_MAX_USER_NAME_LENGTH    128
#define XPR_MAX_PASSWORD_LENGTH     128

#define XPR_MAX_URL_PROTOCOL_LENGTH 32
#define XPR_MAX_URL_PATH_LENGTH     2048
// URRs over 2,000 characters will not work in the most popular web browser.
// (XPR_MAX_URL_PROTOCOL_LENGTH + 3 XPR_MAX_URL_PATH_LENGTH)
#define XPR_MAX_URL_LENGTH          2083
} // namespace xpr

#endif // __XPR_NET_IO_H__
